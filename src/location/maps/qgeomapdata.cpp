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

#include "qgeomapdata.h"
#include "qgeomapdata_p.h"

#include "qgeoboundingbox.h"
#include "qgeocoordinate.h"
#include "qgraphicsgeomap.h"
#include "qgeomapobject.h"
#include "qgeomappingmanagerengine.h"
#include "qgeomapoverlay.h"

#include "qgeomapobject_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoMapData

    \brief The QGeoMapData class are used as a bridge between QGraphicsGeoMap and
    QGeoMappingManager.


    \inmodule QtLocation

    \ingroup maps-impl

    Instances of QGeoMapData are created with
    QGeoMappingManager::createMapData(), and are used internally by
    QGraphicsGeoMap to manage the state of the map and the associated
    QGeoMapObject instances.

    Plugin implementers will need to provide implementations of
    coordinateToScreenPosition(const QGeoCoordinate &coordinate) and
    QGeoCoordinate screenPositionToCoordinate(const QPointF &screenPosition).

     The other virtual functions can be overriden.  If the screen position to
     coordinate tranformations are expensive then overriding these functions
     may allow optimizations based on caching parts of the geometry information.

     Subclasses should override createMapObjecInfo() so that QGeoMapObjectInfo
     instances will be created for each QGeoMapObject type in order to
     provide the QGeoMapData subclass specific behaviours for the map objects.
 */

/*!
    Constructs a new map data object, which stores the map data required by
    \a geoMap and makes use of the functionality provided by \a engine.
*/
QGeoMapData::QGeoMapData(QGeoMappingManagerEngine *engine, QGraphicsGeoMap *geoMap)
        : d_ptr(new QGeoMapDataPrivate(this, engine, geoMap)) {}

/*!
  \internal
*/
QGeoMapData::QGeoMapData(QGeoMapDataPrivate *dd) : d_ptr(dd) {}

/*!
    Destroys this map data object.
*/
QGeoMapData::~QGeoMapData()
{
    Q_D(QGeoMapData);
    delete d;
}

/*!
    Returns the QGraphicsGeoMap instance that this map data object is associated with.
*/
QGraphicsGeoMap* QGeoMapData::geoMap() const
{
    return d_ptr->geoMap;
}

/*!
    Returns the mapping engine that this map data object is associated with.
*/
QGeoMappingManagerEngine* QGeoMapData::engine() const
{
    return d_ptr->engine;
}

/*!
    Returns the QGeoMapObject which acts as the parent to all QGeoMapObject
    instances which are added to the map by the user.
*/
QGeoMapObject* QGeoMapData::containerObject()
{
    return d_ptr->containerObject;
}

/*!
    Sets the size of the map viewport to \a size.

    The size will be adjusted by the associated QGraphicsGeoMap as it resizes.
*/
void QGeoMapData::setWindowSize(const QSizeF &size)
{
    if (d_ptr->windowSize == size)
        return;

    d_ptr->windowSize = size;

    if (!d_ptr->blockPropertyChangeSignals)
        emit windowSizeChanged(d_ptr->windowSize);
}

/*!
    Returns the size of the map viewport.

    The size will be adjusted by the associated QGraphicsGeoMap as it resizes.
*/
QSizeF QGeoMapData::windowSize() const
{
    return d_ptr->windowSize;
}

/*!
    Sets the zoom level of the map to \a zoomLevel.

    Larger values of the zoom level correspond to more detailed views of the
    map.

    If \a zoomLevel is less than minimumZoomLevel() then minimumZoomLevel()
    will be used, and if \a zoomLevel is  larger than
    maximumZoomLevel() then maximumZoomLevel() will be used.
*/
void QGeoMapData::setZoomLevel(qreal zoomLevel)
{
    zoomLevel = qMin(zoomLevel, d_ptr->engine->maximumZoomLevel());
    zoomLevel = qMax(zoomLevel, d_ptr->engine->minimumZoomLevel());

    if (d_ptr->zoomLevel == zoomLevel)
        return;

    d_ptr->zoomLevel = zoomLevel;

    if (!d_ptr->blockPropertyChangeSignals)
        emit zoomLevelChanged(d_ptr->zoomLevel);
}

