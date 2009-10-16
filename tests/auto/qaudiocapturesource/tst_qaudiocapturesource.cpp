/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QDebug>
#include <QAudioFormat>

#include <multimedia/qaudiocapturesource.h>
#include <multimedia/qaudioencodercontrol.h>
#include <multimedia/qmediarecordercontrol.h>
#include <multimedia/qaudiodevicecontrol.h>

class MockAudioEncoderControl : public QAudioEncoderControl
{
    Q_OBJECT
public:
    MockAudioEncoderControl(QObject *parent = 0):
            QAudioEncoderControl(parent)
    {
        m_codecs << "audio/pcm" << "audio/mpeg";
        m_descriptions << "Pulse Code Modulation" << "mp3 format";
        m_codec = "audio/pcm";
        m_freq = 8000;
        m_freqs << 8000 << 11025 << 22050 << 44100;
        m_freqRange = qMakePair(8000,44100);
        m_channels = 1;
        m_sampleSize = 8;
    }

    ~MockAudioEncoderControl() {}

    QStringList supportedAudioCodecs() const { return m_codecs; }
    QString audioCodec() const { return m_codec; }
    bool setAudioCodec(const QString &codecName) { m_codec = codecName; return true; }
    QString codecDescription(const QString &codecName) const { return m_descriptions.at(m_codecs.indexOf(codecName)); }
    int bitrate() const { return 0; }
    void setBitrate(int) {}
    QtMedia::EncodingQuality quality() const { return QtMedia::NormalQuality; }
    void setQuality(QtMedia::EncodingQuality) {}
    QStringList supportedEncodingOptions(const QString &) const { return QStringList() << "bitrate"; }
    QVariant encodingOption(const QString &, const QString &) const { return m_optionValue; }
    void setEncodingOption(const QString &, const QString &, const QVariant &value) { m_optionValue = value; }
    int sampleRate() const { return m_freq; }
    void setSampleRate(int sampleRate) { m_freq = sampleRate; }
    QList<int> supportedSampleRates() const { return m_freqs; }
    int channels() const { return m_channels; }
    void setChannels(int channels) { m_channels = channels; }
    QList<int> supportedChannelCounts() const { QList<int> list; list << 1 << 2; return list; }
    int sampleSize() const { return m_sampleSize; }
    void setSampleSize(int sampleSize) { m_sampleSize = sampleSize; }
    QList<int> supportedSampleSizes() const { QList<int> list; list << 8 << 16; return list; }

    QStringList m_codecs;
    QStringList m_descriptions;
    QString m_codec;
    int m_freq;
    QList<int> m_freqs;
    QPair<int,int> m_freqRange;
    int m_channels;
    int m_sampleSize;
    QVariant m_optionValue;
};

class MockMediaRecorderControl : public QMediaRecorderControl
{
    Q_OBJECT
public:
    MockMediaRecorderControl(QObject *parent = 0):
            QMediaRecorderControl(parent),
            m_state(QMediaRecorder::StoppedState),
            m_position(0) {}

    ~MockMediaRecorderControl() {}

    QUrl sink() const { return m_sink; }
    bool setSink(const QUrl &sink) { m_sink = sink; return true; }
    QMediaRecorder::State state() const { return m_state; }
    qint64 duration() const { return m_position; }
    void applySettings() {}

public slots:
    void record()
    {
        m_state = QMediaRecorder::RecordingState;
        m_position=1;
        emit stateChanged(m_state);
        emit durationChanged(m_position);
    }
    void pause()
    {
        m_state = QMediaRecorder::PausedState;
        emit stateChanged(m_state);
    }

    void stop()
    {
        m_position=0;
        m_state = QMediaRecorder::StoppedState;
        emit stateChanged(m_state);
    }

public:
    QUrl       m_sink;
    QMediaRecorder::State m_state;
    qint64     m_position;
};

class MockAudioDeviceControl : public QAudioDeviceControl
{
    Q_OBJECT
public:
    MockAudioDeviceControl(QObject *parent):
        QAudioDeviceControl(parent)
    {
        m_names << "device1" << "device2" << "device3";
        m_descriptions << "dev1 comment" << "dev2 comment" << "dev3 comment";
        m_index = 0;
        emit devicesChanged();
    }
    ~MockAudioDeviceControl() {};

    int deviceCount() const
    {
        return m_names.count();
    }

    QString name(int index) const
    {
        return m_names[index];
    }

    QString description(int index) const
    {
        return m_descriptions[index];
    }
    QIcon icon(int index) const
    {
        Q_UNUSED(index)
        return QIcon();
    }

    int defaultDevice() const
    {
        return 1;
    }

    int selectedDevice() const
    {
        return m_index;
    }

public Q_SLOTS:
    void setSelectedDevice(int index)
    {
        m_index = index;
        emit selectedDeviceChanged(m_index);
        emit selectedDeviceChanged(m_names[m_index]);
        emit devicesChanged();
    }

private:
    int m_index;
    QStringList m_names;
    QStringList m_descriptions;
};


class MockAudioSourceService : public QMediaService
{
    Q_OBJECT

public:
    MockAudioSourceService(): QMediaService(0), hasAudioDeviceControl(true)
    {
        mockAudioEncoderControl = new MockAudioEncoderControl(this);
        mockMediaRecorderControl = new MockMediaRecorderControl(this);
        mockAudioDeviceControl = new MockAudioDeviceControl(this);
    }

