TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeomaptextobject

include (../../../common.pri)

INCLUDEPATH += ../../../src/location \
                ../../../src/location/maps \
                ../qgeomapobjectplugin

# Input
SOURCES += tst_qgeomaptextobject.cpp \
            ../qgeomapobjectplugin/testhelper.cpp

HEADERS += ../qgeomapobjectplugin/testhelper.h

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}

