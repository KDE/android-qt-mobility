TEMPLATE      = lib
CONFIG       += plugin testplugin
INCLUDEPATH  += ../../serviceframework
HEADERS       = testserviceplugin.h testservice.h testserviceinterface.h
SOURCES       = testserviceplugin.cpp

TARGET        = tst_sfw_testservice2plugin
DESTDIR = .

symbian {
    load(data_caging_paths)
    pluginDep.sources = tst_sfw_testservice2plugin.dll
    pluginDep.path = $$QT_PLUGINS_BASE_DIR    

    DEPLOYMENT += pluginDep    
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB
}

include(../../common.pri)
LIBS += -lQtServiceFramework

xml.path = $$DESTDIR/xmldata
xml.files = xml/testserviceplugin.xml
xml.CONFIG = no_link no_dependencies explicit_dependencies no_build combine ignore_no_exist no_clean
INSTALLS += xml
build_pass:ALL_DEPS+=install_xml
