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

#include "androidaccelerometer.h"
#include "androidambientlight.h"
#include "androidcompass.h"
#include "androidmagnetometer.h"
#include "androidorientation.h"
#include "androidproximity.h"
#include "androidrotation.h"
#include "androidgyroscope.h"
//#include "androidtapsensor.h"
#include <qsensorplugin.h>
#include <qsensorbackend.h>
#include <qsensormanager.h>
#include <androidsensorconstants.h>
extern "C"{
#include <unistd.h>
}

QTM_USE_NAMESPACE

        int getUniqueId ()
{
    static int uniqueId=0;
    return ++uniqueId;
}

class androidSensorPlugin : public QObject, public QSensorPluginInterface, public QSensorBackendFactory
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QSensorPluginInterface)
public:
            void registerSensors()
    {
        QSensorManager::registerBackend(QAccelerometer::type, AndroidAccelerometerSensorBackend::id, this);
        QSensorManager::registerBackend(QAmbientLightSensor::type, AndroidAmbientLightSensorBackend::id,this);
        QSensorManager::registerBackend(QCompass::type, AndroidCompassSensorBackend::id, this);
        QSensorManager::registerBackend(QMagnetometer::type, AndroidMagnetometerSensorBackend::id, this);
        QSensorManager::registerBackend(QOrientationSensor::type, AndroidOrientationSensorBackend::id, this);
        QSensorManager::registerBackend(QProximitySensor::type, AndroidProximitySensorBackend::id, this);
        QSensorManager::registerBackend(QRotationSensor::type, AndroidRotationSensorBackend::id, this);
        QSensorManager::registerBackend(QGyroscope::type, AndroidGyroscopeSensorBackend::id, this);
        //tapsensor is not implemented.if implemented remove the comments in line no.9 and 43
        //QSensorManager::registerBackend(QTapSensor::type, androidtapsensor::id, this);
        QtSensorJNI::registerSensor();

    }

    QSensorBackend *createBackend(QSensor *sensor)
    {
        QtSensorJNI::AndroidSensorType sensorType;
        if (sensor->identifier() == AndroidAccelerometerSensorBackend::id)
        {
            sensorType=QtSensorJNI::TYPE_ACCELEROMETER;
            return new AndroidAccelerometerSensorBackend(sensor,getUniqueId(),sensorType);
        }
        else if (sensor->identifier() == AndroidCompassSensorBackend::id)
        {
            sensorType=QtSensorJNI::TYPE_ORIENTATION;
            return new AndroidCompassSensorBackend(sensor,getUniqueId(),sensorType);
        }
        else if (sensor->identifier() == AndroidMagnetometerSensorBackend::id)
        {
            sensorType=QtSensorJNI::TYPE_MAGNETIC_FIELD;
            return new AndroidMagnetometerSensorBackend(sensor,getUniqueId(),sensorType);
        }
        else if (sensor->identifier() == AndroidOrientationSensorBackend::id)
        {
            sensorType=QtSensorJNI::TYPE_ORIENTATION;
            return new AndroidOrientationSensorBackend(sensor,getUniqueId(),sensorType);
        }
        else if (sensor->identifier() == AndroidProximitySensorBackend::id)
        {
            sensorType=QtSensorJNI::TYPE_PROXIMITY;
            return new AndroidProximitySensorBackend(sensor,getUniqueId(),sensorType);
        }
        else if (sensor->identifier() == AndroidRotationSensorBackend::id)
        {
            sensorType=QtSensorJNI::TYPE_ROTATION_VECTOR;
            return new AndroidRotationSensorBackend(sensor,getUniqueId(),sensorType);
        }
        else if(sensor->identifier() == AndroidGyroscopeSensorBackend::id)
        {
            sensorType=QtSensorJNI::TYPE_GYROSCOPE;
            return new AndroidGyroscopeSensorBackend(sensor,getUniqueId(),sensorType);
        }
        else if(sensor->identifier() == AndroidAmbientLightSensorBackend::id)
        {
            sensorType=QtSensorJNI::TYPE_LIGHT;
            return new AndroidAmbientLightSensorBackend(sensor,getUniqueId(),sensorType);
        }
        return 0;
    }
};

Q_EXPORT_PLUGIN2(libsensors_android, androidSensorPlugin)
#include "main.moc"

