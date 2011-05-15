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

#include "n900lightsensor.h"
#include <QFile>
#include <QDebug>

char const * const n900lightsensor::id("n900.light");
char const * const n900lightsensor::filename("/sys/class/i2c-adapter/i2c-2/2-0029/lux");

n900lightsensor::n900lightsensor(QSensor *sensor)
    : n900filebasedsensor(sensor)
{
    setReading<QLightReading>(&m_reading);
    // Sensor takes 12-400ms to complete one reading and is triggered by
    // a read of the /sys file (no interrupt/timing loop/etc. is used).
    addDataRate(1, 2);
    setDescription(QLatin1String("tsl2563"));

    sensor->setProperty("fieldOfView", 1); // very narrow field of view.
}

void n900lightsensor::start()
{
    if (!QFile::exists(QLatin1String(filename)))
        goto error;

    n900filebasedsensor::start();
    return;

error:
    sensorStopped();
}

void n900lightsensor::poll()
{
    FILE *fd = fopen(filename, "r");
    if (!fd) return;
    int lux;
    int rs = fscanf(fd, "%i", &lux);
    fclose(fd);
    if (rs != 1) return;

    if (m_reading.lux() != lux || m_reading.timestamp() == 0) {
        m_reading.setTimestamp(getTimestamp());
        m_reading.setLux(lux);

        newReadingAvailable();
    }
}

