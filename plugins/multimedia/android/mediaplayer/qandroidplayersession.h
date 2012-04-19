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

#ifndef QANDROIDPLAYERSESSION_H
#define QANDROIDPLAYERSESSION_H

#include <QObject>
#include <QtNetwork/qnetworkrequest.h>
#include <QTimer>
#include <qmediaplayer.h>
#include <qmediastreamscontrol.h>
#include <qmediaplayercontrol.h>

#include "mediaPlayerJNI.h"
#include "qandroidvideothread.h"
#include "qandroidvideowidget.h"

QT_USE_NAMESPACE

class QAndroidVideoHelper;
#define QANDROID_DEBUG qWarning()<<__LINE__<<__FILE__
        class QAndroidPlayerSession : public QObject,
        public QtMediaPlayerJNI::QAndroidMediaListner
{
    Q_OBJECT

public:
    QAndroidPlayerSession(QObject *parent,QAndroidVideoWidgetControl *control);
    virtual ~QAndroidPlayerSession();

    QNetworkRequest request() const;
    QMediaPlayer::State state() const { return m_state; }
    qint64 duration();
    qint64 position();
    bool isMetaDataAvailable();
    bool isBuffering() const;
    int bufferingProgress() const;
    int volume() const;
    bool isMuted() const;
    bool isAudioAvailable() const;
    QString getPath();
    bool isVideoAvailable() const;
    bool isSeekable() const;
    qreal playbackRate() const;
    void setPlaybackRate(qreal rate);
    QVariant getMetaData(int key);
    virtual void updateBufferedProgress (int bufferedProgress);
    virtual void playFinshed();
    QList<QtMultimediaKit::MetaData> availableMetaData();
    int activeStream(QMediaStreamsControl::StreamType streamType) const;
    void setActiveStream(QMediaStreamsControl::StreamType streamType, int streamNumber);
    void stopVideo();
    bool m_stop;



public slots:
    void load(const QNetworkRequest &url);
    bool play();
    bool pause();
    void stop();
    bool seek(qint64 pos);
    void setVolume(int volume);
    void setMuted(bool muted);


signals:
    void durationChanged(qint64 duration);
    void positionChanged(qint64 position);
    void stateChanged(QMediaPlayer::State state);
    void volumeChanged(int volume);
    void mutedStateChanged(bool muted);
    void audioAvailableChanged(bool audioAvailable);
    void videoAvailableChanged(bool videoAvailable);
    void bufferingChanged(bool buffering);
    void bufferingProgressChanged(int percentFilled);
    void playbackFinished();
    void tagsChanged();
    void streamsChanged();
    void seekableChanged(bool);
    void error(int error, const QString &errorString);
    void invalidMedia();
    void playbackRateChanged(qreal);

private slots:
    void setSeekable(bool);
    bool doPlay();

private:
    QNetworkRequest m_request;
    QAndroidVideoHelper *m_helper;
    QAndroidVideoWidget* m_widget;
    bool m_releaseWait;
    QAndroidVideoWidgetControl* m_widgetControl;
    QNetworkRequest m_savedRequest;
    QTimer *m_timer;
    QMediaPlayer::State m_state;
    QMediaPlayer::State m_pendingState;
    bool m_usePlaybin2;
    int m_uniqueID;
    bool m_usingColorspaceElement;
    QObject *m_videoOutput;
    QList< QMap<QtMultimediaKit::MetaData,QVariant> > m_streamProperties;
    QList<QMediaStreamsControl::StreamType> m_streamTypes;
    QMap<QMediaStreamsControl::StreamType, int> m_playbin2StreamOffset;
    int m_volume;
    int m_saveVolume;
    qreal m_playbackRate;
    bool m_muted;
    bool m_audioAvailable;
    bool m_videoAvailable;
    bool m_seekable;
    int m_duartion;
    int m_lastPosition;
    static int m_bufferProgress;
    qint64 m_qcurrPosition;
    qint64 m_qlastPosition;
    qint64 m_qduration;
    const char* m_rawDataPath;

};
#endif // QANDROIDPLAYERSESSION_H
