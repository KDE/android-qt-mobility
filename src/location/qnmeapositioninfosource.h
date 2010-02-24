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
#ifndef QNMEAPOSITIONINFOSOURCE_H
#define QNMEAPOSITIONINFOSOURCE_H

#include "qmobilityglobal.h"
#include "qgeopositioninfosource.h"

QT_BEGIN_NAMESPACE
class QIODevice;
QT_END_NAMESPACE

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QNmeaPositionInfoSourcePrivate;
class Q_LOCATION_EXPORT QNmeaPositionInfoSource : public QGeoPositionInfoSource
{
    Q_OBJECT
public:
    enum UpdateMode {
        RealTimeMode = 1,
        SimulationMode
    };

    explicit QNmeaPositionInfoSource(UpdateMode updateMode, QObject *parent = 0);
    ~QNmeaPositionInfoSource();

    UpdateMode updateMode() const;

    void setDevice(QIODevice *source);
    QIODevice *device() const;

    void setUpdateInterval(int msec);

    QGeoPositionInfo lastKnownPosition(bool fromSatellitePositioningMethodsOnly = false) const;
    PositioningMethods supportedPositioningMethods() const;
    int minimumUpdateInterval() const;

public Q_SLOTS:
    void startUpdates();
    void stopUpdates();
    void requestUpdate(int timeout = 0);

protected:
    virtual bool parsePosInfoFromNmeaData(const char *data,
                                          int size,
                                          QGeoPositionInfo *posInfo,
                                          bool *hasFix);

private:
    Q_DISABLE_COPY(QNmeaPositionInfoSource)
    friend class QNmeaPositionInfoSourcePrivate;
    QNmeaPositionInfoSourcePrivate *d;
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif
