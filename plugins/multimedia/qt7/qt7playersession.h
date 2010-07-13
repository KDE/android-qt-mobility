/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT7PLAYERSESSION_H
#define QT7PLAYERSESSION_H

#include <QtCore/qobject.h>
#include <QtCore/qset.h>

#include <qmediaplayercontrol.h>
#include <qmediaplayer.h>

#include <QtGui/qmacdefines_mac.h>

QT_BEGIN_NAMESPACE
class QT7PlayerControl;
class QMediaPlaylist;
class QMediaPlaylistNavigator;
class QT7VideoOutput;
class QT7PlayerSession;
class QT7PlayerService;


class QT7PlayerSession : public QObject
{
Q_OBJECT
public:
    QT7PlayerSession(QObject *parent = 0);
    ~QT7PlayerSession();

    void *movie() const;

    void setControl(QT7PlayerControl *control);

    void setVideoOutput(QT7VideoOutput *output);

    QMediaPlayer::State state() const;
    QMediaPlayer::MediaStatus mediaStatus() const;

    QMediaContent media() const;
    const QIODevice *mediaStream() const;
    void setMedia(const QMediaContent &content, QIODevice *stream);

    qint64 position() const;
    qint64 duration() const;

    int bufferStatus() const;

    int volume() const;
    bool isMuted() const;

    bool isAudioAvailable() const;
    bool isVideoAvailable() const;

    bool isSeekable() const;

    qreal playbackRate() const;

public slots:
    void setPlaybackRate(qreal rate);

    void setPosition(qint64 pos);

    void play();
    void pause();
    void stop();

    void setVolume(int volume);
    void setMuted(bool muted);

    void processEOS();
    void processLoadStateChange();
    void processVolumeChange();
    void processNaturalSizeChange();

signals:
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void stateChanged(QMediaPlayer::State newState);
    void mediaStatusChanged(QMediaPlayer::MediaStatus status);
    void volumeChanged(int volume);
    void mutedChanged(bool muted);
    void audioAvailableChanged(bool audioAvailable);
    void videoAvailableChanged(bool videoAvailable);
    void error(int error, const QString &errorString);

private:
    void openMovie(bool tryAsync);

    void *m_QTMovie;
    void *m_movieObserver;

    QMediaPlayer::State m_state;
    QMediaPlayer::MediaStatus m_mediaStatus;
    QIODevice *m_mediaStream;
    QMediaContent m_resources;

    QT7VideoOutput * m_videoOutput;

    mutable qint64 m_currentTime;

    bool m_muted;
    bool m_tryingAsync;
    int m_volume;
    qreal m_rate;

    qint64 m_duration;
    bool m_videoAvailable;
    bool m_audioAvailable;
};

QT_END_NAMESPACE

#endif
