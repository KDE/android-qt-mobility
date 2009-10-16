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
#include <multimedia/qmediaobject.h>
#include <multimedia/qmediacontrol.h>
#include <multimedia/qmediaservice.h>
#include <multimedia/qmediarecordercontrol.h>
#include <multimedia/qmediarecorder.h>
#include <multimedia/qaudiodevicecontrol.h>
#include <multimedia/qaudioencodercontrol.h>
#include <multimedia/qmediaformatcontrol.h>
#include <multimedia/qvideoencodercontrol.h>

#ifndef QT_NO_MULTIMEDIA
#include <QtMultimedia/qaudioformat.h>
#else
#include <multimedia/qaudioformat.h>
#endif

class MockMediaFormatControl : public QMediaFormatControl
{
    Q_OBJECT
public:
    MockMediaFormatControl(QObject *parent):
        QMediaFormatControl(parent)
    {
        m_supportedFormats.append("wav");
        m_supportedFormats.append("mp3");
        m_supportedFormats.append("mov");

        m_descriptions.insert("wav", "WAV format");
        m_descriptions.insert("mp3", "MP3 format");
        m_descriptions.insert("mov", "MOV format");
    }

    virtual ~MockMediaFormatControl() {};

    QStringList supportedFormats() const
    {
        return m_supportedFormats;
    }

    QString format() const
    {
        return m_format;
    }

    void setFormat(const QString &formatMimeType)
    {
        if (m_supportedFormats.contains(formatMimeType))
            m_format = formatMimeType;
    }

    QString formatDescription(const QString &formatMimeType) const
    {
        return m_descriptions.value(formatMimeType);
    }

private:
    QStringList m_supportedFormats;
    QMap<QString, QString> m_descriptions;
    QString m_format;
};

class MockVideoEncodeProvider : public QVideoEncoderControl
{
    Q_OBJECT
public:
    MockVideoEncodeProvider(QObject *parent):
        QVideoEncoderControl(parent)
    {
        m_bitrate = -1;
        m_quality = QtMedia::NormalQuality;
        m_supportedEncodeOptions.insert("video/3gpp", QStringList() << "quantizer" << "me");
        m_supportedEncodeOptions.insert("video/H264", QStringList() << "quantizer" << "me" << "bframes");
        m_videoCodecs << "video/3gpp" << "video/H264";
        m_sizes << QSize(320,240) << QSize(640,480);
        m_framerates << 30 << 15 << 1;
        m_framerate = 0.0;
    }
    ~MockVideoEncodeProvider() {}

    QSize resolution() const { return m_size; }
    QSize minimumResolution() const { return QSize(320,240); }
    QSize maximumResolution() const { return QSize(640,480); }
    QList<QSize> supportedResolutions() const { return m_sizes; }
    void setResolution(const QSize &sz) { m_size = sz; }
    qreal frameRate() const { return m_framerate; }
    qreal minimumFrameRate() const { return 1.0; }
    qreal maximumFrameRate() const { return 30.0; }
    QList<qreal> supportedFrameRates() const { return m_framerates; }
    void setFrameRate(const qreal &rate) { m_framerate = rate; }
    QStringList supportedVideoCodecs() const { return m_videoCodecs; }
    QString videoCodec() const { return m_vCodec; }
    bool setVideoCodec(const QString &codecName) { m_vCodec = codecName; return true; }
    QString videoCodecDescription(const QString &codecName) const { return codecName; }
    int bitrate() const { return m_bitrate; }
    void setBitrate(int bitrate) { m_bitrate = bitrate; }
    QtMedia::EncodingQuality quality() const { return m_quality; }
    void setQuality(QtMedia::EncodingQuality quality) { m_quality = quality; }

    QStringList supportedEncodingOptions(const QString &codec) const
    {
        return m_supportedEncodeOptions.value(codec);
    }

    QVariant encodingOption(const QString &codec, const QString &name) const
    {
        return m_encodeOptions[codec].value(name);
    }

