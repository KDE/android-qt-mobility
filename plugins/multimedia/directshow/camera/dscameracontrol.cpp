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

#include <QtCore/qdebug.h>

#include "dscameracontrol.h"
#include "dscameraservice.h"
#include "dscamerasession.h"

QT_BEGIN_NAMESPACE

DSCameraControl::DSCameraControl(QObject *parent)
    :QCameraControl(parent), m_captureMode(QCamera::CaptureStillImage)
{
    m_session = qobject_cast<DSCameraSession*>(parent);
    connect(m_session, SIGNAL(stateChanged(QCamera::State)),this, SIGNAL(stateChanged(QCamera::State)));
}

DSCameraControl::~DSCameraControl()
{
}

void DSCameraControl::setState(QCamera::State state)
{
    switch (state) {
        case QCamera::ActiveState:
            start();
            break;
        case QCamera::UnloadedState: /* fall through */
        case QCamera::LoadedState:
            stop();
            break;
    }
}

bool DSCameraControl::isCaptureModeSupported(QCamera::CaptureMode mode) const
{
    bool bCaptureSupported = false;
    switch (mode) {
    case QCamera::CaptureStillImage:
        bCaptureSupported = true;
        break;
    case QCamera::CaptureVideo:
        bCaptureSupported = false;
        break;
    }
    return bCaptureSupported;
}

void DSCameraControl::start()
{
    m_session->record();
}

void DSCameraControl::stop()
{
    m_session->stop();
}

QCamera::State DSCameraControl::state() const
{
    return (QCamera::State)m_session->state();
}

QT_END_NAMESPACE
