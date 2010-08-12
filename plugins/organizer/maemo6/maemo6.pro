TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtorganizer_maemo6)
PLUGIN_TYPE=organizer

CONFIG += mobility
MOBILITY = organizer

CONFIG += link_pkgconfig
PKGCONFIG += libextendedkcal

include(../../../common.pri)

INCLUDEPATH += ../../../src/organizer \
                ../../../src/organizer/items \
                ../../../src/organizer/requests \
                ../../../src/organizer/filters \
                ../../../src/organizer/details

HEADERS += \
        qorganizermaemo6.h
SOURCES += \
        qorganizermaemo6.cpp

# Once we know what the pkgconfig deps are
# CONFIG += link_pkgconfig
PKGCONFIG += 
