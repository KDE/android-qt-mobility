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

#ifndef QCONTACTSYMBIANENGINEDATA_H
#define QCONTACTSYMBIANENGINEDATA_H

#include <QObject>
#include <QSet>

#include <qtcontacts.h>

#include <cntdb.h>
#include <cntdbobs.h>
#include <cntitem.h>

#include "qtcontactsglobal.h"
#include "qcontactmanager.h"

#include "cnttransformcontact.h"
#include "qabstractcontactfilter.h"



class QContactChangeSet;
class QAbstractContactFilter;
class QAbstractContactSorter;
class CntRelationship;

class QContactSymbianEngineData : public QObject,
							   public MContactDbObserver
{
	Q_OBJECT

public:
    QContactSymbianEngineData(QContactManager::Error& error);
    virtual ~QContactSymbianEngineData();

public:
    /* Access */
    QList<QContactLocalId> contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const;
	QAbstractContactFilter::FilterSupport filterSupported(const QContactFilter& filter) const;
	bool sortOrderSupported(const QList<QContactSortOrder>& sortOrders) const;
    QContact contact(const QContactLocalId& contactId, QContactManager::Error& qtError) const;
    QList<QContactLocalId> contacts(const QList<QContactSortOrder>& sortOrders, QContactManager::Error& qtError) const;
    int count() const;

    /* Add/Update/Remove */
    bool addContact(QContact& contact, QContactChangeSet& changeSet, QContactManager::Error& qtError);
    bool updateContact(QContact& contact, QContactChangeSet& changeSet, QContactManager::Error& qtError);
    bool removeContact(const QContactLocalId &id, QContactChangeSet& changeSet, QContactManager::Error& qtError);

    QList<QContactRelationship> relationships(const QString& relationshipType, const QContactId& participantId, QContactRelationshipFilter::Role role, QContactManager::Error& error) const;
    bool saveRelationship(QContactChangeSet *changeSet, QContactRelationship* relationship, QContactManager::Error& error);
    QList<QContactManager::Error> saveRelationships(QContactChangeSet *changeSet, QList<QContactRelationship>* relationships, QContactManager::Error& error);
    bool removeRelationship(QContactChangeSet *changeSet, const QContactRelationship& relationship, QContactManager::Error& error);
    QList<QContactManager::Error> removeRelationships(QContactChangeSet *changeSet, const QList<QContactRelationship>& relationships, QContactManager::Error& error);

    /* "Self" contact id (MyCard) */
    bool setSelfContactId(const QContactLocalId& contactId, QContactManager::Error& qtError);
    QContactLocalId selfContactId(QContactManager::Error& qtError) const;

public:
	// From MContactDbObserver
	void HandleDatabaseEventL(TContactDbObserverEvent aEvent);

signals:
    // Database change notifications
    void contactAdded(const QContactLocalId &contactId);
    void contactRemoved(const QContactLocalId &contactId);
    void contactChanged(const QContactLocalId &contactId);
    void relationshipAdded(const QContactLocalId &contactId);
    void relationshipRemoved(const QContactLocalId &contactId);
    
public:
    // Utility function to convert symbian error to QContactManager error
    static void transformError(TInt symbianError, QContactManager::Error& qtError);

private:

	// Leaving functions implementing CNTMODEL interaction.
    QContact contactL(const QContactLocalId &id) const;
	int countL() const;

	int addContactL(QContact &contact);
	void updateContactL(QContact &contact);
    int removeContactL(QContactLocalId id);
   
private:
	CContactDatabase* m_contactDatabase;
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
	CContactChangeNotifier* m_contactChangeNotifier;
#endif
        
    QList<QContactLocalId> m_contactsAddedEmitted;
    QList<QContactLocalId> m_contactsChangedEmitted;
    QList<QContactLocalId> m_contactsRemovedEmitted;
    TransformContact       *m_transformContact;
    QAbstractContactFilter* m_contactFilter;
    QAbstractContactSorter* m_contactSorter;
    CntRelationship        *m_relationship;
};

#endif
