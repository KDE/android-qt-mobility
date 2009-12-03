/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <qmediaencodersettings.h>

QTM_BEGIN_NAMESPACE

class QAudioEncoderSettingsPrivate  : public QSharedData
{
public:
    QAudioEncoderSettingsPrivate() :
        isNull(true),
        encodingMode(QtMedia::ConstantQualityEncoding),
        bitrate(-1),
        sampleRate(-1),
        channels(-1),
        quality(QtMedia::NormalQuality)
    {
    }

    QAudioEncoderSettingsPrivate(const QAudioEncoderSettingsPrivate &other):
        QSharedData(other),
        isNull(other.isNull),
        encodingMode(other.encodingMode),
        codec(other.codec),
        bitrate(other.bitrate),
        sampleRate(other.sampleRate),
        channels(other.channels),
        quality(other.quality)
    {
    }

    bool isNull;
    QtMedia::EncodingMode encodingMode;
    QString codec;
    int bitrate;
    int sampleRate;
    int channels;
    QtMedia::EncodingQuality quality;

private:
    QAudioEncoderSettingsPrivate& operator=(const QAudioEncoderSettingsPrivate &other);
};

/*!
    \class QAudioEncoderSettings
    \preliminary
    \brief The QAudioEncoderSettings class provides a set of audio encoder settings.

    \ingroup multimedia

    A audio encoder settings object is used to specify the audio encoder
    settings used by QMediaRecorder.  Audio encoder settings are selected
    by constructing a QAudioEncoderSettings object, setting the desired
    properties and then passing it to a QMediaRecorder instance using
    the QMediaRecorder::setEncodingSettings() function.

    \code
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("audio/mpeg");
    audioSettings.setChannelCount(2);

    recorder->setEncodingSettings(audioSettings);
    \endcode

    \sa QMediaRecorder, QAudioEncoderControl
*/

/*!
    Construct a null audio encoder settings object.
*/
QAudioEncoderSettings::QAudioEncoderSettings()
    :d(new QAudioEncoderSettingsPrivate)
{
}

/*!
    Constructs a copy of the audio encoder settings object \a other.
*/

QAudioEncoderSettings::QAudioEncoderSettings(const QAudioEncoderSettings& other)
    :d(other.d)
{
}

/*!
    Destroys an audio encoder settings object.
*/

QAudioEncoderSettings::~QAudioEncoderSettings()
{
}

/*!
    Assigns the value of \a other to an audio encoder settings object.
*/

QAudioEncoderSettings& QAudioEncoderSettings::operator=(const QAudioEncoderSettings &other)
{
    d = other.d;
    return *this;
}

/*!
    Determines if \a other is of equal value to an audio encoder settings object.

    Returns true if the settings objects are of equal value, and true if they are not of equal
    value.
*/

bool QAudioEncoderSettings::operator==(const QAudioEncoderSettings &other) const
{
    return (d == other.d) ||
           (d->isNull == other.d->isNull &&
            d->encodingMode == other.d->encodingMode &&
            d->bitrate == other.d->bitrate &&
            d->sampleRate == other.d->sampleRate &&
            d->channels == other.d->channels &&
            d->quality == other.d->quality &&
            d->codec == other.d->codec);
}

/*!
    Determines if \a other is of equal value to an audio encoder settings object.

    Returns true if the settings objects are not of equal value, and true if they are of equal
    value.
*/

bool QAudioEncoderSettings::operator!=(const QAudioEncoderSettings &other) const
{
    return !(*this == other);
}

/*!
    Identifies if an audio settings object is initialized.

    Returns true if the settings object is null, and false if it is not.
*/

bool QAudioEncoderSettings::isNull() const
{
    return d->isNull;
}

/*!
    Returns the audio encoding mode.

    \sa QtMedia::EncodingMode
*/
QtMedia::EncodingMode QAudioEncoderSettings::encodingMode() const
{
    return d->encodingMode;
}

/*!
    Sets the audio encoding \a mode setting.

    \sa encodingMode(), QtMedia::EncodingMode
*/
void QAudioEncoderSettings::setEncodingMode(QtMedia::EncodingMode mode)
{
    d->encodingMode = mode;
}

