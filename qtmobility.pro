# config.pri specifies the configure options and is pulled in via staticconfig.pri
include(staticconfig.pri)
!include($$QT_MOBILITY_BUILD_TREE/config.pri) {
    error("Please run configure script");
    #also fails if .qmake.cache was not generated which may
    #happen if we are trying to shadow build w/o running configure
}

#don't build QtMobility if chosen config mismatches Qt's config
win32:!contains(CONFIG_WIN32,build_all) {
   contains(QT_CONFIG,debug):!contains(QT_CONFIG,release):contains(CONFIG_WIN32,release) {
       # Qt only build in debug mode
       error(QtMobility cannot be build in release mode if Qt is build in debug mode only)
   }
   !contains(QT_CONFIG,debug):contains(QT_CONFIG,release):contains(CONFIG_WIN32,debug) {
       # Qt only build in release mode
       error(QtMobility cannot be build in debug mode if Qt is build in release mode only)
   }
}

lessThan(QT_MAJOR_VERSION, 4) {
    error(Qt Mobility requires Qt 4.6 or higher. Qt $${QT_VERSION} was found.);
}

contains(QT_MAJOR_VERSION, 4):lessThan(QT_MINOR_VERSION, 6) {
    error(Qt Mobility requires Qt 4.6 or higher. Qt $${QT_VERSION} was found.);
}


# MCL builds for Symbian do not run configure and require some manual setup steps.
# This test permits SD builds to skip installation of mobility.prf from within qmake.
# It is installed in a separate step. MCL builds for SD must set the
# MOBILITY_SD_MCL_BUILD flag to yes.
!contains(MOBILITY_SD_MCL_BUILD, yes) {
    #generate prf file for Qt integration
    PRF_OUTPUT=$${QT_MOBILITY_BUILD_TREE}/features/mobility.prf

    system(echo MOBILITY_PREFIX=$${QT_MOBILITY_PREFIX} > $$PRF_OUTPUT)
    system(echo MOBILITY_INCLUDE=$${QT_MOBILITY_INCLUDE} >> $$PRF_OUTPUT)
    system(echo MOBILITY_LIB=$${QT_MOBILITY_LIB} >> $$PRF_OUTPUT)

    unix:!symbian:system(cat $${QT_MOBILITY_SOURCE_TREE}/features/mobility.prf.template >> $$PRF_OUTPUT)
    win32:system(type $${QT_MOBILITY_SOURCE_TREE}\features\mobility.prf.template >> $$PRF_OUTPUT)
    symbian:system(type $${QT_MOBILITY_SOURCE_TREE}\features\mobility.prf.template >> $$PRF_OUTPUT)

    PRF_CONFIG=$${QT_MOBILITY_BUILD_TREE}/features/mobilityconfig.prf
    system(echo MOBILITY_CONFIG=$${mobility_modules} > $$PRF_CONFIG)

    #symbian does not generate make install rule. we have to copy prf manually 
    symbian {
        FORMATDIR=$$[QT_INSTALL_DATA]\mkspecs\features
        FORMATDIR=$$replace(FORMATDIR,/,\\ )
        system(copy "$${QT_MOBILITY_BUILD_TREE}\features\mobility.prf $$FORMATDIR")
        system(copy "$${QT_MOBILITY_BUILD_TREE}\features\mobilityconfig.prf $$FORMATDIR")
    }

    # install config file
    config.path = $$[QT_INSTALL_DATA]/mkspecs/features
    config.files = $$QT_MOBILITY_BUILD_TREE/features/mobilityconfig.prf

    # install feature file
    feature.path = $$[QT_INSTALL_DATA]/mkspecs/features
    feature.files = $$QT_MOBILITY_BUILD_TREE/features/mobility.prf
    INSTALLS += feature config
}

TEMPLATE = subdirs
CONFIG+=ordered

SUBDIRS += src

contains(build_tools, yes) {
    SUBDIRS += tools
}

SUBDIRS += plugins

#built documentation snippets, if enabled
contains(build_docs, yes) {
    SUBDIRS += doc
    include(doc/doc.pri)

    OTHER_FILES += doc/src/*.qdoc doc/src/examples/*.qdoc
}

contains(build_unit_tests, yes):SUBDIRS+=tests
contains(build_examples, yes):SUBDIRS+=examples

#updating and deployment of translations requires Qt 4.6.3/qtPrepareTool
!symbian:defined(qtPrepareTool):SUBDIRS += translations

# install Qt style headers
qtmheaders.path = $${QT_MOBILITY_INCLUDE}

!symbian {
    qtmheaders.files = $${QT_MOBILITY_BUILD_TREE}/include/QtmBearer/* \
                         $${QT_MOBILITY_BUILD_TREE}/include/QtmContacts/* \
                         $${QT_MOBILITY_BUILD_TREE}/include/QtmLocation/* \
                         $${QT_MOBILITY_BUILD_TREE}/include/QtmMessaging/* \
                         $${QT_MOBILITY_BUILD_TREE}/include/QtMultimedia/* \
                         $${QT_MOBILITY_BUILD_TREE}/include/QtmPubSub/* \
                         $${QT_MOBILITY_BUILD_TREE}/include/QtmServiceFramework/* \
                         $${QT_MOBILITY_BUILD_TREE}/include/QtmVersit/* \
                         $${QT_MOBILITY_BUILD_TREE}/include/QtmSystemInfo/* \
                         $${QT_MOBILITY_BUILD_TREE}/include/QtmSensors/*
    INSTALLS += qtmheaders
} else {
    #absolute path does not work and so is shadow building for Symbian
    qtmAppHeaders = include/QtmContacts/* \
                          include/QtmVersit/*

    qtmMwHeaders = include/QtmBearer/* \
                       include/QtmLocation/* \
                       include/QtmMessaging/* \
                       include/QtMultimedia/* \
                       include/QtmPubSub/* \
                       include/QtmServiceFramework/* \
                       include/QtmSystemInfo/* \
                       include/QtmSensors/*

    contains(mobility_modules,contacts|versit) {
        for(api, qtmAppHeaders) {
            INCLUDEFILES=$$files($$api);
            #files() attaches a ';' at the end which we need to remove
            cleanedFiles=$$replace(INCLUDEFILES, ;,)
            for(header, cleanedFiles) {
                exists($$header):
                    BLD_INF_RULES.prj_exports += "$$header $$APP_LAYER_PUBLIC_EXPORT_PATH($$basename(header))"
            }
        }
    }

    contains(mobility_modules,serviceframework|location|bearer|publishsubscribe|systeminfo|multimedia|messaging) {
        for(api, qtmMwHeaders) {
            INCLUDEFILES=$$files($$api);
            #files() attaches a ';' at the end which we need to remove
            cleanedFiles=$$replace(INCLUDEFILES, ;,)
            for(header, cleanedFiles) {
                exists($$header):
                    BLD_INF_RULES.prj_exports += "$$header $$MW_LAYER_PUBLIC_EXPORT_PATH($$basename(header))"
            }
        }
    }
}

