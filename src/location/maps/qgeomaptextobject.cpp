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

#include "qgeomaptextobject.h"
#include "qgeomaptextobject_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoMapTextObject
    \brief The QGeoMapTextObject class is a QGeoMapObject used to draw
    text on a map.

    \inmodule QtLocation

    \ingroup maps-mapping-objects

    The text is drawn at a given coordinate on the map.

    The text object is drawn such that the rendered size of the text object is
    independent of the zoom level of the map.

    \note Anchor and/or alignment options didn't make it into the beta release
    but will be present in the final 1.1 release.
*/

/*!
    Constructs a new text object.
*/
QGeoMapTextObject::QGeoMapTextObject()
        : d_ptr(new QGeoMapTextObjectPrivate()) {}

/*!
    Constructs a new text object which will display the text \a text with font
    \a font at the coordinate \a coordinate on the map.
*/
QGeoMapTextObject::QGeoMapTextObject(const QGeoCoordinate &coordinate,
                                     const QString &text,
                                     const QFont &font,
                                     const QPoint &offset,
                                     Qt::Alignment alignment)
        : d_ptr(new QGeoMapTextObjectPrivate())
{
    d_ptr->coordinate = coordinate;
    d_ptr->text = text;
    d_ptr->font = font;
    d_ptr->offset = offset;
    d_ptr->alignment = alignment;
}

/*!
    Destroys this text object.
*/
QGeoMapTextObject::~QGeoMapTextObject()
{
    delete d_ptr;
}

/*!
    \reimp
*/
QGeoMapObject::Type QGeoMapTextObject::type() const
{
    return QGeoMapObject::TextType;
}

/*!
    \property QGeoMapTextObject::coordinate
    \brief This property holds the coordinate at which this text object
    will be rendered.

    The default value of this property is an invalid coordinate. While the
    value of this property is invalid the text object will not be displayed.

    If QGeoMapTextObject::offset and QGeoMapTextObject::alignment are not set
    the text will be drawn so that it is centered both horizontally and
    vertically around the position of QGeoMapTextObject::coordinate on the
    screen.
*/
QGeoCoordinate QGeoMapTextObject::coordinate() const
{
    return d_ptr->coordinate;
}

void QGeoMapTextObject::setCoordinate(const QGeoCoordinate &coordinate)
{
    if (d_ptr->coordinate != coordinate) {
        d_ptr->coordinate = coordinate;
        emit coordinateChanged(d_ptr->coordinate);
    }
}

/*!
    \property QGeoMapTextObject::text
    \brief This property holds the text that will be displayed by this text
    object.

    The default value of this property is an empty string.
*/
QString QGeoMapTextObject::text() const
{
    return d_ptr->text;
}

void QGeoMapTextObject::setText(const QString &text)
{
    if (d_ptr->text != text) {
        d_ptr->text = text;
        emit textChanged(d_ptr->text);
    }
}

/*!
    \property QGeoMapTextObject::font
    \brief This property holds the font that will be used to render the text
    for this text object.

    The default value of this property is the application's default font.

    It is not necessary to account for the zoom level of the map, since text
    objects are scaled such that they appear to be independent of the zoom
    level.
*/
QFont QGeoMapTextObject::font() const
{
    return d_ptr->font;
}

void QGeoMapTextObject::setFont(const QFont &font)
{
    if (d_ptr->font != font) {
        d_ptr->font = font;
        emit fontChanged(d_ptr->font);
    }
}

/*!
    \property QGeoMapTextObject::pen
    \brief This property holds the pen that will be used to draw this object.

    The pen is used to draw an outline around the text. The text itself is
    filled using the QGeoMapTextObject::brush property.

    The pen will be treated as a cosmetic pen, which means that the width
    of the pen will be independent of the zoom level of the map.
*/
QPen QGeoMapTextObject::pen() const
{
    return d_ptr->pen;
}

void QGeoMapTextObject::setPen(const QPen &pen)
{
    QPen newPen = pen;
    newPen.setCosmetic(true);

    if (d_ptr->pen == newPen)
        return;

    d_ptr->pen = newPen;
    emit penChanged(d_ptr->pen);
}

/*!
    \property QGeoMapTextObject::brush
    \brief This property holds the brush that will be used to draw this object.

    The brush is used to fill in the glyphs of the text.

    The outline around the perimeter of the glyphs is drawn using the
    QGeoMapTextObject::pen property.
*/
QBrush QGeoMapTextObject::brush() const
{
    return d_ptr->brush;
}

void QGeoMapTextObject::setBrush(const QBrush &brush)
{
    if (d_ptr->brush != brush) {
        d_ptr->brush = brush;
        emit brushChanged(d_ptr->brush);
    }
}

/*!
    \property QGeoMapTextObject::offset
    \brief This property holds the offset in pixels from the screen position of
    QGeoMapTextObject::coordinate at which the text will be rendered when
    drawing this text object.

    The default value of this property is QPoint(0,0).
*/
QPoint QGeoMapTextObject::offset() const
{
    return d_ptr->offset;
}

void QGeoMapTextObject::setOffset(const QPoint &offset)
{
    if (d_ptr->offset != offset) {
        d_ptr->offset = offset;
        emit offsetChanged(d_ptr->offset);
    }
}

/*!
    \property QGeoMapTextObject::alignment
    \brief This property holds the alignment options used to align the
    text when drawing this text object.

    The default value of this property will align the text so that it is
    centered both horizontally and vertically around the point that is
    QGeoMapTextObject::offset pixels away from the position of
    QGeoMapTextObject::coordinate on the screen.

    The alignment does not take the width of QGeoMapTextObject::pen into
    consideration.
*/
Qt::Alignment QGeoMapTextObject::alignment() const
{
    return d_ptr->alignment;
}

void QGeoMapTextObject::setAlignment(Qt::Alignment alignment)
{
    if (d_ptr->alignment != alignment) {
        d_ptr->alignment = alignment;
        emit alignmentChanged(d_ptr->alignment);
    }
}


/*******************************************************************************
*******************************************************************************/

QGeoMapTextObjectPrivate::QGeoMapTextObjectPrivate()
{
    pen.setCosmetic(true);
    alignment = Qt::AlignCenter;
}

QGeoMapTextObjectPrivate::~QGeoMapTextObjectPrivate() {}

#include "moc_qgeomaptextobject.cpp"

QTM_END_NAMESPACE