/*!
    Returns the zoom level of the map.

    Larger values of the zoom level correspond to more detailed views of the
    map.
*/
qreal QGeoMapData::zoomLevel() const
{
    return d_ptr->zoomLevel;
}

/*!
    Pans the map view \a dx pixels in the x direction and \a dy pixels
    in the y direction.

    The x and y axes are specified in Graphics View Framework coordinates.
    By default this will mean that positive values of \a dx move the
    viewed area to the right and that positive values of \a dy move the
    viewed area down.

    Subclasses should call QGeoMapData::setCenter() when the pan has completed.
*/
void QGeoMapData::pan(int dx, int dy)
{
    QPointF pos = coordinateToScreenPosition(center());
    setCenter(screenPositionToCoordinate(QPointF(pos.x() + dx, pos.y() + dy)));
}

/*!
    Centers the map viewport on the coordinate \a center.
*/
void QGeoMapData::setCenter(const QGeoCoordinate &center)
{
    if (d_ptr->center == center)
        return;

    d_ptr->center = center;

    if (!d_ptr->blockPropertyChangeSignals)
        emit centerChanged(d_ptr->center);
}

/*!
    Returns the coordinate of the point in the center of the map viewport.
*/
QGeoCoordinate QGeoMapData::center() const
{
    return d_ptr->center;
}

/*!
    Changes the type of map data to display to \a mapType.
*/
void QGeoMapData::setMapType(QGraphicsGeoMap::MapType mapType)
{
    if (d_ptr->mapType == mapType)
        return;

    d_ptr->mapType = mapType;

    if (!d_ptr->blockPropertyChangeSignals)
        emit mapTypeChanged(mapType);
}

/*!
    Returns the type of map data which is being displayed.
*/
QGraphicsGeoMap::MapType QGeoMapData::mapType() const
{
    return d_ptr->mapType;
}

/*!
    Returns the map objects associated with this map.
*/
QList<QGeoMapObject*> QGeoMapData::mapObjects() const
{
    return d_ptr->containerObject->childObjects();
}

/*!
    Adds \a mapObject to the list of map objects managed by this map.

    The children objects are drawn in order of the QGeoMapObject::zValue()
    value.  Children objects having the same z value will be drawn
    in the order they were added.

    The map will take ownership of the \a mapObject.
*/
void QGeoMapData::addMapObject(QGeoMapObject *mapObject)
{
    d_ptr->containerObject->addChildObject(mapObject);
}

/*!
    Removes \a mapObject from the list of map objects managed by this map.
    The map will release ownership of the \a mapObject.
*/
void QGeoMapData::removeMapObject(QGeoMapObject *mapObject)
{
    d_ptr->containerObject->removeChildObject(mapObject);
}

/*!
    Clears the map objects associated with this map.

    The map objects will be deleted.
*/
void QGeoMapData::clearMapObjects()
{
    d_ptr->containerObject->clearChildObjects();
}

/*!
    \fn QGeoBoundingBox QGeoMapData::viewport() const
    Returns a bounding box corresponding to the physical area displayed 
    in the viewport of the map.

    The bounding box which is returned is defined by the upper left and 
    lower right corners of the visible area of the map.
*/

/*!
    \fn void QGeoMapData::fitInViewport(const QGeoBoundingBox &bounds, bool preserveViewportCenter)

    Attempts to fit the bounding box \a bounds into the viewport of the map.

    This method will change the zoom level to the maximum zoom level such 
    that all of \bounds is visible within the resulting viewport.

    If \a preserveViewportCenter is false the map will be centered on the 
    bounding box \a bounds before the zoom level is changed, otherwise the 
    center of the map will not be changed.
*/

