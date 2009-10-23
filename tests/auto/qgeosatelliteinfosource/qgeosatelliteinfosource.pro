TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeosatelliteinfosource

include (../../../common.pri)

INCLUDEPATH += ../../../location
        
# Input 
SOURCES += tst_qgeosatelliteinfosource.cpp \
        ../testqgeosatelliteinfosource.cpp \
        ../qlocationtestutils.cpp
HEADERS += ../testqgeosatelliteinfosource_p.h \
        ../qlocationtestutils_p.h

LIBS += -lQtLocation

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

