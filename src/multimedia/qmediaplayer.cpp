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

#include <QtCore/qcoreevent.h>
#include <QtCore/qmetaobject.h>
#include <QtCore/qtimer.h>
#include <QtCore/qdebug.h>
#include <QtCore/qpointer.h>


#include "qmediaplayer.h"

#include <qmediaobject_p.h>
#include <qmediaservice.h>
#include <qmediaplayercontrol.h>
#include <qmediaserviceprovider.h>
#include <qmediaplaylist.h>
#include <qmediaplaylistcontrol.h>
#include <qmediaplaylistsourcecontrol.h>
#include <qvideowidget.h>
#include <qvideosurfaceoutput_p.h>
#include <qgraphicsvideoitem.h>
#include <qmedianetworkaccesscontrol.h>

QT_BEGIN_NAMESPACE

/*!
    \class QMediaPlayer
    \brief The QMediaPlayer class allows the playing of a media source.
    \inmodule QtMultimediaKit
    \ingroup multimedia
    \since 1.0



    The QMediaPlayer class is a high level media playback class. It can be used
    to playback such content as songs, movies and internet radio. The content
    to playback is specified as a QMediaContent, which can be thought of as a
    main or canonical URL with addition information attached. When provided
    with a QMediaContent playback may be able to commence.

    \snippet doc/src/snippets/multimedia-snippets/media.cpp Player

    QVideoWidget can be used with QMediaPlayer for video rendering and QMediaPlaylist
    for accessing playlist functionality.

    \snippet doc/src/snippets/multimedia-snippets/media.cpp Movie playlist

    \sa QMediaObject, QMediaService, QVideoWidget, QMediaPlaylist
*/

namespace
{
class MediaPlayerRegisterMetaTypes
{
public:
    MediaPlayerRegisterMetaTypes()
    {
        qRegisterMetaType<QMediaPlayer::State>("QMediaPlayer::State");
        qRegisterMetaType<QMediaPlayer::MediaStatus>("QMediaPlayer::MediaStatus");
        qRegisterMetaType<QMediaPlayer::Error>("QMediaPlayer::Error");
    }
} _registerPlayerMetaTypes;
}

class QMediaPlayerPrivate : public QMediaObjectPrivate
{
    Q_DECLARE_NON_CONST_PUBLIC(QMediaPlayer)

public:
    QMediaPlayerPrivate()
        : provider(0)
        , control(0)
        , playlistSourceControl(0)
        , state(QMediaPlayer::StoppedState)
        , error(QMediaPlayer::NoError)
        , filterStates(false)
        , playlist(0)
    {}

    QMediaServiceProvider *provider;
    QMediaPlayerControl* control;
    QMediaPlaylistSourceControl* playlistSourceControl;
    QMediaPlayer::State state;
    QMediaPlayer::Error error;
    QString errorString;
    bool filterStates;

    QPointer<QObject> videoOutput;
    QMediaPlaylist *playlist;
    QMediaNetworkAccessControl *networkAccessControl;
    QVideoSurfaceOutput surfaceOutput;

    void _q_stateChanged(QMediaPlayer::State state);
    void _q_mediaStatusChanged(QMediaPlayer::MediaStatus status);
    void _q_error(int error, const QString &errorString);
    void _q_updateMedia(const QMediaContent&);
    void _q_playlistDestroyed();
};

void QMediaPlayerPrivate::_q_stateChanged(QMediaPlayer::State ps)
{
    Q_Q(QMediaPlayer);

    if (filterStates)
        return;

    if (playlist
            && ps != state && ps == QMediaPlayer::StoppedState
            && (control->mediaStatus() == QMediaPlayer::EndOfMedia ||
                control->mediaStatus() == QMediaPlayer::InvalidMedia)) {
        playlist->next();
        ps = control->state();
    }

    if (ps != state) {
        state = ps;

        if (ps == QMediaPlayer::PlayingState)
            q->addPropertyWatch("position");
        else
            q->removePropertyWatch("position");

        emit q->stateChanged(ps);
    }
}

