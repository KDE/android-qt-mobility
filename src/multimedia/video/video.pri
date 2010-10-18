
INCLUDEPATH += video

PUBLIC_HEADERS += \
    video/qabstractvideobuffer.h \
    video/qabstractvideosurface.h \
    video/qvideoframe.h \
    video/qvideosurfaceformat.h

PRIVATE_HEADERS += \
    video/qabstractvideobuffer_p.h \
    video/qimagevideobuffer_p.h \
    video/qmemoryvideobuffer_p.h

SOURCES += \
    video/qabstractvideobuffer.cpp \
    video/qabstractvideosurface.cpp \
    video/qimagevideobuffer.cpp \
    video/qmemoryvideobuffer.cpp \
    video/qvideoframe.cpp \
    video/qvideosurfaceformat.cpp

