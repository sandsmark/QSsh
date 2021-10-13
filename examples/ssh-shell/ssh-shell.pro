include(../../qssh.pri)
QT += network

TARGET=shell
SOURCES=main.cpp shell.cpp argumentscollector.cpp
HEADERS=shell.h argumentscollector.h

LIBS += -L$$IDE_LIBRARY_PATH -l$$qtLibraryName(botan-2) -l$$qtLibraryName(QSsh)
