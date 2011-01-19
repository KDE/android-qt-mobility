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

#ifndef ANDROIDCOMPASS_H
#define ANDROIDCOMPASS_H

#include <qcompass.h>
#include <androidsensorbackend.h>

class AndroidCompassSensorBackend : public AndroidSensorBackend
{
public:
    static char const * const id;
    AndroidCompassSensorBackend(QSensor *sensor,int uniqueId,QtSensorJNI::AndroidSensorType sensorType);
    virtual ~AndroidCompassSensorBackend();
    virtual void dataAvailable(float[],long,int);
private:
    QCompassReading m_reading;
};

#endif // ANDROIDCOMPASS_H
