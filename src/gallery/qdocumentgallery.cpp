/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdocumentgallery.h"
#include "qabstractgallery_p.h"

#include <QtCore/qstringlist.h>

QTM_BEGIN_NAMESPACE

/*!
    \class QDocumentGallery

    \ingroup gallery
    \ingroup gallery-galleries
    \since 1.1

    \inmodule QtGallery

    \brief The QDocumentGallery class provides access to a gallery of documents
    and media present on a device.

    \section2 Requests

    Requests are the interface through which queries and actions are executed
    against a QDocumentGallery.

    \list

    \o QGalleryQueryRequest can be used with QDocumentGallery to find and
    navigate items present on a device.

    \o QGalleryItemRequest provides an API to query information about a single
    item in a QDocumentGallery.

    \o QGalleryTypeRequest provides a query for information about an item type
    supported by QDocumentGallery.

    \endlist

    \section2 Item types

    The items present in a QDocumentGallery can be of both file type such as
    documents and media, or categorical type such as genres or albums.

    \section3 File types

    Currently on Symbian, \l Folder, \l Document, \l Text, and \l Playlist are
    not supported.

    \list
    \o \l File
    \o \l Folder
    \o \l Document
    \o \l Text
    \o \l Audio
    \o \l Image
    \o \l Video
    \o \l Playlist
    \endlist

    \section3 Categorical types

    \list
    \o \l Artist
    \o \l AlbumArtist
    \o \l Album
    \o \l AudioGenre
    \o \l PhotoAlbum
    \endlist

    \section2 Properties

    The item meta-data stored in a QDocumentGallery is addressed through
    properties.

    The set of properties addressable differs between item type but some
    properties are shared by most types.

    \section3 Common item properties

    Currently on Maemo, only the \l title property is available for the
    categorical types. For Maemo 6, all these properties are supported for
    file types.

    For Maemo 5, \l comments is only supported for \l Document. The property of
    \l description is not supported by \l Document, \l Video, and \l Playlist.
    Also, \l rating is not supported for \l Document.

    On Symbian, for file types, \l keywords and \l subject are not supported,
    and \l language is only supported for \l Video; for categorical types, only
    \l title is supported, and \l url is only supported for \l PhotoAlbum.

    \list
    \o \l author
    \o \l comments
    \o \l copyright
    \o \l description
    \o \l keywords
    \o \l language
    \o \l rating
    \o \l subject
    \o \l title
    \o \l url
    \endlist

    \section3 Common type properties

    \list
    \o \l count
    \endlist
*/

/*!
    \enum QDocumentGallery::Error
    This enum defines errors in a Gallery.

    \value NoError No error.
    \value NoGallery No Gallery defined or found.
    \value NotSupported Gallery is not supported.
    \value ConnectionError Connection to gallery is broken.
    \value ItemIdError Id of a item is wrong.
    \value ItemTypeError Type of an item is wrong.
    \value FilterError Error in filter found.
*/

/*!
    \variable QDocumentGallery::File

    This type matches all files in the document gallery.

    Properties typically available from files include:

    \list
    \o \l fileExtension
    \o \l fileName
    \o \l filePath
    \o \l fileSize
    \o \l lastModified
    \o \l lastAccessed
    \o \l mimeType
    \o \l path
    \endlist
*/

Q_DEFINE_GALLERY_TYPE(QDocumentGallery, File)

/*!
    \variable QDocumentGallery::Folder

    This type matches all file-system folders in the document gallery.

    In addition to the properties derived from the \l File type folders may
    also provide:

*/

Q_DEFINE_GALLERY_TYPE(QDocumentGallery, Folder)

/*!
    \variable QDocumentGallery::Document

    This type matches all document files in the document gallery.

    In addition to the properties derived from the \l File type documents may
    also provide:

    \list
    \o \l created
    \o \l pageCount
    \o \l wordCount
    \endlist
*/

Q_DEFINE_GALLERY_TYPE(QDocumentGallery, Document)

/*!
    \variable QDocumentGallery::Text

    This type matches all plain text files, e.g. HTML files, in the document
    gallery.

    In addition to the properties derived from the \l File type text files may
    also provide:

    \list
    \o \l wordCount
    \endlist
*/

Q_DEFINE_GALLERY_TYPE(QDocumentGallery, Text)

/*!
    \variable QDocumentGallery::Audio

    This type matches all audio files in the document gallery.

    In addition to the properties derived from the \l File type audio files may
    also provide:

    \list
    \o \l albumArtist
    \o \l albumTitle
    \o \l artist
    \o \l audioBitRate
    \o \l audioCodec
    \o \l channelCount
    \o \l composer
    \o \l discNumber
    \o \l genre
    \o \l duration
    \o \l lastPlayed
    \o \l lyrics
    \o \l performer
    \o \l playCount
    \o \l producer
    \o \l sampleRate
    \o \l trackNumber
    \endlist
*/

