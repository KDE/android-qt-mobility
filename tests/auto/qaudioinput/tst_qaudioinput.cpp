/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
#include <QtCore/qlocale.h>

#include <qaudioinput.h>
#include <qaudiodeviceinfo.h>
#include <qaudioformat.h>
#include <qaudio.h>

#include "wavheader.h"

#define AUDIO_BUFFER 192000

#if defined(Q_OS_SYMBIAN)
#define SRCDIR ""
#endif

class tst_QAudioInput : public QObject
{
    Q_OBJECT
public:
    tst_QAudioInput(QObject* parent=0) : QObject(parent) {}

private slots:
    void initTestCase();

    void format();
    void invalidFormat();

    void bufferSize();

    void notifyInterval();
    void disableNotifyInterval();

    void stopWhileStopped();
    void suspendWhileStopped();
    void resumeWhileStopped();

    void pull();
    void pullSuspendResume();

    void push();
    void pushSuspendResume();

    void cleanupTestCase();

private:
    QString formatToFileName(const QAudioFormat &format);
    QString workingDir();

    QAudioDeviceInfo audioDevice;
    QList<QAudioFormat> testFormats;
    QList<QFile*> audioFiles;

    QScopedPointer<QByteArray> m_byteArray;
    QScopedPointer<QBuffer> m_buffer;
};

QString tst_QAudioInput::formatToFileName(const QAudioFormat &format)
{
    const QString formatEndian = (format.byteOrder() == QAudioFormat::LittleEndian)
        ?   QString("LE") : QString("BE");

    const QString formatSigned = (format.sampleType() == QAudioFormat::SignedInt)
        ?   QString("signed") : QString("unsigned");

    return QString("%1_%2_%3_%4_%5")
        .arg(format.frequency())
        .arg(format.sampleSize())
        .arg(formatSigned)
        .arg(formatEndian)
        .arg(format.channels());
}


QString tst_QAudioInput::workingDir()
{
    QDir working(QString(SRCDIR));

    if (working.exists())
        return QString(SRCDIR);

    return QDir::currentPath();
}

void tst_QAudioInput::initTestCase()
{
    // Only perform tests if audio output device exists
    const QList<QAudioDeviceInfo> devices =
        QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    QVERIFY(devices.size() > 0);

    audioDevice = QAudioDeviceInfo::defaultInputDevice();


    QAudioFormat format;

    format.setCodec("audio/pcm");

    if (audioDevice.isFormatSupported(audioDevice.preferredFormat()))
        testFormats.append(audioDevice.preferredFormat());

    // PCM 8000  mono S8
    format.setFrequency(8000);
    format.setSampleSize(8);
    format.setSampleType(QAudioFormat::SignedInt);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setChannels(1);
    if (audioDevice.isFormatSupported(format))
        testFormats.append(format);

    // PCM 11025 mono S16LE
    format.setFrequency(11025);
    format.setSampleSize(16);
    if (audioDevice.isFormatSupported(format))
        testFormats.append(format);

    // PCM 22050 mono S16LE
    format.setFrequency(22050);
    if (audioDevice.isFormatSupported(format))
        testFormats.append(format);

    // PCM 22050 stereo S16LE
    format.setChannels(2);
    if (audioDevice.isFormatSupported(format))
        testFormats.append(format);

    // PCM 44100 stereo S16LE
    format.setFrequency(44100);
    if (audioDevice.isFormatSupported(format))
        testFormats.append(format);

    // PCM 48000 stereo S16LE
    format.setFrequency(48000);
    if (audioDevice.isFormatSupported(format))
        testFormats.append(format);

    QVERIFY(testFormats.size());

    foreach (format, testFormats) {
        QFile* file = new QFile(workingDir() + formatToFileName(format) + QString(".wav"));
        audioFiles.append(file);
    }
}

