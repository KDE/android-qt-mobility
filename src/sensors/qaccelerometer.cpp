/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qaccelerometer.h"
#include "qaccelerometer_p.h"

QTM_BEGIN_NAMESPACE

IMPLEMENT_READING(QAccelerometerReading)

/*!
    \class QAccelerometerReading
    \ingroup sensors_reading

    \brief The QAccelerometerReading class reports on linear acceleration
           along the X, Y and Z axes.

    \section2 QAccelerometerReading Units
    The scale of the values is meters per second squared.
    The axes are arranged as follows.

    \image sensors-coordinates2.jpg

    A monoblock device sitting at rest, face up on a desk will experience
    a force of approximately 9.8 on the Z axis (ie. towards the roof).
    This is the proper acceleration the device experiences relative to
    freefall.
*/

/*!
    \property QAccelerometerReading::x
    \brief the acceleration on the X axis.

    The scale of the values is meters per second squared.
    \sa {QAccelerometerReading Units}
*/

qreal QAccelerometerReading::x() const
{
    return d->x;
}

/*!
    Sets the acceleration on the X axis to \a x.
*/
void QAccelerometerReading::setX(qreal x)
{
    d->x = x;
}

/*!
    \property QAccelerometerReading::y
    \brief the acceleration on the Y axis.

    The scale of the values is meters per second squared.
    \sa {QAccelerometerReading Units}
*/

qreal QAccelerometerReading::y() const
{
    return d->y;
}

/*!
    Sets the acceleration on the Y axis to \a y.
*/
void QAccelerometerReading::setY(qreal y)
{
    d->y = y;
}

/*!
    \property QAccelerometerReading::z
    \brief the acceleration on the Z axis.

    The scale of the values is meters per second squared.
    \sa {QAccelerometerReading Units}
*/

qreal QAccelerometerReading::z() const
{
    return d->z;
}

/*!
    Sets the acceleration on the Z axis to \a z.
*/
void QAccelerometerReading::setZ(qreal z)
{
    d->z = z;
}

// =====================================================================

/*!
    \class QAccelerometerFilter
    \ingroup sensors_filter

    \brief The QAccelerometerFilter class is a convenience wrapper around QSensorFilter.

    The only difference is that the filter() method features a pointer to QAccelerometerReading
    instead of QSensorReading.
*/

/*!
    \fn QAccelerometerFilter::filter(QAccelerometerReading *reading)

    Called when \a reading changes. Returns false to prevent the reading from propagating.

    \sa QSensorFilter::filter()
*/

const char *QAccelerometer::type("QAccelerometer");

/*!
    \class QAccelerometer
    \ingroup sensors_type

    \brief The QAccelerometer class is a convenience wrapper around QSensor.

    The only behavioural difference is that this class sets the type properly.

    This class also features a reading() function that returns a QAccelerometerReading instead of a QSensorReading.

    For details about how the sensor works, see \l QAccelerometerReading.

    \sa QAccelerometerReading
*/

/*!
    \fn QAccelerometer::QAccelerometer(QObject *parent)

    Construct the sensor as a child of \a parent.
*/

/*!
    \fn QAccelerometer::~QAccelerometer()

    Destroy the sensor. Stops the sensor if it has not already been stopped.
*/

/*!
    \fn QAccelerometer::reading() const

    Returns the reading class for this sensor.

    \sa QSensor::reading()
*/

#include "moc_qaccelerometer.cpp"
QTM_END_NAMESPACE

