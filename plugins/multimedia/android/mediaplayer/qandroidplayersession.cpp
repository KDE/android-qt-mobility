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
#include <QtCore/qdatetime.h>
#include <QtCore/qdebug.h>
#include <QtCore/qsize.h>
#include <QtCore/qdebug.h>
#include <QTimer>

#include "qandroidplayersession.h"
#include "qandroidplayerservice.h"
#include "mediaPlayerJNI.h"
#include "qandroidvideothread.h"
#include <stdint.h>

int QAndroidPlayerSession::m_bufferProgress=0;

int getUniqueID()
{
    static int uniqueID=0;
    return ++uniqueID;
}

QAndroidPlayerSession::QAndroidPlayerSession(QObject *parent,QAndroidVideoWidgetControl *control)
    :QObject(parent),
    m_widgetControl(control),
    m_state(QMediaPlayer::StoppedState),
    m_pendingState(QMediaPlayer::StoppedState),
    m_usePlaybin2(false),
    m_usingColorspaceElement(false),
    m_videoOutput(0),
    m_volume(100),
    m_playbackRate(1.0),
    m_muted(false),
    m_audioAvailable(false),
    m_videoAvailable(false),
    m_seekable(true),
    m_lastPosition(0),
    m_qlastPosition(0),
    m_qcurrPosition(0),
    m_qduration(-1),
    m_uniqueID(0),
    m_stop(false)

{
    m_widget = m_widgetControl->getWidget();
    m_uniqueID = getUniqueID();
    m_helper = NULL;
}

QAndroidPlayerSession::~QAndroidPlayerSession()
{
    stop();
    QtMediaPlayerJNI::removeListener(m_uniqueID);
}

void QAndroidPlayerSession::load(const QNetworkRequest &request)
{

    m_savedRequest = request;
    QUrl mediaUrl = request.url();
    QString mediaPath = mediaUrl.path();
    if(!mediaPath.isEmpty())
    {
        QByteArray mediaPathByteArray = mediaPath.toLatin1();
        m_rawDataPath = mediaPathByteArray.data();
        if(!m_helper)
        {
            m_helper = new QAndroidVideoHelper(m_widget);
        }
        m_helper->doTheWork(2,0);;
        m_helper->setPath(mediaPath);
        m_helper->doTheWork(0,0);
        m_qduration = (qint64)QtMediaPlayerJNI::setQtMediaPlayer(this,m_uniqueID,mediaPath);
        emit durationChanged(m_qduration);
        m_metaDataRetriever.setDataSource(m_rawDataPath);
        emit tagsChanged();

    }
}

void QAndroidPlayerSession::stopVideo()
{
    m_helper->doTheWork(2,0);
}

qint64 QAndroidPlayerSession::duration()
{
    return m_qduration;
}

bool QAndroidPlayerSession::isMetaDataAvailable()
{
    bool available=false;
    for(int i=0;i<25;i++)
    {
        const char* metaData = m_metaDataRetriever.extractMetadata(i);
        QString metaDataString(metaData);
        if(!metaDataString.isEmpty())
        {
            available = true;
            break;
        }
    }
    return available;
}


QVariant QAndroidPlayerSession::getMetaData(int key)
{

    const char* metaData = m_metaDataRetriever.extractMetadata(key);
    QVariant ret(tr(metaData));
    return ret;
}

QString QAndroidPlayerSession::getPath()
{
    QString temp(m_rawDataPath);
    return temp;
}

QList<QtMultimediaKit::MetaData> QAndroidPlayerSession::availableMetaData()
{
    QList<QtMultimediaKit::MetaData> metaDataList;
    for(int i=0;i<25;i++)
    {
        const char* metaData = m_metaDataRetriever.extractMetadata(i);
        QString metaDataString(metaData);
        if(!metaDataString.isEmpty())
        {
            switch(i)
            {
            case 0:
                metaDataList.append(QtMultimediaKit::TrackNumber);
                break;
            case 1:
                metaDataList.append(QtMultimediaKit::AlbumTitle);
                break;
            case 2:
                break;
            case 3:
                metaDataList.append(QtMultimediaKit::Author);
                break;
            case 4:
                metaDataList.append(QtMultimediaKit::Composer);
                break;
            case 5:
                metaDataList.append(QtMultimediaKit::Date);
                break;
            case 6:
                metaDataList.append(QtMultimediaKit::Genre);
                break;
            case 7:
                metaDataList.append(QtMultimediaKit::Title);
                break;
            case 8:
                metaDataList.append(QtMultimediaKit::Year);
                break;
            case 9:
                metaDataList.append(QtMultimediaKit::Duration);
                break;
            case 10:
                metaDataList.append(QtMultimediaKit::TrackCount);
                break;
            case 11:
                break;
            case 12:
                metaDataList.append(QtMultimediaKit::AudioCodec);
                break;
            case 13:
                metaDataList.append(QtMultimediaKit::UserRating);
                break;
            case 14:
                metaDataList.append(QtMultimediaKit::Comment);
                break;
            case 15:
                metaDataList.append(QtMultimediaKit::Copyright);
                break;
            case 16:
                metaDataList.append(QtMultimediaKit::AudioBitRate);
                break;
            case 17:
                metaDataList.append(QtMultimediaKit::VideoFrameRate);
                break;
            case 18:
                break;
            case 19:
                break;
            case 20:
                break;
            case 21:
                metaDataList.append(QtMultimediaKit::Writer);
                break;
            case 22:
                metaDataList.append(QtMultimediaKit::MediaType);
                break;
            case 23:
                break;
            default:
                metaDataList.append(QtMultimediaKit::AlbumArtist);
                break;
            }
        }

    }
    return metaDataList;
}

