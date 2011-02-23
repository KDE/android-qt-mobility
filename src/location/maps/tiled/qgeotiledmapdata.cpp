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

#include "qgeotiledmapdata.h"
#include "qgeotiledmapdata_p.h"

#include "qgeotiledmappingmanagerengine.h"
#include "qgeotiledmappingmanagerengine_p.h"
#include "qgeotiledmaprequest.h"
#include "qgeocoordinate.h"
#include "qgeoboundingbox.h"
#include "qgeomapoverlay.h"
#include "projwrapper_p.h"

#include <QTimer>
#include <QImage>
#include <QGraphicsScene>

#include <QDebug>

#define DEFAULT_ZOOMLEVEL 8
#define PI 3.14159265
#define HIT_DETECTION_COLOR qRgba(0, 0, 255, 127) //semi-transparent blue

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <math.h>

uint qHash(const QRectF& key)
{
    uint result = qHash(qRound(key.x()));
    result += qHash(qRound(key.y()));
    result += qHash(qRound(key.width()));
    result += qHash(qRound(key.height()));
    return result;
}

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoTiledMapData
    \brief The QGeoTiledMapData class is a subclass of QGeoMapData provided
    to make working with tile based mapping services more convenient.

    \inmodule QtLocation

    \ingroup maps-impl-tiled

    This class assumes that at a zoom level of z the world is represented as a
    2^z by 2^z grid of tiles, and that the Mercator projection is used to map
    back and forth between coordinate and positions on the map.

    Different projections can be provided by reimplementing
    coordinateToWorldReferencePosition() and worldReferencePositionToCoordinate().

    Many of the internal calculations deal with positions as though they are
    pixel positions on the map at the maximum zoom level.  Several functions are
    provided which expose information about the map and the viewport onto the
    map in these terms for use with custom QGeoMapObjectInfo subclasses.

    These functions include worldReferenceViewportCenter(), worldReferenceSize() and
    worldReferenceViewportRect().
*/

/*!
    Constructs a new tiled map data object, which makes use of the functionality provided by \a engine.
*/
QGeoTiledMapData::QGeoTiledMapData(QGeoMappingManagerEngine *engine)
    : QGeoMapData(new QGeoTiledMapDataPrivate(this, engine))
{
    Q_D(QGeoTiledMapData);

    QGeoTiledMappingManagerEngine *tileEngine = static_cast<QGeoTiledMappingManagerEngine *>(d->engine);

    setBlockPropertyChangeSignals(true);

    setZoomLevel(8.0);

    d->worldReferenceSize = (1 << qRound(tileEngine->maximumZoomLevel())) * tileEngine->tileSize();

    d->scene = new QGraphicsScene(QRectF(QPointF(0.0, 0.0), d->worldReferenceSize));
    d->scene->setItemIndexMethod(QGraphicsScene::NoIndex);

    // TODO get this from the engine, which should give different values depending on if this is running on a device or not
#if defined(Q_OS_SYMBIAN) || defined(Q_OS_WINCE_WM) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    d->cache.setMaxCost(5 * 1024 * 1024);
    d->zoomCache.setMaxCost(5 * 1024 * 1024);
#else
    d->cache.setMaxCost(10 * 1024 * 1024);
    d->zoomCache.setMaxCost(10 * 1024 * 1024);
#endif
}

/*!
    Destroys this tiled map data object.
*/
QGeoTiledMapData::~QGeoTiledMapData()
{
}


QPointF QGeoTiledMapDataPrivate::coordinateToScreenPosition(double lon, double lat) const
{
    qreal offsetX = ((windowSize.width() * zoomFactor) - worldReferenceViewportRect.width()) / 2.0;
    if (offsetX < 0.0)
        offsetX = 0.0;
    offsetX /= zoomFactor;
    qreal offsetY = ((windowSize.height() * zoomFactor) - worldReferenceViewportRect.height()) / 2.0;
    if (offsetY < 0.0)
        offsetY = 0.0;
    offsetY /= zoomFactor;

    QPoint pos(coordinateToWorldReferencePosition(lon, lat));

    const int x = pos.x() - worldReferenceViewportRect.left();
    //if (x < 0)
    //    x += worldReferenceSize.width();

    const int y = pos.y() - worldReferenceViewportRect.top();

    QPointF posF(offsetX + qreal(x) / zoomFactor, offsetY + qreal(y) / zoomFactor);

    return posF;
}

/*!
    \reimp
*/
QPointF QGeoTiledMapData::coordinateToScreenPosition(const QGeoCoordinate &coordwgs) const
{
    Q_D(const QGeoTiledMapData);

    //ProjCoordinate c(coordwgs.longitude(), coordwgs.latitude(), 0.0, d->wgs84);
    //c.convert(d->spherical);

    return d->coordinateToScreenPosition(coordwgs.longitude(), coordwgs.latitude());
}

