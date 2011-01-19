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
#include <QDebug>
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
        //if tapsensor is not implemented.if implemented remove the comments in line no.9 and 43
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

