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

#include "androidgyroscope.h"
#include <unistd.h>

char const * const AndroidGyroscopeSensorBackend::id("android.gyroscope");
const long double AndroidGyroscopeSensorBackend::TODEGREES=57.295779513082320876846364344191;

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

void AndroidGyroscopeSensorBackend::dataAvailable(float data[],qint64 timeEvent,int )
{
    qreal x=data[0]*TODEGREES;
    qreal y=data[1]*TODEGREES;
    qreal z=data[2]*TODEGREES;
    m_reading.setX(x);
    m_reading.setY(y);
    m_reading.setZ(z);
    m_reading.setTimestamp(timeEvent/TOMICRO);
    newReadingAvailable();
}
