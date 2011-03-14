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

#ifndef QGEOTILEDMAPDATA_P_H
#define QGEOTILEDMAPDATA_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qgeomapdata_p.h"
#include "qgeomapobject.h"
#include "qgeomapobject_p.h"

#include "qgeotiledmapobjectinfo_p.h"

#include <QRectF>
#include <QHash>
#include <QCache>
#include <QPair>
#include <QList>

class QGraphicsScene;

QTM_BEGIN_NAMESPACE

class QGeoTiledMapData;
class QGeoTiledMapRequest;
class QGeoTiledMapReply;
class QGeoTiledMapObjectInfo;

class QGeoTiledMapDataPrivate : public QGeoMapDataPrivate
{
public:
    QGeoTiledMapDataPrivate(QGeoTiledMapData *parent, QGeoMappingManagerEngine *engine);
    ~QGeoTiledMapDataPrivate();

    void updateMapImage();
    void clearRequests();

    void paintMap(QPainter *painter, const QStyleOptionGraphicsItem *option);
    void paintObjects(QPainter *painter, const QStyleOptionGraphicsItem *option);

    void cleanupCaches();

    bool intersects(QGeoMapObject *mapObject, const QRectF &rect);

    QRect screenRectForZoomFactor(int zoomFactor);
    void updateScreenRect();

    bool containedInScreen(const QPoint &point) const;
    bool intersectsScreen(const QRect &rect) const;
    QList<QPair<QRect, QRect> > intersectedScreen(const QRect &rect, bool translateToScreen = true) const;

    void removeObjectInfo(QGeoTiledMapObjectInfo* object);

    void addObjectInfo(QGeoTiledMapObjectInfo* object);

    int zoomFactor;

    QPoint worldReferenceViewportCenter;
    QSize worldReferenceSize;
    QRect worldReferenceViewportRect;

    QRect worldReferenceViewportRectLeft;
    QRect worldReferenceViewportRectRight;

    QSet<QRect> requestRects;
    QSet<QRect> replyRects;

    QList<QGeoTiledMapRequest> requests;
    QSet<QGeoTiledMapReply*> replies;

    QCache<QGeoTiledMapRequest, QImage> cache;
    QCache<QGeoTiledMapRequest, QPixmap> zoomCache;


    Q_DECLARE_PUBLIC(QGeoTiledMapData)
private:
    QGraphicsScene *scene;

    QHash<QGraphicsItem*, QGeoMapObject*> itemMap;

    Q_DISABLE_COPY(QGeoTiledMapDataPrivate)
};

class QGeoTileIterator
{
public:
    QGeoTileIterator(const QGeoTiledMapDataPrivate *mapData);
    QGeoTileIterator(QGraphicsGeoMap::ConnectivityMode connectivityMode,
                     QGraphicsGeoMap::MapType mapType,
                     const QRect &screenRect,
                     const QSize &tileSize,
                     int zoomLevel);

    bool hasNext();
    QGeoTiledMapRequest next();

private:
    bool atEnd;
    int row;
    int col;
    int width;
    QRect screenRect;
    QSize tileSize;
    QGraphicsGeoMap::MapType mapType;
    QGraphicsGeoMap::ConnectivityMode connectivityMode;
    int zoomLevel;
    QPoint currTopLeft;
    QRect tileRect;
};

QTM_END_NAMESPACE

#endif
