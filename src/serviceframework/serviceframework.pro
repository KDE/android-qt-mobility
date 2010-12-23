TEMPLATE = lib
TARGET = QtServiceFramework
QT = core \
    sql
include(../../common.pri)
DEFINES += QT_BUILD_SFW_LIB \
    QT_MAKEDLL

# include ipc extension
include(ipc/ipc.pri)
INCLUDEPATH += .
PUBLIC_HEADERS += qservice.h \
    qservicemanager.h \
    qserviceplugininterface.h \
    qservicecontext.h \
    qabstractsecuritysession.h \
    qserviceinterfacedescriptor.h \
    qservicefilter.h \
    qremoteserviceregister.h
PRIVATE_HEADERS += servicedatabase_p.h \
    databasemanager_p.h \
    servicemetadata_p.h \
    qserviceinterfacedescriptor_p.h \
    dberror_p.h
SOURCES += servicemetadata.cpp \
    qservicemanager.cpp \
    qserviceplugininterface.cpp \
    qservicecontext.cpp \
    qabstractsecuritysession.cpp \
    qserviceinterfacedescriptor.cpp \
    qservicefilter.cpp \
    dberror.cpp \
    qremoteserviceregister.cpp
symbian {
    contains(S60_VERSION, 5.2)|contains(MOBILITY_SD_MCL_BUILD, yes){
        DEFINES += SYMBIAN_EMULATOR_SUPPORTS_PERPROCESS_WSD
    }
    INCLUDEPATH += ./databasemanagerserver_symbian
    PRIVATE_HEADERS += databasemanager_symbian_p.h
    SOURCES += databasemanager_symbian.cpp
    TARGET.CAPABILITY = ALL \
        -TCB
    TARGET.UID3 = 0x2002AC84
    QtServiceFrameworkDeployment.sources = QtServiceFramework.dll \
        qsfwdatabasemanagerserver.exe
    QtServiceFrameworkDeployment.path = /sys/bin
    DEPLOYMENT += QtServiceFrameworkDeployment
}
else:SOURCES += servicedatabase.cpp \
    databasemanager.cpp
HEADERS += $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS \
    ipc/qremoteserviceregisterentry_p.h
CONFIG += middleware
include(../../features/deploy.pri)
