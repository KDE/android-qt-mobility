/*
Copyright (c) 2011 Elektrobit (EB), All rights reserved.
Contact: oss-devel@elektrobit.com

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are
met:
* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* Neither the name of the Elektrobit (EB) nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY Elektrobit (EB) ''AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Elektrobit (EB) BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "androidorientation.h"
#include <unistd.h>
char const * const AndroidOrientationSensorBackend::id("android.orientation");

//error is taken to be 10 or 15 degrees
//based on testing with various android systems this conculsion was drawn

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

void AndroidOrientationSensorBackend::dataAvailable(float data[],qint64 timeEvent,int)
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
    else if (((data[1]>=170) && (data[1]<=190)) ||((data[1]>=-190)
        && (data[1]<=-170))|| ((data[1]<=10) && (data[1]>=-10)))
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
    m_reading.setTimestamp(timeEvent/TOMICRO);
    m_reading.setOrientation(o);
    newReadingAvailable();
}
