/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QCONTACTABOOK_P_H
#define QCONTACTABOOK_P_H

#include <QObject>
#include <QMutex>

#include "qtcontacts.h"


#undef signals


#include "contactsjni.h"

QTM_USE_NAMESPACE

#define QANDROID_DEBUG qWarning()<<__FILE__<<__LINE__

class QContactABook : public QObject
{
  Q_OBJECT
  
public:  
  QContactABook(QObject* parent = 0);
  ~QContactABook();
  void callInitPhoneBook();
  
  QList<QContactLocalId> contactIds(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error* error) const;
  QContact* getQContact(const QContactLocalId& contactId, QContactManager::Error* error) const;
  bool saveContact(QContact* contact, QContactManager::Error* error);
  bool removeContact(const QContactLocalId& contactId, QContactManager::Error* error);

  const QString getDisplayName(const QContact& contact) const;

  QContactLocalId selfContactId(QContactManager::Error* errors) const;
  QContactName* getNameDetail(QtContactsJNI::MyContacts & contact) const;
  QContact* convert(QtContactsJNI::MyContacts & contact) const;
  QList<QContactPhoneNumber*> getPhoneDetail(QtContactsJNI::MyContacts & eContact) const;
  QList<QContactEmailAddress*> getEmailDetail(QtContactsJNI::MyContacts & contact) const;
  QContactNote* getNoteDetail(QtContactsJNI::MyContacts & contact) const;
  QList<QContactAddress*> getAddressDetail(QtContactsJNI::MyContacts & contact) const;
  QList<QContactOrganization*> getOrganizationDetail(QtContactsJNI::MyContacts & contact) const;
  QList<QContactUrl*> getUrlDetail(QtContactsJNI::MyContacts & contact) const;
  QContactBirthday* getBirthdayDetail(QtContactsJNI::MyContacts & contact) const;
  QContactAnniversary* getAnniVersaryDetail(QtContactsJNI::MyContacts & contact) const;
  QContactNickname* getNicknameDetail(QtContactsJNI::MyContacts & contact) const;
  QContactGuid* getGuidDetail(QtContactsJNI::MyContacts & contact) const;


Q_SIGNALS:
  void contactsAdded(const QList<QContactLocalId>& contactIds);
  void contactsChanged(const QList<QContactLocalId>& contactIds);
  void contactsRemoved(const QList<QContactLocalId>& contactIds);
  void jobSavingCompleted();
  void jobRemovingCompleted();
  
public:
  /* Members used by callbacks */
  void _contactsAdded(const QList<QContactLocalId>& contactIds ){ emit contactsAdded(contactIds); }
  void _contactsRemoved(const QList<QContactLocalId>& contactIds ){ emit contactsRemoved(contactIds); }
  void _contactsChanged(const QList<QContactLocalId>& contactIds ){ emit contactsChanged(contactIds); }
  void _jobSavingCompleted(){ emit jobSavingCompleted(); }
  void _jobRemovingCompleted(){ emit jobRemovingCompleted(); }
  
private:
  void initAddressBook();
  bool setDetailValues(const QVariantMap& data, QContactDetail* detail) const;
  QMap<QContactLocalId,QtContactsJNI::MyContacts> m_contactsMap;
  

 };

#endif
