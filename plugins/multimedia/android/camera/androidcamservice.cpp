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

#include <QtCore/qvariant.h>
#include <QtGui/qwidget.h>
#include <QtCore/qlist.h>
#include <androidcamservice.h>

AndroidCamService::AndroidCamService(QObject *parent) :
        QMediaService(parent)
{
    qDebug()<<"Android camera service started";
    // Different control classes implementing the Camera API
    m_control = new AndroidCamControl();
    m_focusControl = new AndroidCamFocusControl();
    m_exposureControl = new AndroidCamExposureControl();
    m_flashControl = new AndroidCamFlashControl();
    m_imageProcessingControl = new AndroidCamImageProcessingControl();
    m_imageCaptureControl = new AndroidCamImageCaptureControl(this);
    m_videoWidgetControl = new AndroidVideoWidgetControl();
    m_imageEncoderControl = new AndroidCamImageEncoderControl();

}

AndroidCamService::~AndroidCamService()
{
    // Delete controls
    if (m_focusControl)
        delete m_focusControl;
    if (m_exposureControl)
        delete m_exposureControl;
    if (m_flashControl)
        delete m_flashControl;
    if (m_imageProcessingControl)
        delete m_imageProcessingControl;
    if (m_imageCaptureControl)
        delete m_imageCaptureControl;
    if (m_imageEncoderControl)
        delete m_imageEncoderControl;
    if (m_control)
        delete m_control;
    if (m_videoWidgetControl)
        delete m_videoWidgetControl;

}

QMediaControl *AndroidCamService::requestControl(const char *name)
{
    if (qstrcmp(name, QCameraControl_iid) == 0)
        return m_control;

    if (qstrcmp(name, QCameraExposureControl_iid) == 0)
        return m_exposureControl;

    if (qstrcmp(name, QCameraFlashControl_iid) == 0)
        return m_flashControl;

    if (qstrcmp(name, QVideoWidgetControl_iid) == 0) {
        if (m_videoWidgetControl) {
            return m_videoWidgetControl;
        }
        else
            return 0;
    }

    if (qstrcmp(name, QCameraFocusControl_iid) == 0)
        return m_focusControl;

    if (qstrcmp(name, QCameraImageProcessingControl_iid) == 0)
        return m_imageProcessingControl;

    if (qstrcmp(name, QCameraImageCaptureControl_iid) == 0)
        return m_imageCaptureControl;

    if (qstrcmp(name, QImageEncoderControl_iid) == 0)
        return m_imageEncoderControl;

    return 0;
}

void AndroidCamService::releaseControl(QMediaControl *control)
{
    Q_UNUSED(control);
    // Resources reserved and released during Camera Service construction/destruction
}

int AndroidCamService::deviceCount()
{
    return 1;
}

QString AndroidCamService::deviceDescription(const int )
{
    return QLatin1String("Camera");
}

QString AndroidCamService::deviceName(const int )
{
    return QLatin1String("Camera");
}


