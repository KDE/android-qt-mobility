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

#ifndef SENSORJNI_H
#define SENSORJNI_H

#include <jni.h>
#include <stdio.h>
#include <androidsensorconstants.h>
#include <QMap>
#include <QPair>

namespace QtSensorJNI
{
    class SensorAndroidListener
    {
    public :
            SensorAndroidListener ()
    {
    }

    virtual ~SensorAndroidListener ()
    {
    }

    virtual void dataAvailable (float [],long,int) =0;
};

    void  start (int,int,int,SensorAndroidListener *);
    void  stop (int);
    void registerSensor ();
}

#endif // SENSORJNI_H
