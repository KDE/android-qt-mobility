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

#include "genericorientationsensor.h"
#include <QDebug>

const char *genericorientationsensor::id("generic.orientation");

genericorientationsensor::genericorientationsensor(QSensor *sensor)
    : QSensorBackend(sensor)
{
    accelerometer = new QAccelerometer(this);
    accelerometer->setUpdatePolicy(QSensor::FrequentUpdates);
    accelerometer->addFilter(this);

    setSupportedUpdatePolicies(QSensor::OnChangeUpdates);
    setReading<QOrientationReading>(&m_reading);
}

void genericorientationsensor::start()
{
    accelerometer->start();
}

void genericorientationsensor::stop()
{
    accelerometer->stop();
}

void genericorientationsensor::poll()
{
    accelerometer->poll();
}

bool genericorientationsensor::filter(QAccelerometerReading *reading)
{
    QOrientationReading::Orientation o = m_reading.orientation();

    if (reading->x() < -7.35) {
        o = QOrientationReading::LeftUp;
    } else if (reading->y() < -7.35) {
        o = QOrientationReading::BottomUp;
    } else if (reading->x() > 7.35) {
        o = QOrientationReading::RightUp;
    } else if (reading->y() > 7.35) {
        o = QOrientationReading::BottomDown;
    } else if (reading->z() > 7.35) {
        o = QOrientationReading::FaceUp;
    } else if (reading->z() < -7.35) {
        o = QOrientationReading::FaceDown;
    }

    if (o != m_reading.orientation()) {
        m_reading.setTimestamp(reading->timestamp());
        m_reading.setOrientation(o);
        newReadingAvailable();
    }

    return false;
}

