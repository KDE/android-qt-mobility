TEMPLATE=app
TARGET=orientation
include(../../examples.pri)
QT+=declarative
CONFIG+=mobility
MOBILITY+=sensors
INCLUDEPATH += ../../../src/sensors
SOURCES=main.cpp
RESOURCES = orientation.qrc
OTHER_FILES += orientation.qml

CONFIG+=strict_flags

