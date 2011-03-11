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

#include <androidcamimagecapturecontrol.h>
#include <QTime>
#include <QFile>
#include <QDateTime>


AndroidCamImageCaptureControl::AndroidCamImageCaptureControl(AndroidCamService * service,QObject *parent) :
        QCameraImageCaptureControl(parent)
{
    m_service = service;

}

AndroidCamImageCaptureControl::~AndroidCamImageCaptureControl()
{
    m_service = NULL;
}

bool AndroidCamImageCaptureControl::isReadyForCapture() const
{

    if(m_service->m_control->m_requestedState == QCamera::ActiveState)
    {
        return true;
    }

    return false;
}

QCameraImageCapture::DriveMode AndroidCamImageCaptureControl::driveMode() const
{
    return m_driveMode;

}

void AndroidCamImageCaptureControl::setDriveMode(QCameraImageCapture::DriveMode mode)
{
    if (mode != QCameraImageCapture::SingleImageCapture) {
        return;
    }

    m_driveMode = mode;
}

int AndroidCamImageCaptureControl::capture(const QString &fileName)
{
    m_lock.lock();
    m_imagename.append(fileName);
    QtCameraJni::setCameraState(QtCameraJni::StartPreview);
    QtCameraJni::takePicture(this);
    m_lock.unlock();
    return 0;
}

void AndroidCamImageCaptureControl::ImageRecieved(QImage &image)
{
    m_lock.lock();
    const QImage imageTaken(image);
    QDateTime id = QDateTime::currentDateTime();
    QString requestID = id.toString("dd-MM-yy(hh_mm_ss)");
    emit imageCaptured(id.currentMSecsSinceEpoch(),image);
    if(m_imagename.isEmpty())
    {
        QString imageName(QLatin1String("/sdcard/")+requestID + QLatin1String(".jpg"));
        imageTaken.save(imageName);
        emit imageSaved(id.currentMSecsSinceEpoch(),(const QString)imageName);
    }
    else
    {
        QString imageName(QLatin1String("/sdcard/")+m_imagename+QLatin1String(".jpg"));
        image.save(imageName);
        emit imageSaved(id.currentMSecsSinceEpoch(),(const QString)imageName);
        m_imagename.clear();
    }
    m_lock.unlock();

}

void AndroidCamImageCaptureControl::cancelCapture()
{
    //not supported
}
