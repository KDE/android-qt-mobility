/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qorganizeritemsortorder.h"
#include "qorganizeritemsortorder_p.h"

#ifndef QT_NO_DEBUG_STREAM
#include <QDebug>
#endif

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerItemSortOrder
  \brief The QOrganizerItemSortOrder class defines how a list of organizer items should be ordered according to some criteria

  \inmodule QtOrganizer
 */

/*!
 * \enum QOrganizerItemSortOrder::BlankPolicy
 * Enumerates the ways in which the sort order interprets blanks when sorting organizer items
 * \value BlanksFirst Considers blank values to evaluate to less than all other values in comparisons
 * \value BlanksLast Considers blank values to evaluate to greater than all other values in comparisons
 */

/*!
 * \fn QOrganizerItemSortOrder::operator QList<QOrganizerItemSortOrder>() const
 * Constructs a new list of sort orders containing only the current sort order
 */

/*!
 * \fn QOrganizerItemSortOrder::operator!=(const QOrganizerItemSortOrder& other) const
 * Returns true if this sort order is not identical to the \a other sort order
 * \sa operator==()
 */

/*!
 * Constructs a new sort order
 */
QOrganizerItemSortOrder::QOrganizerItemSortOrder()
    : d(new QOrganizerItemSortOrderPrivate())
{
}

/*!
 * Frees any memory in use by this sort order
 */
QOrganizerItemSortOrder::~QOrganizerItemSortOrder()
{
}

/*!
 * Constructs a copy of the \a other sort order
 */
QOrganizerItemSortOrder::QOrganizerItemSortOrder(const QOrganizerItemSortOrder& other)
    : d(other.d)
{

}

/*!
 * Assigns this sort order to be equal to \a other
 */
QOrganizerItemSortOrder& QOrganizerItemSortOrder::operator=(const QOrganizerItemSortOrder& other)
{
    if (this != &other) {
        d = other.d;
    }
    return *this;
}

/*!
 * Returns true if the sort order is able to be used to sort a list of organizer items; otherwise, returns false
 */
bool QOrganizerItemSortOrder::isValid() const
{
    /* We clear both when one is empty, so we only need to check one */
    if (d->m_definitionName.isEmpty())
        return false;
    return true;
}

/*!
 * Returns true if this sort order is identical to the \a other sort order
 * \sa operator!=()
 */
bool QOrganizerItemSortOrder::operator ==(const QOrganizerItemSortOrder& other) const
{
    if (d->m_blankPolicy == other.d->m_blankPolicy &&
        d->m_direction == other.d->m_direction &&
        d->m_sensitivity == other.d->m_sensitivity &&
        d->m_definitionName == other.d->m_definitionName &&
        d->m_fieldName == other.d->m_fieldName)
        return true;
    return false;
}

#ifndef QT_NO_DATASTREAM
/*!
 * Writes \a sortOrder to the stream \a out.
 */
QDataStream& operator<<(QDataStream& out, const QOrganizerItemSortOrder& sortOrder)
{
    quint8 formatVersion = 1; // Version of QDataStream format for QOrganizerItemSortOrder
    return out << formatVersion
               << sortOrder.detailDefinitionName()
               << sortOrder.detailFieldName()
               << static_cast<quint32>(sortOrder.blankPolicy())
               << static_cast<quint32>(sortOrder.direction())
               << static_cast<quint32>(sortOrder.caseSensitivity());
}

/*!
 * Reads a sort order from stream \a in into \a sortOrder.
 */
QDataStream& operator>>(QDataStream& in, QOrganizerItemSortOrder& sortOrder)
{
    quint8 formatVersion;
    in >> formatVersion;
    if (formatVersion == 1) {
        QString definitionName;
        QString fieldName;
        quint32 blankPolicy;
        quint32 direction;
        quint32 caseSensitivity;
        in >> definitionName >> fieldName >> blankPolicy >> direction >> caseSensitivity;
        sortOrder.setDetailDefinitionName(definitionName, fieldName);
        sortOrder.setBlankPolicy(static_cast<QOrganizerItemSortOrder::BlankPolicy>(blankPolicy));
        sortOrder.setDirection(static_cast<Qt::SortOrder>(direction));
        sortOrder.setCaseSensitivity(static_cast<Qt::CaseSensitivity>(caseSensitivity));
    } else {
        in.setStatus(QDataStream::ReadCorruptData);
    }
    return in;
}
#endif

