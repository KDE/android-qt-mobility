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

#include "qcontactrelationshipremoverequest.h"
#include "qcontactrequests_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QContactRelationshipRemoveRequest

  \brief The QContactRelationshipRemoveRequest class allows a client
  to asynchronously request that certain relationships be removed from
  a contacts store.

  For a QContactRelationshipRemoveRequest, the resultsUpdated() signal will be emitted when
  the individual item errors (which may be retrieved by calling errorMap()) are updated, or if the overall
  operation error (which may be retrieved by calling error()) is updated.

  \ingroup contacts-requests
 */

/*! Constructs a new relationship remove request */
QContactRelationshipRemoveRequest::QContactRelationshipRemoveRequest()
    : QContactAbstractRequest(new QContactRelationshipRemoveRequestPrivate)
{
}

/*! Cleans up the memory in use by this relationship remove request */
QContactRelationshipRemoveRequest::~QContactRelationshipRemoveRequest()
{
}

/*!
  \deprecated
   Sets the first contact criterion of the remove request to \a firstId.
   If \a firstId is the default-constructed id, or the first contact is not set,
   the request will remove relationships involving any first contact.

   This function is obsolete; set the list of relationships to remove by calling setRelationships() instead.
 */
void QContactRelationshipRemoveRequest::setFirst(const QContactId& firstId)
{
    Q_D(QContactRelationshipRemoveRequest);
    d->m_first = firstId;
}

/*!
  \deprecated
  Returns the first contact criterion of the remove request.
  This function is obsolete; retrieve the lists of relationships that will be removed by calling relationships() instead.
 */
QContactId QContactRelationshipRemoveRequest::first() const
{
    Q_D(const QContactRelationshipRemoveRequest);
    return d->m_first;
}

/*!
  \deprecated
   Sets the relationship type criterion of the remove request to \a relationshipType.
   If \a relationshipType is empty, or the relationship type is not set,
   the request will remove relationships of any type.

   This function is obsolete; set the list of relationships to remove by calling setRelationships() instead.
 */
void QContactRelationshipRemoveRequest::setRelationshipType(const QString& relationshipType)
{
    Q_D(QContactRelationshipRemoveRequest);
    d->m_relationshipType = relationshipType;
}

/*!
  \deprecated
  Returns the relationship type criterion of the fetch request.
  This function is obsolete; retrieve the lists of relationships that will be removed by calling relationships() instead.
 */
QString QContactRelationshipRemoveRequest::relationshipType() const
{
    Q_D(const QContactRelationshipRemoveRequest);
    return d->m_relationshipType;
}

/*!
  \deprecated
  Sets the second contact criterion of the remove request to \a secondId.
  If \a secondId is the default-constructed id, or the second contact is not set,
  the request will remove relationships involving any second contact.

   This function is obsolete; set the list of relationships to remove by calling setRelationships() instead.
 */
void QContactRelationshipRemoveRequest::setSecond(const QContactId& secondId)
{
    Q_D(QContactRelationshipRemoveRequest);
    d->m_second = secondId;
}

/*!
  \deprecated
  Returns the second contact criterion of the remove request.
  This function is obsolete; retrieve the lists of relationships that will be removed by calling relationships() instead.
 */
QContactId QContactRelationshipRemoveRequest::second() const
{
    Q_D(const QContactRelationshipRemoveRequest);
    return d->m_second;
}

/*! Sets the list of relationships which will be removed to \a relationships */
void QContactRelationshipRemoveRequest::setRelationships(const QList<QContactRelationship>& relationships)
{
    Q_D(QContactRelationshipRemoveRequest);
    d->m_relationships = relationships;
}

/*! Returns the list of relationships which will be removed */
QList<QContactRelationship> QContactRelationshipRemoveRequest::relationships() const
{
    Q_D(const QContactRelationshipRemoveRequest);
    return d->m_relationships;
}

/*! Returns the map of input contact list indices to errors which occurred */
QMap<int, QContactManager::Error> QContactRelationshipRemoveRequest::errorMap() const
{
    Q_D(const QContactRelationshipRemoveRequest);
    return d->m_errors;
}

#include "moc_qcontactrelationshipremoverequest.cpp"

QTM_END_NAMESPACE
