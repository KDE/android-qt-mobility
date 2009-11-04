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

#include <QDebug>
#include <QTimer>

#include "qcontactrequests.h"
#include "qcontactmanagerengine.h"
#include "qcontactrequestworker_p.h"
#include "qcontactabstractrequest_p.h"

/*!
 * \class QContactRequestWorker
 *
 * \brief The QContactRequestWorker class provides a common thread worker queue for QContact asynchronous requests.
 *
 * A QContactRequestWorker consists of a QContactAbstractRequest request queue.
 *
 * An instance of the QContactRequestWorker class is a thread object which is dedicated for processing asynchronous contact requests.
 * All of these requests are instances of derived classes from QContactAbstractRequest. These request instances can be added to, removed/cancelled from the
 * worker thread's internal request queue. Once these requests are processed by the worker thread, the related signals will be emitted
 * and all these requests' waiting clients will also wake up.
 *
 * \sa QContactAbstractRequest
 */

/*! Construct an worker thread. */
QContactRequestWorker::QContactRequestWorker()
    :QThread(), d(new QContactRequestWorkerData)
{
}

/*! Initializes this QContactRequestWorker from \a other */
QContactRequestWorker::QContactRequestWorker(const QContactRequestWorker& other)
    : QThread(), d(other.d)
{
}

/*! Replace the contents of this QContactRequestWorker with \a other */
QContactRequestWorker& QContactRequestWorker::operator=(const QContactRequestWorker& other)
{
    // assign
    d = other.d;
    return *this;
}

/*! Frees the memory used by this QContactRequestWorker, stops the thread and clean up all request elements in the working queue */
QContactRequestWorker::~QContactRequestWorker()
{
    stop();
    quit();
    while (isRunning() && !wait(1)) 
        d->m_newRequestAdded.wakeAll();
}

/*!
 * Stops the worker thread. The worker thread may still run a while until it tries to process next request. 
 * If the worker thread was waiting on an empty queue, wakes it up.
 * \sa run()
 */
void QContactRequestWorker::stop()
{
    QMutexLocker locker(&d->m_mutex);
    d->m_stop = true;
    d->m_newRequestAdded.wakeAll();
}

/*!
 * Worker thread's main working loop. Runs forever until the \l stop() function called.
 * The worker thread initionally waits on the empty request queue, once the request queue is not empty, takes the first element of
 * the queue, then calls related request processing functions based on the request type. Once the request was processed, update the request's
 * status and wakes up any threads which waiting for this request.
 *
 * \sa stop()
 * \sa processContactFetchRequest()
 * \sa processContactLocalIdFetchRequest()
 * \sa processContactSaveRequest()
 * \sa processContactRemoveRequest()
 * \sa processContactRelationshipFetchRequest()
 * \sa processContactRelationshipSaveRequest()
 * \sa processContactRelationshipRemoveRequest()
 * \sa processContactDetailDefinitionFetchRequest()
 * \sa processContactDetailDefinitionSaveRequest()
 * \sa processContactDetailDefinitionRemoveRequest()
 */
void QContactRequestWorker::run()
{
    QContactAbstractRequest *req;
    
    for(;;) {
        req = d->takeFirstRequest();
        if (d->m_stop || req == 0)
            break;
        
        // check to see if it is cancelling; if so, cancel it and perform update.
        if (req->status() == QContactAbstractRequest::Cancelling) {
            QList<QContactManager::Error> dummy;
            QContactManagerEngine::updateRequestStatus(req, QContactManager::NoError, dummy, QContactAbstractRequest::Cancelled);
            continue;
        }

        // Now perform the active request and emit required signals.
        switch (req->type()) {
            case QContactAbstractRequest::ContactFetchRequest:
                processContactFetchRequest(static_cast<QContactFetchRequest*>(req));
                break;
            case QContactAbstractRequest::ContactLocalIdFetchRequest:
                processContactLocalIdFetchRequest(static_cast<QContactLocalIdFetchRequest*>(req));
                break;
            case QContactAbstractRequest::ContactSaveRequest:
                processContactSaveRequest(static_cast<QContactSaveRequest*>(req));
                break;
            case QContactAbstractRequest::ContactRemoveRequest:
                processContactRemoveRequest(static_cast<QContactRemoveRequest*>(req));
                break;
            case QContactAbstractRequest::RelationshipFetchRequest:
                processContactRelationshipFetchRequest(static_cast<QContactRelationshipFetchRequest*>(req));
                break;
            case QContactAbstractRequest::RelationshipSaveRequest:
                processContactRelationshipSaveRequest(static_cast<QContactRelationshipSaveRequest*>(req));
                break;
            case QContactAbstractRequest::RelationshipRemoveRequest:
                processContactRelationshipRemoveRequest(static_cast<QContactRelationshipRemoveRequest*>(req));
                break;
            case QContactAbstractRequest::DetailDefinitionFetchRequest:
                processContactDetailDefinitionFetchRequest(static_cast<QContactDetailDefinitionFetchRequest*>(req));
                break;
            case QContactAbstractRequest::DetailDefinitionSaveRequest:
                processContactDetailDefinitionSaveRequest(static_cast<QContactDetailDefinitionSaveRequest*>(req));
                break;
            case QContactAbstractRequest::DetailDefinitionRemoveRequest:
                processContactDetailDefinitionRemoveRequest(static_cast<QContactDetailDefinitionRemoveRequest*>(req));
                break;
            default:
                break;
        }
        req->d_ptr->m_condition.wakeAll();
    }

    // clean up our requests.
    foreach (QContactAbstractRequest* req, d->m_requestQueue) {
        if (req) {
            removeRequest(req);
        }
    }

    d->m_requestQueue.clear();
}


