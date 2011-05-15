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
#include "contactsjni.h"
#undef signals

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
  QContactName* getNameDetail(QtContactsJNI::Contacts & contact) const;
  QContact* convert(QtContactsJNI::Contacts & contact) const;
  QList<QContactPhoneNumber*> getPhoneDetail(QtContactsJNI::Contacts & eContact) const;
  QList<QContactEmailAddress*> getEmailDetail(QtContactsJNI::Contacts & contact) const;
  QContactNote* getNoteDetail(QtContactsJNI::Contacts & contact) const;
  QList<QContactAddress*> getAddressDetail(QtContactsJNI::Contacts & contact) const;
  QList<QContactOrganization*> getOrganizationDetail(QtContactsJNI::Contacts & contact) const;
  QList<QContactUrl*> getUrlDetail(QtContactsJNI::Contacts & contact) const;
  QContactBirthday* getBirthdayDetail(QtContactsJNI::Contacts & contact) const;
  QContactAnniversary* getAnniVersaryDetail(QtContactsJNI::Contacts & contact) const;
  QContactNickname* getNicknameDetail(QtContactsJNI::Contacts & contact) const;
  QContactGuid* getGuidDetail(QtContactsJNI::Contacts & contact) const;
  void getOnlineAccountAndPresenceDetails(QtContactsJNI::Contacts & contact,
                                                        QList<QContactOnlineAccount*>& onlineAccounts,
                                                        QList<QContactPresence*>& presences) const;

  void setEmailDetail(QtContactsJNI::Contacts & contact, const QContactEmailAddress& detail);
  void setNameDetail(QtContactsJNI::Contacts & contact, const QContactName& detail);
  void setPhoneDetail(QtContactsJNI::Contacts & contact, const QContactPhoneNumber& detail);
  void setAddressDetail(QtContactsJNI::Contacts & contact, const QContactAddress& detail);
  void setBirthdayDetail(QtContactsJNI::Contacts & contact, const QContactBirthday& detail);
  void setAnniversaryDetail(QtContactsJNI::Contacts & contact, const QContactAnniversary& detail);
  void setOrganizationDetail(QtContactsJNI::Contacts & contact, const QContactOrganization& detail);
  void setNicknameDetail(QtContactsJNI::Contacts & contact, const QContactNickname& detail);
  void setNoteDetail(QtContactsJNI::Contacts & contact, const QContactNote& detail);
  void setUrlDetail(QtContactsJNI::Contacts & contact, const QContactUrl& detail);
  void setOnlineAccountDetail(QtContactsJNI::Contacts & contact, const QContactOnlineAccount& detail) const;

Q_SIGNALS:
  void contactsAdded(const QList<QContactLocalId>& contactIds);
  void contactsChanged(const QList<QContactLocalId>& contactIds);
  void contactsRemoved(const QList<QContactLocalId>& contactIds);

public:
  /* Members used by callbacks */
  void _contactsAdded(const QList<QContactLocalId>& contactIds ){ emit contactsAdded(contactIds); }
  void _contactsRemoved(const QList<QContactLocalId>& contactIds ){ emit contactsRemoved(contactIds); }
  void _contactsChanged(const QList<QContactLocalId>& contactIds ){ emit contactsChanged(contactIds); }

private:
  void initAddressBook();
  bool setDetailValues(const QVariantMap& data, QContactDetail* detail) const;
  QMap<QContactLocalId,QContact*> m_qContactsmap;

 };

#endif // QCONTACTABOOK_P_H
