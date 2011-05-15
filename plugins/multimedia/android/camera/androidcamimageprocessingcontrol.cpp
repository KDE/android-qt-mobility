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

#include <androidcamimageprocessingcontrol.h>


AndroidCamImageProcessingControl::AndroidCamImageProcessingControl(QObject *parent) :
        QCameraImageProcessingControl(parent),
        m_whiteBalanceMode(QCameraImageProcessing::WhiteBalanceAuto)
{

}

AndroidCamImageProcessingControl::~AndroidCamImageProcessingControl()
{
}

bool AndroidCamImageProcessingControl::isWhiteBalanceModeSupported(QCameraImageProcessing::WhiteBalanceMode mode) const
{
    bool isSupported = false;
    switch(mode)
    {
    case QCameraImageProcessing::WhiteBalanceAuto:
        if(m_supportedWhiteBalanceModes.contains(QLatin1String("auto")))
        {
            isSupported = true;
        }
        break;
    case QCameraImageProcessing::WhiteBalanceIncandescent:
        if(m_supportedWhiteBalanceModes.contains(QLatin1String("incandescent")))
        {
            isSupported = true;
        }
        break;
    case QCameraImageProcessing::WhiteBalanceSunlight:
        if(m_supportedWhiteBalanceModes.contains(QLatin1String("daylight")))
        {
            isSupported = true;
        }
        break;
    case QCameraImageProcessing::WhiteBalanceCloudy:
        if(m_supportedWhiteBalanceModes.contains(QLatin1String("cloudy-daylight")))
        {
            isSupported = true;
        }
        break;
    case QCameraImageProcessing::WhiteBalanceFluorescent:
        if(m_supportedWhiteBalanceModes.contains(QLatin1String("fluorescent")))
        {
            isSupported = true;
        }
        break;
    case QCameraImageProcessing::WhiteBalanceShade:
        if(m_supportedWhiteBalanceModes.contains(QLatin1String("shade")))
        {
            isSupported = true;
        }
        break;
    case QCameraImageProcessing::WhiteBalanceSunset:
        if(m_supportedWhiteBalanceModes.contains(QLatin1String("twilight")))
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

QCameraImageProcessing::WhiteBalanceMode AndroidCamImageProcessingControl::whiteBalanceMode() const
{
    return m_whiteBalanceMode;
}
void AndroidCamImageProcessingControl::setWhiteBalanceMode(QCameraImageProcessing::WhiteBalanceMode mode)
{
    m_whiteBalanceMode = mode;
    switch(mode)
    {
    case QCameraImageProcessing::WhiteBalanceAuto:
        m_mode = QLatin1String("auto");
        break;

    case QCameraImageProcessing::WhiteBalanceIncandescent:
        m_mode = QLatin1String("incandescent");
        break;

    case QCameraImageProcessing::WhiteBalanceSunlight:
        m_mode = QLatin1String("daylight");
        break;

    case QCameraImageProcessing::WhiteBalanceCloudy:
        m_mode = QLatin1String("cloudy-daylight");
        break;

    case QCameraImageProcessing::WhiteBalanceFluorescent:
        m_mode = QLatin1String("fluorescent");
        break;

    case QCameraImageProcessing::WhiteBalanceShade:
        m_mode = QLatin1String("shade");
        break;

    case QCameraImageProcessing::WhiteBalanceSunset:
        m_mode = QLatin1String("twilight");
        break;

    default:
        m_mode = QLatin1String("auto");
        break;

    }
    QtCameraJni::setWhiteBalanceMode(m_mode);
}

bool AndroidCamImageProcessingControl::isProcessingParameterSupported(ProcessingParameter) const
{
    return false;
}
QVariant AndroidCamImageProcessingControl::processingParameter(QCameraImageProcessingControl::ProcessingParameter) const
{
    return QVariant();
}
void AndroidCamImageProcessingControl::setProcessingParameter(QCameraImageProcessingControl::ProcessingParameter, QVariant value)
{
    Q_UNUSED(value);
    //not supported
}
