/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QDECLARATIVEORGANIZERITEMSORTORDER_H
#define QDECLARATIVEORGANIZERITEMSORTORDER_H

#include "qdeclarative.h"

#include "qorganizeritemsortorder.h"

QTM_USE_NAMESPACE

class QDeclarativeOrganizerItemSortOrder : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString definitionName READ definitionName WRITE setDefinitionName NOTIFY valueChanged)
    Q_PROPERTY(QString fieldName READ fieldName WRITE setFieldName NOTIFY valueChanged)
    Q_PROPERTY(BlankPolicy blankPolicy READ blankPolicy WRITE setBlankPolicy NOTIFY valueChanged)
    Q_PROPERTY(Qt::SortOrder direction READ direction WRITE setDirection NOTIFY valueChanged)
    Q_PROPERTY(Qt::CaseSensitivity sensitivity READ caseSensitivity WRITE setCaseSensitivity NOTIFY valueChanged)

    Q_ENUMS(BlankPolicy)
public:
    QDeclarativeOrganizerItemSortOrder(QObject* parent = 0)
        :QObject(parent)
    {}

    enum BlankPolicy {
        BlanksFirst = QOrganizerItemSortOrder::BlanksFirst,
        BlanksLast = QOrganizerItemSortOrder::BlanksLast,
    };


    void setDefinitionName(const QString& name)
    {
        d.setDetailDefinitionName(name, d.detailFieldName());
    }

    void setFieldName(const QString& name)
    {
        d.setDetailDefinitionName(d.detailDefinitionName(), name);
    }

    void setBlankPolicy(BlankPolicy blankPolicy)
    {
        d.setBlankPolicy(static_cast<QOrganizerItemSortOrder::BlankPolicy>(blankPolicy));
    }

    void setDirection(Qt::SortOrder direction)
    {
        d.setDirection(direction);
    }

    void setCaseSensitivity(Qt::CaseSensitivity sensitivity)
    {
        d.setCaseSensitivity(sensitivity);
    }

    QString definitionName() const
    {
        return d.detailDefinitionName();
    }
    QString fieldName() const
    {
        return d.detailFieldName();
    }
    BlankPolicy blankPolicy() const
    {
        return static_cast<QDeclarativeOrganizerItemSortOrder::BlankPolicy>(d.blankPolicy());
    }
    Qt::SortOrder direction() const
    {
        return d.direction();
    }
    Qt::CaseSensitivity caseSensitivity() const
    {
        return d.caseSensitivity();
    }

    QOrganizerItemSortOrder sortOrder() const
    {
        return d;
    }

    void setSortOrder(const QOrganizerItemSortOrder& sortOrder)
    {
        d = sortOrder;
    }

signals:
    void valueChanged();
private:
    QOrganizerItemSortOrder d;
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemSortOrder)


#endif
