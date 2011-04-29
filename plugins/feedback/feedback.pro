TEMPLATE = subdirs

include(../../common.pri)

contains(immersion_enabled, yes) {
    message("Building with Immersion TouchSense support")
    SUBDIRS += immersion
}

contains(meegotouchfeedback_enabled, yes) {
    message("Building with meegotouch theme haptics support")
    SUBDIRS += meegotouch
}

symbian:SUBDIRS += symbian
android:SUBDIRS += android
