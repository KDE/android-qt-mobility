TEMPLATE = app
TARGET = servicebrowser
INCLUDEPATH += ../../src/serviceframework

include(../mobility_examples.pri)

QT += gui

# Input 
HEADERS += servicebrowser.h
SOURCES += servicebrowser.cpp \
           main.cpp

CONFIG += mobility
MOBILITY = serviceframework

mac:CONFIG -= app_bundle

symbian {
    addFiles.sources = ../filemanagerplugin/filemanagerservice.xml
    addFiles.sources += ../bluetoothtransferplugin/bluetoothtransferservice.xml
    addFiles.sources += ../notesmanagerplugin/notesmanagerservice.xml
    addFiles.path = xmldata
    DEPLOYMENT += addFiles

    TARGET.CAPABILITY = ReadUserData WriteUserData
}