void tst_QAudioInput::format()
{
    QAudioInput audioInput(audioDevice.preferredFormat(), this);

    QAudioFormat requested = audioDevice.preferredFormat();
    QAudioFormat actual    = audioInput.format();

    QVERIFY2((requested.channels() == actual.channels()),
            QString("channels: requested=%1, actual=%2").arg(requested.channels()).arg(actual.channels()).toLocal8Bit().constData());
    QVERIFY2((requested.frequency() == actual.frequency()),
            QString("frequency: requested=%1, actual=%2").arg(requested.frequency()).arg(actual.frequency()).toLocal8Bit().constData());
    QVERIFY2((requested.sampleSize() == actual.sampleSize()),
            QString("sampleSize: requested=%1, actual=%2").arg(requested.sampleSize()).arg(actual.sampleSize()).toLocal8Bit().constData());
    QVERIFY2((requested.codec() == actual.codec()),
            QString("codec: requested=%1, actual=%2").arg(requested.codec()).arg(actual.codec()).toLocal8Bit().constData());
    QVERIFY2((requested.byteOrder() == actual.byteOrder()),
            QString("byteOrder: requested=%1, actual=%2").arg(requested.byteOrder()).arg(actual.byteOrder()).toLocal8Bit().constData());
    QVERIFY2((requested.sampleType() == actual.sampleType()),
            QString("sampleType: requested=%1, actual=%2").arg(requested.sampleType()).arg(actual.sampleType()).toLocal8Bit().constData());
}

void tst_QAudioInput::invalidFormat()
{
    QAudioFormat invalidFormat;
    invalidFormat.setFrequency(0);

    QVERIFY2(!audioDevice.isFormatSupported(invalidFormat),
            "isFormatSupported() is returning true on an invalid format");

    QAudioInput audioInput(invalidFormat, this);

    // Check that we are in the default state before calling start
    QVERIFY2((audioInput.state() == QAudio::StoppedState), "state() was not set to StoppedState before start()");
    QVERIFY2((audioInput.error() == QAudio::NoError), "error() was not set to QAudio::NoError before start()");

    audioInput.start();

    // Check that error is raised
    QVERIFY2((audioInput.error() == QAudio::OpenError),"error() was not set to QAudio::OpenError after start()");
}

void tst_QAudioInput::bufferSize()
{
    QAudioInput audioInput(audioDevice.preferredFormat(), this);

    QVERIFY2((audioInput.error() == QAudio::NoError), "error() was not set to QAudio::NoError on creation");

    audioInput.setBufferSize(512);
    QVERIFY2((audioInput.error() == QAudio::NoError), "error() is not QAudio::NoError after setBufferSize(512)");
    QVERIFY2((audioInput.bufferSize() == 512),
            QString("bufferSize: requested=512, actual=%2").arg(audioInput.bufferSize()).toLocal8Bit().constData());

    audioInput.setBufferSize(4096);
    QVERIFY2((audioInput.error() == QAudio::NoError), "error() is not QAudio::NoError after setBufferSize(4096)");
    QVERIFY2((audioInput.bufferSize() == 4096),
            QString("bufferSize: requested=4096, actual=%2").arg(audioInput.bufferSize()).toLocal8Bit().constData());

    audioInput.setBufferSize(8192);
    QVERIFY2((audioInput.error() == QAudio::NoError), "error() is not QAudio::NoError after setBufferSize(8192)");
    QVERIFY2((audioInput.bufferSize() == 8192),
            QString("bufferSize: requested=8192, actual=%2").arg(audioInput.bufferSize()).toLocal8Bit().constData());
}

