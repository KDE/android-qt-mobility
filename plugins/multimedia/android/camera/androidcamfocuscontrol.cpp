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

#include <androidcamfocuscontrol.h>


AndroidCamFocusControl::AndroidCamFocusControl(QObject *parent) :
        QCameraFocusControl(parent),
        m_mode(QCameraFocus::ManualFocus)
{
}

AndroidCamFocusControl::~AndroidCamFocusControl()
{

}

bool AndroidCamFocusControl::isFocusModeSupported(QCameraFocus::FocusMode mode) const
{
    bool isSupported = false;
    switch(mode)
    {
    case QCameraFocus::AutoFocus:
        if(QtCameraJni::getSupportedFocusModes().contains(QLatin1String("auto")))
        {
            isSupported = true;
        }
        break;
    case QCameraFocus::ContinuousFocus:
        if(QtCameraJni::getSupportedFocusModes().contains(QLatin1String("continuous-video")))
        {
            isSupported = true;
        }
        break;
    case QCameraFocus::ManualFocus:
        if(QtCameraJni::getSupportedFocusModes().contains(QLatin1String("fixed")))
        {
            isSupported = true;
        }
        break;
    case QCameraFocus::MacroFocus:
        if(QtCameraJni::getSupportedFocusModes().contains(QLatin1String("macro")))
        {
            isSupported = true;
        }
        break;
    case QCameraFocus::InfinityFocus:
        if(QtCameraJni::getSupportedFocusModes().contains(QLatin1String("infinity")))
        {
            isSupported = true;
        }
        break;
    case QCameraFocus::HyperfocalFocus:
        if(QtCameraJni::getSupportedFocusModes().contains(QLatin1String("edof")))
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

void AndroidCamFocusControl::setFocusMode(QCameraFocus::FocusMode mode)
{
    m_mode = mode;
    switch(mode)
    {
    case QCameraFocus::AutoFocus:
        m_focusMode = QLatin1String("auto");
        break;
    case QCameraFocus::ContinuousFocus:
        m_focusMode = QLatin1String("continuous-video");
        break;
    case QCameraFocus::ManualFocus:
        m_focusMode = QLatin1String("fixed");
        break;
    case QCameraFocus::MacroFocus:
        m_focusMode = QLatin1String("macro");
        break;
    case QCameraFocus::InfinityFocus:
        m_focusMode = QLatin1String("infinity");
        break;
    case QCameraFocus::HyperfocalFocus:
        m_focusMode = QLatin1String("edof");
        break;
    default:
        m_focusMode = QLatin1String("fixed");
        break;
    }
    QtCameraJni::setFocusMode(m_focusMode);
}

QCameraFocus::FocusMode AndroidCamFocusControl::focusMode() const
{
    return m_mode;
}

qreal AndroidCamFocusControl::maximumOpticalZoom() const
{
    return 1.0;
}

qreal AndroidCamFocusControl::maximumDigitalZoom() const
{
    if(QtCameraJni::getMaxZoom() == 0)
    {
        return 1.0;
    }
    else
    {
        return QtCameraJni::getMaxZoom();
    }
}

qreal  AndroidCamFocusControl::opticalZoom() const
{
    return qreal();
}
qreal  AndroidCamFocusControl::digitalZoom() const
{
    return (qreal)QtCameraJni::getZoom();
}

void AndroidCamFocusControl::zoomTo(qreal , qreal digital)
{
    QtCameraJni::setZoom((int)digital);
    emit digitalZoomChanged(digital);
}
QCameraFocus::FocusPointMode AndroidCamFocusControl::focusPointMode() const
{
    return QCameraFocus::FocusPointMode();
}
void AndroidCamFocusControl::setFocusPointMode(QCameraFocus::FocusPointMode)
{
    //not supported
}

bool AndroidCamFocusControl::isFocusPointModeSupported(QCameraFocus::FocusPointMode) const
{
    return false;
}
QPointF AndroidCamFocusControl::customFocusPoint() const
{
    return QPointF();
}
void AndroidCamFocusControl::setCustomFocusPoint(const QPointF &)
{
    //not supported
}

QCameraFocusZoneList AndroidCamFocusControl::focusZones() const
{
    return QCameraFocusZoneList();
}
