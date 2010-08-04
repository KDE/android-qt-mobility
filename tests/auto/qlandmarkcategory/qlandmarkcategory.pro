TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qlandmarkcategory

include (../../../common.pri)

INCLUDEPATH += ../../../src/location \
                 ../../../src/location/landmarks

# Input 
SOURCES += tst_qlandmarkcategory.cpp

CONFIG += mobility
MOBILITY = landmarks location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
