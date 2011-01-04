TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtcontacts_android)
PLUGIN_TYPE=contacts
#include(../../qpluginbase.pri)


include(../../../common.pri)
#QT += core
INCLUDEPATH += $$SOURCE_DIR/src/contacts $$SOURCE_DIR/src/contacts/details $$SOURCE_DIR/src/contacts/filters $$SOURCE_DIR/src/contacts/requests
CONFIG(android-4) : LIBS += -lQtCore -lQtContacts -landroid_runtime -lsgl
             else : LIBS += -lQtCore -lQtContacts
HEADERS += \
        qcontactabook_p.h \
        qcontactandroidbackend_p.h \
        contactsjni.h
SOURCES += \
        qcontactabook.cpp \
        qcontactandroidbackend.cpp \
        contactsjni.cpp
#INCLUDEPATH += $$QT_SOURCE_TREE/src/contacts $$QT_SOURCE_TREE/src/contacts/details $$QT_SOURCE_TREE/src/contacts/filters \
#                $$QT_SOURCE_TREE/src/contacts/requests

#QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/contacts
#target.path += $$[QT_INSTALL_PLUGINS]/contacts
#INSTALLS += target
target.path=$$QT_MOBILITY_PREFIX/plugins/contacts
INSTALLS += target
CONFIG += mobility
MOBILITY = contacts
