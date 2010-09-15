/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore>
#include <qaccelerometer.h>

QTM_USE_NAMESPACE

class AccelerometerFilter : public QAccelerometerFilter
{
public:
    qtimestamp stamp;
    bool filter(QAccelerometerReading *reading)
    {
        int diff = ( reading->timestamp() - stamp );
        stamp = reading->timestamp();
        QTextStream out(stdout);
        out << QString("Acceleration: %1 x").arg(reading->x(), 5, 'f', 1)
            << QString(" %1 y").arg(reading->y(), 5, 'f', 1)
            << QString(" %1 z m/s^2").arg(reading->z(), 5, 'f', 1)
            << QString(" (%1 ms since last, %2 Hz)").arg(diff / 1000, 4).arg( 1000000.0 / diff, 5, 'f', 1) << endl;
        return false; // don't store the reading in the sensor
    }
};

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QStringList args = app.arguments();
    int rate_place = args.indexOf("-r");
    int rate_val = 0;
    if (rate_place != -1)
        rate_val = args.at(rate_place + 1).toInt();
    QAccelerometer sensor;
    if (rate_val > 0) {
        sensor.setDataRate(rate_val);
    }
    AccelerometerFilter filter;
    sensor.addFilter(&filter);
    sensor.start();
    if (!sensor.isActive()) {
        qWarning("Accelerometersensor didn't start!");
        return 1;
    }

    return app.exec();
}
