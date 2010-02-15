
#This is a temporary workaround for internal Symbian builds
#QT_MAJOR_VERSION et al are not set
symbian {
    isEmpty(QT_MAJOR_VERSION)  {
         exists($${EPOCROOT}epoc32/data/z/system/install/Series60v5.2.sis) {
           QT_MAJOR_VERSION=4;
           QT_MINOR_VERSION=6;
           QT_PATCH_VERSION=0;
        }
    }
}

# config.pri specifies the configure options
include(pathconfig.pri)
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


#generate prf file for Qt integration
PRF_OUTPUT=$${QT_MOBILITY_BUILD_TREE}/features/mobility.prf

system(echo MOBILITY_PREFIX=$${QT_MOBILITY_PREFIX} > $$PRF_OUTPUT)
system(echo MOBILITY_INCLUDE=$${QT_MOBILITY_INCLUDE} >> $$PRF_OUTPUT)
system(echo MOBILITY_LIB=$${QT_MOBILITY_LIB} >> $$PRF_OUTPUT)

unix:!symbian:system(cat $${QT_MOBILITY_SOURCE_TREE}/features/mobility.prf.template >> $$PRF_OUTPUT)
win32:system(type $${QT_MOBILITY_SOURCE_TREE}\features\mobility.prf.template >> $$PRF_OUTPUT)
symbian:system(type $${QT_MOBILITY_SOURCE_TREE}\features\mobility.prf.template >> $$PRF_OUTPUT)

#symbian does not generate make install rule. we have to copy prf manually 
symbian:system(copy $${QT_MOBILITY_BUILD_TREE}\features\mobility.prf $$[QT_INSTALL_DATA]\mkspecs\features)

# install feature file
feature.path = $$[QT_INSTALL_DATA]/mkspecs/features
feature.files = $$QT_MOBILITY_BUILD_TREE/features/mobility.prf
INSTALLS += feature

TEMPLATE = subdirs
CONFIG+=ordered

SUBDIRS += src tools plugins
#built documentation snippets, if enabled
contains(build_docs, yes) {
    SUBDIRS += doc
    include(doc/doc.pri)

    OTHER_FILES += doc/src/*.qdoc doc/src/examples/*.qdoc
}

contains(build_unit_tests, yes):SUBDIRS+=tests
contains(build_examples, yes):SUBDIRS+=examples

# install Qt style headers
qtmheaders.path = $${QT_MOBILITY_INCLUDE}
qtmheaders.files = $${QT_MOBILITY_BUILD_TREE}/include/*

INSTALLS += qtmheaders
