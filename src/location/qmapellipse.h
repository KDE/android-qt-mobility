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

#ifndef QLOCATION_MAPELLIPSE_H
#define QLOCATION_MAPELLIPSE_H

#include <QPen>
#include <QBrush>
#include <QPainterPath>

#include "qmapobject.h"
#include "qgeocoordinatemaps.h"

QTM_BEGIN_NAMESPACE

/*!
* Internal representation of an ellipse that has been added to the map.
*/
class Q_LOCATION_EXPORT QMapEllipse : public QMapObject
{
    friend class QMapView;

public:
    /*!
    * Constructor.
    * @param topLeft The top left geo coordinate of the bounding box of the ellipse.
    * @param bottomRight The bottom right geo coordinate of the bounding box of the ellipse.
    * @param pen Then pen used for drawing the ellipse.
    * @param brush The brush used for drawing the ellipse.
    */
    QMapEllipse(const QGeoCoordinateMaps& topLeft, const QGeoCoordinateMaps& bottomRight,
                const QPen& pen = QPen(), const QBrush& brush = QBrush(),
                quint16 layerIndex = 0);

    virtual bool intersects(const QRectF& rect) const;
    virtual void compMapCoords();
    virtual void paint(QPainter* painter, const QRectF& viewPort);

    /*!
    * @return The top left geo coordinate of the bounding box of the ellipse.
    */
    QGeoCoordinateMaps topLeft() const {
        return geoTopLeft;
    }
    /*!
    * @return The bottom right geo coordinate of the bounding box of the ellipse.
    */
    QGeoCoordinateMaps bottomRight() const {
        return geoBottomRight;
    }
    /*!
    * @return pen Then pen used for drawing the ellipse.
    */
    QPen pen() const {
        return p;
    }
    /*!
    * @return brush The brush used for drawing the ellipse.
    */
    QBrush brush() const {
        return b;
    }

private:
    QGeoCoordinateMaps geoTopLeft; //!< The top left geo coordinate of the bounding box of the ellipse.
    QGeoCoordinateMaps geoBottomRight; //!< The bottom right geo coordinate of the bounding box of the ellipse.
    QPen p; //!< The pen used for drawing the ellipse.
    QBrush b; //!< The brush used for drawing the ellipse.
    QPainterPath path; //!< The painter path of the ellipse
};

QTM_END_NAMESPACE

#endif