/*!
    \reimp
*/
QGeoCoordinate QGeoTiledMapData::screenPositionToCoordinate(const QPointF &screenPosition) const
{
    Q_D(const QGeoTiledMapData);

    QPoint worldRef = d->screenPositionToWorldReferencePosition(screenPosition);

    if (worldRef.isNull())
        return QGeoCoordinate();

    const QGeoCoordinate insph = worldReferencePositionToCoordinate(worldRef);

    //ProjCoordinate c(insph.longitude(), insph.latitude(), 0.0, d->spherical);
    //c.convert(d->wgs84);
    return insph;
}

// this belongs to QGeoTiledMapDataPrivate in order to avoid
// breaking B.C.
QPoint QGeoTiledMapDataPrivate::screenPositionToWorldReferencePosition(const QPointF &screenPosition) const
{
    qreal offsetX = ((windowSize.width() * zoomFactor) - worldReferenceViewportRect.width()) / 2.0;
    if (offsetX < 0.0)
        offsetX = 0.0;
    offsetX /= zoomFactor;
    qreal offsetY = ((windowSize.height() * zoomFactor) - worldReferenceViewportRect.height()) / 2.0;
    if (offsetY < 0.0)
        offsetY = 0.0;
    offsetY /= zoomFactor;

    QPointF pos(screenPosition.x() - offsetX, screenPosition.y() - offsetY);

    const int worldX = int(worldReferenceViewportRect.left() + pos.x() * zoomFactor + 0.5) % worldReferenceSize.width();
    const int worldY = int(worldReferenceViewportRect.top() + pos.y() * zoomFactor + 0.5) % worldReferenceSize.height();

    return QPoint(worldX, worldY);
}

QPoint QGeoTiledMapDataPrivate::coordinateToWorldReferencePosition(double lng, double lat) const
{
    lng = lng / 360.0 + 0.5;

    lat = 0.5 - (log(tan((PI / 4.0) + (PI / 2.0) * lat / 180.0)) / PI) / 2.0;
    lat = qMax(0.0, lat);
    lat = qMin(1.0, lat);

    const double x = lng * worldReferenceSize.width();
    const double y = lat * worldReferenceSize.height();

    const QPoint r(int(x > 0 ? x + 0.5 : x - 0.5),
                   int(y > 0 ? y + 0.5 : y - 0.5));
    return r;
}

/*!
    Converts the coordinate \a coordinate to a pixel position on the entire
    map at the maximum zoom level.

    The default implementation is based on the Mercator projection.
*/
QPoint QGeoTiledMapData::coordinateToWorldReferencePosition(const QGeoCoordinate &coordinate) const
{
    Q_D(const QGeoTiledMapData);
    return d->coordinateToWorldReferencePosition(coordinate.longitude(),
                                                 coordinate.latitude());
}

qreal rmod(const qreal a, const qreal b)
{
    quint64 div = static_cast<quint64>(a / b);
    return a - static_cast<qreal>(div) * b;
}

/*!
    Converts the pixel position \a pixel on the map to a coordinate.

    The pixel position is relative the entire map at the maximum zoom level.

    The default implementation is based on the Mercator projection.
*/
QGeoCoordinate QGeoTiledMapData::worldReferencePositionToCoordinate(const QPoint &pixel) const
{
    Q_D(const QGeoTiledMapData);

    qreal fx = qreal(pixel.x()) / d->worldReferenceSize.width();
    qreal fy = qreal(pixel.y()) / d->worldReferenceSize.height();

    if (fy < 0.0f)
        fy = 0.0f;
    else if (fy > 1.0f)
        fy = 1.0f;

    qreal lat;

    if (fy == 0.0f)
        lat = 90.0f;
    else if (fy == 1.0f)
        lat = -90.0f;
    else
        lat = (180.0f / PI) * (2.0f * atan(exp(PI * (1.0f - 2.0f * fy))) - (PI / 2.0f));

    qreal lng;
    if (fx >= 0) {
        lng = rmod(fx, 1.0f);
    } else {
        lng = rmod(1.0f - rmod(-1.0f * fx, 1.0f), 1.0f);
    }

    lng = lng * 360.0f - 180.0f;

    return QGeoCoordinate(lat, lng, 0.0);
}

/*!
    \reimp
*/
void QGeoTiledMapData::setCenter(const QGeoCoordinate &center)
{
    Q_D(QGeoTiledMapData);

    bool changed = (d->center != center);

    if (!changed)
        return;

    d->worldReferenceViewportCenter = coordinateToWorldReferencePosition(center);
    d->updateScreenRect();
    emit updateMapDisplay();

    emit centerChanged(center);

    d->updateMapImage();

    QGeoMapData::setCenter(center);
}

