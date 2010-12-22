/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qvideosurfaceoutput_p.h"

#include <qabstractvideosurface.h>
#include <qmediaservice.h>
#include <qvideorenderercontrol.h>


QVideoSurfaceOutput::QVideoSurfaceOutput(QObject*parent)
    :  QObject(parent)
{
}

QVideoSurfaceOutput::~QVideoSurfaceOutput()
{
    if (m_control) {
        m_control.data()->setSurface(0);
        m_service.data()->releaseControl(m_control.data());
    }
}

QMediaObject *QVideoSurfaceOutput::mediaObject() const
{
    return m_object.data();
}

void QVideoSurfaceOutput::setVideoSurface(QAbstractVideoSurface *surface)
{
    m_surface = surface;

    if (m_control)
        m_control.data()->setSurface(surface);
}

bool QVideoSurfaceOutput::setMediaObject(QMediaObject *object)
{
    if (m_control) {
        m_control.data()->setSurface(0);
        m_service.data()->releaseControl(m_control.data());
    }
    m_control.clear();
    m_service.clear();
    m_object.clear();

    if (object) {
        if (QMediaService *service = object->service()) {
            if (QMediaControl *control = service->requestControl(QVideoRendererControl_iid)) {
                if ((m_control = qobject_cast<QVideoRendererControl *>(control))) {
                    m_service = service;
                    m_object = object;
                    m_control.data()->setSurface(m_surface.data());

                    return true;
                }
                service->releaseControl(control);
            }
        }
    }
    return false;
}
