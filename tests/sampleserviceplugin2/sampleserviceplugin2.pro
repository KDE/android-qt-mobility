TEMPLATE      = lib
CONFIG       += plugin testplugin
INCLUDEPATH  += ../../src/serviceframework
HEADERS       = sampleserviceplugin2.h
SOURCES       = sampleserviceplugin2.cpp
TARGET        = tst_sfw_sampleserviceplugin2
DESTDIR = .

include(../../common.pri)
CONFIG += mobility
MOBILITY = serviceframework

symbian {
    load(data_caging_paths)
    pluginDep.sources = tst_sfw_sampleserviceplugin2.dll
    pluginDep.path = $$QT_PLUGINS_BASE_DIR/plugins   
    
    DEPLOYMENT += pluginDep

    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB
}

xml.path = $$DESTDIR/xmldata
xml.files = xml/sampleservice2.xml
xml.CONFIG = no_link no_dependencies explicit_dependencies no_build combine ignore_no_exist no_clean
INSTALLS += xml
build_pass:ALL_DEPS+=install_xml
