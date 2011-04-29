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

#ifndef ANDROIDVIDEOENCODERCONTROL_H
#define ANDROIDVIDEOENCODERCONTROL_H

#include <QtCore/qstringlist.h>
#include <QtCore/qmap.h>
#include "androidcamjni.h"

#include "qvideoencodercontrol.h"

QT_USE_NAMESPACE


/*
* Control for video settings when recording video using QMediaRecorder.
*/
class AndroidVideoEncoderControl : public QVideoEncoderControl
{
    Q_OBJECT

public: // Contructors & Destructor

    AndroidVideoEncoderControl(QObject *parent = 0);
    virtual ~AndroidVideoEncoderControl();

public: // QVideoEncoderControl

    // Resolution
    QList<QSize> supportedResolutions(const QVideoEncoderSettings &settings, bool *continuous = 0) const;

    // Framerate
    QList<qreal>  supportedFrameRates(const QVideoEncoderSettings &settings, bool *continuous = 0) const;

    // Video Codec
    QStringList supportedVideoCodecs() const;
    QString videoCodecDescription(const QString &codecName) const;

    // Video Settings
    QVideoEncoderSettings videoSettings() const;
    void setVideoSettings(const QVideoEncoderSettings &settings);
    void videoEncoderSettings(QVideoEncoderSettings &videoSettings) const;
    // Encoding Options
    QStringList supportedEncodingOptions(const QString &codec) const;
    QVariant encodingOption(const QString &codec, const QString &name) const;
    void setEncodingOption(const QString &codec, const QString &name, const QVariant &value);

private: // Data
    qulonglong m_maxClipSizeInBytes;
    int m_bitRate;
    QList<int> m_videoSettings;

};
#endif // ANDROIDVIDEOENCODERCONTROL_H
