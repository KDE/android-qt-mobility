TEMPLATE = subdirs

SUBDIRS = m3u

win32:!wince* {
    TMP_INCLUDE = $$quote($$(INCLUDE))
    TMP_SEARCHPATHS = $$split(TMP_INCLUDE, ";") $$QMAKE_INCDIR
    for(p, TMP_SEARCHPATHS): exists($${p}/wmp.h): SUBDIRS *= wmp
}

unix:!mac {
    TMP_GST_LIBS = \
        gstreamer-0.10 >= 0.10.19 \
        gstreamer-base-0.10 >= 0.10.19 \
        gstreamer-interfaces-0.10 >= 0.10.19 \
        gstreamer-audio-0.10 >= 0.10.19 \
        gstreamer-video-0.10 >= 0.10.19
    system(pkg-config --exists \'$${TMP_GST_LIBS}\' --print-errors): {
        SUBDIRS += gstreamer
    } else {
        contains(QT_CONFIG, multimedia): SUBDIRS += audiocapture v4l
    }

} else {
    contains(QT_CONFIG, phonon): SUBDIRS += phonon
    contains(QT_CONFIG, multimedia): SUBDIRS += audiocapture
}