void QMediaPlayerPrivate::_q_mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    Q_Q(QMediaPlayer);

    switch (status) {
    case QMediaPlayer::StalledMedia:
    case QMediaPlayer::BufferingMedia:
        q->addPropertyWatch("bufferStatus");
        emit q->mediaStatusChanged(status);
        break;
    default:
        q->removePropertyWatch("bufferStatus");
        emit q->mediaStatusChanged(status);
        break;
    }

}

void QMediaPlayerPrivate::_q_error(int error, const QString &errorString)
{
    Q_Q(QMediaPlayer);

    this->error = QMediaPlayer::Error(error);
    this->errorString = errorString;

    emit q->error(this->error);
}

void QMediaPlayerPrivate::_q_updateMedia(const QMediaContent &media)
{
    Q_Q(QMediaPlayer);

    if (!control)
        return;

    const QMediaPlayer::State currentState = state;

    filterStates = true;
    control->setMedia(media, 0);

    if (!media.isNull()) {
        switch (currentState) {
        case QMediaPlayer::PlayingState:
            control->play();
            break;
        case QMediaPlayer::PausedState:
            control->pause();
            break;
        default:
            break;
        }
    }
    filterStates = false;

    state = control->state();

    if (state != currentState) {
        if (state == QMediaPlayer::PlayingState)
            q->addPropertyWatch("position");
        else
            q->removePropertyWatch("position");

        emit q->stateChanged(state);
    }
}

void QMediaPlayerPrivate::_q_playlistDestroyed()
{
    playlist = 0;

    if (!control)
        return;

    if (playlistSourceControl)
        playlistSourceControl->setPlaylist(0);

    control->setMedia(QMediaContent(), 0);
}

static QMediaService *playerService(QMediaPlayer::Flags flags, QMediaServiceProvider *provider)
{
    if (flags) {
        QMediaServiceProviderHint::Features features = 0;
        if (flags & QMediaPlayer::LowLatency)
            features |= QMediaServiceProviderHint::LowLatencyPlayback;

        if (flags & QMediaPlayer::StreamPlayback)
            features |= QMediaServiceProviderHint::StreamPlayback;

        if (flags & QMediaPlayer::VideoSurface)
            features |= QMediaServiceProviderHint::VideoSurface;

        return provider->requestService(Q_MEDIASERVICE_MEDIAPLAYER,
                                        QMediaServiceProviderHint(features));
    } else
        return provider->requestService(Q_MEDIASERVICE_MEDIAPLAYER);
}


/*!
    Construct a QMediaPlayer that uses the playback service from \a provider,
    parented to \a parent and with \a flags.

    If a playback service is not specified the system default will be used.
*/

QMediaPlayer::QMediaPlayer(QObject *parent, QMediaPlayer::Flags flags, QMediaServiceProvider *provider):
    QMediaObject(*new QMediaPlayerPrivate,
                 parent,
                 playerService(flags,provider))
{
    Q_D(QMediaPlayer);

    d->provider = provider;

    if (d->service == 0) {
        d->error = ServiceMissingError;
    } else {
        d->control = qobject_cast<QMediaPlayerControl*>(d->service->requestControl(QMediaPlayerControl_iid));
        d->playlistSourceControl = qobject_cast<QMediaPlaylistSourceControl*>(d->service->requestControl(QMediaPlaylistSourceControl_iid));
        d->networkAccessControl = qobject_cast<QMediaNetworkAccessControl*>(d->service->requestControl(QMediaNetworkAccessControl_iid));
        if (d->control != 0) {
            connect(d->control, SIGNAL(mediaChanged(QMediaContent)), SIGNAL(mediaChanged(QMediaContent)));
            connect(d->control, SIGNAL(stateChanged(QMediaPlayer::State)), SLOT(_q_stateChanged(QMediaPlayer::State)));
            connect(d->control, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
                    SLOT(_q_mediaStatusChanged(QMediaPlayer::MediaStatus)));
            connect(d->control, SIGNAL(error(int,QString)), SLOT(_q_error(int,QString)));

            connect(d->control, SIGNAL(durationChanged(qint64)), SIGNAL(durationChanged(qint64)));
            connect(d->control, SIGNAL(positionChanged(qint64)), SIGNAL(positionChanged(qint64)));
            connect(d->control, SIGNAL(audioAvailableChanged(bool)), SIGNAL(audioAvailableChanged(bool)));
            connect(d->control, SIGNAL(videoAvailableChanged(bool)), SIGNAL(videoAvailableChanged(bool)));
            connect(d->control, SIGNAL(volumeChanged(int)), SIGNAL(volumeChanged(int)));
            connect(d->control, SIGNAL(mutedChanged(bool)), SIGNAL(mutedChanged(bool)));
            connect(d->control, SIGNAL(seekableChanged(bool)), SIGNAL(seekableChanged(bool)));
            connect(d->control, SIGNAL(playbackRateChanged(qreal)), SIGNAL(playbackRateChanged(qreal)));
            connect(d->control, SIGNAL(bufferStatusChanged(int)), SIGNAL(bufferStatusChanged(int)));

            if (d->control->state() == PlayingState)
                addPropertyWatch("position");

            if (d->control->mediaStatus() == StalledMedia || d->control->mediaStatus() == BufferingMedia)
                addPropertyWatch("bufferStatus");
        }
        if (d->networkAccessControl != 0) {
            connect(d->networkAccessControl, SIGNAL(configurationChanged(QNetworkConfiguration)),
            this, SIGNAL(networkConfigurationChanged(QNetworkConfiguration)));
        }
    }
}


