/*
Copyright (c) 2011 Elektrobit (EB), All rights reserved.
Contact: oss-devel@elektrobit.com

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
* Neither the name of the Elektrobit (EB) nor the names of its
contributors may be used to endorse or promote products derived from
 this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY Elektrobit (EB) ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Elektrobit
(EB) BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "androidaudioencodercontrol.h"


AndroidAudioEncoderControl::AndroidAudioEncoderControl(QObject *parent) :
        QAudioEncoderControl(parent)
{

}


AndroidAudioEncoderControl::~AndroidAudioEncoderControl()
{
}

QStringList AndroidAudioEncoderControl::supportedAudioCodecs() const
{
    QStringList supportedCodecs;
    supportedCodecs.append("audio/amr-nb");
    return supportedCodecs;
}

QString AndroidAudioEncoderControl::codecDescription(const QString &codecName) const
{
    if(qstrcmp(codecName.toLocal8Bit().constData(), "audio/amr-nb") == 0)
    {
        return QString("Adaptive Multi-Rate Audio Codec");
    }
    else
    {
        return QString();
    }
}

QStringList AndroidAudioEncoderControl::supportedEncodingOptions(const QString &codec) const
{
    // Possible settings: EncodingMode, Codec, BitRate, ChannelCount, SampleRate, Quality
    // Possible (codec specific) Options: None
    Q_UNUSED(codec);
    QStringList options;
    options.append("BitRate");
    options.append("ChannelCount");
    return QStringList();
}

QVariant AndroidAudioEncoderControl::encodingOption(const QString &codec, const QString &name) const
{
    // Possible settings: EncodingMode, Codec, BitRate, ChannelCount, SampleRate, Quality
    // Possible (codec specific) Options: None
    Q_UNUSED(codec);
    if(qstrcmp(name.toLocal8Bit().constData(), "BitRate") == 0) {
        return QVariant(m_bitRate);
    }
    else if(qstrcmp(name.toLocal8Bit().constData(), "ChannelCount") == 0) {
        return QVariant(m_channelCount);
    }
    else
    {
        return QVariant();
    }
}

void AndroidAudioEncoderControl::setEncodingOption(
        const QString &codec, const QString &name, const QVariant &value)
{
    Q_UNUSED(codec);
    if(qstrcmp(name.toLocal8Bit().constData(), "BitRate") == 0) {
        QtCameraJni::setAudioBitRate(m_bitRate = value.toInt());
    }
    else if(qstrcmp(name.toLocal8Bit().constData(), "ChannelCount") == 0) {
        QtCameraJni::setAudioChannelsCount(m_channelCount = value.toInt());
    }
}

QList<int> AndroidAudioEncoderControl::supportedSampleRates(
        const QAudioEncoderSettings &settings, bool *continuous) const
{
    Q_UNUSED(settings);
    Q_UNUSED(continuous);
    QList<int> supportedSampleRates;
    supportedSampleRates<<8;
    return supportedSampleRates;
}

QAudioEncoderSettings AndroidAudioEncoderControl::audioSettings() const
{
    QAudioEncoderSettings settings;
    settings.setCodec(tr("audio/amr-nb"));
    settings.setSampleRate(8);
    return settings;
}

void AndroidAudioEncoderControl::setAudioSettings(const QAudioEncoderSettings &settings)
{
    Q_UNUSED(settings);
    //Nothing to be implemented
}

// End of file


