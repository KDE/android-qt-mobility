TARGET = btclient
CONFIG += link_pkgconfig
PKGCONFIG += dbus-1
QT -= gui \
    core
SOURCES = btclient.c
HEADERS += btclient.h