/*!
    Returns the audio codec.
*/
QString QAudioEncoderSettings::codec() const
{
    return d->codec;
}

/*!
    Sets the audio \a codec.
*/
void QAudioEncoderSettings::setCodec(const QString& codec)
{
    d->isNull = false;
    d->codec = codec;
}

/*!
    Returns the bit rate of the compressed audio stream in bits per second.
*/
int QAudioEncoderSettings::bitRate() const
{
    return d->bitrate;
}

/*!
    Returns the number of audio channels.
*/
int QAudioEncoderSettings::channelCount() const
{
    return d->channels;
}

/*!
    Sets the number of audio \a channels.

    A value of -1 indicates the encoder should make an optimal choice based on what is available
    from the audio source and the limitations of the codec.
*/
void QAudioEncoderSettings::setChannelCount(int channels)
{
    d->isNull = false;
    d->channels = channels;
}

/*!
    Sets the audio bit \a rate.
*/
void QAudioEncoderSettings::setBitRate(int rate)
{
    d->isNull = false;
    d->bitrate = rate;
}

/*!
    Returns the audio sample rate.
*/
int QAudioEncoderSettings::sampleRate() const
{
    return d->sampleRate;
}

/*!
    Sets the audio sample \a rate.

    A value of -1 indicates the encoder should make an optimal choice based on what is avaialbe
    from the audio source and the limitations of the codec.
  */
void QAudioEncoderSettings::setSampleRate(int rate)
{
    d->isNull = false;
    d->sampleRate = rate;
}

/*!
    Returns the audio encoding quality.
*/

QtMedia::EncodingQuality QAudioEncoderSettings::quality() const
{
    return d->quality;
}

/*!
    Set the audio encoding \a quality.
*/
void QAudioEncoderSettings::setQuality(QtMedia::EncodingQuality quality)
{
    d->isNull = false;
    d->quality = quality;
}

class QVideoEncoderSettingsPrivate  : public QSharedData
{
public:
    QVideoEncoderSettingsPrivate() :
        isNull(true),
        encodingMode(QtMedia::ConstantQualityEncoding),
        bitrate(-1),
        frameRate(0),
        quality(QtMedia::NormalQuality)
    {
    }

    QVideoEncoderSettingsPrivate(const QVideoEncoderSettingsPrivate &other):
        QSharedData(other),
        isNull(other.isNull),
        encodingMode(other.encodingMode),
        codec(other.codec),
        bitrate(other.bitrate),
        resolution(other.resolution),
        frameRate(other.frameRate),
        quality(other.quality)
    {
    }

    bool isNull;
    QtMedia::EncodingMode encodingMode;
    QString codec;
    int bitrate;
    QSize resolution;
    qreal frameRate;
    QtMedia::EncodingQuality quality;

private:
    QVideoEncoderSettingsPrivate& operator=(const QVideoEncoderSettingsPrivate &other);
};

/*!
    \class QVideoEncoderSettings
    \preliminary
    \brief The QVideoEncoderSettings class provides a set of video encoder settings.

    A video encoder settings object is used to specify the video encoder settings used by
    QMediaRecorder.  Video encoder settings are selected by constructing a QVideoEncoderSettings
    object, setting the desired properties and then passing it to a QMediaRecorder instance using
    the QMediaRecorder::setEncodingSettings() function.

    \code
    QVideoEncoderSettings videoSettings;
    videoSettings.setCodec("video/mpeg2");
    videoSettings.setResolution(640, 480);

    recorder->setEncodingSettings(audioSettings, videoSettings);
    \endcode

    \sa QMediaRecorder, QVideoEncoderControl
*/

/*!
    Constructs a null video encoder settings object.
*/

QVideoEncoderSettings::QVideoEncoderSettings()
    :d(new QVideoEncoderSettingsPrivate)
{
}

/*!
    Constructs a copy of the video encoder settings object \a other.
*/

QVideoEncoderSettings::QVideoEncoderSettings(const QVideoEncoderSettings& other)
    :d(other.d)
{
}

/*!
    Destroys a video encoder settings object.
*/

QVideoEncoderSettings::~QVideoEncoderSettings()
{
}

