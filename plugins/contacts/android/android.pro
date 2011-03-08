TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtcontacts_android)
PLUGIN_TYPE=contacts

include(../../../common.pri)

INCLUDEPATH += $$SOURCE_DIR/src/contacts $$SOURCE_DIR/src/contacts/details $$SOURCE_DIR/src/contacts/filters $$SOURCE_DIR/src/contacts/requests
CONFIG(android) : LIBS += -lQtCore -lQtContacts
HEADERS += \
        qcontactabook_p.h \
        qcontactandroidbackend_p.h \
        contactsjni.h
SOURCES += \
        qcontactabook.cpp \
        qcontactandroidbackend.cpp \
        contactsjni.cpp

target.path=$$QT_MOBILITY_PREFIX/plugins/contacts
INSTALLS += target
CONFIG += mobility
MOBILITY = contacts
