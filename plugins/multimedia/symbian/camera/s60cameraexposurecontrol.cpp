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

#include "s60cameraexposurecontrol.h"
#include "s60cameraservice.h"
#include "s60camerasession.h"

#include <QtCore/qdebug.h>
#include <QtCore/qstring.h>


S60CameraExposureControl::S60CameraExposureControl(QObject *parent)
    :QCameraExposureControl(parent)
{
}

S60CameraExposureControl::S60CameraExposureControl(QObject *session, QObject *parent)
   :QCameraExposureControl(parent)
    , m_error(QCamera::NoError)
    , m_flashMode(QCamera::FlashOff)
    , m_exposureMode(QCamera::ExposureAuto)
    , m_meteringMode(QCamera::MeteringMatrix)
    , m_ev(0.0)
{
    // use cast if we want to change session class later on..
    m_session = qobject_cast<S60CameraSession*>(session);
    m_advancedSettings = m_session->advancedSettings();
    
    connect(m_advancedSettings, SIGNAL(exposureLocked()), this, SIGNAL(exposureLocked()));
    connect(m_advancedSettings, SIGNAL(flashReady(bool)), this, SIGNAL(flashReady(bool)));
    connect(m_advancedSettings, SIGNAL(apertureChanged(qreal)), this, SIGNAL(apertureChanged(qreal)));
    connect(m_advancedSettings, SIGNAL(apertureRangeChanged()), this, SIGNAL(apertureRangeChanged()));
    connect(m_advancedSettings, SIGNAL(shutterSpeedChanged(qreal)), this, SIGNAL(shutterSpeedChanged(qreal)));
    connect(m_advancedSettings, SIGNAL(isoSensitivityChanged(int)), this, SIGNAL(isoSensitivityChanged(int)));
}

S60CameraExposureControl::~S60CameraExposureControl()
{
    m_advancedSettings = NULL;
}


QCamera::FlashMode S60CameraExposureControl::flashMode() const
{
    return m_session->flashMode();
}

void S60CameraExposureControl::setFlashMode(QCamera::FlashMode mode)
{
    QCamera::FlashModes supportedModes = supportedFlashModes();
    if (supportedModes & mode) {
        m_flashMode = mode;
        m_session->setFlashMode(m_flashMode);
    }
}

QCamera::FlashModes S60CameraExposureControl::supportedFlashModes() const
{
    return m_session->supportedFlashModes();
}

bool S60CameraExposureControl::isFlashReady() const
{
    return m_advancedSettings->isFlashReady();
}

QCamera::ExposureMode S60CameraExposureControl::exposureMode() const
{
    return m_session->exposureMode();
}

void S60CameraExposureControl::setExposureMode(QCamera::ExposureMode mode)
{
    //qDebug() << "S60CameraExposureControl::setExposureMode";
    QCamera::ExposureModes supportedModes = supportedExposureModes();
    if (supportedModes & mode) {
        m_exposureMode = mode;
        //qDebug() << "Set exposure mode";
        m_session->setExposureMode(m_exposureMode);
    } 
}

QCamera::ExposureModes S60CameraExposureControl::supportedExposureModes() const
{
    return m_session->supportedExposureModes();
}

qreal S60CameraExposureControl::exposureCompensation() const
{
    return m_advancedSettings->exposureCompensation();
}

void S60CameraExposureControl::setExposureCompensation(qreal ev)
{
    m_advancedSettings->setExposureCompensation(ev);
    m_ev = ev;
}

QCamera::MeteringMode S60CameraExposureControl::meteringMode() const
{
    return m_advancedSettings->meteringMode();
}

void S60CameraExposureControl::setMeteringMode(QCamera::MeteringMode mode)
{
    QCamera::MeteringModes supportedModes = supportedMeteringModes();
    if (supportedModes & mode) {
        m_meteringMode = mode;
        m_advancedSettings->setMeteringMode(mode);
    } 
}

QCamera::MeteringModes S60CameraExposureControl::supportedMeteringModes() const
{
    return m_advancedSettings->supportedMeteringModes();
}

int S60CameraExposureControl::isoSensitivity() const
{
    return m_advancedSettings->isoSensitivity();
}

QList<int> S60CameraExposureControl::supportedIsoSensitivities(bool *continuous) const
{
    Q_UNUSED(continuous);
    return m_advancedSettings->supportedIsoSensitivities();
}

void S60CameraExposureControl::setManualIsoSensitivity(int iso)
{
    int minIso = supportedIsoSensitivities().first();
    int maxIso = supportedIsoSensitivities().last();
    if (iso < minIso) {
        iso = minIso;
    } else if (iso > maxIso) {
        iso = maxIso;
    }
    m_advancedSettings->setManualIsoSensitivity(iso);
}

void S60CameraExposureControl::setAutoIsoSensitivity()
{
    m_error = QCamera::NotSupportedFeatureError;   
}

qreal S60CameraExposureControl::aperture() const
{
    return m_advancedSettings->aperture();
}

QList<qreal> S60CameraExposureControl::supportedApertures(bool *continuous) const
{
    return m_advancedSettings->supportedApertures(continuous);
}

void S60CameraExposureControl::setManualAperture(qreal aperture)
{
    int minIso = supportedApertures().first();
    int maxIso = supportedApertures().last();
    if (aperture < minIso) {
        aperture = minIso;
    } else if (aperture > maxIso) {
        aperture = maxIso;
    }
    m_advancedSettings->setManualAperture(aperture);
}

void S60CameraExposureControl::setAutoAperture()
{
    m_error = QCamera::NotSupportedFeatureError;
}

qreal S60CameraExposureControl::shutterSpeed() const
{
    return m_advancedSettings->shutterSpeed();
}

/*
  Returns the list of shutter speed values if camera supports only fixed set of shutter speed values,
  otherwise returns an empty list.
 */
QList<qreal> S60CameraExposureControl::supportedShutterSpeeds(bool *continuous) const
{
    return m_advancedSettings->supportedShutterSpeeds(continuous);;
}

void S60CameraExposureControl::setManualShutterSpeed(qreal seconds)
{
    m_advancedSettings->setShutterSpeed(seconds);
}

void S60CameraExposureControl::setAutoShutterSpeed()
{
    m_error = QCamera::NotSupportedFeatureError;
}

bool S60CameraExposureControl::isExposureLocked() const
{
    return m_advancedSettings->isExposureLocked();
}

void S60CameraExposureControl::lockExposure()
{
    m_advancedSettings->lockExposure(true);
}

void S60CameraExposureControl::unlockExposure()
{
    m_advancedSettings->lockExposure(false);
}
