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

#include <qaudiooutput.h>
#include <qaudiodeviceinfo.h>
#include <qaudioformat.h>
#include <qaudio.h>

#include "wavheader.h"

#define AUDIO_BUFFER 192000

#if defined(Q_OS_SYMBIAN)
#define SRCDIR ""
#endif

class tst_QAudioOutput : public QObject
{
    Q_OBJECT
public:
    tst_QAudioOutput(QObject* parent=0) : QObject(parent) {}

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
    void pushUnderrun();

    void cleanupTestCase();

private:
    QString formatToFileName(const QAudioFormat &format);
    QString workingDir();
    void createSineWaveData(const QAudioFormat &format, qint64 length, int frequency = 440);

    QAudioDeviceInfo audioDevice;
    QList<QAudioFormat> testFormats;
    QList<QFile*> audioFiles;

    QScopedPointer<QByteArray> m_byteArray;
    QScopedPointer<QBuffer> m_buffer;
};

QString tst_QAudioOutput::formatToFileName(const QAudioFormat &format)
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


QString tst_QAudioOutput::workingDir()
{
    QDir working(QString(SRCDIR));

    if (working.exists())
        return QString(SRCDIR);

    return QDir::currentPath();
}

void tst_QAudioOutput::createSineWaveData(const QAudioFormat &format, qint64 length, int frequency)
{
    const int channelBytes = format.sampleSize() / 8;
    const int sampleBytes = format.channels() * channelBytes;

    Q_ASSERT(length % sampleBytes == 0);
    Q_UNUSED(sampleBytes) // suppress warning in release builds

    m_byteArray.reset(new QByteArray(length, 0));
    unsigned char *ptr = reinterpret_cast<unsigned char *>(m_byteArray->data());
    int sampleIndex = 0;

    while (length) {
        const qreal x = qSin(2 * M_PI * frequency * qreal(sampleIndex % format.frequency()) / format.frequency());
        for (int i=0; i<format.channels(); ++i) {
            if (format.sampleSize() == 8 && format.sampleType() == QAudioFormat::UnSignedInt) {
                const quint8 value = static_cast<quint8>((1.0 + x) / 2 * 255);
                *reinterpret_cast<quint8*>(ptr) = value;
            } else if (format.sampleSize() == 8 && format.sampleType() == QAudioFormat::SignedInt) {
                const qint8 value = static_cast<qint8>(x * 127);
                *reinterpret_cast<quint8*>(ptr) = value;
            } else if (format.sampleSize() == 16 && format.sampleType() == QAudioFormat::UnSignedInt) {
                quint16 value = static_cast<quint16>((1.0 + x) / 2 * 65535);
                if (format.byteOrder() == QAudioFormat::LittleEndian)
                    qToLittleEndian<quint16>(value, ptr);
                else
                    qToBigEndian<quint16>(value, ptr);
            } else if (format.sampleSize() == 16 && format.sampleType() == QAudioFormat::SignedInt) {
                qint16 value = static_cast<qint16>(x * 32767);
                if (format.byteOrder() == QAudioFormat::LittleEndian)
                    qToLittleEndian<qint16>(value, ptr);
                else
                    qToBigEndian<qint16>(value, ptr);
            }

            ptr += channelBytes;
            length -= channelBytes;
        }
        ++sampleIndex;
    }

    m_buffer.reset(new QBuffer(m_byteArray.data(), this));
    Q_ASSERT(m_buffer->open(QIODevice::ReadOnly));
}

void tst_QAudioOutput::initTestCase()
{
    // Only perform tests if audio output device exists
    const QList<QAudioDeviceInfo> devices =
        QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    QVERIFY(devices.size() > 0);

    audioDevice = QAudioDeviceInfo::defaultOutputDevice();


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
        qint64 len = (format.frequency()*format.channels()*(format.sampleSize()/8)*2); // 2 seconds
        createSineWaveData(format, len);
        // Write generate sine wave data to file
        QFile* file = new QFile(workingDir() + QString("generated") + formatToFileName(format) + QString(".wav"));
        if (file->open(QIODevice::WriteOnly)) {
            WavHeader wavHeader(format, len);
            wavHeader.write(*file);
            file->write(m_byteArray->data(), len);
            file->close();
            audioFiles.append(file);
        }
    }
}

