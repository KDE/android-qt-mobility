# #####################################################################
# Telephony Mobility API
# #####################################################################

TEMPLATE = lib
TARGET = QtTelephony

include(../../common.pri)

# Target gets fixed up in common.pri
DEFINES += QT_BUILD_TELEPHONY_LIB QT_MAKEDLL

# Input
PUBLIC_HEADERS += qtelephonycalllist.h \
                  qtelephonycallinfo.h \
                  qtelephonycallinfo_p.h

SOURCES +=        qtelephonycalllist.cpp \
                  qtelephonycallinfo.cpp

# Private Headers and sources
win32: {
    HEADERS += qtelephonycalllist_win_p.h
    SOURCES += qtelephonycalllist_win.cpp
}

symbian: {
    HEADERS += qtelephonycalllist_symbian_p.h
    SOURCES += qtelephonycalllist_symbian.cpp
}
 
linux-*: {
    QT += dbus
    HEADERS += linux/qtelephonycalllist_linux_p.h
    SOURCES += linux/qtelephonycalllist_linux.cpp
    SOURCES +=  linux/telepathy.cpp \
                linux/telepathylistener.cpp \
                linux/message.cpp \
                linux/dbusadaptor.cpp \
                linux/dbusinterface.cpp
    HEADERS +=  linux/telepathy_p.h \
                linux/telepathylistener_p.h \
                linux/message_p.h \
                linux/dbusadaptor_p.h \
                linux/dbusinterface_p.h
}
# TODO: remove comment to enable maemo
#        maemo* {
#            HEADERS += qtelephonycalllist_maemo_p.h
#            SOURCES += qtelephonycalllist_maemo.cpp
#        }
mac|maemo* {
    HEADERS += qtelephonycalllist_unsupported_p.h
}

HEADERS += $$PUBLIC_HEADERS

include (../../features/deploy.pri)
