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

#ifndef ANDROIDCAMCONTROL_H
#define ANDROIDCAMCONTROL_H


#include <qcameracontrol.h>
#include <qcameraviewfinder.h>
#include <androidcamjni.h>
#include <QDebug>



QT_USE_NAMESPACE

class AndroidCamControl:public QCameraControl
{
    Q_OBJECT
public:
    AndroidCamControl(QObject *parent = 0);
    ~AndroidCamControl();
public: // QCameraControl

    // State
    QCamera::State state() const;
    void setState(QCamera::State state);

    // Status
    QCamera::Status status() const;

    // Capture Mode
    QCamera::CaptureMode captureMode() const;
    void setCaptureMode(QCamera::CaptureMode);
    bool isCaptureModeSupported(QCamera::CaptureMode mode) const;

    // Property Setting
    bool canChangeProperty(QCameraControl::PropertyChangeType changeType, QCamera::Status status) const;
    QCamera::Status   m_internalState;
    QCamera::State    m_requestedState;

    QCamera::CaptureMode  m_requestedCaptureMode;
    QCamera::CaptureMode        m_captureMode;

    void cameraLoad();
    void cameraUnload();
    void cameraActive();
    void cameraStop();

};









#endif
