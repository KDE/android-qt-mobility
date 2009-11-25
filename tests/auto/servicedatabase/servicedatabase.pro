TARGET=tst_servicedatabase
QT = core sql
INCLUDEPATH += ../../../src/serviceframework
DEPENDPATH += ../../../src/serviceframework

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

CONFIG+=testcase

include(../../../common.pri)

# Input 
SOURCES += tst_servicedatabase.cpp \
            
CONFIG += mobility
MOBILITY = serviceframework

symbian {
    libBlock = \
        "$${LITERAL_HASH}ifdef WINSCW" \
        "LIBRARY SFWDatabaseManagerServer.lib" \
        "$${LITERAL_HASH}endif"
	DEFINES += QT_NODLL
    MMP_RULES += libBlock
    SOURCES += servicedatabase.cpp
    HEADERS += servicedatabase_p.h
    TARGET.CAPABILITY = ALL -TCB
}

wince* {
    DEPLOYMENT_PLUGIN += qsqlite
}
