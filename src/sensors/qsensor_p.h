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
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
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
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSENSOR_P_H
#define QSENSOR_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qsensor.h"

QTM_BEGIN_NAMESPACE

typedef QList<QSensorFilter*> QFilterList;

class QSensorPrivate
{
public:
    QSensorPrivate()
        : identifier()
        , type()
        , outputRange(-1)
        , dataRate(0)
        , backend(0)
        , active(false)
        , busy(false)
        , device_reading(0)
        , filter_reading(0)
        , cache_reading(0)
        , error(0)
    {
    }

    // meta-data
    QByteArray identifier;
    QByteArray type;

    QString description;

    qoutputrangelist outputRanges;
    int outputRange;

    // policy
    qrangelist availableDataRates;
    int dataRate;

    QSensorBackend *backend;
    QFilterList filters;
    bool active;
    bool busy;
    QSensorReading *device_reading;
    QSensorReading *filter_reading;
    QSensorReading *cache_reading;

    int error;
};

class QSensorReadingPrivate
{
public:
    QSensorReadingPrivate()
        : timestamp(0)
    {
    }

    // sensor data cache
    qtimestamp timestamp;
};

QTM_END_NAMESPACE

#endif

