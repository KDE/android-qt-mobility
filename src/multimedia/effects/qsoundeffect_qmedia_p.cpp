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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// INTERNAL USE ONLY: Do NOT use for any other purpose.
//

#include "qsoundeffect_qmedia_p.h"

#include <QtCore/qcoreapplication.h>

#include "qmediacontent.h"
#include "qmediaplayer.h"


QT_BEGIN_NAMESPACE

QSoundEffectPrivate::QSoundEffectPrivate(QObject* parent):
    QObject(parent),
    m_loopCount(1),
    m_runningCount(0),
    m_player(0),
    m_status(QSoundEffect::Null),
    m_playing(false)
{
    m_player = new QMediaPlayer(this, QMediaPlayer::LowLatency);
    connect(m_player, SIGNAL(stateChanged(QMediaPlayer::State)), SLOT(stateChanged(QMediaPlayer::State)));
    connect(m_player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));
    connect(m_player, SIGNAL(error(QMediaPlayer::Error)), SLOT(error(QMediaPlayer::Error)));
    connect(m_player, SIGNAL(mutedChanged(bool)), SIGNAL(mutedChanged()));
    connect(m_player, SIGNAL(volumeChanged(int)), SIGNAL(volumeChanged()));
}

QSoundEffectPrivate::~QSoundEffectPrivate()
{
}

QStringList QSoundEffectPrivate::supportedMimeTypes()
{
    return QMediaPlayer::supportedMimeTypes();
}

QUrl QSoundEffectPrivate::source() const
{
    return m_player->media().canonicalUrl();
}

void QSoundEffectPrivate::setSource(const QUrl &url)
{
    m_player->setMedia(url);
}

int QSoundEffectPrivate::loopCount() const
{
    return m_loopCount;
}

void QSoundEffectPrivate::setLoopCount(int loopCount)
{
    m_loopCount = loopCount;
}

int QSoundEffectPrivate::volume() const
{
    return m_player->volume();
}

void QSoundEffectPrivate::setVolume(int volume)
{
    m_player->setVolume(volume);
}

bool QSoundEffectPrivate::isMuted() const
{
    return m_player->isMuted();
}

void QSoundEffectPrivate::setMuted(bool muted)
{
    m_player->setMuted(muted);
}

bool QSoundEffectPrivate::isLoaded() const
{
    return m_status == QSoundEffect::Ready;
}

bool QSoundEffectPrivate::isPlaying() const
{
    return m_playing;
}

QSoundEffect::Status QSoundEffectPrivate::status() const
{
    return m_status;
}

void QSoundEffectPrivate::play()
{
    if (m_status == QSoundEffect::Null || m_status == QSoundEffect::Error)
        return;
    if (m_loopCount < 0) {
        m_runningCount = -1;
    }
    else {
        if (m_runningCount < 0)
            m_runningCount = 0;
        m_runningCount += m_loopCount;
    }
    m_player->play();
}

void QSoundEffectPrivate::stop()
{
    m_runningCount = 0;
    m_player->stop();
}

void QSoundEffectPrivate::stateChanged(QMediaPlayer::State state)
{
    if (state == QMediaPlayer::StoppedState) {
        if (m_runningCount < 0) {
            m_player->play();
        } else if (m_runningCount == 0) {
            setPlaying(false);
            return;
        } else if (--m_runningCount > 0) {
            m_player->play();
        } else {
            setPlaying(false);
        }
    } else {
        setPlaying(true);
    }
}

void QSoundEffectPrivate::mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    switch(status) {
    case QMediaPlayer::LoadingMedia:
        setStatus(QSoundEffect::Loading);
        break;
    case QMediaPlayer::NoMedia:
        setStatus(QSoundEffect::Null);
        break;
    case QMediaPlayer::InvalidMedia:
        setStatus(QSoundEffect::Error);
        break;
    default:
        setStatus(QSoundEffect::Ready);
        break;
    }
}

void QSoundEffectPrivate::error(QMediaPlayer::Error err)
{
    bool playingDirty = false;
    if (m_playing) {
        m_playing = false;
        playingDirty = true;
    }
    setStatus(QSoundEffect::Error);
    if (playingDirty)
        emit playingChanged();
}

void QSoundEffectPrivate::setStatus(QSoundEffect::Status status)
{
    if (m_status == status)
        return;
    bool oldLoaded = isLoaded();
    m_status = status;
    emit statusChanged();
    if (oldLoaded != isLoaded())
        emit loadedChanged();
}

void QSoundEffectPrivate::setPlaying(bool playing)
{
    if (m_playing == playing)
        return;
    m_playing = playing;
    emit playingChanged();
}

QT_END_NAMESPACE

#include "moc_qsoundeffect_qmedia_p.cpp"