/*!
 * Returns true if the given \a req successfully added into the working queue, false if not.
 * \sa removeRequest()
 * \sa cancelRequest()
 */
bool QContactRequestWorker::addRequest(QContactAbstractRequest* req)
{
    if (req) {
        QMutexLocker locker(&d->m_mutex);
        
        if (!d->m_requestQueue.contains(req)) {
            d->m_requestQueue.enqueue(req);
            QList<QContactManager::Error> dummy;
            bool ret = QContactManagerEngine::updateRequestStatus(req, QContactManager::NoError, dummy, QContactAbstractRequest::Active);
            d->m_newRequestAdded.wakeAll();
            return ret;
        }
    }
    return false;
}

/*!
 * Returns true if the given \a req successfully removed from the working queue, false if not.
 * \sa addRequest()
 */
bool QContactRequestWorker::removeRequest(QContactAbstractRequest* req)
{
    if (req) {
        QMutexLocker workerLocker(&d->m_mutex);
        d->m_requestQueue.removeOne(req);
        if (req == d->m_currentRequest)
            d->m_currentRequest = 0;
        if (req->d_ptr->m_waiting) {
            req->d_ptr->m_condition.wakeAll();
        }
        return true;
    }
    return false;
}

/*!
 * Update the status of the given request \a req to QContactAbstractRequest::Cancelling, returns true if sucessful, false if not.
 * \sa addRequest()
 */
bool QContactRequestWorker::cancelRequest(QContactAbstractRequest* req)
{
    if (req) {
        QMutexLocker workerLocker(&d->m_mutex);
        if (d->m_requestQueue.isEmpty() || (req == d->m_requestQueue.head() && req == d->m_currentRequest)) {
            return false;
        }
        QList<QContactManager::Error> dummy;
        return QContactManagerEngine::updateRequestStatus(req, QContactManager::NoError, dummy, QContactAbstractRequest::Cancelling);
    }
    return false;
}

/*!
 * Blocks the caller until the given request \a req has been completed by the worker thread or worker thread signals that more partial results
 * are available for the request, or until \a msecs milliseconds has elapsed.
 * If \a msecs is zero, this function will block indefinitely.
 * Returns true if the request was cancelled or completed successfully within the given period, otherwise false. 
 *
 * \sa QContactAbstractRequest::waitForFinished(), QContactAbstractRequest::waitForProgress()
 */
bool QContactRequestWorker::waitRequest(QContactAbstractRequest* req, int msecs)
{
    bool ret = false;
    if (req) {
        QMutexLocker locker(&req->d_ptr->m_mutex);
        QContactAbstractRequest::Status status = req->d_ptr->m_status;
        if (status == QContactAbstractRequest::Active || status == QContactAbstractRequest::Cancelling) {
            req->d_ptr->m_waiting = true;
            if (msecs) {
                ret = req->d_ptr->m_condition.wait(&req->d_ptr->m_mutex, msecs);
            } else {
                ret = req->d_ptr->m_condition.wait(&req->d_ptr->m_mutex);
            }
            req->d_ptr->m_waiting = false;
        } else if (status == QContactAbstractRequest::Finished || status == QContactAbstractRequest::Cancelled) {
            ret = false;
        }
    }
    return ret;
}

