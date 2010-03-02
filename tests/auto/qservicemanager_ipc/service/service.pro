TARGET = qservicemanager_ipc_service
INCLUDEPATH += ../../../../src/serviceframework \
                ../../../../src/serviceframework/ipc
DEPENDPATH += ../../../../src/serviceframework

QT = core testlib
CONFIG+=testcase
TEMPLATE=app

include(../../../../common.pri)

CONFIG += mobility
MOBILITY = serviceframework

SOURCES += main.cpp
HEADERS += 

symbian {
    TARGET.CAPABILITY = ALL -TCB
}

wince*|symbian*: {
    addFiles.sources = testdata/*
    addFiles.path = testdata
    DEPLOYMENT += addFiles
}
wince* {
    DEFINES+= TESTDATA_DIR=\\\".\\\"
} else:!symbian {
    DEFINES += TESTDATA_DIR=\\\"$$PWD/\\\"
}

xml.path = $$DESTDIR/xmldata
xml.files = testdata/ipcexampleservice.xml
xml.CONFIG = no_link no_dependencies explicit_dependencies no_build combine ignore_no_exist no_clean
INSTALLS += xml
build_pass:ALL_DEPS+=install_xml

