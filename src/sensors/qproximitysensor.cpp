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

#include "qproximitysensor.h"
#include "qproximitysensor_p.h"

QTM_BEGIN_NAMESPACE

IMPLEMENT_READING(QProximityReading)

/*!
    \class QProximityReading
    \ingroup sensors_reading

    \brief The QProximityReading class represents one reading from the
           proximity sensor.

    \target QProximityReading_Units
    The proximity sensor can only indicate if an object is close or not.

    The distance at which an object is considered close is device-specific. This
    distance may be available in the QSensor::outputRanges property.
*/

/*!
    \property QProximityReading::close
    \brief a value indicating if something is close.

    Set to true if something is close.
    Set to false is nothing is close.

    \sa QProximityReading_Units
*/

bool QProximityReading::close() const
{
    return d->close;
}

/*!
    Sets the close value to \a close.
*/
void QProximityReading::setClose(bool close)
{
    d->close = close;
}

// =====================================================================

/*!
    \class QProximityFilter
    \ingroup sensors_filter

    \brief The QProximityFilter class is a convenience wrapper around QSensorFilter.

    The only difference is that the filter() method features a pointer to QProximityReading
    instead of QSensorReading.
*/

/*!
    \fn QProximityFilter::filter(QProximityReading *reading)

    Called when \a reading changes. Returns false to prevent the reading from propagating.

    \sa QSensorFilter::filter()
*/

char const * const QProximitySensor::type("QProximitySensor");

/*!
    \class QProximitySensor
    \ingroup sensors_type

    \brief The QProximitySensor class is a convenience wrapper around QSensor.

    The only behavioural difference is that this class sets the type properly.

    This class also features a reading() function that returns a QProximityReading instead of a QSensorReading.

    For details about how the sensor works, see \l QProximityReading.

    \sa QProximityReading
*/

/*!
    \fn QProximitySensor::QProximitySensor(QObject *parent)

    Construct the sensor as a child of \a parent.
*/

/*!
    \fn QProximitySensor::~QProximitySensor()

    Destroy the sensor. Stops the sensor if it has not already been stopped.
*/

/*!
    \fn QProximitySensor::reading() const

    Returns the reading class for this sensor.

    \sa QSensor::reading()
*/

#include "moc_qproximitysensor.cpp"
QTM_END_NAMESPACE