/*!
 * Processes the QContactFetchRequest \a req
 * \sa QContactFetchRequest
 */
void QContactRequestWorker::processContactFetchRequest(QContactFetchRequest* req)
{
    if (req->manager()) {   
        QContactFilter filter = req->filter();
        QList<QContactSortOrder> sorting = req->sorting();
        QStringList defs = req->definitionRestrictions();

        QContactManager::Error operationError;
        QList<QContactManager::Error> operationErrors;
        QList<QContact> requestedContacts;

        QList<QContactLocalId> requestedContactIds = req->manager()->contacts(filter, sorting);
        operationError = req->manager()->error();

        QContactManager::Error tempError;
        for (int i = 0; i < requestedContactIds.size(); i++) {
            QContact current = req->manager()->contact(requestedContactIds.at(i));
            tempError = req->manager()->error();
            operationErrors.append(tempError);

            // check for single error; update total operation error if required
            if (tempError != QContactManager::NoError)
                operationError = tempError;

            // apply the required detail definition restrictions
            if (!defs.isEmpty()) {
                QList<QContactDetail> allDetails = current.details();
                for (int j = 0; j < allDetails.size(); j++) {
                    QContactDetail d = allDetails.at(j);
                    if (!defs.contains(d.definitionName())) {
                        // this detail is not required.
                        current.removeDetail(&d);
                    }
                }
            }

            // add the contact to the result list.
            requestedContacts.append(current);
        }

        // update the request with the results.
        QContactManagerEngine::updateRequest(req, requestedContacts, operationError, operationErrors, QContactAbstractRequest::Finished);
    }
}

/*!
 * Processes the QContactLocalIdFetchRequest \a req
 * \sa QContactLocalIdFetchRequest
 */
void QContactRequestWorker::processContactLocalIdFetchRequest(QContactLocalIdFetchRequest* req)
{
    if (req->manager()) {
        QContactFilter filter = req->filter();
        QList<QContactSortOrder> sorting = req->sorting();

        QContactManager::Error operationError = QContactManager::NoError;
        QList<QContactLocalId> requestedContactIds = req->manager()->contacts(filter, sorting);
        operationError = req->manager()->error();

        QContactManagerEngine::updateRequest(req, requestedContactIds, operationError, QList<QContactManager::Error>(), QContactAbstractRequest::Finished);
    }
}

/*!
 * Processes the QContactSaveRequest \a req
 * \sa QContactSaveRequest
 */
void QContactRequestWorker::processContactSaveRequest(QContactSaveRequest* req)
{
    if (req->manager()) {
        QList<QContact> contacts = req->contacts();

        QContactManager::Error operationError = QContactManager::NoError;
        QList<QContactManager::Error> operationErrors = req->manager()->saveContacts(&contacts);
        operationError = req->manager()->error();

        for (int i = 0; i < operationErrors.size(); i++) {
            if (operationErrors.at(i) != QContactManager::NoError) {
                operationError = operationErrors.at(i);
                break;
            }
        }

        QContactManagerEngine::updateRequest(req, contacts, operationError, operationErrors, QContactAbstractRequest::Finished);
    }
}

/*!
 * Processes the QContactRemoveRequest \a req
 * \sa QContactRemoveRequest
 */
void QContactRequestWorker::processContactRemoveRequest(QContactRemoveRequest* req )
{
    if (req->manager()) {
        // this implementation provides scant information to the user
        // the operation either succeeds (all contacts matching the filter were removed)
        // or it fails (one or more contacts matching the filter could not be removed)
        // if a failure occurred, the request error will be set to the most recent
        // error that occurred during the remove operation.
        QContactFilter filter = req->filter();

        QContactManager::Error operationError = QContactManager::NoError;
        QList<QContactLocalId> contactsToRemove = req->manager()->contacts(filter, QList<QContactSortOrder>());
        operationError = req->manager()->error();
    
        for (int i = 0; i < contactsToRemove.size(); i++) {
            QContactManager::Error tempError;
            
            req->manager()->removeContact(contactsToRemove.at(i));
            tempError = req->manager()->error();

            if (tempError != QContactManager::NoError)
                operationError = tempError;
        }

        // there are no results, so just update the status with the error.
        QList<QContactManager::Error> dummy;
        QContactManagerEngine::updateRequestStatus(req, operationError, dummy, QContactAbstractRequest::Finished);
    }
}


