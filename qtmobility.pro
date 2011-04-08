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
    win32 {
        nativePath=$$replace(QT_MOBILITY_SOURCE_TREE,/,\\)
        system(type $${nativePath}\\features\\mobility.prf.template >> $$PRF_OUTPUT)
    }
    symbian {
        nativePath=$$replace(QT_MOBILITY_SOURCE_TREE,/,\\)
        system(type $${nativePath}\\features\\mobility.prf.template >> $$PRF_OUTPUT)
    }

    PRF_CONFIG=$${QT_MOBILITY_BUILD_TREE}/features/mobilityconfig.prf
    system(echo MOBILITY_CONFIG=$${mobility_modules} > $$PRF_CONFIG)
    system(echo MOBILITY_VERSION = 1.1.3 >> $$PRF_CONFIG)
    system(echo MOBILITY_MAJOR_VERSION = 1 >> $$PRF_CONFIG)
    system(echo MOBILITY_MINOR_VERSION = 1 >> $$PRF_CONFIG)
    system(echo MOBILITY_PATCH_VERSION = 3 >> $$PRF_CONFIG)

    #symbian does not generate make install rule. we have to copy prf manually 
    symbian {
        nativePath=$$replace(QT_MOBILITY_BUILD_TREE,/,\\)
        FORMATDIR=$$[QT_INSTALL_DATA]\\mkspecs\\features
        FORMATDIR=$$replace(FORMATDIR,/,\\)
        system(copy "$${nativePath}\\features\\mobility.prf $$FORMATDIR")
        system(copy "$${nativePath}\\features\\mobilityconfig.prf $$FORMATDIR")
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

    OTHER_FILES += \
        doc/src/*.qdoc \
        doc/src/legal/*.qdoc \
        doc/src/examples/*.qdoc \
        doc/src/plugins/*.qdoc
}

contains(build_unit_tests, yes):SUBDIRS+=tests
contains(build_examples, yes):SUBDIRS+=examples
contains(build_demos, yes):SUBDIRS+=demos

#updating and deployment of translations requires Qt 4.6.3/qtPrepareTool
!symbian:defined(qtPrepareTool):SUBDIRS += translations

# install Qt style headers

!symbian {
    qtmheadersglobal.path = $${QT_MOBILITY_INCLUDE}/QtMobility
    qtmheadersglobal.files = $${QT_MOBILITY_BUILD_TREE}/include/QtMobility/*
    INSTALLS += qtmheadersglobal

    contains(mobility_modules,bearer) {
        qtmheadersbearer.path = $${QT_MOBILITY_INCLUDE}/QtBearer
        qtmheadersbearer.files = $${QT_MOBILITY_BUILD_TREE}/include/QtBearer/*
        INSTALLS += qtmheadersbearer
    }

    contains(mobility_modules,contacts) {
        qtmheaderscontacts.path = $${QT_MOBILITY_INCLUDE}/QtContacts
        qtmheaderscontacts.files = $${QT_MOBILITY_BUILD_TREE}/include/QtContacts/*
        INSTALLS += qtmheaderscontacts
    }

    contains(mobility_modules,location) {
        qtmheaderslocation.path = $${QT_MOBILITY_INCLUDE}/QtLocation
        qtmheaderslocation.files = $${QT_MOBILITY_BUILD_TREE}/include/QtLocation/*
        INSTALLS += qtmheaderslocation
    }

    contains(qmf_enabled, yes)|wince*|win32|maemo5 {
        contains(mobility_modules,messaging) {
            qtmheadersmessaging.path = $${QT_MOBILITY_INCLUDE}/QtMessaging
            qtmheadersmessaging.files = $${QT_MOBILITY_BUILD_TREE}/include/QtMessaging/*
            INSTALLS += qtmheadersmessaging
        }
    }

    contains(mobility_modules,multimedia) {
        qtmheadersmultimedia.path = $${QT_MOBILITY_INCLUDE}/QtMultimediaKit
        qtmheadersmultimedia.files = $${QT_MOBILITY_BUILD_TREE}/include/QtMultimediaKit/*
        INSTALLS += qtmheadersmultimedia
    }

    contains(mobility_modules,publishsubscribe) {
        qtmheaderspubsub.path = $${QT_MOBILITY_INCLUDE}/QtPublishSubscribe
        qtmheaderspubsub.files = $${QT_MOBILITY_BUILD_TREE}/include/QtPublishSubscribe/*
        INSTALLS += qtmheaderspubsub
    }

    contains(mobility_modules,serviceframework) {
        qtmheaderssfw.path = $${QT_MOBILITY_INCLUDE}/QtServiceFramework
        qtmheaderssfw.files = $${QT_MOBILITY_BUILD_TREE}/include/QtServiceFramework/*
        INSTALLS += qtmheaderssfw
    }

    contains(mobility_modules,versit) {
        qtmheadersversit.path = $${QT_MOBILITY_INCLUDE}/QtVersit
        qtmheadersversit.files = $${QT_MOBILITY_BUILD_TREE}/include/QtVersit/*
        INSTALLS += qtmheadersversit

        contains(mobility_modules,organizer) {
            qtmheadersversitorg.path = $${QT_MOBILITY_INCLUDE}/QtVersitOrganizer
            qtmheadersversitorg.files = $${QT_MOBILITY_BUILD_TREE}/include/QtVersitOrganizer/*
            INSTALLS += qtmheadersversitorg
        }
    }

    contains(mobility_modules,systeminfo) {
        qtmheaderssysteminfo.path = $${QT_MOBILITY_INCLUDE}/QtSystemInfo
        qtmheaderssysteminfo.files = $${QT_MOBILITY_BUILD_TREE}/include/QtSystemInfo/*
        INSTALLS += qtmheaderssysteminfo
    }

    contains(mobility_modules,sensors) {
        qtmheaderssensors.path = $${QT_MOBILITY_INCLUDE}/QtSensors
        qtmheaderssensors.files = $${QT_MOBILITY_BUILD_TREE}/include/QtSensors/*
        INSTALLS += qtmheaderssensors
    }

    contains(mobility_modules,organizer) {
        qtmheadersorganizer.path = $${QT_MOBILITY_INCLUDE}/QtOrganizer
        qtmheadersorganizer.files = $${QT_MOBILITY_BUILD_TREE}/include/QtOrganizer/*
        INSTALLS += qtmheadersorganizer
    }

    contains(mobility_modules,feedback) {
        qtmheadersfeedback.path = $${QT_MOBILITY_INCLUDE}/QtFeedback
        qtmheadersfeedback.files = $${QT_MOBILITY_BUILD_TREE}/include/QtFeedback/*
        INSTALLS += qtmheadersfeedback
    }

    contains(mobility_modules,gallery) {
        qtmheadersgallery.path = $${QT_MOBILITY_INCLUDE}/QtGallery
        qtmheadersgallery.files = $${QT_MOBILITY_BUILD_TREE}/include/QtGallery/*
        INSTALLS += qtmheadersgallery
    }
} else {
    #absolute path does not work and 
    #include <QtMyLibrary/class.h> style does not work either
    qtmGlobalHeaders = include/QtMobility/*
    for(api, qtmGlobalHeaders) {
        INCLUDEFILES=$$files($$api);
        #files() attaches a ';' at the end which we need to remove
        cleanedFiles=$$replace(INCLUDEFILES, ;,)
        for(header, cleanedFiles) {
            exists($$header):
                BLD_INF_RULES.prj_exports += "$$header $$MW_LAYER_PUBLIC_EXPORT_PATH($$basename(header))"
        }
    }


    qtmAppHeaders = include/QtContacts/* \
                       include/QtVersit/* \
                       include/QtVersitOrganizer/* \
                       include/QtOrganizer/*

    qtmMwHeaders = include/QtBearer/* \
                       include/QtLocation/* \
                       include/QtMessaging/* \
                       include/QtMultimediaKit/* \
                       include/QtPublishSubscribe/* \
                       include/QtServiceFramework/* \
                       include/QtSystemInfo/* \
                       include/QtSensors/* \
                       include/QtFeedback/* \
                       include/QtGallery/*

    contains(mobility_modules,contacts|versit|organizer) {
        for(api, qtmAppHeaders) {
            INCLUDEFILES=$$files($$api);

            #files() attaches a ';' at the end which we need to remove
            cleanedFiles=$$replace(INCLUDEFILES, ;,)

            #files() uses windows path separator ('\')  but bld.inf requires '/'
            INCLUDEFILES=$$cleanedFiles
            cleanedFiles=$$replace(INCLUDEFILES, \\\\,/)

            for(header, cleanedFiles) {
                exists($$header):
                    BLD_INF_RULES.prj_exports += "$$header $$APP_LAYER_PUBLIC_EXPORT_PATH($$basename(header))"
            }
        }
    }

    contains(mobility_modules,serviceframework|location|bearer|publishsubscribe|systeminfo|multimedia|messaging|feedback|sensors|gallery) {
        for(api, qtmMwHeaders) {
            INCLUDEFILES=$$files($$api);

            #files() attaches a ';' at the end which we need to remove
            cleanedFiles=$$replace(INCLUDEFILES, ;,)

            #files() uses windows path separator ('\')  but bld.inf requires '/'
            INCLUDEFILES=$$cleanedFiles
            cleanedFiles=$$replace(INCLUDEFILES, \\\\,/)

            for(header, cleanedFiles) {
                exists($$header):
                    BLD_INF_RULES.prj_exports += "$$header $$MW_LAYER_PUBLIC_EXPORT_PATH($$basename(header))"
            }
        }
    }
}

