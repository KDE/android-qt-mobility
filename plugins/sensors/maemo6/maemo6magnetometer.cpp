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

#include "maemo6magnetometer.h"

char const * const maemo6magnetometer::id("maemo6.magnetometer");
bool maemo6magnetometer::m_initDone = false;
const float maemo6magnetometer::NANO = 0.000000001;


maemo6magnetometer::maemo6magnetometer(QSensor *sensor)
    : maemo6sensorbase(sensor)
{
    const QString sensorName = "magnetometersensor";
    initSensor<MagnetometerSensorChannelInterface>(sensorName, m_initDone);

    setRanges(NANO);

    if (m_sensorInterface){
        if (!(QObject::connect(m_sensorInterface, SIGNAL(dataAvailable(const MagneticField&)),
                               this, SLOT(slotDataAvailable(const MagneticField&)))))
            qWarning() << "Unable to connect "<< sensorName;
    }
    else
        qWarning() << "Unable to initialize "<< sensorName;

    setReading<QMagnetometerReading>(&m_reading);

}

void maemo6magnetometer::start(){
    maemo6sensorbase::start();
    QVariant v = sensor()->property("returnGeoValues");
    m_isGeoMagnetometer =  v.isValid() && v.toBool()? true: false;
}

void maemo6magnetometer::slotDataAvailable(const MagneticField& data)
{

    //nanoTeslas given, divide with 10^9 to get Teslas
    m_reading.setX( NANO * m_isGeoMagnetometer?data.x():data.rx());
    m_reading.setY( NANO * m_isGeoMagnetometer?data.y():data.ry());
    m_reading.setZ( NANO * m_isGeoMagnetometer?data.z():data.rz());
    m_reading.setCalibrationLevel( m_isGeoMagnetometer?((float) data.level()) / 3.0 :1);


    m_reading.setTimestamp(data.timestamp());
    newReadingAvailable();
}
