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

#include "qcontactremoverequest.h"
#include "qcontactrequests_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QContactRemoveRequest
  \brief The QContactRemoveRequest class allows a client to asynchronously
    request that certain contacts be removed from a contacts store.

  For a QContactRemoveRequest, the resultsUpdated() signal will be emitted when
  the individual item errors (which may be retrieved by calling errorMap()) are updated, or if the overall
  operation error (which may be retrieved by calling error()) is updated.

  \ingroup contacts-requests
 */

/*! Constructs a new contact remove request whose parent is the specified \a parent */
QContactRemoveRequest::QContactRemoveRequest(QObject* parent)
    : QContactAbstractRequest(new QContactRemoveRequestPrivate, parent)
{
}

/*!
  Sets the id of the contact which will be removed to \a contactId.
  Equivalent to calling:
  \code
      setContactIds(QList<QContactLocalId>() << contactIds);
  \endcode
 */
void QContactRemoveRequest::setContactId(const QContactLocalId& contactId)
{
    Q_D(QContactRemoveRequest);
    d->m_contactIds.clear();
    d->m_contactIds.append(contactId);
}

/*! Sets the list of ids of contacts which will be removed to \a contactIds */
void QContactRemoveRequest::setContactIds(const QList<QContactLocalId>& contactIds)
{
    Q_D(QContactRemoveRequest);
    d->m_contactIds = contactIds;
}

/*! Returns the list of ids of contacts which will be removed */
QList<QContactLocalId> QContactRemoveRequest::contactIds() const
{
    Q_D(const QContactRemoveRequest);
    return d->m_contactIds;
}

/*! Returns the map of input contact list indices to errors which occurred */
QMap<int, QContactManager::Error> QContactRemoveRequest::errorMap() const
{
    Q_D(const QContactRemoveRequest);
    return d->m_errors;
}

#include "moc_qcontactremoverequest.cpp"

QTM_END_NAMESPACE