void tst_QAudioInput::notifyInterval()
{
    QAudioInput audioInput(audioDevice.preferredFormat(), this);

    QVERIFY2((audioInput.error() == QAudio::NoError), "error() was not set to QAudio::NoError on creation");

    audioInput.setNotifyInterval(50);
    QVERIFY2((audioInput.error() == QAudio::NoError), "error() is not QAudio::NoError after setNotifyInterval(50)");
    QVERIFY2((audioInput.notifyInterval() == 50),
            QString("notifyInterval: requested=50, actual=%2").arg(audioInput.notifyInterval()).toLocal8Bit().constData());

    audioInput.setNotifyInterval(100);
    QVERIFY2((audioInput.error() == QAudio::NoError), "error() is not QAudio::NoError after setNotifyInterval(100)");
    QVERIFY2((audioInput.notifyInterval() == 100),
            QString("notifyInterval: requested=100, actual=%2").arg(audioInput.notifyInterval()).toLocal8Bit().constData());

    audioInput.setNotifyInterval(250);
    QVERIFY2((audioInput.error() == QAudio::NoError), "error() is not QAudio::NoError after setNotifyInterval(250)");
    QVERIFY2((audioInput.notifyInterval() == 250),
            QString("notifyInterval: requested=250, actual=%2").arg(audioInput.notifyInterval()).toLocal8Bit().constData());

    audioInput.setNotifyInterval(1000);
    QVERIFY2((audioInput.error() == QAudio::NoError), "error() is not QAudio::NoError after setNotifyInterval(1000)");
    QVERIFY2((audioInput.notifyInterval() == 1000),
            QString("notifyInterval: requested=1000, actual=%2").arg(audioInput.notifyInterval()).toLocal8Bit().constData());
}

void tst_QAudioInput::disableNotifyInterval()
{
    // Sets an invalid notification interval (QAudioInput::setNotifyInterval(0))
    // Checks that
    //  - No error is raised (QAudioInput::error() returns QAudio::NoError)
    //  - if <= 0, set to zero and disable notify signal

    QAudioInput audioInput(audioDevice.preferredFormat(), this);

    QVERIFY2((audioInput.error() == QAudio::NoError), "error() was not set to QAudio::NoError on creation");

    audioInput.setNotifyInterval(0);
    QVERIFY2((audioInput.error() == QAudio::NoError), "error() is not QAudio::NoError after setNotifyInterval(0)");
    QVERIFY2((audioInput.notifyInterval() == 0),
            "notifyInterval() is not zero after setNotifyInterval(0)");

    audioInput.setNotifyInterval(-1);
    QVERIFY2((audioInput.error() == QAudio::NoError), "error() is not QAudio::NoError after setNotifyInterval(-1)");
    QVERIFY2((audioInput.notifyInterval() == 0),
            "notifyInterval() is not zero after setNotifyInterval(-1)");

    //start and run to check if notify() is emitted
    if (audioFiles.size() > 0) {
        QAudioInput audioInputCheck(testFormats.at(0), this);
        audioInputCheck.setNotifyInterval(0);
        QSignalSpy notifySignal(&audioInputCheck, SIGNAL(notify()));
        audioFiles.at(0)->open(QIODevice::WriteOnly);
        audioInputCheck.start(audioFiles.at(0));
        QTest::qWait(3000); // 3 seconds should be plenty
        audioInputCheck.stop();
        QVERIFY2((notifySignal.count() == 0),
                QString("didn't disable notify interval: shouldn't have got any but got %1").arg(notifySignal.count()).toLocal8Bit().constData());
        audioFiles.at(0)->close();
    }
}

void tst_QAudioInput::stopWhileStopped()
{
    // Calls QAudioInput::stop() when object is already in StoppedState
    // Checks that
    //  - No state change occurs
    //  - No error is raised (QAudioInput::error() returns QAudio::NoError)

    QAudioInput audioInput(audioDevice.preferredFormat(), this);

    QVERIFY2((audioInput.state() == QAudio::StoppedState), "state() was not set to StoppedState before start()");
    QVERIFY2((audioInput.error() == QAudio::NoError), "error() was not set to QAudio::NoError before start()");

    QSignalSpy stateSignal(&audioInput, SIGNAL(stateChanged(QAudio::State)));
    audioInput.stop();

    // Check that no state transition occurred
    QVERIFY2((stateSignal.count() == 0), "stop() while stopped is emitting a signal and it shouldn't");
    QVERIFY2((audioInput.error() == QAudio::NoError), "error() was not set to QAudio::NoError after stop()");
}

