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

#include "s60camerasettings.h"

#include <QtCore/qdebug.h>
#ifdef USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
#include <ecamadvancedsettings.h> // CCameraAdvancedSettings (includes TValueInfo)
#endif
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER
#include <ecamadvsettings.h>   // CCameraAdvancedSettings
#include <ecam/ecamconstants.h>  // TValueInfo
#endif



S60CameraSettings::S60CameraSettings(QObject *parent, CCameraEngine *engine)
    : QObject(parent)
{
    m_cameraEngine = engine;
    queryAdvancedSettingsInfo();
}

S60CameraSettings::~S60CameraSettings()
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    m_advancedSettings = NULL;
#endif
}

/*
 * Queries advanced settings information
 * Results are returned to member variable m_advancedSettings
 * @return boolean indicating if querying the info was a success
 */
bool S60CameraSettings::queryAdvancedSettingsInfo()
{
    bool returnValue = false;
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (m_cameraEngine) {
        m_advancedSettings = NULL;
        m_advancedSettings = m_cameraEngine->AdvancedSettings();
        if (m_advancedSettings)
            returnValue = true;
    }
#endif
    return returnValue;
}

void S60CameraSettings::setFocusMode(QCamera::FocusMode mode)
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (m_advancedSettings) {
        switch(mode) {
            case QCamera::ManualFocus: // Manual focus mode
                //qDebug() << "Settings: set manual focus";
                m_advancedSettings->SetFocusMode(CCamera::CCameraAdvancedSettings::EFocusModeManual);
                break;
            case QCamera::AutoFocus: // One-shot auto focus mode
                //qDebug() << "Settings: set auto focus";
                m_advancedSettings->SetAutoFocusType(CCamera::CCameraAdvancedSettings::EAutoFocusTypeSingle);
                m_advancedSettings->SetFocusMode(CCamera::CCameraAdvancedSettings::EFocusModeAuto);
                break;
        }
    }
#endif
}

QCamera::FocusMode S60CameraSettings::focusMode()
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (m_advancedSettings) {
        CCamera::CCameraAdvancedSettings::TFocusMode mode = m_advancedSettings->FocusMode();
        switch(mode) {
            case CCamera::CCameraAdvancedSettings::EFocusModeManual:
                return QCamera::ManualFocus;
            case CCamera::CCameraAdvancedSettings::EFocusModeAuto:
                return QCamera::AutoFocus;
        }
    } 
#else

#endif
}

QCamera::FocusModes S60CameraSettings::supportedFocusModes()
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    TInt supportedModes = 0;
    TInt autoFocusTypes = 0;
    QCamera::FocusModes modes = QCamera::AutoFocus;
    if (m_advancedSettings) {
        supportedModes = m_advancedSettings->SupportedFocusModes();
        autoFocusTypes = m_advancedSettings->SupportedAutoFocusTypes();
        if (supportedModes == 0)
            return modes;
        if (supportedModes & CCamera::CCameraAdvancedSettings::EFocusModeAuto) {
            if (autoFocusTypes & CCamera::CCameraAdvancedSettings::EAutoFocusTypeSingle)
                modes |= QCamera::AutoFocus;
            if (autoFocusTypes & CCamera::CCameraAdvancedSettings::EAutoFocusTypeContinuous)
                modes |= QCamera::ContinuousFocus;
        }
    }
    return modes;
#endif
}

