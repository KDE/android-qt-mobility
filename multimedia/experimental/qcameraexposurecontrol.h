/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
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
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QCAMERAEXPOSURECONTROL_H
#define QCAMERAEXPOSURECONTROL_H

#include <multimedia/qmediacontrol.h>
#include <multimedia/qmediaobject.h>

#include <multimedia/experimental/qcamera.h>

class Q_MEDIA_EXPORT QCameraExposureControl : public QMediaControl
{
    Q_OBJECT

public:
    ~QCameraExposureControl();

    virtual QCamera::FlashMode flashMode() const = 0;
    virtual void setFlashMode(QCamera::FlashMode mode) = 0;
    virtual QCamera::FlashModes supportedFlashModes() const = 0;
    virtual bool isFlashReady() const = 0;

    virtual QCamera::ExposureMode exposureMode() const = 0;
    virtual void setExposureMode(QCamera::ExposureMode mode) = 0;
    virtual QCamera::ExposureModes supportedExposureModes() const = 0;

    virtual qreal exposureCompensation() const = 0;
    virtual void setExposureCompensation(qreal ev) = 0;

    virtual QCamera::MeteringMode meteringMode() const = 0;
    virtual void setMeteringMode(QCamera::MeteringMode mode) = 0;
    virtual QCamera::MeteringModes supportedMeteringModes() const = 0;

    virtual int isoSensitivity() const = 0;
    virtual int minimumIsoSensitivity() const = 0;
    virtual int maximumIsoSensitivity() const = 0;
    virtual QList<int> supportedIsoSensitivities() const = 0;
    virtual void setManualIsoSensitivity(int iso) = 0;
    virtual void setAutoIsoSensitivity() = 0;

    virtual qreal aperture() const = 0;
    virtual qreal minimumAperture() const = 0;
    virtual qreal maximumAperture() const = 0;
    virtual QList<qreal> supportedApertures() const = 0;
    virtual void setManualAperture(qreal aperture) = 0;
    virtual void setAutoAperture() = 0;

    virtual qreal shutterSpeed() const = 0;
    virtual qreal minimumShutterSpeed() const = 0;
    virtual qreal maximumShutterSpeed() const = 0;
    virtual QList<qreal> supportedShutterSpeeds() const = 0;
    virtual void setManualShutterSpeed(qreal seconds) = 0;
    virtual void setAutoShutterSpeed() = 0;

    virtual bool isExposureLocked() const = 0;

public Q_SLOTS:
    virtual void lockExposure() = 0;
    virtual void unlockExposure() = 0;

Q_SIGNALS:
    void exposureLocked();
    void flashReady(bool);

    void apertureChanged(qreal);
    void apertureRangeChanged();
    void shutterSpeedChanged(qreal);
    void isoSensitivityChanged(int);

protected:
    QCameraExposureControl(QObject* parent = 0);
};

#define QCameraExposureControl_iid "com.nokia.Qt.QCameraExposureControl/1.0"
Q_MEDIA_DECLARE_CONTROL(QCameraExposureControl, QCameraExposureControl_iid)

#endif  // QCAMERACONTROL_H

