TARGET = tst_headers
INCLUDEPATH += ../../../src/publishsubscribe
INCLUDEPATH += ../../../src/bearer
INCLUDEPATH += ../../../src/contacts \
                ../../../src/contacts/filters \
                ../../../src/contacts/requests \
                ../../../src/contacts/details \
                ../../../src/feedback \
                ../../../src/gallery \
                ../../../src/location \
                ../../../src/location/maps \
                ../../../src/location/landmarks \
                ../../../src/messaging\ 
                ../../../src/multimedia \
                ../../../src/multimedia/audio \
                ../../../src/multimedia/video \
                ../../../src/organizer \
                ../../../src/organizer/details \
                ../../../src/organizer/filters \
                ../../../src/organizer/requests \
                ../../../src/organizer/items \
                ../../../src/sensors \
                ../../../src/serviceframework \
                ../../../src/systeminfo \
                ../../../src/telephony \
                ../../../src/versit

CONFIG += testcase

QT+= core network

SOURCES += tst_headers.cpp headersclean.cpp

include (../../../common.pri)
