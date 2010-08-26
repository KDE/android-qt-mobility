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
#include "qlandmarkabstractrequest.h"
#include "qlandmarkabstractrequest_p.h"
#include "qlandmarkmanagerengine.h"
#include "qlandmarkmanager_p.h"
#include <QDebug>
#include <QMutexLocker>

QTM_USE_NAMESPACE

QLandmarkAbstractRequestPrivate::QLandmarkAbstractRequestPrivate(QLandmarkManager *mgr)
    : type(QLandmarkAbstractRequest::InvalidRequest),
      state(QLandmarkAbstractRequest::InactiveState),
      error(QLandmarkManager::NoError),
      errorString(QString()),
      manager(mgr)
{
}

/*!
    \class QLandmarkAbstractRequest
    \brief The QLandmarkAbstractRequest class provides the interface
    from which all asynchronous request classes inherit.


    \inmodule QtLocation

    \ingroup landmarks-request

    It allows a client to asynchronously request some functionality
    from a QLandmarkManager.
*/

/*!
    \enum QLandmarkAbstractRequest::RequestType
    Defines the possible types of asynchronous requests.
    \value  InvalidRequest An invalid request
    \value  LandmarkIdFetchRequest A request to fetch a list of landmark
            identifiers.
    \value  CategoryIdFetchRequest A request to fetch a list of catgory
            identifiers.

    \value  LandmarkFetchRequest A request to fetch a list of landmarks
    \value  LandmarkFetchByIdRequest A request to fetch a list of landmarks by id.
    \value  CategoryFetchRequest A request to fetch a list of categories
    \value  CategoryFetchByIdRequest A request to fetch a list of categories by id
    \value  LandmarkSaveRequest A request to save a list of landmarks.
    \value  LandmarkRemoveRequest A request to remove a list of landmarks.
    \value  CategorySaveRequest A request to save a list of categories.
    \value  CategoryRemoveRequest A request to remove a list of categories.
    \value  ImportRequest A request import landmarks.
    \value  ExportRequest A request export landmarks.
*/
QLandmarkAbstractRequestPrivate::~QLandmarkAbstractRequestPrivate()
{
}

void QLandmarkAbstractRequestPrivate::notifyEngine(QLandmarkAbstractRequest* request)
{
        Q_ASSERT(request);
        QLandmarkAbstractRequestPrivate* d = request->d_ptr;
        if (d) {
            QMutexLocker ml(&d->mutex);
            QLandmarkManagerEngine *engine = QLandmarkManagerPrivate::getEngine(d->manager);
            ml.unlock();
            if (engine) {
                engine->requestDestroyed(request);
            }
        }
}

/*!
    \enum QLandmarkAbstractRequest::State
    Defines the possible states of asynchronous requests.
    \value  InactiveState Operation not yet started.
    \value  ActiveState Operation started, not yet finished.
    \value  FinishedState Operation completed.  (Can be mean either successful or
            unsuccessful completion).
*/

/*!
    Constructs a new, invalid asynchronous request with the given \a manager and \a parent.
*/
QLandmarkAbstractRequest::QLandmarkAbstractRequest(QLandmarkManager *manager, QObject *parent)
    : QObject(parent),
      d_ptr(new QLandmarkAbstractRequestPrivate(manager))
{
}

/*!
    \internal
*/
QLandmarkAbstractRequest::QLandmarkAbstractRequest(QLandmarkAbstractRequestPrivate *dd, QObject *parent)
    : QObject(parent),
     d_ptr(dd)
{
}

/*!
    Destroys the asynchronous request
*/
QLandmarkAbstractRequest::~QLandmarkAbstractRequest()
{
    QLandmarkAbstractRequestPrivate::notifyEngine(this);
    delete d_ptr;
}

/*!
    Returns the type of this asynchronous request.
*/
QLandmarkAbstractRequest::RequestType QLandmarkAbstractRequest::type() const
{
    return d_ptr->type;
}

/*!
    Returns the state of the request
*/
QLandmarkAbstractRequest::State QLandmarkAbstractRequest::state()
{
    return d_ptr->state;
}

/*!
    Returns true if the request is in the \c QLandmarkAbstractRequest::Inactive state;
    otherwise, returns false.
    \sa state()
*/
bool QLandmarkAbstractRequest::isInactive() const
{
    QMutexLocker ml(&d_ptr->mutex);
    return d_ptr->state == QLandmarkAbstractRequest::InactiveState;
}

/*!
    Returns true if the request is in the \c QLandmarkAbstractRequest::Active state;
    otherwise, returns false.
    \sa state()
*/
bool QLandmarkAbstractRequest::isActive() const
{
    QMutexLocker ml(&d_ptr->mutex);
    return d_ptr->state == QLandmarkAbstractRequest::ActiveState;
}