void tst_QAudioInput::suspendWhileStopped()
{
    // Calls QAudioInput::suspend() when object is already in StoppedState
    // Checks that
    //  - No state change occurs
    //  - No error is raised (QAudioInput::error() returns QAudio::NoError)

    QAudioInput audioInput(audioDevice.preferredFormat(), this);

    QVERIFY2((audioInput.state() == QAudio::StoppedState), "state() was not set to StoppedState before start()");
    QVERIFY2((audioInput.error() == QAudio::NoError), "error() was not set to QAudio::NoError before start()");

    QSignalSpy stateSignal(&audioInput, SIGNAL(stateChanged(QAudio::State)));
    audioInput.suspend();

    // Check that no state transition occurred
    QVERIFY2((stateSignal.count() == 0), "stop() while suspended is emitting a signal and it shouldn't");
    QVERIFY2((audioInput.error() == QAudio::NoError), "error() was not set to QAudio::NoError after stop()");
}

void tst_QAudioInput::resumeWhileStopped()
{
    // Calls QAudioInput::resume() when object is already in StoppedState
    // Checks that
    //  - No state change occurs
    //  - No error is raised (QAudioInput::error() returns QAudio::NoError)

    QAudioInput audioInput(audioDevice.preferredFormat(), this);

    QVERIFY2((audioInput.state() == QAudio::StoppedState), "state() was not set to StoppedState before start()");
    QVERIFY2((audioInput.error() == QAudio::NoError), "error() was not set to QAudio::NoError before start()");

    QSignalSpy stateSignal(&audioInput, SIGNAL(stateChanged(QAudio::State)));
    audioInput.resume();

    // Check that no state transition occurred
    QVERIFY2((stateSignal.count() == 0), "resume() while stopped is emitting a signal and it shouldn't");
    QVERIFY2((audioInput.error() == QAudio::NoError), "error() was not set to QAudio::NoError after resume()");
}

void tst_QAudioInput::pull()
{
    for(int i=0; i<audioFiles.count(); i++) {
        QAudioInput audioInput(testFormats.at(i), this);

        audioInput.setNotifyInterval(100);

        QSignalSpy notifySignal(&audioInput, SIGNAL(notify()));
        QSignalSpy stateSignal(&audioInput, SIGNAL(stateChanged(QAudio::State)));

        // Check that we are in the default state before calling start
        QVERIFY2((audioInput.state() == QAudio::StoppedState), "state() was not set to StoppedState before start()");
        QVERIFY2((audioInput.error() == QAudio::NoError), "error() was not set to QAudio::NoError before start()");
        QVERIFY2((audioInput.elapsedUSecs() == qint64(0)),"elapsedUSecs() not zero on creation");

        audioFiles.at(i)->close();
        audioFiles.at(i)->open(QIODevice::WriteOnly);
        WavHeader wavHeader(testFormats.at(i));
        QVERIFY(wavHeader.write(*audioFiles.at(i)));

        audioInput.start(audioFiles.at(i));

        // Check that QAudioInput immediately transitions to ActiveState or IdleState
        QVERIFY2((stateSignal.count() > 0),"didn't emit signals on start()");
        QVERIFY2((audioInput.state() == QAudio::ActiveState || audioInput.state() == QAudio::IdleState),
                "didn't transition to ActiveState or IdleState after start()");
        QVERIFY2((audioInput.error() == QAudio::NoError), "error state is not equal to QAudio::NoError after start()");
        QVERIFY(audioInput.periodSize() > 0);
        stateSignal.clear();

        // Check that 'elapsed' increases
        QTest::qWait(40);
        QVERIFY2((audioInput.elapsedUSecs() > 0), "elapsedUSecs() is still zero after start()");

        // Allow some recording to happen
        QTest::qWait(3000); // 3 seconds should be plenty

        stateSignal.clear();

        qint64 processedUs = audioInput.processedUSecs();

        audioInput.stop();
        QTest::qWait(40);
        QVERIFY2((stateSignal.count() == 1),
            QString("didn't emit StoppedState signal after stop(), got %1 signals instead").arg(stateSignal.count()).toLocal8Bit().constData());
        QVERIFY2((audioInput.state() == QAudio::StoppedState), "didn't transitions to StoppedState after stop()");

        QVERIFY2((processedUs > 2800000 && processedUs < 3200000),
                QString("processedUSecs() doesn't fall in acceptable range, should be 3040000 (%1)").arg(processedUs).toLocal8Bit().constData());
        QVERIFY2((audioInput.error() == QAudio::NoError), "error() is not QAudio::NoError after stop()");
        QVERIFY2((audioInput.elapsedUSecs() == (qint64)0), "elapsedUSecs() not equal to zero in StoppedState");
        QVERIFY2((notifySignal.count() > 20 && notifySignal.count() < 40),
                QString("notify() signals emitted (%1) should be 30").arg(notifySignal.count()).toLocal8Bit().constData());

        WavHeader::writeDataLength(*audioFiles.at(i),audioFiles.at(i)->pos()-WavHeader::headerLength());
        audioFiles.at(i)->close();
    }
}

