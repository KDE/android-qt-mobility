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

#include "qlandmarkproximityfilter.h"
#include "qlandmarkproximityfilter_p.h"
#include "qgeocoordinate.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkProximityFilter
    \brief The QLandmarkProximityFilter class is used to search for landmarks based on the radius
    around a given coordinate.

    \inmodule QtLocation
    
    \ingroup landmarks-filter

    In order to be a valid filter, the region covered by the proximity filter must not cover one of the poles.
    The landmarks are returned in ascending order of distance.  A proximity filter may
    be used inside an intersection filter to sort landmarks by distance in combination with other
    criteria such as categories.
*/

Q_IMPLEMENT_LANDMARKFILTER_PRIVATE(QLandmarkProximityFilter)

/*!
    Creates a filter that will select landmarks within a given \a radius around
    a \a center coordinate.
*/
QLandmarkProximityFilter::QLandmarkProximityFilter(const QGeoCoordinate &center, qreal radius)
        : QLandmarkFilter(new QLandmarkProximityFilterPrivate(center, radius))
{
}

/*!
    \fn QLandmarkProximityFilter::QLandmarkProximityFilter(const QLandmarkFilter &other)
    Constructs a copy of \a other if possible, otherwise constructs a new proximity filter.
*/

/*!
    Destroys the filter.
*/
QLandmarkProximityFilter::~QLandmarkProximityFilter()
{
    // pointer deleted in superclass destructor
}

/*!
    Returns the center coordinate of the filter.
*/
QGeoCoordinate QLandmarkProximityFilter::center() const
{
    Q_D(const QLandmarkProximityFilter);
    return d->center;
}

/*!
    Sets the \a center coordinate of the filter.
*/
void QLandmarkProximityFilter::setCenter(const QGeoCoordinate &center)
{
    Q_D(QLandmarkProximityFilter);
    d->center = center;
}

/*!
    Returns the radius of the filter.  the unit of the radius is meters.
*/
qreal QLandmarkProximityFilter::radius() const
{
    Q_D(const QLandmarkProximityFilter);
    return d->radius;
}

/*!
    Sets the \a radius of the filter.  The unit of the radius is meters.
*/
void QLandmarkProximityFilter::setRadius(qreal radius)
{
    Q_D(QLandmarkProximityFilter);
    d->radius = radius;
}


/*******************************************************************************
*******************************************************************************/

QLandmarkProximityFilterPrivate::QLandmarkProximityFilterPrivate(const QGeoCoordinate &center, qreal radius)
        : QLandmarkFilterPrivate(),
        center(center),
        radius(radius)
{
    type = QLandmarkFilter::ProximityFilter;
}

QLandmarkProximityFilterPrivate::QLandmarkProximityFilterPrivate(const QLandmarkProximityFilterPrivate &other)
        : QLandmarkFilterPrivate(other),
        center(other.center),
        radius(other.radius)
        {}

QLandmarkProximityFilterPrivate::~QLandmarkProximityFilterPrivate() {}

QTM_END_NAMESPACE
