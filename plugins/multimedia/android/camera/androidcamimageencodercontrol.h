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

#ifndef ANDROIDCAMIMAGEENCODERCONTROL_H
#define ANDROIDCAMIMAGEENCODERCONTROL_H
#include <QtCore/qobject.h>
#include "qimageencodercontrol.h"
#include <androidcamjni.h>

QT_USE_NAMESPACE

class AndroidCamImageEncoderControl : public QImageEncoderControl
{
    Q_OBJECT

public: // Contructors & Destructor

    AndroidCamImageEncoderControl(QObject *parent = 0);
    ~AndroidCamImageEncoderControl();

public: // QImageEncoderControl

    // Codec
    QStringList supportedImageCodecs() const;
    QString imageCodecDescription(const QString &codec) const;

    // Resolution
    QList<QSize> supportedResolutions(const QImageEncoderSettings &settings,
                                      bool *continuous = 0) const;

    // Settings
    QImageEncoderSettings imageSettings() const;
    void setImageSettings(const QImageEncoderSettings &settings);
private:
    QList<int> m_imageSettings;
};
#endif // ANDROIDCAMIMAGEENCODERCONTROL_H