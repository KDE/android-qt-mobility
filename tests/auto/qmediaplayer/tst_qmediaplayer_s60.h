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
#ifndef TST_QMEDIAPLAYER_S60_H
#define TST_QMEDIAPLAYER_S60_H

#include <QtTest/QtTest>
#include <QtCore>
#include <QtGui>
#include <QFile>

#include <QMediaPlayer>
#include <QMediaPlayerControl>
#include <QMediaPlaylist>
#include <QMediaService>
#include <QMediaStreamsControl>
#include <QVideoWidget>

QT_USE_NAMESPACE

#define WAIT_FOR_CONDITION(a,e)            \
    for (int _i = 0; _i < 500; _i += 1) {  \
        if ((a) == (e)) break;             \
        QTest::qWait(10);}


#define WAIT_LONG_FOR_CONDITION(a,e)        \
    for (int _i = 0; _i < 1800; _i += 1) {  \
        if ((a) == (e)) break;              \
        QTest::qWait(100);}

class mediaStatusList : public QObject, public QList<QMediaPlayer::MediaStatus>
{
    Q_OBJECT
public slots:
    void mediaStatus(QMediaPlayer::MediaStatus status) {
        append(status);
    }

public:
    mediaStatusList(QObject *obj, const char *aSignal)
    : QObject()
    {
        connect(obj, aSignal, this, SLOT(mediaStatus(QMediaPlayer::MediaStatus)));
    }
};

class MockProvider_s60 : public QMediaServiceProvider
{
public:
    MockProvider_s60(QMediaService *service):mockService(service) {}
    QMediaService *requestService(const QByteArray &, const QMediaServiceProviderHint &)
    {
        return mockService;
    }

    void releaseService(QMediaService *service) { delete service; }

    QMediaService *mockService;
};

class tst_QMediaPlayer_s60: public QObject
{
    Q_OBJECT

public slots:
    void initTestCase_data();
    void initTestCase_data_default_winscw();
    void initTestCase_data_default_armv5();
    void initTestCase_data_50_winscw();
    void initTestCase_data_50_armv5();
    void initTestCase_data_32_winscw();
    void initTestCase_data_32_armv5();
    void initTestCase_data_31_winscw();
    void initTestCase_data_31_armv5();
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void testPositionWhilePlaying();
    void testNullService();
    void testMedia();
    void testDuration();
    void testPosition();

    void testVolume();
    void testVolumeWhilePlaying();
    void testMuted();
    void testMutedWhilePlaying();
    void testVideoAndAudioAvailability();
    void testError();
    void testPlay();
    void testPause();
    void testStop();
    void testMediaStatus();
    void testPlaylist();
    void testPlaybackRate();
    void testPlaybackRateWhilePlaying();

private:
    QMediaPlayer *m_player;
    QVideoWidget *m_widget;
    bool runonce;
};

#endif // TST_QMEDIAPLAYER_S60_H
