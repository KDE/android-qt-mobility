/****************************************************************************
**
** Copyright 2010 Elektrobit(EB)(http://www.elektrobit.com)
**
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
**
****************************************************************************/

#include "androidaccelerometer.h"
#include <unistd.h>

char const * const AndroidAccelerometerSensorBackend::id("android.accelerometer");

AndroidAccelerometerSensorBackend::AndroidAccelerometerSensorBackend(QSensor *sensor,int uniqueId
                                                                     ,QtSensorJNI::AndroidSensorType sensorType)
                                                                         : AndroidSensorBackend(sensor,uniqueId,sensorType)
{
    setReading<QAccelerometerReading>(&m_reading);
    addDataRate(0,3);
}

AndroidAccelerometerSensorBackend::~AndroidAccelerometerSensorBackend()
{
}

void AndroidAccelerometerSensorBackend::dataAvailable(float data[],long,int)
{
    qreal x=data[0];
    qreal y=-1 * data[1];
    qreal z=data[2];
    m_reading.setX(x);
    m_reading.setY(y);
    m_reading.setZ(z);
    newReadingAvailable();
}