    void setEncodingOption(const QString &codec, const QString &name, const QVariant &value)
    {
        m_encodeOptions[codec][name] = value;
    }

private:
    int m_bitrate;
    QtMedia::EncodingQuality m_quality;

    QMap<QString, QStringList> m_supportedEncodeOptions;
    QMap< QString, QMap<QString, QVariant> > m_encodeOptions;

    QStringList m_videoCodecs;
    QString m_vCodec;

    QSize m_size;
    QList<QSize> m_sizes;
    qreal m_framerate;
    QList<qreal> m_framerates;
};

class MockAudioEncodeProvider : public QAudioEncoderControl
{
    Q_OBJECT
public:
    MockAudioEncodeProvider(QObject *parent):
        QAudioEncoderControl(parent)
    {
        m_codecs << "audio/pcm" << "audio/mpeg";
        m_supportedEncodeOptions.insert("audio/pcm", QStringList());
        m_supportedEncodeOptions.insert("audio/mpeg", QStringList() << "quality" << "bitrate" << "mode" << "vbr");
        m_codec = "audio/pcm";
        m_bitrate = 128;
        m_quality = QtMedia::NormalQuality;
        m_sampleRate = -1;
        m_sampleSize = -1;
        m_channels = -1;
    }

    ~MockAudioEncodeProvider() {}

    int sampleRate() const { return m_sampleRate; }
    void setSampleRate(int sampleRate) { m_sampleRate = sampleRate; }
    QList<int> supportedSampleRates() const { return QList<int>() << 44100; }

    int channels() const { return m_channels; }
    void setChannels(int channels) { m_channels = channels; }
    QList<int> supportedChannelCounts() const { return QList<int>() << 2; }

    int sampleSize() const { return m_sampleSize; }
    void setSampleSize(int sampleSize) { m_sampleSize = sampleSize; }
    QList<int> supportedSampleSizes() const { return QList<int>() << 16; }

    QStringList supportedAudioCodecs() const
    {
        return m_codecs;
    }

    QString audioCodec() const
    {
        return m_codec;
    }

    bool setAudioCodec(const QString &codecName)
    {
        if (!m_codecs.contains(codecName))
            return false;

        m_codec = codecName;
        return true;
    }

    QString codecDescription(const QString &codecName) const
    {
        if (codecName == "audio/pcm")
            return QString("Pulse Code Modulation");

        if (codecName == "audio/mpeg")
            return QString("MP3 audio format");

        return QString();
    }

    int bitrate() const
    {
        return m_bitrate;
    }

    void setBitrate(int bitrate)
    {
        m_bitrate = bitrate;
    }

    QtMedia::EncodingQuality quality() const
    {
        return m_quality;
    }

    void setQuality(QtMedia::EncodingQuality qual)
    {
        m_quality = qual;
    }

    QStringList supportedEncodingOptions(const QString &codec) const
    {
        return m_supportedEncodeOptions.value(codec);
    }

    QVariant encodingOption(const QString &codec, const QString &name) const
    {
        return m_encodeOptions[codec].value(name);
    }

    void setEncodingOption(const QString &codec, const QString &name, const QVariant &value)
    {
        m_encodeOptions[codec][name] = value;
    }

private:
    int m_sampleRate;
    int m_channels;
    int m_sampleSize;

    QStringList  m_codecs;
    QStringList  m_codecsDesc;
    int          m_bitrate;
    QtMedia::EncodingQuality        m_quality;

    QMap<QString, QStringList> m_supportedEncodeOptions;
    QMap< QString, QMap<QString, QVariant> > m_encodeOptions;

    QString m_codec;


};

class MockAudioDeviceProvider : public QAudioDeviceControl
{
    Q_OBJECT
public:
    MockAudioDeviceProvider(QObject *parent):
        QAudioDeviceControl(parent)
    {
        m_names << "device1" << "device2" << "device3";
        m_descriptions << "dev1 comment" << "dev2 comment" << "dev3 comment";
        m_index = 0;
        emit devicesChanged();
    }
    ~MockAudioDeviceProvider() {};

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

class MockProvider : public QMediaRecorderControl
{
    Q_OBJECT

public:
    MockProvider(QObject *parent):
        QMediaRecorderControl(parent),
    m_state(QMediaRecorder::StoppedState),
    m_position(0) {}