/*!
    Destroys the player object.
*/

QMediaPlayer::~QMediaPlayer()
{
    Q_D(QMediaPlayer);

    if (d->service) {
        if (d->control)
            d->service->releaseControl(d->control);
    }

    d->provider->releaseService(d->service);
}

QMediaContent QMediaPlayer::media() const
{
    Q_D(const QMediaPlayer);

    if (d->control != 0)
        return d->control->media();

    return QMediaContent();
}

/*!
    Returns the stream source of media data.

    This is only valid if a stream was passed to setMedia().

    \sa setMedia()
*/

const QIODevice *QMediaPlayer::mediaStream() const
{
    Q_D(const QMediaPlayer);

    if (d->control != 0)
        return d->control->mediaStream();

    return 0;
}

QMediaPlaylist *QMediaPlayer::playlist() const
{
    return d_func()->playlistSourceControl ?
            d_func()->playlistSourceControl->playlist() :
            d_func()->playlist;
}

void QMediaPlayer::setPlaylist(QMediaPlaylist *playlist)
{
    Q_D(QMediaPlayer);

    if (d->playlistSourceControl) {
        if (d->playlistSourceControl->playlist())
            disconnect(d->playlist, SIGNAL(destroyed()), this, SLOT(_q_playlistDestroyed()));

        d->playlistSourceControl->setPlaylist(playlist);

        if (playlist)
            connect(d->playlist, SIGNAL(destroyed()), this, SLOT(_q_playlistDestroyed()));
    } else {
        if (d->playlist) {
            disconnect(d->playlist, SIGNAL(currentMediaChanged(QMediaContent)),
                    this, SLOT(_q_updateMedia(QMediaContent)));
            disconnect(d->playlist, SIGNAL(destroyed()), this, SLOT(_q_playlistDestroyed()));
        }

        d->playlist = playlist;

        if (d->playlist) {
            connect(d->playlist, SIGNAL(currentMediaChanged(QMediaContent)),
                    this, SLOT(_q_updateMedia(QMediaContent)));
            connect(d->playlist, SIGNAL(destroyed()), this, SLOT(_q_playlistDestroyed()));

            if (d->control != 0)
                d->control->setMedia(playlist->currentMedia(), 0);
        } else {
            setMedia(QMediaContent(), 0);
        }

    }
}

/*!
    Sets the network access points for remote media playback.
    \a configurations contains, in ascending preferential order, a list of
    configuration  that can be used for network access.

    This will invalidate the choice of previous configurations.
*/
void QMediaPlayer::setNetworkConfigurations(const QList<QNetworkConfiguration> &configurations)
{
    Q_D(QMediaPlayer);

    if (d->networkAccessControl)
        d->networkAccessControl->setConfigurations(configurations);
}

QMediaPlayer::State QMediaPlayer::state() const
{
    return d_func()->state;
}

