/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef S60CAMERAEXPOSURECONTROL_H
#define S60CAMERAEXPOSURECONTROL_H

#include <QtCore/qobject.h>
#include "qcameraexposurecontrol.h"

class S60CameraService;
class S60CameraSession;

class S60CameraExposureControl : public QCameraExposureControl
{
    Q_OBJECT
public:
    S60CameraExposureControl(QObject *parent = 0);
    S60CameraExposureControl(QObject *session, QObject *parent = 0);
    ~S60CameraExposureControl();

    QCamera::FlashMode flashMode() const;
    void setFlashMode(QCamera::FlashMode mode);
    QCamera::FlashModes supportedFlashModes() const;
    bool isFlashReady() const;

    QCamera::ExposureMode exposureMode() const;
    void setExposureMode(QCamera::ExposureMode mode);
    QCamera::ExposureModes supportedExposureModes() const;

    qreal exposureCompensation() const;
    void setExposureCompensation( qreal ev);

    QCamera::MeteringMode meteringMode() const;
    void setMeteringMode(QCamera::MeteringMode mode);
    QCamera::MeteringModes supportedMeteringModes() const;
    // todo move this to S60ImageProcessingControl class
    QCamera::WhiteBalanceMode whiteBalanceMode() const;
    void setWhiteBalanceMode(QCamera::WhiteBalanceMode mode);
    QCamera::WhiteBalanceModes supportedWhiteBalanceModes() const;
    int manualWhiteBalance() const;
    void setManualWhiteBalance(int colorTemperature);
    //end move
    int isoSensitivity() const;
    QPair<int, int> supportedIsoSensitivityRange() const;
    void setManualIsoSensitivity(int iso);
    void setAutoIsoSensitivity();

    qreal aperture() const;
    QPair< qreal,  qreal> supportedApertureRange() const;
    void setManualAperture( qreal aperture);
    void setAutoAperture();

    qreal shutterSpeed() const;
    QPair< qreal,  qreal> supportedShutterSpeedRange() const;
    void setManualShutterSpeed( qreal seconds);
    void setAutoShutterSpeed();

    bool isExposureLocked() const;
    
public Q_SLOTS:
    void lockExposure();
    void unlockExposure();

private:
    S60CameraSession *m_session;
    S60CameraService *m_service;
};

#endif
