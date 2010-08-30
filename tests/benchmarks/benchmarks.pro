include(../../staticconfig.pri)

TEMPLATE = subdirs

contains(mobility_modules,serviceframework): SUBDIRS += sampleserviceplugin  serviceframework
contains(mobility_modules,contacts): SUBDIRS += contactsoverhead
contains(mobility_modules,location): SUBDIRS += location
contains(mobility_modules,systeminfo): SUBDIRS += qsysteminfo
contains(mobility_modules,bearer): SUBDIRS += bearer
contains(mobility_modules,messaging) {
    contains(qmf_enabled,yes)|symbian {
        !win32-g++: SUBDIRS += messaging
    }
}