/*!
    \reimp
*/
void QGeoTiledMapData::setMapType(QGraphicsGeoMap::MapType mapType)
{
    Q_D(QGeoTiledMapData);

    bool changed = (d->mapType != mapType);

    if (!changed)
        return;

    QGeoMapData::setMapType(mapType);

    d->clearRequests();
    d->cache.clear();
    d->zoomCache.clear();
    emit updateMapDisplay();

    emit mapTypeChanged(d->mapType);

    d->updateMapImage();
}

/*!
    \reimp
*/
QGeoCoordinate QGeoTiledMapData::center() const
{
    Q_D(const QGeoTiledMapData);
    return worldReferencePositionToCoordinate(d->worldReferenceViewportCenter);
}

/*!
    \reimp
*/
void QGeoTiledMapData::setZoomLevel(qreal zoomLevelf)
{
    Q_D(QGeoTiledMapData);

    QPixmap oldImage(windowSize().toSize());
    if (!oldImage.isNull()) {
        // grab the old image
        QPainter painter1(&oldImage);
        d->paintMap(&painter1, 0);
        painter1.end();
    }

    qreal oldZoomLevel = d->zoomLevel;
    int zoomLevel = qRound(zoomLevelf);

    QGeoMapData::setZoomLevel(zoomLevel);

    // QGeoMapData::setZoomLevel clips the zoom level to be
    // in between the minimum and maximum zoom levels
    zoomLevel = QGeoMapData::zoomLevel();

    int zoomDiff = qRound(zoomLevel - oldZoomLevel);

    if (zoomDiff == 0)
        return;

    d->zoomFactor = 1 << qRound(d->engine->maximumZoomLevel() - d->zoomLevel);

    QGeoTiledMappingManagerEngine *tileEngine = static_cast<QGeoTiledMappingManagerEngine *>(d->engine);
    QSize tileSize = tileEngine->tileSize();

    d->updateScreenRect();

    if (oldImage.isNull()) {
        d->updateMapImage();
        emit zoomLevelChanged(d->zoomLevel);
        return;
    }

    //scale old image
    QRectF target = oldImage.rect();
    qreal width = target.width() / (1 << qAbs(zoomDiff));
    qreal height = target.height() / (1 << qAbs(zoomDiff));
    qreal x = target.x() + ((target.width() - width) / 2.0);
    qreal y = target.y() + ((target.height() - height) / 2.0);
    QRectF source = QRectF(x, y, width, height);

    QPixmap newImage(oldImage.size());
    newImage.fill(Qt::lightGray);
    QPainter painter2(&newImage);
#if !(defined(Q_OS_SYMBIAN) || defined(Q_OS_WINCE_WM) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6))
    painter2.setRenderHint(QPainter::SmoothPixmapTransform, true);
#endif
    if (zoomDiff < 0) {
        painter2.drawPixmap(source, oldImage, target);
    } else {
        painter2.drawPixmap(target, oldImage, source);
    }
    painter2.end();

    d->zoomCache.clear();

    QGeoTileIterator it(d);

    qreal offsetX = ((d->windowSize.width() * d->zoomFactor) - d->worldReferenceViewportRect.width()) / 2.0;
    if (offsetX < 0.0)
        offsetX = 0.0;
    offsetX /= d->zoomFactor;
    qreal offsetY = ((d->windowSize.height() * d->zoomFactor) - d->worldReferenceViewportRect.height()) / 2.0;
    if (offsetY < 0.0)
        offsetY = 0.0;
    offsetY /= d->zoomFactor;

    while (it.hasNext()) {
        QGeoTiledMapRequest req = it.next();
        QRect tileRect = req.tileRect();

        if (d->cache.contains(req))
            continue;

        if (!d->intersectsScreen(tileRect))
            continue;

        QList<QPair<QRect, QRect> > overlaps = d->intersectedScreen(tileRect);
        for (int i = 0; i < overlaps.size(); ++i) {
            QRect s = overlaps.at(i).first;
            QRect t = overlaps.at(i).second;

            QRectF source = QRectF(offsetX + int(t.left()) / d->zoomFactor,
                                   offsetY + int(t.top()) / d->zoomFactor,
                                   int(t.width()) / d->zoomFactor,
                                   int(t.height()) / d->zoomFactor);

            QPixmap *tile = new QPixmap(tileSize);
            tile->fill(Qt::lightGray); // TODO: this looks useless

            QRectF target = QRectF(int(s.left()) / d->zoomFactor,
                                   int(s.top()) / d->zoomFactor,
                                   int(s.width()) / d->zoomFactor,
                                   int(s.height()) / d->zoomFactor);

            QPainter painter3(tile);
            painter3.drawPixmap(target, newImage, source);
            painter3.end();

            d->zoomCache.insert(req, tile, (tile->depth() * tile->width() * tile->height()) / 8);
        }
    }

    emit updateMapDisplay();

    d->clearRequests();
    d->updateMapImage();

    emit zoomLevelChanged(d->zoomLevel);

    // call again
    QGeoMapData::setZoomLevel(zoomLevel);
}

