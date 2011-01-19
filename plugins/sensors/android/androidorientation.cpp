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

#include "androidorientation.h"
#include <unistd.h>

char const * const AndroidOrientationSensorBackend::id("android.orientation");

AndroidOrientationSensorBackend::AndroidOrientationSensorBackend(QSensor *sensor,int uniqueId
                                                                 ,QtSensorJNI::AndroidSensorType sensorType)
                                                                     : AndroidSensorBackend(sensor,uniqueId,sensorType)
{
    setReading<QOrientationReading>(&m_reading);
    addDataRate(0,3);
}

AndroidOrientationSensorBackend::~AndroidOrientationSensorBackend()
{
}

void AndroidOrientationSensorBackend::dataAvailable(float data[],long timeEvent,int)
{
    QOrientationReading::Orientation o;
    if(data[2]>=-15 && data[2]<=15)
    {
        if(data[1]>=-15 && data[1]<=15)
        {
            o=QOrientationReading::FaceUp;
        }

        else
            if((data[1]>=(180-15) && data[1]<=(180+15))
                ||(data[1]>=(-180-15)&& data[1]<=(-180+15)))
                {
            o=QOrientationReading::FaceDown;

        }

        else
            if(data[1]>=(90-15) && data[1]<=(90+15))
            {
            o=QOrientationReading::TopDown;
        }

        else
            if(data[1]>=(-90-15) && data[1]<=(-90+15))
            {
            o=QOrientationReading::TopUp;
        }
    }
    else if ((data[1]>=170) && (data[1]<=190) ||(data[1]>=-190)
        && (data[1]<=-170)|| (data[1]<=10) && (data[1]>=10))
        {

        if(data[2]>=(-90-15) && data[2]<=(-90+15))
        {
            o=QOrientationReading::LeftUp;
        }
        else if(data[2]>=(90-15) && data[2]<=(90+15))
        {
            o=QOrientationReading::RightUp;
        }
    }

    else
    {
        o=QOrientationReading::Undefined;
    }
    m_reading.setTimestamp(timeEvent*1000);
    m_reading.setOrientation(o);
    newReadingAvailable();
}