    ~MockAudioSourceService()
    {
        delete mockAudioEncoderControl;
        delete mockMediaRecorderControl;
        delete mockAudioDeviceControl;
    }

    QMediaControl* control(const char *iid) const
    {
        if (qstrcmp(iid, QAudioEncoderControl_iid) == 0)
            return mockAudioEncoderControl;

        if (qstrcmp(iid, QMediaRecorderControl_iid) == 0)
            return mockMediaRecorderControl;

        if (hasAudioDeviceControl && qstrcmp(iid, QAudioDeviceControl_iid) == 0)
            return mockAudioDeviceControl;

        return 0;
    }

    MockAudioEncoderControl *mockAudioEncoderControl;
    MockMediaRecorderControl *mockMediaRecorderControl;
    MockAudioDeviceControl *mockAudioDeviceControl;
    bool hasAudioDeviceControl;
};

class MockProvider : public QMediaServiceProvider
{
public:
    MockProvider(MockAudioSourceService *service):mockService(service) {}
    QMediaService *requestService(const QByteArray&, const QMediaServiceProviderHint &)
    {
        return mockService;
    }

    void releaseService(QMediaService *) {}

    MockAudioSourceService *mockService;
};


class tst_QAudioCaptureSource: public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();

private slots:
    void testNullService();
    void testNullControl();
    void testAudioSource();
    void testOptions();
    void testDevices();

private:
    QAudioCaptureSource *audiosource;
    MockAudioSourceService  *mockAudioSourceService;
    MockProvider *mockProvider;
};

void tst_QAudioCaptureSource::initTestCase()
{
    mockAudioSourceService = new MockAudioSourceService;
    mockProvider = new MockProvider(mockAudioSourceService);
}

void tst_QAudioCaptureSource::cleanupTestCase()
{
    delete audiosource;
    delete mockProvider;
}

void tst_QAudioCaptureSource::testNullService()
{
    MockProvider provider(0);
    QAudioCaptureSource source(0, &provider);

    QCOMPARE(source.deviceCount(), 0);
    QCOMPARE(source.defaultDevice(), 0);
    QCOMPARE(source.selectedDevice(), 0);
}

void tst_QAudioCaptureSource::testNullControl()
{
    MockAudioSourceService service;
    service.hasAudioDeviceControl = false;
    MockProvider provider(&service);
    QAudioCaptureSource source(0, &provider);

    QCOMPARE(source.deviceCount(), 0);
    QCOMPARE(source.defaultDevice(), 0);
    QCOMPARE(source.selectedDevice(), 0);

    QCOMPARE(source.name(0), QString());
    QCOMPARE(source.name(-3), QString());
    QCOMPARE(source.name(93), QString());
    QCOMPARE(source.icon(0), QIcon());
    QCOMPARE(source.icon(-3), QIcon());
    QCOMPARE(source.icon(93), QIcon());
    QCOMPARE(source.description(0), QString());
    QCOMPARE(source.description(-3), QString());
    QCOMPARE(source.description(93), QString());

    QSignalSpy deviceIndexSpy(&source, SIGNAL(selectedDeviceChanged(int)));
    QSignalSpy deviceNameSpy(&source, SIGNAL(selectedDeviceChanged(QString)));

    source.setSelectedDevice(0);
    QCOMPARE(deviceIndexSpy.count(), 0);
    QCOMPARE(deviceNameSpy.count(), 0);

    source.setSelectedDevice(93);
    QCOMPARE(source.selectedDevice(), 0);
    QCOMPARE(deviceIndexSpy.count(), 0);
    QCOMPARE(deviceNameSpy.count(), 0);

    source.setSelectedDevice(-3);
    QCOMPARE(source.selectedDevice(), 0);
    QCOMPARE(deviceIndexSpy.count(), 0);
    QCOMPARE(deviceNameSpy.count(), 0);
}

void tst_QAudioCaptureSource::testAudioSource()
{
    audiosource = new QAudioCaptureSource(0, mockProvider);

    QCOMPARE(audiosource->service(), mockAudioSourceService);
}

void tst_QAudioCaptureSource::testOptions()
{
    const QString codec(QLatin1String("mp3"));

    QStringList options = mockAudioSourceService->mockAudioEncoderControl->supportedEncodingOptions(codec);
    QVERIFY(options.count() == 1);
    mockAudioSourceService->mockAudioEncoderControl->setEncodingOption(codec, options.first(),8000);
    QVERIFY(mockAudioSourceService->mockAudioEncoderControl->encodingOption(codec, options.first()).toInt() == 8000);
}

void tst_QAudioCaptureSource::testDevices()
{
    int devices = audiosource->deviceCount();
    QVERIFY(devices > 0);
    QVERIFY(audiosource->name(0).compare("device1") == 0);
    QVERIFY(audiosource->description(0).compare("dev1 comment") == 0);
    QVERIFY(audiosource->icon(0).isNull());
    QVERIFY(audiosource->defaultDevice() == 1);

    QSignalSpy checkSignal(audiosource, SIGNAL(selectedDeviceChanged(int)));
    audiosource->setSelectedDevice(2);
    QVERIFY(audiosource->selectedDevice() == 2);
    QVERIFY(checkSignal.count() == 1);
}

QTEST_MAIN(tst_QAudioCaptureSource)

#include "tst_qaudiocapturesource.moc"