/*!
    \reimp
*/
void QGeoTiledMapData::setWindowSize(const QSizeF &size)
{
    Q_D(QGeoTiledMapData);

    bool changed = (d->windowSize != size);

    if (!changed)
        return;

    QGeoMapData::setWindowSize(size);

    d->updateScreenRect();

    emit windowSizeChanged(d->windowSize);

    d->updateMapImage();
}

/*!
    \reimp
*/
void QGeoTiledMapData::pan(int dx, int dy)
{
    Q_D(QGeoTiledMapData);

    int x = d->worldReferenceViewportCenter.x();
    int y = d->worldReferenceViewportCenter.y();

    x = (x + dx * d->zoomFactor) % d->worldReferenceSize.width();
    if (x < 0)
        x += d->worldReferenceSize.width();

    y = (y + dy * d->zoomFactor);
    int height = int(d->worldReferenceViewportRect.height() / 2.0);
    if (y < height)
        y = height;
    if (y > d->worldReferenceSize.height() - height)
        y = d->worldReferenceSize.height() - height;

    d->worldReferenceViewportCenter.setX(x);
    d->worldReferenceViewportCenter.setY(y);

    QGeoCoordinate centerCoord = center();

    QGeoMapData::setCenter(centerCoord);

    d->updateScreenRect();

    emit centerChanged(centerCoord);

    d->updateMapImage();
}

/*!
    \reimp
*/
QGeoBoundingBox QGeoTiledMapData::viewport() const
{
    Q_D(const QGeoTiledMapData);
    if (d->worldReferenceViewportRectRight.isValid()) {
        return QGeoBoundingBox(worldReferencePositionToCoordinate(d->worldReferenceViewportRectLeft.topLeft()),
                               worldReferencePositionToCoordinate(d->worldReferenceViewportRectRight.bottomRight()));
    } else {
        return QGeoBoundingBox(worldReferencePositionToCoordinate(d->worldReferenceViewportRect.topLeft()),
                               worldReferencePositionToCoordinate(d->worldReferenceViewportRect.bottomRight()));
    }
}

/*!
    \reimp
*/
void QGeoTiledMapData::fitInViewport(const QGeoBoundingBox &bounds, bool preserveViewportCenter)
{
    Q_D(QGeoTiledMapData);

    if (!preserveViewportCenter)
        setCenter(bounds.center());

    int minZoomLevel = engine()->minimumZoomLevel();
    int maxZoomLevel = engine()->maximumZoomLevel();

    int zoomFactor = 1 << maxZoomLevel;

    for (int i = minZoomLevel; i <= maxZoomLevel; ++i) {
        QRect rect = d->screenRectForZoomFactor(zoomFactor);
        QGeoBoundingBox viewport = QGeoBoundingBox(worldReferencePositionToCoordinate(rect.topLeft()),
                                   worldReferencePositionToCoordinate(rect.bottomRight()));

        qWarning() << i << zoomFactor
                   << viewport.topLeft()
                   << viewport.bottomRight();

        if (!viewport.contains(bounds)) {
            setZoomLevel(qMax(minZoomLevel, i - 1));
            return;
        }

        zoomFactor /= 2;
    }

    setZoomLevel(maxZoomLevel);
}

/*!
    \reimp
*/
void QGeoTiledMapData::paintMap(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    Q_D(QGeoTiledMapData);
    d->paintMap(painter, option);
}

/*!
    \reimp
*/
void QGeoTiledMapData::paintObjects(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    QGeoMapData::paintObjects(painter, option);
}

/*!
    \reimp
*/
QGeoMapObjectInfo *QGeoTiledMapData::createMapObjectInfo(QGeoMapObject *mapObject)
{
    Q_UNUSED(mapObject);
    qWarning("QGeoTiledMapData::createMapObjectInfo is obsolete, returning NULL");
    return 0;
}

