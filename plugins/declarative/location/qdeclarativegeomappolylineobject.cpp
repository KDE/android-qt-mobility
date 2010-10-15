
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

#include "qdeclarativegeomappolylineobject_p.h"

#include <QColor>
#include <QBrush>

QTM_BEGIN_NAMESPACE

QDeclarativeGeoMapPolylineObject::QDeclarativeGeoMapPolylineObject()
{
    connect(&m_border,
            SIGNAL(colorChanged(QColor)),
            this,
            SLOT(borderColorChanged(QColor)));
    connect(&m_border,
            SIGNAL(widthChanged(int)),
            this,
            SLOT(borderWidthChanged(int)));
}

QDeclarativeGeoMapPolylineObject::~QDeclarativeGeoMapPolylineObject()
{
    qDeleteAll(m_path);
}

QDeclarativeListProperty<QDeclarativeCoordinate> QDeclarativeGeoMapPolylineObject::declarativePath()
{
    return QDeclarativeListProperty<QDeclarativeCoordinate>(this,
            0,
            path_append,
            path_count,
            path_at,
            path_clear);
}

void QDeclarativeGeoMapPolylineObject::path_append(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, QDeclarativeCoordinate *coordinate)
{
    QDeclarativeGeoMapPolylineObject* poly = static_cast<QDeclarativeGeoMapPolylineObject*>(prop->object);
    poly->m_path.append(coordinate);
    QList<QGeoCoordinate> p = poly->path();
    p.append(coordinate->coordinate());
    poly->setPath(p);
}

int QDeclarativeGeoMapPolylineObject::path_count(QDeclarativeListProperty<QDeclarativeCoordinate> *prop)
{
    return static_cast<QDeclarativeGeoMapPolylineObject*>(prop->object)->m_path.count();
}

QDeclarativeCoordinate* QDeclarativeGeoMapPolylineObject::path_at(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, int index)
{
    return static_cast<QDeclarativeGeoMapPolylineObject*>(prop->object)->m_path.at(index);
}

void QDeclarativeGeoMapPolylineObject::path_clear(QDeclarativeListProperty<QDeclarativeCoordinate> *prop)
{
    QDeclarativeGeoMapPolylineObject* poly = static_cast<QDeclarativeGeoMapPolylineObject*>(prop->object);
    QList<QDeclarativeCoordinate*> p = poly->m_path;
    qDeleteAll(p);
    p.clear();
    poly->setPath(QList<QGeoCoordinate>());
}

QDeclarativeGeoMapObjectBorder* QDeclarativeGeoMapPolylineObject::border()
{
    return &m_border;
}

void QDeclarativeGeoMapPolylineObject::borderColorChanged(const QColor &color)
{
    QPen p = pen();
    p.setColor(color);
    setPen(p);
}

void QDeclarativeGeoMapPolylineObject::borderWidthChanged(int width)
{
    QPen p = pen();
    p.setWidth(width);
    if (width == 0)
        p.setStyle(Qt::NoPen);
    else
        p.setStyle(Qt::SolidLine);
    setPen(p);
}

#include "moc_qdeclarativegeomappolylineobject_p.cpp"

QTM_END_NAMESPACE

