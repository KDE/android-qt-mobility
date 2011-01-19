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

#ifndef ANDROIDACCELEROMETER_H
#define ANDROIDACCELEROMETER_H

#include <qaccelerometer.h>
#include <androidsensorbackend.h>

class AndroidAccelerometerSensorBackend : public AndroidSensorBackend
{
public:
    static char const * const id;
    AndroidAccelerometerSensorBackend(QSensor *sensor,int uniqueId,QtSensorJNI::AndroidSensorType sensorType);
    virtual ~AndroidAccelerometerSensorBackend();
    virtual void dataAvailable(float[],long,int);
private:
    QAccelerometerReading m_reading;
};

#endif
