/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2012 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: http://www.qt-project.org/
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**************************************************************************/

#include "sshconnectionmanager.h"

#include "sshconnection.h"

#include <QCoreApplication>
#include <QList>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QThread>
#include <QTimer>

namespace QSsh {
namespace Internal {
class UnaquiredConnection {
public:
    UnaquiredConnection(SshConnection *conn) : connection(conn), scheduledForRemoval(false) {}

    SshConnection *connection;
    bool scheduledForRemoval;
};
bool operator==(UnaquiredConnection c1, UnaquiredConnection c2) {
    return c1.connection == c2.connection;
}
bool operator!=(UnaquiredConnection c1, UnaquiredConnection c2) {
    return !(c1 == c2);
}

class SshConnectionManager : public QObject
{
    Q_OBJECT

public:
    SshConnectionManager()
    {
        moveToThread(QCoreApplication::instance()->thread());
        connect(&m_removalTimer, &QTimer::timeout,
                this, &SshConnectionManager::removeInactiveConnections);
        m_removalTimer.setTimerType(Qt::VeryCoarseTimer);
        m_removalTimer.start(150000); // For a total timeout of five minutes.
    }

    ~SshConnectionManager()
    {
        for (const UnaquiredConnection &connection : m_unacquiredConnections) {
            disconnect(connection.connection, nullptr, this, nullptr);
            delete connection.connection;
        }

        QSSH_ASSERT(m_acquiredConnections.isEmpty());
        QSSH_ASSERT(m_deprecatedConnections.isEmpty());
    }

    SshConnection *acquireConnection(const SshConnectionParameters &sshParams)
    {
        QMutexLocker locker(&m_listMutex);

        // Check in-use connections:
        for (SshConnection * const connection : m_acquiredConnections) {
            if (connection->connectionParameters() != sshParams)
                continue;

            if (connection->thread() != QThread::currentThread())
                continue;

            if (m_deprecatedConnections.contains(connection)) // we were asked to no longer use this one...
                continue;

            m_acquiredConnections.append(connection);
            return connection;
        }

        // Check cached open connections:
        for (const UnaquiredConnection &c : m_unacquiredConnections) {
            SshConnection * const connection = c.connection;
            if (connection->state() != SshConnection::Connected
                    || connection->connectionParameters() != sshParams)
                continue;

            if (connection->thread() != QThread::currentThread()) {
                if (connection->channelCount() != 0)
                    continue;
                QMetaObject::invokeMethod(this, "switchToCallerThread",
                    Qt::BlockingQueuedConnection,
                    Q_ARG(SshConnection *, connection),
                    Q_ARG(QObject *, QThread::currentThread()));
            }

            m_unacquiredConnections.removeOne(c);
            m_acquiredConnections.append(connection);
            return connection;
        }

        // create a new connection:
        SshConnection * const connection = new SshConnection(sshParams);
        connect(connection, &SshConnection::disconnected,
                this, &SshConnectionManager::cleanup);
        m_acquiredConnections.append(connection);

        return connection;
    }

    void releaseConnection(SshConnection *connection)
    {
        QMutexLocker locker(&m_listMutex);

        const bool wasAquired = m_acquiredConnections.removeOne(connection);
        QSSH_ASSERT_AND_RETURN(wasAquired);
        if (m_acquiredConnections.contains(connection))
            return;

        bool doDelete = false;
        connection->moveToThread(QCoreApplication::instance()->thread());
        if (m_deprecatedConnections.removeOne(connection)
                || connection->state() != SshConnection::Connected) {
            doDelete = true;
        } else {
            QSSH_ASSERT_AND_RETURN(!m_unacquiredConnections.contains(UnaquiredConnection(connection)));

            // It can happen that two or more connections with the same parameters were acquired
            // if the clients were running in different threads. Only keep one of them in
            // such a case.
            bool haveConnection = false;
            for (const UnaquiredConnection &c : m_unacquiredConnections) {
                if (c.connection->connectionParameters() == connection->connectionParameters()) {
                    haveConnection = true;
                    break;
                }
            }
            if (!haveConnection) {
                connection->closeAllChannels(); // Clean up after neglectful clients.
                m_unacquiredConnections.append(UnaquiredConnection(connection));
            } else {
                doDelete = true;
            }
        }

        if (doDelete) {
            disconnect(connection, nullptr, this, nullptr);
            m_deprecatedConnections.removeAll(connection);
            connection->deleteLater();
        }
    }

    void forceNewConnection(const SshConnectionParameters &sshParams)
    {
        QMutexLocker locker(&m_listMutex);

        for (int i = 0; i < m_unacquiredConnections.count(); ++i) {
            SshConnection * const connection = m_unacquiredConnections.at(i).connection;
            if (connection->connectionParameters() == sshParams) {
                disconnect(connection, nullptr, this, nullptr);
                delete connection;
                m_unacquiredConnections.removeAt(i);
                break;
            }
        }

        for (SshConnection * const connection : m_acquiredConnections) {
            if (connection->connectionParameters() == sshParams) {
                if (!m_deprecatedConnections.contains(connection))
                    m_deprecatedConnections.append(connection);
            }
        }
    }

private:
    Q_INVOKABLE void switchToCallerThread(SshConnection *connection, QObject *threadObj)
    {
        connection->moveToThread(qobject_cast<QThread *>(threadObj));
    }

    void cleanup()
    {
        QMutexLocker locker(&m_listMutex);

        SshConnection *currentConnection = qobject_cast<SshConnection *>(sender());
        if (!currentConnection)
            return;

        if (m_unacquiredConnections.removeOne(UnaquiredConnection(currentConnection))) {
            disconnect(currentConnection, nullptr, this, nullptr);
            currentConnection->deleteLater();
        }
    }

    void removeInactiveConnections()
    {
        QMutexLocker locker(&m_listMutex);
        for (int i = m_unacquiredConnections.count() - 1; i >= 0; --i) {
            UnaquiredConnection &c = m_unacquiredConnections[i];
            if (c.scheduledForRemoval) {
                disconnect(c.connection, nullptr, this, nullptr);
                c.connection->deleteLater();
                m_unacquiredConnections.removeAt(i);
            } else {
                c.scheduledForRemoval = true;
            }
        }
    }

private:
    // We expect the number of concurrently open connections to be small.
    // If that turns out to not be the case, we can still use a data
    // structure with faster access.
    QList<UnaquiredConnection> m_unacquiredConnections;

    // Can contain the same connection more than once; this acts as a reference count.
    QList<SshConnection *> m_acquiredConnections;

    QList<SshConnection *> m_deprecatedConnections;
    QMutex m_listMutex;
    QTimer m_removalTimer;
};

} // namespace Internal

static QMutex instanceMutex;

static Internal::SshConnectionManager &instance()
{
    static Internal::SshConnectionManager manager;
    return manager;
}

SshConnection *acquireConnection(const SshConnectionParameters &sshParams)
{
    QMutexLocker locker(&instanceMutex);
    return instance().acquireConnection(sshParams);
}

void releaseConnection(SshConnection *connection)
{
    QMutexLocker locker(&instanceMutex);
    instance().releaseConnection(connection);
}

void forceNewConnection(const SshConnectionParameters &sshParams)
{
    QMutexLocker locker(&instanceMutex);
    instance().forceNewConnection(sshParams);
}

} // namespace QSsh

#include "sshconnectionmanager.moc"