/*!
    Assigns the value of \a other to a video encoder settings object.
*/
QVideoEncoderSettings &QVideoEncoderSettings::operator=(const QVideoEncoderSettings &other)
{
    d = other.d;
    return *this;
}

/*!
    Determines if \a other is of equal value to a video encoder settings object.

    Returns true if the settings objects are of equal value, and true if they are not of equal
    value.
*/
bool QVideoEncoderSettings::operator==(const QVideoEncoderSettings &other) const
{
    return (d == other.d) ||
           (d->isNull == other.d->isNull &&
            d->encodingMode == other.d->encodingMode &&
            d->bitrate == other.d->bitrate &&
            d->quality == other.d->quality &&
            d->codec == other.d->codec &&
            d->resolution == other.d->resolution &&
            qFuzzyCompare(d->frameRate, other.d->frameRate));
}

/*!
    Determines if \a other is of equal value to a video encoder settings object.

    Returns true if the settings objects are not of equal value, and true if they are of equal
    value.
*/
bool QVideoEncoderSettings::operator!=(const QVideoEncoderSettings &other) const
{
    return !(*this == other);
}

/*!
    Identifies if a video encoder settings object is uninitalized.

    Returns true if the settings are null, and false if they are not.
*/
bool QVideoEncoderSettings::isNull() const
{
    return d->isNull;
}

/*!
    Returns the video encoding mode.

    \sa QtMedia::EncodingMode
*/
QtMedia::EncodingMode QVideoEncoderSettings::encodingMode() const
{
    return d->encodingMode;
}

/*!
    Sets the video encoding \a mode.

    \sa QtMedia::EncodingMode
*/
void QVideoEncoderSettings::setEncodingMode(QtMedia::EncodingMode mode)
{
    d->isNull = false;
    d->encodingMode = mode;
}

/*!
    Returns the video codec.
*/

QString QVideoEncoderSettings::codec() const
{
    return d->codec;
}

/*!
    Sets the video \a codec.
*/
void QVideoEncoderSettings::setCodec(const QString& codec)
{
    d->isNull = false;
    d->codec = codec;
}

/*!
    Returns bit rate of the encoded video stream.
*/
int QVideoEncoderSettings::bitRate() const
{
    return d->bitrate;
}

/*!
    Sets the bit rate of the encoded video stream to \a value.
*/

void QVideoEncoderSettings::setBitRate(int value)
{
    d->isNull = false;
    d->bitrate = value;
}

/*!
    Returns the video frame rate.
*/
qreal QVideoEncoderSettings::frameRate() const
{
    return d->frameRate;
}

/*!
    \fn QVideoEncoderSettings::setFrameRate(qreal rate)

    Sets the video frame \a rate.

    A value of 0 indicates the encoder should make an optimal choice based on what is available
    from the video source and the limitations of the codec.
*/

void QVideoEncoderSettings::setFrameRate(qreal rate)
{
    d->isNull = false;
    d->frameRate = rate;
}

/*!
    Returns the resolution of the encoded video.
*/

QSize QVideoEncoderSettings::resolution() const
{
    return d->resolution;
}

/*!
    Sets the \a resolution of the encoded video.

    An empty QSize indicates the encoder should make an optimal choice based on what is available
    from the video source and the limitations of the codec.
*/

void QVideoEncoderSettings::setResolution(const QSize &resolution)
{
    d->isNull = false;
    d->resolution = resolution;
}

/*!
    Sets the \a width and \a height of the resolution of the encoded video.

    \overload
*/

void QVideoEncoderSettings::setResolution(int width, int height)
{
    d->isNull = false;
    d->resolution = QSize(width, height);
}

/*!
    Returns the video encoding quality.
*/

QtMedia::EncodingQuality QVideoEncoderSettings::quality() const
{
    return d->quality;
}

/*!
    Sets the video encoding \a quality.
*/

void QVideoEncoderSettings::setQuality(QtMedia::EncodingQuality quality)
{
    d->isNull = false;
    d->quality = quality;
}



class QImageEncoderSettingsPrivate  : public QSharedData
{
public:
    QImageEncoderSettingsPrivate() :
        isNull(true),
        quality(QtMedia::NormalQuality)
    {
    }