    QUrl sink() const
    {
        return m_sink;
    }

    bool setSink(const QUrl &sink)
    {
        m_sink = sink;
        return true;
    }

    QMediaRecorder::State state() const
    {
        return m_state;
    }

    qint64 duration() const
    {
        return m_position;
    }

    using QMediaRecorderControl::error;

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

class MockService : public QMediaService
{
    Q_OBJECT
public:
    MockService(QObject *parent, QMediaControl *control):
        QMediaService(parent),
        mockControl(control),
        hasControls(true)
    {
        mockAudioDeviceControl = new MockAudioDeviceProvider(parent);
        mockAudioEncodeControl = new MockAudioEncodeProvider(parent);
        mockFormatControl = new MockMediaFormatControl(parent);
        mockVideoEncodeControl = new MockVideoEncodeProvider(parent);
    }

    QMediaControl* control(const char *name) const
    {
        if(hasControls && qstrcmp(name,QAudioEncoderControl_iid) == 0)
            return mockAudioEncodeControl;
        if(hasControls && qstrcmp(name,QAudioDeviceControl_iid) == 0)
            return mockAudioDeviceControl;
        if(hasControls && qstrcmp(name,QMediaRecorderControl_iid) == 0)
            return mockControl;
        if(hasControls && qstrcmp(name,QMediaFormatControl_iid) == 0)
            return mockFormatControl;
        if(hasControls && qstrcmp(name,QVideoEncoderControl_iid) == 0)
            return mockVideoEncodeControl;

        return 0;
    }

