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

#include "qgeomappolylineobject.h"
#include "qgeomappolylineobject_p.h"

#include "qgeocoordinate.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoMapPolylineObject
    \brief The QGeoMapPolylineObject class is a QGeoMapObject used to draw
    a segmented line on a map.

    \inmodule QtLocation

    \ingroup maps-mapping-objects

    The polyline is specified by a list of at least 2 valid QGeoCoordinate
    instances and a line will be drawn between every adjacent pairs of
    coordinates in the list.
*/

/*!
    Constructs a new polyline object.
*/
QGeoMapPolylineObject::QGeoMapPolylineObject()
    : d_ptr(new QGeoMapPolylineObjectPrivate()) {}

/*!
    Destroys this polyline object.
*/
QGeoMapPolylineObject::~QGeoMapPolylineObject()
{
    delete d_ptr;
}

/*!
    \reimp
*/
QGeoMapObject::Type QGeoMapPolylineObject::type() const
{
    return QGeoMapObject::PolylineType;
}

/*!
    \property QGeoMapPolylineObject::path
    \brief This property holds the ordered list of coordinates which define the
    segmented line to be drawn by this polyline object.

    The default value of this property is an empty list of coordinates.

    A line will be drawn between every pair of coordinates which are adjacent in
    the list.

    Invalid coordinates in the list will be ignored, and if the list of
    coordinates contains less than 2 valid coordinates then the polyline object
    will not be displayed.
*/
void QGeoMapPolylineObject::setPath(const QList<QGeoCoordinate> &path)
{
    if (d_ptr->path != path) {
        d_ptr->path = path;
        emit pathChanged(d_ptr->path);
    }
}

QList<QGeoCoordinate> QGeoMapPolylineObject::path() const
{
    return d_ptr->path;
}

/*!
    \property QGeoMapPolylineObject::pen
    \brief This property holds the pen that will be used to draw this object.

    The pen is used to draw the polyline.

    The pen will be treated as a cosmetic pen, which means that the width
    of the pen will be independent of the zoom level of the map.
*/
void QGeoMapPolylineObject::setPen(const QPen &pen)
{
    QPen newPen = pen;
    newPen.setCosmetic(false);

    if (d_ptr->pen == newPen)
        return;

    d_ptr->pen = newPen;
    emit penChanged(d_ptr->pen);
}

QPen QGeoMapPolylineObject::pen() const
{
    return d_ptr->pen;
}

/*!
\fn void QGeoMapPolylineObject::pathChanged(const QList<QGeoCoordinate> &path)

    This signal is emitted when the ordered list of coordinates that define 
    the polyline to be drawn by this polyline object has changed.

    The new value is \a path.
*/

/*!
\fn void QGeoMapPolylineObject::penChanged(const QPen &pen)

    This signal is emitted when the pen used to draw the edge of this 
    polyline object has changed.

    The new value is \a pen.
*/

/*******************************************************************************
*******************************************************************************/

QGeoMapPolylineObjectPrivate::QGeoMapPolylineObjectPrivate()
{
    pen.setCosmetic(false);
}

QGeoMapPolylineObjectPrivate::~QGeoMapPolylineObjectPrivate() {}

#include "moc_qgeomappolylineobject.cpp"

QTM_END_NAMESPACE

