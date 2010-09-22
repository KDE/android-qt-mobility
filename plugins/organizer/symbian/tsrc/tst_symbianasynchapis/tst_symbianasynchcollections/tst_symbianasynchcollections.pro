



TEMPLATE = app
TARGET = tst_symbianasynchcollections
QT += testlib
CONFIG += qtestlib

symbian:
{
	INCLUDEPATH += ./inc
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    SOURCES += tst_symbianasynchcollections.cpp

    CONFIG += mobility
    MOBILITY = organizer

    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData

    LIBS += -lqtorganizer
}