Q_DEFINE_GALLERY_TYPE(QDocumentGallery, Audio)

/*!
    \variable QDocumentGallery::Image

    This type matches all image files in the document gallery.

    In addition to the properties derived from the \l File type image files may
    also provide:

    \list
    \o \l cameraManufacturer
    \o \l cameraModel
    \o \l dateTaken
    \o \l exposureProgram
    \o \l exposureTime
    \o \l flashEnabled
    \o \l fNumber
    \o \l focalLength
    \o \l height
    \o \l meteringMode
    \o \l orientation
    \o \l width
    \o \l whiteBalance
    \endlist
*/

Q_DEFINE_GALLERY_TYPE(QDocumentGallery, Image)

/*!
    \variable QDocumentGallery::Video

    This type matches all video files in the document gallery.

    In addition to the properties derived from the \l File type video files may
    also provide:

    \list
    \o \l audioBitRate
    \o \l audioCodec
    \o \l channelCount
    \o \l director
    \o \l duration
    \o \l frameRate
    \o \l height
    \o \l lastPlayed
    \o \l performer
    \o \l playCount
    \o \l producer
    \o \l resumePosition
    \o \l sampleRate
    \o \l videoBitRate
    \o \l videoCodec
    \o \l width
    \endlist
*/

Q_DEFINE_GALLERY_TYPE(QDocumentGallery, Video)

/*!
    \variable QDocumentGallery::Playlist

    This type matches all playlist files in the document gallery.

    In addition to the properties derived from the \l File type playlist files
    may also provide:

    \list
    \o \l duration
    \o \l trackCount
    \endlist

*/

Q_DEFINE_GALLERY_TYPE(QDocumentGallery, Playlist)

/*!
    \variable QDocumentGallery::Artist

    This type matches all artists in the document gallery.

    Properties typically available from artists include:

    \list
    \o \l artist
    \o \l duration
    \o \l trackCount
    \endlist

    Note that only \l artist is supported on Symbian.
*/

Q_DEFINE_GALLERY_TYPE(QDocumentGallery, Artist)

/*!
    \variable QDocumentGallery::AlbumArtist

    This type matches all album artists in the document gallery.

    Properties typically available from album artists include:

    \list
    \o \l albumArtist
    \o \l artist
    \o \l duration
    \o \l trackCount
    \endlist

    Note that only \l albumArtist and \l artist are supported on Symbian.
*/

Q_DEFINE_GALLERY_TYPE(QDocumentGallery, AlbumArtist)

/*!
    \variable QDocumentGallery::Album

    This type matches all albums in the document gallery.

    Properties typically available from albums include:

    \list
    \o \l albumArtist
    \o \l albumTitle
    \o \l artist
    \o \l duration
    \o \l trackCount
    \endlist

    Note that only \l albumTitle is supported on Symbian.
*/

Q_DEFINE_GALLERY_TYPE(QDocumentGallery, Album)

/*!
    \variable QDocumentGallery::AudioGenre

    This type matches all audio genres in the document gallery.

    Properties typically available from genres include:

    \list
    \o \l duration
    \o \l genre
    \o \l trackCount
    \endlist

    Note that only \l genre is supported on Symbian.
*/

Q_DEFINE_GALLERY_TYPE(QDocumentGallery, AudioGenre)

/*!
    \variable QDocumentGallery::PhotoAlbum

    This type matches all photo albums in the document gallery.

    \list
    \o \l count
    \endlist
*/

Q_DEFINE_GALLERY_TYPE(QDocumentGallery, PhotoAlbum)

// Common

