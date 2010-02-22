TEMPLATE = app
TARGET = tst_qmessageservice

CONFIG += testcase
QT += testlib

include(../../../common.pri)
include(../support/support.pri)

qtAddLibrary(QtMessaging)
INCLUDEPATH += ../../../src/messaging

symbian|wince*|maemo5|win32|mac {
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
    tst_qmessageservice.cpp

symbian {
    LIBS += -limcm \
            -lcommsdat \
            -lmsgs

    TARGET.CAPABILITY = NetworkServices \
                        LocalServices \
                        ReadUserData \
                        WriteUserData \
                        UserEnvironment \
                        ReadDeviceData \
                        WriteDeviceData
}
