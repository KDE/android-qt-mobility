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

#include <androidcamflashcontrol.h>

AndroidCamFlashControl::AndroidCamFlashControl(QObject *parent) :
        QCameraFlashControl(parent)
        ,m_flashMode(QCameraExposure::FlashOff)
{
}

AndroidCamFlashControl::~AndroidCamFlashControl()
{

}

bool AndroidCamFlashControl::isFlashModeSupported(QCameraExposure::FlashModes mode) const
{
    bool isSupported = false;
    switch(mode)
    {
    case QCameraExposure::FlashOn:
        if(QtCameraJni::getSupportedFlashModes().contains(QLatin1String("on")))
        {
            isSupported = true;
        }
        break;
    case QCameraExposure::FlashOff:
        if(QtCameraJni::getSupportedFlashModes().contains(QLatin1String("off")))
        {
            isSupported = true;
        }
        break;
    case QCameraExposure::FlashAuto:
        if(QtCameraJni::getSupportedFlashModes().contains(QLatin1String("auto")))
        {
            isSupported = true;
        }
        break;
    case QCameraExposure::FlashRedEyeReduction:
        if(QtCameraJni::getSupportedFlashModes().contains(QLatin1String("red-eye")))
        {
            isSupported = true;
        }
        break;
    case QCameraExposure::FlashTorch:
        if(QtCameraJni::getSupportedFlashModes().contains(QLatin1String("torch")))
        {
            isSupported = true;
        }
        break;
    default:
        isSupported =  false;
        break;
    }
    return isSupported;
}


QCameraExposure::FlashModes AndroidCamFlashControl::flashMode() const
{
    return m_flashMode;
}

void AndroidCamFlashControl::setFlashMode(QCameraExposure::FlashModes mode)
{
    m_flashMode = mode;
    switch(mode)
    {
    case QCameraExposure::FlashOn:
        m_mode = QLatin1String("on");
        break;
    case QCameraExposure::FlashOff:
        m_mode = QLatin1String("off");
        break;
    case QCameraExposure::FlashAuto:
        m_mode = QLatin1String("auto");
        break;
    case QCameraExposure::FlashRedEyeReduction:
        m_mode = QLatin1String("red-eye");
        break;
    case QCameraExposure::FlashTorch:
        m_mode = QLatin1String("torch");
        break;
    default:
        m_mode = QLatin1String("off");
        break;
    }
    QtCameraJni::setFlashMode(m_mode);
}

bool AndroidCamFlashControl::isFlashReady() const
{
    return true;
}