void QGeoTiledMapData::processRequests()
{
    Q_D(QGeoTiledMapData);

    QMutableSetIterator<QGeoTiledMapReply*> replyIter(d->replies);
    // Abort off-screen replies
    while (replyIter.hasNext()) {
        QGeoTiledMapReply *reply = replyIter.next();
        if (!d->intersectsScreen(reply->request().tileRect())
                || (zoomLevel() != reply->request().zoomLevel())
                || (mapType() != reply->request().mapType())
                || (connectivityMode() != reply->request().connectivityMode())) {
            reply->abort();
            d->replyRects.remove(reply->request().tileRect());
            replyIter.remove();
            d->zoomCache.remove(reply->request());
        }
    }

    QGeoTiledMappingManagerEngine *tiledEngine
    = static_cast<QGeoTiledMappingManagerEngine*>(engine());

    QMutableListIterator<QGeoTiledMapRequest> requestIter(d->requests);
    while (requestIter.hasNext()) {
        QGeoTiledMapRequest req = requestIter.next();

        d->requestRects.remove(req.tileRect());
        requestIter.remove();

        // Do not use the requests which have pending replies or are off screen
        if (d->replyRects.contains(req.tileRect()) || !d->intersectsScreen(req.tileRect())) {
            continue;
        }

        QGeoTiledMapReply *reply = tiledEngine->getTileImage(req);

        if (!reply) {
            continue;
        }

        if (reply->error() != QGeoTiledMapReply::NoError) {
            tileError(reply->error(), reply->errorString());
            reply->deleteLater();
            d->zoomCache.remove(reply->request());
            continue;
        }

        connect(reply,
                SIGNAL(finished()),
                this,
                SLOT(tileFinished()));

        connect(reply,
                SIGNAL(error(QGeoTiledMapReply::Error, QString)),
                this,
                SLOT(tileError(QGeoTiledMapReply::Error, QString)));

        d->replies.insert(reply);
        d->replyRects.insert(reply->request().tileRect());

        if (reply->isFinished())
            replyFinished(reply);

        if (reply->isCached())
            break;
    }
}

void QGeoTiledMapData::tileFinished()
{
    Q_D(QGeoTiledMapData);

    QGeoTiledMapReply *reply = qobject_cast<QGeoTiledMapReply*>(sender());

    if (!reply) {
        if (d->requests.size() > 0)
            QTimer::singleShot(0, this, SLOT(processRequests()));
        return;
    } else {
        replyFinished(reply);
    }
}

void QGeoTiledMapData::replyFinished(QGeoTiledMapReply *reply)
{
    Q_D(QGeoTiledMapData);

    d->replyRects.remove(reply->request().tileRect());
    d->replies.remove(reply);
    d->zoomCache.remove(reply->request());

    if (reply->error() != QGeoTiledMapReply::NoError) {
        if (d->requests.size() > 0)
            QTimer::singleShot(0, this, SLOT(processRequests()));
        QTimer::singleShot(0, reply, SLOT(deleteLater()));
        return;
    }

    if ((zoomLevel() != reply->request().zoomLevel())
            || (mapType() != reply->request().mapType())
            || (connectivityMode() != reply->request().connectivityMode())) {
        if (d->requests.size() > 0)
            QTimer::singleShot(0, this, SLOT(processRequests()));
        QTimer::singleShot(0, reply, SLOT(deleteLater()));
        return;
    }

    //QPixmap *tile = new QPixmap();
    QImage *tile = new QImage();

    if (!tile->loadFromData(reply->mapImageData(), reply->mapImageFormat().toAscii())) {
        delete tile;
        if (d->requests.size() > 0)
            QTimer::singleShot(0, this, SLOT(processRequests()));
        QTimer::singleShot(0, reply, SLOT(deleteLater()));
        return;
        //setError(QGeoTiledMapReply::ParseError, "The response from the service was not in a recognisable format.");
    }

    if (tile->isNull() || tile->size().isEmpty()) {
        delete tile;
        if (d->requests.size() > 0)
            QTimer::singleShot(0, this, SLOT(processRequests()));
        QTimer::singleShot(0, reply, SLOT(deleteLater()));
        return;
        //setError(QGeoTiledMapReply::ParseError, "The map image is empty.");
    }

    d->cache.insert(reply->request(), tile, (tile->depth() * tile->width() * tile->height()) / 8);

    d->cleanupCaches();

    QRect tileRect = reply->request().tileRect();

    qreal offsetX = ((d->windowSize.width() * d->zoomFactor) - d->worldReferenceViewportRect.width()) / 2.0;
    if (offsetX < 0.0)
        offsetX = 0.0;
    offsetX /= d->zoomFactor;
    qreal offsetY = ((d->windowSize.height() * d->zoomFactor) - d->worldReferenceViewportRect.height()) / 2.0;
    if (offsetY < 0.0)
        offsetY = 0.0;
    offsetY /= d->zoomFactor;

    QList<QPair<QRect, QRect> > overlaps = d->intersectedScreen(tileRect);
    for (int i = 0; i < overlaps.size(); ++i) {
        QRect t = overlaps.at(i).second;
        QRectF target = QRectF(offsetX + int(t.left()) / d->zoomFactor,
                               offsetY + int(t.top()) / d->zoomFactor,
                               int(t.width()) / d->zoomFactor,
                               int(t.height()) / d->zoomFactor);

        emit updateMapDisplay(target);
    }

    if (d->requests.size() > 0)
        QTimer::singleShot(0, this, SLOT(processRequests()));

    QTimer::singleShot(0, reply, SLOT(deleteLater()));
}

