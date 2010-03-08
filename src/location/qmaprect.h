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

#ifndef QLOCATION_MAPRECT_H
#define QLOCATION_MAPRECT_H

#include <QPen>
#include <QBrush>
#include <QRectF>

#include "qmapobject.h"
#include "qgeocoordinatemaps.h"

QTM_BEGIN_NAMESPACE

/*!
* Internal representation of a rectangle that has been added to the map.
*/
class QMapRect : public QMapObject
{
    friend class QMapView;

public:
    /*!
    * Constructor.
    * @param topLeft The top left geo coordinate of the rectangle.
    * @param bottomRight The bottom right geo coordinate of the rectangle.
    * @param pen Then pen used for drawing the rectangle.
    * @param brush The brush used for drawing the rectangle.
    * @param layerIndex The layer index of the rectangle. Higher layers are stacked on top of lower layers.
    */
    QMapRect(const QMapView& mapView, const QGeoCoordinateMaps& topLeft,
             const QGeoCoordinateMaps& bottomRight, const QPen& pen = QPen(),
             const QBrush& brush = QBrush(), quint16 layerIndex = 0);

    /*!
    * @return The top left geo coordinate of the rectangle.
    */
    QGeoCoordinateMaps topLeft() const {
        return geoTopLeft;
    }
    /*!
    * @return The bottom right geo coordinate of the rectangle.
    */
    QGeoCoordinateMaps bottomRight() const {
        return geoBottomRight;
    }
    /*!
    * @return pen Then pen used for drawing the rectangle.
    */
    QPen pen() const {
        return p;
    }
    /*!
    * @return brush The brush used for drawing the rectangle.
    */
    QBrush brush() const {
        return b;
    }

protected:
    virtual void compMapCoords();
    virtual bool intersects(const QRectF& rect) const;
    virtual void paint(QPainter* painter, const QRectF& viewPort);

private:
    QGeoCoordinateMaps geoTopLeft; //!< The top left geo coordinate of the rectangle.
    QGeoCoordinateMaps geoBottomRight; //!< The bottom right geo coordinate of the rectangle.
    QPen p; //!< The pen used for drawing the rectangle.
    QBrush b; //!< The brush used for drawing the rectangle.
    QRectF rect;
};

QTM_END_NAMESPACE

#endif
