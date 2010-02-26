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

#include "qlandmarkremoverequest.h"


QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkRemoveRequest
    \brief The QLandmarkRemoveRequest class allows a client to asynchronously
    request that certain landmarks be removed from a landmark manager.

    For a QLandmarkRemoveRequest, the resultsAvailable() signal will be emitted
    when either an individual items error out (individaul errors may be retrieved
    by calling errorMap()), or when an overall operation error occurs(which may be
    retrieved by calling error()).

    \ingroup location
*/

/*!
    Constructs a landmark save request
*/
QLandmarkRemoveRequest::QLandmarkRemoveRequest()
{
}

/*!
    Destroys the request object.
*/
QLandmarkRemoveRequest::~QLandmarkRemoveRequest()
{
}

/*!
    Returns the list of identifiers of landmarks which will be removed.
*/
QList<QLandmarkId> QLandmarkRemoveRequest::landmarkIds() const
{
}

/*!
    Sets the list of \a landmarkIds of landmarks which will be removed.
*/
void QLandmarkRemoveRequest::setLandmarkIds(const QList<QLandmarkId> &landmarkIds)
{
}

/*!
    Returns the mapping of input landmark id list indices
    to the errors which occurred.
*/
QMap<int, QLandmarkManager::Error> QLandmarkRemoveRequest::errorMap() const
{
}

#include "moc_qlandmarkremoverequest.cpp"

QTM_END_NAMESPACE


