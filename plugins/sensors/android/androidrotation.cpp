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

#include "androidrotation.h"
#include <unistd.h>

char const * const AndroidRotationSensorBackend::id("android.rotation");

AndroidRotationSensorBackend::AndroidRotationSensorBackend(QSensor *sensor,int uniqueId,QtSensorJNI::AndroidSensorType sensorType)
    : AndroidSensorBackend(sensor,uniqueId,sensorType)
{
    setReading<QRotationReading>(&m_reading);
    addDataRate(0,3);
}

AndroidRotationSensorBackend::~AndroidRotationSensorBackend()
{
}

void AndroidRotationSensorBackend::dataAvailable(float data[],long timeEvent,int)
{
    //no support on android 2.2
    m_reading.setX(data[0]);
    m_reading.setY(data[1]);
    m_reading.setZ(data[2]);
    m_reading.setTimestamp(timeEvent);
    //newReadingAvailable();
}
