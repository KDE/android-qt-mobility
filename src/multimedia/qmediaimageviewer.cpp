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

#include "qmediaimageviewer.h"

#include "qmediaobject_p.h"
#include "qmediaimageviewerservice_p.h"

#include "qmediaplaylist.h"
#include "qmediacontent.h"
#include "qmediaresource.h"

#include <QtCore/qcoreevent.h>
#include <QtCore/qtextstream.h>
#include <QtCore/qdatetime.h>

QT_BEGIN_NAMESPACE

class QMediaImageViewerPrivate : public QMediaObjectPrivate
{
    Q_DECLARE_NON_CONST_PUBLIC(QMediaImageViewer)
public:
    QMediaImageViewerPrivate():
        viewerControl(0), playlist(0),
        state(QMediaImageViewer::StoppedState), timeout(3000), pauseTime(0)
    {
    }

    void _q_mediaStatusChanged(QMediaImageViewer::MediaStatus status);
    void _q_playlistMediaChanged(const QMediaContent &content);
    void _q_playlistDestroyed();

    QMediaImageViewerControl *viewerControl;
    QMediaPlaylist *playlist;
    QMediaImageViewer::State state;
    int timeout;
    int pauseTime;
    QTime time;
    QBasicTimer timer;
    QMediaContent media;
};

void QMediaImageViewerPrivate::_q_mediaStatusChanged(QMediaImageViewer::MediaStatus status)
{
    switch (status) {
    case QMediaImageViewer::NoMedia:
    case QMediaImageViewer::LoadingMedia:
        emit q_func()->mediaStatusChanged(status);
        break;
    case QMediaImageViewer::LoadedMedia:
        if (state == QMediaImageViewer::PlayingState) {
            time.start();
            timer.start(qMax(0, timeout), q_func());
            q_func()->addPropertyWatch("elapsedTime");
        }
        emit q_func()->mediaStatusChanged(status);
        emit q_func()->elapsedTimeChanged(0);
        break;
    case QMediaImageViewer::InvalidMedia:
        emit q_func()->mediaStatusChanged(status);

        if (state == QMediaImageViewer::PlayingState) {
            playlist->next();
            if (playlist->currentIndex() < 0)
                emit q_func()->stateChanged(state = QMediaImageViewer::StoppedState);
        }
        break;
    }
}

void QMediaImageViewerPrivate::_q_playlistMediaChanged(const QMediaContent &content)
{
    media = content;
    pauseTime = 0;

    viewerControl->showMedia(media);

    emit q_func()->mediaChanged(media);
}

void QMediaImageViewerPrivate::_q_playlistDestroyed()
{
    playlist = 0;
    timer.stop();

    if (state != QMediaImageViewer::StoppedState)
        emit q_func()->stateChanged(state = QMediaImageViewer::StoppedState);
}

/*!
    \class QMediaImageViewer
    \brief The QMediaImageViewer class provides a means of viewing image media.
    \ingroup multimedia
    \preliminary

    QMediaImageViewer is used together with a media display object such as
    QVideoWidget to present an image.  A display object is attached to the
    image viewer by passing a pointer to the QMediaImageViewer instance to the
    setMediaObject() function of the display object.

    \code
    viewer = new QMediaImageViewer(this);

    display = new QVideoWidget;
    display->setMediaObject(viewer);
    display->show();
    \endcode

    QMediaImageViewer can be paired with a QMediaPlaylist to create a slide
    show of images. Constructing a QMediaPlaylist with a pointer to an
    instance of QMediaImageViewer will attach it to the image viewer;
    changing the playlist's selection will then change the media displayed
    by the image viewer.  With a playlist attached QMediaImageViewer's
    play(), pause(), and stop() slots can be control the progression of the
    playlist.  The \l timeout property determines how long an image is
    displayed for before progressing to the next in the playlist, and the
    \l elapsedTime property holds how the duration the current image has
    been displayed for.

    \code
    playlist = new QMediaPlaylist(this);
    playlist->setMediaObject(viewer);
    playlist->setPlaybackMode(QMediaPlaylist::Loop);
    playlist->addMedia(image1);
    playlist->addMedia(image2);
    playlist->addMedia(image3);

    viewer->setTimeout(5000);
    viewer->play();
    \endcode
*/

