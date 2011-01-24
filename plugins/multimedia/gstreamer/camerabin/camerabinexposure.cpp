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

#include "camerabinexposure.h"
#include "camerabinsession.h"
#include <gst/interfaces/photography.h>

#include <QDebug>

CameraBinExposure::CameraBinExposure(CameraBinSession *session)
    :QCameraExposureControl(session),
     m_session(session)
{
}

CameraBinExposure::~CameraBinExposure()
{
}

QCameraExposure::ExposureMode CameraBinExposure::exposureMode() const
{
    GstSceneMode sceneMode;
    gst_photography_get_scene_mode(m_session->photography(), &sceneMode);

    switch (sceneMode) {
    case GST_PHOTOGRAPHY_SCENE_MODE_PORTRAIT: return QCameraExposure::ExposurePortrait;
    case GST_PHOTOGRAPHY_SCENE_MODE_SPORT: return QCameraExposure::ExposureSports;
    case GST_PHOTOGRAPHY_SCENE_MODE_NIGHT: return QCameraExposure::ExposureNight;
    case GST_PHOTOGRAPHY_SCENE_MODE_MANUAL: return QCameraExposure::ExposureManual;
    case GST_PHOTOGRAPHY_SCENE_MODE_CLOSEUP:    //no direct mapping available so mapping to auto mode
    case GST_PHOTOGRAPHY_SCENE_MODE_LANDSCAPE:  //no direct mapping available so mapping to auto mode
    case GST_PHOTOGRAPHY_SCENE_MODE_AUTO:
    default:
         return QCameraExposure::ExposureAuto;
    }
}

void CameraBinExposure::setExposureMode(QCameraExposure::ExposureMode mode)
{
    GstSceneMode sceneMode;
    gst_photography_get_scene_mode(m_session->photography(), &sceneMode);

    switch (mode) {
    case QCameraExposure::ExposureManual: sceneMode = GST_PHOTOGRAPHY_SCENE_MODE_MANUAL; break;
    case QCameraExposure::ExposurePortrait: sceneMode = GST_PHOTOGRAPHY_SCENE_MODE_PORTRAIT; break;
    case QCameraExposure::ExposureSports: sceneMode = GST_PHOTOGRAPHY_SCENE_MODE_SPORT; break;
    case QCameraExposure::ExposureNight: sceneMode = GST_PHOTOGRAPHY_SCENE_MODE_NIGHT; break;
    case QCameraExposure::ExposureAuto: sceneMode = GST_PHOTOGRAPHY_SCENE_MODE_AUTO; break;
    default:
        break;
    }

    gst_photography_set_scene_mode(m_session->photography(), sceneMode);
}

bool CameraBinExposure::isExposureModeSupported(QCameraExposure::ExposureMode mode) const
{
    //Similar mode names can be found in gst as GstSceneMode
    return  mode == QCameraExposure::ExposureAuto ||
            mode == QCameraExposure::ExposurePortrait ||
            mode == QCameraExposure::ExposureSports ||
            mode == QCameraExposure::ExposureNight;

    //No direct mapping available for GST_PHOTOGRAPHY_SCENE_MODE_CLOSEUP and
    //GST_PHOTOGRAPHY_SCENE_MODE_LANDSCAPE
}

QCameraExposure::MeteringMode CameraBinExposure::meteringMode() const
{
    return QCameraExposure::MeteringMatrix;
}

void CameraBinExposure::setMeteringMode(QCameraExposure::MeteringMode mode)
{
    Q_UNUSED(mode);
}

bool CameraBinExposure::isMeteringModeSupported(QCameraExposure::MeteringMode mode) const
{
    return mode == QCameraExposure::MeteringMatrix;
}

bool CameraBinExposure::isParameterSupported(ExposureParameter parameter) const
{
    switch (parameter) {
    case QCameraExposureControl::ExposureCompensation:
    case QCameraExposureControl::ISO:
    case QCameraExposureControl::Aperture:
    case QCameraExposureControl::ShutterSpeed:
        return true;
    default:
        return false;
    }
}

QVariant CameraBinExposure::exposureParameter(ExposureParameter parameter) const
{
    switch (parameter) {
    case QCameraExposureControl::ExposureCompensation:
        {
            gfloat ev;
            gst_photography_get_ev_compensation(m_session->photography(), &ev);
            return QVariant(ev);
        }
    case QCameraExposureControl::ISO:
        {
            guint isoSpeed = 0;
            gst_photography_get_iso_speed(m_session->photography(), &isoSpeed);
            return QVariant(isoSpeed);
        }
    case QCameraExposureControl::Aperture:
        return QVariant(2.8);
    case QCameraExposureControl::ShutterSpeed:
        {
            guint32 shutterSpeed = 0;
            gst_photography_get_exposure(m_session->photography(), &shutterSpeed);

            return QVariant(shutterSpeed/1000000.0);
        }
    default:
        return QVariant();
    }
}

QCameraExposureControl::ParameterFlags CameraBinExposure::exposureParameterFlags(ExposureParameter parameter) const
{
    QCameraExposureControl::ParameterFlags flags = 0;

    switch (parameter) {
    case QCameraExposureControl::ExposureCompensation:
        flags |= ContinuousRange;
        break;
    case QCameraExposureControl::Aperture:
        flags |= ReadOnly;
        break;
    default:
        break;
    }

    return flags;
}

QVariantList CameraBinExposure::supportedParameterRange(ExposureParameter parameter) const
{
    QVariantList res;
    switch (parameter) {
    case QCameraExposureControl::ExposureCompensation:
        res << -2.0 << 2.0;
        break;
    case QCameraExposureControl::ISO:
        res << 100 << 200 << 400;
        break;
    case QCameraExposureControl::Aperture:
        res << 2.8;
        break;
    default:
        break;
    }

    return res;
}

bool CameraBinExposure::setExposureParameter(ExposureParameter parameter, const QVariant& value)
{
    switch (parameter) {
    case QCameraExposureControl::ExposureCompensation:
        gst_photography_set_ev_compensation(m_session->photography(), value.toReal());
        break;
    case QCameraExposureControl::ISO:
        gst_photography_set_iso_speed(m_session->photography(), value.toInt());
        break;
    case QCameraExposureControl::Aperture:
        gst_photography_set_aperture(m_session->photography(), guint(value.toReal()*1000000));
        break;
    case QCameraExposureControl::ShutterSpeed:
        gst_photography_set_exposure(m_session->photography(), guint(value.toReal()*1000000));
        break;
    default:
        return false;
    }

    return true;
}

QString CameraBinExposure::extendedParameterName(ExposureParameter)
{
    return QString();
}