/*!
    \variable QDocumentGallery::url

    This property contains canonical url of an item in the document gallery.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, url)

/*!
    \variable QDocumentGallery::author

    This property contains the name of the author of an item in the document
    gallery.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, author)

/*!
    \variable QDocumentGallery::copyright

    This property contains a copyright statement for an item in the document
    gallery.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, copyright)

/*!
    \variable QDocumentGallery::description

    This property contains a description of an item in the document gallery.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, description)

/*!
    \variable QDocumentGallery::comments

    This property contains a user comment about an item in the document gallery.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, comments)

/*!
    \variable QDocumentGallery::rating

    This property contains a rating for an item in the document gallery.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, rating)

/*!
    \variable QDocumentGallery::title

    This property contains the title of an item in the document gallery.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, title)

/*!
    \variable QDocumentGallery::subject

    This property contains the subject of item in the document gallery.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, subject)

/*!
    \variable QDocumentGallery::keywords

    This property contains keywords relevant to an item in the document gallery.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, keywords)

/*!
    \variable QDocumentGallery::language

    This property contains the language of the content of an item in the
    document gallery.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, language)

// File

/*!
    \variable QDocumentGallery::path

    This property contains the absolute path excluding the file name of a file
    in the document gallery.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, path)

/*!
    \variable QDocumentGallery::filePath

    This property contains the absolute path including the file name of a file
    in the document gallery.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, filePath)

/*!
    \variable QDocumentGallery::fileName

    This property contains the file name excluding the path of a file in
    the document gallery.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, fileName)

/*!
    \variable QDocumentGallery::fileExtension

    This property contains the file extension of a file.

    Note that this property is not supported on Symbian and Maemo 5.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, fileExtension)

/*!
    \variable QDocumentGallery::fileSize

    This property contains the size in bytes of a file.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, fileSize)

/*!
    \variable QDocumentGallery::mimeType

    This property contains the MIME type of a file.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, mimeType)

/*!
    \variable QDocumentGallery::lastModified

    This property contains the date and time a file was last modified.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, lastModified)

/*!
    \variable QDocumentGallery::lastAccessed

    This property contains the date and time a file was last accessed.

    Note that this property is not supported on Symbian.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, lastAccessed)

// Document

/*!
    \variable QDocumentGallery::pageCount

    This property contains the number of pages in a document.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, pageCount)

/*!
    \variable QDocumentGallery::wordCount

    This property contains the number of words in document.

    This property for \l Text is not supported on Maemo 5.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, wordCount)

/*!
    \variable QDocumentGallery::created

    This property contains the date and time a document was created.

    This property is not supported on Maemo 5.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, created)

// Media

/*!
    \variable QDocumentGallery::duration

    This property contains the duration of an audio or video file.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, duration)

/*!
    \variable QDocumentGallery::producer

    This property contains the name of the producer of a media file.

    Note that this property for \l Audio is not supported on Maemo,
    for \l Video is not supported on Maemo 5.

    This property is not supported on Symbian for neither \l Audio
    nor \l Video.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, producer)

/*!
    \variable QDocumentGallery::lastPlayed

    This property contains the date and time an audio or video file was last
    played.

    Note that this property is not supported on Symbian.

    This property for \l Video is not supported on Maemo 5.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, lastPlayed)

/*!
    \variable QDocumentGallery::playCount

    This property contains the number of times an audio or video file has been
    played.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, playCount)

/*!
    \variable QDocumentGallery::performer

    This property contains the names of performers in a media file.

    Note that this property for \l Video is not supported on Maemo 5.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, performer)

// Audio

/*!
    \variable QDocumentGallery::audioCodec

    This property contains the name of the codec used to encode audio in a media
    file.

    Note that this property for Video is not supported on Symbian and Maemo 5.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, audioCodec)

/*!
    \variable QDocumentGallery::audioBitRate

    This property contains the bit rate of the audio in a media file.

    Note that this property for Video is not supported on Symbian and Maemo 5.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, audioBitRate)

/*!
    \variable QDocumentGallery::sampleRate

    This property contains the sample rate of the audio in a media file.

    Note that this property for Video is not supported on Symbian and Maemo 5.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, sampleRate)
/*!
    \variable QDocumentGallery::channelCount

    This property contains the number of audio channels in a media file.

    Note that this property is not supported on Symbian.

    This property for \l Video is not supported on Maemo 5.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, channelCount)

// Music

/*!
    \variable QDocumentGallery::artist

    This property contains the names of artists contributing to a music track.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, artist)

/*!
    \variable QDocumentGallery::albumArtist

    This property contains the name of the title artist of a music album.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, albumArtist)

/*!
    \variable QDocumentGallery::albumTitle

    This property contains the title of the album a music track belongs to.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, albumTitle)

/*!
    \variable QDocumentGallery::composer

    This property contains the name of the composer of a music track.

    This property is not supported on Maemo 5.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, composer)

/*!
    \variable QDocumentGallery::genre

    This property contains the genre of a media file.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, genre)

/*!
    \variable QDocumentGallery::lyrics

    This property contains the lyrics to a music track.

    Note that this property is not supported on Symbian.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, lyrics)

/*!
    \variable QDocumentGallery::trackCount

    This property contains the number of music tracks in an album.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, trackCount)

/*!
    \variable QDocumentGallery::trackNumber

    This property contains the track number of a music track in an album.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, trackNumber)

/*!
    \variable QDocumentGallery::discNumber

    This property contains the disc number of an album in a set.

    Note that this property is not supported on Symbian.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, discNumber)

// Image, Video common.

/*!
    \variable QDocumentGallery::width

    This property contains the width in pixels of an image or video file.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, width)

/*!
    \variable QDocumentGallery::height

    This property contains the height in pixels of an image or video file.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, height)

// Image
/*!
    \variable QDocumentGallery::orientation

    This property contains the orientation of an image.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, orientation)

// Photo

/*!
    \variable QDocumentGallery::dateTaken

    This property contains the date and time a photo was taken.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, dateTaken)

/*!
    \variable QDocumentGallery::cameraManufacturer

    This property contains the manufacturer name of the camera used to take a
    photo.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, cameraManufacturer)

/*!
    \variable QDocumentGallery::cameraModel

    This property contains the model name of the camera used to take a photo.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, cameraModel)

/*!
    \variable QDocumentGallery::exposureProgram

    This property contains the name of the exposure program used when taking a
    photo.

    Note that this property is not supported on Maemo.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, exposureProgram)

/*!
    \variable QDocumentGallery::exposureTime

    This property contains the exposure time of a photo in seconds.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, exposureTime)

/*!
    \variable QDocumentGallery::fNumber

    This property contains the F-number of a photo.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, fNumber)

/*!
    \variable QDocumentGallery::flashEnabled

    This property contains whether a flash was used when taking a photo.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, flashEnabled)

/*!
    \variable QDocumentGallery::focalLength

    This property contains the focal length used when taking a photo.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, focalLength)

/*!
    \variable QDocumentGallery::meteringMode

    This property contains the metering mode used when taking a photo.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, meteringMode)

/*!
    \variable QDocumentGallery::whiteBalance

    This property contains the white balance setting used when taking a photo.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, whiteBalance)

// Video

/*!
    \variable QDocumentGallery::frameRate

    This property contains the frame rate of a video.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, frameRate)

/*!
    \variable QDocumentGallery::videoCodec

    This property contains the codec used to encode the video in a media file.

    Note that this property is not supported on Symbian.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, videoCodec)

/*!
    \variable QDocumentGallery::videoBitRate

    This property contains the bit rate of the video in a media file.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, videoBitRate)

/*!
    \variable QDocumentGallery::resumePosition

    This property contains the position in a video where playback was
    interrupted.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, resumePosition)

/*!
    \variable QDocumentGallery::director

    This property contains the name of the director of a video.

    Note that this property is not supported on Symbian and Maemo 5.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, director)

// Type

/*!
    \variable QDocumentGallery::count

    This property contains the number of items within the scope of a parent
    item or an item type.
*/

