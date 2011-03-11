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

#ifndef ANDROIDCAMFOCUSCONTROL_H
#define ANDROIDCAMFOCUSCONTROL_H


#include <qcamerafocuscontrol.h>
#include <androidcamjni.h>

QT_USE_NAMESPACE

class AndroidCamFocusControl : public QCameraFocusControl
{
    Q_OBJECT

public: // Constructors & Destructor

    AndroidCamFocusControl(QObject *parent = 0);
    ~AndroidCamFocusControl();

public: // QCameraFocusControl

    // Focus Mode
    QCameraFocus::FocusMode focusMode() const;
    void setFocusMode(QCameraFocus::FocusMode mode);
    bool isFocusModeSupported(QCameraFocus::FocusMode) const;

    // Zoom
    qreal maximumOpticalZoom() const;
    qreal maximumDigitalZoom() const;
    qreal opticalZoom() const;
    qreal digitalZoom() const;

    void zoomTo(qreal optical, qreal digital);

    // Focus Point
    QCameraFocus::FocusPointMode focusPointMode() const;
    void setFocusPointMode(QCameraFocus::FocusPointMode mode);
    bool isFocusPointModeSupported(QCameraFocus::FocusPointMode mode) const;
    QPointF customFocusPoint() const;
    void setCustomFocusPoint(const QPointF &point);

    QCameraFocusZoneList focusZones() const;
private:
    QString m_focusMode;
    QCameraFocus::FocusMode m_mode;

};

#endif // ANDROIDCAMFOCUSCONTROL_H
