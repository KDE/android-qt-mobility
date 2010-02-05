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

#include "qcontactrelationshipfetchrequest.h"
#include "qcontactrelationship.h"
#include "qcontactrelationshipfilter.h"
#include "qcontactrequests_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QContactRelationshipFetchRequest

  \brief The QContactRelationshipFetchRequest class allows a client to
  asynchronously request relationships from a contacts store manager.

  For a QContactRelationshipFetchRequest, the resultsAvailable() signal will be emitted when the resultant
  relationships (which may be retrieved by calling relationships()), are updated, as well as if
  the overall operation error (which may be retrieved by calling error()) is updated.

  \ingroup contacts-requests
 */

/*!

  \fn QContactRelationshipFetchRequest::progress(QContactRelationshipFetchRequest* self, bool appendOnly)
  \internal

  This signal is emitted when some progress has been made on the
  request, causing either a change of status or an update of results,
  or both.  It identifies which request the signal originated from by
  including a pointer to \a self, and contains an \a appendOnly flag
  which signifies whether or not the total ordering of the results
  have been maintained since the last progress signal was emitted.
  This signal is deprecated and will be removed once the transition period has elapsed.
  Use the signals emitted by the base class, combined with \l QObject::sender(), instead.
 */

/*! Constructs a new relationship fetch request
 */
QContactRelationshipFetchRequest::QContactRelationshipFetchRequest()
    : QContactAbstractRequest(new QContactRelationshipFetchRequestPrivate)
{
}

/*! Cleans up the memory in use by this relationship fetch request
 */
QContactRelationshipFetchRequest::~QContactRelationshipFetchRequest()
{
}

/*! Sets the source contact criterion of the fetch request to \a firstId.
    If \a firstId is the default-constructed id, or the first contact id is not set,
    the request will fetch relationships involving any first contact.
*/
void QContactRelationshipFetchRequest::setFirst(const QContactId& firstId)
{
    Q_D(QContactRelationshipFetchRequest);
    d->m_first = firstId;
}

/*! Returns the source contact criterion of the fetch request
 */
QContactId QContactRelationshipFetchRequest::first() const
{
    Q_D(const QContactRelationshipFetchRequest);
    return d->m_first;
}

/*! Sets the relationship type criterion of the fetch request to \a relationshipType.
    If \a relationshipType is empty, or the relationship type is not set,
    the request will fetch relationships of any type.
*/
void QContactRelationshipFetchRequest::setRelationshipType(const QString& relationshipType)
{
    Q_D(QContactRelationshipFetchRequest);
    d->m_relationshipType = relationshipType;
}

/*! Returns the relationship type criterion of the fetch request
 */
QString QContactRelationshipFetchRequest::relationshipType() const
{
    Q_D(const QContactRelationshipFetchRequest);
    return d->m_relationshipType;
}

/*! Sets the destination contact criterion of the fetch request to \a secondId.
    If \a secondId is the default-constructed id, or the second contact id is not set,
    the request will fetch relationships involving any second contact.
*/
void QContactRelationshipFetchRequest::setSecond(const QContactId& secondId)
{
    Q_D(QContactRelationshipFetchRequest);
    d->m_second = secondId;
}

/*! Returns the destination contact criterion of the fetch request
 */
QContactId QContactRelationshipFetchRequest::second() const
{
    Q_D(const QContactRelationshipFetchRequest);
    return d->m_second;
}

/*!
  \internal
  Sets the participant criterion of the fetch request to \a
  participantUri.  If the \a participantUri references a contact in
  the manager from which the relationships are being fetched and the
  \a role is \c QContactRelationshipFilter::Either, a relationship
  will match the criterion if the contact appears in the relationship
  as either the source or a destination contact.  If the \a
  participantUri references a contact in a different manager to the
  one from which the relationships are being fetched and the \a role
  is \c QContactRelationshipFilter::Either, a relationship will match
  the criterion only if the contact appears in the relationship as a
  destination contact.  If the \a participantUri references a contact
  in a different manager to the one from which the relationships are
  being fetched and the \a role is \c
  QContactRelationshipFilter::Source, no relationships will be
  fetched.
  
  If the \a participantUri consists of an empty manager URI and the
  zero contact id, or if the participant criterion is not set, the
  request will fetch relationships involving any participant.
 */
void QContactRelationshipFetchRequest::setParticipant(const QContactId& participantUri, QContactRelationshipFilter::Role role)
{
    Q_D(QContactRelationshipFetchRequest);
    d->m_participantUri = participantUri;
    d->m_role = role;
}

/*!
  \internal
  Returns the participant criterion of the fetch request
 */
QContactId QContactRelationshipFetchRequest::participant() const
{
    Q_D(const QContactRelationshipFetchRequest);
    return d->m_participantUri;
}

/*!
  \internal
  Returns the role of the participant criterion of the fetch request
 */
QContactRelationshipFilter::Role QContactRelationshipFetchRequest::participantRole() const
{
    Q_D(const QContactRelationshipFetchRequest);
    return d->m_role;
}

/*! Returns the list of relationships that was the result of the request
 */
QList<QContactRelationship> QContactRelationshipFetchRequest::relationships() const
{
    Q_D(const QContactRelationshipFetchRequest);
    return d->m_relationships;
}

#include "moc_qcontactrelationshipfetchrequest.cpp"

QTM_END_NAMESPACE
