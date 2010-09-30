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

#include "qgeomapobject.h"
#include "qgeomapobject_p.h"
#include "qgeomapdata.h"
#include "qgeoboundingbox.h"

#include <QtAlgorithms>

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoMapObject
    \brief The QGeoMapObject class is graphical item for display in
    QGraphicsGeoMap instancse, that is specified in terms of coordinates and
    distances.

    \inmodule QtLocation

    \ingroup maps-mapping-objects

    QGeoMapObject instances can also be grouped into heirarchies in order to
    simplify the process of creating compound objects and managing groups of
    objects.
*/

/*!
    \enum QGeoMapObject::Type

    Describes the type of a map object.

    \value NullType
        An empty QGeoMapObject.
    \value GroupType
        A QGeoMapObject used to organize other map objects into groups.
    \value RectangleType
        A QGeoMapObject used to display a rectangular region.
    \value CircleType
        A QGeoMapObject used to display a circular region.
    \value PolylineType
        A QGeoMapObject used to display a multi-segment line.
    \value PolygonType
        A QGeoMapObject used to display a polygonal region.
    \value PixmapType
        A QGeoMapObject used to display a pixmap on a map.
    \value TextType
        A QGeoMapObject used to display text on a map
    \value RouteType
        A QGeoMapObject used to display a route.
*/

/*!
    Constructs a new root map object associated with \a mapData.
*/
QGeoMapObject::QGeoMapObject(QGeoMapData *mapData)
        : d_ptr(new QGeoMapObjectPrivate())
{
    setMapData(mapData);
}

/*!
    Destroys this map object.
*/
QGeoMapObject::~QGeoMapObject()
{
    setMapData(0);
    delete d_ptr;
}

/*!
    Returns the type of this map object.
*/
QGeoMapObject::Type QGeoMapObject::type() const
{
    return QGeoMapObject::NullType;
}

/*!
    \property QGeoMapObject::zValue
    \brief This property holds the z-value of the map object.

    The z-value determines the order in which the objects are drawn on the
    screen.  Objects with the same value will be drawn in the order that
    they were added to the map or map object.

    This is the same behaviour as QGraphicsItem.
*/
void QGeoMapObject::setZValue(int zValue)
{
    if (d_ptr->zValue != zValue) {
        d_ptr->zValue = zValue;
        emit zValueChanged(d_ptr->zValue);
    }
}

int QGeoMapObject::zValue() const
{
    return d_ptr->zValue;
}

/*!
    \property QGeoMapObject::visible
    \brief This property holds whether the map object is visible.

    If this map object is not visible then none of the childObjects() will
    be displayed either.
*/
void QGeoMapObject::setVisible(bool visible)
{
    if (d_ptr->isVisible != visible) {
        d_ptr->isVisible = visible;
        emit visibleChanged(d_ptr->isVisible);
    }
}

bool QGeoMapObject::isVisible() const
{
    return d_ptr->isVisible;
}

/*!
    \property QGeoMapObject::selected
    \brief This property holds whether the map object is selected.
*/
void QGeoMapObject::setSelected(bool selected)
{
    if (d_ptr->isSelected != selected) {
        d_ptr->isSelected = selected;
        emit selectedChanged(d_ptr->isSelected);
    }
}

bool QGeoMapObject::isSelected() const
{
    return d_ptr->isSelected;
}

/*!
    Returns a bounding box which contains this map object.
*/
QGeoBoundingBox QGeoMapObject::boundingBox() const
{
    if (!d_ptr->info)
        return QGeoBoundingBox();

    return d_ptr->info->boundingBox();
}

/*!
    Returns whether \a coordinate is contained with the boundary of this
    map object.
*/
bool QGeoMapObject::contains(const QGeoCoordinate &coordinate) const
{
    if (!d_ptr->info)
        return false;

    return d_ptr->info->contains(coordinate);
}

/*!
    \internal
*/
bool QGeoMapObject::operator<(const QGeoMapObject &other) const
{
    Q_D(const QGeoMapObject);
    return d_ptr->zValue < other.d_ptr->zValue;
}

/*!
    \internal
*/
bool QGeoMapObject::operator>(const QGeoMapObject &other) const
{
    return d_ptr->zValue > other.d_ptr->zValue;
}

void QGeoMapObject::setMapData(QGeoMapData *mapData)
{
    if (d_ptr->mapData == mapData)
        return;

    if (d_ptr->info) {
        delete d_ptr->info;
        d_ptr->info = 0;
    }

    d_ptr->mapData = mapData;
    if (!d_ptr->mapData)
        return;

    d_ptr->info = mapData->createMapObjectInfo(this);

    if (!d_ptr->info)
        return;

    connect(d_ptr->mapData,
            SIGNAL(windowSizeChanged(QSizeF)),
            d_ptr->info,
            SLOT(windowSizeChanged(QSizeF)));
    connect(d_ptr->mapData,
            SIGNAL(zoomLevelChanged(qreal)),
            d_ptr->info,
            SLOT(zoomLevelChanged(qreal)));
    connect(d_ptr->mapData,
            SIGNAL(centerChanged(QGeoCoordinate)),
            d_ptr->info,
            SLOT(centerChanged(QGeoCoordinate)));

    connect(this,
            SIGNAL(zValueChanged(int)),
            d_ptr->info,
            SLOT(zValueChanged(int)));
    connect(this,
            SIGNAL(visibleChanged(bool)),
            d_ptr->info,
            SLOT(visibleChanged(bool)));
    connect(this,
            SIGNAL(selectedChanged(bool)),
            d_ptr->info,
            SLOT(selectedChanged(bool)));

    d_ptr->info->setup();

}

QGeoMapData* QGeoMapObject::mapData() const
{
    return d_ptr->mapData;
}

/*******************************************************************************
*******************************************************************************/

QGeoMapObjectPrivate::QGeoMapObjectPrivate()
        : zValue(0),
        isVisible(true),
        isSelected(false),
        mapData(0),
        info(0) {}

QGeoMapObjectPrivate::~QGeoMapObjectPrivate()
{
    if (info)
        delete info;
}

/*******************************************************************************
*******************************************************************************/

#include "moc_qgeomapobject.cpp"

QTM_END_NAMESPACE
