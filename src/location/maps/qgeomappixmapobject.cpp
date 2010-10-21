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

#include "qgeomappixmapobject.h"
#include "qgeomappixmapobject_p.h"
#include "qgeoboundingbox.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoMapPixmapObject
    \brief The QGeoMapPixmapObject class is a QGeoMapObject used to draw
    a pixmap on a map.

    \inmodule QtLocation

    \ingroup maps-mapping-objects

    The pixmap object is rendered such that the upper left corner of
    QGeoMapPixmapObject::pixmap will be drawn QGeoMapPixmapObject::offset
    pixels away from the position of QGeoMapPixmapObject::coordinate on the
    map.
*/

/*!
    Constructs a new pixmap object.
*/
QGeoMapPixmapObject::QGeoMapPixmapObject()
    : d_ptr(new QGeoMapPixmapObjectPrivate()) {}

/*!
    Constructs a new pixmap object which will draw the pixmap \a pixmap at an
    offset of \a offset pixels from the coordinate \a coordinate.
*/
QGeoMapPixmapObject::QGeoMapPixmapObject(const QGeoCoordinate &coordinate, const QPoint &offset, const QPixmap &pixmap)
    : d_ptr(new QGeoMapPixmapObjectPrivate())
{
    d_ptr->coordinate = coordinate;
    d_ptr->pixmap = pixmap;
    d_ptr->offset = offset;
}

/*!
    Destroys this pixmap object.
*/
QGeoMapPixmapObject::~QGeoMapPixmapObject()
{
    delete d_ptr;
}

/*!
    \reimp
*/
QGeoMapObject::Type QGeoMapPixmapObject::type() const
{
    return QGeoMapObject::PixmapType;
}

/*!
    \property QGeoMapPixmapObject::coordinate
    \brief This property holds the coordinate that specifies where the pixmap
    will be drawn by this pixmap object.

    The default value of this property is an invalid coordinate. While the
    value of this property is invalid the pixmap object will not be displayed.
*/
QGeoCoordinate QGeoMapPixmapObject::coordinate() const
{
    return d_ptr->coordinate;
}

void QGeoMapPixmapObject::setCoordinate(const QGeoCoordinate &coordinate)
{
    if (d_ptr->coordinate != coordinate) {
        d_ptr->coordinate = coordinate;
        emit coordinateChanged(d_ptr->coordinate);
    }
}

/*!
    \property QGeoMapPixmapObject::pixmap
    \brief This property holds the pixmap that will be drawn by this pixmap
    object.

    The default value of this property is a null pixmap. While the value of
    this property is the null pixmap the pixmap object will not be displayed.

    The pixmap will be drawn such that the upper left corner of the pixmap
    will be drawn QGeoMapPixmapObject::offset pixels away from the position of
    QGeoMapPixmapObject::coordinate on the map.
*/
QPixmap QGeoMapPixmapObject::pixmap() const
{
    return d_ptr->pixmap;
}

void QGeoMapPixmapObject::setPixmap(const QPixmap &pixmap)
{
    if (d_ptr->pixmap.isNull() && pixmap.isNull())
        return;

    if ((d_ptr->pixmap.isNull() && !pixmap.isNull())
            || (!d_ptr->pixmap.isNull() && pixmap.isNull())
            || (d_ptr->pixmap.toImage() != pixmap.toImage())) {
        d_ptr->pixmap = pixmap;
        emit pixmapChanged(d_ptr->pixmap);
    }
}

/*!
    \property QGeoMapPixmapObject::offset
    \brief This property holds the offset in pixels at which the pixmap will be
    drawn by this pixmap object.

    The default value of this property is QPoint(0, 0). If this value is not
    changed the upper left coordinate of the pixmap will be drawn at the
    coordinate specified by QGeoMapPixmapObject::coordinate.

    The offset is in pixels and is independent of the zoom level of the map.
    The offset property is provided so that pixmaps such as arrows can be drawn
    with the point of the arrow placed exactly on the associated coordinate.
*/
QPoint QGeoMapPixmapObject::offset() const
{
    return d_ptr->offset;
}

void QGeoMapPixmapObject::setOffset(const QPoint &offset)
{
    if (d_ptr->offset != offset) {
        d_ptr->offset = offset;
        emit offsetChanged(d_ptr->offset);
    }
}

/*!
\fn void QGeoMapPixmapObject::coordinateChanged(const QGeoCoordinate &coordinate)

    This signal is emitted when the coordinate at which the pixmap 
    should be drawn has changed.

    The new value will be \a coordinate.
*/

/*!
\fn void QGeoMapPixmapObject::pixmapChanged(const QPixmap &pixmap)
    
    This signal is emitted when the pixmap associated with this 
    pixmap object has changed.

    The new value will be \a pixmap.
*/

/*!
\fn void QGeoMapPixmapObject::offsetChanged(const QPoint &offset)
    
    This signal is emitted when the on-screen offset from the coordinate 
    at which this pixmap object should be drawn has changed.

    The new value will be \a offset.
*/

/*******************************************************************************
*******************************************************************************/

QGeoMapPixmapObjectPrivate::QGeoMapPixmapObjectPrivate() {}

QGeoMapPixmapObjectPrivate::~QGeoMapPixmapObjectPrivate() {}

#include "moc_qgeomappixmapobject.cpp"

QTM_END_NAMESPACE

