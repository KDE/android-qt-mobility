TEMPLATE = app
TARGET = battery-provider
INCLUDEPATH += ../../../context
DEPENDPATH += ../../../context

HEADERS = batteryprovider.h

SOURCES = main.cpp \
          batteryprovider.cpp

FORMS = batteryprovider.ui

include(../../../common.pri)
LIBS += -lQtPublishSubscribe
