QT += core network

TARGET=sftp
SOURCES=main.cpp sftptest.cpp argumentscollector.cpp 
HEADERS=sftptest.h argumentscollector.h parameters.h

include(../../qssh.pri) ## Required for IDE_LIBRARY_PATH and qtLibraryName

# Don't clutter the example
DEFINES -= QT_NO_CAST_FROM_ASCII
DEFINES -= QT_NO_CAST_TO_ASCII

LIBS += -L$$IDE_LIBRARY_PATH -l$$qtLibraryName(botan-2) -l$$qtLibraryName(QSsh)
