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

#include "qcontactsymbianengine_p.h"
#include "qcontact_p.h"

#include <cntfldst.h>
#include <cntfield.h>

#include <qtcontacts.h>
#include <QSet>
#include "qcontactchangeset.h"
#include "qcontactsymbianfilterdbms.h"
#include "qcontactsymbianfiltersql.h"
#include "qcontactsymbiansorterdbms.h"
#include "cntrelationship.h"
#include "qcontactsymbiantransformerror.h"

#include <QDebug>

typedef QList<QContactLocalId> QContactLocalIdList;

/* ... The macros changed names */
#if QT_VERSION < QT_VERSION_CHECK(4, 6, 0)
#define QT_TRAP_THROWING QT_TRANSLATE_SYMBIAN_LEAVE_TO_EXCEPTION
#define QT_TRYCATCH_LEAVING QT_TRANSLATE_EXCEPTION_TO_SYMBIAN_LEAVE
#endif

// NOTE: There is a bug with RVCT compiler which causes the local stack
// variable to corrupt if the called function leaves. As a workaround we are
// reserving the objects from heap so it will not get corrupted.
// This of course applies only to those stack variables which are passed to
// the called function or the return value of the function is placed to the
// variable.

QContactSymbianEngineData::QContactSymbianEngineData(const QMap<QString, QString>& parameters, QContactManager::Error& error) :
    m_contactDatabase(0),
    m_transformContact(0)
{
    TRAPD(err, m_contactDatabase = CContactDatabase::OpenL())

    //Database not found, create it
    if(err == KErrNotFound)
    {
        TRAP(err, m_contactDatabase = CContactDatabase::CreateL())
    }

    //If database didn't open successfully return error
    if(err != KErrNone)
    {
        qContactSymbianTransformError(err, error);
    }

    //Database opened successfully
    else
    {
    // In pre 10.1 platforms the AddObserverL & RemoveObserver functions are not
    // exported so we need to use CContactChangeNotifier.
    // TODO: Is it ok to use __SYMBIAN_CNTMODEL_USE_SQLITE__ flag for this?
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
        TRAP(err, m_contactChangeNotifier = CContactChangeNotifier::NewL(*m_contactDatabase, this));
#else
        TRAP(err, m_contactDatabase->AddObserverL(*this));
#endif

        m_managerUri = QContactManager::buildUri(CNT_SYMBIAN_MANAGER_NAME, parameters);
    	m_transformContact = new CntTransformContact;
        m_contactFilter    = new QContactSymbianFilter(*m_contactDatabase);
        m_contactSorter    = new QContactSymbianSorter(*m_contactDatabase, *m_transformContact);
        m_relationship     = new CntRelationship(m_contactDatabase);
        qContactSymbianTransformError(err, error);
    }
}

QContactSymbianEngineData::~QContactSymbianEngineData()
{
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
	delete m_contactChangeNotifier;
#else
	if (m_contactDatabase != 0) {
	    m_contactDatabase->RemoveObserver(*this);
	}
#endif
	// m_contactFilter needs to be deleted before m_contactDatabase
    delete m_contactFilter;
    delete m_contactSorter;
	delete m_contactDatabase;
	delete m_transformContact;
	delete m_relationship;
}

/* Access */
QList<QContactLocalId> QContactSymbianEngineData::contacts(
        const QContactFilter& filter,
        const QList<QContactSortOrder>& sortOrders,
        QContactManager::Error& error) const
{
    return m_contactFilter->contacts(filter, sortOrders, error);
}

QAbstractContactFilter::FilterSupport QContactSymbianEngineData::filterSupported(
        const QContactFilter& filter) const
{
    return m_contactFilter->filterSupported(filter);
}

bool QContactSymbianEngineData::sortOrderSupported(const QList<QContactSortOrder>& sortOrders) const
{
    return m_contactSorter->sortOrderSupported(sortOrders);
}