/*!
    Returns the list of visible map objects managed by this map which
    contain the point \a screenPosition within their boundaries.
*/
QList<QGeoMapObject*> QGeoMapData::mapObjectsAtScreenPosition(const QPointF &screenPosition) const
{
    QList<QGeoMapObject*> results;

    QGeoCoordinate coord = screenPositionToCoordinate(screenPosition);
    int childObjectCount = d_ptr->containerObject->childObjects().count();
    for (int i = 0; i < childObjectCount; ++i) {
        QGeoMapObject *object = d_ptr->containerObject->childObjects().at(i);
        if (object->contains(coord) && object->isVisible())
            results.append(object);
    }

    return results;
}

/*!
    Returns the list of visible map objects managed by this map which are displayed at
    least partially within the on screen rectangle \a screenRect.
*/
QList<QGeoMapObject*> QGeoMapData::mapObjectsInScreenRect(const QRectF &screenRect) const
{
    QList<QGeoMapObject*> results;

    QGeoCoordinate topLeft = screenPositionToCoordinate(screenRect.topLeft());
    QGeoCoordinate bottomRight = screenPositionToCoordinate(screenRect.bottomRight());

    QGeoBoundingBox bounds(topLeft, bottomRight);

    int childObjectCount = d_ptr->containerObject->childObjects().count();
    for (int i = 0; i < childObjectCount; ++i) {
        QGeoMapObject *object = d_ptr->containerObject->childObjects().at(i);
        if (bounds.intersects(object->boundingBox()) && object->isVisible())
            results.append(object);
    }

    return results;
}

/*!
    Returns the list of visible map objects manager by this widget which 
    are displayed at least partially within the viewport of the map.
*/
QList<QGeoMapObject*> QGeoMapData::mapObjectsInViewport() const
{
    return mapObjectsInScreenRect(QRectF(0.0,
                                         0.0,
                                         d_ptr->windowSize.width(),
                                         d_ptr->windowSize.height()));
}

/*!
\fn QPointF QGeoMapData::coordinateToScreenPosition(const QGeoCoordinate &coordinate) const

    Returns the position on the screen at which \a coordinate is displayed.

    An invalid QPointF will be returned if \a coordinate is invalid or is not
    within the current viewport.
*/

/*!
\fn QGeoCoordinate QGeoMapData::screenPositionToCoordinate(const QPointF &screenPosition) const

    Returns the coordinate corresponding to the point in the viewport at \a
    screenPosition.

    An invalid QGeoCoordinate will be returned if \a screenPosition is invalid
    or is not within the current viewport.
*/

/*!
    Paints the map and everything associated with it on \a painter, using the
    options \a option.

    This will paint the map with paintMap(), then the map overlays with
    QGeoMapOverlay::paint(), then the map objects with paintObjects(), and
    finally paintProviderNotices().
*/
void QGeoMapData::paint(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    paintMap(painter, option);

    for (int i = 0; i < d_ptr->overlays.size(); ++i)
        d_ptr->overlays[i]->paint(painter, option);

    paintObjects(painter, option);

    paintProviderNotices(painter, option);
}

/*!
    Paints the map on \a painter, using the options \a option.

    The map overlays, map objects and the provider notices (such as copyright
    and powered by notices) are painted in separate methods, which are combined
    in the paint() method.

    The default implementation does not paint anything.
*/
void QGeoMapData::paintMap(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    Q_UNUSED(painter)
    Q_UNUSED(option)
}

/*!
    Paints the map objects on \a painter, using the options \a option.

    The default implementation does not paint anything.
*/
void QGeoMapData::paintObjects(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    Q_UNUSED(painter)
    Q_UNUSED(option)
}

/*!
    Paints the provider notices on \a painter, using the options \a option.

    The provider notices are things like the copyright and powered by notices.

    The provider may not want the client developers to be able to move the
    notices from their standard positions and so we have not provided API
    support for specifying the position of the notices at this time.

    If support for hinting at the positon of the notices is to be provided by
    plugin parameters, the suggested parameter keys are
    "mapping.notices.copyright.alignment" and
    "mapping.notices.poweredby.alignment", with type Qt::Alignment.

    The default implementation does not paint anything.
*/
void QGeoMapData::paintProviderNotices(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    Q_UNUSED(painter)
    Q_UNUSED(option)
}

/*!
    Returns the map overlays associated with this map.
*/
QList<QGeoMapOverlay*> QGeoMapData::mapOverlays() const
{
    return d_ptr->overlays;
}