/*!
    Returns true if the request is in the \c QLandmarkAbstractRequest::Finished state;
    otherwise, returns false.
    \sa state()
*/
bool QLandmarkAbstractRequest::isFinished() const
{
    QMutexLocker ml(&d_ptr->mutex);
    return d_ptr->state == QLandmarkAbstractRequest::FinishedState;
}

/*!
    Returns the error of the most recent asynchronous operation.
    \sa errorString()
*/
QLandmarkManager::Error QLandmarkAbstractRequest::error() const
{
    QMutexLocker ml(&d_ptr->mutex);
    return d_ptr->error;
}

/*!
    Returns a human readable string of the last error
    that occurred.
    \sa error()
*/
QString QLandmarkAbstractRequest::errorString() const
{
    QMutexLocker ml(&d_ptr->mutex);
    return d_ptr->errorString;
}

/*!
    Returns a pointer to the landmark manager which
    this request operates on.
*/
QLandmarkManager *QLandmarkAbstractRequest::manager() const
{
    QMutexLocker ml(&d_ptr->mutex);
    return d_ptr->manager;
}

/*!
    Sets the \a manager which this request operates on.

    Note that if a NULL manager is set, the functions
    start(), cancel() and waitForFinished() will return false and
    error will be set to QLandmarkManager::InvalidManagerError.
*/
void QLandmarkAbstractRequest::setManager(QLandmarkManager *manager)
{
    QMutexLocker ml(&d_ptr->mutex);
    if (d_ptr->state == QLandmarkAbstractRequest::ActiveState && d_ptr->manager)
        return;
    d_ptr->manager = manager;
}

/*!
    Attempts to start the request.

    Returns true if the request was started, otherwise false.
    \sa cancel()
*/
bool QLandmarkAbstractRequest::start()
{
    QMutexLocker ml(&d_ptr->mutex);
    if (!d_ptr->manager) {
        d_ptr->error = QLandmarkManager::BadArgumentError;
        d_ptr->errorString = "No manager assigned to landmark request object";
        qWarning() << d_ptr->errorString;
        return false;
    }
    QLandmarkManagerEngine *engine = d_ptr->manager->engine();

    if (d_ptr->state != QLandmarkAbstractRequest::ActiveState) {
        ml.unlock();
        return engine->startRequest(this);
    }
     else {
        return false;
     }
}

/*!
    Attempts to cancel the request.

    Returns true if the request was canceled, otherwise false.
    \sa start()
*/
bool QLandmarkAbstractRequest::cancel()
{
    QMutexLocker ml(&d_ptr->mutex);
    if (!d_ptr->manager) {
        d_ptr->error = QLandmarkManager::BadArgumentError;
        d_ptr->errorString = "No manager assigned to landmark request object";
        qWarning() << d_ptr->errorString;
        return false;
    }
    QLandmarkManagerEngine *engine = d_ptr->manager->engine();

    if(d_ptr->state == QLandmarkAbstractRequest::ActiveState) {
        ml.unlock();
        return engine->cancelRequest(this);
    }
    else
        return false;
}

/*!
    Blocks until the request has been completed or until \a msecs milliseconds
    has elapsed.  If \a msecs is zero, this function will block indefinitely.

    Returns true if the request was canceled or completed
    within the given period, otherwise returns false.
*/
bool QLandmarkAbstractRequest::waitForFinished(int msecs)
{

    QMutexLocker ml(&d_ptr->mutex);
    if (!d_ptr->manager) {
        d_ptr->error = QLandmarkManager::BadArgumentError;
        d_ptr->errorString = "No manager assigned to landmark request object";
        qWarning() << d_ptr->errorString;
        return false;
    }
    QLandmarkManagerEngine *engine = d_ptr->manager->engine();

    switch(d_ptr->state) {
        case QLandmarkAbstractRequest::ActiveState:
            ml.unlock();
            return engine->waitForRequestFinished(this, msecs);
        case QLandmarkAbstractRequest::FinishedState:
            return true;
        default:
            return false;
    }
    return false;
}

/*!
  \fn void QLandmarkAbstractRequest::resultsAvailable()
  This signal is emitted when new results are available.  Results
  can include the operation error which may be accessed via error(),
  or derived-class specific results which are accessible through
  the derived class API.

  \sa error()
*/

/*!
    \fn void QLandmarkAbstractRequest::stateChanged(QLandmarkAbstractRequest::State newState)
    This signal is emitted when the state of the request is changed.  The new state of
    the request will be contained in \a newState.
*/

#include "moc_qlandmarkabstractrequest.cpp"