/*!
 * Processes the QContactDetailDefinitionFetchRequest \a req
 * \sa QContactDetailDefinitionFetchRequest
 */
void QContactRequestWorker::processContactDetailDefinitionFetchRequest(QContactDetailDefinitionFetchRequest* req)
{
    if (req->manager()) {
        QContactManager::Error operationError = QContactManager::NoError;
        QList<QContactManager::Error> operationErrors;
        QMap<QString, QContactDetailDefinition> requestedDefinitions;
        QStringList names = req->names();
        if (names.isEmpty()) {
            names = req->manager()->detailDefinitions().keys(); // all definitions.
            operationError = req->manager()->error();
        }

        QContactManager::Error tempError;
        for (int i = 0; i < names.size(); i++) {
            QContactDetailDefinition current = req->manager()->detailDefinition(names.at(i));
            tempError = req->manager()->error();
            operationErrors.append(tempError);
            requestedDefinitions.insert(names.at(i), current);

            if (tempError != QContactManager::NoError)
                operationError = tempError;
        }

        // update the request with the results.
        QContactManagerEngine::updateRequest(req, requestedDefinitions, operationError, operationErrors, QContactAbstractRequest::Finished);
    }
}
/*!
 * Processes the QContactDetailDefinitionSaveRequest \a req
 * \sa QContactDetailDefinitionSaveRequest
 */
void QContactRequestWorker::processContactDetailDefinitionSaveRequest(QContactDetailDefinitionSaveRequest* req)
{
    if (req->manager()) {
        QContactManager::Error operationError = QContactManager::NoError;
        QList<QContactManager::Error> operationErrors;
        QList<QContactDetailDefinition> definitions = req->definitions();
        QList<QContactDetailDefinition> savedDefinitions;

        QContactManager::Error tempError;
        for (int i = 0; i < definitions.size(); i++) {
            QContactDetailDefinition current = definitions.at(i);
            req->manager()->saveDetailDefinition(current);
            tempError = req->manager()->error();
            savedDefinitions.append(current);
            operationErrors.append(tempError);

            if (tempError != QContactManager::NoError)
                operationError = tempError;
        }

        // update the request with the results.
        QContactManagerEngine::updateRequest(req, savedDefinitions, operationError, operationErrors, QContactAbstractRequest::Finished);
    }
}
/*!
 * Processes the QContactDetailDefinitionRemoveRequest \a req
 * \sa QContactDetailDefinitionRemoveRequest
 */
void QContactRequestWorker::processContactDetailDefinitionRemoveRequest(QContactDetailDefinitionRemoveRequest* req)
{
    if (req->manager()) {
        QStringList names = req->names();

        QContactManager::Error operationError = QContactManager::NoError;
        QList<QContactManager::Error> operationErrors;
        
        for (int i = 0; i < names.size(); i++) {
            QContactManager::Error tempError;
            req->manager()->removeDetailDefinition(names.at(i));
            tempError = req->manager()->error();
            operationErrors.append(tempError);

            if (tempError != QContactManager::NoError)
                operationError = tempError;
        }

        // there are no results, so just update the status with the error.
        QContactManagerEngine::updateRequestStatus(req, operationError, operationErrors, QContactAbstractRequest::Finished);
    }
}

/*!
 * Processes the QContactRelationshipFetchRequest \a req
 * \sa QContactRelationshipFetchRequest
 */
