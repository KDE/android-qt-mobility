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

#include <androidsensorbackend.h>

enum DataRate
{
    SENSOR_DELAY_FASTEST =0,
    SENSOR_DELAY_GAME =1,
    SENSOR_DELAY_UI =2,
    SENSOR_DELAY_NORMAL =3
};

AndroidSensorBackend::AndroidSensorBackend(QSensor *sensor,int uniqueId,QtSensorJNI::AndroidSensorType sensorType)
    : QSensorBackend(sensor),m_sensor(sensor),m_uniqueID(uniqueId),m_sensorType(sensorType)
{
}

AndroidSensorBackend::~AndroidSensorBackend()
{
    stop ();
}

void AndroidSensorBackend::start()
{
    int getDataConstant=0;
    switch(m_sensor->dataRate())
    {
    case 0:
        getDataConstant=SENSOR_DELAY_UI;
        break;
    case 1:
        getDataConstant=SENSOR_DELAY_NORMAL;
        break;
    case 2:
        getDataConstant=SENSOR_DELAY_GAME;
        break;
    case 3:
        getDataConstant=SENSOR_DELAY_FASTEST;
        break;
    default:
        getDataConstant=SENSOR_DELAY_NORMAL;
    }
    QtSensorJNI::start (getDataConstant,m_uniqueID,m_sensorType,this);
}

void AndroidSensorBackend::stop()
{
    QtSensorJNI::stop(m_uniqueID);
}
