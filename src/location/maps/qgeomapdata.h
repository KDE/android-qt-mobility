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

#ifndef QGEOMAPDATA_H
#define QGEOMAPDATA_H

#include "qgraphicsgeomap.h"

#include <QObject>
#include <QSize>
#include <QRectF>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

QTM_BEGIN_NAMESPACE

class QGeoCoordinate;
class QGeoBoundingBox;
class QGeoMappingManagerEngine;
class QGeoMapObject;
class QGeoMapObjectInfo;
class QGeoMapDataPrivate;
class QGeoMapOverlay;

class Q_LOCATION_EXPORT QGeoMapData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QSizeF windowSize READ windowSize WRITE setWindowSize NOTIFY windowSizeChanged)
    Q_PROPERTY(qreal zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(QGraphicsGeoMap::MapType mapType READ mapType WRITE setMapType NOTIFY mapTypeChanged)
    Q_PROPERTY(QGraphicsGeoMap::ConnectivityMode connectivityMode READ connectivityMode WRITE setConnectivityMode NOTIFY connectivityModeChanged)
    Q_PROPERTY(QGeoCoordinate center READ center WRITE setCenter NOTIFY centerChanged)

public:
    QGeoMapData(QGeoMappingManagerEngine *engine);
    virtual ~QGeoMapData();

    virtual void init();

    virtual void setWindowSize(const QSizeF &size);
    virtual QSizeF windowSize() const;

    virtual void setZoomLevel(qreal zoomLevel);
    virtual qreal zoomLevel() const;

    virtual void pan(int dx, int dy);

    virtual void setCenter(const QGeoCoordinate &center);
    virtual QGeoCoordinate center() const;

    virtual void setMapType(QGraphicsGeoMap::MapType mapType);
    virtual QGraphicsGeoMap::MapType mapType() const;

    virtual void setConnectivityMode(QGraphicsGeoMap::ConnectivityMode connectivityMode);
    virtual QGraphicsGeoMap::ConnectivityMode connectivityMode() const;

    QList<QGeoMapObject*> mapObjects() const;
    void addMapObject(QGeoMapObject *mapObject);
    void removeMapObject(QGeoMapObject *mapObject);
    void clearMapObjects();

    virtual QGeoBoundingBox viewport() const = 0;
    virtual void fitInViewport(const QGeoBoundingBox &bounds, bool preserveViewportCenter = false) = 0;

    virtual QList<QGeoMapObject*> mapObjectsAtScreenPosition(const QPointF &screenPosition) const;
    virtual QList<QGeoMapObject*> mapObjectsInScreenRect(const QRectF &screenRect) const;
    virtual QList<QGeoMapObject*> mapObjectsInViewport() const;

    QList<QGeoMapOverlay*> mapOverlays() const;
    void addMapOverlay(QGeoMapOverlay *overlay);
    void removeMapOverlay(QGeoMapOverlay *overlay);
    void clearMapOverlays();

    virtual QPointF coordinateToScreenPosition(const QGeoCoordinate &coordinate) const = 0;
    virtual QGeoCoordinate screenPositionToCoordinate(const QPointF &screenPosition) const = 0;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option);

protected:
    QGeoMapData(QGeoMapDataPrivate *dd);

    QGeoMappingManagerEngine* engine() const;
    QGeoMapObject* containerObject();

    void setBlockPropertyChangeSignals(bool block);

    virtual void paintMap(QPainter *painter, const QStyleOptionGraphicsItem *option);
    virtual void paintObjects(QPainter *painter, const QStyleOptionGraphicsItem *option);
    virtual void paintProviderNotices(QPainter *painter, const QStyleOptionGraphicsItem *option);

    virtual QGeoMapObjectInfo* createMapObjectInfo(QGeoMapObject *object);

    QGeoMapDataPrivate* d_ptr;

Q_SIGNALS:
    void windowSizeChanged(const QSizeF &windowSize);
    void zoomLevelChanged(qreal zoomLevel);
    void centerChanged(const QGeoCoordinate &coordinate);
    void mapTypeChanged(QGraphicsGeoMap::MapType mapType);
    void connectivityModeChanged(QGraphicsGeoMap::ConnectivityMode connectivityMode);
    void updateMapDisplay(const QRectF &target = QRectF());

private:
    Q_DECLARE_PRIVATE(QGeoMapData)
    Q_DISABLE_COPY(QGeoMapData)

    friend class QGeoMapObject;
    friend class QGeoMapObjectInfo;
};

QTM_END_NAMESPACE

#endif
