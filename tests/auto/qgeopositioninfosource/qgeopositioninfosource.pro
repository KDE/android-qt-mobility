TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeopositioninfosource

include (../../../common.pri)

INCLUDEPATH += ../../../location
DEPENDPATH += ../../../buid/Debug/bin

# Input 
HEADERS += ../qlocationtestutils_p.h \
           ../testqgeopositioninfosource_p.h
SOURCES += ../qlocationtestutils.cpp \
           ../testqgeopositioninfosource.cpp \
           tst_qgeopositioninfosource.cpp

LIBS += -lQtLocation

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

