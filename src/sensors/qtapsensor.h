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

#ifndef QTAPSENSOR_H
#define QTAPSENSOR_H

#include "qsensor.h"

QTM_BEGIN_NAMESPACE

class QTapReadingPrivate;

class Q_SENSORS_EXPORT QTapReading : public QSensorReading
{
    Q_OBJECT
    Q_ENUMS(TapDirection)
    Q_PROPERTY(TapDirection tapDirection READ tapDirection)
    Q_PROPERTY(bool doubleTap READ isDoubleTap)
    DECLARE_READING(QTapReading)
public:
    enum TapDirection {
        Undefined = 0,
        X     = 0x0001,
        Y     = 0x0002,
        Z     = 0x0004,
        X_Pos = 0x0011,
        Y_Pos = 0x0022,
        Z_Pos = 0x0044,
        X_Neg = 0x0101,
        Y_Neg = 0x0202,
        Z_Neg = 0x0404
    };

    TapDirection tapDirection() const;
    void setTapDirection(TapDirection tapDirection);

    bool isDoubleTap() const;
    void setDoubleTap(bool doubleTap);
};

// begin generated code

class Q_SENSORS_EXPORT QTapFilter : public QSensorFilter
{
public:
    virtual bool filter(QTapReading *reading) = 0;
private:
    bool filter(QSensorReading *reading) { return filter(static_cast<QTapReading*>(reading)); }
};

class Q_SENSORS_EXPORT QTapSensor : public QSensor
{
    Q_OBJECT
public:
    explicit QTapSensor(QObject *parent = 0) : QSensor(parent)
    { setType(QTapSensor::type); }
    virtual ~QTapSensor() {}
    QTapReading *reading() const { return static_cast<QTapReading*>(QSensor::reading()); }
    static const char *type;
};
// end generated code

QTM_END_NAMESPACE

#endif