void tst_QAudioInput::pullSuspendResume()
{
    for(int i=0; i<audioFiles.count(); i++) {
        QAudioInput audioInput(testFormats.at(i), this);

        audioInput.setNotifyInterval(100);

        QSignalSpy notifySignal(&audioInput, SIGNAL(notify()));
        QSignalSpy stateSignal(&audioInput, SIGNAL(stateChanged(QAudio::State)));

        // Check that we are in the default state before calling start
        QVERIFY2((audioInput.state() == QAudio::StoppedState), "state() was not set to StoppedState before start()");
        QVERIFY2((audioInput.error() == QAudio::NoError), "error() was not set to QAudio::NoError before start()");
        QVERIFY2((audioInput.elapsedUSecs() == qint64(0)),"elapsedUSecs() not zero on creation");

        audioFiles.at(i)->close();
        audioFiles.at(i)->open(QIODevice::WriteOnly);
        WavHeader wavHeader(testFormats.at(i));
        QVERIFY(wavHeader.write(*audioFiles.at(i)));

        audioInput.start(audioFiles.at(i));

        // Check that QAudioInput immediately transitions to ActiveState or IdleState
        QVERIFY2((stateSignal.count() > 0),"didn't emit signals on start()");
        QVERIFY2((audioInput.state() == QAudio::ActiveState || audioInput.state() == QAudio::IdleState),
                "didn't transition to ActiveState or IdleState after start()");
        QVERIFY2((audioInput.error() == QAudio::NoError), "error state is not equal to QAudio::NoError after start()");
        QVERIFY(audioInput.periodSize() > 0);
        stateSignal.clear();

        // Check that 'elapsed' increases
        QTest::qWait(40);
        QVERIFY2((audioInput.elapsedUSecs() > 0), "elapsedUSecs() is still zero after start()");

        // Allow some recording to happen
        QTest::qWait(3000); // 3 seconds should be plenty

        QVERIFY2((audioInput.state() == QAudio::ActiveState),
                "didn't transition to ActiveState after some recording");
        QVERIFY2((audioInput.error() == QAudio::NoError), "error state is not equal to QAudio::NoError after some recording");

        stateSignal.clear();

        audioInput.suspend();

        // Give backends running in separate threads a chance to suspend.
        QTest::qWait(100);

        QVERIFY2((stateSignal.count() == 1),
            QString("didn't emit SuspendedState signal after suspend(), got %1 signals instead").arg(stateSignal.count()).toLocal8Bit().constData());
        QVERIFY2((audioInput.state() == QAudio::SuspendedState), "didn't transitions to SuspendedState after stop()");
        QVERIFY2((audioInput.error() == QAudio::NoError), "error() is not QAudio::NoError after stop()");
        stateSignal.clear();

        // Check that only 'elapsed', and not 'processed' increases while suspended
        qint64 elapsedUs = audioInput.elapsedUSecs();
        qint64 processedUs = audioInput.processedUSecs();
        QTest::qWait(1000);
        QVERIFY(audioInput.elapsedUSecs() > elapsedUs);
        QVERIFY(audioInput.processedUSecs() == processedUs);

        audioInput.resume();

        // Give backends running in separate threads a chance to resume.
        QTest::qWait(100);

        // Check that QAudioInput immediately transitions to ActiveState
        QVERIFY2((stateSignal.count() == 1),
                QString("didn't emit signal after resume(), got %1 signals instead").arg(stateSignal.count()).toLocal8Bit().constData());
        QVERIFY2((audioInput.state() == QAudio::ActiveState), "didn't transition to ActiveState after resume()");
        QVERIFY2((audioInput.error() == QAudio::NoError), "error state is not equal to QAudio::NoError after resume()");
        stateSignal.clear();

        processedUs = audioInput.processedUSecs();

        audioInput.stop();
        QTest::qWait(40);
        QVERIFY2((stateSignal.count() == 1),
            QString("didn't emit StoppedState signal after stop(), got %1 signals instead").arg(stateSignal.count()).toLocal8Bit().constData());
        QVERIFY2((audioInput.state() == QAudio::StoppedState), "didn't transitions to StoppedState after stop()");

        QVERIFY2((processedUs > 2800000 && processedUs < 3200000),
                QString("processedUSecs() doesn't fall in acceptable range, should be 3040000 (%1)").arg(processedUs).toLocal8Bit().constData());
        QVERIFY2((audioInput.error() == QAudio::NoError), "error() is not QAudio::NoError after stop()");
        QVERIFY2((audioInput.elapsedUSecs() == (qint64)0), "elapsedUSecs() not equal to zero in StoppedState");
        QVERIFY2((notifySignal.count() > 20 && notifySignal.count() < 40),
                QString("notify() signals emitted (%1) should be 30").arg(notifySignal.count()).toLocal8Bit().constData());

        WavHeader::writeDataLength(*audioFiles.at(i),audioFiles.at(i)->pos()-WavHeader::headerLength());
        audioFiles.at(i)->close();
    }
}

