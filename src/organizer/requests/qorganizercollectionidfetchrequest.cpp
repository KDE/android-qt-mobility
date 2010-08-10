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

#include "qorganizercollectionidfetchrequest.h"
#include "qorganizeritemrequests_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerCollectionIdFetchRequest
  \brief The QOrganizerCollectionIdFetchRequest class allows a client to asynchronously
    request collection ids from an organizer manager.


  For a QOrganizerCollectionIdFetchRequest, the resultsAvailable() signal will be emitted when the resultant
  collection ids (which may be retrieved by calling collectionIds()), are updated, as well as if
  the overall operation error (which may be retrieved by calling error()) is updated.

  \ingroup organizeritems-requests
 */

/*! Constructs a new organizeritem fetch request whose parent is the specified \a parent */
QOrganizerCollectionIdFetchRequest::QOrganizerCollectionIdFetchRequest(QObject* parent)
    : QOrganizerItemAbstractRequest(new QOrganizerCollectionIdFetchRequestPrivate, parent)
{
}

/*! Returns the list of collection ids retrieved by this request */
QList<QOrganizerCollectionId> QOrganizerCollectionIdFetchRequest::collectionIds() const
{
    Q_D(const QOrganizerCollectionIdFetchRequest);
    return d->m_collectionIds;
}

#include "moc_qorganizercollectionidfetchrequest.cpp"

QTM_END_NAMESPACE
