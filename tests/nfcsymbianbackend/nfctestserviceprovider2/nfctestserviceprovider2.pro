TEMPLATE = app
TARGET = nfctestserviceprovider2 

QT        += core \
    gui 



CONFIG += mobility no_icon
MOBILITY = serviceframework \
             connectivity
  
INCLUDEPATH += ../../../src/connectivity/nfc
INCLUDEPATH += ../common

HEADERS   += nfctestserviceprovider2.h
SOURCES   += nfctestserviceprovider2_reg.rss \
    main.cpp \
    nfctestserviceprovider2.cpp
FORMS	  += nfctestserviceprovider2.ui
RESOURCES +=



wince*|symbian*: {
    addFiles.sources = nfctestserviceprovider2.xml
    addFiles.path = xmldata
    addFiles2.sources = nfctestserviceprovider2.xml
    addFiles2.path = /private/2002AC7F/import/
    DEPLOYMENT += addFiles addFiles2
}

symbian { 
	TARGET.UID3 = 0xe347d950
    TARGET.CAPABILITY = LocalServices
   }


wince* {
    DEFINES+= TESTDATA_DIR=\\\".\\\"
} else:!symbian {
    DEFINES += TESTDATA_DIR=\\\"$$PWD/\\\"
}

xml.path = $$QT_MOBILITY_EXAMPLES/xmldata
xml.files = nfctestserviceprovider2.xml
INSTALLS += xml