Q_DEFINE_GALLERY_PROPERTY(QDocumentGallery, count)

/*!
    \fn QDocumentGallery::QDocumentGallery(QObject *parent)

    Constructs a new document gallery.

    The \a parent is passed to QAbstractGallery.
*/

/*!
    \fn QDocumentGallery::~QDocumentGallery()

    Destroys a document gallery.
*/

/*!
    \fn QDocumentGallery::isRequestSupported(QGalleryAbstractRequest::RequestType type) const

    \reimp
*/

/*!
    \fn QDocumentGallery::itemTypePropertyNames(const QString &itemType) const;

    Returns a list of names of properties that are valid for an \a itemType.
*/

/*!
    \fn QDocumentGallery::propertyAttributes(const QString &propertyName, const QString &itemType) const

    Returns the attributes of a property of \a itemType identified by
    \a propertyName
*/

/*!
    \fn QDocumentGallery::createResponse(QGalleryAbstractRequest *request)

    \reimp
*/


#ifdef QT_DOCUMENT_GALLERY_NULL

QDocumentGallery::QDocumentGallery(QObject *parent)
    : QAbstractGallery(parent)
{

}


QDocumentGallery::~QDocumentGallery()
{

}

bool QDocumentGallery::isRequestSupported(QGalleryAbstractRequest::RequestType) const
{
    return false;
}

QStringList QDocumentGallery::itemTypePropertyNames(const QString &) const
{
    return QStringList();
}

QGalleryProperty::Attributes QDocumentGallery::propertyAttributes(const QString &, const QString &) const
{
    return QGalleryProperty::Attributes();
}

QGalleryAbstractResponse *QDocumentGallery::createResponse(QGalleryAbstractRequest *)
{
    return 0;
}

#include "moc_qdocumentgallery.cpp"

#endif

QTM_END_NAMESPACE

