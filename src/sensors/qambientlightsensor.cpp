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

#include "qambientlightsensor.h"
#include "qambientlightsensor_p.h"

QTM_BEGIN_NAMESPACE

IMPLEMENT_READING(QAmbientLightReading)

/*!
    \class QAmbientLightReading
    \ingroup sensors_reading

    \brief The QAmbientLightReading class represents one reading from the
           ambient light sensor.

    \section2 QAmbientLightReading Units
    The ambient light sensor returns the intensity of the ambient light
    using the pre-defined values found in the QAmbientLightReading::LightLevel
    enum.
*/

/*!
    \enum QAmbientLightReading::LightLevel

    This enum represents the ambient light level.

    \value Undefined  The light level is unknown.
    \value Dark       It is dark.
    \value Twilight   It is moderately dark.
    \value Light      It is light (eg. internal lights).
    \value Bright     It is bright (eg. shade).
    \value Sunny      It is very bright (eg. direct sunlight).
*/

/*!
    \property QAmbientLightReading::lightLevel
    \brief the ambient light level.

    The value represents the ambient light and comes from QAmbientLightReading::LightLevel.
    \sa {QAmbientLightReading Units}
*/

QAmbientLightReading::LightLevel QAmbientLightReading::lightLevel() const
{
    return static_cast<LightLevel>(d->lightLevel);
}

/*!
    Sets the ambient light level to \a lightLevel.
*/
void QAmbientLightReading::setLightLevel(QAmbientLightReading::LightLevel lightLevel)
{
    d->lightLevel = lightLevel;
}

// =====================================================================

/*!
    \class QAmbientLightFilter
    \ingroup sensors_filter

    \brief The QAmbientLightFilter class is a convenience wrapper around QSensorFilter.

    The only difference is that the filter() method features a pointer to QAmbientLightReading
    instead of QSensorReading.
*/

/*!
    \fn QAmbientLightFilter::filter(QAmbientLightReading *reading)

    Called when \a reading changes. Returns false to prevent the reading from propagating.

    \sa QSensorFilter::filter()
*/

const char *QAmbientLightSensor::type("QAmbientLightSensor");

/*!
    \class QAmbientLightSensor
    \ingroup sensors_type

    \brief The QAmbientLightSensor class is a convenience wrapper around QSensor.

    The only behavioural difference is that this class sets the type properly.

    This class also features a reading() function that returns a QAmbientLightReading instead of a QSensorReading.

    For details about how the sensor works, see \l QAmbientLightReading.

    \sa QAmbientLightReading
*/

/*!
    \fn QAmbientLightSensor::QAmbientLightSensor(QObject *parent)

    Construct the sensor as a child of \a parent.
*/

/*!
    \fn QAmbientLightSensor::~QAmbientLightSensor()

    Destroy the sensor. Stops the sensor if it has not already been stopped.
*/

/*!
    \fn QAmbientLightSensor::reading() const

    Returns the reading class for this sensor.

    \sa QSensor::reading()
*/

#include "moc_qambientlightsensor.cpp"
QTM_END_NAMESPACE

