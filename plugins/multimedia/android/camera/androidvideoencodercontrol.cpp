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

#include "androidvideoencodercontrol.h"
#include <QtCore/QtGlobal>

AndroidVideoEncoderControl::AndroidVideoEncoderControl(QObject *parent):
        QVideoEncoderControl(parent)
{
    m_videoSettings.clear();
    m_videoSettings << 30 << 480 << 360;
}

AndroidVideoEncoderControl::~AndroidVideoEncoderControl()
{
}

QStringList AndroidVideoEncoderControl::supportedVideoCodecs() const
{
    QStringList list;
    list << "video/H.263";
    return list;
}

QString AndroidVideoEncoderControl::videoCodecDescription(const QString &codecName) const
{
    QString codecDescription;
    if (codecName.contains("video/H263-2000", Qt::CaseInsensitive))
    {
        codecDescription.append("H.263 Video Codec");
    }
    else
    {
        codecDescription.append("Video Codec");
    }

    return codecDescription;

}

QList<qreal> AndroidVideoEncoderControl::supportedFrameRates(const QVideoEncoderSettings &settings, bool *continuous) const
{
    Q_UNUSED(settings);
    Q_UNUSED(continuous);
    QList<qreal> supportedRates;
    supportedRates << qreal(30)<<qreal(12);

    return supportedRates;
}

QList<QSize> AndroidVideoEncoderControl::supportedResolutions(const QVideoEncoderSettings &settings, bool *continuous) const
{
    Q_UNUSED(settings);
    Q_UNUSED(continuous);
    QList<QSize> supportedResolutions;
    supportedResolutions << QSize(480,360);
    supportedResolutions << QSize(176,144);
    return supportedResolutions;
}

QStringList AndroidVideoEncoderControl::supportedEncodingOptions(const QString &codec) const
{
    Q_UNUSED(codec);

    QStringList options;
    options.append("BitRate");
    options.append("maxClipSizeInBytes");

    return options;
}

QVariant AndroidVideoEncoderControl::encodingOption(const QString &codec, const QString &name) const
{
    Q_UNUSED(codec);
    if(qstrcmp(name.toLocal8Bit().constData(), "BitRate") == 0) {
        return QVariant(m_bitRate);
    }
    else if(qstrcmp(name.toLocal8Bit().constData(), "maxClipSizeInBytes") == 0) {
        return QVariant(m_maxClipSizeInBytes);
    }

}

void AndroidVideoEncoderControl::setEncodingOption(
        const QString &codec, const QString &name, const QVariant &value)
{
    Q_UNUSED(codec);
    if(qstrcmp(name.toLocal8Bit().constData(), "BitRate") == 0) {
        QtCameraJni::setVideoEncodingBitrate(m_bitRate = value.toInt());
    }
    else if(qstrcmp(name.toLocal8Bit().constData(), "maxClipSizeInBytes") == 0) {
        QtCameraJni::setMaxVideoSize((long long)(m_maxClipSizeInBytes = value.toULongLong()));
    }

}

QVideoEncoderSettings AndroidVideoEncoderControl::videoSettings() const
{
    QVideoEncoderSettings settings;
    settings.setCodec(tr("video/H.263"));
    settings.setFrameRate((qreal)m_videoSettings.at(0));
    settings.setResolution(m_videoSettings.at(1),m_videoSettings.at(2));
    return settings;
}


void AndroidVideoEncoderControl::setVideoSettings(const QVideoEncoderSettings &settings)
{
    m_videoSettings.clear();
    if(settings.frameRate() != 0) {
        m_videoSettings << (int)settings.frameRate();
    }
    if(!settings.resolution().isEmpty()){
        m_videoSettings << settings.resolution().width() <<settings.resolution().height();
    }
    else
    {
        m_videoSettings << 30 << 480 << 360;
    }
    QtCameraJni::setVideoSettings(m_videoSettings);
}

// End of file
