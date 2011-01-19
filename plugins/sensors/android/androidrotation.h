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

#ifndef ANDROIDROTATION_H
#define ANDROIDROTATION_H

#include <qrotationsensor.h>
#include <androidsensorbackend.h>

class AndroidRotationSensorBackend : public AndroidSensorBackend
{
public:
    static char const * const id;
    AndroidRotationSensorBackend(QSensor *sensor,int uniqueId,QtSensorJNI::AndroidSensorType sensorType);
    virtual ~AndroidRotationSensorBackend();
    virtual void dataAvailable(float[],long,int);
private:
    QRotationReading m_reading;
};
#endif // ANDROIDROTATION_H