/*!
 * Read a contact from the contact database.
 *
 * Internal implementation to read a conact, called by
 * QContactManager::contact().
 *
 * \param contactId The Id of the contact to be retrieved.
 * \param qtError Qt error code.
 * \return A QContact for the requested QUniquId value or 0 if the read
 *  operation was unsuccessful (e.g. contact not found).
 */
QContact QContactSymbianEngineData::contact(const QContactLocalId& contactId, QContactManager::Error& qtError) const
{
    // See QT_TRYCATCH_LEAVING note at the begginning of this file
    QContact* contact = new QContact();
    TRAPD(err, QT_TRYCATCH_LEAVING(*contact = contactL(contactId)));
    qContactSymbianTransformError(err, qtError);
    return *QScopedPointer<QContact>(contact);
}


/*!
 * Check how many contacts are in the database. This is an internal
 * wrapper function for CContactDatabase::CountL().
 *
 * If an error occurs, count will contain the error code and will be
 * less than zero.
 *
 * \return Number of contacts in database or an error code.
 */
int QContactSymbianEngineData::count() const
{
	// Call leaving implementation and trap the error code
	int count(0);
	// countL() can't throw c++ exception
	TRAPD(err, count = countL());

	// If there was a problem, return the negative value as the
	// error code.
	if (err != KErrNone) {
		count = err;
	}

	return count;
}

/*!
 * Complete list of contact IDs in database is needed until we
 * do something with views so that QContactModel can function.
 *
 * It works by retrieving a list of contact IDs changed since
 * epoch, as this API does not exist "properly" in CNTMODEL.
 *
 * \param sortOrders Sort order
 * \param qtError Qt error code.
 * \return List of all IDs for contact entries in the database,
 *  or an empty list if there was a problem or the database is
 *  empty.
 */
QList<QContactLocalId> QContactSymbianEngineData::contacts( const QList<QContactSortOrder>& sortOrders, QContactManager::Error& qtError) const
{
    return m_contactSorter->contacts( sortOrders, qtError );
}

/* groups */

/*!
 * Return a list of group UIDs.
 *
 * \param qtError Qt error code.
 * \return List of group IDs.
 */
QList<QContactLocalId> QContactSymbianEngineData::groups(const QList<QContactSortOrder>& sortOrders, QContactManager::Error& qtError) const
{
    // See QT_TRYCATCH_LEAVING note at the begginning of this file
    QContactLocalIdList *list = new QContactLocalIdList();
    TRAPD(err, QT_TRYCATCH_LEAVING(*list = groupsL()));
    qContactSymbianTransformError(err, qtError);
    return *QScopedPointer<QContactLocalIdList>(list);
}

/*!
 * Private leaving implementation for groups
 */
QList<QContactLocalId> QContactSymbianEngineData::groupsL() const
{
    QList<QContactLocalId> list;
    CContactIdArray* cIdList = m_contactDatabase->GetGroupIdListL();
    CleanupStack::PushL(cIdList);
    const int count = cIdList->Count();
    for (int i = 0; i < count; ++i)
    {
        list.append(QContactLocalId((*cIdList)[i]));
    }
    CleanupStack::PopAndDestroy(cIdList);
    return list;
}



/* add/update/delete */

/*!
 * Add the specified contact item to the persistent contacts store.
 *
 * \param contact The QContact to be saved.
 * \param id The Id of new contact
 * \param qtError Qt error code.
 * \return Error status
 */
bool QContactSymbianEngineData::addContact(QContact& contact, QContactChangeSet& changeSet, QContactManager::Error& qtError)
{
	// Attempt to persist contact, trapping errors
    int err(0);
    QContactLocalId id(0);
    TRAP(err, QT_TRYCATCH_LEAVING(id = addContactL(contact)));
    if(err == KErrNone)
    {
        changeSet.addedContacts().insert(id);
        m_contactsAddedEmitted.append(id);
    }
    qContactSymbianTransformError(err, qtError);

	return (err==KErrNone);
}

