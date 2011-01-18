TARGET = tst_qremoteserviceregister
INCLUDEPATH += ../../../src/serviceframework
INCLUDEPATH += ../../../src/serviceframework/ipc
DEPENDPATH += ../../../src/serviceframework
DEPENDPATH += ../../../src/serviceframework/ipc

QT = core testlib
CONFIG+=testcase
TEMPLATE=app

include(../../../common.pri)

CONFIG += mobility
MOBILITY = serviceframework

SOURCES += tst_qremoteserviceregister.cpp
HEADERS += service.h

symbian {
    TARGET.CAPABILITY = ALL -TCB
}

wince*|symbian*: {
    addFiles.sources = testdata/*
    addFiles.path = xmldata
    DEPLOYMENT += addFiles
}
wince* {
    DEFINES+= TESTDATA_DIR=\\\".\\\"
} else:!symbian {
    DEFINES += TESTDATA_DIR=\\\"$$PWD/\\\"
}

xml.path = $$DESTDIR/xmldata
xml.files = testdata/rsrexampleservice.xml
xml.CONFIG = no_link no_dependencies explicit_dependencies no_build combine ignore_no_exist no_clean
INSTALLS += xml
build_pass:ALL_DEPS+=install_xml

