TEMPLATE = app
TARGET = servicebrowser
INCLUDEPATH += ../../serviceframework

include(../examples.pri)

QT += gui

# Input 
HEADERS += servicebrowser.h
SOURCES += servicebrowser.cpp \
           main.cpp

LIBS += -lQtServiceFramework

symbian {
    addFiles.sources = ../filemanagerplugin/filemanagerservice.xml
    addFiles.sources += ../bluetoothtransferplugin/bluetoothtransferservice.xml
    addFiles.path = xmldata
    DEPLOYMENT += addFiles

    TARGET.CAPABILITY = ALL -TCB
}