/*!
    Adds \a overlay to the list of map overlays associated with this map.

    The overlays will be drawn in the order in which they were added.

    The map will take ownership of \a overlay.
*/
void QGeoMapData::addMapOverlay(QGeoMapOverlay *overlay)
{
    if (!overlay)
        return;

    d_ptr->overlays.append(overlay);
}

/*!
    Removes \a overlay from the list of map overlays associated with this map.

    The map will release ownership of \a overlay.
*/
void QGeoMapData::removeMapOverlay(QGeoMapOverlay *overlay)
{
    if (!overlay)
        return;

    d_ptr->overlays.removeAll(overlay);
}

/*!
    Clears the map overlays associated with this map.

    The map overlays will be deleted.
*/
void QGeoMapData::clearMapOverlays()
{
    qDeleteAll(d_ptr->overlays);
    d_ptr->overlays.clear();
}

/*!
    Sets up the associated between the map object \a mapObject and this map.

    This will setup an instance of a QGeoMapObjectInfo subclass for \a
    mapObject which will handle any of the map object behaviour which is
    specifical to this QGeoMapData subclass.

    This function uses createMapObjectInfo as a factory for the
    QGeoMapObjectInfo instance, and so subclasses should reimplement
    createMapObjectInfo to create the appropriate QGeoMapObjectInfo objects.
*/
void QGeoMapData::associateMapObject(QGeoMapObject *mapObject)
{
    QGeoMapObjectInfo* info = createMapObjectInfo(mapObject);
    d_ptr->setObjectInfo(mapObject, info);

    connect(this,
            SIGNAL(windowSizeChanged(QSizeF)),
            info,
            SLOT(windowSizeChanged(QSizeF)));
    connect(this,
            SIGNAL(zoomLevelChanged(qreal)),
            info,
            SLOT(zoomLevelChanged(qreal)));
    connect(this,
            SIGNAL(centerChanged(QGeoCoordinate)),
            info,
            SLOT(centerChanged(QGeoCoordinate)));

    connect(mapObject,
            SIGNAL(zValueChanged(int)),
            info,
            SLOT(zValueChanged(int)));
    connect(mapObject,
            SIGNAL(visibleChanged(bool)),
            info,
            SLOT(visibleChanged(bool)));
    connect(mapObject,
            SIGNAL(selectedChanged(bool)),
            info,
            SLOT(selectedChanged(bool)));

}

/*!
    Creates a QGeoMapObjectInfo instance which implements the behaviours of
    the map object \a object which are specific to this QGeoMapData.

    The default implementation returns 0.
*/
QGeoMapObjectInfo* QGeoMapData::createMapObjectInfo(QGeoMapObject *object)
{
    Q_UNUSED(object)
    return 0;
}

void QGeoMapData::setBlockPropertyChangeSignals(bool block)
{
    d_ptr->blockPropertyChangeSignals = block;
}

/*******************************************************************************
*******************************************************************************/

QGeoMapDataPrivate::QGeoMapDataPrivate(QGeoMapData *parent, QGeoMappingManagerEngine *engine, QGraphicsGeoMap *geoMap)
        : q_ptr(parent),
        engine(engine),
        geoMap(geoMap),
        zoomLevel(-1.0),
        blockPropertyChangeSignals(false)
{
    Q_Q(QGeoMapData);
    containerObject = new QGeoMapObject(q);
}

QGeoMapDataPrivate::~QGeoMapDataPrivate()
{
    delete containerObject;
    qDeleteAll(overlays);
}

void QGeoMapDataPrivate::setObjectInfo(QGeoMapObject *object, QGeoMapObjectInfo *info)
{
    object->d_ptr->info = info;
}

QGeoMapObjectInfo* QGeoMapDataPrivate::parentObjectInfo(QGeoMapObject *object) const
{
    QGeoMapObject *parent = object->d_ptr->parent;
    if (!parent)
        return 0;
    return parent->d_ptr->info;
}

#include "moc_qgeomapdata.cpp"

QTM_END_NAMESPACE
