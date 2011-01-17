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

#include "projwrapper_p.h"
#include <proj_api.h>
#include <QSharedData>
#include <QString>
#include <QPolygonF>
#include <QPointF>
#include "qgeocoordinate.h"

QTM_BEGIN_NAMESPACE

class ProjCoordinateSystemPrivate : public QSharedData
{
public:
    ProjCoordinateSystemPrivate(const QString &projection, bool latLon);
    ProjCoordinateSystemPrivate(const ProjCoordinateSystemPrivate &other);
    ~ProjCoordinateSystemPrivate();

    projPJ projection;
    bool latLon;
};

ProjCoordinateSystemPrivate::ProjCoordinateSystemPrivate(const QString &projStr, bool isLatLon)
{
    projection = pj_init_plus(projStr.toLatin1().constData());
    Q_ASSERT_X(projection, "pj_init_plus", "invalid projection string");
    latLon = isLatLon;
}

ProjCoordinateSystemPrivate::ProjCoordinateSystemPrivate(const ProjCoordinateSystemPrivate &other) :
    QSharedData(other),
    latLon(other.latLon)
{
    char *str = pj_get_def(other.projection, 0);
    projection = pj_init_plus(str);
    Q_ASSERT_X(projection, "pj_init_plus", "invalid projection string");
    delete str;
}

ProjCoordinateSystemPrivate::~ProjCoordinateSystemPrivate()
{
    pj_free(projection);
}

ProjCoordinateSystem::ProjCoordinateSystem(const QString &projection, bool latLon) :
    d(new ProjCoordinateSystemPrivate(projection, latLon))
{}

ProjCoordinateSystem::ProjCoordinateSystem(const ProjCoordinateSystem &other) :
    d(other.d)
{}

ProjCoordinateSystem::~ProjCoordinateSystem()
{}

bool ProjCoordinateSystem::isLatLon() const
{
    return d->latLon;
}

class ProjCoordinatePrivate
{
public:
    ProjCoordinateSystem currentSystem;
    double x;
    double y;
    double z;
};

ProjCoordinate::ProjCoordinate(double x, double y, double z, const ProjCoordinateSystem &system) :
    d(new ProjCoordinatePrivate)
{
    d->x = x;
    d->y = y;
    d->z = z;
    d->currentSystem = system;
}

ProjCoordinate::ProjCoordinate(const ProjCoordinate &other) :
    d(new ProjCoordinatePrivate)
{
    d->x = other.d->x;
    d->y = other.d->y;
    d->z = other.d->z;
    d->currentSystem = other.d->currentSystem;
}

ProjCoordinate::~ProjCoordinate()
{
    delete d;
}

double ProjCoordinate::x() const
{
    return d->x;
}

double ProjCoordinate::y() const
{
    return d->y;
}

double ProjCoordinate::z() const
{
    return d->z;
}

bool ProjCoordinate::convert(const ProjCoordinateSystem &system)
{
    int result;
    double x = d->x, y = d->y, z = d->z;

    if (d->currentSystem.isLatLon()) {
        x *= DEG_TO_RAD;
        y *= DEG_TO_RAD;
    }

    result = pj_transform(d->currentSystem.d->projection,
                          system.d->projection,
                          1, 1, &x, &y, &z);
    if (result) {
        return false;
    } else {
        if (system.isLatLon()) {
            x *= RAD_TO_DEG;
            y *= RAD_TO_DEG;
        }
        d->x = x;
        d->y = y;
        d->z = z;
        d->currentSystem = system;
        return true;
    }
}

QGeoCoordinate ProjCoordinate::toGeoCoordinate() const
{
    return QGeoCoordinate(d->y, d->x, d->z);
}

class ProjPolygonPrivate
{
public:
    ProjCoordinateSystem currentSystem;
};

ProjPolygon::ProjPolygon(const ProjCoordinateSystem &system) :
    QList<ProjCoordinate>(),
    d(new ProjPolygonPrivate)
{
    d->currentSystem = system;
}

ProjPolygon::ProjPolygon(const QPolygonF &poly, const ProjCoordinateSystem &system, double scale) :
    QList<ProjCoordinate>(),
    d(new ProjPolygonPrivate)
{
    d->currentSystem = system;
    foreach (QPointF point, poly) {
        double x = point.x();
        x /= scale;
        double y = point.y();
        y /= scale;
        append(ProjCoordinate(x, y, 0.0, system));
    }
}

ProjPolygon::~ProjPolygon()
{}

bool ProjPolygon::convert(const ProjCoordinateSystem &system)
{
    for (int i=0; i<size(); ++i) {
        ProjCoordinate coord = at(i);
        if (!coord.convert(system))
            return false;
        replace(i, coord);
    }

    d->currentSystem = system;
    return true;
}

QPolygonF ProjPolygon::toPolygonF(double scale) const
{
    QPolygonF poly;
    for (int i=0; i<size(); ++i) {
        const ProjCoordinate &coord = at(i);
        double x = coord.x() * scale;
        double y = coord.y() * scale;
        poly << QPointF(x, y);
    }
    return poly;
}

QTM_END_NAMESPACE
