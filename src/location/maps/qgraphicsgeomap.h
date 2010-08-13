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

#ifndef QGRAPHICSGEOMAP_H
#define QGRAPHICSGEOMAP_H

#include "qmobilityglobal.h"

#include <QGraphicsWidget>

#include <QDeclarativeListProperty>

QTM_BEGIN_NAMESPACE

class QGeoCoordinate;
class QGeoMappingManager;
class QGeoBoundingBox;
class QGeoMapObject;
class QGraphicsGeoMapPrivate;

class Q_LOCATION_EXPORT QGraphicsGeoMap : public QGraphicsWidget
{
    Q_OBJECT
    Q_ENUMS(MapType)

    Q_PROPERTY(qreal minimumZoomLevel READ minimumZoomLevel)
    Q_PROPERTY(qreal maximumZoomLevel READ maximumZoomLevel)
    Q_PROPERTY(qreal zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(MapType mapType READ mapType WRITE setMapType NOTIFY mapTypeChanged)
    Q_PROPERTY(QGeoCoordinate center READ center WRITE setCenter NOTIFY centerChanged)
    Q_PROPERTY(QDeclarativeListProperty<QGeoMapObject> mapObjects READ mapObjects)

    Q_CLASSINFO("DefaultProperty", "mapObjects")

public:
    enum MapType {
        NoMap,
        StreetMap,
        SatelliteMapDay,
        SatelliteMapNight,
        TerrainMap
    };

    QGraphicsGeoMap(QGraphicsItem *parent = 0);
    QGraphicsGeoMap(QGeoMappingManager *manager, QGraphicsItem *parent = 0);
    virtual ~QGraphicsGeoMap();

    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *parent);

    qreal minimumZoomLevel() const;
    qreal maximumZoomLevel() const;

    void setZoomLevel(qreal zoomLevel);
    qreal zoomLevel() const;

    void setCenter(const QGeoCoordinate &center);
    QGeoCoordinate center() const;

    QList<MapType> supportedMapTypes() const;
    void setMapType(MapType mapType);
    MapType mapType() const;

    void startPanning();
    void stopPanning();

    QDeclarativeListProperty<QGeoMapObject> mapObjects();
    int mapObjectCount() const;
    void addMapObject(QGeoMapObject *mapObject);
    void removeMapObject(QGeoMapObject *mapObject);
    QGeoMapObject* mapObject(int index) const;
    void clearMapObjects();

    QList<QGeoMapObject*> mapObjectsAtScreenPosition(const QPointF &screenPosition);
    QList<QGeoMapObject*> mapObjectsInScreenRect(const QRectF &screenRect);

    QPointF coordinateToScreenPosition(const QGeoCoordinate &coordinate) const;
    QGeoCoordinate screenPositionToCoordinate(QPointF screenPosition) const;

public slots:
    void pan(int dx, int dy);

protected:
    void resizeEvent(QGraphicsSceneResizeEvent *event);

signals:
    void zoomLevelChanged(qreal zoomLevel);
    void centerChanged(const QGeoCoordinate &coordinate);
    void mapTypeChanged(QGraphicsGeoMap::MapType mapType);

private:
    QGraphicsGeoMapPrivate *d_ptr;
    Q_DISABLE_COPY(QGraphicsGeoMap);
};

QTM_END_NAMESPACE

#endif