/*!
 * Update an existing contact entry in the database.
 *
 * \param contact The contact to update in the database.
 * \param qtError Qt error code.
 * \return Error status.
 */
bool QContactSymbianEngineData::updateContact(QContact& contact, QContactChangeSet& changeSet, QContactManager::Error& qtError)
{
    int err(0);
    TRAP(err, QT_TRYCATCH_LEAVING(updateContactL(contact)));
    if(err == KErrNone)
    {
        //TODO: check what to do with groupsChanged
        changeSet.changedContacts().insert(contact.localId());
        m_contactsChangedEmitted.append(contact.localId());
    }
    qContactSymbianTransformError(err, qtError);
    return (err==KErrNone);
}


/*!
 * Remove the specified contact object from the database.
 *
 * The removal of contacts from the underlying contacts model database
 * is performed in transactions of maximum 50 items at a time. E.g.
 * deleting 177 contacts would be done in 3 transactions of 50 and a
 * final transaction of 27.
 *
 * \param contact The QContact to be removed.
 * \param qtError Qt error code.
 * \return Error status.
 */
bool QContactSymbianEngineData::removeContact(const QContactLocalId &id, QContactChangeSet& changeSet, QContactManager::Error& qtError)
{
    // removeContactL() can't throw c++ exception
	TRAPD(err, removeContactL(id));
    if(err == KErrNone)
    {
        //TODO: check what to do with groupsChanged?
        changeSet.removedContacts().insert(id);
        m_contactsRemovedEmitted.append(id);
    }
    qContactSymbianTransformError(err, qtError);
	return (err==KErrNone);
}
/* relationships */

QList<QContactRelationship> QContactSymbianEngineData::relationships(const QString& relationshipType, const QContactId& participantId, QContactRelationshipFilter::Role role, QContactManager::Error& error) const
{
    //retrieve the relationships
    QList<QContactRelationship> returnValue = m_relationship->relationships(relationshipType, participantId, role, error);

    return returnValue;
}

bool QContactSymbianEngineData::saveRelationship(QContactChangeSet *changeSet, QContactRelationship* relationship, QContactManager::Error& error)
{
    //save the relationship
    bool returnValue = m_relationship->saveRelationship(&changeSet->addedRelationshipsContacts(), relationship, error);

    //add contacts to the list that shouldn't be emitted
    m_contactsAddedEmitted += changeSet->addedRelationshipsContacts().toList();

    return returnValue;
}

QList<QContactManager::Error> QContactSymbianEngineData::saveRelationships(QContactChangeSet *changeSet, QList<QContactRelationship>* relationships, QContactManager::Error& error)
{
    //save the relationships
    QList<QContactManager::Error> returnValue = m_relationship->saveRelationships(&changeSet->addedRelationshipsContacts(), relationships, error);

    //add contacts to the list that shouldn't be emitted
    m_contactsAddedEmitted += changeSet->addedRelationshipsContacts().toList();

    return returnValue;
}

bool QContactSymbianEngineData::removeRelationship(QContactChangeSet *changeSet, const QContactRelationship& relationship, QContactManager::Error& error)
{
    //remove the relationship
    bool returnValue = m_relationship->removeRelationship(&changeSet->removedRelationshipsContacts(), relationship, error);

    //add contacts to the list that shouldn't be emitted
    m_contactsRemovedEmitted += changeSet->removedRelationshipsContacts().toList();

    return returnValue;
}

QList<QContactManager::Error> QContactSymbianEngineData::removeRelationships(QContactChangeSet *changeSet, const QList<QContactRelationship>& relationships, QContactManager::Error& error)
{
    //remove the relationships
    QList<QContactManager::Error> returnValue = m_relationship->removeRelationships(&changeSet->removedRelationshipsContacts(), relationships, error);

    //add contacts to the list that shouldn't be emitted
    m_contactsRemovedEmitted += changeSet->removedRelationshipsContacts().toList();

    return returnValue;
}