QMediaPlayer::MediaStatus QMediaPlayer::mediaStatus() const
{
    Q_D(const QMediaPlayer);

    if (d->control != 0)
        return d->control->mediaStatus();

    return QMediaPlayer::UnknownMediaStatus;
}

qint64 QMediaPlayer::duration() const
{
    Q_D(const QMediaPlayer);

    if (d->control != 0)
        return d->control->duration();

    return -1;
}

qint64 QMediaPlayer::position() const
{
    Q_D(const QMediaPlayer);

    if (d->control != 0)
        return d->control->position();

    return 0;
}

int QMediaPlayer::volume() const
{
    Q_D(const QMediaPlayer);

    if (d->control != 0)
        return d->control->volume();

    return 0;
}

bool QMediaPlayer::isMuted() const
{
    Q_D(const QMediaPlayer);

    if (d->control != 0)
        return d->control->isMuted();

    return false;
}

int QMediaPlayer::bufferStatus() const
{
    Q_D(const QMediaPlayer);

    if (d->control != 0)
        return d->control->bufferStatus();

    return 0;
}

bool QMediaPlayer::isAudioAvailable() const
{
    Q_D(const QMediaPlayer);

    if (d->control != 0)
        return d->control->isAudioAvailable();

    return false;
}

bool QMediaPlayer::isVideoAvailable() const
{
    Q_D(const QMediaPlayer);

    if (d->control != 0)
        return d->control->isVideoAvailable();

    return false;
}

bool QMediaPlayer::isSeekable() const
{
    Q_D(const QMediaPlayer);

    if (d->control != 0)
        return d->control->isSeekable();

    return false;
}

qreal QMediaPlayer::playbackRate() const
{
    Q_D(const QMediaPlayer);

    if (d->control != 0)
        return d->control->playbackRate();

    return 0.0;
}

/*!
    Returns the current error state.
*/

QMediaPlayer::Error QMediaPlayer::error() const
{
    return d_func()->error;
}

QString QMediaPlayer::errorString() const
{
    return d_func()->errorString;
}

/*!
    Returns the current network access point  in use.
    If a default contructed QNetworkConfiguration is returned
    this feature is not available or that none of the
    current supplied configurations are in use.
*/
QNetworkConfiguration QMediaPlayer::currentNetworkConfiguration() const
{
    Q_D(const QMediaPlayer);

    if (d->networkAccessControl)
        return d_func()->networkAccessControl->currentConfiguration();

    return QNetworkConfiguration();
}

//public Q_SLOTS:
/*!
    Start or resume playing the current source.
*/

void QMediaPlayer::play()
{
    Q_D(QMediaPlayer);

    if (d->control == 0) {
        QMetaObject::invokeMethod(this, "_q_error", Qt::QueuedConnection,
                                    Q_ARG(int, QMediaPlayer::ServiceMissingError),
                                    Q_ARG(QString, tr("The QMediaPlayer object does not have a valid service")));
        return;
    }

    //if playlist control is available, the service should advance itself
    if (d->playlist && d->playlist->currentIndex() == -1 && !d->playlist->isEmpty())
        d->playlist->setCurrentIndex(0);

    // Reset error conditions
    d->error = NoError;
    d->errorString = QString();

    d->control->play();
}

/*!
    Pause playing the current source.
*/

void QMediaPlayer::pause()
{
    Q_D(QMediaPlayer);

    if (d->control != 0)
        d->control->pause();
}

/*!
    Stop playing, and reset the play position to the beginning.
*/

void QMediaPlayer::stop()
{
    Q_D(QMediaPlayer);

    if (d->control != 0)
        d->control->stop();
}

void QMediaPlayer::setPosition(qint64 position)
{
    Q_D(QMediaPlayer);

    if (d->control == 0 || !isSeekable())
        return;

    d->control->setPosition(qBound(qint64(0), position, duration()));
}

void QMediaPlayer::setVolume(int v)
{
    Q_D(QMediaPlayer);

    if (d->control == 0)
        return;

    int clamped = qBound(0, v, 100);
    if (clamped == volume())
        return;

    d->control->setVolume(clamped);
}

