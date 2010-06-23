TEMPLATE=app
INCLUDEPATH += ../../src/location

HEADERS = logfilepositionsource.h \
          clientapplication.h
SOURCES = logfilepositionsource.cpp \
          clientapplication.cpp \
          main.cpp

CONFIG += console

include(../examples.pri)

CONFIG += mobility
MOBILITY = location

symbian|wince* {
    symbian {
        addFiles.sources = simplelog.txt
        DEPLOYMENT += addFiles
    
        TARGET.CAPABILITY = Location
    }
    wince* {
        addFiles.sources = ./simplelog.txt
        addFiles.path = .
        DEPLOYMENT += addFiles
    }
} else {
    logfileexample.path = $$QT_MOBILITY_PREFIX/bin
    logfileexample.files = simplelog.txt
    INSTALLS += logfileexample
}

