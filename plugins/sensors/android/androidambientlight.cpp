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

#include "androidambientlight.h"
#include <unistd.h>

char const * const AndroidAmbientLightSensorBackend::id("android.ambientlight");
struct LuxLimit {
    int min;
    int max;
};
// Defines the min and max lux values that a given level has.
// These are used to add histeresis to the sensor.
// If the previous level is below a level, the lux must be at or above the minimum.
// If the previous level is above a level, the lux muyt be at or below the maximum.
static LuxLimit limits[] = {
    { 0,    0    }, // Undefined (not used)
    { 0,    5    }, // Dark
    { 10,   50   }, // Twilight
    { 100,  200  }, // Light
    { 500,  2000 }, // Bright
    { 5000, 0    }  // Sunny
};

AndroidAmbientLightSensorBackend::AndroidAmbientLightSensorBackend(QSensor *sensor,int uniqueId,QtSensorJNI::AndroidSensorType sensorType)
    : AndroidSensorBackend(sensor,uniqueId,sensorType)
{
    setReading<QAmbientLightReading>(&m_reading);
    addDataRate(0,3);
}

AndroidAmbientLightSensorBackend::~AndroidAmbientLightSensorBackend()
{

}



void AndroidAmbientLightSensorBackend::dataAvailable(float data[],qint64 timeEvent,int)
{
    enum {
        Undefined = QAmbientLightReading::Undefined,
        Dark = QAmbientLightReading::Dark,
        Twilight = QAmbientLightReading::Twilight,
        Light = QAmbientLightReading::Light,
        Bright = QAmbientLightReading::Bright,
        Sunny = QAmbientLightReading::Sunny
            };

    int lightLevel = m_reading.lightLevel();
    qreal lux = data[0];

    // Check for change direction to allow for histeresis
    if      (lightLevel < Sunny    && lux >= limits[Sunny   ].min) lightLevel = Sunny;
    else if (lightLevel < Bright   && lux >= limits[Bright  ].min) lightLevel = Bright;
    else if (lightLevel < Light    && lux >= limits[Light   ].min) lightLevel = Light;
    else if (lightLevel < Twilight && lux >= limits[Twilight].min) lightLevel = Twilight;
    else if (lightLevel < Dark     && lux >= limits[Dark    ].min) lightLevel = Dark;
    else if (lightLevel > Dark     && lux <= limits[Dark    ].max) lightLevel = Dark;
    else if (lightLevel > Twilight && lux <= limits[Twilight].max) lightLevel = Twilight;
    else if (lightLevel > Light    && lux <= limits[Light   ].max) lightLevel = Light;
    else if (lightLevel > Bright   && lux <= limits[Bright  ].max) lightLevel = Bright;

    if (static_cast<int>(m_reading.lightLevel()) != lightLevel)
    {
        m_reading.setTimestamp(timeEvent/TOMICRO);
        m_reading.setLightLevel(static_cast<QAmbientLightReading::LightLevel>(lightLevel));
        newReadingAvailable();
    }
}