void tst_QAudioOutput::format()
{
    QAudioOutput audioOutput(audioDevice.preferredFormat(), this);

    QAudioFormat requested = audioDevice.preferredFormat();
    QAudioFormat actual    = audioOutput.format();

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

void tst_QAudioOutput::invalidFormat()
{
    QAudioFormat invalidFormat;
    invalidFormat.setFrequency(0);

    QVERIFY2(!audioDevice.isFormatSupported(invalidFormat),
            "isFormatSupported() is returning true on an invalid format");

    QAudioOutput audioOutput(invalidFormat, this);

    // Check that we are in the default state before calling start
    QVERIFY2((audioOutput.state() == QAudio::StoppedState), "state() was not set to StoppedState before start()");
    QVERIFY2((audioOutput.error() == QAudio::NoError), "error() was not set to QAudio::NoError before start()");

    audioOutput.start();

    // Check that error is raised
    QVERIFY2((audioOutput.error() == QAudio::OpenError),"error() was not set to QAudio::OpenError after start()");
}

void tst_QAudioOutput::bufferSize()
{
    QAudioOutput audioOutput(audioDevice.preferredFormat(), this);

    QVERIFY2((audioOutput.error() == QAudio::NoError), "error() was not set to QAudio::NoError on creation");

    audioOutput.setBufferSize(512);
    QVERIFY2((audioOutput.error() == QAudio::NoError), "error() is not QAudio::NoError after setBufferSize(512)");
    QVERIFY2((audioOutput.bufferSize() == 512),
            QString("bufferSize: requested=512, actual=%2").arg(audioOutput.bufferSize()).toLocal8Bit().constData());

    audioOutput.setBufferSize(4096);
    QVERIFY2((audioOutput.error() == QAudio::NoError), "error() is not QAudio::NoError after setBufferSize(4096)");
    QVERIFY2((audioOutput.bufferSize() == 4096),
            QString("bufferSize: requested=4096, actual=%2").arg(audioOutput.bufferSize()).toLocal8Bit().constData());

    audioOutput.setBufferSize(8192);
    QVERIFY2((audioOutput.error() == QAudio::NoError), "error() is not QAudio::NoError after setBufferSize(8192)");
    QVERIFY2((audioOutput.bufferSize() == 8192),
            QString("bufferSize: requested=8192, actual=%2").arg(audioOutput.bufferSize()).toLocal8Bit().constData());
}

void tst_QAudioOutput::notifyInterval()
{
    QAudioOutput audioOutput(audioDevice.preferredFormat(), this);

    QVERIFY2((audioOutput.error() == QAudio::NoError), "error() was not set to QAudio::NoError on creation");

    audioOutput.setNotifyInterval(50);
    QVERIFY2((audioOutput.error() == QAudio::NoError), "error() is not QAudio::NoError after setNotifyInterval(50)");
    QVERIFY2((audioOutput.notifyInterval() == 50),
            QString("notifyInterval: requested=50, actual=%2").arg(audioOutput.notifyInterval()).toLocal8Bit().constData());

    audioOutput.setNotifyInterval(100);
    QVERIFY2((audioOutput.error() == QAudio::NoError), "error() is not QAudio::NoError after setNotifyInterval(100)");
    QVERIFY2((audioOutput.notifyInterval() == 100),
            QString("notifyInterval: requested=100, actual=%2").arg(audioOutput.notifyInterval()).toLocal8Bit().constData());

    audioOutput.setNotifyInterval(250);
    QVERIFY2((audioOutput.error() == QAudio::NoError), "error() is not QAudio::NoError after setNotifyInterval(250)");
    QVERIFY2((audioOutput.notifyInterval() == 250),
            QString("notifyInterval: requested=250, actual=%2").arg(audioOutput.notifyInterval()).toLocal8Bit().constData());

    audioOutput.setNotifyInterval(1000);
    QVERIFY2((audioOutput.error() == QAudio::NoError), "error() is not QAudio::NoError after setNotifyInterval(1000)");
    QVERIFY2((audioOutput.notifyInterval() == 1000),
            QString("notifyInterval: requested=1000, actual=%2").arg(audioOutput.notifyInterval()).toLocal8Bit().constData());
}

void tst_QAudioOutput::disableNotifyInterval()
{
    // Sets an invalid notification interval (QAudioOutput::setNotifyInterval(0))
    // Checks that
    //  - No error is raised (QAudioOutput::error() returns QAudio::NoError)
    //  - if <= 0, set to zero and disable notify signal

    QAudioOutput audioOutput(audioDevice.preferredFormat(), this);

    QVERIFY2((audioOutput.error() == QAudio::NoError), "error() was not set to QAudio::NoError on creation");

    audioOutput.setNotifyInterval(0);
    QVERIFY2((audioOutput.error() == QAudio::NoError), "error() is not QAudio::NoError after setNotifyInterval(0)");
    QVERIFY2((audioOutput.notifyInterval() == 0),
            "notifyInterval() is not zero after setNotifyInterval(0)");

    audioOutput.setNotifyInterval(-1);
    QVERIFY2((audioOutput.error() == QAudio::NoError), "error() is not QAudio::NoError after setNotifyInterval(-1)");
    QVERIFY2((audioOutput.notifyInterval() == 0),
            "notifyInterval() is not zero after setNotifyInterval(-1)");

    //start and run to check if notify() is emitted
    if (audioFiles.size() > 0) {
        QAudioOutput audioOutputCheck(testFormats.at(0), this);
        audioOutputCheck.setNotifyInterval(0);
        QSignalSpy notifySignal(&audioOutputCheck, SIGNAL(notify()));
        audioFiles.at(0)->open(QIODevice::ReadOnly);
        audioOutputCheck.start(audioFiles.at(0));
        QTest::qWait(3000); // 3 seconds should be plenty
        audioOutputCheck.stop();
        QVERIFY2((notifySignal.count() == 0),
                QString("didn't disable notify interval: shouldn't have got any but got %1").arg(notifySignal.count()).toLocal8Bit().constData());
        audioFiles.at(0)->close();
    }
}

void tst_QAudioOutput::stopWhileStopped()
{
    // Calls QAudioOutput::stop() when object is already in StoppedState
    // Checks that
    //  - No state change occurs
    //  - No error is raised (QAudioOutput::error() returns QAudio::NoError)

    QAudioOutput audioOutput(audioDevice.preferredFormat(), this);

    QVERIFY2((audioOutput.state() == QAudio::StoppedState), "state() was not set to StoppedState before start()");
    QVERIFY2((audioOutput.error() == QAudio::NoError), "error() was not set to QAudio::NoError before start()");

    QSignalSpy stateSignal(&audioOutput, SIGNAL(stateChanged(QAudio::State)));
    audioOutput.stop();

    // Check that no state transition occurred
    QVERIFY2((stateSignal.count() == 0), "stop() while stopped is emitting a signal and it shouldn't");
    QVERIFY2((audioOutput.error() == QAudio::NoError), "error() was not set to QAudio::NoError after stop()");
}

void tst_QAudioOutput::suspendWhileStopped()
{
    // Calls QAudioOutput::suspend() when object is already in StoppedState
    // Checks that
    //  - No state change occurs
    //  - No error is raised (QAudioOutput::error() returns QAudio::NoError)

    QAudioOutput audioOutput(audioDevice.preferredFormat(), this);

    QVERIFY2((audioOutput.state() == QAudio::StoppedState), "state() was not set to StoppedState before start()");
    QVERIFY2((audioOutput.error() == QAudio::NoError), "error() was not set to QAudio::NoError before start()");

    QSignalSpy stateSignal(&audioOutput, SIGNAL(stateChanged(QAudio::State)));
    audioOutput.suspend();

    // Check that no state transition occurred
    QVERIFY2((stateSignal.count() == 0), "stop() while suspended is emitting a signal and it shouldn't");
    QVERIFY2((audioOutput.error() == QAudio::NoError), "error() was not set to QAudio::NoError after stop()");
}

void tst_QAudioOutput::resumeWhileStopped()
{
    // Calls QAudioOutput::resume() when object is already in StoppedState
    // Checks that
    //  - No state change occurs
    //  - No error is raised (QAudioOutput::error() returns QAudio::NoError)

    QAudioOutput audioOutput(audioDevice.preferredFormat(), this);

    QVERIFY2((audioOutput.state() == QAudio::StoppedState), "state() was not set to StoppedState before start()");
    QVERIFY2((audioOutput.error() == QAudio::NoError), "error() was not set to QAudio::NoError before start()");

    QSignalSpy stateSignal(&audioOutput, SIGNAL(stateChanged(QAudio::State)));
    audioOutput.resume();

    // Check that no state transition occurred
    QVERIFY2((stateSignal.count() == 0), "resume() while stopped is emitting a signal and it shouldn't");
    QVERIFY2((audioOutput.error() == QAudio::NoError), "error() was not set to QAudio::NoError after resume()");
}

void tst_QAudioOutput::pull()
{
    for(int i=0; i<audioFiles.count(); i++) {
        QAudioOutput audioOutput(testFormats.at(i), this);

        audioOutput.setNotifyInterval(100);

        QSignalSpy notifySignal(&audioOutput, SIGNAL(notify()));
        QSignalSpy stateSignal(&audioOutput, SIGNAL(stateChanged(QAudio::State)));

        // Check that we are in the default state before calling start
        QVERIFY2((audioOutput.state() == QAudio::StoppedState), "state() was not set to StoppedState before start()");
        QVERIFY2((audioOutput.error() == QAudio::NoError), "error() was not set to QAudio::NoError before start()");
        QVERIFY2((audioOutput.elapsedUSecs() == qint64(0)),"elapsedUSecs() not zero on creation");

        audioFiles.at(i)->close();
        audioFiles.at(i)->open(QIODevice::ReadOnly);
        audioFiles.at(i)->seek(WavHeader::headerLength());

        audioOutput.start(audioFiles.at(i));

        // Check that QAudioOutput immediately transitions to ActiveState
        QVERIFY2((stateSignal.count() == 1),
                QString("didn't emit signal on start(), got %1 signals instead").arg(stateSignal.count()).toLocal8Bit().constData());
        QVERIFY2((audioOutput.state() == QAudio::ActiveState), "didn't transition to ActiveState after start()");
        QVERIFY2((audioOutput.error() == QAudio::NoError), "error state is not equal to QAudio::NoError after start()");
        QVERIFY(audioOutput.periodSize() > 0);
        stateSignal.clear();

        // Check that 'elapsed' increases
        QTest::qWait(40);
        QVERIFY2((audioOutput.elapsedUSecs() > 0), "elapsedUSecs() is still zero after start()");

        // Wait until playback finishes
        QTest::qWait(3000); // 3 seconds should be plenty

        QVERIFY2(audioFiles.at(i)->atEnd(), "didn't play to EOF");
        QVERIFY2((stateSignal.count() == 1),
            QString("didn't emit IdleState signal when at EOF, got %1 signals instead").arg(stateSignal.count()).toLocal8Bit().constData());
        QVERIFY2((audioOutput.state() == QAudio::IdleState), "didn't transitions to IdleState when at EOF");
        stateSignal.clear();

        qint64 processedUs = audioOutput.processedUSecs();

        audioOutput.stop();
        QTest::qWait(40);
        QVERIFY2((stateSignal.count() == 1),
            QString("didn't emit StoppedState signal after stop(), got %1 signals instead").arg(stateSignal.count()).toLocal8Bit().constData());
        QVERIFY2((audioOutput.state() == QAudio::StoppedState), "didn't transitions to StoppedState after stop()");

        QVERIFY2((processedUs == 2000000),
                QString("processedUSecs() doesn't equal file duration in us (%1)").arg(processedUs).toLocal8Bit().constData());
        QVERIFY2((audioOutput.error() == QAudio::NoError), "error() is not QAudio::NoError after stop()");
        QVERIFY2((audioOutput.elapsedUSecs() == (qint64)0), "elapsedUSecs() not equal to zero in StoppedState");
        QVERIFY2((notifySignal.count() > 15 && notifySignal.count() < 25),
                QString("too many notify() signals emitted (%1)").arg(notifySignal.count()).toLocal8Bit().constData());

        audioFiles.at(i)->close();
    }
}

void tst_QAudioOutput::pullSuspendResume()
{
    for(int i=0; i<audioFiles.count(); i++) {
        QAudioOutput audioOutput(testFormats.at(i), this);

        audioOutput.setNotifyInterval(100);

        QSignalSpy notifySignal(&audioOutput, SIGNAL(notify()));
        QSignalSpy stateSignal(&audioOutput, SIGNAL(stateChanged(QAudio::State)));

        // Check that we are in the default state before calling start
        QVERIFY2((audioOutput.state() == QAudio::StoppedState), "state() was not set to StoppedState before start()");
        QVERIFY2((audioOutput.error() == QAudio::NoError), "error() was not set to QAudio::NoError before start()");
        QVERIFY2((audioOutput.elapsedUSecs() == qint64(0)),"elapsedUSecs() not zero on creation");

        audioFiles.at(i)->close();
        audioFiles.at(i)->open(QIODevice::ReadOnly);
        audioFiles.at(i)->seek(WavHeader::headerLength());

        audioOutput.start(audioFiles.at(i));

        // Check that QAudioOutput immediately transitions to ActiveState
        QVERIFY2((stateSignal.count() == 1),
                QString("didn't emit signal on start(), got %1 signals instead").arg(stateSignal.count()).toLocal8Bit().constData());
        QVERIFY2((audioOutput.state() == QAudio::ActiveState), "didn't transition to ActiveState after start()");
        QVERIFY2((audioOutput.error() == QAudio::NoError), "error state is not equal to QAudio::NoError after start()");
        QVERIFY(audioOutput.periodSize() > 0);
        stateSignal.clear();

        // Wait for half of clip to play
        QTest::qWait(1000);

        audioOutput.suspend();

        // Give backends running in separate threads a chance to suspend.
        QTest::qWait(100);

        QVERIFY2((stateSignal.count() == 1),
                QString("didn't emit SuspendedState signal after suspend(), got %1 signals instead")
                .arg(stateSignal.count()).toLocal8Bit().constData());
        QVERIFY2((audioOutput.state() == QAudio::SuspendedState), "didn't transition to SuspendedState after suspend()");
        QVERIFY2((audioOutput.error() == QAudio::NoError), "error state is not equal to QAudio::NoError after suspend()");
        stateSignal.clear();

        // Check that only 'elapsed', and not 'processed' increases while suspended
        qint64 elapsedUs = audioOutput.elapsedUSecs();
        qint64 processedUs = audioOutput.processedUSecs();
        QTest::qWait(1000);
        QVERIFY(audioOutput.elapsedUSecs() > elapsedUs);
        QVERIFY(audioOutput.processedUSecs() == processedUs);

        audioOutput.resume();

        // Give backends running in separate threads a chance to suspend.
        QTest::qWait(100);

        // Check that QAudioOutput immediately transitions to ActiveState
        QVERIFY2((stateSignal.count() == 1),
                QString("didn't emit signal after resume(), got %1 signals instead").arg(stateSignal.count()).toLocal8Bit().constData());
        QVERIFY2((audioOutput.state() == QAudio::ActiveState), "didn't transition to ActiveState after resume()");
        QVERIFY2((audioOutput.error() == QAudio::NoError), "error state is not equal to QAudio::NoError after resume()");
        stateSignal.clear();

        // Wait until playback finishes
        QTest::qWait(3000); // 3 seconds should be plenty

        QVERIFY2(audioFiles.at(i)->atEnd(), "didn't play to EOF");
        QVERIFY2((stateSignal.count() == 1),
            QString("didn't emit IdleState signal when at EOF, got %1 signals instead").arg(stateSignal.count()).toLocal8Bit().constData());
        QVERIFY2((audioOutput.state() == QAudio::IdleState), "didn't transitions to IdleState when at EOF");
        stateSignal.clear();

        processedUs = audioOutput.processedUSecs();

        audioOutput.stop();
        QTest::qWait(40);
        QVERIFY2((stateSignal.count() == 1),
            QString("didn't emit StoppedState signal after stop(), got %1 signals instead").arg(stateSignal.count()).toLocal8Bit().constData());
        QVERIFY2((audioOutput.state() == QAudio::StoppedState), "didn't transitions to StoppedState after stop()");

        QVERIFY2((processedUs == 2000000),
                QString("processedUSecs() doesn't equal file duration in us (%1)").arg(processedUs).toLocal8Bit().constData());
        QVERIFY2((audioOutput.error() == QAudio::NoError), "error() is not QAudio::NoError after stop()");
        QVERIFY2((audioOutput.elapsedUSecs() == (qint64)0), "elapsedUSecs() not equal to zero in StoppedState");

        audioFiles.at(i)->close();
    }
}

void tst_QAudioOutput::push()
{
    for(int i=0; i<audioFiles.count(); i++) {
        QAudioOutput audioOutput(testFormats.at(i), this);

        audioOutput.setNotifyInterval(100);

        QSignalSpy notifySignal(&audioOutput, SIGNAL(notify()));
        QSignalSpy stateSignal(&audioOutput, SIGNAL(stateChanged(QAudio::State)));

        // Check that we are in the default state before calling start
        QVERIFY2((audioOutput.state() == QAudio::StoppedState), "state() was not set to StoppedState before start()");
        QVERIFY2((audioOutput.error() == QAudio::NoError), "error() was not set to QAudio::NoError before start()");
        QVERIFY2((audioOutput.elapsedUSecs() == qint64(0)),"elapsedUSecs() not zero on creation");

        audioFiles.at(i)->close();
        audioFiles.at(i)->open(QIODevice::ReadOnly);
        audioFiles.at(i)->seek(WavHeader::headerLength());

        QIODevice* feed = audioOutput.start();

        // Check that QAudioOutput immediately transitions to IdleState
        QVERIFY2((stateSignal.count() == 1),
                QString("didn't emit signal on start(), got %1 signals instead").arg(stateSignal.count()).toLocal8Bit().constData());
        QVERIFY2((audioOutput.state() == QAudio::IdleState), "didn't transition to IdleState after start()");
        QVERIFY2((audioOutput.error() == QAudio::NoError), "error state is not equal to QAudio::NoError after start()");
        QVERIFY(audioOutput.periodSize() > 0);
        stateSignal.clear();

        // Check that 'elapsed' increases
        QTest::qWait(40);
        QVERIFY2((audioOutput.elapsedUSecs() > 0), "elapsedUSecs() is still zero after start()");
        QVERIFY2((audioOutput.processedUSecs() == qint64(0)), "processedUSecs() is not zero after start()");

        qint64 written = 0;
        bool firstBuffer = true;
        char buffer[AUDIO_BUFFER];

        while (written < audioFiles.at(i)->size()-WavHeader::headerLength()) {

            if (audioOutput.bytesFree() > audioOutput.periodSize()) {
                qint64 len = audioFiles.at(i)->read(buffer,audioOutput.periodSize());
                written += feed->write(buffer, len);

                if (firstBuffer) {
                    // Check for transition to ActiveState when data is provided
                    QVERIFY2((stateSignal.count() == 1),
                            QString("didn't emit signal after receiving data, got %1 signals instead")
                            .arg(stateSignal.count()).toLocal8Bit().constData());
                    QVERIFY2((audioOutput.state() == QAudio::ActiveState), "didn't transition to ActiveState after receiving data");
                    QVERIFY2((audioOutput.error() == QAudio::NoError), "error state is not equal to QAudio::NoError after receiving data");
                    firstBuffer = false;
                }
            } else
                QTest::qWait(20);
        }
        stateSignal.clear();

        // Wait until playback finishes
        QTest::qWait(3000); // 3 seconds should be plenty

        QVERIFY2(audioFiles.at(i)->atEnd(), "didn't play to EOF");
        QVERIFY2((stateSignal.count() == 1),
            QString("didn't emit IdleState signal when at EOF, got %1 signals instead").arg(stateSignal.count()).toLocal8Bit().constData());
        QVERIFY2((audioOutput.state() == QAudio::IdleState), "didn't transitions to IdleState when at EOF");
        stateSignal.clear();

        qint64 processedUs = audioOutput.processedUSecs();

        audioOutput.stop();
        QTest::qWait(40);
        QVERIFY2((stateSignal.count() == 1),
            QString("didn't emit StoppedState signal after stop(), got %1 signals instead").arg(stateSignal.count()).toLocal8Bit().constData());
        QVERIFY2((audioOutput.state() == QAudio::StoppedState), "didn't transitions to StoppedState after stop()");

        QVERIFY2((processedUs == 2000000),
                QString("processedUSecs() doesn't equal file duration in us (%1)").arg(processedUs).toLocal8Bit().constData());
        QVERIFY2((audioOutput.error() == QAudio::NoError), "error() is not QAudio::NoError after stop()");
        QVERIFY2((audioOutput.elapsedUSecs() == (qint64)0), "elapsedUSecs() not equal to zero in StoppedState");
        QVERIFY2((notifySignal.count() > 15 && notifySignal.count() < 25),
                QString("too many notify() signals emitted (%1)").arg(notifySignal.count()).toLocal8Bit().constData());

        audioFiles.at(i)->close();
    }
}

void tst_QAudioOutput::pushSuspendResume()
{
    for(int i=0; i<audioFiles.count(); i++) {
        QAudioOutput audioOutput(testFormats.at(i), this);

        audioOutput.setNotifyInterval(100);

        QSignalSpy notifySignal(&audioOutput, SIGNAL(notify()));
        QSignalSpy stateSignal(&audioOutput, SIGNAL(stateChanged(QAudio::State)));

        // Check that we are in the default state before calling start
        QVERIFY2((audioOutput.state() == QAudio::StoppedState), "state() was not set to StoppedState before start()");
        QVERIFY2((audioOutput.error() == QAudio::NoError), "error() was not set to QAudio::NoError before start()");
        QVERIFY2((audioOutput.elapsedUSecs() == qint64(0)),"elapsedUSecs() not zero on creation");

        audioFiles.at(i)->close();
        audioFiles.at(i)->open(QIODevice::ReadOnly);
        audioFiles.at(i)->seek(WavHeader::headerLength());

        QIODevice* feed = audioOutput.start();

        // Check that QAudioOutput immediately transitions to IdleState
        QVERIFY2((stateSignal.count() == 1),
                QString("didn't emit signal on start(), got %1 signals instead").arg(stateSignal.count()).toLocal8Bit().constData());
        QVERIFY2((audioOutput.state() == QAudio::IdleState), "didn't transition to IdleState after start()");
        QVERIFY2((audioOutput.error() == QAudio::NoError), "error state is not equal to QAudio::NoError after start()");
        QVERIFY(audioOutput.periodSize() > 0);
        stateSignal.clear();

        // Check that 'elapsed' increases
        QTest::qWait(40);
        QVERIFY2((audioOutput.elapsedUSecs() > 0), "elapsedUSecs() is still zero after start()");
        QVERIFY2((audioOutput.processedUSecs() == qint64(0)), "processedUSecs() is not zero after start()");

        qint64 written = 0;
        bool firstBuffer = true;
        char buffer[AUDIO_BUFFER];

        // Play half of the clip
        while (written < (audioFiles.at(i)->size()-WavHeader::headerLength())/2) {

            if (audioOutput.bytesFree() > audioOutput.periodSize()) {
                qint64 len = audioFiles.at(i)->read(buffer,audioOutput.periodSize());
                written += feed->write(buffer, len);

                if (firstBuffer) {
                    // Check for transition to ActiveState when data is provided
                    QVERIFY2((stateSignal.count() == 1),
                            QString("didn't emit signal after receiving data, got %1 signals instead")
                            .arg(stateSignal.count()).toLocal8Bit().constData());
                    QVERIFY2((audioOutput.state() == QAudio::ActiveState), "didn't transition to ActiveState after receiving data");
                    QVERIFY2((audioOutput.error() == QAudio::NoError), "error state is not equal to QAudio::NoError after receiving data");
                    firstBuffer = false;
                }
            } else
                QTest::qWait(20);
        }
        stateSignal.clear();

        audioOutput.suspend();

        // Give backends running in separate threads a chance to suspend.
        QTest::qWait(100);

        QVERIFY2((stateSignal.count() == 1),
                QString("didn't emit SuspendedState signal after suspend(), got %1 signals instead")
                .arg(stateSignal.count()).toLocal8Bit().constData());
        QVERIFY2((audioOutput.state() == QAudio::SuspendedState), "didn't transition to SuspendedState after suspend()");
        QVERIFY2((audioOutput.error() == QAudio::NoError), "error state is not equal to QAudio::NoError after suspend()");
        stateSignal.clear();

        // Check that only 'elapsed', and not 'processed' increases while suspended
        qint64 elapsedUs = audioOutput.elapsedUSecs();
        qint64 processedUs = audioOutput.processedUSecs();
        QTest::qWait(1000);
        QVERIFY(audioOutput.elapsedUSecs() > elapsedUs);
        QVERIFY(audioOutput.processedUSecs() == processedUs);

        audioOutput.resume();

        // Give backends running in separate threads a chance to suspend.
        QTest::qWait(100);

        // Check that QAudioOutput immediately transitions to ActiveState
        QVERIFY2((stateSignal.count() == 1),
                QString("didn't emit signal after resume(), got %1 signals instead").arg(stateSignal.count()).toLocal8Bit().constData());
        QVERIFY2((audioOutput.state() == QAudio::ActiveState), "didn't transition to ActiveState after resume()");
        QVERIFY2((audioOutput.error() == QAudio::NoError), "error state is not equal to QAudio::NoError after resume()");
        stateSignal.clear();

        // Play rest of the clip
        while (!audioFiles.at(i)->atEnd()) {
            if (audioOutput.bytesFree() >= audioOutput.periodSize()) {
                qint64 len = audioFiles.at(i)->read(buffer,audioOutput.periodSize());
                written += feed->write(buffer, len);
            } else
                QTest::qWait(20);
        }
        stateSignal.clear();

        // Wait until playback finishes
        QTest::qWait(1000); // 1 seconds should be plenty

        QVERIFY2(audioFiles.at(i)->atEnd(), "didn't play to EOF");
        QVERIFY2((stateSignal.count() == 1),
            QString("didn't emit IdleState signal when at EOF, got %1 signals instead").arg(stateSignal.count()).toLocal8Bit().constData());
        QVERIFY2((audioOutput.state() == QAudio::IdleState), "didn't transitions to IdleState when at EOF");
        stateSignal.clear();

        processedUs = audioOutput.processedUSecs();

        audioOutput.stop();
        QTest::qWait(40);
        QVERIFY2((stateSignal.count() == 1),
            QString("didn't emit StoppedState signal after stop(), got %1 signals instead").arg(stateSignal.count()).toLocal8Bit().constData());
        QVERIFY2((audioOutput.state() == QAudio::StoppedState), "didn't transitions to StoppedState after stop()");

        QVERIFY2((processedUs == 2000000),
                QString("processedUSecs() doesn't equal file duration in us (%1)").arg(processedUs).toLocal8Bit().constData());
        QVERIFY2((audioOutput.error() == QAudio::NoError), "error() is not QAudio::NoError after stop()");
        QVERIFY2((audioOutput.elapsedUSecs() == (qint64)0), "elapsedUSecs() not equal to zero in StoppedState");

        audioFiles.at(i)->close();
    }
}

void tst_QAudioOutput::pushUnderrun()
{
    for(int i=0; i<audioFiles.count(); i++) {
        QAudioOutput audioOutput(testFormats.at(i), this);

        audioOutput.setNotifyInterval(100);

        QSignalSpy notifySignal(&audioOutput, SIGNAL(notify()));
        QSignalSpy stateSignal(&audioOutput, SIGNAL(stateChanged(QAudio::State)));

        // Check that we are in the default state before calling start
        QVERIFY2((audioOutput.state() == QAudio::StoppedState), "state() was not set to StoppedState before start()");
        QVERIFY2((audioOutput.error() == QAudio::NoError), "error() was not set to QAudio::NoError before start()");
        QVERIFY2((audioOutput.elapsedUSecs() == qint64(0)),"elapsedUSecs() not zero on creation");

        audioFiles.at(i)->close();
        audioFiles.at(i)->open(QIODevice::ReadOnly);
        audioFiles.at(i)->seek(WavHeader::headerLength());

        QIODevice* feed = audioOutput.start();

        // Check that QAudioOutput immediately transitions to IdleState
        QVERIFY2((stateSignal.count() == 1),
                QString("didn't emit signal on start(), got %1 signals instead").arg(stateSignal.count()).toLocal8Bit().constData());
        QVERIFY2((audioOutput.state() == QAudio::IdleState), "didn't transition to IdleState after start()");
        QVERIFY2((audioOutput.error() == QAudio::NoError), "error state is not equal to QAudio::NoError after start()");
        QVERIFY(audioOutput.periodSize() > 0);
        stateSignal.clear();

        // Check that 'elapsed' increases
        QTest::qWait(40);
        QVERIFY2((audioOutput.elapsedUSecs() > 0), "elapsedUSecs() is still zero after start()");
        QVERIFY2((audioOutput.processedUSecs() == qint64(0)), "processedUSecs() is not zero after start()");

        qint64 written = 0;
        bool firstBuffer = true;
        char buffer[AUDIO_BUFFER];

        // Play half of the clip
        while (written < (audioFiles.at(i)->size()-WavHeader::headerLength())/2) {

            if (audioOutput.bytesFree() > audioOutput.periodSize()) {
                qint64 len = audioFiles.at(i)->read(buffer,audioOutput.periodSize());
                written += feed->write(buffer, len);

                if (firstBuffer) {
                    // Check for transition to ActiveState when data is provided
                    QVERIFY2((stateSignal.count() == 1),
                            QString("didn't emit signal after receiving data, got %1 signals instead")
                            .arg(stateSignal.count()).toLocal8Bit().constData());
                    QVERIFY2((audioOutput.state() == QAudio::ActiveState), "didn't transition to ActiveState after receiving data");
                    QVERIFY2((audioOutput.error() == QAudio::NoError), "error state is not equal to QAudio::NoError after receiving data");
                    firstBuffer = false;
                }
            } else
                QTest::qWait(20);
        }
        stateSignal.clear();

        // Wait for data to be played
        QTest::qWait(1000);

        QVERIFY2((stateSignal.count() == 1),
                QString("didn't emit IdleState signal after suspend(), got %1 signals instead")
                .arg(stateSignal.count()).toLocal8Bit().constData());
        QVERIFY2((audioOutput.state() == QAudio::IdleState), "didn't transition to IdleState, no data");
        QVERIFY2((audioOutput.error() == QAudio::UnderrunError), "error state is not equal to QAudio::UnderrunError, no data");
        stateSignal.clear();

        firstBuffer = true;
        // Play rest of the clip
        while (!audioFiles.at(i)->atEnd()) {
            if (audioOutput.bytesFree() >= audioOutput.periodSize()) {
                qint64 len = audioFiles.at(i)->read(buffer,audioOutput.periodSize());
                written += feed->write(buffer, len);
                if (firstBuffer) {
                    // Check for transition to ActiveState when data is provided
                    QVERIFY2((stateSignal.count() == 1),
                            QString("didn't emit signal after receiving data, got %1 signals instead")
                            .arg(stateSignal.count()).toLocal8Bit().constData());
                    QVERIFY2((audioOutput.state() == QAudio::ActiveState), "didn't transition to ActiveState after receiving data");
                    QVERIFY2((audioOutput.error() == QAudio::NoError), "error state is not equal to QAudio::NoError after receiving data");
                    firstBuffer = false;
                }
            } else
                QTest::qWait(20);
        }
        stateSignal.clear();

        // Wait until playback finishes
        QTest::qWait(1000); // 1 seconds should be plenty

        QVERIFY2(audioFiles.at(i)->atEnd(), "didn't play to EOF");
        QVERIFY2((stateSignal.count() == 1),
            QString("didn't emit IdleState signal when at EOF, got %1 signals instead").arg(stateSignal.count()).toLocal8Bit().constData());
        QVERIFY2((audioOutput.state() == QAudio::IdleState), "didn't transitions to IdleState when at EOF");
        stateSignal.clear();

        qint64 processedUs = audioOutput.processedUSecs();

        audioOutput.stop();
        QTest::qWait(40);
        QVERIFY2((stateSignal.count() == 1),
            QString("didn't emit StoppedState signal after stop(), got %1 signals instead").arg(stateSignal.count()).toLocal8Bit().constData());
        QVERIFY2((audioOutput.state() == QAudio::StoppedState), "didn't transitions to StoppedState after stop()");

        QVERIFY2((processedUs == 2000000),
                QString("processedUSecs() doesn't equal file duration in us (%1)").arg(processedUs).toLocal8Bit().constData());
        QVERIFY2((audioOutput.error() == QAudio::NoError), "error() is not QAudio::NoError after stop()");
        QVERIFY2((audioOutput.elapsedUSecs() == (qint64)0), "elapsedUSecs() not equal to zero in StoppedState");

        audioFiles.at(i)->close();
    }
}

void tst_QAudioOutput::cleanupTestCase()
{
    QFile* file;

    foreach (file, audioFiles) {
        file->remove();
        delete file;
    }
}

QTEST_MAIN(tst_QAudioOutput)

#include "tst_qaudiooutput.moc"