    QMediaControl   *mockControl;
    QAudioDeviceControl     *mockAudioDeviceControl;
    QAudioEncoderControl    *mockAudioEncodeControl;
    QMediaFormatControl     *mockFormatControl;
    QVideoEncoderControl    *mockVideoEncodeControl;
    bool hasControls;
};

class MockObject : public QMediaObject
{
    Q_OBJECT
public:
    MockObject(QObject *parent, MockService *service):
        QMediaObject(parent, service)
    {
    }
};

class tst_QMediaRecorder: public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();

private slots:
    void testNullService();
    void testNullControls();
    void testError();
    void testSink();
    void testRecord();
    void testAudioDeviceControl();
    void testAudioEncodeControl();
    void testMediaFormatsControl();
    void testVideoEncodeControl();
    void testEncodingSettings();
    void testAudioSettings();
    void testVideoSettings();

private:
    QAudioEncoderControl* encode;
    QAudioDeviceControl* audio;
    MockObject      *object;
    MockService		*service;
    MockProvider    *mock;
    QMediaRecorder  *capture;
    QVideoEncoderControl* videoEncode;
};

Q_DECLARE_METATYPE(QMediaRecorder::Error);

void tst_QMediaRecorder::initTestCase()
{
    qRegisterMetaType<QMediaRecorder::State>("QMediaRecorder::State");
    qRegisterMetaType<QMediaRecorder::Error>();

    mock = new MockProvider(this);
    service = new MockService(this, mock);
    object = new MockObject(this, service);
    capture = new QMediaRecorder(object);

    audio = qobject_cast<QAudioDeviceControl*>(capture->service()->control(QAudioDeviceControl_iid));
    encode = qobject_cast<QAudioEncoderControl*>(capture->service()->control(QAudioEncoderControl_iid));
    videoEncode = qobject_cast<QVideoEncoderControl*>(capture->service()->control(QVideoEncoderControl_iid));
}

void tst_QMediaRecorder::cleanupTestCase()
{
    delete capture;
    delete object;
    delete service;
    delete mock;
}

void tst_QMediaRecorder::testNullService()
{
    const QString id(QLatin1String("application/x-format"));

    MockObject object(0, 0);
    QMediaRecorder recorder(&object);

    QCOMPARE(recorder.sink(), QUrl());
    QCOMPARE(recorder.state(), QMediaRecorder::StoppedState);
    QCOMPARE(recorder.error(), QMediaRecorder::NoError);
    QCOMPARE(recorder.duration(), qint64(0));
    QCOMPARE(recorder.supportedFormats(), QStringList());
    QCOMPARE(recorder.formatDescription(id), QString());
    QCOMPARE(recorder.supportedAudioCodecs(), QStringList());
    QCOMPARE(recorder.audioCodecDescription(id), QString());
    QCOMPARE(recorder.supportedAudioSampleRates(), QList<int>());
    QCOMPARE(recorder.supportedVideoCodecs(), QStringList());
    QCOMPARE(recorder.videoCodecDescription(id), QString());
    QCOMPARE(recorder.minimumResolution(), QSize());
    QCOMPARE(recorder.maximumResolution(), QSize());
    QCOMPARE(recorder.supportedResolutions(), QList<QSize>());
    QCOMPARE(recorder.audioSettings(), QAudioEncoderSettings());
    QCOMPARE(recorder.videoSettings(), QVideoEncoderSettings());
    QCOMPARE(recorder.format(), QString());
}

void tst_QMediaRecorder::testNullControls()
{
    const QString id(QLatin1String("application/x-format"));

    MockService service(0, 0);
    service.hasControls = false;
    MockObject object(0, &service);
    QMediaRecorder recorder(&object);

    QCOMPARE(recorder.sink(), QUrl());
    QCOMPARE(recorder.state(), QMediaRecorder::StoppedState);
    QCOMPARE(recorder.error(), QMediaRecorder::NoError);
    QCOMPARE(recorder.duration(), qint64(0));
    QCOMPARE(recorder.supportedFormats(), QStringList());
    QCOMPARE(recorder.formatDescription(id), QString());
    QCOMPARE(recorder.supportedAudioCodecs(), QStringList());
    QCOMPARE(recorder.audioCodecDescription(id), QString());
    QCOMPARE(recorder.supportedAudioSampleRates(), QList<int>());
    QCOMPARE(recorder.supportedVideoCodecs(), QStringList());
    QCOMPARE(recorder.videoCodecDescription(id), QString());
    QCOMPARE(recorder.minimumResolution(), QSize());
    QCOMPARE(recorder.maximumResolution(), QSize());
    QCOMPARE(recorder.supportedResolutions(), QList<QSize>());
    QCOMPARE(recorder.audioSettings(), QAudioEncoderSettings());
    QCOMPARE(recorder.videoSettings(), QVideoEncoderSettings());
    QCOMPARE(recorder.format(), QString());

    QAudioEncoderSettings audio;
    audio.setCodec(id);
    audio.setQuality(QtMedia::LowQuality);

    QVideoEncoderSettings video;
    video.setCodec(id);
    video.setResolution(640, 480);

    recorder.setEncodingSettings(audio, video, id);

    QCOMPARE(recorder.audioSettings(), QAudioEncoderSettings());
    QCOMPARE(recorder.videoSettings(), QVideoEncoderSettings());
    QCOMPARE(recorder.format(), QString());

    QSignalSpy spy(&recorder, SIGNAL(stateChanged(QMediaRecorder::State)));

    recorder.record();
    QCOMPARE(recorder.state(), QMediaRecorder::StoppedState);
    QCOMPARE(recorder.error(), QMediaRecorder::NoError);
    QCOMPARE(spy.count(), 0);

    recorder.pause();
    QCOMPARE(recorder.state(), QMediaRecorder::StoppedState);
    QCOMPARE(recorder.error(), QMediaRecorder::NoError);
    QCOMPARE(spy.count(), 0);

    recorder.stop();
    QCOMPARE(recorder.state(), QMediaRecorder::StoppedState);
    QCOMPARE(recorder.error(), QMediaRecorder::NoError);
    QCOMPARE(spy.count(), 0);
}

void tst_QMediaRecorder::testError()
{
    const QString errorString(QLatin1String("format error"));

    QSignalSpy spy(capture, SIGNAL(error(QMediaRecorder::Error)));

    QCOMPARE(capture->error(), QMediaRecorder::NoError);
    QCOMPARE(capture->errorString(), QString());

    mock->error(QMediaRecorder::FormatError, errorString);
    QCOMPARE(capture->error(), QMediaRecorder::FormatError);
    QCOMPARE(capture->errorString(), errorString);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(qvariant_cast<QMediaRecorder::Error>(spy.last().value(0)), QMediaRecorder::FormatError);
}

void tst_QMediaRecorder::testSink()
{
    capture->setSink(QUrl("test.tmp"));
    QUrl s = capture->sink();
    QCOMPARE(s.toString(), QString("test.tmp"));
}

void tst_QMediaRecorder::testRecord()
{
    QSignalSpy stateSignal(capture,SIGNAL(stateChanged(QMediaRecorder::State)));
    QSignalSpy progressSignal(capture, SIGNAL(durationChanged(qint64)));
    capture->record();
    QVERIFY(capture->state() == QMediaRecorder::RecordingState);
    QVERIFY(capture->error() == QMediaRecorder::NoError);
    QVERIFY(capture->errorString() == QString());
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(stateSignal.count() == 1);
    QVERIFY(progressSignal.count() == 1);
    capture->pause();
    QVERIFY(capture->state() == QMediaRecorder::PausedState);
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(stateSignal.count() == 2);
    capture->stop();
    QVERIFY(capture->state() == QMediaRecorder::StoppedState);
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(stateSignal.count() == 3);
}

void tst_QMediaRecorder::testAudioDeviceControl()
{
    QSignalSpy readSignal(audio,SIGNAL(selectedDeviceChanged(int)));
    QVERIFY(audio->deviceCount() == 3);
    QVERIFY(audio->defaultDevice() == 1);
    audio->setSelectedDevice(1);
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(audio->selectedDevice() == 1);
    QVERIFY(readSignal.count() == 1);
    QVERIFY(audio->name(1) == QString("device2"));
    QVERIFY(audio->description(1) == "dev2 comment");
}

void tst_QMediaRecorder::testAudioEncodeControl()
{
    QStringList codecs = capture->supportedAudioCodecs();
    QVERIFY(codecs.count() == 2);
    QVERIFY(capture->audioCodecDescription("audio/pcm") == "Pulse Code Modulation");
    QStringList options = encode->supportedEncodingOptions("audio/mpeg");
    QCOMPARE(options.count(), 4);
    QVERIFY(encode->encodingOption("audio/mpeg","bitrate").isNull());
    encode->setEncodingOption("audio/mpeg", "bitrate", QString("vbr"));
    QCOMPARE(encode->encodingOption("audio/mpeg","bitrate").toString(), QString("vbr"));
}

void tst_QMediaRecorder::testMediaFormatsControl()
{
    QCOMPARE(capture->supportedFormats(), QStringList() << "wav" << "mp3" << "mov");

    QCOMPARE(capture->formatDescription("wav"), QString("WAV format"));
    QCOMPARE(capture->formatDescription("mp3"), QString("MP3 format"));
    QCOMPARE(capture->formatDescription("ogg"), QString());
}

void tst_QMediaRecorder::testVideoEncodeControl()
{
    QList<QSize> sizes = capture->supportedResolutions();
    QVERIFY(sizes.count() == 2);
    QVERIFY(capture->minimumResolution() == QSize(320,240));
    QVERIFY(capture->maximumResolution() == QSize(640,480));

    QList<qreal> rates = capture->supportedFrameRates();
    QVERIFY(rates.count() == 3);
    QVERIFY(qFuzzyCompare(capture->minimumFrameRate(), 1.0));
    QVERIFY(qFuzzyCompare(capture->maximumFrameRate(), 30.0));

    QStringList vCodecs = capture->supportedVideoCodecs();
    QVERIFY(vCodecs.count() == 2);
    QCOMPARE(capture->videoCodecDescription("video/3gpp"), QString("video/3gpp"));

    QStringList options = videoEncode->supportedEncodingOptions("video/3gpp");
    QCOMPARE(options.count(), 2);

    QVERIFY(encode->encodingOption("video/3gpp","me").isNull());
    encode->setEncodingOption("video/3gpp", "me", QString("dia"));
    QCOMPARE(encode->encodingOption("video/3gpp","me").toString(), QString("dia"));

}

void tst_QMediaRecorder::testEncodingSettings()
{
    QAudioEncoderSettings audioSettings = capture->audioSettings();
    QCOMPARE(audioSettings.codec(), QString("audio/pcm"));
    QCOMPARE(audioSettings.bitrate(), 128);
    QCOMPARE(audioSettings.sampleRate(), -1);
    QCOMPARE(audioSettings.quality(), QtMedia::NormalQuality);

    QVideoEncoderSettings videoSettings = capture->videoSettings();
    QCOMPARE(videoSettings.codec(), QString());
    QCOMPARE(videoSettings.bitrate(), -1);
    QCOMPARE(videoSettings.resolution(), QSize());
    QCOMPARE(videoSettings.frameRate(), 0.0);
    QCOMPARE(videoSettings.quality(), QtMedia::NormalQuality);

    QString format = capture->format();
    QCOMPARE(format, QString());

    audioSettings.setCodec("audio/mpeg");
    audioSettings.setSampleRate(44100);
    audioSettings.setBitrate(256);
    audioSettings.setQuality(QtMedia::HighQuality);

    videoSettings.setCodec("video/3gpp");
    videoSettings.setBitrate(800);
    videoSettings.setFrameRate(24);
    videoSettings.setResolution(QSize(800,600));
    videoSettings.setQuality(QtMedia::HighQuality);

    format = QString("mov");

    capture->setEncodingSettings(audioSettings,videoSettings,format);

    QCOMPARE(capture->audioSettings(), audioSettings);
    QCOMPARE(capture->videoSettings(), videoSettings);
    QCOMPARE(capture->format(), format);
}

void tst_QMediaRecorder::testAudioSettings()
{
    QAudioEncoderSettings settings;
    QVERIFY(settings.isNull());
    QVERIFY(settings == QAudioEncoderSettings());

    QCOMPARE(settings.codec(), QString());
    settings.setCodec(QLatin1String("codecName"));
    QCOMPARE(settings.codec(), QLatin1String("codecName"));
    QVERIFY(!settings.isNull());
    QVERIFY(settings != QAudioEncoderSettings());

    settings = QAudioEncoderSettings();
    QCOMPARE(settings.bitrate(), -1);
    settings.setBitrate(128000);
    QCOMPARE(settings.bitrate(), 128000);
    QVERIFY(!settings.isNull());

    settings = QAudioEncoderSettings();
    QCOMPARE(settings.quality(), QtMedia::NormalQuality);
    settings.setQuality(QtMedia::HighQuality);
    QCOMPARE(settings.quality(), QtMedia::HighQuality);
    QVERIFY(!settings.isNull());

    settings = QAudioEncoderSettings();
    QCOMPARE(settings.sampleRate(), -1);
    settings.setSampleRate(44100);
    QCOMPARE(settings.sampleRate(), 44100);
    QVERIFY(!settings.isNull());

    settings = QAudioEncoderSettings();
    QVERIFY(settings.isNull());
    QCOMPARE(settings.codec(), QString());
    QCOMPARE(settings.bitrate(), -1);
    QCOMPARE(settings.quality(), QtMedia::NormalQuality);
    QCOMPARE(settings.sampleRate(), -1);

    {
        QAudioEncoderSettings settings1;
        QAudioEncoderSettings settings2;
        QCOMPARE(settings2, settings1);

        settings2 = settings1;
        QCOMPARE(settings2, settings1);
        QVERIFY(settings2.isNull());

        settings1.setQuality(QtMedia::HighQuality);

        QVERIFY(settings2.isNull());
        QVERIFY(!settings1.isNull());
        QVERIFY(settings1 != settings2);
    }

    {
        QAudioEncoderSettings settings1;
        QAudioEncoderSettings settings2(settings1);
        QCOMPARE(settings2, settings1);

        settings2 = settings1;
        QCOMPARE(settings2, settings1);
        QVERIFY(settings2.isNull());

        settings1.setQuality(QtMedia::HighQuality);

        QVERIFY(settings2.isNull());
        QVERIFY(!settings1.isNull());
        QVERIFY(settings1 != settings2);
    }
}

void tst_QMediaRecorder::testVideoSettings()
{
    QVideoEncoderSettings settings;
    QVERIFY(settings.isNull());
    QVERIFY(settings == QVideoEncoderSettings());

    QCOMPARE(settings.codec(), QString());
    settings.setCodec(QLatin1String("codecName"));
    QCOMPARE(settings.codec(), QLatin1String("codecName"));
    QVERIFY(!settings.isNull());
    QVERIFY(settings != QVideoEncoderSettings());

    settings = QVideoEncoderSettings();
    QCOMPARE(settings.bitrate(), -1);
    settings.setBitrate(128000);
    QCOMPARE(settings.bitrate(), 128000);
    QVERIFY(!settings.isNull());

    settings = QVideoEncoderSettings();
    QCOMPARE(settings.quality(), QtMedia::NormalQuality);
    settings.setQuality(QtMedia::HighQuality);
    QCOMPARE(settings.quality(), QtMedia::HighQuality);
    QVERIFY(!settings.isNull());

    settings = QVideoEncoderSettings();
    QCOMPARE(settings.frameRate(), qreal());
    settings.setFrameRate(30000.0/10001);
    QVERIFY(qFuzzyCompare(settings.frameRate(),30000.0/10001));
    settings.setFrameRate(24.0);
    QVERIFY(qFuzzyCompare(settings.frameRate(),24.0));
    QVERIFY(!settings.isNull());

    settings = QVideoEncoderSettings();
    QCOMPARE(settings.resolution(), QSize());
    settings.setResolution(QSize(320,240));
    QCOMPARE(settings.resolution(), QSize(320,240));
    settings.setResolution(800,600);
    QCOMPARE(settings.resolution(), QSize(800,600));
    QVERIFY(!settings.isNull());

    settings = QVideoEncoderSettings();
    QVERIFY(settings.isNull());
    QCOMPARE(settings.codec(), QString());
    QCOMPARE(settings.bitrate(), -1);
    QCOMPARE(settings.quality(), QtMedia::NormalQuality);
    QCOMPARE(settings.frameRate(), qreal());
    QCOMPARE(settings.resolution(), QSize());

    {
        QVideoEncoderSettings settings1;
        QVideoEncoderSettings settings2;
        QCOMPARE(settings2, settings1);

        settings2 = settings1;
        QCOMPARE(settings2, settings1);
        QVERIFY(settings2.isNull());

        settings1.setQuality(QtMedia::HighQuality);

        QVERIFY(settings2.isNull());
        QVERIFY(!settings1.isNull());
        QVERIFY(settings1 != settings2);
    }

    {
        QVideoEncoderSettings settings1;
        QVideoEncoderSettings settings2(settings1);
        QCOMPARE(settings2, settings1);

        settings2 = settings1;
        QCOMPARE(settings2, settings1);
        QVERIFY(settings2.isNull());

        settings1.setQuality(QtMedia::HighQuality);

        QVERIFY(settings2.isNull());
        QVERIFY(!settings1.isNull());
        QVERIFY(settings1 != settings2);
    }
}

QTEST_MAIN(tst_QMediaRecorder)

#include "tst_qmediarecorder.moc"
