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

#include "qorganizercollectionfetchrequest.h"
#include "qorganizeritemrequests_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerCollectionFetchRequest
  \brief The QOrganizerCollectionFetchRequest class allows a client to asynchronously
    request collections from an organizer manager.

  \inmodule QtOrganizer


  For a QOrganizerCollectionFetchRequest, the resultsAvailable() signal will be emitted when the resultant
  collections (which may be retrieved by calling collections()), are updated, as well as if
  the overall operation error (which may be retrieved by calling error()) is updated.

  \ingroup organizeritems-requests
 */

/*! Constructs a new organizeritem fetch request whose parent is the specified \a parent */
QOrganizerCollectionFetchRequest::QOrganizerCollectionFetchRequest(QObject* parent)
    : QOrganizerAbstractRequest(new QOrganizerCollectionFetchRequestPrivate, parent)
{
}

/*! Frees memory in use by this request */
QOrganizerCollectionFetchRequest::~QOrganizerCollectionFetchRequest()
{
    QOrganizerAbstractRequestPrivate::notifyEngine(this);
}

/*! Returns the collections retrieved by this request */
QList<QOrganizerCollection> QOrganizerCollectionFetchRequest::collections() const
{
    Q_D(const QOrganizerCollectionFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_collections;
}

#include "moc_qorganizercollectionfetchrequest.cpp"

QTM_END_NAMESPACE
