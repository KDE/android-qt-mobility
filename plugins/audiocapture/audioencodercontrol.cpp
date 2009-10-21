#include "audioencodercontrol.h"
#include "audiocapturesession.h"

#include <QtMultimedia/qaudioformat.h>

#include <QtCore/qdebug.h>

AudioEncoderControl::AudioEncoderControl(QObject *parent)
    :QAudioEncoderControl(parent)
{
    QAudioFormat fmt;
    fmt.setSampleSize(8);
    fmt.setChannels(1);
    fmt.setFrequency(8000);
    fmt.setSampleType(QAudioFormat::SignedInt);
    fmt.setCodec("audio/pcm");

    m_session = qobject_cast<AudioCaptureSession*>(parent);
}

AudioEncoderControl::~AudioEncoderControl()
{
}

QStringList AudioEncoderControl::supportedAudioCodecs() const
{
    return m_session->supportedAudioCodecs();
}

QString AudioEncoderControl::audioCodec() const
{
    return m_session->format().codec();
}

bool AudioEncoderControl::setAudioCodec(const QString &codecName)
{
    QAudioFormat fmt = m_session->format();
    fmt.setCodec(codecName);
    return m_session->setFormat(fmt);
}

QString AudioEncoderControl::codecDescription(const QString &codecName) const
{
    if(qstrcmp(codecName.toLocal8Bit().constData(),"audio/x-wav") == 0)
        return QString("wav file format");

    return QString();
}

int AudioEncoderControl::bitrate() const
{
    return (m_session->format().frequency()*m_session->format().channels()*(m_session->format().sampleSize()/8));
}

void AudioEncoderControl::setBitrate(int value)
{
    QAudioFormat fmt = m_session->format();

    if(value <= 8) {
        // low, 8000Hz mono U8
        fmt.setSampleType(QAudioFormat::UnSignedInt);
        fmt.setSampleSize(8);
        fmt.setFrequency(8000);
        fmt.setChannels(1);

    } else if(value <= 44) {
        // medium, 22050Hz mono S16
        fmt.setSampleType(QAudioFormat::SignedInt);
        fmt.setSampleSize(16);
        fmt.setFrequency(22050);
        fmt.setChannels(1);

    } else {
        // high, 44100Hz mono S16
        fmt.setSampleType(QAudioFormat::SignedInt);
        fmt.setSampleSize(16);
        fmt.setFrequency(44100);
        fmt.setChannels(1);

    }
    m_session->setFormat(fmt);
}

QtMedia::EncodingQuality AudioEncoderControl::quality() const
{
    return QtMedia::NormalQuality;
}

void AudioEncoderControl::setQuality(QtMedia::EncodingQuality value)
{
    Q_UNUSED(value)
}

QStringList AudioEncoderControl::supportedEncodingOptions(const QString &codec) const
{
    Q_UNUSED(codec)

    QStringList list;
    return list;
}

QVariant AudioEncoderControl::encodingOption(const QString &codec, const QString &name) const
{
    Q_UNUSED(codec)
    if(qstrcmp(name.toLocal8Bit().constData(),"bitrate") == 0) {
        return QVariant(8000);
    }

    return QVariant();
}

void AudioEncoderControl::setEncodingOption(
        const QString &codec, const QString &name, const QVariant &value)
{
    Q_UNUSED(value)
    Q_UNUSED(codec)

    QAudioFormat fmt = m_session->format();

    if(qstrcmp(name.toLocal8Bit().constData(),"bitrate") == 0) {
        //TODO

    } else if(qstrcmp(name.toLocal8Bit().constData(),"quality") == 0) {
        //TODO

    } else
        qWarning()<<"option: "<<name<<" is an unknown option!";
}

int AudioEncoderControl::sampleRate() const
{
    return m_session->format().frequency();
}

void AudioEncoderControl::setSampleRate(int sampleRate)
{
    QAudioFormat fmt = m_session->format();
    fmt.setFrequency(sampleRate);
    m_session->setFormat(fmt);
}

QList<int> AudioEncoderControl::supportedSampleRates() const
{
    return m_session->deviceInfo()->supportedFrequencies();
}

int AudioEncoderControl::channels() const
{
    return m_session->format().channels();
}

void AudioEncoderControl::setChannels(int channels)
{
    QAudioFormat fmt = m_session->format();
    fmt.setChannels(channels);
    m_session->setFormat(fmt);
}

QList<int> AudioEncoderControl::supportedChannelCounts() const
{
    return QList<int>() << 1 << 2;
}

int AudioEncoderControl::sampleSize() const
{
    return m_session->format().sampleSize();
}

void AudioEncoderControl::setSampleSize(int sampleSize)
{
    QAudioFormat fmt = m_session->format();
    fmt.setSampleSize(sampleSize);
    m_session->setFormat(fmt);
}

QList<int> AudioEncoderControl::supportedSampleSizes() const
{
    QList<int> sizes = m_session->deviceInfo()->supportedSampleSizes();
    return sizes;
}

QAudioEncoderSettings AudioEncoderControl::audioSettings() const
{
    QAudioEncoderSettings settings;
    settings.setCodec(audioCodec());
    settings.setBitrate(bitrate());
    settings.setQuality(quality());
    settings.setSampleRate(sampleRate());
    settings.setChannels(channels());
    return settings;
}

void AudioEncoderControl::setAudioSettings(const QAudioEncoderSettings &settings)
{
    setAudioCodec(settings.codec());
    setBitrate(settings.bitrate());
    setQuality(settings.quality());
    setSampleRate(settings.sampleRate());
    setChannels(settings.channels());
}
