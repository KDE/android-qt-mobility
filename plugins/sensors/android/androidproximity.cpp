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


#include "androidproximity.h"
#include <unistd.h>

char const * const AndroidProximitySensorBackend::id("android.proximity");

AndroidProximitySensorBackend::AndroidProximitySensorBackend(QSensor *sensor,int uniqueId
                                                             ,QtSensorJNI::AndroidSensorType sensorType)
                                                                 : AndroidSensorBackend(sensor,uniqueId,sensorType)
{
    setReading<QProximityReading>(&m_reading);
    addDataRate(0,3); // 100Hz
}

AndroidProximitySensorBackend::~AndroidProximitySensorBackend()
{
}

void AndroidProximitySensorBackend::dataAvailable(float data[],long timeEvent,int)
{
    m_reading.setClose(data[0]? true: false);
    m_reading.setTimestamp(timeEvent*1000);
    newReadingAvailable();
}
