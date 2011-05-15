TEMPLATE = app
TARGET = declarative-music-browser

include(../mobility_examples.pri)

win32 {
    #required by Qt SDK to resolve Mobility libraries
    CONFIG+=mobility
    MOBILITY+=docgallery
}


QT += declarative

HEADERS = \
        utility.h

SOURCES = \
    main.cpp \
    utility.cpp

OTHER_FILES += \
        musicbrowser.qml \
        MusicBrowserCore/AlbumDelegate.qml \
        MusicBrowserCore/AlbumView.qml \
        MusicBrowserCore/CategoryDelegate.qml \
        MusicBrowserCore/CategoryView.qml \
        MusicBrowserCore/LineEdit.qml \
        MusicBrowserCore/MenuButton.qml \
        MusicBrowserCore/AlbumDelegate.qml \
        MusicBrowserCore/SongDelegate.qml \
        MusicBrowserCore/SongProperties.qml \
        MusicBrowserCore/SongView.qml \
        MusicBrowserCore/TypeDelegate.qml

symbian: {
    load(data_caging_paths)
    TARGET.CAPABILITY = ReadDeviceData WriteDeviceData
}

RESOURCES += \
    musicbrowser.qrc
