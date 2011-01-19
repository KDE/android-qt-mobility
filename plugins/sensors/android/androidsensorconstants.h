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

#ifndef ANDROIDSENSORCONSTANTS_H
#define ANDROIDSENSORCONSTANTS_H

namespace QtSensorJNI
{
    enum AndroidSensorType {
        TYPE_ACCELEROMETER=1,
        TYPE_ALL=-1,
        TYPE_GRAVITY=9,
        TYPE_GYROSCOPE=4,
        TYPE_LIGHT=5,
        TYPE_LINEAR_ACCELERATION=10,
        TYPE_MAGNETIC_FIELD =2,
        TYPE_ORIENTATION=3,
        TYPE_PRESSURE=6,
        TYPE_PROXIMITY=8,
        TYPE_ROTATION_VECTOR=11,
        TYPE_TEMPERATURE=7
                     };
}
#endif // ANDROIDSENSORCONSTANTS_H
