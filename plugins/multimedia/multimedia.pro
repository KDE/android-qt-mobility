######################################################################
#
# Mobility API project - multimedia 
#
######################################################################

TEMPLATE = subdirs

include (../../staticconfig.pri)

SUBDIRS += m3u

win32 {
    SUBDIRS += audiocapture
}

win32:!wince* {
    contains(directshow_enabled, yes): SUBDIRS += directshow
    contains(wmp_enabled, yes): SUBDIRS += wmp
}

unix:!mac:!symbian {
    TMP_GST_LIBS = \
        gstreamer-0.10 >= 0.10.19 \
        gstreamer-base-0.10 >= 0.10.19 \
        gstreamer-interfaces-0.10 >= 0.10.19 \
        gstreamer-audio-0.10 >= 0.10.19 \
        gstreamer-video-0.10 >= 0.10.19

    system(pkg-config --exists \'$${TMP_GST_LIBS}\' --print-errors): {
        SUBDIRS += gstreamer
    } else {
        SUBDIRS += audiocapture
    }
    SUBDIRS += v4l
}

mac {
    SUBDIRS += audiocapture qt7
}

symbian:SUBDIRS += symbian
