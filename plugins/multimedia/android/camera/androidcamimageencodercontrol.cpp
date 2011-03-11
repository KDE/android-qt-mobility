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

#include <androidcamimageencodercontrol.h>

AndroidCamImageEncoderControl::AndroidCamImageEncoderControl(QObject *parent) :
        QImageEncoderControl(parent)

{
    m_imageSettings.clear();
}

AndroidCamImageEncoderControl::~AndroidCamImageEncoderControl()
{

}

QStringList AndroidCamImageEncoderControl::supportedImageCodecs() const
{
    QStringList supportedFormats;
    if(QtCameraJni::getSupportedImageFormats().contains(256))
    {
        supportedFormats.append(QLatin1String("JPEG"));
    }
    if(QtCameraJni::getSupportedImageFormats().contains(17))
    {
        supportedFormats.append(QLatin1String("NV21"));
    }
    if(QtCameraJni::getSupportedImageFormats().contains(4))
    {
        supportedFormats.append(QLatin1String("RGB_565"));
    }
    if(QtCameraJni::getSupportedImageFormats().contains(20))
    {
        supportedFormats.append(QLatin1String("YUY2"));
    }

    return supportedFormats;
}

QString AndroidCamImageEncoderControl::imageCodecDescription(const QString &codec) const
{
    if(codec == QLatin1String("JPEG"))
    {
        return QString("JPEG image codec");
    }
    else
    {
        return QString();
    }
}

QList<QSize> AndroidCamImageEncoderControl::supportedResolutions(const QImageEncoderSettings &,
                                                                 bool *) const
{

    QList<QSize> supportedResolutions;

    for(int i= 0 ; i < QtCameraJni::getSupportedImageResolutions().count() ; i = i+2)
    {
        if(QtCameraJni::getSupportedImageResolutions().at(i) != 0)
        {
              supportedResolutions << QSize(QtCameraJni::getSupportedImageResolutions().at(i),QtCameraJni::getSupportedImageResolutions().at(i+1));
        }
    }
    return supportedResolutions;
}

void AndroidCamImageEncoderControl::setImageSettings(const QImageEncoderSettings &settings)
{
    m_imageSettings.clear();
    if(settings.codec() == QLatin1String("JPEG"))
    {
        m_imageSettings.append(256);
    }
    else if(settings.codec() == QLatin1String("NV21"))
    {
        m_imageSettings.append(17);
    }
    else if(settings.codec() == QLatin1String("RGB_565"))
    {
        m_imageSettings.append(4);
    }
    else if(settings.codec() == QLatin1String("YUY2"))
    {
        m_imageSettings.append(20);
    }
    else
    {
        m_imageSettings.append(256);
    }
    QSize resolution = settings.resolution();
    if(!resolution.isEmpty())
    {
        m_imageSettings.append(resolution.width());
        m_imageSettings.append(resolution.height());
    }
    else
    {
        m_imageSettings.append(QtCameraJni::getSupportedImageResolutions().at(0));
        m_imageSettings.append(QtCameraJni::getSupportedImageResolutions().at(1));
    }
    QtCameraJni::setImageSettings(m_imageSettings);
}

QImageEncoderSettings AndroidCamImageEncoderControl::imageSettings() const
{
    QImageEncoderSettings imageEncoderSettings;
    if(!m_imageSettings.isEmpty())
    {
        switch(m_imageSettings.at(0))
        {
        case 256:
            imageEncoderSettings.setCodec(QLatin1String("JPEG"));
            break;
        case 17:
            imageEncoderSettings.setCodec(QLatin1String("NV21"));
            break;
        case 4:
            imageEncoderSettings.setCodec(QLatin1String("RGB_565"));
            break;
        case 20:
            imageEncoderSettings.setCodec(QLatin1String("YUY2"));
            break;
        default:
            imageEncoderSettings.setCodec(QLatin1String("JPEG"));
            break;
        }
        imageEncoderSettings.setResolution(QSize(m_imageSettings.at(1),m_imageSettings.at(2)));

    }
    else
    {
        imageEncoderSettings.setCodec(QLatin1String("JPEG"));
        imageEncoderSettings.setResolution(QSize(QtCameraJni::getSupportedImageResolutions().at(0),QtCameraJni::getSupportedImageResolutions().at(1)));
    }
    return imageEncoderSettings;
}
