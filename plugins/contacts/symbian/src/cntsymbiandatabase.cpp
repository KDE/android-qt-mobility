/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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
//system includes
#include <e32base.h>
#include <s32mem.h>

//user includes
#include "cntsymbiandatabase.h"
#include "cntsymbiantransformerror.h"
#include "qcontactchangeset.h"
#include "qcontactmanagerengine.h"

// Constant
typedef QPair<QContactLocalId, QContactLocalId> QOwnCardPair;

CntSymbianDatabase::CntSymbianDatabase(QContactManagerEngine *engine, QContactManager::Error& error) :
    m_contactDatabase(0),
    m_currentOwnCardId(0)
{
    TRAPD(err, m_contactDatabase = CContactDatabase::OpenL())

    //Database not found, create it
    if(err == KErrNotFound)
    {
        TRAP(err, m_contactDatabase = CContactDatabase::CreateL())
    }

    //Database opened successfully
    if (err == KErrNone)
    {
    // In pre 10.1 platforms the AddObserverL & RemoveObserver functions are not
    // exported so we need to use CContactChangeNotifier.
#ifndef SYMBIAN_BACKEND_USE_SQLITE
        TRAP(err, m_contactChangeNotifier = CContactChangeNotifier::NewL(*m_contactDatabase, this));
#else
        TRAP(err, m_contactDatabase->AddObserverL(*this));
#endif
        if (err == KErrNone)
            m_engine = engine;

        // Read current own card id (self contact id)
        TContactItemId myCard = m_contactDatabase->OwnCardId();
        if (myCard > 0)
            m_currentOwnCardId = QContactLocalId(myCard);
    }
    CntSymbianTransformError::transformError(err, error);
}

CntSymbianDatabase::~CntSymbianDatabase()
{
    m_engine = NULL;
#ifndef SYMBIAN_BACKEND_USE_SQLITE
    delete m_contactChangeNotifier;
#else
    if (m_contactDatabase != 0) {
        m_contactDatabase->RemoveObserver(*this);
    }
#endif
    delete m_contactDatabase;
}

CContactDatabase* CntSymbianDatabase::contactDatabase()
{
    return m_contactDatabase;
}

void CntSymbianDatabase::appendContactsEmitted(const QList<QContactLocalId> &contactList)
{
    m_contactsEmitted += contactList;
}

void CntSymbianDatabase::appendContactEmitted(QContactLocalId id)
{
    m_contactsEmitted.append(id);
}

/*!
 * Respond to a contacts database event, delegating this event to
 * an appropriate signal as required.
 *
 * \param aEvent Contacts database event describing the change to the
 *  database.
 */
void CntSymbianDatabase::HandleDatabaseEventL(TContactDbObserverEvent aEvent)
{
    QContactChangeSet changeSet;
    TContactItemId id = aEvent.iContactId;

    switch (aEvent.iType)
    {
    case EContactDbObserverEventContactAdded:
        if(m_contactsEmitted.contains(id))
            m_contactsEmitted.removeOne(id);
        else
            changeSet.addedContacts().insert(id);
        break;
    case EContactDbObserverEventOwnCardDeleted:
    case EContactDbObserverEventContactDeleted:
        if(m_contactsEmitted.contains(id))
            m_contactsEmitted.removeOne(id);
        else
            changeSet.removedContacts().insert(id);
        break;
    case EContactDbObserverEventContactChanged:
        if(m_contactsEmitted.contains(id))
            m_contactsEmitted.removeOne(id);
        else
            changeSet.changedContacts().insert(id);
        break;
    case EContactDbObserverEventGroupAdded:
        if(m_contactsEmitted.contains(id))
            m_contactsEmitted.removeOne(id);
        else
            changeSet.addedRelationshipsContacts().insert(id);
        break;
    case EContactDbObserverEventGroupDeleted:
        if(m_contactsEmitted.contains(id))
            m_contactsEmitted.removeOne(id);
        else
            changeSet.removedRelationshipsContacts().insert(id);
        break;
    case EContactDbObserverEventGroupChanged:
        if(m_contactsEmitted.contains(id))
            m_contactsEmitted.removeOne(id);
        else
            changeSet.changedContacts().insert(id); //group is a contact
        break;
    case EContactDbObserverEventOwnCardChanged:
        {
            QOwnCardPair ownCard(m_currentOwnCardId, QContactLocalId(id));
            changeSet.oldAndNewSelfContactId() = ownCard;
            m_currentOwnCardId = QContactLocalId(id);
            break;
        }
    default:
        break; // ignore other events
    }

    changeSet.emitSignals(m_engine);
}






