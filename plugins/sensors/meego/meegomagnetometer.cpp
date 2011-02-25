/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
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
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "meegomagnetometer.h"

char const * const meegomagnetometer::id("meego.magnetometer");
bool meegomagnetometer::m_initDone = false;
const float meegomagnetometer::NANO = 0.000000001;


meegomagnetometer::meegomagnetometer(QSensor *sensor)
    : meegosensorbase(sensor)
{
    initSensor<MagnetometerSensorChannelInterface>(m_initDone);
    setDescription(QLatin1String("magnetic flux density in teslas (T)"));
    setRanges(NANO);
    setReading<QMagnetometerReading>(&m_reading);
}

void meegomagnetometer::start(){
    QVariant v = sensor()->property("returnGeoValues");
    if (!(v.isValid())){
        sensor()->setProperty("returnGeoValues", false); //Set to false (the default) to return raw magnetic flux density
        m_isGeoMagnetometer=false;
    }
    else m_isGeoMagnetometer =  v.toBool();

    meegosensorbase::start();
}

void meegomagnetometer::slotDataAvailable(const MagneticField& data)
{
    //nanoTeslas given, divide with 10^9 to get Teslas
    m_reading.setX( NANO * m_isGeoMagnetometer?data.x():data.rx());
    m_reading.setY( NANO * m_isGeoMagnetometer?data.y():data.ry());
    m_reading.setZ( NANO * m_isGeoMagnetometer?data.z():data.rz());
    m_reading.setCalibrationLevel( m_isGeoMagnetometer?((float) data.level()) / 3.0 :1);
    m_reading.setTimestamp(data.timestamp());
    newReadingAvailable();
}


void meegomagnetometer::slotFrameAvailable(const QVector<MagneticField>&   frame)
{
    for (int i=0, l=frame.size(); i<l; i++){
        slotDataAvailable(frame.at(i));
    }
}

bool meegomagnetometer::doConnect(){
    if (m_bufferSize==1?
                QObject::connect(m_sensorInterface, SIGNAL(dataAvailable(const MagneticField&)), this, SLOT(slotDataAvailable(const MagneticField&))):
                QObject::connect(m_sensorInterface, SIGNAL(frameAvailable(const QVector<MagneticField>& )),this, SLOT(slotFrameAvailable(const QVector<MagneticField>& ))))
        return true;
    return false;
}

const QString meegomagnetometer::sensorName(){
    return "magnetometersensor";
}

const qreal meegomagnetometer::correctionFactor(){return meegomagnetometer::NANO;}