/*!
    \enum QMediaImageViewer::State

    Enumerates the possible control states an image viewer may be in.  The
    control state of an image viewer determines whether the image viewer is
    automatically progressing through images in an attached playlist.

    \value StoppedState The image viewer is stopped, and will not automatically move to the next
    image.  The \l elapsedTime is fixed at 0.
    \value PlayingState The slide show is playing, and will move to the next image when the
    \l elapsedTime reaches the \l timeout.  The \l elapsedTime is being incremented.
    \value PausedState The image viewer is paused, and will not automatically move the to next
    image.  The \l elapsedTime is fixed at the time the image viewer was paused.
*/

/*!
    \enum QMediaImageViewer::MediaStatus

    Enumerates the status of an image viewer's current media.

    \value NoMedia  There is no current media.
    \value LoadingMedia The image viewer is loading the current media.
    \value LoadedMedia The image viewer has loaded the current media.
    \value InvalidMedia The current media cannot be loaded.
*/

/*!
    Constructs a new image viewer with the given \a parent.
*/
QMediaImageViewer::QMediaImageViewer(QObject *parent)
    : QMediaObject(*new QMediaImageViewerPrivate, parent, new QMediaImageViewerService)
{
    Q_D(QMediaImageViewer);

    d->viewerControl = qobject_cast<QMediaImageViewerControl*>(
            d->service->control(QMediaImageViewerControl_iid));

    connect(d->viewerControl, SIGNAL(mediaStatusChanged(QMediaImageViewer::MediaStatus)),
            this, SLOT(_q_mediaStatusChanged(QMediaImageViewer::MediaStatus)));
}

/*!
    Destroys an image viewer.
*/
QMediaImageViewer::~QMediaImageViewer()
{
    Q_D(QMediaImageViewer);

    delete d->service;
}

/*!
    \property QMediaImageViewer::state
    \brief the playlist control state of a slide show.
*/

QMediaImageViewer::State QMediaImageViewer::state() const
{
    return d_func()->state;
}

/*!
    \fn QMediaImageViewer::stateChanged(QMediaImageViewer::State state)

    Signals that the playlist control \a state of an image viewer has changed.
*/

/*!
    \property QMediaImageViewer::mediaStatus
    \brief the status of the current media.
*/

QMediaImageViewer::MediaStatus QMediaImageViewer::mediaStatus() const
{
    return d_func()->viewerControl->mediaStatus();
}

/*!
    \fn QMediaImageViewer::mediaStatusChanged(QMediaImageViewer::MediaStatus status)

    Signals the the \a status of the current media has changed.
*/

/*!
    \property QMediaImageViewer::media
    \brief the media an image viewer is presenting.
*/

QMediaContent QMediaImageViewer::media() const
{
    Q_D(const QMediaImageViewer);

    return d->media;
}

void QMediaImageViewer::setMedia(const QMediaContent &media)
{
    Q_D(QMediaImageViewer);

    d->media = media;

    if (d->timer.isActive()) {
        d->pauseTime = 0;
        d->timer.stop();
        removePropertyWatch("elapsedTime");
        emit elapsedTimeChanged(0);
    }

    if (d->state != QMediaImageViewer::StoppedState)
        emit stateChanged(d->state = QMediaImageViewer::StoppedState);

    d->viewerControl->showMedia(d->media);

    emit mediaChanged(d->media);
}

/*!
    \fn QMediaImageViewer::mediaChanged(const QMediaContent &media)

    Signals that the \a media an image viewer is presenting has changed.
*/

/*!
    \property QMediaImageViewer::timeout
    \brief the amount of time in milliseconds an image is displayed for before moving to the next
    image.

    The timeout only applies if the image viewer has a playlist attached and is in the PlayingState.
*/

int QMediaImageViewer::timeout() const
{
    return d_func()->timeout;
}

void QMediaImageViewer::setTimeout(int timeout)
{
    Q_D(QMediaImageViewer);

    d->timeout = qMax(0, timeout);

    if (d->timer.isActive())
        d->timer.start(qMax(0, d->timeout - d->pauseTime - d->time.elapsed()), this);
}

/*!
    \property QMediaImageViewer::elapsedTime
    \brief the amount of time in milliseconds that has elapsed since the current image was loaded.

    The elapsed time only increases while the image viewer is in the PlayingState.  If stopped the
    elapsed time will be reset to 0.
*/

int QMediaImageViewer::elapsedTime() const
{
    Q_D(const QMediaImageViewer);

    int elapsedTime = d->pauseTime;

    if (d->timer.isActive())
        elapsedTime += d->time.elapsed();

    return elapsedTime;
}

