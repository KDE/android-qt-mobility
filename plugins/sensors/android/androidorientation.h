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

#ifndef ANDROIDORIENTATION_H
#define ANDROIDORIENTATION_H

#include <qorientationsensor.h>
#include <androidsensorbackend.h>

class AndroidOrientationSensorBackend : public AndroidSensorBackend
{
public:
    static char const * const id;
    AndroidOrientationSensorBackend(QSensor *sensor,int uniqueId,QtSensorJNI::AndroidSensorType sensorType);
    virtual ~AndroidOrientationSensorBackend();
    virtual void dataAvailable(float[],long,int);
private:
    QOrientationReading m_reading;
};
#endif // ANDROIDORIENTATION_H
