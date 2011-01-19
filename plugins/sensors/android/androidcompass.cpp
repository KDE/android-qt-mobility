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

#include "androidcompass.h"
#include <unistd.h>

char const * const AndroidCompassSensorBackend::id("android.compass");
AndroidCompassSensorBackend::AndroidCompassSensorBackend(QSensor *sensor,int uniqueId
                                                         ,QtSensorJNI::AndroidSensorType sensorType)
                                                             : AndroidSensorBackend(sensor,uniqueId,sensorType)
{
    setReading<QCompassReading>(&m_reading);
    addDataRate(0,3);
}

AndroidCompassSensorBackend::~AndroidCompassSensorBackend()
{
}


void AndroidCompassSensorBackend::dataAvailable(float data[],long timeEvent,int accuracy)
{
    // The scale for level is [0,3], where 3 is the best
    // Qt: Measured as a value from 0 to 1 with higher values being better.
    m_reading.setCalibrationLevel(accuracy);
    // The scale for degrees from sensord is [0,359]
    // Value can be directly used as azimuth
    m_reading.setAzimuth(data[0]);
    m_reading.setTimestamp(timeEvent);
    newReadingAvailable();
}