void QGeoTiledMapData::tileError(QGeoTiledMapReply::Error error, QString errorString)
{
    Q_UNUSED(error)
    qWarning() << errorString;
}

/*!
    \reimp
*/
QList<QGeoMapObject*> QGeoTiledMapData::mapObjectsAtScreenPosition(const QPointF &screenPosition) const
{
    return QGeoMapData::mapObjectsAtScreenPosition(screenPosition);
}

/*!
    \reimp
*/
QList<QGeoMapObject*> QGeoTiledMapData::mapObjectsInScreenRect(const QRectF &screenRect) const
{
    return QGeoMapData::mapObjectsInScreenRect(screenRect);
}

/*!
    Returns the center of the viewport, in pixels on the entire
    map as a pixmap at the maximum zoom level.
*/
QPoint QGeoTiledMapData::worldReferenceViewportCenter() const
{
    Q_D(const QGeoTiledMapData);
    return d->worldReferenceViewportCenter;
}

/*!
    Returns the size, in pixels, of the entire map as a pixmap at the maximum
    zoom level.
*/
QSize QGeoTiledMapData::worldReferenceSize() const
{
    Q_D(const QGeoTiledMapData);
    return d->worldReferenceSize;
}

/*!
    Returns the visible screen rectangle, in pixels on the entire map
    as a pixmap at the maximum zoom level.
*/
QRect QGeoTiledMapData::worldReferenceViewportRect() const
{
    Q_D(const QGeoTiledMapData);
    return d->worldReferenceViewportRect;
}

/*!
    Returns the ratio between a single pixel on the screen and a pixel on
    the entire map as a pixmap at the maximum zoom level.
*/
int QGeoTiledMapData::zoomFactor() const
{
    Q_D(const QGeoTiledMapData);
    return d->zoomFactor;
}

/*!
    Forces the map display to update in the region specified by \a target.

    If \a target is empty the entire map display will be updated.
*/
void QGeoTiledMapData::triggerUpdateMapDisplay(const QRectF &target)
{
    emit updateMapDisplay(target);
}

/*******************************************************************************
*******************************************************************************/

QGeoTiledMapDataPrivate::QGeoTiledMapDataPrivate(QGeoTiledMapData *parent, QGeoMappingManagerEngine *engine)
    : QGeoMapDataPrivate(parent, engine),
    zoomFactor(0),
    spherical("+proj=latlon +ellps=sphere"),
    wgs84("+proj=latlon +ellps=WGS84"),
    scene(0)
{
}

QGeoTiledMapDataPrivate::~QGeoTiledMapDataPrivate()
{
    foreach(QGeoTiledMapReply * reply, replies) {
        reply->abort();
        reply->deleteLater();
    }

    //before the model(scene) is destroyed , let the info object bound to this scene
    //be destoryed.

    QList<QGraphicsItem*> keys = itemMap.keys();

    foreach(QGraphicsItem * object, keys) {

        QGeoMapObject* o = itemMap.value(object);

        //check if we have still this info object ,
        //since it could be already removed
        //by previous loop in case of group object

        if (o != 0) o->setMapData(0);
    }

    itemMap.clear();


    if (scene)
        delete scene;
}

void QGeoTiledMapDataPrivate::updateMapImage()
{
    Q_Q(QGeoTiledMapData);

    if ((zoomLevel == -1.0) || !windowSize.isValid())
        return;

    bool wasEmpty = (requests.size() == 0);

    QMutableListIterator<QGeoTiledMapRequest> requestIter(requests);
    while (requestIter.hasNext()) {
        QGeoTiledMapRequest req = requestIter.next();
        if (!intersectsScreen(req.tileRect())) {
            requestRects.remove(req.tileRect());
            requestIter.remove();
        }
    }

    QGeoTileIterator it(this);

    while (it.hasNext()) {
        QGeoTiledMapRequest req = it.next();
        QRect tileRect = req.tileRect();

        if (!cache.contains(req)) {
            if (!requestRects.contains(tileRect) && !replyRects.contains(tileRect)) {
                requests.append(req);
                requestRects.insert(tileRect);
            }
        }
    }

//    qWarning()
//            << d->requests.size()
//            << d->cache.size()
//            << d->cache.totalCost()
//            << d->zoomCache.size()
//            << d->zoomCache.totalCost();

    if (wasEmpty && requests.size() > 0) {
        QTimer::singleShot(0, q, SLOT(processRequests()));
    }
}

void QGeoTiledMapDataPrivate::clearRequests()
{
    requests.clear();
    requestRects.clear();
}

