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

#ifndef QGEOMAPOBJECT_H
#define QGEOMAPOBJECT_H

#include "qmobilityglobal.h"

#include <QList>
#include <QSharedPointer>
#include <QObject>

class QPainter;
class QRectF;

QTM_BEGIN_NAMESPACE

class QGeoCoordinate;
class QGeoBoundingBox;
class QGeoMapObjectPrivate;
class QGeoMapContainer;

class QGeoMapDataPrivate;

class Q_LOCATION_EXPORT QGeoMapObject : public QObject
{
    Q_OBJECT

public:
    enum Type {
        ContainerType,
        RectangleType,
        CircleType,
        PolylineType,
        PolygonType,
        MarkerType,
        GeoRouteType
    };

    QGeoMapObject(QGeoMapObject *parent = 0);
    virtual ~QGeoMapObject();

    Type type() const;

    void setZValue(int zValue);
    int zValue() const;

    void setVisible(bool visible);
    bool isVisible() const;

    // TODO selection and selectability?

    virtual QGeoBoundingBox boundingBox() const;
    virtual bool contains(const QGeoCoordinate &coordinate) const;

    QGeoMapObject* parentObject() const;
    void addChildObject(QGeoMapObject *childObject);
    void removeChildObject(QGeoMapObject *childObject);
    QList<QGeoMapObject*> childObjects() const;

    bool intersects(const QRectF &rect) const;
    void paint(QPainter *painter, const QRectF &viewPort, bool hitDetection);
    void update();

    bool operator<(const QGeoMapObject &other) const;
    bool operator>(const QGeoMapObject &other) const;

signals:
    void childObjectAdded(QGeoMapObject *childObject);
    void childObjectRemoved(QGeoMapObject *childObject);
    void zValueChanged(int newZValue, int oldZValue);

protected:
    QGeoMapObject(QGeoMapObjectPrivate *dd);

    QGeoMapObjectPrivate *d_ptr;

private slots:
    void childObjectDestroyed(QObject *obj);

private:
    QGeoMapObject(QSharedPointer<QGeoMapDataPrivate> mapData);

    Q_DECLARE_PRIVATE(QGeoMapObject)
    Q_DISABLE_COPY(QGeoMapObject)

    friend class QGeoMapDataPrivate;
};

QTM_END_NAMESPACE

#endif