void QMediaPlayer::setMuted(bool muted)
{
    Q_D(QMediaPlayer);

    if (d->control == 0 || muted == isMuted())
        return;

    d->control->setMuted(muted);
}

void QMediaPlayer::setPlaybackRate(qreal rate)
{
    Q_D(QMediaPlayer);

    if (d->control != 0)
        d->control->setPlaybackRate(rate);
}

/*!
    Sets the current \a media source.

    If a \a stream is supplied; media data will be read from it instead of resolving the media
    source.  In this case the media source may still be used to resolve additional information
    about the media such as mime type.

    Setting the media to a null QMediaContent will cause the player to discard all
    information relating to the current media source and to cease all I/O operations related
    to that media.
*/

void QMediaPlayer::setMedia(const QMediaContent &media, QIODevice *stream)
{
    Q_D(QMediaPlayer);

    if (playlist() && playlist()->currentMedia() != media)
        setPlaylist(0);

    if (d->control != 0)
        d_func()->control->setMedia(media, stream);
}

/*!
    \internal
*/

bool QMediaPlayer::bind(QObject *obj)
{
    return QMediaObject::bind(obj);
}

/*!
    \internal
*/

void QMediaPlayer::unbind(QObject *obj)
{
    QMediaObject::unbind(obj);
}

/*!
    Returns the level of support a media player has for a \a mimeType and a set of \a codecs.

    The \a flags argument allows additional requirements such as performance indicators to be
    specified.
*/
QtMultimediaKit::SupportEstimate QMediaPlayer::hasSupport(const QString &mimeType,
                                               const QStringList& codecs,
                                               Flags flags)
{
    return QMediaServiceProvider::defaultServiceProvider()->hasSupport(QByteArray(Q_MEDIASERVICE_MEDIAPLAYER),
                                                                    mimeType,
                                                                    codecs,
                                                                    flags);
}

/*!
    Returns a list of MIME types supported by the media player.

    The \a flags argument causes the resultant list to be restricted to MIME types which can be supported
    given additional requirements, such as performance indicators.
*/
QStringList QMediaPlayer::supportedMimeTypes(Flags flags)
{
    return QMediaServiceProvider::defaultServiceProvider()->supportedMimeTypes(QByteArray(Q_MEDIASERVICE_MEDIAPLAYER),
                                                                               flags);
}

/*!
    \fn void QMediaPlayer::setVideoOutput(QVideoWidget* output)

    Attach a QVideoWidget video \a output to the media player.

    If the media player has already video output attached,
    it will be replaced with a new one.
*/
void QMediaPlayer::setVideoOutput(QVideoWidget *output)
{
    Q_D(QMediaPlayer);

    if (d->videoOutput)
        unbind(d->videoOutput);

    d->videoOutput = output && bind(output) ? output : 0;
}

/*!
    \fn void QMediaPlayer::setVideoOutput(QGraphicsVideoItem* output)

    Attach a QGraphicsVideoItem video \a output to the media player.

    If the media player has already video output attached,
    it will be replaced with a new one.
*/
void QMediaPlayer::setVideoOutput(QGraphicsVideoItem *output)
{
    Q_D(QMediaPlayer);

    if (d->videoOutput)
        unbind(d->videoOutput);

    d->videoOutput = output && bind(output) ? output : 0;
}

/*!
    Sets a video \a surface as the video output of a media player.

    If a video output has already been set on the media player the new surface
    will replace it.
*/

void QMediaPlayer::setVideoOutput(QAbstractVideoSurface *surface)
{
    Q_D(QMediaPlayer);

    d->surfaceOutput.setVideoSurface(surface);

    if (d->videoOutput != &d->surfaceOutput) {
        if (d->videoOutput)
            unbind(d->videoOutput);

        d->videoOutput = bind(&d->surfaceOutput) ? &d->surfaceOutput : 0;
    }
}

// Enums
/*!
    \enum QMediaPlayer::State

    Defines the current state of a media player.

    \value PlayingState The media player is currently playing content.
    \value PausedState The media player has paused playback, playback of the current track will
    resume from the position the player was paused at.
    \value StoppedState The media player is not playing content, playback will begin from the start
    of the current track.
*/

