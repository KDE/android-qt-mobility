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

#include <androidcamexposurecontrol.h>



AndroidCamExposureControl::AndroidCamExposureControl(QObject *parent) :
        QCameraExposureControl(parent),
        m_mode(QCameraExposure::ExposureAuto),
        m_exposureParameter ((QVariant)0)
{
}

AndroidCamExposureControl::~AndroidCamExposureControl()
{

}

bool AndroidCamExposureControl::isExposureModeSupported(QCameraExposure::ExposureMode mode) const
{
    bool isSupported = false;
    switch(mode)
    {
    case QCameraExposure::ExposureAuto:
        if(QtCameraJni::getSupportedSceneModes().contains(QLatin1String("auto")))
        {
            isSupported = true;
        }
        break;
    case QCameraExposure::ExposureNight:
        if(QtCameraJni::getSupportedSceneModes().contains(QLatin1String("night")))
        {
            isSupported = true;
        }
        break;
    case QCameraExposure::ExposureSports:
        if(QtCameraJni::getSupportedSceneModes().contains(QLatin1String("sports")))
        {
            isSupported = true;
        }
        break;
    case QCameraExposure::ExposureSnow:
        if(QtCameraJni::getSupportedSceneModes().contains(QLatin1String("snow")))
        {
            isSupported = true;
        }
        break;
    case QCameraExposure::ExposureBeach:
        if(QtCameraJni::getSupportedSceneModes().contains(QLatin1String("beach")))
        {
            isSupported = true;
        }
        break;
    case QCameraExposure::ExposurePortrait:
        if(QtCameraJni::getSupportedSceneModes().contains(QLatin1String("portrait")))
        {
            isSupported = true;
        }
        break;
    default:
        isSupported = false;
        break;

    }
    return isSupported;
}

void AndroidCamExposureControl::setExposureMode(QCameraExposure::ExposureMode mode)
{
    m_mode = mode;
    switch(mode)
    {
    case QCameraExposure::ExposureAuto:
        m_scene = QLatin1String("auto");
        break;
    case QCameraExposure::ExposureNight:
        m_scene = QLatin1String("night");
        break;
    case QCameraExposure::ExposureSports:
        m_scene =QLatin1String("sports");
        break;
    case QCameraExposure::ExposureSnow:
        m_scene = QLatin1String("snow");
        break;
    case QCameraExposure::ExposureBeach:
        m_scene = QLatin1String("beach");
        break;
    case QCameraExposure::ExposurePortrait:
        m_scene = QLatin1String("portrait");
        break;
    default:
        break;

    }
    QtCameraJni::setSceneMode(m_scene);
}

QCameraExposure::ExposureMode AndroidCamExposureControl::exposureMode() const
{
    return m_mode;
}

bool AndroidCamExposureControl::isMeteringModeSupported(QCameraExposure::MeteringMode ) const
{
    return false;
}

void AndroidCamExposureControl::setMeteringMode(QCameraExposure::MeteringMode )
{
    //metering mode not supported
}

QCameraExposure::MeteringMode AndroidCamExposureControl::meteringMode() const
{
    return QCameraExposure::MeteringMode();
}

bool AndroidCamExposureControl::isParameterSupported(ExposureParameter parameter) const
{
    switch(parameter)
    {
    case QCameraExposureControl::Aperture:
    case QCameraExposureControl::ShutterSpeed:
    case QCameraExposureControl::ISO:
    case QCameraExposureControl::FlashPower:
        return false;
    case QCameraExposureControl::ExposureCompensation:
        return true;
    default:
        return false;
    }

}

QVariant AndroidCamExposureControl::exposureParameter(ExposureParameter parameter) const
{
    if(parameter == QCameraExposureControl::ExposureCompensation)
    {
        return m_exposureParameter;
    }
    else
    {
        return QVariant();
    }


}

QCameraExposureControl::ParameterFlags AndroidCamExposureControl::exposureParameterFlags(ExposureParameter parameter) const
{
    if(parameter == QCameraExposureControl::ExposureCompensation)
    {
        return QCameraExposureControl::ContinuousRange;
    }
    else
    {
        return 0;
    }

}

QVariantList AndroidCamExposureControl::supportedParameterRange(ExposureParameter parameter) const
{

    if(parameter == QCameraExposureControl::ExposureCompensation)
    {
        QVariantList values;
        QList<int> compensationRange;
        QtCameraJni::getCompensationRange((int *)m_range);
        for(int i =m_range[0];i<m_range[1];i++)
        {
            values.append((QVariant)compensationRange[i]);
        }

        return values;

    }
    else
    {
        return QVariantList();
    }


}

bool AndroidCamExposureControl::setExposureParameter(ExposureParameter parameter, const QVariant& value)
{
    m_exposureParameter = value;
    if(parameter == QCameraExposureControl::ExposureCompensation)
    {
        QtCameraJni::setCompensation(value.toInt());
        return true;
    }
    return false;
}

QString AndroidCamExposureControl::extendedParameterName(ExposureParameter parameter)
{
    switch (parameter) {

    case QCameraExposureControl::ExposureCompensation:
        return QString("Exposure Compensation");
    default:
        return QString();
    }
}



