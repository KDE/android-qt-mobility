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

#ifndef QGEOMAPRECTANGLEOBJECT_H
#define QGEOMAPRECTANGLEOBJECT_H

#include "qgeomapobject.h"

#include <QPen>
#include <QBrush>

QTM_BEGIN_NAMESPACE

class QGeoMapRectangleObjectPrivate;
class QGeoBoundingBox;
class QGeoCoordinate;

class Q_LOCATION_EXPORT QGeoMapRectangleObject : public QGeoMapObject
{
    Q_OBJECT
    Q_PROPERTY(QGeoCoordinate topLeft READ topLeft WRITE setTopLeft NOTIFY topLeftChanged)
    Q_PROPERTY(QGeoCoordinate bottomRight READ bottomRight WRITE setBottomRight NOTIFY bottomRightChanged)
    Q_PROPERTY(QPen pen READ pen WRITE setPen NOTIFY penChanged)
    Q_PROPERTY(QBrush brush READ brush WRITE setBrush NOTIFY brushChanged)

public:
    QGeoMapRectangleObject();
    QGeoMapRectangleObject(const QGeoBoundingBox &boundingBox);
    QGeoMapRectangleObject(const QGeoCoordinate &topLeft, const QGeoCoordinate &bottomRight);
    ~QGeoMapRectangleObject();

    QGeoMapObject::Type type() const;

    QGeoBoundingBox bounds() const;
    void setBounds(const QGeoBoundingBox &bounds);

    QGeoCoordinate topLeft() const;
    void setTopLeft(const QGeoCoordinate &topLeft);

    QGeoCoordinate bottomRight() const;
    void setBottomRight(const QGeoCoordinate &bottomRight);

    QPen pen() const;
    void setPen(const QPen &pen);

    QBrush brush() const;
    void setBrush(const QBrush &brush);

Q_SIGNALS:
    void topLeftChanged(const QGeoCoordinate &topLeft);
    void bottomRightChanged(const QGeoCoordinate &bottomRight);
    void penChanged(const QPen &pen);
    void brushChanged(const QBrush &brush);

private:
    QGeoMapRectangleObjectPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QGeoMapRectangleObject)
    Q_DISABLE_COPY(QGeoMapRectangleObject)
};

QTM_END_NAMESPACE

#endif