/*!
    \enum QMediaPlayer::MediaStatus

    Defines the status of a media player's current media.

    \value UnknownMediaStatus The status of the media cannot be determined.
    \value NoMedia The is no current media.  The player is in the StoppedState.
    \value LoadingMedia The current media is being loaded. The player may be in any state.
    \value LoadedMedia The current media has been loaded. The player is in the StoppedState.
    \value StalledMedia Playback of the current media has stalled due to insufficient buffering or
    some other temporary interruption.  The player is in the PlayingState or PausedState.
    \value BufferingMedia The player is buffering data but has enough data buffered for playback to
    continue for the immediate future.  The player is in the PlayingState or PausedState.
    \value BufferedMedia The player has fully buffered the current media.  The player is in the
    PlayingState or PausedState.
    \value EndOfMedia Playback has reached the end of the current media.  The player is in the
    StoppedState.
    \value InvalidMedia The current media cannot be played.  The player is in the StoppedState.
*/

/*!
    \enum QMediaPlayer::Error

    Defines a media player error condition.

    \value NoError No error has occurred.
    \value ResourceError A media resource couldn't be resolved.
    \value FormatError The format of a media resource isn't (fully) supported.  Playback may still
    be possible, but without an audio or video component.
    \value NetworkError A network error occurred.
    \value AccessDeniedError There are not the appropriate permissions to play a media resource.
    \value ServiceMissingError A valid playback service was not found, playback cannot proceed.
*/

// Signals
/*!
    \fn QMediaPlayer::error(QMediaPlayer::Error error)

    Signals that an \a error condition has occurred.

    \sa errorString()
*/

/*!
    \fn void QMediaPlayer::stateChanged(State state)

    Signal the \a state of the Player object has changed.
*/

/*!
    \fn QMediaPlayer::mediaStatusChanged(QMediaPlayer::MediaStatus status)

    Signals that the \a status of the current media has changed.

    \sa mediaStatus()
*/

/*!
    \fn void QMediaPlayer::mediaChanged(const QMediaContent &media);

    Signals that the current playing content will be obtained from \a media.

    \sa media()
*/

/*!
    \fn void QMediaPlayer::playbackRateChanged(qreal rate);

    Signals the playbackRate has changed to \a rate.
*/

/*!
    \fn void QMediaPlayer::seekableChanged(bool seekable);

    Signals the \a seekable status of the player object has changed.
*/

// Properties
/*!
    \property QMediaPlayer::state
    \brief the media player's playback state.

    By default this property is QMediaPlayer::Stopped

    \sa mediaStatus(), play(), pause(), stop()
*/

/*!
    \property QMediaPlayer::error
    \brief a string describing the last error condition.

    \sa error()
*/

/*!
    \property QMediaPlayer::media
    \brief the active media source being used by the player object.

    The player object will use the QMediaContent for selection of the content to
    be played.

    By default this property has a null QMediaContent.

    Setting this property to a null QMediaContent will cause the player to discard all
    information relating to the current media source and to cease all I/O operations related
    to that media.

    \sa QMediaContent
*/

/*!
    \property QMediaPlayer::playlist
    \brief the media playlist being used by the player object.

    The player object will use the current playlist item for selection of the content to
    be played.

    By default this property is set to null.

    If the media playlist is used as a source, QMediaPlayer::media is updated with
    a current playlist item. The current source should be selected with
    QMediaPlaylist::setCurrentIndex(int) instead of QMediaPlayer::setMedia(),
    otherwise the current playlist will be discarded.

    \sa QMediaContent
*/


/*!
    \property QMediaPlayer::mediaStatus
    \brief the status of the current media stream.

    The stream status describes how the playback of the current stream is
    progressing.

    By default this property is QMediaPlayer::NoMedia

    \sa state
*/

/*!
    \property QMediaPlayer::duration
    \brief the duration of the current media.

    The value is the total playback time in milliseconds of the current media.
    The value may change across the life time of the QMediaPlayer object and
    may not be available when initial playback begins, connect to the
    durationChanged() signal to receive status notifications.
*/