// from MCameraObserver2
void S60CameraSettings::HandleEvent(const TECAMEvent& aEvent)
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (aEvent.iEventType == KUidECamEventCameraSettingExposureLock) {
        emit exposureLocked();
    } else if (aEvent.iEventType == KUidECamEventCameraSettingAperture) {
        emit apertureChanged(aperture());
    } else if (aEvent.iEventType == KUidECamEventCameraSettingApertureRange) {
        emit apertureRangeChanged();
    } else if (aEvent.iEventType == KUidECamEventCameraSettingIsoRate) {
        emit isoSensitivityChanged(isoSensitivity());
    } else if (aEvent.iEventType == KUidECamEventCameraSettingShutterSpeed) {
        emit shutterSpeedChanged(shutterSpeed());
    } else if (aEvent.iEventType == KUidECamEventCameraSettingExposureLock) {
        emit exposureLocked();
    } else if (aEvent.iEventType == KUidECamEventFlashReady) {
        emit flashReady(true);
    } else if (aEvent.iEventType == KUidECamEventFlashNotReady) {
        emit flashReady(false);
    }
#endif
}

void S60CameraSettings::ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError)
{
    Q_UNUSED(aCameraBuffer);
    Q_UNUSED(aError);
}

void S60CameraSettings::ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError)
{
    Q_UNUSED(aCameraBuffer);
    Q_UNUSED(aError);
}
void S60CameraSettings::VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError)
{
    Q_UNUSED(aCameraBuffer);
    Q_UNUSED(aError);
}

bool S60CameraSettings::isFlashReady()
{
    TBool ready = false;
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (queryAdvancedSettingsInfo()) {
        int i = m_advancedSettings->IsFlashReady(ready);
        if (i == KErrNotSupported) {
            // TODO: error flash not supported
            return false;
        }
    }
#endif
    return ready;
}

void S60CameraSettings::setExposureCompensation(qreal ev)
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (queryAdvancedSettingsInfo()) {
        m_advancedSettings->SetExposureCompensation(ev);
    }
#endif
}

qreal S60CameraSettings::exposureCompensation()
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (queryAdvancedSettingsInfo()) {
        return m_advancedSettings->ExposureCompensation();
    } else {
        return 0;
    }
#else
    return 0;
#endif
}

QCamera::MeteringMode S60CameraSettings::meteringMode()
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (queryAdvancedSettingsInfo()) {
        CCamera::CCameraAdvancedSettings::TMeteringMode mode = m_advancedSettings->MeteringMode();
        switch(mode) {
            case CCamera::CCameraAdvancedSettings::EMeteringModeCenterWeighted:
                return QCamera::MeteringAverage;
            case CCamera::CCameraAdvancedSettings::EMeteringModeEvaluative:
                return QCamera::MeteringMatrix;
            case CCamera::CCameraAdvancedSettings::EMeteringModeSpot:
                return QCamera::MeteringSpot;
            default:
                break;
        }
    }
#else

#endif
}

void S60CameraSettings::setMeteringMode(QCamera::MeteringMode mode)
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (queryAdvancedSettingsInfo()) {
        switch(mode) {
            case QCamera::MeteringAverage:
                m_advancedSettings->SetMeteringMode(CCamera::CCameraAdvancedSettings::EMeteringModeCenterWeighted);
                break;
            case QCamera::MeteringMatrix:
                m_advancedSettings->SetMeteringMode(CCamera::CCameraAdvancedSettings::EMeteringModeEvaluative);
                break;
            case QCamera::MeteringSpot:
                m_advancedSettings->SetMeteringMode(CCamera::CCameraAdvancedSettings::EMeteringModeSpot);
                break;
            default:
                break;
        }
    }
#endif
}

QCamera::MeteringModes S60CameraSettings::supportedMeteringModes()
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    TInt supportedModes = 0;
    QCamera::MeteringModes modes = QCamera::MeteringAverage;
    if (queryAdvancedSettingsInfo()) {
        supportedModes = m_advancedSettings->SupportedMeteringModes();
        if (supportedModes == 0) {
            emit error(QCamera::NotSupportedFeatureError);
            return modes;
        } if (supportedModes & CCamera::CCameraAdvancedSettings::EMeteringModeCenterWeighted) {
             modes |= QCamera::MeteringAverage;
        } if (supportedModes & CCamera::CCameraAdvancedSettings::EMeteringModeEvaluative) {
             modes |= QCamera::MeteringMatrix;
        } if (supportedModes & CCamera::CCameraAdvancedSettings::EMeteringModeSpot) {
             modes |= QCamera::MeteringSpot;
        }
    }
    return modes;
