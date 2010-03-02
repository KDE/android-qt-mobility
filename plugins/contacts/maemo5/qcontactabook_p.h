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

#ifndef QCONTACTABOOK_P_H
#define QCONTACTABOOK_P_H

#include <QObject>

#include "qtcontacts.h"

#include <libosso-abook/osso-abook-init.h>
#include <libosso-abook/osso-abook-types.h>
#include <libosso-abook/osso-abook-waitable.h>
#include <libosso-abook/osso-abook-presence.h>
#include <libosso-abook/osso-abook-avatar.h>
#include <libmcclient/mc-profile.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <libmcclient/mc-account.h>

extern "C" {
	struct _OssoABookContact {
		EContact parent;
	};
	
	OssoABookRoster*    osso_abook_roster_new               (const char *name,
                                                                 EBookView *book_view,
                                                                 const char *vcard_field);
	gboolean            osso_abook_roster_is_running        (OssoABookRoster *roster);
	void                osso_abook_roster_start             (OssoABookRoster *roster);

	gboolean            osso_abook_contact_has_valid_name   (OssoABookContact *contact);
	gboolean            osso_abook_contact_is_roster_contact(OssoABookContact *contact);

	OssoABookRoster*    osso_abook_aggregator_get_default   (GError **error);
	GList*              osso_abook_aggregator_list_master_contacts
                                                                (OssoABookAggregator *aggregator);
	unsigned            osso_abook_aggregator_get_master_contact_count
                                                                (OssoABookAggregator *aggregator);
	GList*              osso_abook_aggregator_lookup        (OssoABookAggregator *aggregator,
                                                                 const char *uid);
	const char*         osso_abook_contact_get_uid          (OssoABookContact *contact); 
	EBook*              osso_abook_roster_get_book          (OssoABookRoster *roster);
	char*               osso_abook_contact_to_string        (OssoABookContact *contact,
                                                                 EVCardFormat format,
                                                                 gboolean inline_avatar);
	char*               osso_abook_contact_get_value        (EContact *contact,
                                                                 const char *attr_name);
        GList*              osso_abook_aggregator_find_contacts (OssoABookAggregator *aggregator,
                                                                 EBookQuery *query);
        GList*              osso_abook_contact_get_values       (EContact *contact,
                                                                 const char *attr_name);
        GList*              osso_abook_contact_get_attributes   (EContact *contact,
                                                                 const char *attr_name);
	GList*              osso_abook_contact_get_roster_contacts
                                                                (OssoABookContact *master_contact);
	McProfile*          osso_abook_contact_get_profile      (OssoABookContact *contact);
	
	//TEST
	GList*              osso_abook_aggregator_find_contacts (OssoABookAggregator *aggregator,
                                                                 EBookQuery *query);
	const char*         osso_abook_contact_get_display_name (OssoABookContact *contact);
	GdkPixbuf*          osso_abook_contact_get_avatar_pixbuf
                                                                (OssoABookContact *contact,
                                                                 const char *username,
                                                                 const char *vcard_field);
	McAccount*          osso_abook_contact_get_account      (OssoABookContact *contact);
	GdkPixbuf*          osso_abook_avatar_get_image_rounded (OssoABookAvatar *avatar,
                                                                 int width,
                                                                 int height,
                                                                 gboolean crop,
                                                                 int radius,
                                                                 const guint8 border_color[4]);
}

QTM_USE_NAMESPACE

/* QContactIDsHash stores abookContact IDs (strings)*/
class QContactIDsHash{
public:
  QContactIDsHash(): key(1){};
  
  /* Append */
  QContactIDsHash& operator<< (const QByteArray& eContactId){ m_localIds[key] = eContactId; key++; return (*this); };
  
  /* Find */
  const char* operator[] (const QContactLocalId localId) { return m_localIds.value(localId).constData(); };
  const QContactLocalId operator[] (const QByteArray& eContactId) { return m_localIds.key(eContactId, 0); };

  /* Remove */ //TEST Remove functions not used nor tested yet
  bool remove(const QContactLocalId localId){ return (m_localIds.remove(localId) == 1) ? true : false;};
  bool remove(const QByteArray& eContactId){ const QContactLocalId hashKey= m_localIds.key(eContactId, 0); return remove(hashKey); };
  
private:
  uint key; //LocalID
  QHash<QContactLocalId, QByteArray> m_localIds; //[int/QContactLocalId Maemo5LocalId, QByteArray eContactID]
};

class QContactABook : public QObject
{
  Q_OBJECT
  
public:  
  QContactABook(QObject* parent = 0);
  virtual ~QContactABook();
  
  QList<QContactLocalId> contactIds(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const;
  //QList<QContactLocalId> contactIds(const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const;
  QContact* contact(const QContactLocalId& contactId, QContactManager::Error& error) const;
  bool removeContact(const QContactLocalId& contactId, QContactManager::Error& error);

private:
  void initAddressBook();
  void initLocalIdHash();
  
  bool setDetailValues(const QVariantMap& data, QContactDetail* detail) const;
  
  /* Searching */
  EBookQuery* convert(const QContactFilter& filter) const;
  
  /* eContact/abookContact to QContact methods */
  QContact* convert(EContact *eContact) const;

  QContactId createContactId(EContact *eContact) const;
  QList<QContactAddress*> createAddressDetail(EContact *eContact) const;
  QContactName* createNameDetail(EContact *eContact) const;
  QContactNickname* createNicknameDetail(EContact *eContact) const;
  QList<QContactEmailAddress*> createEmailDetail(EContact *eContact) const;
  //QContactAnniversary*  createAnniversaryDetail(EContact *eContact) const; NOT SUPPORTED BY MAEMO5
  QContactAvatar* createAvatarDetail(EContact *eContact) const;
  QContactBirthday* createBirthdayDetail(EContact *eContact) const;
  QContactGender* createGenderDetail(EContact *eContact) const;
  QContactGuid* createGuidDetail(EContact *eContact) const;
  QContactNote* createNoteDetail(EContact *eContact) const;
  QList<QContactOnlineAccount*> createOnlineAccountDetail(EContact *eContact) const;
  QContactOrganization* createOrganizationDetail(EContact *eContact) const;
  QList<QContactPhoneNumber*> createPhoneDetail(EContact *eContact) const; 
  QContactTimestamp* createTimestampDetail(EContact *eContact) const; 
  QContactUrl* createUrlDetail(EContact *eContact) const;
  
  /* Internal Vars */
  OssoABookAggregator *m_abookAgregator;
  mutable QContactIDsHash m_localIds; //Converts QLocalId <=> eContactId
};

#endif
