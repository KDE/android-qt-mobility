######################################################################
# Automatically generated by qmake (2.01a) pe 27. helmi 08:49:52 2009
######################################################################

TEMPLATE = app
TARGET = 

QT += testlib
CONFIG  += qtestlib
#DEFINES += PBK_UNIT_TEST

DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += .\inc
INCLUDEPATH += ..\..\inc

symbian:
{
	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
	
	# Input
	HEADERS +=  ut_transformcontact.h \
				../../inc/cnttransformcontact.h 
	
	SOURCES += ut_transformcontact.cpp \ 
				../../src/cnttransformcontact.cpp 
			
	TARGET.CAPABILITY = ALL -TCB
  
  LIBS += \
  	-lcntmodel \
  	-lQtContacts 
}
