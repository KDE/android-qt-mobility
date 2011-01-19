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

#ifndef ANDROIDSENSORBACKEND_H
#define ANDROIDSENSORBACKEND_H

#include <qsensorbackend.h>
#include <sensorjni.h>

QTM_USE_NAMESPACE

        class AndroidSensorBackend : public QSensorBackend,public QtSensorJNI::SensorAndroidListener
{
public:
    AndroidSensorBackend(QSensor *sensor,int uniqueId,QtSensorJNI::AndroidSensorType sensorType);
    virtual ~AndroidSensorBackend();
    virtual void dataAvailable(float[],long,int)=0;
private:
    QSensor *m_sensor;
    int m_uniqueID;
    QtSensorJNI::AndroidSensorType m_sensorType;
protected:
    virtual void start();
    virtual void stop();

};


#endif // ANDROIDSENSORBACKEND_H