/*!
    \property QMediaPlayer::position
    \brief the playback position of the current media.

    The value is the current playback position, expressed in milliseconds since
    the beginning of the media. Periodically changes in the position will be
    indicated with the signal positionChanged(), the interval between updates
    can be set with QMediaObject's method setNotifyInterval().
*/

/*!
    \property QMediaPlayer::volume
    \brief the current playback volume.

    The playback volume is a linear in effect and the value can range from 0 -
    100, values outside this range will be clamped.
*/

/*!
    \property QMediaPlayer::muted
    \brief the muted state of the current media.

    The value will be true if the playback volume is muted; otherwise false.
*/

/*!
    \property QMediaPlayer::bufferStatus
    \brief the percentage of the temporary buffer filled before playback begins.

    When the player object is buffering; this property holds the percentage of
    the temporary buffer that is filled. The buffer will need to reach 100%
    filled before playback can resume, at which time the MediaStatus will be
    BufferedMedia.

    \sa mediaStatus()
*/

/*!
    \property QMediaPlayer::audioAvailable
    \brief the audio availabilty status for the current media.

    As the life time of QMediaPlayer can be longer than the playback of one
    QMediaContent, this property may change over time, the
    audioAvailableChanged signal can be used to monitor it's status.
*/

/*!
    \property QMediaPlayer::videoAvailable
    \brief the video availability status for the current media.

    If available, the QVideoWidget class can be used to view the video. As the
    life time of QMediaPlayer can be longer than the playback of one
    QMediaContent, this property may change over time, the
    videoAvailableChanged signal can be used to monitor it's status.

    \sa QVideoWidget, QMediaContent
*/

/*!
    \property QMediaPlayer::seekable
    \brief the seek-able status of the current media

    If seeking is supported this property will be true; false otherwise. The
    status of this property may change across the life time of the QMediaPlayer
    object, use the seekableChanged signal to monitor changes.
*/

/*!
    \property QMediaPlayer::playbackRate
    \brief the playback rate of the current media.

    This value is a multiplier applied to the media's standard play rate. By
    default this value is 1.0, indicating that the media is playing at the
    standard pace. Values higher than 1.0 will increase the rate of play.
    Values less than zero can be set and indicate the media will rewind at the
    multiplier of the standard pace.

    Not all playback services support change of the playback rate. It is
    framework defined as to the status and quality of audio and video
    while fast forwarding or rewinding.
*/

/*!
    \fn void QMediaPlayer::durationChanged(qint64 duration)

    Signal the duration of the content has changed to \a duration, expressed in milliseconds.
*/

/*!
    \fn void QMediaPlayer::positionChanged(qint64 position)

    Signal the position of the content has changed to \a position, expressed in
    milliseconds.
*/

/*!
    \fn void QMediaPlayer::volumeChanged(int volume)

    Signal the playback volume has changed to \a volume.
*/

/*!
    \fn void QMediaPlayer::mutedChanged(bool muted)

    Signal the mute state has changed to \a muted.
*/

/*!
    \fn void QMediaPlayer::videoAvailableChanged(bool videoAvailable)

    Signal the availability of visual content has changed to \a videoAvailable.
*/

/*!
    \fn void QMediaPlayer::audioAvailableChanged(bool available)

    Signals the availability of audio content has changed to \a available.
*/

/*!
    \fn void QMediaPlayer::bufferStatusChanged(int percentFilled)

    Signal the amount of the local buffer filled as a percentage by \a percentFilled.
*/

/*!
   \fn void QMediaPlayer::networkConfigurationChanged(const QNetworkConfiguration &configuration)

    Signal that the active in use network access point  has been changed to \a configuration and all subsequent network access will use this \a configuration.
*/

/*!
    \enum QMediaPlayer::Flag

    \value LowLatency       The player is expected to be used with simple audio formats,
            but playback should start without significant delay.
            Such playback service can be used for beeps, ringtones, etc.

    \value StreamPlayback   The player is expected to play QIODevice based streams.
            If passed to QMediaPlayer constructor, the service supporting
            streams playback will be chosen.

    \value VideoSurface     The player is expected to be able to render to a
            QAbstractVideoSurface \l {setVideoOutput()}{output}.
*/

#include "moc_qmediaplayer.cpp"
QT_END_NAMESPACE

