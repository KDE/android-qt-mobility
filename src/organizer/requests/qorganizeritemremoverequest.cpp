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

#include "qorganizeritemremoverequest.h"
#include "qorganizeritemrequests_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerItemRemoveRequest
  \brief The QOrganizerItemRemoveRequest class allows a client to asynchronously
    request that certain organizer items be removed from a organizer items store.
  \inmodule QtOrganizer

  For a QOrganizerItemRemoveRequest, the resultsUpdated() signal will be emitted when
  the individual item errors (which may be retrieved by calling errorMap()) are updated, or if the overall
  operation error (which may be retrieved by calling error()) is updated.

  \ingroup organizer-requests
 */

/*! Constructs a new organizer item remove request whose parent is the specified \a parent */
QOrganizerItemRemoveRequest::QOrganizerItemRemoveRequest(QObject* parent)
    : QOrganizerAbstractRequest(new QOrganizerItemRemoveRequestPrivate, parent)
{
}

/*! Frees memory in use by this request */
QOrganizerItemRemoveRequest::~QOrganizerItemRemoveRequest()
{
    QOrganizerAbstractRequestPrivate::notifyEngine(this);
}

/*!
  Sets the id of the organizer item which will be removed to \a organizeritemId.
  Equivalent to calling:
  \code
      setOrganizerItemIds(QList<QOrganizerItemId>() << organizeritemIds);
  \endcode
 */
void QOrganizerItemRemoveRequest::setItemId(const QOrganizerItemId& organizeritemId)
{
    Q_D(QOrganizerItemRemoveRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_organizeritemIds.clear();
    d->m_organizeritemIds.append(organizeritemId);
}

/*! Sets the list of ids of organizer items which will be removed to \a organizeritemIds */
void QOrganizerItemRemoveRequest::setItemIds(const QList<QOrganizerItemId>& organizeritemIds)
{
    Q_D(QOrganizerItemRemoveRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_organizeritemIds = organizeritemIds;
}

/*! Returns the list of ids of organizer items which will be removed */
QList<QOrganizerItemId> QOrganizerItemRemoveRequest::itemIds() const
{
    Q_D(const QOrganizerItemRemoveRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_organizeritemIds;
}

/*! Returns the map of input organizer item list indices to errors which occurred */
QMap<int, QOrganizerManager::Error> QOrganizerItemRemoveRequest::errorMap() const
{
    Q_D(const QOrganizerItemRemoveRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_errors;
}

#include "moc_qorganizeritemremoverequest.cpp"

QTM_END_NAMESPACE
