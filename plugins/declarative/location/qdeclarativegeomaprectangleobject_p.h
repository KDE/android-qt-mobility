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

#ifndef QDECLARATIVEGEOMAPRECTANGLEOBJECT_H
#define QDECLARATIVEGEOMAPRECTANGLEOBJECT_H

#include "qdeclarativecoordinate_p.h"
#include "qgeomaprectangleobject.h"

class QColor;
class QBrush;

QTM_BEGIN_NAMESPACE

class QDeclarativeGeoMapRectangleObject : public QGeoMapRectangleObject
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeCoordinate* topLeft READ declarativeTopLeft WRITE setDeclarativeTopLeft NOTIFY declarativeTopLeftChanged)
    Q_PROPERTY(QDeclarativeCoordinate* bottomRight READ declarativeBottomRight WRITE setDeclarativeBottomRight NOTIFY declarativeBottomRightChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    QDeclarativeGeoMapRectangleObject();
    ~QDeclarativeGeoMapRectangleObject();

    QDeclarativeCoordinate* declarativeTopLeft() const;
    void setDeclarativeTopLeft(const QDeclarativeCoordinate *center);

    QDeclarativeCoordinate* declarativeBottomRight() const;
    void setDeclarativeBottomRight(const QDeclarativeCoordinate *center);

    QColor color() const;
    void setColor(const QColor &color);

private slots:
    void memberTopLeftChanged(const QGeoCoordinate &coordinate);
    void memberBottomRightChanged(const QGeoCoordinate &coordinate);
    void memberBrushChanged(const QBrush &brush);

signals:
    void declarativeTopLeftChanged(const QDeclarativeCoordinate *center);
    void declarativeBottomRightChanged(const QDeclarativeCoordinate *center);
    void colorChanged(const QColor &color);

private:
    mutable QDeclarativeCoordinate* m_topLeft;
    mutable QDeclarativeCoordinate* m_bottomRight;
    mutable QColor m_color;
    Q_DISABLE_COPY(QDeclarativeGeoMapRectangleObject)
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGeoMapRectangleObject));

#endif