/*!
    \fn QMediaImageViewer::elapsedTimeChanged(int time)

    Signals that the amount of \a time in milliseconds since the current
    image was loaded has changed.

    This signal is emitted at a regular interval when the image viewer is
    in the PlayingState and an image is loaded.  The notification interval
    is controlled by the QMediaObject::notifyInterval property.

    \sa timeout, QMediaObject::notifyInterval
*/

/*!
    \internal
*/
void QMediaImageViewer::bind(QObject *object)
{
    Q_D(QMediaImageViewer);

    if (QMediaPlaylist *playlist = qobject_cast<QMediaPlaylist *>(object)) {
        if (d->playlist) {
            qWarning("QMediaImageViewer::bind(): already bound to a playlist, detaching the current one");
            d->playlist->setMediaObject(0);
        }
        d->playlist = playlist;

        connect(d->playlist, SIGNAL(currentMediaChanged(QMediaContent)),
                this, SLOT(_q_playlistMediaChanged(QMediaContent)));
        connect(d->playlist, SIGNAL(destroyed()), this, SLOT(_q_playlistDestroyed()));
    }
}

/*!
     \internal
 */
void QMediaImageViewer::unbind(QObject *object)
{
    Q_D(QMediaImageViewer);

    if (object == d->playlist) {
        disconnect(d->playlist, SIGNAL(currentMediaChanged(QMediaContent)),
                   this, SLOT(_q_playlistMediaChanged(QMediaContent)));
        disconnect(d->playlist, SIGNAL(destroyed()), this, SLOT(_q_playlistDestroyed()));

        d->playlist = 0;
        setMedia(QMediaContent());
    }
}

/*!
    Starts a slide show.

    If the playlist has no current media this will start at the beginning of the playlist, otherwise
    it will resume from the current media.

    If no playlist is attached to an image viewer this will do nothing.
*/
void QMediaImageViewer::play()
{
    Q_D(QMediaImageViewer);

    if (d->playlist && d->playlist->mediaCount() > 0 && d->state != PlayingState) {
        d->state = PlayingState;

        switch (d->viewerControl->mediaStatus()) {
        case NoMedia:
        case InvalidMedia:
            d->playlist->next();
            if (d->playlist->currentIndex() < 0)
                d->state = StoppedState;
            break;
        case LoadingMedia:
            break;
        case LoadedMedia:
            d->time.start();
            d->timer.start(qMax(0, d->timeout - d->pauseTime), this);
            break;
        }

        if (d->state == PlayingState)
            emit stateChanged(d->state);
    }
}

/*!
    Pauses a slide show.

    The current media and elapsed time are retained.  If resumed, the current image will be
    displayed for the remainder of the time out period before the next image is loaded.
*/
void QMediaImageViewer::pause()
{
    Q_D(QMediaImageViewer);

    if (d->state == PlayingState) {
        if (d->viewerControl->mediaStatus() == LoadedMedia) {
            d->pauseTime += d->timeout - d->time.elapsed();
            d->timer.stop();
            removePropertyWatch("elapsedTime");
        }

        emit stateChanged(d->state = PausedState);
        emit elapsedTimeChanged(d->pauseTime);
    }
}

/*!
    Stops a slide show.

    The current media is retained, but the elapsed time is discarded.  If resumed, the current
    image will be displayed for the full time out period before the next image is loaded.
*/
void QMediaImageViewer::stop()
{
    Q_D(QMediaImageViewer);

    switch (d->state) {
    case PlayingState:
        d->timer.stop();
        removePropertyWatch("elapsedTime");
        // fall through.
    case PausedState:
        d->pauseTime = 0;
        d->state = QMediaImageViewer::StoppedState;

        emit stateChanged(d->state);
        emit elapsedTimeChanged(0);
        break;
    case StoppedState:
        break;
    }
}

/*!
    \reimp

    \internal
*/
void QMediaImageViewer::timerEvent(QTimerEvent *event)
{
    Q_D(QMediaImageViewer);

    if (event->timerId() == d->timer.timerId()) {
        d->timer.stop();
        removePropertyWatch("elapsedTime");
        emit elapsedTimeChanged(d->pauseTime = d->timeout);

        d->playlist->next();

        if (d->playlist->currentIndex() < 0) {
            d->pauseTime = 0;
            emit stateChanged(d->state = StoppedState);
            emit elapsedTimeChanged(0);
        }
    } else {
        QMediaObject::timerEvent(event);
    }
}

#include "moc_qmediaimageviewer.cpp"
QT_END_NAMESPACE

