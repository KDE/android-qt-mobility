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

#include "n900filebasedsensor.h"

n900filebasedsensor::n900filebasedsensor(QSensor *sensor)
    : QSensorBackend(sensor)
    , m_timerid(0)
{
    setSupportedUpdatePolicies(QSensor::OccasionalUpdates |
            QSensor::InfrequentUpdates |
            QSensor::FrequentUpdates |
            QSensor::TimedUpdates |
            QSensor::PolledUpdates);
}

void n900filebasedsensor::start()
{
    if (m_timerid)
        return;

    int interval = m_sensor->updateInterval();

    switch (m_sensor->updatePolicy()) {
    case QSensor::OccasionalUpdates:
        interval = 5000;
        break;
    case QSensor::InfrequentUpdates:
        interval = 1000;
        break;
    case QSensor::Undefined: /* fall through */
    case QSensor::FrequentUpdates:
        interval = 100;
        break;
    case QSensor::TimedUpdates:
        // already set
        break;
    default:
        interval = 0;
        break;
    }

    if (interval)
        m_timerid = startTimer(interval);
}

void n900filebasedsensor::stop()
{
    if (m_timerid) {
        killTimer(m_timerid);
        m_timerid = 0;
    }
}

void n900filebasedsensor::timerEvent(QTimerEvent * /*event*/)
{
    poll();
}