#endif
}

int S60CameraSettings::isoSensitivity()
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (queryAdvancedSettingsInfo()) {
        return m_advancedSettings->IsoRate();
    } else {
        return 0;
    }
#endif
}

QList<int> S60CameraSettings::supportedIsoSensitivities()
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    QList<int> isoSentitivities;
    if (queryAdvancedSettingsInfo()) {
        RArray<TInt> supportedIsoRates;
        TRAPD(err, m_advancedSettings->GetSupportedIsoRatesL(supportedIsoRates));
        if (err != KErrNone) {
            emit error(QCamera::NotSupportedFeatureError);
        } else {
            for (int i=0; i < supportedIsoRates.Count(); i++) {
                int q = supportedIsoRates[i];
                isoSentitivities.append(q);
            }
        }
        supportedIsoRates.Close();
    }
    return isoSentitivities;
#endif
}

void S60CameraSettings::setManualIsoSensitivity(int iso)
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (queryAdvancedSettingsInfo()) {
        m_advancedSettings->SetIsoRate(iso);
    }
#endif
}

qreal S60CameraSettings::aperture()
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (queryAdvancedSettingsInfo()) {
        return m_advancedSettings->Aperture();
    } else {
        return -1.0;
    }
#endif
}

QList<qreal> S60CameraSettings::supportedApertures(bool *continuous)
{
    Q_UNUSED(continuous);
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    QList<qreal> apertures;
    if (queryAdvancedSettingsInfo()) {
        RArray<TInt> supportedApertures;
        TValueInfo info = ENotActive;
        TRAPD(err, m_advancedSettings->GetAperturesL(supportedApertures, info));
        if (err != KErrNone) {
            emit error(QCamera::NotSupportedFeatureError);
        } else {
            for (int i=0; i < supportedApertures.Count(); i++) {
                qreal q = supportedApertures[i];
                apertures.append(q);
            }
        }
        supportedApertures.Close();
    }
    return apertures;
#endif
}

void S60CameraSettings::setManualAperture(qreal aperture)
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (queryAdvancedSettingsInfo()) {
        m_advancedSettings->SetAperture(aperture);
    }
#endif
}

void S60CameraSettings::lockExposure(bool lock)
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (queryAdvancedSettingsInfo()) {
            m_advancedSettings->SetExposureLockOn(lock);
        }
#endif
}

bool S60CameraSettings::isExposureLocked()
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (queryAdvancedSettingsInfo()) {
        return m_advancedSettings->ExposureLockOn();
    } else {
        return false;
    }
#endif
}

TInt S60CameraSettings::shutterSpeed()
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (queryAdvancedSettingsInfo()) {
        return m_advancedSettings->ShutterSpeed();
    } else {
        return -1.0;
    }
#endif
}

QList<qreal> S60CameraSettings::supportedShutterSpeeds(bool *continuous)
{
    Q_UNUSED(continuous);
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    QList<qreal> speeds;
    if (queryAdvancedSettingsInfo()) {
        RArray<TInt> supportedSpeeds;
        TValueInfo info = ENotActive;
        TRAPD(err, m_advancedSettings->GetShutterSpeedsL(supportedSpeeds, info));
        if (err != KErrNone) {
            emit error(QCamera::NotSupportedFeatureError);
        } else {
            for (int i=0; i < supportedSpeeds.Count(); i++) {
                qreal q = supportedSpeeds[i];
                speeds.append(q);
            }
        }
        supportedSpeeds.Close();
    }
    return speeds;
#endif
}

void S60CameraSettings::setShutterSpeed(TInt speed)
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (queryAdvancedSettingsInfo()) {
        m_advancedSettings->SetShutterSpeed(speed);
    }
#endif
}

