/****************************************************************************
**
** Copyright 2010 Elektrobit(EB)(http://www.elektrobit.com)
**
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
**
****************************************************************************/

#ifndef CONTACTSJNI_H
#define CONTACTSJNI_H

#include <QObject>
#include <jni.h>

#include "qtcontacts.h"

QTM_USE_NAMESPACE
namespace QtContactsJNI
{
    struct PhoneNumber
    {
        QString m_number;
        int m_type;
    };
    struct EmailData
    {
        QString m_email;
        int m_type;
    };
    struct OrganizationalData
    {
        QString m_organization;
        QString m_title;
        int m_type;
    };
    struct AddressData
    {
        QString m_pobox;
        QString m_street;
        QString m_city;
        QString m_region;
        QString m_postCode;
        QString m_country;
        int m_type;
    };
    struct NameData
    {
        QString m_firstName;
        QString m_lastName;
        QString m_middleName;
        QString m_prefix;
        QString m_suffix;
    };
    struct OnlineAccount
    {
        QString m_acoountUri;
        QString m_status;
        QString m_customProtocol;
        long m_timeStamp;
        int m_presence;
        int m_protocol;
        int m_type;
    };


    class Contacts
    {
    public:
            QString m_displayName;
            NameData m_names;
            QList<PhoneNumber> m_phonenumber;
            QList<EmailData> m_email;
            QString m_note;
            QList<AddressData> m_address;
            QList<OrganizationalData> m_organization;
            QList<OnlineAccount> m_onlineAccount;
            QString m_birthday;
            QString m_anniverasary;
            QString m_nickName;
            QList<QString> m_url;
            QString m_id;
     };


    QMap< QContactLocalId, Contacts> initAndroidContacts();
    QString saveToAndroidContacts(Contacts &contact);
    int removeFromAndroidContacts(int id);
    void updateInAndroidContacts(int id,Contacts &savingcontact);
    jobject constructAndroidContactObject(JNIEnv* env,Contacts& savingcontact);

    struct FieldID
    {
        jfieldID m_androidContacts_fid;
        jfieldID m_myContacts_fid;
        jfieldID m_displayName_fid;
        jfieldID m_nameData_fid;
        jfieldID m_firstName_fid;
        jfieldID m_lastName_fid;
        jfieldID m_middleName_fid;
        jfieldID m_prefix_fid;
        jfieldID m_suffix_fid;
        jfieldID m_phonenumberobjects_fid;
        jfieldID m_phonenumber_fid;
        jfieldID m_phonetype_fid;
        jfieldID m_emailDataObjects_fid;
        jfieldID m_email_fid;
        jfieldID m_emailtype_fid;
        jfieldID m_note_fid;
        jfieldID m_addressDataObject_fid;
        jfieldID m_pobox_fid;
        jfieldID m_street_fid;
        jfieldID m_city_fid;
        jfieldID m_region_fid;
        jfieldID m_postCode_fid;
        jfieldID m_country_fid;
        jfieldID m_addresstype_fid;
        jfieldID m_onlineAccount_fid;
        jfieldID m_onlineAccountUri_fid;
        jfieldID m_onlineAccountStatus_fid;
        jfieldID m_onlineAccountCustomProtocol_fid;
        jfieldID m_onlineAccountPresnce_fid;
        jfieldID m_onlineAccountProtocol_fid;
        jfieldID m_onlineAccountTimeStamp_fid;
        jfieldID m_onlineAccountType_fid;
        jfieldID m_organizationalData_fid;
        jfieldID m_organizaion_fid;
        jfieldID m_organizaiontitle_fid;
        jfieldID m_organizationtype_fid;
        jfieldID m_birthday_fid;
        jfieldID m_anniversary_fid;
        jfieldID m_nickName_fid;
        jfieldID m_url_fid;
        jfieldID m_id_fid;

    };


}


#endif // CONTACTSJNI_H
