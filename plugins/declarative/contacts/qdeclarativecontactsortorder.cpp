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


#include "qdeclarativecontactsortorder_p.h"


QDeclarativeContactSortOrder::QDeclarativeContactSortOrder(QObject* parent)
    :QObject(parent)
{
}

QString QDeclarativeContactSortOrder::detailDefinitionName() const
{
    return m_sortOrder.detailDefinitionName();
}

void QDeclarativeContactSortOrder::setDetailDefinitionName(const QString& definitionName)
{
    m_sortOrder.setDetailDefinitionName(definitionName, m_sortOrder.detailFieldName());
}

QDeclarativeContactSortOrder::BlankPolicy QDeclarativeContactSortOrder::blankPolicy() const
{
    if (m_sortOrder.blankPolicy() == QContactSortOrder::BlanksFirst)
        return QDeclarativeContactSortOrder::BlanksFirst;
    return QDeclarativeContactSortOrder::BlanksLast;
}

void QDeclarativeContactSortOrder::setBlankPolicy(QDeclarativeContactSortOrder::BlankPolicy blankPolicy)
{
    if (blankPolicy == QDeclarativeContactSortOrder::BlanksFirst)
        m_sortOrder.setBlankPolicy(QContactSortOrder::BlanksFirst);
    else
        m_sortOrder.setBlankPolicy(QContactSortOrder::BlanksLast);
}

QDeclarativeContactSortOrder::SortOrder QDeclarativeContactSortOrder::direction() const
{
    if (m_sortOrder.direction() == Qt::DescendingOrder)
        return QDeclarativeContactSortOrder::DescendingOrder;
    return QDeclarativeContactSortOrder::AscendingOrder;
}
void QDeclarativeContactSortOrder::setDirection(QDeclarativeContactSortOrder::SortOrder direction)
{
    if (direction == QDeclarativeContactSortOrder::DescendingOrder)
        m_sortOrder.setDirection(Qt::DescendingOrder);
    else
        m_sortOrder.setDirection(Qt::AscendingOrder);
}

QDeclarativeContactSortOrder::CaseSensitivity QDeclarativeContactSortOrder::caseSensitivity() const
{
    if (m_sortOrder.caseSensitivity() == Qt::CaseInsensitive)
        return QDeclarativeContactSortOrder::CaseInsensitive;
    return QDeclarativeContactSortOrder::CaseSensitive;
}
void QDeclarativeContactSortOrder::setCaseSensitivity(QDeclarativeContactSortOrder::CaseSensitivity sensitivity)
{
    if (sensitivity == QDeclarativeContactSortOrder::CaseInsensitive)
        m_sortOrder.setCaseSensitivity(Qt::CaseInsensitive);
    else
        m_sortOrder.setCaseSensitivity(Qt::CaseSensitive);
}

QString QDeclarativeContactSortOrder::detailFieldName() const
{
    return m_sortOrder.detailFieldName();
}
void QDeclarativeContactSortOrder::setDetailFieldName(const QString& fieldName)
{
    m_sortOrder.setDetailDefinitionName(m_sortOrder.detailDefinitionName(), fieldName);
}

QContactSortOrder QDeclarativeContactSortOrder::sortOrder()
{
    return m_sortOrder;
}

void QDeclarativeContactSortOrder::setSortOrder(const QContactSortOrder& sortOrder)
{
    m_sortOrder = sortOrder;
    emit sortOrderChanged();
}
