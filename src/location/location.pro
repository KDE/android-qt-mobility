TEMPLATE = lib
TARGET = QtLocation
QT = core gui network sql

include(../../common.pri)

DEFINES += QT_BUILD_LOCATION_LIB QT_MAKEDLL

INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
INCLUDEPATH += .
DEPENDPATH += .

include(landmarks/landmarks.pri)
include(maps/maps.pri)

PUBLIC_HEADERS += \
                    qgeoaddress.h \
                    qgeoareamonitor.h \
                    qgeoboundingarea.h \
                    qgeoboundingbox.h \
                    qgeoboundingcircle.h \
                    qgeocoordinate.h \
                    qgeoplace.h \
                    qgeopositioninfo.h \
                    qgeopositioninfosource.h \
                    qgeosatelliteinfo.h \
                    qgeosatelliteinfosource.h \
                    qnmeapositioninfosource.h

PRIVATE_HEADERS += \
                    qgeoaddress_p.h \
                    qgeoboundingbox_p.h \
                    qgeoboundingcircle_p.h \
                    qgeoplace_p.h \
                    qlocationutils_p.h \
                    qnmeapositioninfosource_p.h

symbian {
    PRIVATE_HEADERS += qgeopositioninfosource_s60_p.h \
                       qmlbackendao_s60_p.h \
                       qgeosatelliteinfosource_s60_p.h \
                       notificationcallback_s60_p.h \
                       notificationsatellitecallback_s60_p.h

    contains(lbt_enabled, yes) {
        PRIVATE_HEADERS += \
                       qgeoareamonitor_s60_p.h \
                       qmlbackendmonitorao_s60_p.h \
                       qmlbackendmonitorcreatetriggerao_s60_p.h \
                       qmlbackendmonitorinfo_s60_p.h \
                       qmlbackendtriggerchangeao_s60_p.h \
                       notificationmonitorcallback_s60_p.h
    }

    SOURCES += qgeopositioninfosource_s60.cpp \
               qgeosatelliteinfosource_s60.cpp \
               qmlbackendao_s60.cpp

    contains(lbt_enabled, yes) {
        SOURCES += \
               qgeoareamonitor_s60.cpp \
               qmlbackendmonitorao_s60.cpp \
               qmlbackendmonitorcreatetriggerao_s60.cpp \
               qmlbackendmonitorinfo_s60.cpp \
               qmlbackendtriggerchangeao_s60.cpp
    }
}

wince* {
    PRIVATE_HEADERS += qgeopositioninfosource_wince_p.h \
                       qgeosatelliteinfosource_wince_p.h \
                       qgeoinfothread_wince_p.h
    SOURCES += qgeopositioninfosource_wince.cpp \
               qgeosatelliteinfosource_wince.cpp \
               qgeoinfothread_wince.cpp
    LIBS += -lgpsapi
}

maemo6 {
    CONFIG += qdbus link_pkgconfig
    SOURCES += qgeopositioninfosource_maemo.cpp \
                qgeosatelliteinfosource_maemo.cpp \
                dbuscomm_maemo.cpp \
                dbusserver_maemo.cpp
    PRIVATE_HEADERS += qgeopositioninfosource_maemo_p.h \
                qgeosatelliteinfosource_maemo_p.h \
                dbuscomm_maemo_p.h \
                dbusserver_maemo_p.h
    CONFIG += create_pc create_prl
    QMAKE_PKGCONFIG_NAME = libQtLocation
    QMAKE_PKGCONFIG_DESCRIPTION = libQtLocation library
    QMAKE_PKGCONFIG_INCDIR = $${QT_MOBILITY_INCLUDE}/QtLocation
    QMAKE_PKGCONFIG_CFLAGS += -I$${QT_MOBILITY_INCLUDE}/QtMobility
    pkgconfig.path = $$QT_MOBILITY_LIB/pkgconfig
    pkgconfig.files = QtLocation.pc
}

maemo5 {
    CONFIG += qdbus link_pkgconfig
    SOURCES += gconfitem.cpp \
               liblocationwrapper.cpp \
               qgeopositioninfosource_maemo5.cpp \
               qgeosatelliteinfosource_maemo5.cpp \
               qgeoareamonitor_maemo.cpp
    HEADERS += gconfitem_p.h  \
               liblocationwrapper_p.h \
               qgeopositioninfosource_maemo5_p.h \
               qgeosatelliteinfosource_maemo5_p.h \
               qgeoareamonitor_maemo_p.h
    PKGCONFIG += glib-2.0  gconf-2.0
    CONFIG += create_pc create_prl
    LIBS += -llocation
    QMAKE_PKGCONFIG_REQUIRES = glib-2.0 gconf-2.0
    pkgconfig.path = $$QT_MOBILITY_LIB/pkgconfig
    pkgconfig.files = QtLocation.pc
}
HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

SOURCES += \
            qgeoaddress.cpp \
            qgeoareamonitor.cpp \
            qgeoboundingarea.cpp \
            qgeoboundingbox.cpp \
            qgeoboundingcircle.cpp \
            qgeocoordinate.cpp \
            qgeoplace.cpp \
            qgeopositioninfo.cpp \
            qgeopositioninfosource.cpp \
            qgeosatelliteinfo.cpp \
            qgeosatelliteinfosource.cpp \
            qlocationutils.cpp \
            qnmeapositioninfosource.cpp

symbian {
    TARGET.CAPABILITY = ALL -TCB
    TARGET.UID3 = 0x2002AC83

    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions \
                   $${EPOCROOT}epoc32/include/LBTHeaders \
                   $${EPOCROOT}epoc32/include/platform
    LIBS += -llbs
    LIBS += -lefsrv
    contains(lbt_enabled, yes) {
        LIBS += -llbt
    }

    QtLocationDeployment.sources = QtLocation.dll
    QtLocationDeployment.path = /sys/bin
    DEPLOYMENT += QtLocationDeployment
}

simulator {
    QT += gui
    SOURCES += qgeopositioninfosource_simulator.cpp \
                qlocationdata_simulator.cpp \
                qgeosatelliteinfosource_simulator.cpp \
                qgeoareamonitor_simulator.cpp \
                qlocationconnection_simulator.cpp
    HEADERS += qgeopositioninfosource_simulator_p.h \
                qlocationdata_simulator_p.h \
                qgeosatelliteinfosource_simulator_p.h \
                qgeoareamonitor_simulator_p.h \
                qlocationconnection_simulator_p.h
    INCLUDEPATH += ../mobilitysimulator
    qtAddLibrary(QtMobilitySimulator)
}

CONFIG += middleware
include(../../features/deploy.pri)
