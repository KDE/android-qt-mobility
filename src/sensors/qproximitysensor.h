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

#ifndef QPROXIMITYSENSOR_H
#define QPROXIMITYSENSOR_H

#include "qsensor.h"

QTM_BEGIN_NAMESPACE

class QProximityReadingPrivate;

class Q_SENSORS_EXPORT QProximityReading : public QSensorReading
{
    Q_OBJECT
    Q_ENUMS(Proximity)
    Q_PROPERTY(Proximity proximity READ proximity)
    DECLARE_READING(QProximityReading)
public:
    enum Proximity {
        Undefined = 0,
        Close,
        NotClose
    };

    Proximity proximity() const;
    void setProximity(Proximity proximity);
};

// begin generated code

class Q_SENSORS_EXPORT QProximityFilter : public QSensorFilter
{
public:
    virtual bool filter(QProximityReading *reading) = 0;
private:
    bool filter(QSensorReading *reading) { return filter(static_cast<QProximityReading*>(reading)); }
};

class Q_SENSORS_EXPORT QProximitySensor : public QSensor
{
    Q_OBJECT
public:
    explicit QProximitySensor(QObject *parent = 0) : QSensor(parent)
    { setType(QProximitySensor::type); }
    virtual ~QProximitySensor() {}
    QProximityReading *reading() const { return static_cast<QProximityReading*>(QSensor::reading()); }
    static const char *type;
};
// end generated code

QTM_END_NAMESPACE

#endif

