TEMPLATE = app
TARGET = tst_qmessage

CONFIG += testcase
QT += testlib

include(../../../common.pri)

qtAddLibrary(QtMessaging)
INCLUDEPATH += ../../../messaging

# Link against the test support library
LIBS += -lsupport

symbian|wince*|maemo|win32 {
} else {
# Temporarily link against local qtopiamail lib (should be part of the platform)
LIBS += -L $$(QMF_LIBDIR) -lqtopiamail
}

wince*|symbian*: {
    addFiles.sources = testdata/*
    addFiles.path = testdata
    DEPLOYMENT += addFiles
}

wince* {
    DEFINES += TEADATA_DIR=\\\".\\\"
} !symbian {
    DEFINES += TESTDATA_DIR=\\\"$$PWD/\\\"
}

SOURCES += \
    tst_qmessage.cpp

