
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
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativegeomappolygonobject_p.h"

#include <QColor>
#include <QBrush>

QTM_BEGIN_NAMESPACE

QDeclarativeGeoMapPolygonObject::QDeclarativeGeoMapPolygonObject()
{
}

QDeclarativeGeoMapPolygonObject::~QDeclarativeGeoMapPolygonObject()
{
    qDeleteAll(m_path);
}

QDeclarativeListProperty<QDeclarativeCoordinate> QDeclarativeGeoMapPolygonObject::declarativePath()
{
    return QDeclarativeListProperty<QDeclarativeCoordinate>(this,
            0,
            path_append,
            path_count,
            path_at,
            path_clear);
}

void QDeclarativeGeoMapPolygonObject::path_append(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, QDeclarativeCoordinate *coordinate)
{
    QDeclarativeGeoMapPolygonObject* poly = static_cast<QDeclarativeGeoMapPolygonObject*>(prop->object);
    poly->m_path.append(coordinate);
    QList<QGeoCoordinate> p = poly->path();
    p.append(coordinate->coordinate());
    poly->setPath(p);
}

int QDeclarativeGeoMapPolygonObject::path_count(QDeclarativeListProperty<QDeclarativeCoordinate> *prop)
{
    return static_cast<QDeclarativeGeoMapPolygonObject*>(prop->object)->m_path.count();
}

QDeclarativeCoordinate* QDeclarativeGeoMapPolygonObject::path_at(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, int index)
{
    return static_cast<QDeclarativeGeoMapPolygonObject*>(prop->object)->m_path.at(index);
}

void QDeclarativeGeoMapPolygonObject::path_clear(QDeclarativeListProperty<QDeclarativeCoordinate> *prop)
{
    QDeclarativeGeoMapPolygonObject* poly = static_cast<QDeclarativeGeoMapPolygonObject*>(prop->object);
    QList<QDeclarativeCoordinate*> p = poly->m_path;
    qDeleteAll(p);
    p.clear();
    poly->setPath(QList<QGeoCoordinate>());
}

void QDeclarativeGeoMapPolygonObject::setColor(const QColor &color)
{
    if (m_color == color)
        return;

    m_color = color;
    QBrush m_brush(color);
    setBrush(m_brush);
    emit colorChanged(m_color);
}

QColor QDeclarativeGeoMapPolygonObject::color() const
{
    return m_color;
}

#include "moc_qdeclarativegeomappolygonobject_p.cpp"

QTM_END_NAMESPACE