    QImageEncoderSettingsPrivate(const QImageEncoderSettingsPrivate &other):
        QSharedData(other),
        isNull(other.isNull),
        codec(other.codec),
        resolution(other.resolution),
        quality(other.quality)
    {
    }

    bool isNull;
    QString codec;
    QSize resolution;
    QtMedia::EncodingQuality quality;

private:
    QImageEncoderSettingsPrivate& operator=(const QImageEncoderSettingsPrivate &other);
};

/*!
    \class QImageEncoderSettings
    \preliminary

    \brief The QImageEncoderSettings class provides a set of image
    encoder settings.

    A image encoder settings object is used to specify the image
    encoder settings used by QStillImageCapture.  Image encoder
    settings are selected by constructing a QImageEncoderSettings
    object, setting the desired properties and then passing it to a
    QStillImageCapture instance using the
    QStillImageCapture::setImageSettings() function.

    \code
    QImageEncoderSettings imageSettings;
    imageSettings.setCodec("image/jpeg");
    imageSettings.setResolution(1600, 1200);

    imageCapture->setImageSettings(imageSettings);
    \endcode

    \sa QStillImageCapture, QImageEncoderControl
*/

/*!
    Constructs a null image encoder settings object.
*/

QImageEncoderSettings::QImageEncoderSettings()
    :d(new QImageEncoderSettingsPrivate)
{
}

/*!
    Constructs a copy of the image encoder settings object \a other.
*/

QImageEncoderSettings::QImageEncoderSettings(const QImageEncoderSettings& other)
    :d(other.d)
{
}

/*!
    Destroys a image encoder settings object.
*/

QImageEncoderSettings::~QImageEncoderSettings()
{
}

/*!
    Assigns the value of \a other to a image encoder settings object.
*/
QImageEncoderSettings &QImageEncoderSettings::operator=(const QImageEncoderSettings &other)
{
    d = other.d;
    return *this;
}

/*!
    Determines if \a other is of equal value to a image encoder settings object.

    Returns true if the settings objects are of equal value, and true if they are not of equal
    value.
*/
bool QImageEncoderSettings::operator==(const QImageEncoderSettings &other) const
{
    return (d == other.d) ||
           (d->isNull == other.d->isNull &&
            d->quality == other.d->quality &&
            d->codec == other.d->codec &&
            d->resolution == other.d->resolution);

}

/*!
    Determines if \a other is of equal value to a image encoder settings object.

    Returns true if the settings objects are not of equal value, and true if they are of equal
    value.
*/
bool QImageEncoderSettings::operator!=(const QImageEncoderSettings &other) const
{
    return !(*this == other);
}

/*!
    Identifies if a image encoder settings object is uninitalized.

    Returns true if the settings are null, and false if they are not.
*/
bool QImageEncoderSettings::isNull() const
{
    return d->isNull;
}

/*!
    Returns the image codec.
*/

QString QImageEncoderSettings::codec() const
{
    return d->codec;
}

/*!
    Sets the image \a codec.
*/
void QImageEncoderSettings::setCodec(const QString& codec)
{
    d->isNull = false;
    d->codec = codec;
}

/*!
    Returns the resolution of the encoded image.
*/

QSize QImageEncoderSettings::resolution() const
{
    return d->resolution;
}

/*!
    Sets the \a resolution of the encoded image.

    An empty QSize indicates the encoder should make an optimal choice based on what is available
    from the image source and the limitations of the codec.
*/

void QImageEncoderSettings::setResolution(const QSize &resolution)
{
    d->isNull = false;
    d->resolution = resolution;
}

/*!
    Sets the \a width and \a height of the resolution of the encoded image.

    \overload
*/

void QImageEncoderSettings::setResolution(int width, int height)
{
    d->isNull = false;
    d->resolution = QSize(width, height);
}

/*!
    Returns the image encoding quality.
*/

QtMedia::EncodingQuality QImageEncoderSettings::quality() const
{
    return d->quality;
}

/*!
    Sets the image encoding \a quality.
*/

void QImageEncoderSettings::setQuality(QtMedia::EncodingQuality quality)
{
    d->isNull = false;
    d->quality = quality;
}
QTM_END_NAMESPACE

