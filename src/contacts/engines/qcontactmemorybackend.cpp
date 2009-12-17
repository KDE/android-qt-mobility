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

#include "qcontactmanager.h"

#include "qcontactdetaildefinition.h"
#include "qcontactmanagerengine.h"
#include "qcontactabstractrequest.h"
#include "qcontactrequests.h"
#include "qcontactchangeset.h"
#include "qcontactdetails.h"

#include "qcontactmemorybackend_p.h"

#include <QTimer>
#include <QUuid>
#include <QSharedData>

QTM_BEGIN_NAMESPACE

/*!
  \class QContactMemoryEngine
  \brief The QContactMemoryEngine class provides an in-memory implementation
  of a contacts backend.
 
  It may be used as a reference implementation, or when persistent storage is not required.
 
  During construction, it will load the in-memory data associated with the memory store
  identified by the "id" parameter from the given parameters if it exists, or a new,
  anonymous store if it does not.
 
  Data stored in this engine is only available in the current process.
 
  This engine supports sharing, so an internal reference count is increased
  whenever a manager uses this backend, and is decreased when the manager
  no longer requires this engine.
 */

/* static data for manager class */
QMap<QString, QContactMemoryEngine*> QContactMemoryEngine::engines;

/*!
 * Factory function for creating a new in-memory backend, based
 * on the given \a parameters.
 *
 * The same engine will be returned for multiple calls with the
 * same value for the "id" parameter, while one of them is in scope.
 */
QContactMemoryEngine* QContactMemoryEngine::createMemoryEngine(const QMap<QString, QString>& parameters)
{
    bool anonymous = false;
    QString idValue = parameters.value(QLatin1String("id"));
    if (idValue.isNull() || idValue.isEmpty()) {
        // no store given?  new, anonymous store.
        idValue = QUuid::createUuid().toString();
        anonymous = true;
    }

    if (engines.contains(idValue)) {
        QContactMemoryEngine *engine = engines.value(idValue);
        engine->d->m_refCount.ref();
        engine->d->m_anonymous = anonymous;
        return engine;
    } else {
        QContactMemoryEngine *engine = new QContactMemoryEngine(parameters);
        engine->d->m_engineName = QString(QLatin1String("memory"));
        engine->d->m_engineVersion = 1;
        engine->d->m_id = idValue;
        engine->d->m_anonymous = anonymous;
        engines.insert(idValue, engine);
        return engine;
    }
}

/*!
 * Constructs a new in-memory backend.
 *
 * Loads the in-memory data associated with the memory store identified by the "id" parameter
 * from the given \a parameters if it exists, or a new, anonymous store if it does not.
 */
QContactMemoryEngine::QContactMemoryEngine(const QMap<QString, QString>& parameters)
    : d(new QContactMemoryEngineData)
{
    Q_UNUSED(parameters);
}

/*! \reimp */
void QContactMemoryEngine::deref()
{
    if (!d->m_refCount.deref()) {
        engines.remove(d->m_id);
        delete d;
        delete this;
    }
}

/*! \reimp */
QString QContactMemoryEngine::managerName() const
{
    return d->m_engineName;
}

/*! \reimp */
int QContactMemoryEngine::implementationVersion() const
{
    return d->m_engineVersion;
}

/*! \reimp */
QMap<QString, QString> QContactMemoryEngine::managerParameters() const
{
    QMap<QString, QString> params;
    params.insert(QLatin1String("id"), d->m_id);
    return params;
}

/*! \reimp */
bool QContactMemoryEngine::setSelfContactId(const QContactLocalId& contactId, QContactManager::Error& error)
{
    if (contactId == QContactLocalId(0) || d->m_contactIds.contains(contactId)) {
        error = QContactManager::NoError;
        QContactLocalId oldId = d->m_selfContactId;
        d->m_selfContactId = contactId;

        QContactChangeSet cs;
        cs.oldAndNewSelfContactId() = QPair<QContactLocalId, QContactLocalId>(oldId, contactId);
        cs.emitSignals(this);
        return true;
    }

    error = QContactManager::DoesNotExistError;
    return false;
}

/*! \reimp */
QContactLocalId QContactMemoryEngine::selfContactId(QContactManager::Error& error) const
{
    error = QContactManager::DoesNotExistError;
    if (d->m_selfContactId != QContactLocalId(0))
        error = QContactManager::NoError;
    return d->m_selfContactId;
}

