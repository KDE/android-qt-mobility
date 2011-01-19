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

#include "androidgyroscope.h"
#include <unistd.h>

char const * const AndroidGyroscopeSensorBackend::id("android.gyroscope");
const double AndroidGyroscopeSensorBackend::piBy180=0.017453292519943295769236907684886;

AndroidGyroscopeSensorBackend::AndroidGyroscopeSensorBackend(QSensor *sensor,int uniqueId
                                                             ,QtSensorJNI::AndroidSensorType sensorType)
                                                                 : AndroidSensorBackend(sensor,uniqueId,sensorType)
{
    setReading<QGyroscopeReading>(&m_reading);
    addDataRate(0,3);
}

AndroidGyroscopeSensorBackend::~AndroidGyroscopeSensorBackend()
{
}

void AndroidGyroscopeSensorBackend::dataAvailable(float data[],long timeEvent,int )
{
    qreal x=data[0]*piBy180;
    qreal y=data[1]*piBy180;
    qreal z=data[2]*piBy180;
    m_reading.setX(x);
    m_reading.setY(y);
    m_reading.setZ(z);
    m_reading.setTimestamp(timeEvent);
    newReadingAvailable();
}