void QGeoTiledMapDataPrivate::paintMap(QPainter *painter, const QStyleOptionGraphicsItem * /*option*/)
{
    qreal offsetX = ((windowSize.width() * zoomFactor) - worldReferenceViewportRect.width()) / 2.0;
    if (offsetX < 0.0)
        offsetX = 0.0;
    offsetX /= zoomFactor;
    qreal offsetY = ((windowSize.height() * zoomFactor) - worldReferenceViewportRect.height()) / 2.0;
    if (offsetY < 0.0)
        offsetY = 0.0;
    offsetY /= zoomFactor;

    QGeoTileIterator it(this);

    while (it.hasNext()) {
        QGeoTiledMapRequest req = it.next();
        QRect tileRect = req.tileRect();

        QList<QPair<QRect, QRect> > overlaps = intersectedScreen(tileRect);
        for (int i = 0; i < overlaps.size(); ++i) {
            QRect s = overlaps.at(i).first;
            QRect t = overlaps.at(i).second;

            QRectF source = QRectF(int(s.left()) / zoomFactor,
                                   int(s.top()) / zoomFactor,
                                   int(s.width()) / zoomFactor,
                                   int(s.height()) / zoomFactor);
            QRectF target = QRectF(offsetX + int(t.left()) / zoomFactor,
                                   offsetY + int(t.top()) / zoomFactor,
                                   int(t.width()) / zoomFactor,
                                   int(t.height()) / zoomFactor);

            if (cache.contains(req)) {
                //painter->drawPixmap(target, *cache.object(req), source);
                painter->drawImage(target, *cache.object(req), source);
            } else {
                if (zoomCache.contains(req)) {
                    painter->drawPixmap(target, *zoomCache.object(req), source);
                } else {
                    painter->fillRect(target, Qt::lightGray);
                }
            }
        }
    }
}

void QGeoTiledMapDataPrivate::cleanupCaches()
{
    int boundaryTiles = 3;

    QGeoTiledMappingManagerEngine *tiledEngine = static_cast<QGeoTiledMappingManagerEngine*>(engine);

    QSize tileSize = tiledEngine->tileSize();

    QRectF cacheRect1;
    QRectF cacheRect2;

    cacheRect1 = worldReferenceViewportRect.adjusted(-boundaryTiles * tileSize.width(),
                 -boundaryTiles * tileSize.height(),
                 boundaryTiles * tileSize.width(),
                 boundaryTiles * tileSize.height());

    if (cacheRect1.width() > worldReferenceSize.width()) {
        cacheRect1.setX(0);
        cacheRect1.setWidth(worldReferenceSize.width());
    } else {
        if (cacheRect1.x() + cacheRect1.width() > worldReferenceSize.width()) {
            int oldWidth = cacheRect1.width();
            cacheRect1.setWidth(worldReferenceSize.width() - cacheRect1.x());
            cacheRect2 = QRectF(0,
                                cacheRect1.y(),
                                oldWidth - cacheRect1.width(),
                                cacheRect1.height());
        }
    }

    QList<QGeoTiledMapRequest> keys = cache.keys();
    for (int i = 0; i < keys.size(); ++i) {
        QRectF tileRect = keys.at(i).tileRect();
        if (!cacheRect1.intersects(tileRect)) {
            if (cacheRect2.isNull() || !cacheRect2.intersects(tileRect)) {
                cache.remove(keys.at(i));
            }
        }
    }
}

QRect QGeoTiledMapDataPrivate::screenRectForZoomFactor(int factor)
{
    qreal viewportWidth = q_ptr->windowSize().width();
    qreal viewportHeight = q_ptr->windowSize().height();

    int width = int(viewportWidth * factor);
    int height = int(viewportHeight * factor);

    if (width > worldReferenceSize.width())
        width = worldReferenceSize.width();

    if (height > worldReferenceSize.height())
        height = worldReferenceSize.height();

    int x = (worldReferenceViewportCenter.x() - (width / 2)) % worldReferenceSize.width();
    if (x < 0)
        x += worldReferenceSize.width();

    int y = worldReferenceViewportCenter.y() - (height / 2);

    if (height == worldReferenceSize.height())
        y = 0;

    return QRect(x, y, width, height);
}

void QGeoTiledMapDataPrivate::updateScreenRect()
{
    worldReferenceViewportRect = screenRectForZoomFactor(zoomFactor);

    int x = worldReferenceViewportRect.x();
    int y = worldReferenceViewportRect.y();
    int width = worldReferenceViewportRect.width();
    int height = worldReferenceViewportRect.height();

    if (x + width < worldReferenceSize.width()) {
        worldReferenceViewportRectLeft = worldReferenceViewportRect;
        worldReferenceViewportRectRight = QRect();
    } else {
        int widthLeft = worldReferenceSize.width() - x;
        int widthRight = width - widthLeft;
        worldReferenceViewportRectLeft = QRect(x, y, widthLeft, height);
        worldReferenceViewportRectRight = QRect(0, y, widthRight, height);
    }
}