void tst_QAudioInput::push()
{
    for(int i=0; i<audioFiles.count(); i++) {
        QAudioInput audioInput(testFormats.at(i), this);

        audioInput.setNotifyInterval(100);

        QSignalSpy notifySignal(&audioInput, SIGNAL(notify()));
        QSignalSpy stateSignal(&audioInput, SIGNAL(stateChanged(QAudio::State)));

        // Check that we are in the default state before calling start
        QVERIFY2((audioInput.state() == QAudio::StoppedState), "state() was not set to StoppedState before start()");
        QVERIFY2((audioInput.error() == QAudio::NoError), "error() was not set to QAudio::NoError before start()");
        QVERIFY2((audioInput.elapsedUSecs() == qint64(0)),"elapsedUSecs() not zero on creation");

        audioFiles.at(i)->close();
        audioFiles.at(i)->open(QIODevice::WriteOnly);
        WavHeader wavHeader(testFormats.at(i));
        QVERIFY(wavHeader.write(*audioFiles.at(i)));

        QIODevice* feed = audioInput.start();

        // Check that QAudioInput immediately transitions to IdleState
        QVERIFY2((stateSignal.count() == 1),"didn't emit IdleState signal on start()");
        QVERIFY2((audioInput.state() == QAudio::IdleState),
                "didn't transition to IdleState after start()");
        QVERIFY2((audioInput.error() == QAudio::NoError), "error state is not equal to QAudio::NoError after start()");
        QVERIFY(audioInput.periodSize() > 0);
        stateSignal.clear();

        // Check that 'elapsed' increases
        QTest::qWait(40);
        QVERIFY2((audioInput.elapsedUSecs() > 0), "elapsedUSecs() is still zero after start()");

        qint64 totalBytesRead = 0;
        bool firstBuffer = true;
        char buffer[AUDIO_BUFFER];
        qint64 len = (testFormats.at(i).frequency()*testFormats.at(i).channels()*(testFormats.at(i).sampleSize()/8)*2); // 2 seconds
        while (totalBytesRead < len) {
            if (audioInput.bytesReady() > audioInput.periodSize()) {
                qint64 bytesRead = feed->read(buffer, audioInput.periodSize());
                audioFiles.at(i)->write(buffer,bytesRead);
                totalBytesRead+=bytesRead;
                if (firstBuffer && bytesRead) {
                    // Check for transition to ActiveState when data is provided
                    QVERIFY2((stateSignal.count() == 1),"didn't emit ActiveState signal on data");
                    QVERIFY2((audioInput.state() == QAudio::ActiveState),
                            "didn't transition to ActiveState after data");
                    QVERIFY2((audioInput.error() == QAudio::NoError), "error state is not equal to QAudio::NoError after start()");
                    firstBuffer = false;
                }
            } else
                QTest::qWait(20);
        }

        QTest::qWait(1000);

        stateSignal.clear();

        qint64 processedUs = audioInput.processedUSecs();

        audioInput.stop();
        QTest::qWait(40);
        QVERIFY2((stateSignal.count() == 1),
            QString("didn't emit StoppedState signal after stop(), got %1 signals instead").arg(stateSignal.count()).toLocal8Bit().constData());
        QVERIFY2((audioInput.state() == QAudio::StoppedState), "didn't transitions to StoppedState after stop()");

        QVERIFY2((processedUs > 1800000 && processedUs < 2200000),
                QString("processedUSecs() doesn't fall in acceptable range, should be 2040000 (%1)").arg(processedUs).toLocal8Bit().constData());
        QVERIFY2((audioInput.error() == QAudio::NoError), "error() is not QAudio::NoError after stop()");
        QVERIFY2((audioInput.elapsedUSecs() == (qint64)0), "elapsedUSecs() not equal to zero in StoppedState");
        QVERIFY2((notifySignal.count() > 20 && notifySignal.count() < 40),
                QString("notify() signals emitted (%1) should be 30").arg(notifySignal.count()).toLocal8Bit().constData());

        WavHeader::writeDataLength(*audioFiles.at(i),audioFiles.at(i)->pos()-WavHeader::headerLength());
        audioFiles.at(i)->close();
    }
}