void QContactRequestWorker::processContactRelationshipFetchRequest(QContactRelationshipFetchRequest* req)
{
    if (req->manager()) {
        QList<QContactManager::Error> operationErrors;
        QList<QContactRelationship> allRelationships = req->manager()->relationships(QString(), QContactId(), QContactRelationshipFilter::Either);
        QContactManager::Error operationError = req->manager()->error();
        QList<QContactRelationship> requestedRelationships;

        // first criteria: source contact id must be empty or must match
        if (req->first() == QContactId()) {
            // all relationships match this criteria (zero id denotes "any")
            requestedRelationships = allRelationships;
        } else {
            for (int i = 0; i < allRelationships.size(); i++) {
                QContactRelationship currRelationship = allRelationships.at(i);
                if (req->first() == currRelationship.first()) {
                    requestedRelationships.append(currRelationship);
                }
            }
        }

        // second criteria: relationship type must be empty or must match
        if (!req->relationshipType().isEmpty()) {
            allRelationships = requestedRelationships;
            requestedRelationships.clear();
            for (int i = 0; i < allRelationships.size(); i++) {
                QContactRelationship currRelationship = allRelationships.at(i);
                if (req->relationshipType() == currRelationship.relationshipType()) {
                    requestedRelationships.append(currRelationship);
                }
            }
        }

        // third criteria: participant must be empty or must match (including role in relationship)
        QString myUri = req->manager()->managerUri();
        QContactId anonymousParticipant;
        anonymousParticipant.setLocalId(QContactLocalId(0));
        anonymousParticipant.setManagerUri(QString());
        if (req->participant() != anonymousParticipant) {
            allRelationships = requestedRelationships;
            requestedRelationships.clear();
            for (int i = 0; i < allRelationships.size(); i++) {
                QContactRelationship currRelationship = allRelationships.at(i);
                if ((req->participantRole() == QContactRelationshipFilter::Either || req->participantRole() == QContactRelationshipFilter::Second)
                        && currRelationship.second() == req->participant()) {
                    requestedRelationships.append(currRelationship);
                } else if ((req->participantRole() == QContactRelationshipFilter::Either || req->participantRole() == QContactRelationshipFilter::First)
                        && currRelationship.first() == req->participant()) {
                    requestedRelationships.append(currRelationship);
                }
            }
        }

        // update the request with the results.
        QContactManagerEngine::updateRequest(req, requestedRelationships, operationError, operationErrors, QContactAbstractRequest::Finished);
    }
}

/*!
 * Processes the QContactRelationshipRemoveRequest \a req
 * \sa QContactRelationshipRemoveRequest
 */
void QContactRequestWorker::processContactRelationshipRemoveRequest(QContactRelationshipRemoveRequest* req)
{
    if (req->manager()) {
        QList<QContactManager::Error> operationErrors;
        QList<QContactRelationship> matchingRelationships = req->manager()->relationships(req->relationshipType(), req->first(), QContactRelationshipFilter::First);
        QContactManager::Error operationError = req->manager()->error();

        for (int i = 0; i < matchingRelationships.size(); i++) {
            QContactManager::Error tempError;
            QContactRelationship possibleMatch = matchingRelationships.at(i);

            // if the second criteria matches, or is default constructed id, then we have a match and should remove it.
            if (req->second() == QContactId() || possibleMatch.second() == req->second()) {
                req->manager()->removeRelationship(matchingRelationships.at(i));
                tempError = req->manager()->error();
                operationErrors.append(tempError);

                if (tempError != QContactManager::NoError)
                     operationError = tempError;
            }
        }

        // there are no results, so just update the status with the error.
        QContactManagerEngine::updateRequestStatus(req, operationError, operationErrors, QContactAbstractRequest::Finished);
    }
}

/*!
 * Processes the QContactRelationshipSaveRequest \a req
 * \sa QContactRelationshipSaveRequest
 */
void QContactRequestWorker::processContactRelationshipSaveRequest(QContactRelationshipSaveRequest* req)
{
    if (req->manager()) {
        QContactManager::Error operationError = QContactManager::NoError;
        QList<QContactManager::Error> operationErrors;
        QList<QContactRelationship> requestRelationships = req->relationships();
        QList<QContactRelationship> savedRelationships;

        QContactManager::Error tempError;
        for (int i = 0; i < requestRelationships.size(); i++) {
            QContactRelationship current = requestRelationships.at(i);
            req->manager()->saveRelationship(&current);
            tempError = req->manager()->error();
            savedRelationships.append(current);
            operationErrors.append(tempError);

            if (tempError != QContactManager::NoError)
                operationError = tempError;
        }

        // update the request with the results.
        QContactManagerEngine::updateRequest(req, savedRelationships, operationError, operationErrors, QContactAbstractRequest::Finished);
    }
}

QContactAbstractRequest* QContactRequestWorkerData::takeFirstRequest()
{
    QMutexLocker locker(&m_mutex);

    // take the first pending request and finish it
    if (m_requestQueue.isEmpty())
        m_newRequestAdded.wait(&m_mutex);
    if (!m_requestQueue.isEmpty()) {
        m_currentRequest = m_requestQueue.head();
        return m_currentRequest;
    }

    // means that we are stopping the request worker (ie, deleting the manager)
    return 0;
}
