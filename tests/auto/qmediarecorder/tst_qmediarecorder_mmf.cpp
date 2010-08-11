#include "tst_qmediarecorder_mmf.h"

QT_USE_NAMESPACE

void tst_QMediaRecorder_mmf::initTestCase()
{
    qRegisterMetaType<QMediaRecorder::State>("QMediaRecorder::State");
    qRegisterMetaType<QMediaRecorder::Error>("QMediaRecorder::Error");

    captureSource = new QAudioCaptureSource;
    audiocapture = new QMediaRecorder(captureSource);

    audioEndpoint = qobject_cast<QAudioEndpointSelector*>(captureSource->service()->requestControl(QAudioEndpointSelector_iid));
    audioEncoder = qobject_cast<QAudioEncoderControl*>(captureSource->service()->requestControl(QAudioEncoderControl_iid));
}

void tst_QMediaRecorder_mmf::cleanupTestCase()
{
    delete audiocapture;
    delete captureSource;
}

QUrl tst_QMediaRecorder_mmf::recordPathAudio(QDir outputDir)
{
    int lastImage = 0;
    int fileCount = 0;
    foreach( QString fileName, outputDir.entryList(QStringList() << "testclip_*.amr") ) {
        int imgNumber = fileName.mid(5, fileName.size()-9).toInt();
        lastImage = qMax(lastImage, imgNumber);
        if (outputDir.exists(fileName))
            fileCount+=1;
    }
    lastImage+=fileCount;

    QUrl location(QDir::toNativeSeparators(outputDir.canonicalPath()+QString("/testclip_%1.amr").arg(lastImage+1,4,10,QLatin1Char('0'))));
    return location;
}

void tst_QMediaRecorder_mmf::testAudioSink()
{
    audiocapture->setOutputLocation(QUrl("test.tmp"));
    QUrl s = audiocapture->outputLocation();
    QCOMPARE(s.toString(), QString("test.tmp"));
}

void tst_QMediaRecorder_mmf::testAudioRecord()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->setOutputLocation(recordPathAudio(QDir::rootPath()));
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
}

void tst_QMediaRecorder_mmf::testAudioRecordWithAMR()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    audiocapture->setOutputLocation(recordPathAudio(QDir::rootPath()));
    QAudioEncoderSettings audioSettings;
    QVideoEncoderSettings videoSettings;
    audioSettings.setCodec("AMR");
    QString format = audiocapture->containerMimeType();
    format = QString("audio/amr");
    audiocapture->setEncodingSettings(audioSettings,videoSettings,format);
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
}

void tst_QMediaRecorder_mmf::testAudioEndPointSelector()
{
    QSignalSpy audioSignal(audioEndpoint,SIGNAL(activeEndpointChanged(QString)));
    QVERIFY(audioEndpoint->availableEndpoints().count() == 1);
    QVERIFY(audioEndpoint->defaultEndpoint().compare("") == 0);
    audioEndpoint->setActiveEndpoint("device2");
    QVERIFY(audioEndpoint->activeEndpoint().compare("device2") == 0);
    QVERIFY(audioSignal.count() == 1);
    QVERIFY(audioEndpoint->endpointDescription("device2").compare("") == 0);
}

void tst_QMediaRecorder_mmf::testAudioEncoderControl()
{
    QStringList codecs = audiocapture->supportedAudioCodecs();
    QVERIFY(codecs.count() == 2);
    QVERIFY(audiocapture->audioCodecDescription("PCM") == "Pulse code modulation");
    QStringList options = audioEncoder->supportedEncodingOptions("PCM");
    QCOMPARE(options.count(), 3);
    QCOMPARE(audiocapture->supportedAudioSampleRates().count(), 12);
    audioEncoder->setEncodingOption("PCM", "channels", QVariant(2));
    QCOMPARE(audioEncoder->encodingOption("PCM","channels").toInt(), 2);
    audioEncoder->setEncodingOption("PCM", "quality", QVariant(int(QtMultimediaKit::NormalQuality)));
    QCOMPARE(audioEncoder->encodingOption("PCM","quality").toInt(), int(QtMultimediaKit::NormalQuality));
    audioEncoder->setEncodingOption("PCM", "samplerate", QVariant(44100));
    QCOMPARE(audioEncoder->encodingOption("PCM","samplerate").toInt(), 44100);
}

void tst_QMediaRecorder_mmf::testMediaFormatsControl()
{
    //audioocontainer types
    QCOMPARE(audiocapture->supportedContainers(), QStringList() << "audio/wav" << "audio/amr");
    QCOMPARE(audiocapture->containerDescription("audio/wav"), QString("WAV Write Format"));
    QCOMPARE(audiocapture->containerDescription("audio/amr"), QString("AMR Write Format"));
}

void tst_QMediaRecorder_mmf::testDefaultAudioEncodingSettings()
{
    QAudioEncoderSettings audioSettings = audiocapture->audioSettings();
    QCOMPARE(audioSettings.codec(), QString("AMR"));
    QString format = audiocapture->containerMimeType();
    QCOMPARE(format, QString("audio/amr"));
}