/*! \reimp */
QList<QContactLocalId> QContactMemoryEngine::contacts(const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const
{
    // TODO: this needs to be done properly...
    error = QContactManager::NoError;
    QList<QContactLocalId> sortedIds;
    QList<QContact> sortedContacts;
    for (int i = 0; i < d->m_contacts.size(); i++)
        QContactManagerEngine::addSorted(&sortedContacts, d->m_contacts.at(i), sortOrders);
    for (int i = 0; i < sortedContacts.size(); i++)
        sortedIds.append(sortedContacts.at(i).id().localId());
    return sortedIds;
}

/*! \reimp */
QContact QContactMemoryEngine::contact(const QContactLocalId& contactId, QContactManager::Error& error) const
{
    int index = d->m_contactIds.indexOf(contactId);
    if (index != -1) {
        // found the contact successfully.
        error = QContactManager::NoError;
        QContact retn = d->m_contacts.at(index);

        // synthesize the display label if we need to.
        QContactDisplayLabel dl = retn.detail(QContactDisplayLabel::DefinitionName);
        if (dl.label().isEmpty()) {
            QContactManager::Error synthError;
            retn = setContactDisplayLabel(synthesizeDisplayLabel(retn, synthError), retn);
        }

        // also, retrieve the current relationships the contact is involved with.
        QList<QContactRelationship> relationshipCache = d->m_orderedRelationships.value(contactId);
        QContactManagerEngine::setContactRelationships(&retn, relationshipCache);

        // and return the contact
        return retn;
    }

    error = QContactManager::DoesNotExistError;
    return QContact();
}

bool QContactMemoryEngine::saveContact(QContact* theContact, QContactChangeSet& changeSet, QContactManager::Error& error)
{
    // ensure that the contact's details conform to their definitions
    if (!validateContact(*theContact, error)) {
        return false;
    }

    // check to see if this contact already exists
    int index = d->m_contactIds.indexOf(theContact->id().localId());
    if (index != -1) {
        /* We also need to check that there are no modified create only details */
        QContact oldContact = d->m_contacts.at(index);

        if (oldContact.type() != theContact->type()) {
            error = QContactManager::AlreadyExistsError;
            return false;
        }

        QSetIterator<QString> it(d->m_createOnlyIds.value(theContact->type()));
        while (it.hasNext()) {
            const QString& id = it.next();
            QList<QContactDetail> details = oldContact.details(id);
            QList<QContactDetail> newDetails = theContact->details(id);

            /* Any entries in old should still be in new */
            if (newDetails.count() < details.count()) {
                error = QContactManager::DetailAccessError;
                return false;
            }

            /* Now do a more detailed check */
            for (int i=0; i < details.count(); i++) {
                if (!newDetails.contains(details.at(i))) {
                    error = QContactManager::DetailAccessError;
                    return false;
                }
            }
        }

        QContactTimestamp ts = theContact->detail(QContactTimestamp::DefinitionName);
        ts.setLastModified(QDateTime::currentDateTime());
        theContact->saveDetail(&ts);

        /* And we need to check that the relationships are up-to-date or not modified */
        QList<QContactRelationship> orderedList = theContact->relationshipOrder();
        QList<QContactRelationship> upToDateList = d->m_orderedRelationships.value(theContact->localId());
        if (theContact->relationships() != orderedList) {
            // the user has modified the order of relationships; we may need to update the lists etc.
            if (upToDateList.size() != orderedList.size()) {
                // the cache was stale; relationships have been added or removed in the meantime.
                error = QContactManager::InvalidRelationshipError;
                return false;
            }
            
            // size is the same, need to ensure that no invalid relationships are in the list.
            for (int i = 0; i < orderedList.size(); i++) {
                QContactRelationship currOrderedRel = orderedList.at(i);
                if (!upToDateList.contains(currOrderedRel)) {
                    // the cache was stale; relationships have been added and removed in the meantime.
                    error = QContactManager::InvalidRelationshipError;
                    return false;
                }
            }

            // everything is fine.  update the up-to-date list
            d->m_orderedRelationships.insert(theContact->localId(), orderedList);
        }

        // synthesize the display label for the contact.
        QContact saveContact = setContactDisplayLabel(synthesizeDisplayLabel(*theContact, error), *theContact);
        *theContact = saveContact;

        // Looks ok, so continue
        d->m_contacts.replace(index, *theContact);
        changeSet.changedContacts().insert(theContact->localId());
    } else {
        // id does not exist; if not zero, fail.
        QContactId newId;
        newId.setManagerUri(managerUri());
        if (theContact->id() != QContactId() && theContact->id() != newId) {
            // the ID is not empty, and it doesn't identify an existing contact in our database either.
            error = QContactManager::DoesNotExistError;
            return false;
        }

        /* New contact */
        QContactTimestamp ts = theContact->detail(QContactTimestamp::DefinitionName);
        ts.setLastModified(QDateTime::currentDateTime());
        ts.setCreated(ts.lastModified());
        theContact->saveDetail(&ts);

        // update the contact item - set its ID
        newId.setLocalId(++d->m_nextContactId);
        theContact->setId(newId);

        // synthesize the display label for the contact.
        QContact saveContact = setContactDisplayLabel(synthesizeDisplayLabel(*theContact, error), *theContact);
        *theContact = saveContact;

        // finally, add the contact to our internal lists and return
        d->m_contacts.append(*theContact);                   // add contact to list
        d->m_contactIds.append(theContact->localId());  // track the contact id.

        changeSet.addedContacts().insert(theContact->localId());
    }

    error = QContactManager::NoError;     // successful.
    return true;
}

/*! \reimp */
bool QContactMemoryEngine::saveContact(QContact* contact, QContactManager::Error& error)
{
    QContactChangeSet changeSet;
    bool retn = saveContact(contact, changeSet, error);
    changeSet.emitSignals(this);
    return retn;
}

/*! \reimp */
QList<QContactManager::Error> QContactMemoryEngine::saveContacts(QList<QContact>* contacts, QContactManager::Error& error)
{
    QList<QContactManager::Error> ret;
    if (!contacts) {
        error = QContactManager::BadArgumentError;
        return ret;
    } else {
        // for batch processing, we store up the changes and emit at the end.
        QContactChangeSet changeSet;
        QContactManager::Error functionError = QContactManager::NoError;
        for (int i = 0; i < contacts->count(); i++) {
            QContact current = contacts->at(i);
            if (!saveContact(&current, changeSet, error)) {
                functionError = error;
                ret.append(functionError);
            } else {
                (*contacts)[i] = current;
                ret.append(QContactManager::NoError);
            }
        }

        error = functionError;
        changeSet.emitSignals(this);
        return ret;
    }
}

bool QContactMemoryEngine::removeContact(const QContactLocalId& contactId, QContactChangeSet& changeSet, QContactManager::Error& error)
{
    int index = d->m_contactIds.indexOf(contactId);

    if (index == -1) {
        error = QContactManager::DoesNotExistError;
        return false;
    }

    // remove the contact from any relationships it was in.
    QContactId thisContact;
    thisContact.setManagerUri(managerUri());
    thisContact.setLocalId(contactId);
    QList<QContactRelationship> allRelationships = relationships(QString(), thisContact, QContactRelationshipFilter::Either, error);
    if (error != QContactManager::NoError && error != QContactManager::DoesNotExistError) {
        error = QContactManager::UnspecifiedError; // failed to clean up relationships
        return false;
    }

    // this is meant to be a transaction, so if any of these fail, we're in BIG TROUBLE.
    // a real backend will use DBMS transactions to ensure database integrity.
    for (int i = 0; i < allRelationships.size(); i++) {
        QContactRelationship currRel = allRelationships.at(i);
        removeRelationship(currRel, error);
    }

    // having cleaned up the relationships, remove the contact from the lists.
    d->m_contacts.removeAt(index);
    d->m_contactIds.removeAt(index);
    error = QContactManager::NoError;

    // and if it was the self contact, reset the self contact id
    if (contactId == d->m_selfContactId) {
        d->m_selfContactId = QContactLocalId(0);
        changeSet.oldAndNewSelfContactId() = QPair<QContactLocalId, QContactLocalId>(contactId, QContactLocalId(0));
    }

    changeSet.removedContacts().insert(contactId);
    return true;
}

/*! \reimp */
bool QContactMemoryEngine::removeContact(const QContactLocalId& contactId, QContactManager::Error& error)
{
    QContactChangeSet changeSet;
    bool retn = removeContact(contactId, changeSet, error);
    changeSet.emitSignals(this);
    return retn;
}

/*! \reimp */
QList<QContactManager::Error> QContactMemoryEngine::removeContacts(QList<QContactLocalId>* contactIds, QContactManager::Error& error)
{
    QList<QContactManager::Error> ret;
    if (!contactIds) {
        error = QContactManager::BadArgumentError;
        return ret;
    }

    // for batch processing, we store up the changes and emit at the end.
    QContactChangeSet changeSet;
    QContactManager::Error functionError = QContactManager::NoError;
    for (int i = 0; i < contactIds->count(); i++) {
        QContactLocalId current = contactIds->at(i);
        if (!removeContact(current, changeSet, error)) {
            functionError = error;
            ret.append(functionError);
        } else {
            (*contactIds)[i] = 0;
            ret.append(QContactManager::NoError);
        }
    }

    error = functionError;
    changeSet.emitSignals(this);
    return ret;
}

/*! \reimp */
QList<QContactRelationship> QContactMemoryEngine::relationships(const QString& relationshipType, const QContactId& participantId, QContactRelationshipFilter::Role role, QContactManager::Error& error) const
{
    QContactId defaultId;
    QList<QContactRelationship> retn;
    for (int i = 0; i < d->m_relationships.size(); i++) {
        QContactRelationship curr = d->m_relationships.at(i);

        // check that the relationship type matches
        if (curr.relationshipType() != relationshipType && !relationshipType.isEmpty())
            continue;

        // if the participantId argument is default constructed, then the relationship matches.
        if (participantId == defaultId) {
            retn.append(curr);
            continue;
        }

        // otherwise, check that the participant exists and plays the required role in the relationship.
        if (role == QContactRelationshipFilter::First && curr.first() == participantId) {
            retn.append(curr);
        } else if (role == QContactRelationshipFilter::Second && curr.second() == participantId) {
            retn.append(curr);
        } else if (role == QContactRelationshipFilter::Either && (curr.first() == participantId || curr.second() == participantId)) {
            retn.append(curr);
        }
    }

    error = QContactManager::NoError;
    if (retn.isEmpty())
        error = QContactManager::DoesNotExistError;
    return retn;
}

bool QContactMemoryEngine::saveRelationship(QContactRelationship* relationship, QContactChangeSet& changeSet, QContactManager::Error& error)
{
    // Attempt to validate the relationship.
    // first, check that the source contact exists and is in this manager.
    QString myUri = managerUri();
    if ((!relationship->first().managerUri().isEmpty() && relationship->first().managerUri() != myUri)
            ||!d->m_contactIds.contains(relationship->first().localId())) {
        error = QContactManager::InvalidRelationshipError;
        return false;
    }

    // second, check that the second contact exists (if it's local); we cannot check other managers' contacts.
    QContactId dest = relationship->second();

    if (dest.managerUri().isEmpty() || dest.managerUri() == myUri) {
        // this entry in the destination list is supposedly stored in this manager.
        // check that it exists, and that it isn't the source contact (circular)
        if (!d->m_contactIds.contains(dest.localId()) || dest.localId() == relationship->first().localId()) {
            error = QContactManager::InvalidRelationshipError;
            return false;
        }
    }

    // the relationship is valid.  We need to update the manager URIs in the second contact if it is empty to our URI.
    if (dest.managerUri().isEmpty()) {
        // need to update the URI
        dest.setManagerUri(myUri);
        relationship->setSecond(dest);
    }

    // check to see if the relationship already exists in the database.  If so, replace.
    // We do this because we don't want duplicates in our lists / maps of relationships.
    error = QContactManager::NoError;
    QList<QContactRelationship> allRelationships = d->m_relationships;
    for (int i = 0; i < allRelationships.size(); i++) {
        QContactRelationship curr = allRelationships.at(i);
        if (curr == *relationship) {
            d->m_relationships.removeAt(i);
            d->m_relationships.insert(i, *relationship);
            return true;
            // TODO: set error to AlreadyExistsError and return false?
        }
    }

    // no matching relationship; must be new.  append it to lists in our map of relationships where required.
    QList<QContactRelationship> firstRelationships = d->m_orderedRelationships.value(relationship->first().localId());
    QList<QContactRelationship> secondRelationships = d->m_orderedRelationships.value(relationship->second().localId());
    firstRelationships.append(*relationship);
    secondRelationships.append(*relationship);
    d->m_orderedRelationships.insert(relationship->first().localId(), firstRelationships);
    d->m_orderedRelationships.insert(relationship->second().localId(), secondRelationships);
    changeSet.addedRelationshipsContacts().insert(relationship->first().localId());
    changeSet.addedRelationshipsContacts().insert(relationship->second().localId());

    // finally, insert into our list of all relationships, and return.
    d->m_relationships.append(*relationship);
    return true;
}

/*! \reimp */
bool QContactMemoryEngine::saveRelationship(QContactRelationship* relationship, QContactManager::Error& error)
{
    QContactChangeSet changeSet;
    bool retn = saveRelationship(relationship, changeSet, error);
    changeSet.emitSignals(this);
    return retn;
}

/*! \reimp */
QList<QContactManager::Error> QContactMemoryEngine::saveRelationships(QList<QContactRelationship>* relationships, QContactManager::Error& error)
{
    error = QContactManager::NoError;
    QContactManager::Error functionError;
    QContactChangeSet changeSet;
    QList<QContactManager::Error> retn;
    for (int i = 0; i < relationships->size(); i++) {
        QContactRelationship curr = relationships->at(i);
        saveRelationship(&curr, changeSet, functionError);
        retn.append(functionError);

        // and replace the current relationship with the updated version.
        relationships->replace(i, curr);

        // also, update the total error if it did not succeed.
        if (functionError != QContactManager::NoError)
            error = functionError;
    }

    changeSet.emitSignals(this);
    return retn;
}

bool QContactMemoryEngine::removeRelationship(const QContactRelationship& relationship, QContactChangeSet& changeSet, QContactManager::Error& error)
{
    // attempt to remove it from our list of relationships.
    if (!d->m_relationships.removeOne(relationship)) {
        error = QContactManager::DoesNotExistError;
        return false;
    }

    // if that worked, then we need to remove it from the two locations in our map, also.
    QList<QContactRelationship> firstRelationships = d->m_orderedRelationships.value(relationship.first().localId());
    QList<QContactRelationship> secondRelationships = d->m_orderedRelationships.value(relationship.second().localId());
    firstRelationships.removeOne(relationship);
    secondRelationships.removeOne(relationship);
    d->m_orderedRelationships.insert(relationship.first().localId(), firstRelationships);
    d->m_orderedRelationships.insert(relationship.second().localId(), secondRelationships);

    // set our changes, and return.
    changeSet.removedRelationshipsContacts().insert(relationship.first().localId());
    changeSet.removedRelationshipsContacts().insert(relationship.second().localId());
    error = QContactManager::NoError;
    return true;
}

/*! \reimp */
bool QContactMemoryEngine::removeRelationship(const QContactRelationship& relationship, QContactManager::Error& error)
{
    QContactChangeSet changeSet;
    bool retn = removeRelationship(relationship, changeSet, error);
    changeSet.emitSignals(this);
    return retn;
}

/*! \reimp */
QList<QContactManager::Error> QContactMemoryEngine::removeRelationships(const QList<QContactRelationship>& relationships, QContactManager::Error& error)
{
    QList<QContactManager::Error> retn;
    QContactManager::Error functionError;
    for (int i = 0; i < relationships.size(); i++) {
        removeRelationship(relationships.at(i), functionError);
        retn.append(functionError);

        // update the total error if it did not succeed.
        if (functionError != QContactManager::NoError) {
            error = functionError;
        }
    }

    return retn;
}

/*! \reimp */
QMap<QString, QContactDetailDefinition> QContactMemoryEngine::detailDefinitions(const QString& contactType, QContactManager::Error& error) const
{
    // lazy initialisation of schema definitions.
    if (d->m_definitions.isEmpty()) {
        d->m_definitions = QContactManagerEngine::schemaDefinitions();

        // Extract create only definitions
        QMap<QString, QContactDetailDefinition> defMapForThisType(d->m_definitions.value(contactType));
        QStringList defMapKeys = defMapForThisType.keys();
        QSet<QString> createOnlyDefs;
        for (int i = 0; i < defMapKeys.size(); i++) {
            if (defMapForThisType.value(defMapKeys.at(i)).accessConstraint() == QContactDetailDefinition::CreateOnly) {
                createOnlyDefs.insert(defMapKeys.at(i));
            }
        }

        if (!createOnlyDefs.isEmpty()) {
            d->m_createOnlyIds.insert(contactType, createOnlyDefs);
        }
    }

    error = QContactManager::NoError;
    return d->m_definitions.value(contactType);
}

bool QContactMemoryEngine::saveDetailDefinition(const QContactDetailDefinition& def, const QString& contactType, QContactChangeSet& changeSet, QContactManager::Error& error)
{
    // we should check for changes to the database in this function, and add ids of changed data to changeSet. TODO.
    Q_UNUSED(changeSet);

    if (!validateDefinition(def, error)) {
        return false;
    }

    detailDefinitions(contactType, error); // just to populate the definitions if we haven't already.
    QMap<QString, QContactDetailDefinition> defsForThisType = d->m_definitions.value(contactType);
    defsForThisType.insert(def.name(), def);
    d->m_definitions.insert(contactType, defsForThisType);
    if (def.accessConstraint() == QContactDetailDefinition::CreateOnly) {
        QSet<QString> createOnlyDefs = d->m_createOnlyIds.value(contactType);
        createOnlyDefs.insert(def.name());
        d->m_createOnlyIds.insert(contactType, createOnlyDefs);
    }

    error = QContactManager::NoError;
    return true;
}

/*! \reimp */
bool QContactMemoryEngine::saveDetailDefinition(const QContactDetailDefinition& def, const QString& contactType, QContactManager::Error& error)
{
    QContactChangeSet changeSet;
    bool retn = saveDetailDefinition(def, contactType, changeSet, error);
    changeSet.emitSignals(this);
    return retn;
}

bool QContactMemoryEngine::removeDetailDefinition(const QString& definitionId, const QString& contactType, QContactChangeSet& changeSet, QContactManager::Error& error)
{
    // we should check for changes to the database in this function, and add ids of changed data to changeSet...
    // we should also check to see if the changes have invalidated any contact data, and add the ids of those contacts
    // to the change set.  TODO!
    Q_UNUSED(changeSet);

    if (definitionId.isEmpty()) {
        error = QContactManager::BadArgumentError;
        return false;
    }

    detailDefinitions(contactType, error); // just to populate the definitions if we haven't already.
    QMap<QString, QContactDetailDefinition> defsForThisType = d->m_definitions.value(contactType);
    bool success = defsForThisType.remove(definitionId);
    d->m_definitions.insert(contactType, defsForThisType);
    QSet<QString> createOnlyDefsForThisType = d->m_createOnlyIds.value(contactType);
    createOnlyDefsForThisType.remove(definitionId);
    d->m_createOnlyIds.insert(contactType, createOnlyDefsForThisType);
    if (success)
        error = QContactManager::NoError;
    else
        error = QContactManager::DoesNotExistError;
    return success;
}

/*! \reimp */
bool QContactMemoryEngine::removeDetailDefinition(const QString& definitionId, const QString& contactType, QContactManager::Error& error)
{
    QContactChangeSet changeSet;
    bool retn = removeDetailDefinition(definitionId, contactType, changeSet, error);
    changeSet.emitSignals(this);
    return retn;
}

/*! \reimp */
void QContactMemoryEngine::requestDestroyed(QContactAbstractRequest* req)
{
    d->m_asynchronousOperations.removeOne(req);
}

/*! \reimp */
bool QContactMemoryEngine::startRequest(QContactAbstractRequest* req)
{
    d->m_asynchronousOperations.enqueue(req);
    QList<QContactManager::Error> dummy;
    updateRequestStatus(req, QContactManager::NoError, dummy, QContactAbstractRequest::Active);
    QTimer::singleShot(0, this, SLOT(performAsynchronousOperation()));
    return true;
}

/*! \reimp */
bool QContactMemoryEngine::cancelRequest(QContactAbstractRequest* req)
{
    QList<QContactManager::Error> dummy;
    updateRequestStatus(req, QContactManager::NoError, dummy, QContactAbstractRequest::Cancelling);
    return true;
}

/*! \reimp */
bool QContactMemoryEngine::waitForRequestProgress(QContactAbstractRequest* req, int msecs)
{
    Q_UNUSED(msecs);

    if (!d->m_asynchronousOperations.removeOne(req))
        return false; // didn't exist.

    // replace at head of queue
    d->m_asynchronousOperations.insert(0, req);

    // and perform the operation.
    performAsynchronousOperation();

    return true;
}

/*! \reimp */
bool QContactMemoryEngine::waitForRequestFinished(QContactAbstractRequest* req, int msecs)
{
    // in our implementation, we always complete any operation we start.
    // so, waitForRequestFinished is equivalent to waitForRequestProgress.
    return waitForRequestProgress(req, msecs);
}

/*!
 * This slot is called some time after an asynchronous request is started.
 * It performs the required operation, sets the result and returns.
 */
void QContactMemoryEngine::performAsynchronousOperation()
{
    QContactAbstractRequest *currentRequest;

    // take the first pending request and finish it
    if (d->m_asynchronousOperations.isEmpty())
        return;
    currentRequest = d->m_asynchronousOperations.dequeue();

    // check to see if it is cancelling; if so, cancel it and perform update.
    if (currentRequest->status() == QContactAbstractRequest::Cancelling) {
        QList<QContactManager::Error> dummy;
        updateRequestStatus(currentRequest, QContactManager::NoError, dummy, QContactAbstractRequest::Cancelled);
        return;
    }

    // store up changes, and emit signals once at the end of the (possibly batch) operation.
    QContactChangeSet changeSet;

    // Now perform the active request and emit required signals.
    Q_ASSERT(currentRequest->status() == QContactAbstractRequest::Active);
    switch (currentRequest->type()) {
        case QContactAbstractRequest::ContactFetchRequest:
        {
            QContactFetchRequest* r = static_cast<QContactFetchRequest*>(currentRequest);
            QContactFilter filter = r->filter();
            QList<QContactSortOrder> sorting = r->sorting();
            QStringList defs = r->definitionRestrictions();

            QContactManager::Error operationError;
            QList<QContactManager::Error> operationErrors;
            QList<QContact> requestedContacts;
            QList<QContactLocalId> requestedContactIds = contacts(filter, sorting, operationError);

            QContactManager::Error tempError;
            for (int i = 0; i < requestedContactIds.size(); i++) {
                QContact current = contact(requestedContactIds.at(i), tempError);
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
            updateRequest(currentRequest, requestedContacts, operationError, operationErrors, QContactAbstractRequest::Finished);
        }
        break;

        case QContactAbstractRequest::ContactLocalIdFetchRequest:
        {
            QContactLocalIdFetchRequest* r = static_cast<QContactLocalIdFetchRequest*>(currentRequest);
            QContactFilter filter = r->filter();
            QList<QContactSortOrder> sorting = r->sorting();

            QContactManager::Error operationError = QContactManager::NoError;
            QList<QContactLocalId> requestedContactIds = contacts(filter, sorting, operationError);

            updateRequest(currentRequest, requestedContactIds, operationError, QList<QContactManager::Error>(), QContactAbstractRequest::Finished);
        }
        break;

        case QContactAbstractRequest::ContactSaveRequest:
        {
            QContactSaveRequest* r = static_cast<QContactSaveRequest*>(currentRequest);
            QList<QContact> contacts = r->contacts();

            QContactManager::Error operationError = QContactManager::NoError;
            QList<QContactManager::Error> operationErrors = saveContacts(&contacts, operationError);

            for (int i = 0; i < operationErrors.size(); i++) {
                if (operationErrors.at(i) != QContactManager::NoError) {
                    operationError = operationErrors.at(i);
                    break;
                }
            }

            updateRequest(currentRequest, contacts, operationError, operationErrors, QContactAbstractRequest::Finished);
        }
        break;

        case QContactAbstractRequest::ContactRemoveRequest:
        {
            // this implementation provides scant information to the user
            // the operation either succeeds (all contacts matching the filter were removed)
            // or it fails (one or more contacts matching the filter could not be removed)
            // if a failure occurred, the request error will be set to the most recent
            // error that occurred during the remove operation.
            QContactRemoveRequest* r = static_cast<QContactRemoveRequest*>(currentRequest);
            QContactFilter filter = r->filter();

            QContactManager::Error operationError = QContactManager::NoError;
            QList<QContactLocalId> contactsToRemove = contacts(filter, QList<QContactSortOrder>(), operationError);

            for (int i = 0; i < contactsToRemove.size(); i++) {
                QContactManager::Error tempError;
                removeContact(contactsToRemove.at(i), changeSet, tempError);

                if (tempError != QContactManager::NoError)
                    operationError = tempError;
            }

            // there are no results, so just update the status with the error.
            QList<QContactManager::Error> dummy;
            updateRequestStatus(currentRequest, operationError, dummy, QContactAbstractRequest::Finished);
        }
        break;

        case QContactAbstractRequest::DetailDefinitionFetchRequest:
        {
            QContactDetailDefinitionFetchRequest* r = static_cast<QContactDetailDefinitionFetchRequest*>(currentRequest);
            QContactManager::Error operationError = QContactManager::NoError;
            QList<QContactManager::Error> operationErrors;
            QMap<QString, QContactDetailDefinition> requestedDefinitions;
            QStringList names = r->names();
            if (names.isEmpty())
                names = detailDefinitions(r->contactType(), operationError).keys(); // all definitions.

            QContactManager::Error tempError;
            for (int i = 0; i < names.size(); i++) {
                QContactDetailDefinition current = detailDefinition(names.at(i), r->contactType(), tempError);
                operationErrors.append(tempError);
                requestedDefinitions.insert(names.at(i), current);

                if (tempError != QContactManager::NoError)
                    operationError = tempError;
            }

            // update the request with the results.
            updateRequest(currentRequest, requestedDefinitions, operationError, operationErrors, QContactAbstractRequest::Finished);
        }
        break;

        case QContactAbstractRequest::DetailDefinitionSaveRequest:
        {
            QContactDetailDefinitionSaveRequest* r = static_cast<QContactDetailDefinitionSaveRequest*>(currentRequest);
            QContactManager::Error operationError = QContactManager::NoError;
            QList<QContactManager::Error> operationErrors;
            QList<QContactDetailDefinition> definitions = r->definitions();
            QList<QContactDetailDefinition> savedDefinitions;

            QContactManager::Error tempError;
            for (int i = 0; i < definitions.size(); i++) {
                QContactDetailDefinition current = definitions.at(i);
                saveDetailDefinition(current, r->contactType(), changeSet, tempError);
                savedDefinitions.append(current);
                operationErrors.append(tempError);

                if (tempError != QContactManager::NoError)
                    operationError = tempError;
            }

            // update the request with the results.
            updateRequest(currentRequest, savedDefinitions, operationError, operationErrors, QContactAbstractRequest::Finished);
        }
        break;

        case QContactAbstractRequest::DetailDefinitionRemoveRequest:
        {
            QContactDetailDefinitionRemoveRequest* r = static_cast<QContactDetailDefinitionRemoveRequest*>(currentRequest);
            QStringList names = r->names();

            QContactManager::Error operationError = QContactManager::NoError;
            QList<QContactManager::Error> operationErrors;

            for (int i = 0; i < names.size(); i++) {
                QContactManager::Error tempError;
                removeDetailDefinition(names.at(i), r->contactType(), changeSet, tempError);
                operationErrors.append(tempError);

                if (tempError != QContactManager::NoError)
                    operationError = tempError;
            }

            // there are no results, so just update the status with the error.
            updateRequestStatus(currentRequest, operationError, operationErrors, QContactAbstractRequest::Finished);
        }
        break;

        case QContactAbstractRequest::RelationshipFetchRequest:
        {
            QContactRelationshipFetchRequest* r = static_cast<QContactRelationshipFetchRequest*>(currentRequest);
            QContactManager::Error operationError = QContactManager::NoError;
            QList<QContactManager::Error> operationErrors;
            QList<QContactRelationship> allRelationships = relationships(QString(), QContactId(), QContactRelationshipFilter::Either, operationError);
            QList<QContactRelationship> requestedRelationships;

            // first criteria: source contact id must be empty or must match
            if (r->first() == QContactId()) {
                // all relationships match this criteria (zero id denotes "any")
                requestedRelationships = allRelationships;
            } else {
                for (int i = 0; i < allRelationships.size(); i++) {
                    QContactRelationship currRelationship = allRelationships.at(i);
                    if (r->first() == currRelationship.first()) {
                        requestedRelationships.append(currRelationship);
                    }
                }
            }

            // second criteria: relationship type must be empty or must match
            if (!r->relationshipType().isEmpty()) {
                allRelationships = requestedRelationships;
                requestedRelationships.clear();
                for (int i = 0; i < allRelationships.size(); i++) {
                    QContactRelationship currRelationship = allRelationships.at(i);
                    if (r->relationshipType() == currRelationship.relationshipType()) {
                        requestedRelationships.append(currRelationship);
                    }
                }
            }

            // third criteria: participant must be empty or must match (including role in relationship)
            QString myUri = managerUri();
            QContactId anonymousParticipant;
            anonymousParticipant.setLocalId(QContactLocalId(0));
            anonymousParticipant.setManagerUri(QString());
            if (r->participant() != anonymousParticipant) {
                allRelationships = requestedRelationships;
                requestedRelationships.clear();
                for (int i = 0; i < allRelationships.size(); i++) {
                    QContactRelationship currRelationship = allRelationships.at(i);
                    if ((r->participantRole() == QContactRelationshipFilter::Either || r->participantRole() == QContactRelationshipFilter::Second)
                            && currRelationship.second() == r->participant()) {
                        requestedRelationships.append(currRelationship);
                    } else if ((r->participantRole() == QContactRelationshipFilter::Either || r->participantRole() == QContactRelationshipFilter::First)
                            && currRelationship.first() == r->participant()) {
                        requestedRelationships.append(currRelationship);
                    }
                }
            }

            // update the request with the results.
            updateRequest(currentRequest, requestedRelationships, operationError, operationErrors, QContactAbstractRequest::Finished);
        }
        break;

        case QContactAbstractRequest::RelationshipRemoveRequest:
        {
            QContactRelationshipRemoveRequest* r = static_cast<QContactRelationshipRemoveRequest*>(currentRequest);
            QContactManager::Error operationError = QContactManager::NoError;
            QList<QContactManager::Error> operationErrors;
            QList<QContactRelationship> matchingRelationships = relationships(r->relationshipType(), r->first(), QContactRelationshipFilter::First, operationError);

            bool foundMatch = false;
            for (int i = 0; i < matchingRelationships.size(); i++) {
                QContactManager::Error tempError;
                QContactRelationship possibleMatch = matchingRelationships.at(i);

                // if the second criteria matches, or is default constructed id, then we have a match and should remove it.
                if (r->second() == QContactId() || possibleMatch.second() == r->second()) {
                    foundMatch = true;
                    removeRelationship(matchingRelationships.at(i), tempError);
                    operationErrors.append(tempError);

                    if (tempError != QContactManager::NoError)
                        operationError = tempError;
                }
            }
            
            if (foundMatch == false && operationError == QContactManager::NoError)
                operationError = QContactManager::DoesNotExistError;

            // there are no results, so just update the status with the error.
            updateRequestStatus(currentRequest, operationError, operationErrors, QContactAbstractRequest::Finished);
        }
        break;

        case QContactAbstractRequest::RelationshipSaveRequest:
        {
            QContactRelationshipSaveRequest* r = static_cast<QContactRelationshipSaveRequest*>(currentRequest);
            QContactManager::Error operationError = QContactManager::NoError;
            QList<QContactManager::Error> operationErrors;
            QList<QContactRelationship> requestRelationships = r->relationships();
            QList<QContactRelationship> savedRelationships;

            QContactManager::Error tempError;
            for (int i = 0; i < requestRelationships.size(); i++) {
                QContactRelationship current = requestRelationships.at(i);
                saveRelationship(&current, tempError);
                savedRelationships.append(current);
                operationErrors.append(tempError);

                if (tempError != QContactManager::NoError)
                    operationError = tempError;
            }

            // update the request with the results.
            updateRequest(currentRequest, savedRelationships, operationError, operationErrors, QContactAbstractRequest::Finished);
        }
        break;

        default: // unknown request type.
        break;
    }

    // now emit any signals we have to emit
    changeSet.emitSignals(this);
}

/*!
 * \reimp
 */
bool QContactMemoryEngine::hasFeature(QContactManager::ManagerFeature feature, const QString& contactType) const
{
    if (!supportedContactTypes().contains(contactType))
        return false;

    switch (feature) {
        case QContactManager::Groups:
        case QContactManager::ActionPreferences:
        case QContactManager::Relationships:
        case QContactManager::ArbitraryRelationshipTypes:
        case QContactManager::RelationshipOrdering:
        case QContactManager::MutableDefinitions:
            return true;
        case QContactManager::Anonymous:
            return d->m_anonymous;
        case QContactManager::SelfContact:
            return true;

        default:
            return false;
    }
}

/*!
 * \reimp
 */
QList<QVariant::Type> QContactMemoryEngine::supportedDataTypes() const
{
    QList<QVariant::Type> st;
    st.append(QVariant::String);
    st.append(QVariant::Date);
    st.append(QVariant::DateTime);
    st.append(QVariant::Time);
    st.append(QVariant::Bool);
    st.append(QVariant::Char);
    st.append(QVariant::Int);
    st.append(QVariant::UInt);
    st.append(QVariant::LongLong);
    st.append(QVariant::ULongLong);
    st.append(QVariant::Double);

    return st;
}

/*!
 * \reimp
 */
bool QContactMemoryEngine::filterSupported(const QContactFilter& filter) const
{
    Q_UNUSED(filter);
    // Until we add hashes for common stuff, fall back to slow code
    return false;
}

#include "moc_qcontactmemorybackend_p.cpp"

QTM_END_NAMESPACE