/*!
 * Assigns some contact to be a "my card" contact
 *
 * \param contactId The id of contact to be set up as a "my card" contact.
 * \param qtError Qt error code.
 * \return Operation success status.
 */
bool QContactSymbianEngineData::setSelfContactId(const QContactLocalId& contactId, QContactManager::Error& qtError)
{
    if (contactId <= 0) {
        qtError = QContactManager::BadArgumentError;
        return false;
    }

    TContactItemId id(contactId);
    CContactItem* symContact = 0;
    TRAPD(err,
        symContact = m_contactDatabase->ReadContactL(id);
        m_contactDatabase->SetOwnCardL(*symContact);
        );
    delete symContact;
    qContactSymbianTransformError(err, qtError);
    return (err==KErrNone);
}

/*!
 * Returns an Id of the "my card" contact
 *
 * \param qtError Qt error code.
 * \return Id of the "my card" contact.
 */
QContactLocalId QContactSymbianEngineData::selfContactId(QContactManager::Error& qtError) const
{
    qtError = QContactManager::NoError;
    QContactLocalId id = 0;

    TContactItemId myCard = m_contactDatabase->OwnCardId();
    if (myCard < 0) {
        qtError = QContactManager::DoesNotExistError;
    }
    else {
        id = myCard;
    }
    return id;
}

/*!
 * Respond to a contacts database event, delegating this event to
 * an appropriate signal as required.
 *
 * \param aEvent Contacts database event describing the change to the
 *  database.
 */
void QContactSymbianEngineData::HandleDatabaseEventL(TContactDbObserverEvent aEvent)
{
        // TODO: Conversion to/from QContactLocalId

    TContactItemId id = aEvent.iContactId;

	switch (aEvent.iType)
	{
    case EContactDbObserverEventContactAdded:
        if(m_contactsAddedEmitted.contains(id))
            m_contactsAddedEmitted.removeOne(id);
        else
            emit contactAdded(id);
        break;
    case EContactDbObserverEventContactDeleted:
        if(m_contactsRemovedEmitted.contains(id))
            m_contactsRemovedEmitted.removeOne(id);
        else
            emit contactRemoved(id);
        break;
	case EContactDbObserverEventContactChanged:
        if(m_contactsChangedEmitted.contains(id))
            m_contactsChangedEmitted.removeOne(id);
        else
            emit contactChanged(id);
        break;
	case EContactDbObserverEventGroupAdded:
        if(m_contactsAddedEmitted.contains(id))
            m_contactsAddedEmitted.removeOne(id);
        else
            emit relationshipAdded(id);
		break;
	case EContactDbObserverEventGroupDeleted:
        if(m_contactsRemovedEmitted.contains(id))
            m_contactsRemovedEmitted.removeOne(id);
        else
            emit relationshipRemoved(id);
        break;
	case EContactDbObserverEventGroupChanged:
        if(m_contactsChangedEmitted.contains(id))
            m_contactsChangedEmitted.removeOne(id);
        else
            emit contactChanged(id); //group is a contact
        break;
	default:
		break; // ignore other events
	}
}

/*!
 * Private leaving implementation for contact()
 */
QContact QContactSymbianEngineData::contactL(const QContactLocalId &localId) const
{
    // A contact with a zero id is not expected to exist.
    // Symbian contact database uses id 0 internally as the id of the
    // system template.
    if(localId == 0)
        User::Leave(KErrNotFound);

    // Read the contact from the CContactDatabase
	CContactItem* symContact = m_contactDatabase->ReadContactL(localId);
	CleanupStack::PushL(symContact);

	// Convert to a QContact
	QContact contact = m_transformContact->transformContactL(*symContact, *m_contactDatabase);

	// Convert id
    QContactId contactId;
    contactId.setLocalId(localId);
    contactId.setManagerUri(m_managerUri);
    contact.setId(contactId);

    CleanupStack::PopAndDestroy(symContact);

	return contact;
}

