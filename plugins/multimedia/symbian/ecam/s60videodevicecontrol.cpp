/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/qstring.h>
#include <QtGui/qicon.h>

#include "s60videodevicecontrol.h"
#include "s60cameracontrol.h"
#include "s60cameraconstants.h"

S60VideoDeviceControl::S60VideoDeviceControl(QObject *parent) :
    QVideoDeviceControl(parent)
{
}

S60VideoDeviceControl::S60VideoDeviceControl(S60CameraControl *control, QObject *parent) :
    QVideoDeviceControl(parent),
    m_selectedDevice(KDefaultCameraDevice)
{
    if (control)
        m_control = control;
    else
        Q_ASSERT(true);
    // From now on it's safe to assume control exists

    connect(m_control, SIGNAL(devicesChanged()), this, SIGNAL(devicesChanged()));
}

S60VideoDeviceControl::~S60VideoDeviceControl()
{
}

int S60VideoDeviceControl::deviceCount() const
{
    return S60CameraControl::deviceCount();
}

QString S60VideoDeviceControl::deviceName(int index) const
{
    return S60CameraControl::name(index);
}

QString S60VideoDeviceControl::deviceDescription(int index) const
{
    return S60CameraControl::description(index);
}

QIcon S60VideoDeviceControl::deviceIcon(int index) const
{
    Q_UNUSED(index);
    return QIcon();
}

int S60VideoDeviceControl::defaultDevice() const
{
    return KDefaultCameraDevice;
}

int S60VideoDeviceControl::selectedDevice() const
{
    return m_selectedDevice;
}

void S60VideoDeviceControl::setSelectedDevice(int index)
{
    // Inform that we selected new device
    if (m_selectedDevice != index) {
        m_control->setSelectedDevice(index);
        m_selectedDevice = index;
        emit selectedDeviceChanged(m_selectedDevice);
        emit selectedDeviceChanged(deviceName(m_selectedDevice));
    }
}

// End of file
