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

#include "qorganizeritemlocalidfetchrequest.h"
#include "qorganizeritemrequests_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerItemLocalIdFetchRequest
  \brief The QOrganizerItemLocalIdFetchRequest class allows a client to asynchronously
    request a list of organizer item ids from a organizer item store manager.
  \inmodule QtOrganizer


  For a QOrganizerItemLocalIdFetchRequest, the resultsAvailable() signal will be emitted when the resultant
  manager-local organizer item ids (which may be retrieved by calling ids()), are updated, as well as if
  the overall operation error (which may be retrieved by calling error()) is updated.

  \ingroup organizer-requests
 */

/*! Constructs a new organizer item id fetch request whose parent is the specified \a parent */
QOrganizerItemLocalIdFetchRequest::QOrganizerItemLocalIdFetchRequest(QObject* parent)
    : QOrganizerItemAbstractRequest(new QOrganizerItemLocalIdFetchRequestPrivate, parent)
{
}

/*! Sets the filter which will be used to select the organizer items whose ids will be returned to \a filter */
void QOrganizerItemLocalIdFetchRequest::setFilter(const QOrganizerItemFilter& filter)
{
    Q_D(QOrganizerItemLocalIdFetchRequest);
    d->m_filter = filter;
}

/*! Sets the future sort ordering of the result of the request to \a sorting.  This function only has
    effect on the result if called prior to calling \c start() */
void QOrganizerItemLocalIdFetchRequest::setSorting(const QList<QOrganizerItemSortOrder>& sorting)
{
    Q_D(QOrganizerItemLocalIdFetchRequest);
    d->m_sorting = sorting;
}

void QOrganizerItemLocalIdFetchRequest::setStartDate(const QDateTime &date)
{
    Q_D(QOrganizerItemLocalIdFetchRequest);
    d->m_startDate = date;
}

void QOrganizerItemLocalIdFetchRequest::setEndDate(const QDateTime &date)
{
    Q_D(QOrganizerItemLocalIdFetchRequest);
    d->m_endDate = date;
}

void QOrganizerItemLocalIdFetchRequest::setFindMethod(const QOrganizerItemManager::ItemFindMethod& findMethod)
{
    Q_D(QOrganizerItemLocalIdFetchRequest);
    d->m_findMethod = findMethod;
}

/*! Returns the filter which will be used to select the organizer items whose ids will be returned */
QOrganizerItemFilter QOrganizerItemLocalIdFetchRequest::filter() const
{
    Q_D(const QOrganizerItemLocalIdFetchRequest);
    return d->m_filter;
}

/*! Returns the sort ordering which will be used to sort the result */
QList<QOrganizerItemSortOrder> QOrganizerItemLocalIdFetchRequest::sorting() const
{
    Q_D(const QOrganizerItemLocalIdFetchRequest);
    return d->m_sorting;
}

QDateTime QOrganizerItemLocalIdFetchRequest::startDate() const
{
    Q_D(const QOrganizerItemLocalIdFetchRequest);
    return d->m_startDate;
}

QDateTime QOrganizerItemLocalIdFetchRequest::endDate() const
{
    Q_D(const QOrganizerItemLocalIdFetchRequest);
    return d->m_endDate;
}

QOrganizerItemManager::ItemFindMethod QOrganizerItemLocalIdFetchRequest::findMethod() const
{
    Q_D(const QOrganizerItemLocalIdFetchRequest);
    return d->m_findMethod;
}

/*! Returns the list of ids of organizer items which matched the request */
QList<QOrganizerItemLocalId> QOrganizerItemLocalIdFetchRequest::itemIds() const
{
    Q_D(const QOrganizerItemLocalIdFetchRequest);
    return d->m_ids;
}

#include "moc_qorganizeritemlocalidfetchrequest.cpp"

QTM_END_NAMESPACE
