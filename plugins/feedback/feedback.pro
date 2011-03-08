TEMPLATE = subdirs

include(../../common.pri)

contains(immersion_enabled, yes) {
    message("Building with Immersion TouchSense support")
    SUBDIRS += immersion
}

symbian:SUBDIRS += symbian
maemo6:SUBDIRS += meegotouch
android:SUBDIRS += android
