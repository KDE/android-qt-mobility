TEMPLATE=app
TARGET=servicexmlgen

INCLUDEPATH += ../../src/serviceframework

DEFINES += SERVICE_XML_GENERATOR
HEADERS = servicexmlgen.h \
          servicewidget.h \
          interfacestabwidget.h \
          interfacewidget.h \
          mandatorylineedit.h \
          errorcollector.h

SOURCES = servicexmlgen.cpp \
          servicewidget.cpp \
          interfacestabwidget.cpp \
          interfacewidget.cpp \
          mandatorylineedit.cpp \
          errorcollector.cpp \
	  ../../src/serviceframework/servicemetadata.cpp \
	  ../../src/serviceframework/qserviceinterfacedescriptor.cpp

include(../../common.pri)

include(../../features/deploy.pri)