/*!
 * Private leaving implementation for count()
 */
int QContactSymbianEngineData::countL() const
{
	// Call CountL and return number of contacts in database
	return m_contactDatabase->CountL();
}

/*!
 * Private leaving implementation for addContact()
 *
 * \param contact The contact item to save in the database.
 * \return The new contact ID.
 */
int QContactSymbianEngineData::addContactL(QContact &contact)
{
    CContactItem* contactItem(0);
    int id(0);

    //handle normal contact
    if(contact.type() == QContactType::TypeContact)
    {
        // Create a new contact card.
        contactItem = CContactCard::NewLC();
        m_transformContact->transformContactL(contact, *contactItem);
        // Add to the database
        id = m_contactDatabase->AddNewContactL(*contactItem);
        CleanupStack::PopAndDestroy(contactItem);

        // Update the changed values to the QContact
        // id
        QContactId contactId;
        contactId.setLocalId(id);
        contactId.setManagerUri(m_managerUri);
        contact.setId(contactId);
        contactItem = m_contactDatabase->ReadContactLC(id);
        // Guid
        QContactDetail* detail = m_transformContact->transformGuidItemFieldL(*contactItem, *m_contactDatabase);
        contact.saveDetail(detail);
        // Timestamp
        detail = m_transformContact->transformTimestampItemFieldL(*contactItem, *m_contactDatabase);
        contact.saveDetail(detail);
        CleanupStack::PopAndDestroy(contactItem);
    }
    //group contact
    else if(contact.type() == QContactType::TypeGroup)
    {
        // Create a new group, which is added to the database
        contactItem = m_contactDatabase->CreateContactGroupLC();

        //set the id for the contact, needed by update
        id = contactItem->Id();
        QContactId contactId;
        contactId.setLocalId(QContactLocalId(id));
        contactId.setManagerUri(m_managerUri);
        contact.setId(contactId);

        //update contact, will add the fields to the already saved group
        updateContactL(contact);

        CleanupStack::PopAndDestroy(contactItem);
    }

    //else{ should this leave? }

    // Return the new ID.
	return id;
}

/*!
 * Private leaving implementation for updateContact()
 *
 * \param contact The contact to update in the database.
 */
void QContactSymbianEngineData::updateContactL(QContact &contact)
{
    // Need to open the contact for write, leaving this item
    // on the cleanup stack to unlock the item in the event of a leave.
    CContactItem* contactItem = m_contactDatabase->OpenContactLX(contact.localId());
    CleanupStack::PushL(contactItem);

    // Copy the data from QContact to CContactItem
    m_transformContact->transformContactL(contact, *contactItem);

    // Write the entry using the converted  contact
    // note commitContactL removes empty fields from the contact
    m_contactDatabase->CommitContactL(*contactItem);

    // retrieve the contact in case of empty fields that have been removed, this could also be handled in transformcontact.
    contact = contactL(contact.localId());

    // Update group memberships to contact database
    //updateMemberOfGroupsL(contact);

    CleanupStack::PopAndDestroy(contactItem);
    CleanupStack::PopAndDestroy(1); // commit lock
}

/*!
 * Private leaving implementation for removeContact
 */
int QContactSymbianEngineData::removeContactL(QContactLocalId id)
{
    // A contact with a zero id is not expected to exist.
    // Symbian contact database uses id 0 internally as the id of the
    // system template.
	if(id == 0)
	    User::Leave(KErrNotFound);

    //TODO: in future QContactLocalId will be a class so this will need to be changed.
    TContactItemId cId = static_cast<TContactItemId>(id);

    //TODO: add code to remove all relationships.

	m_contactDatabase->DeleteContactL(cId);


	return 0;
}