qint64 QAndroidPlayerSession::position()
{

    m_qlastPosition = (qint64)QtMediaPlayerJNI::getCurrentPosition();

    return m_qlastPosition;
}

qreal QAndroidPlayerSession::playbackRate() const
{
    return 1.0;
}
//Android Media Player Currently not Supporting PlayBack Rates
void QAndroidPlayerSession::setPlaybackRate(qreal rate)
{
    Q_UNUSED(rate);
}

bool QAndroidPlayerSession::isBuffering() const
{
    bool buffering = false;
    if(m_bufferProgress!= 0)
    {
        buffering = true;
    }
    return buffering;
}

int QAndroidPlayerSession::bufferingProgress() const
{
    return m_bufferProgress;
}

int QAndroidPlayerSession::volume() const
{
    return m_volume;
}

bool QAndroidPlayerSession::isMuted() const
{
    return m_muted;
}

bool QAndroidPlayerSession::isAudioAvailable() const
{
    return true;
}
//Currently we are not supporting video playback
bool QAndroidPlayerSession::isVideoAvailable() const
{
    return false;
}

bool QAndroidPlayerSession::isSeekable() const
{
    return m_seekable;
}

bool QAndroidPlayerSession::play()
{
    return doPlay();
}

void QAndroidPlayerSession::updateBufferedProgress(int bufferedPercent)
{
    m_bufferProgress = bufferedPercent;
    emit bufferingProgressChanged(m_bufferProgress);
}

bool QAndroidPlayerSession::pause()
{
    if(m_pendingState != QMediaPlayer::StoppedState)
    {
        m_pendingState =  m_state = QMediaPlayer::PausedState;
        m_helper->doTheWork(1,position());
        QtMediaPlayerJNI::pause();
        return true;
    }
    return false;

}

void QAndroidPlayerSession::stop()
{
    QMediaPlayer::State oldState = m_state;
    m_pendingState = m_state = QMediaPlayer::StoppedState;

    QtMediaPlayerJNI::stop();
    if (oldState != m_state)
    {
        emit stateChanged(m_state);
    }
}

void QAndroidPlayerSession::playFinshed()
{
    emit playbackFinished();
    m_helper->doTheWork(2,0);
}

bool QAndroidPlayerSession::seek(qint64 ms)
{
    if(m_pendingState!= QMediaPlayer::StoppedState)
    {
        unsigned int mSec = (unsigned int)ms;
        m_helper->doTheWork(3,mSec);
        if(!m_stop)
        {

           m_helper->doTheWork(0,mSec);
           QtMediaPlayerJNI::WakeUpThread();
        }
        else
        {
            m_stop = false;
        }
        QtMediaPlayerJNI::seekTo(mSec);
        m_seekable = true;
    }
    return m_seekable;
}

void QAndroidPlayerSession::setVolume(int volume)
{
    if (m_volume != volume)
    {
        m_volume = volume;
        float fVolume= (float)volume;
        while(fVolume>100)
        {
            fVolume = fVolume/10;
        }
        QtMediaPlayerJNI::setVolume(fVolume,fVolume);
        emit volumeChanged(m_volume);
    }
}

void QAndroidPlayerSession::setMuted(bool muted)
{
    if (m_muted != muted) {
        m_muted = muted;
        if(m_muted)
        {
            m_saveVolume = m_volume;
            setVolume(0);
        }
        else
        {
            setVolume(m_saveVolume);
        }

        emit mutedStateChanged(m_muted);
    }
}

void QAndroidPlayerSession::setSeekable(bool seekable)
{
    if (seekable != m_seekable) {
        m_seekable = seekable;
        emit seekableChanged(m_seekable);
    }
}

bool QAndroidPlayerSession::doPlay()
{
    if(m_state == QMediaPlayer::PausedState)
    {
        m_helper->doTheWork(0,position());
    }
    m_pendingState = QMediaPlayer::PlayingState;
    if(m_muted)
    {
        QtMediaPlayerJNI::setVolume(0,0);
    }

    QtMediaPlayerJNI::play();
    emit stateChanged(m_pendingState);
    return true;
}