void tst_QAudioInput::pushSuspendResume()
{
    for(int i=0; i<audioFiles.count(); i++) {
        QAudioInput audioInput(testFormats.at(i), this);

        audioInput.setNotifyInterval(100);

        QSignalSpy notifySignal(&audioInput, SIGNAL(notify()));
        QSignalSpy stateSignal(&audioInput, SIGNAL(stateChanged(QAudio::State)));

        // Check that we are in the default state before calling start
        QVERIFY2((audioInput.state() == QAudio::StoppedState), "state() was not set to StoppedState before start()");
        QVERIFY2((audioInput.error() == QAudio::NoError), "error() was not set to QAudio::NoError before start()");
        QVERIFY2((audioInput.elapsedUSecs() == qint64(0)),"elapsedUSecs() not zero on creation");

        audioFiles.at(i)->close();
        audioFiles.at(i)->open(QIODevice::WriteOnly);
        WavHeader wavHeader(testFormats.at(i));
        QVERIFY(wavHeader.write(*audioFiles.at(i)));

        QIODevice* feed = audioInput.start();

        // Check that QAudioInput immediately transitions to IdleState
        QVERIFY2((stateSignal.count() == 1),"didn't emit IdleState signal on start()");
        QVERIFY2((audioInput.state() == QAudio::IdleState),
                "didn't transition to IdleState after start()");
        QVERIFY2((audioInput.error() == QAudio::NoError), "error state is not equal to QAudio::NoError after start()");
        QVERIFY(audioInput.periodSize() > 0);
        stateSignal.clear();

        // Check that 'elapsed' increases
        QTest::qWait(40);
        QVERIFY2((audioInput.elapsedUSecs() > 0), "elapsedUSecs() is still zero after start()");

        qint64 totalBytesRead = 0;
        bool firstBuffer = true;
        char buffer[AUDIO_BUFFER];
        qint64 len = (testFormats.at(i).frequency()*testFormats.at(i).channels()*(testFormats.at(i).sampleSize()/8)); // 1 seconds
        while (totalBytesRead < len) {
            if (audioInput.bytesReady() > audioInput.periodSize()) {
                qint64 bytesRead = feed->read(buffer, audioInput.periodSize());
                audioFiles.at(i)->write(buffer,bytesRead);
                totalBytesRead+=bytesRead;
                if (firstBuffer && bytesRead) {
                    // Check for transition to ActiveState when data is provided
                    QVERIFY2((stateSignal.count() == 1),"didn't emit ActiveState signal on data");
                    QVERIFY2((audioInput.state() == QAudio::ActiveState),
                            "didn't transition to ActiveState after data");
                    QVERIFY2((audioInput.error() == QAudio::NoError), "error state is not equal to QAudio::NoError after start()");
                    firstBuffer = false;
                }
            } else
                QTest::qWait(20);
        }
        stateSignal.clear();

        audioInput.suspend();

        // Give backends running in separate threads a chance to suspend
        QTest::qWait(100);

        QVERIFY2((stateSignal.count() == 1),
            QString("didn't emit SuspendedState signal after suspend(), got %1 signals instead").arg(stateSignal.count()).toLocal8Bit().constData());
        QVERIFY2((audioInput.state() == QAudio::SuspendedState), "didn't transitions to SuspendedState after stop()");
        QVERIFY2((audioInput.error() == QAudio::NoError), "error() is not QAudio::NoError after stop()");
        stateSignal.clear();

        // Check that only 'elapsed', and not 'processed' increases while suspended
        qint64 elapsedUs = audioInput.elapsedUSecs();
        qint64 processedUs = audioInput.processedUSecs();
        QTest::qWait(1000);
        QVERIFY(audioInput.elapsedUSecs() > elapsedUs);
        QVERIFY(audioInput.processedUSecs() == processedUs);

        audioInput.resume();

        // Give backends running in separate threads a chance to resume.
        QTest::qWait(100);

        // Check that QAudioInput immediately transitions to Active or IdleState
        QVERIFY2((stateSignal.count() > 0),"didn't emit signals on resume()");
        QVERIFY2((audioInput.state() == QAudio::ActiveState || audioInput.state() == QAudio::IdleState),
                "didn't transition to ActiveState or IdleState after resume()");
        QVERIFY2((audioInput.error() == QAudio::NoError), "error state is not equal to QAudio::NoError after resume()");
        QVERIFY(audioInput.periodSize() > 0);

        // Let it play out what is in buffer and go to Idle before continue
        QTest::qWait(1000);
        stateSignal.clear();

        // Read another seconds worth
        totalBytesRead = 0;
        firstBuffer = true;
        while (totalBytesRead < len) {
            if (audioInput.bytesReady() >= audioInput.periodSize()) {
                qint64 bytesRead = feed->read(buffer, audioInput.periodSize());
                audioFiles.at(i)->write(buffer,bytesRead);
                totalBytesRead+=bytesRead;
            } else
                QTest::qWait(20);
        }
        stateSignal.clear();

        processedUs = audioInput.processedUSecs();

        audioInput.stop();
        QTest::qWait(40);
        QVERIFY2((stateSignal.count() == 1),
            QString("didn't emit StoppedState signal after stop(), got %1 signals instead").arg(stateSignal.count()).toLocal8Bit().constData());
        QVERIFY2((audioInput.state() == QAudio::StoppedState), "didn't transitions to StoppedState after stop()");

        QVERIFY2((processedUs > 1800000 && processedUs < 2200000),
                QString("processedUSecs() doesn't fall in acceptable range, should be 2040000 (%1)").arg(processedUs).toLocal8Bit().constData());
        QVERIFY2((audioInput.elapsedUSecs() == (qint64)0), "elapsedUSecs() not equal to zero in StoppedState");

        WavHeader::writeDataLength(*audioFiles.at(i),audioFiles.at(i)->pos()-WavHeader::headerLength());
        audioFiles.at(i)->close();
    }
}

void tst_QAudioInput::cleanupTestCase()
{
    QFile* file;

    foreach (file, audioFiles) {
        file->remove();
        delete file;
    }
}

QTEST_MAIN(tst_QAudioInput)

#include "tst_qaudioinput.moc"
