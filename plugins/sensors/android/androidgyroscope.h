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

#ifndef ANDROIDGYROSCOPE_H
#define ANDROIDGYROSCOPE_H

#include <qgyroscope.h>
#include <androidsensorbackend.h>

class AndroidGyroscopeSensorBackend : public AndroidSensorBackend
{
public:
    static char const * const id;
    AndroidGyroscopeSensorBackend(QSensor *sensor,int uniqueId,QtSensorJNI::AndroidSensorType sensorType);
    virtual ~AndroidGyroscopeSensorBackend();
    virtual void dataAvailable(float[],long,int);
private:
    QGyroscopeReading m_reading;
    static const double piBy180;//0.017453292519943295769236907684886
};

#endif // ANDROIDGYROSCOPE_H