bool QGeoTiledMapDataPrivate::containedInScreen(const QPoint &point) const
{
    return (worldReferenceViewportRectLeft.contains(point)
            || (worldReferenceViewportRectRight.isValid()
                && worldReferenceViewportRectRight.contains(point)));
}

bool QGeoTiledMapDataPrivate::intersectsScreen(const QRect &rect) const
{
    return (worldReferenceViewportRectLeft.intersects(rect)
            || (worldReferenceViewportRectRight.isValid()
                && worldReferenceViewportRectRight.intersects(rect)));
}

QList<QPair<QRect, QRect> > QGeoTiledMapDataPrivate::intersectedScreen(const QRect &rect, bool translateToScreen) const
{
    QList<QPair<QRect, QRect> > result;

    QRect rectL = rect.intersected(worldReferenceViewportRectLeft);
    if (!rectL.isEmpty()) {
        QRect source = QRect(rectL.topLeft() - rect.topLeft(), rectL.size());
        QRect target = QRect(rectL.topLeft() - worldReferenceViewportRectLeft.topLeft(), rectL.size());
        result << QPair<QRect, QRect>(source, target);
    }

    if (worldReferenceViewportRectRight.isValid()) {
        QRect rectR = rect.intersected(worldReferenceViewportRectRight);
        if (!rectR.isEmpty()) {
            QRect source = QRect(rectR.topLeft() - rect.topLeft(), rectR.size());
            QRect target = QRect(rectR.topLeft() - worldReferenceViewportRectRight.topLeft(), rectR.size());
            if (translateToScreen)
                target.translate(worldReferenceViewportRectLeft.width(), 0);
            result << QPair<QRect, QRect>(source, target);
        }
    }

    return result;
}

/*******************************************************************************
*******************************************************************************/

QGeoTileIterator::QGeoTileIterator(const QGeoTiledMapDataPrivate *mapDataPrivate)
    : atEnd(false),
      row(-1),
      col(-1),
      screenRect(mapDataPrivate->worldReferenceViewportRect),
      mapType(mapDataPrivate->mapType),
      connectivityMode(mapDataPrivate->connectivityMode),
      zoomLevel(mapDataPrivate->zoomLevel)
{
    QGeoTiledMappingManagerEngine *tiledEngine
    = static_cast<QGeoTiledMappingManagerEngine*>(mapDataPrivate->engine);
    tileSize = tiledEngine->tileSize() * mapDataPrivate->zoomFactor;
    tileRect = QRect(QPoint(0, 0), tileSize);

    qulonglong x = static_cast<qulonglong>(screenRect.topLeft().x() / tileSize.width());
    qulonglong y = static_cast<qulonglong>(screenRect.topLeft().y() / tileSize.height());

    width = tileSize.width() * (1 << zoomLevel);

    currTopLeft.setX(x * tileSize.width());
    currTopLeft.setY(y * tileSize.height());
}

QGeoTileIterator::QGeoTileIterator(QGraphicsGeoMap::ConnectivityMode connectivityMode,
                                   QGraphicsGeoMap::MapType mapType,
                                   const QRect &screenRect,
                                   const QSize &tileSize,
                                   int zoomLevel)
    : atEnd(false),
      row(-1),
      col(-1),
      screenRect(screenRect),
      tileSize(tileSize),
      mapType(mapType),
      connectivityMode(connectivityMode),
      zoomLevel(zoomLevel),
      tileRect(QPoint(0, 0), tileSize)
{
    qulonglong x = static_cast<qulonglong>(screenRect.topLeft().x() / tileSize.width());
    qulonglong y = static_cast<qulonglong>(screenRect.topLeft().y() / tileSize.height());

    width = tileSize.width() * (1 << zoomLevel);

    currTopLeft.setX(x * tileSize.width());
    currTopLeft.setY(y * tileSize.height());
}

bool QGeoTileIterator::hasNext()
{
    return !atEnd;
}

QGeoTiledMapRequest QGeoTileIterator::next()
{
    int numCols = 1 << zoomLevel;
    col = static_cast<int>(currTopLeft.x() / tileSize.width()) % numCols;
    row = static_cast<int>(currTopLeft.y() / tileSize.height()) % numCols;
    tileRect.moveTopLeft(currTopLeft);
    if (tileRect.left() >= width)
        tileRect.translate(-width, 0);

    currTopLeft.rx() += tileSize.width();

    if (currTopLeft.x() > screenRect.right()) { //next row
        qulonglong x = static_cast<qulonglong>(screenRect.topLeft().x() / tileSize.width());
        currTopLeft.setX(x * tileSize.width());
        currTopLeft.ry() += tileSize.height();
    }

    if (currTopLeft.y() > screenRect.bottom()) //done
        atEnd = true;

    return QGeoTiledMapRequest(connectivityMode, mapType, zoomLevel, row, col, tileRect);
}

#include "moc_qgeotiledmapdata.cpp"

QTM_END_NAMESPACE
