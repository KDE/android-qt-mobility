/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "genericorientationsensor.h"
#include "genericrotationsensor.h"
#include "genericalssensor.h"
#include <qsensorplugin.h>
#include <qsensorbackend.h>
#include <qsensormanager.h>
#include <QFile>
#include <QDebug>

class genericSensorPlugin : public QObject, public QSensorPluginInterface, public QSensorChangesInterface, public QSensorBackendFactory
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QSensorPluginInterface QtMobility::QSensorChangesInterface)
public:
    void registerSensors()
    {
        qDebug() << "loaded the Generic plugin";
        // Nothing to register here
    }

    void sensorsChanged()
    {
        if (!QSensor::defaultSensorForType(QAccelerometer::type).isEmpty()) {
            // There is an accelerometer available. Register the backends
            if (!QSensorManager::isBackendRegistered(QOrientationSensor::type, genericorientationsensor::id))
                QSensorManager::registerBackend(QOrientationSensor::type, genericorientationsensor::id, this);
            if (!QSensorManager::isBackendRegistered(QRotationSensor::type, genericrotationsensor::id))
                QSensorManager::registerBackend(QRotationSensor::type, genericrotationsensor::id, this);
            if (!QSensorManager::isBackendRegistered(QAmbientLightSensor::type, genericalssensor::id))
                QSensorManager::registerBackend(QAmbientLightSensor::type, genericalssensor::id, this);
        } else {
            if (QSensorManager::isBackendRegistered(QOrientationSensor::type, genericorientationsensor::id))
                QSensorManager::unregisterBackend(QOrientationSensor::type, genericorientationsensor::id);
            if (QSensorManager::isBackendRegistered(QRotationSensor::type, genericrotationsensor::id))
                QSensorManager::unregisterBackend(QRotationSensor::type, genericrotationsensor::id);
            if (QSensorManager::isBackendRegistered(QAmbientLightSensor::type, genericalssensor::id))
                QSensorManager::unregisterBackend(QAmbientLightSensor::type, genericalssensor::id);
        }
    }

    QSensorBackend *createBackend(QSensor *sensor)
    {
        if (sensor->identifier() == genericorientationsensor::id)
            return new genericorientationsensor(sensor);

        if (sensor->identifier() == genericrotationsensor::id)
            return new genericrotationsensor(sensor);

        if (sensor->identifier() == genericalssensor::id)
            return new genericalssensor(sensor);

        return 0;
    }
};

Q_EXPORT_PLUGIN2(qtsensors_generic, genericSensorPlugin)

#include "main.moc"

