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

#include "qcontactlocalidfetchrequest.h"
#include "qcontactrequests_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QContactLocalIdFetchRequest
  \brief The QContactLocalIdFetchRequest class allows a client to asynchronously
    request a list of contact ids from a contacts store manager.


  For a QContactLocalIdFetchRequest, the resultsAvailable() signal will be emitted when the resultant
  manager-local contact ids (which may be retrieved by calling ids()), are updated, as well as if
  the overall operation error (which may be retrieved by calling error()) is updated.

  \ingroup contacts-requests
 */

/*!
  \fn QContactLocalIdFetchRequest::progress(QContactLocalIdFetchRequest* self, bool appendOnly)
  \internal
  This signal is emitted when some progress has been made on the request, causing either a change of
  status or an update of results, or both.  It identifies which request the signal originated from
  by including a pointer to \a self, and contains an \a appendOnly flag which signifies whether or not the total
  ordering of the results have been maintained since the last progress signal was emitted.
  This signal is deprecated and will be removed once the transition period has elapsed.
  Use the signals emitted by the base class, combined with \l QObject::sender(), instead.
 */

/*! Constructs a new contact id fetch request */
QContactLocalIdFetchRequest::QContactLocalIdFetchRequest()
    : QContactAbstractRequest(new QContactLocalIdFetchRequestPrivate)
{
}

/*! Cleans up the memory in use by this contact id fetch request */
QContactLocalIdFetchRequest::~QContactLocalIdFetchRequest()
{
}

/*! Sets the filter which will be used to select the contacts whose ids will be returned to \a filter */
void QContactLocalIdFetchRequest::setFilter(const QContactFilter& filter)
{
    Q_D(QContactLocalIdFetchRequest);
    d->m_filter = filter;
}

/*! Sets the future sort ordering of the result of the request to \a sorting.  This function only has
    effect on the result if called prior to calling \c start() */
void QContactLocalIdFetchRequest::setSorting(const QList<QContactSortOrder>& sorting)
{
    Q_D(QContactLocalIdFetchRequest);
    d->m_sorting = sorting;
}

/*! Returns the filter which will be used to select the contacts whose ids will be returned */
QContactFilter QContactLocalIdFetchRequest::filter() const
{
    Q_D(const QContactLocalIdFetchRequest);
    return d->m_filter;
}

/*! Returns the sort ordering which will be used to sort the result */
QList<QContactSortOrder> QContactLocalIdFetchRequest::sorting() const
{
    Q_D(const QContactLocalIdFetchRequest);
    return d->m_sorting;
}

/*! Returns the list of ids of contacts which matched the request */
QList<QContactLocalId> QContactLocalIdFetchRequest::ids() const
{
    Q_D(const QContactLocalIdFetchRequest);
    return d->m_ids;
}

#include "moc_qcontactlocalidfetchrequest.cpp"

QTM_END_NAMESPACE
