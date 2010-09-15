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

#include "qlandmarksortorder.h"
#include "qlandmarksortorder_p.h"
#include "qlandmarkid.h"

#if !defined(Q_CC_MWERKS)
template<> QTM_PREPEND_NAMESPACE(QLandmarkSortOrderPrivate) *QSharedDataPointer<QTM_PREPEND_NAMESPACE(QLandmarkSortOrderPrivate)>::clone()
{
    return d->clone();
}
#endif

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkSortOrder
    \brief The QLandmarkSortOrder class serves as a base class
           for the different sort order types.

    \inmodule QtLocation
    
    \ingroup landmarks-sort

    The sort orders define how a list of landmarks should be ordered
    when they searched using a QLandmarkManager or one of the
    landmark request classes.
*/

/*!
    \enum QLandmarkSortOrder::SortType
    Defines the type of sort order.
    \value DefaultSort The default sort order that does no sorting.
    \value NameSort   Sorts landmarks by name.
*/

/*!
    Constructs a landmark sort order.

    The type of the sort order is InvalidSort.
*/
QLandmarkSortOrder::QLandmarkSortOrder()
        : d_ptr(new QLandmarkSortOrderPrivate())
{
}

/*!
  \internal
*/
QLandmarkSortOrder::QLandmarkSortOrder(QLandmarkSortOrderPrivate *d_ptr)
        : d_ptr(d_ptr)
{
}

/*!
    Constructs a copy of \a other.
*/
QLandmarkSortOrder::QLandmarkSortOrder(const QLandmarkSortOrder &other)
        : d_ptr(other.d_ptr)
{
}

/*!
    Assigns \a other to this sort order and returns a reference to this
    sort order.
*/
QLandmarkSortOrder &QLandmarkSortOrder::operator=(const QLandmarkSortOrder & other)
{
    d_ptr = other.d_ptr;
    return *this;
}

/*!
    Destroys the sort order
*/
QLandmarkSortOrder::~QLandmarkSortOrder()
{
}

/*!
    Returns the type of sort.
*/
QLandmarkSortOrder::SortType QLandmarkSortOrder::type() const
{
    return d_ptr->type;
}

/*!
    Returns the direction of the sort order.
*/
Qt::SortOrder QLandmarkSortOrder::direction() const
{
    return d_ptr->order;
}

/*!
    Sets the \a direction of the sort order.
*/
void QLandmarkSortOrder::setDirection(Qt::SortOrder direction)
{
    d_ptr->order = direction;
}

/*!
    \fn QLandmarkSortOrder::operator==(const QLandmarkSortOrder& other) const
    Returns true if this sort order is identical to \a other.
    \sa operator!=()
*/
bool QLandmarkSortOrder::operator==(const QLandmarkSortOrder& other) const
{

    if(!d_ptr->QLandmarkSortOrderPrivate::compare(other.d_ptr))
        return false;

    /* Different types can't be equal */
    if (other.type() != type())
        return false;

    /* Otherwise, use the virtual op == */
    return d_ptr->compare(other.d_ptr);
}

/*!
    \fn QLandmarkSortOrder::operator!=(const QLandmarkSortOrder &other) const
    Returns true if this sort order is not identical to \a other.

    \sa operator==()
*/

/*******************************************************************************
*******************************************************************************/

QLandmarkSortOrderPrivate::QLandmarkSortOrderPrivate()
        : QSharedData(),
          type(QLandmarkSortOrder::DefaultSort),
          order(Qt::AscendingOrder)
{
}

QLandmarkSortOrderPrivate::QLandmarkSortOrderPrivate(const QLandmarkSortOrderPrivate &other)
        : QSharedData(),
          type(other.type),
          order(other.order)
{
}

QLandmarkSortOrderPrivate::~QLandmarkSortOrderPrivate()
{
}

QTM_END_NAMESPACE
