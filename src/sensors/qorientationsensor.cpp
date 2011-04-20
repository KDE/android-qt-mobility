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

#include "qorientationsensor.h"
#include "qorientationsensor_p.h"

QTM_BEGIN_NAMESPACE

IMPLEMENT_READING(QOrientationReading)

/*!
    \class QOrientationReading
    \ingroup sensors_reading
    \inmodule QtSensors
    \since 1.0

    \brief The QOrientationReading class represents one reading from the
           orientation sensor.

    The orientation sensor reports the orientation of the device. As it operates below
    the UI level it does not report on or even know how the UI is rotated. Most importantly
    this means that this sensor cannot be used to detect if a device is in portrait or
    landscape mode.

    This sensor is useful to detect that a particular side of the device is pointing up.

    \section2 QOrientationReading Units
    The orientation sensor returns the orientation of the device using
    the pre-defined values found in the QOrientationReading::Orientation
    enum.
*/

/*!
    \enum QOrientationReading::Orientation

    This enum represents the orientation of the device.

    To explain the meaning of each value it is helpful to refer to the following diagram.

    \image sensors-sides.jpg

    The orientations are shown here in order: TopUp, TopDown, LeftUp, RightUp, FaceUp, FaceDown.

    \image sensors-orientation.jpg

    \value Undefined        The orientation is unknown.
    \value TopUp            The Top edge of the device is pointing up.
    \value TopDown          The Top edge of the device is pointing down.
    \value LeftUp           The Left edge of the device is pointing up.
    \value RightUp          The Right edge of the device is pointing up.
    \value FaceUp           The Face of the device is pointing up.
    \value FaceDown         The Face of the device is pointing down.

    It should be noted that the orientation sensor reports the orientation of the device
    and not the UI. The orientation of the device will not change just because the UI is
    rotated. This means this sensor cannot be used to detect if a device is in portrait
    or landscape mode.
*/

/*!
    \property QOrientationReading::orientation
    \brief the orientation of the device.

    The unit is an enumeration describing the orientation of the device.

    \sa {QOrientationReading Units}
*/

QOrientationReading::Orientation QOrientationReading::orientation() const
{
    return static_cast<QOrientationReading::Orientation>(d->orientation);
}

/*!
    Sets the \a orientation for the reading.
*/
void QOrientationReading::setOrientation(QOrientationReading::Orientation orientation)
{
    switch (orientation) {
    case TopUp:
    case TopDown:
    case LeftUp:
    case RightUp:
    case FaceUp:
    case FaceDown:
        d->orientation = orientation;
        break;
    default:
        d->orientation = Undefined;
        break;
    }
}

// =====================================================================

/*!
    \class QOrientationFilter
    \ingroup sensors_filter
    \inmodule QtSensors

    \brief The QOrientationFilter class is a convenience wrapper around QSensorFilter.

    The only difference is that the filter() method features a pointer to QOrientationReading
    instead of QSensorReading.
*/

/*!
    \fn QOrientationFilter::filter(QOrientationReading *reading)

    Called when \a reading changes. Returns false to prevent the reading from propagating.

    \sa QSensorFilter::filter()
*/

char const * const QOrientationSensor::type("QOrientationSensor");

/*!
    \class QOrientationSensor
    \ingroup sensors_type
    \inmodule QtSensors

    \brief The QOrientationSensor class is a convenience wrapper around QSensor.

    The only behavioural difference is that this class sets the type properly.

    This class also features a reading() function that returns a QOrientationReading instead of a QSensorReading.

    For details about how the sensor works, see \l QOrientationReading.

    \sa QOrientationReading
*/

/*!
    \fn QOrientationSensor::QOrientationSensor(QObject *parent)

    Construct the sensor as a child of \a parent.
*/

/*!
    \fn QOrientationSensor::~QOrientationSensor()

    Destroy the sensor. Stops the sensor if it has not already been stopped.
*/

/*!
    \fn QOrientationSensor::reading() const

    Returns the reading class for this sensor.

    \sa QSensor::reading()
*/

#include "moc_qorientationsensor.cpp"
QTM_END_NAMESPACE

