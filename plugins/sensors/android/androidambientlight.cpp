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
    addDataRate(0,5);
}

AndroidAmbientLightSensorBackend::~AndroidAmbientLightSensorBackend()
{

}



void AndroidAmbientLightSensorBackend::dataAvailable(float data[],long timeEvent,int)
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
        m_reading.setTimestamp(timeEvent);
        m_reading.setLightLevel(static_cast<QAmbientLightReading::LightLevel>(lightLevel));
        newReadingAvailable();
    }
}

