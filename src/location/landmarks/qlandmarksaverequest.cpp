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

#include "qlandmarksaverequest.h"
#include "qlandmarkrequests_p.h"
#include "qlandmark.h"


QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkSaveRequest
    \brief The QLandmarkSaveRequest class allows a client to asynchronously
    request that certain landmarks be saved by a landmark manager.

    \inmodule QtLocation

    \ingroup landmarks-request

    For a QLandmarkSaveRequest, the resultsAvailable() signal will be emitted
    when either an individual items error out (individaul errors may be retrieved
    by calling errorMap()), an  overall operation error occurs(which may be
    retrieved by calling error()), or when individual items have been
    saved (which may be retrieved by calling landmarks()).
*/

/*!
    Constructs a landmark save request with the given \a manager and \a parent.
*/
QLandmarkSaveRequest::QLandmarkSaveRequest(QLandmarkManager *manager, QObject *parent)
    : QLandmarkAbstractRequest(new QLandmarkSaveRequestPrivate(manager), parent)
{
}

/*!
    Destroys the request object.
*/
QLandmarkSaveRequest::~QLandmarkSaveRequest()
{
}

/*!
    Returns the list of landmarks which will be saved if called prior to
    callling \l QLandmarkAbstractRequest::start(), otherwise returns the list of landmarks as they
    were saved into the landmark manager.
*/
QList<QLandmark> QLandmarkSaveRequest::landmarks() const
{
    Q_D(const QLandmarkSaveRequest);
    QMutexLocker ml(&d->mutex);
    return d->landmarks;
}

/*!
    Sets the list of \a landmarks to be saved.

    \sa setLandmark()
*/
void QLandmarkSaveRequest::setLandmarks(const QList<QLandmark> &landmarks)
{
    Q_D(QLandmarkSaveRequest);
    QMutexLocker ml(&d->mutex);
    d->landmarks = landmarks;
}

/*!
    Convenience function to set a single \a landmark to be saved.

    \sa setLandmarks()
*/
void QLandmarkSaveRequest::setLandmark(const QLandmark &landmark)
{
    Q_D(QLandmarkSaveRequest);
    QMutexLocker ml(&d->mutex);
    d->landmarks.clear();
    d->landmarks.append(landmark);
}

/*!
    Returns the mapping of input landmark list indices to errors which occurred.
*/
QMap<int, QLandmarkManager::Error> QLandmarkSaveRequest::errorMap() const
{
    Q_D(const QLandmarkSaveRequest);
    QMutexLocker ml(&d->mutex);
    return d->errorMap;
}

#include "moc_qlandmarksaverequest.cpp"

QTM_END_NAMESPACE


