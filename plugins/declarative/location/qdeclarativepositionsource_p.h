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
***************************************************************************/

#ifndef QDECLARATIVEPOSITIONSOURCE_H
#define QDECLARATIVEPOSITIONSOURCE_H

#include <QtCore>
#include <QDateTime>
#include <qgeopositioninfosource.h>
#include <qgeopositioninfo.h>
#include <QtDeclarative/qdeclarative.h>
#include "qdeclarativeposition_p.h"

class QFile;

QTM_BEGIN_NAMESPACE

class QDeclarativePositionSource : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativePosition* position READ position NOTIFY positionChanged)
    Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(QUrl nmeaSource READ nmeaSource WRITE setNmeaSource NOTIFY nmeaSourceChanged)
    Q_PROPERTY(int updateInterval READ updateInterval WRITE setUpdateInterval NOTIFY updateIntervalChanged)
    Q_PROPERTY(PositioningMethod positioningMethod READ positioningMethod NOTIFY positioningMethodChanged)
    Q_ENUMS(PositioningMethod)

public:

    enum PositioningMethod {
        NoPositioningMethod = 0,
        SatellitePositioningMethod = QGeoPositionInfoSource::SatellitePositioningMethods,
        NonSatellitePositioningMethod = QGeoPositionInfoSource::NonSatellitePositioningMethods,
        AllPositioningMethods = QGeoPositionInfoSource::AllPositioningMethods
    };
    QDeclarativePositionSource();
    ~QDeclarativePositionSource();
    void setNmeaSource(const QUrl& nmeaSource);
    void setUpdateInterval(int updateInterval);
    void setActive(bool active);

    QUrl nmeaSource() const;
    int updateInterval() const;
    bool isActive() const;
    QDeclarativePosition* position();
    PositioningMethod positioningMethod() const;

public Q_SLOTS:
    void update();
    void start();
    void stop();

Q_SIGNALS:
    void positionChanged();
    void activeChanged();
    void nmeaSourceChanged();
    void updateIntervalChanged();
    void positioningMethodChanged();

private Q_SLOTS:
    void positionUpdateReceived(const QGeoPositionInfo& update);

private:
    QGeoPositionInfoSource* m_positionSource;
    PositioningMethod m_positioningMethod;
    QDeclarativePosition m_position;
    QFile* m_nmeaFile;
    QUrl m_nmeaSource;
    bool m_active;
    bool m_singleUpdate;
    int m_updateInterval;
};

QTM_END_NAMESPACE
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativePositionSource));

#endif
