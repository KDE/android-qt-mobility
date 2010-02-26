# #####################################################################
# Contacts Mobility API Tracker storage plugin
# #####################################################################

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtcontacts_tracker)
PLUGIN_TYPE=contacts

LIBS += -L/usr/lib
LIBS += -lqttracker
CONFIG += mobility
MOBILITY = contacts

# include version number for the plugin
include(version.pri)

INCLUDEPATH += /usr/include/qt4/QtMobility
INCLUDEPATH += .

HEADERS += qcontacttrackerbackend_p.h \
           qtrackercontactasyncrequest.h \
           trackerchangelistener.h \
           qtrackercontactslive.h \
           qtrackercontactsaverequest.h \
           qtrackerrelationshipfetchrequest.h \
           qtrackerrelationshipsaverequest.h \
           qtrackercontactidfetchrequest.h

SOURCES += qcontacttrackerbackend.cpp \
           qtrackercontactasyncrequest.cpp \
           trackerchangelistener.cpp \
           qtrackercontactslive.cpp \
           qtrackercontactsaverequest.cpp \
           qtrackerrelationshipfetchrequest.cpp \
           qtrackerrelationshipsaverequest.cpp \
           qtrackercontactidfetchrequest.cpp

target.path = /usr/lib/qt4/plugins/contacts
INSTALLS+=target
