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

#include "qcontactfetchrequest.h"
#include "qcontactrequests_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QContactFetchRequest
  \brief The QContactFetchRequest class allows a client to asynchronously
    request contacts from a contacts store manager.
   \ingroup contacts-requests
 */

/*!
 * \fn QContactFetchRequest::progress(QContactFetchRequest* self, bool appendOnly)
 * This signal is emitted when some progress has been made on the request, causing either a change of
 * status or an update of results, or both.  It identifies which request the signal originated from
 * by including a pointer to \a self, and contains an \a appendOnly flag which signifies whether or not the total
 * ordering of the results have been maintained since the last progress signal was emitted.
 */

/*! Constructs a new contact fetch request */
QContactFetchRequest::QContactFetchRequest()
    : QContactAbstractRequest(new QContactFetchRequestPrivate)
{
}

/*! Cleans up the memory in use by this contact fetch request */
QContactFetchRequest::~QContactFetchRequest()
{
}

/*! Sets the contact filter used to determine which contacts will be retrieved to \a filter */
void QContactFetchRequest::setFilter(const QContactFilter& filter)
{
    Q_D(QContactFetchRequest);
    d->m_filter = filter;
}

/*! Sets the sort order of the result to \a sorting.  Only has an effect if called prior to calling \c start() */
void QContactFetchRequest::setSorting(const QList<QContactSortOrder>& sorting)
{
    Q_D(QContactFetchRequest);
    d->m_sorting = sorting;
}

/*! Sets the list of allowable detail definition names to \a definitionNames.  Any contacts retrieved
    by the request will have any details whose definition name is not on the restricted list
    removed prior to being returned. */
void QContactFetchRequest::setDefinitionRestrictions(const QStringList& definitionNames)
{
    Q_D(QContactFetchRequest);
    d->m_definitionRestrictions = definitionNames;
}

/*! Returns the filter that will be used to select contacts to be returned */
QContactFilter QContactFetchRequest::filter() const
{
    Q_D(const QContactFetchRequest);
    return d->m_filter;
}

/*! Returns the sort ordering that will be used sort the results of this request */
QList<QContactSortOrder> QContactFetchRequest::sorting() const
{
    Q_D(const QContactFetchRequest);
    return d->m_sorting;
}

/*! Returns the list of definition names which define which details contacts in the result list will be limited to */
QStringList QContactFetchRequest::definitionRestrictions() const
{
    Q_D(const QContactFetchRequest);
    return d->m_definitionRestrictions;
}

/*! Returns the list of contacts retrieved by this request */
QList<QContact> QContactFetchRequest::contacts() const
{
    Q_D(const QContactFetchRequest);
    return d->m_contacts;
}

#include "moc_qcontactfetchrequest.cpp"

QTM_END_NAMESPACE
