# #####################################################################
# Versit
# #####################################################################
TEMPLATE = lib
TARGET = QtVersit
include(../../common.pri)

DEFINES += QT_BUILD_VERSIT_LIB QT_MAKEDLL QT_ASCII_CAST_WARNINGS

qtAddLibrary(QtContacts)

# Note, these need to go at the front of INCLUDEPATH, in case there
# are QtContacts headers already installed on the system
INCLUDEPATH =  . \
               ../contacts \
               ../contacts/requests \
               ../contacts/filters \
               ../contacts/details \
               $$INCLUDEPATH

# Input
PUBLIC_HEADERS +=  \
    qversitdocument.h \
    qversitproperty.h \
    qversitreader.h \
    qversitwriter.h \
    qversitcontactexporter.h \
    qversitcontactimporter.h \
    qversitcontacthandler.h \
    qversitresourcehandler.h

# Private Headers
PRIVATE_HEADERS += \
    qversitdocument_p.h \
    qversitdocumentwriter_p.h \
    qversitproperty_p.h \
    qversitreader_p.h \
    qversitwriter_p.h \
    qvcard21writer_p.h \
    qvcard30writer_p.h \
    qvcardrestorehandler_p.h \
    qversitcontactexporter_p.h \
    qversitcontactimporter_p.h \
    qversitdefs_p.h \
    qversitcontactsdefs_p.h \
    qversitcontactpluginloader_p.h \
    versitutils_p.h

# Implementation
SOURCES += \
    qversitdocument.cpp \
    qversitdocument_p.cpp \
    qversitdocumentwriter_p.cpp \
    qversitproperty.cpp \
    qversitreader.cpp \
    qversitreader_p.cpp \
    qversitwriter.cpp \
    qversitwriter_p.cpp \
    qvcard21writer.cpp \
    qvcard30writer.cpp \
    qvcardrestorehandler_p.cpp \
    qversitcontactexporter.cpp \
    qversitcontactexporter_p.cpp \
    qversitcontactimporter.cpp \
    qversitcontactimporter_p.cpp \
    qversitresourcehandler.cpp \
    qversitcontacthandler.cpp \
    qversitcontactpluginloader_p.cpp \
    versitutils.cpp

HEADERS += \
    $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS

symbian { 
    TARGET.UID3 = 0x2002BFBF
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB

    LIBS += -lefsrv

    VERSIT_DEPLOYMENT.sources = QtVersit.dll
    VERSIT_DEPLOYMENT.path = /sys/bin
    DEPLOYMENT += VERSIT_DEPLOYMENT
}

maemo5|maemo6 {
    CONFIG += create_pc create_prl
    QMAKE_PKGCONFIG_DESCRIPTION = Qt Mobility - Versit API
    pkgconfig.path = $$QT_MOBILITY_LIB/pkgconfig
    pkgconfig.files = QtVersit.pc

    INSTALLS += pkgconfig
}

CONFIG += middleware
include(../../features/deploy.pri)
