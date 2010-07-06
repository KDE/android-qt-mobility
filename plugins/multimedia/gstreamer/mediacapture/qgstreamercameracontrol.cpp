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

#include "qgstreamercameracontrol.h"

#include <QtCore/qdebug.h>
#include <QtCore/qfile.h>

#include <linux/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <linux/videodev2.h>


QGstreamerCameraControl::QGstreamerCameraControl(QGstreamerCaptureSession *session)
    :QCameraControl(session),
    m_captureMode(QCamera::CaptureDisabled),
    m_session(session),
    m_state(QCamera::StoppedState),
    m_requestedState(QCamera::StoppedState)
{
    connect(m_session, SIGNAL(stateChanged(QGstreamerCaptureSession::State)),
            this, SLOT(updateState()));
}

QGstreamerCameraControl::~QGstreamerCameraControl()
{
}

void QGstreamerCameraControl::setCaptureMode(QCamera::CaptureMode mode)
{
    m_captureMode = mode;
    switch (mode) {
    case QCamera::CaptureStillImage:
        m_session->setCaptureMode(QGstreamerCaptureSession::Image);
        setState(QCamera::IdleState);
        break;
    case QCamera::CaptureVideo:
        m_session->setCaptureMode(QGstreamerCaptureSession::AudioAndVideo);
        setState(QCamera::IdleState);
        break;
    case QCamera::CaptureDisabled:
        setState(QCamera::StoppedState);
    }

    emit captureModeChanged(mode);

    updateState();
}

void QGstreamerCameraControl::setState(QCamera::State state)
{
    m_requestedState = state;
    switch (state) {
    case QCamera::StoppedState:
    case QCamera::IdleState:
        m_session->setState(QGstreamerCaptureSession::StoppedState);
        break;
    case QCamera::ActiveState:
        if (m_session->state() == QGstreamerCaptureSession::StoppedState)
            m_session->setState(QGstreamerCaptureSession::PreviewState);
        break;
    default:
        emit error(QCamera::NotSupportedFeatureError, tr("State not supported."));
    }
}

QCamera::State QGstreamerCameraControl::state() const
{
    if (m_session->state() == QGstreamerCaptureSession::StoppedState)
        return m_requestedState == QCamera::StoppedState ? QCamera::StoppedState : QCamera::IdleState;
    else
        return QCamera::ActiveState;
}

void QGstreamerCameraControl::updateState()
{
    QCamera::State newState = state();
    if (m_state != newState) {
        m_state = newState;
        emit stateChanged(m_state);
    }
}
