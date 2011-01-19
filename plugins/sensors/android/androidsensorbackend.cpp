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

#include <androidsensorbackend.h>

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
    case 3:
        getDataConstant=3;
        break;
    case 2:
        getDataConstant=2;
        break;
    case 1:
        getDataConstant=1;
        break;
    case 0:
        getDataConstant=0;
        break;
    default:
        getDataConstant=3;
    }
    QtSensorJNI::start (getDataConstant,m_uniqueID,m_sensorType,this);
}

void AndroidSensorBackend::stop()
{
    QtSensorJNI::stop(m_uniqueID);
}
