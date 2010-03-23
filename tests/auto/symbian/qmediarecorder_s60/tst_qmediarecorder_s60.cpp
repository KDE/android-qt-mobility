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

#include <QtTest/QtTest>
#include <QDebug>
#include <qmediaobject.h>
#include <qmediacontrol.h>
#include <qmediaservice.h>
#include <qmediarecordercontrol.h>
#include <qmediarecorder.h>
#include <qaudioendpointselector.h>
#include <qaudioencodercontrol.h>
#include <qmediacontainercontrol.h>
#include <QtMultimedia/qaudioformat.h>
#include <qaudiocapturesource.h>

#define QTRY_COMPARE(a,e)                       \
    for (int _i = 0; _i < 5000; _i += 100) {    \
        if ((a) == (e)) break;                  \
        QTest::qWait(100);                      \
    }                                           \
    QCOMPARE(a, e)

#define QTRY_VERIFY(a)                       \
    for (int _i = 0; _i < 5000; _i += 100) {    \
        if (a) break;                  \
        QTest::qWait(100);                      \
    }                                           \
    QVERIFY(a)

QTM_USE_NAMESPACE
class tst_QMediaRecorder: public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();

private slots:
    void testAudioSink();
    void testAudioRecord();
    void testAudioEndPointSelector();
    void testAudioEncoderControl();
    void testMediaFormatsControl();
//    void testDefaultAudioEncodingSettings();

private:

    QAudioEncoderControl *audioEncoder;
    QAudioEndpointSelector *audioEndpoint;
    QMediaRecorder  *audiocapture;
    QAudioCaptureSource *captureSource;
};

void tst_QMediaRecorder::initTestCase()
{
    qRegisterMetaType<QtMobility::QMediaRecorder::State>("QMediaRecorder::State");
    qRegisterMetaType<QtMobility::QMediaRecorder::Error>("QMediaRecorder::Error");

    captureSource = new QAudioCaptureSource;
    audiocapture = new QMediaRecorder(captureSource);

    audioEndpoint = qobject_cast<QAudioEndpointSelector*>(audiocapture->service()->control(QAudioEndpointSelector_iid));
    audioEncoder = qobject_cast<QAudioEncoderControl*>(audiocapture->service()->control(QAudioEncoderControl_iid));
}

void tst_QMediaRecorder::cleanupTestCase()
{
    delete audiocapture;
    delete captureSource;
}

void tst_QMediaRecorder::testAudioSink()
{
    audiocapture->setOutputLocation(QUrl("test.tmp"));
    QUrl s = audiocapture->outputLocation();
    QCOMPARE(s.toString(), QString("test.tmp"));
}

void tst_QMediaRecorder::testAudioRecord()
{
}

void tst_QMediaRecorder::testAudioEndPointSelector()
{
    QSignalSpy audioSignal(audioEndpoint,SIGNAL(activeEndpointChanged(QString)));
    QVERIFY(audioEndpoint->availableEndpoints().count() == 1);
    QVERIFY(audioEndpoint->defaultEndpoint().compare("MMF") == 0);
    audioEndpoint->setActiveEndpoint("device2");
    QVERIFY(audioEndpoint->activeEndpoint().compare("device2") == 0);
    QVERIFY(audioSignal.count() == 1);
    QVERIFY(audioEndpoint->endpointDescription("device2").compare("") == 0);
}

void tst_QMediaRecorder::testAudioEncoderControl()
{
    QStringList codecs = audiocapture->supportedAudioCodecs();
    QVERIFY(codecs.count() == 1);
    QVERIFY(audiocapture->audioCodecDescription("audio/wav") == "WAV Write Format");
    QStringList options = audioEncoder->supportedEncodingOptions("audio/wav");
    QCOMPARE(options.count(), 0);
    QVERIFY(audioEncoder->encodingOption("audio/wav","bitrate").toInt() == -1);
    audioEncoder->setEncodingOption("audio/wav", "bitrate", QString("vbr"));
    QCOMPARE(audioEncoder->encodingOption("audio/wav","bitrate").toInt(), -1);
    QCOMPARE(audiocapture->supportedAudioSampleRates(), QList<int>() << 44100);
}

void tst_QMediaRecorder::testMediaFormatsControl()
{
    //audioocontainer types
    QCOMPARE(audiocapture->supportedContainers(), QStringList() << "audio/wav" << "audio/pcm");
    QCOMPARE(audiocapture->containerDescription("audio/pcm"), QString("WAV file format"));
    QCOMPARE(audiocapture->containerDescription("audio/x-wav"), QString("RAW file format"));
}

/*
void tst_QMediaRecorder::testDefaultAudioEncodingSettings()
{
    QAudioEncoderSettings audioSettings = audiocapture->audioSettings();
    QCOMPARE(audioSettings.codec(), QString("AMR"));
    QString format = audiocapture->containerMimeType();
    QCOMPARE(format, QString("audio/amr"));
/*    QCOMPARE(audioSettings.bitRate(), 128*1024);
    QCOMPARE(audioSettings.sampleRate(), -1);
    QCOMPARE(audioSettings.quality(), QtMedia::NormalQuality);
    QCOMPARE(audioSettings.channelCount(), -1);

    QCOMPARE(audioSettings.encodingMode(), QtMedia::ConstantQualityEncoding);

}*/


QTEST_MAIN(tst_QMediaRecorder)

#include "tst_qmediarecorder_s60.moc"
