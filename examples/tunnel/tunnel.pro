QT += core network
TARGET =tunnel
SOURCES = \
    main.cpp \
    argumentscollector.cpp \
    directtunnel.cpp \
    forwardtunnel.cpp
HEADERS = \
    argumentscollector.h \
    directtunnel.h \
    forwardtunnel.h

include(../../qssh.pri) ## Required for IDE_LIBRARY_PATH and qtLibraryName

# Don't clutter the example
DEFINES -= QT_NO_CAST_FROM_ASCII
DEFINES -= QT_NO_CAST_TO_ASCII

LIBS += -L$$IDE_LIBRARY_PATH -l$$qtLibraryName(botan-2) -l$$qtLibraryName(QSsh)
