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

#include "androidmagnetometer.h"
#include <unistd.h>

char const * const AndroidMagnetometerSensorBackend::id("android.magnetometer");
const float AndroidMagnetometerSensorBackend::MICRO=.000001;

AndroidMagnetometerSensorBackend::AndroidMagnetometerSensorBackend(QSensor *sensor,int uniqueId
                                                                   ,QtSensorJNI::AndroidSensorType sensorType)
                                                                       : AndroidSensorBackend(sensor,uniqueId,sensorType)
{
    setReading<QMagnetometerReading>(&m_reading);
    addDataRate(0,3);
}

AndroidMagnetometerSensorBackend::~AndroidMagnetometerSensorBackend()
{
}

void AndroidMagnetometerSensorBackend::dataAvailable(float data[],long timeEvent,int accuracy)
{
    m_reading.setX( MICRO * data[0]);
    m_reading.setY( MICRO * data[1]);
    m_reading.setZ( MICRO* data[2]);
    m_reading.setCalibrationLevel( accuracy);
    m_reading.setTimestamp(timeEvent*1000);
    newReadingAvailable();
}
