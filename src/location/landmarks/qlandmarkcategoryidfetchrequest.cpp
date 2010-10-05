/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qlandmarkcategoryidfetchrequest.h"
#include "qlandmarkrequests_p.h"
#include "qlandmarkcategoryid.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkCategoryIdFetchRequest
    \brief The QLandmarkCategoryIdFetchRequest class allows a client to asynchronously
    request a list of category identifiers from a landmark manager.

    For a QLandmarkCategoryIdFetchRequest, the resultsAvailable() signal will be emitted
    as resultant category identifiers are found (these are retrievable via the callings ids()),
     as well as if an overall operation error occurred(which may be retrieved by calling
     QLandmarkAbstractRequest::error()).


    \inmodule QtLocation

    \ingroup landmarks-request
*/

/*!
    Creates a new category identifier fetch request object with the given \a manager
    aand \a parent.
*/
QLandmarkCategoryIdFetchRequest::QLandmarkCategoryIdFetchRequest(QLandmarkManager * manager, QObject *parent)
    : QLandmarkAbstractRequest(new QLandmarkCategoryIdFetchRequestPrivate(manager), parent)
{
}

/*!
    Destroys the request object.
*/
QLandmarkCategoryIdFetchRequest::~QLandmarkCategoryIdFetchRequest()
{
}

/*!
    Returns the list of category identifiers that have been found during the
    request.
*/
QList<QLandmarkCategoryId> QLandmarkCategoryIdFetchRequest::categoryIds() const
{
    Q_D(const QLandmarkCategoryIdFetchRequest);
    QMutexLocker ml(&d->mutex);
    return d->categoryIds;
}

/*!
    Returns the sorting of the categories.
    By default the sorting is case insensitive and in ascending order
    according to the category name.
*/
QLandmarkNameSort QLandmarkCategoryIdFetchRequest::sorting() const
{
    Q_D(const QLandmarkCategoryIdFetchRequest);
    QMutexLocker ml(&d->mutex);
    return d->sorting;
}

/*!
    Sets \a nameSort to specify the sorting of the returned category ids.
*/
void QLandmarkCategoryIdFetchRequest::setSorting(const QLandmarkNameSort &nameSort)
{
    Q_D(QLandmarkCategoryIdFetchRequest);
    QMutexLocker ml(&d->mutex);
    d->sorting = nameSort;
}

/*!
    Returns the maximum number of category ids to be returned.
*/
int QLandmarkCategoryIdFetchRequest::limit() const
{
    Q_D(const QLandmarkCategoryIdFetchRequest);
    QMutexLocker ml(&d->mutex);
    return d->limit;
}

/*!
    Sets the maximum number of category ids to be returned to \a limit.
 */
void QLandmarkCategoryIdFetchRequest::setLimit(int limit)
{
    Q_D(QLandmarkCategoryIdFetchRequest);
    QMutexLocker ml(&d->mutex);
    d->limit = limit;
}

/*!
    Returns the index offset for the request.
*/
int QLandmarkCategoryIdFetchRequest::offset() const
{
    Q_D(const QLandmarkCategoryIdFetchRequest);
    QMutexLocker ml(&d->mutex);
    return d->offset;
}

/*!
    Sets the index \a offset for the request.
*/
void QLandmarkCategoryIdFetchRequest::setOffset(int offset) {
    Q_D(QLandmarkCategoryIdFetchRequest);
    QMutexLocker ml(&d->mutex);
    d->offset = offset;
}

#include "moc_qlandmarkcategoryidfetchrequest.cpp"

QTM_END_NAMESPACE