#ifndef QT_NO_DEBUG_STREAM
/*!
  Outputs \a sortOrder to the debug stream \a dbg
 */
QDebug operator<<(QDebug dbg, const QOrganizerItemSortOrder& sortOrder)
{
    dbg.nospace() << "QOrganizerItemSortOrder(";
    dbg.nospace() << "detailDefinitionName=";
    dbg.nospace() << sortOrder.detailDefinitionName();
    dbg.nospace() << ",";
    dbg.nospace() << "detailFieldName=";
    dbg.nospace() << sortOrder.detailFieldName();
    dbg.nospace() << ",";
    dbg.nospace() << "blankPolicy=";
    dbg.nospace() << static_cast<quint32>(sortOrder.blankPolicy());
    dbg.nospace() << ","; 
    dbg.nospace() << "direction=";
    dbg.nospace() << static_cast<quint32>(sortOrder.direction());
    dbg.nospace() << ",";
    dbg.nospace() << "caseSensitivity=";
    dbg.nospace() << static_cast<quint32>(sortOrder.caseSensitivity());
    dbg.nospace() << ")";
    return dbg.maybeSpace();
}
#endif

/*!
 * Sets the definition name of the details which will be inspected to perform sorting to \a definitionName,
 * and the name of those details' fields which contains the value which organizer items will be sorted by to \a fieldName
 * \sa detailDefinitionName(), detailFieldName()
 */
void QOrganizerItemSortOrder::setDetailDefinitionName(const QString& definitionName, const QString& fieldName)
{
    if (definitionName.isEmpty() || fieldName.isEmpty()) {
        d->m_definitionName.clear();
        d->m_fieldName.clear();
    } else {
        d->m_definitionName = definitionName;
        d->m_fieldName = fieldName;
    }
}

/*!
 * Sets the sort order's policy on blank values with respect to sorting to \a blankPolicy
 * \sa blankPolicy()
 */
void QOrganizerItemSortOrder::setBlankPolicy(BlankPolicy blankPolicy)
{
    d->m_blankPolicy = blankPolicy;
}

/*!
 * Sets the sort order direction to \a direction
 * \sa direction()
 */
void QOrganizerItemSortOrder::setDirection(Qt::SortOrder direction)
{
    d->m_direction = direction;
}

/*!
 * Returns the definition name of the details which will be inspected to perform sorting.
 * Note that if an organizer item has multiple details of the definition, the result of the sorting
 * is undefined.
 * \sa setDetailDefinitionName()
 */
QString QOrganizerItemSortOrder::detailDefinitionName() const
{
    return d->m_definitionName;
}

/*!
 * Returns the name of the field in the definition which will be inspected to perform sorting
 * \sa setDetailDefinitionName()
 */
QString QOrganizerItemSortOrder::detailFieldName() const
{
    return d->m_fieldName;
}

/*!
 * Returns the blank policy of the sort order
 * \sa setBlankPolicy()
 */
QOrganizerItemSortOrder::BlankPolicy QOrganizerItemSortOrder::blankPolicy() const
{
    return d->m_blankPolicy;
}

/*!
 * Returns the direction of the sort order
 * \sa setDirection()
 */
Qt::SortOrder QOrganizerItemSortOrder::direction() const
{
    return d->m_direction;
}

/*!
 * Returns the case sensitivity of the sort order
 * \sa setCaseSensitivity()
 */
Qt::CaseSensitivity QOrganizerItemSortOrder::caseSensitivity() const
{
    return d->m_sensitivity;
}

/*!
 * Sets the case sensitivity of the sort order to \a sensitivity
 * \sa caseSensitivity()
 */
void QOrganizerItemSortOrder::setCaseSensitivity(Qt::CaseSensitivity sensitivity)
{
    d->m_sensitivity = sensitivity;
}

QTM_END_NAMESPACE
