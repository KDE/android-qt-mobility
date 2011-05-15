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
#include <iostream>
#include <QEventLoop>

#include "contactsjni.h"
#include "qcontactandroidbackend_p.h"
#include "qcontactabook_p.h"

/* QContactABook */
QContactABook::QContactABook(QObject* parent) :QObject(parent)
{
    initAddressBook();
}

QContactABook::~QContactABook()
{
    QList<QContactLocalId> allIds  = m_qContactsmap.keys();
    foreach(const QContactLocalId& currId,allIds)
    {
        QContact* deleteContact = m_qContactsmap.value(currId);
        m_qContactsmap.remove(currId);
        delete(deleteContact);
    }
}

void QContactABook::initAddressBook()
{
    QMap<QContactLocalId,QtContactsJNI::Contacts> contactsMap;
    contactsMap = QtContactsJNI::initAndroidContacts();
    QList<QContactLocalId> allIds = contactsMap.keys();
    QANDROID_DEBUG<<"Converting Android contact into QContact";

    foreach(const QContactLocalId& currId, allIds)
    {
        QtContactsJNI::Contacts contact = contactsMap.value(currId);
        QContact* qContact = convert(contact);
        QContactId cId;
        cId.setLocalId(currId);
        qContact->setId(cId);
        m_qContactsmap.insert(currId,qContact);
    }
    QANDROID_DEBUG<<"Address Book Initilized and Mapping Contacts is Done";

}
//TODO Use native filters
QList<QContactLocalId> QContactABook::contactIds(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error* error) const
{
    Q_UNUSED(sortOrders)
    Q_UNUSED(filter);

    *error = QContactManager::NoError;
    return m_qContactsmap.keys();
}

QContact* QContactABook::getQContact(const QContactLocalId& contactId, QContactManager::Error* error) const
{
    QContact *qtContact=NULL;
    qtContact = m_qContactsmap.value(contactId);
    *error = QContactManager::NoError;

    if(!qtContact)
    {
        *error = QContactManager::DoesNotExistError;
    }
    return qtContact;
}

bool QContactABook::removeContact(const QContactLocalId& contactId, QContactManager::Error* error)
{

    int deleted = QtContactsJNI::removeFromAndroidContacts(contactId);
    bool retValue = true;
    if(deleted)
    {
        QContact* deleteContact = m_qContactsmap.value(contactId);
        m_qContactsmap.remove(contactId);
        delete deleteContact;
        *error = QContactManager::NoError;
        _contactsRemoved(QList<QContactLocalId>() << contactId);
    }
    else
    {
        *error = QContactManager::UnspecifiedError;
        QANDROID_DEBUG<<"Contacts Deletion Failed";
        retValue = false;
    }
    return retValue;
}

bool QContactABook::saveContact(QContact* contact, QContactManager::Error* error)
{
    QtContactsJNI::Contacts savingContact;
    QList<QContactDetail> allDetails = contact->details();

    foreach(const QContactDetail &detail, allDetails){
        QString definitionName = detail.definitionName();
        QANDROID_DEBUG << "Saving" << definitionName;
        if (definitionName == QContactAddress::DefinitionName){
            setAddressDetail(savingContact, detail);
        }else
            if (definitionName == QContactAnniversary::DefinitionName){
            setAnniversaryDetail(savingContact, detail);
        }else
            if (definitionName == QContactBirthday::DefinitionName){
            setBirthdayDetail(savingContact, detail);
        }else
            if (definitionName == QContactEmailAddress::DefinitionName){
            setEmailDetail(savingContact, detail);
        }else
            if (definitionName == QContactName::DefinitionName){
            setNameDetail(savingContact, detail);
        } else
            if (definitionName == QContactNickname::DefinitionName){
            setNicknameDetail(savingContact, detail);
        } else
            if (definitionName == QContactNote::DefinitionName){
            setNoteDetail(savingContact, detail);
        } else
            if (definitionName == QContactOnlineAccount::DefinitionName){
            setOnlineAccountDetail(savingContact, detail);
        }else
            if (definitionName == QContactOrganization::DefinitionName){
            setOrganizationDetail(savingContact, detail);
        } else
            if (definitionName == QContactPhoneNumber::DefinitionName){
            setPhoneDetail(savingContact, detail);
        }else
            if (definitionName == QContactUrl::DefinitionName){
            setUrlDetail(savingContact, detail);
        }
    }

    QContactLocalId lid = contact->localId();
    QANDROID_DEBUG << "Converting QContact id:" << lid << " to AndroidContact";
    if (lid) {
        QtContactsJNI::updateInAndroidContacts(lid,savingContact);
        QContact *updatedContact = new QContact(*contact);
        QContact *oldContact = m_qContactsmap.value(lid);
        m_qContactsmap[lid] = updatedContact;
        delete oldContact;
        *error = QContactManager::NoError;
        _contactsChanged(QList<QContactLocalId>() << lid);
    }
    else
    {
        QString id =QtContactsJNI::saveToAndroidContacts(savingContact);
        if(id.isEmpty())
        {
            QANDROID_DEBUG<<"Saving in Android Contacts Failed";
            *error = QContactManager::UnspecifiedError;
            return false;
        }
        else
        {
            QContact *newContact = new QContact(*contact);
            QContactId cId;
            cId.setLocalId(id.toInt(0,10));
            newContact->setId(cId);
            m_qContactsmap.insert(id.toInt(0,10),newContact);
            *error = QContactManager::NoError;
            _contactsAdded(QList<QContactLocalId>() << id.toInt());

        }
    }
    return true;
}

const QString QContactABook::getDisplayName(const QContact& contact) const
{
    QContactLocalId lid = contact.localId();
    if(lid)
    {
        QContactDetail  detail = contact.detail(QContactDisplayLabel::DefinitionName);
        QString displayName = detail.value(QContactDisplayLabel::FieldLabel);
        return displayName;
    }
    else
    {
        return QString();
    }
}
//FIX ME Currently We didn't implement selfContactId as we couldn't find corresponding detail in Android
QContactLocalId QContactABook::selfContactId(QContactManager::Error* errors) const
{
    QContactLocalId id=0;
    *errors = QContactManager::NoError;
    return id;
}

bool QContactABook::setDetailValues(const QVariantMap& data, QContactDetail* detail) const
{
    QMapIterator<QString, QVariant> i(data);
    QVariant value;
    while (i.hasNext()) {
        i.next();
        value = i.value();

        if (value.isNull())
        {
            continue;
        }
        if (value.canConvert<QString>() && value.toString().isEmpty())
        {
            continue;
        }
        detail->setValue(i.key(), i.value());
    }

    if (detail->isEmpty())
    {
        return false;
    }
    return true;
}

QContact* QContactABook::convert(QtContactsJNI::Contacts & contact) const
{
    QContact *contactData = new QContact();
    QList<QContactDetail*> detailList;

    QList<QContactAddress*> addressList = getAddressDetail(contact);
    QContactAddress* address;
    foreach(address, addressList)
        detailList << address;

    detailList << getBirthdayDetail(contact);

    detailList << getAnniVersaryDetail(contact);

    QList<QContactEmailAddress*> emailList = getEmailDetail(contact);
    QContactEmailAddress* email;
    foreach(email, emailList)
        detailList << email;

    detailList << getGuidDetail(contact);

    detailList << getNameDetail(contact);

    detailList << getNicknameDetail(contact);

    detailList << getNoteDetail(contact);

    QList<QContactOrganization*> orgList= getOrganizationDetail(contact);
    QContactOrganization* org;
    foreach(org,orgList)
        detailList << org;

    QList<QContactOnlineAccount*> onlineAccounts;
    QList<QContactPresence*> presences;
    getOnlineAccountAndPresenceDetails(contact, onlineAccounts, presences);

    QList<QContactPhoneNumber*> phoneNumberList = getPhoneDetail(contact);
    QContactPhoneNumber* phoneNumber;
    foreach(phoneNumber, phoneNumberList)
        detailList << phoneNumber;

    QList<QContactUrl*> urlList = getUrlDetail(contact);
    QContactUrl *url;
    foreach(url, urlList)
        detailList << url;

    bool ok;
    QContactDetail* detail;

    foreach(detail, detailList){
        if (detail->isEmpty()){
            delete detail;
            continue;
        }

        ok = contactData->saveDetail(detail);
        if (!ok){
            delete detail;
            continue;
        }
        delete detail;
    }
    QString displayLabel = contact.m_displayName;
    QContactManagerEngine::setContactDisplayLabel(contactData,displayLabel);

    return contactData;
}

QContactName* QContactABook::getNameDetail(QtContactsJNI::Contacts & contact) const
{
    QContactName* rtn = new QContactName;
    QVariantMap map;

    if(contact.m_names.m_firstName.isEmpty() &&
       contact.m_names.m_lastName.isEmpty() &&
       contact.m_names.m_middleName.isEmpty() &&
       contact.m_names.m_prefix.isEmpty() &&
       contact.m_names.m_suffix.isEmpty())
    {
        return rtn;
    }

    if(!contact.m_names.m_firstName.isEmpty())
    {
        map[QContactName::FieldFirstName] = contact.m_names.m_firstName;
    }

    if(!contact.m_names.m_lastName.isEmpty())
    {
        map[QContactName::FieldLastName] = contact.m_names.m_lastName;
    }

    if(!contact.m_names.m_middleName.isEmpty())
    {
        map[QContactName::FieldMiddleName] = contact.m_names.m_middleName;
    }

    if(!contact.m_names.m_prefix.isEmpty())
    {
        map[QContactName::FieldPrefix] = contact.m_names.m_prefix;
    }

    if(!contact.m_names.m_suffix.isEmpty())
    {
        map[QContactName::FieldSuffix] = contact.m_names.m_suffix;
    }

    setDetailValues(map, rtn);
    return rtn;
}

QList<QContactPhoneNumber*> QContactABook::getPhoneDetail(QtContactsJNI::Contacts & contact) const
{
    QList<QContactPhoneNumber*> rtnList;

    QVariantMap map;


    int numbercount = contact.m_phonenumber.count();
    for(int i=0;i<numbercount;i++)
    {
        QContactPhoneNumber* phoneNumber = new QContactPhoneNumber;

        QtContactsJNI::PhoneNumber phoneNumberInfo = contact.m_phonenumber.takeFirst();
        QString number = phoneNumberInfo.m_number;
        if(number.isEmpty())
        {
            return rtnList;
        }
        switch(phoneNumberInfo.m_type)
        {
        case 1:
            phoneNumber->setContexts(QContactDetail::ContextHome);
            phoneNumber->setSubTypes(QContactPhoneNumber::SubTypeLandline);
            break;
        case 2:
            phoneNumber->setContexts(QContactDetail::ContextHome);
            phoneNumber->setSubTypes(QContactPhoneNumber::SubTypeMobile);
            break;
        case 3:
            phoneNumber->setContexts(QContactDetail::ContextWork);
            phoneNumber->setSubTypes(QContactPhoneNumber::SubTypeLandline);
            break;
        case 4:
            phoneNumber->setContexts(QContactDetail::ContextWork);
            phoneNumber->setSubTypes(QContactPhoneNumber::SubTypeFax);
            break;
        case 5:
            phoneNumber->setContexts(QContactDetail::ContextHome);
            phoneNumber->setSubTypes(QContactPhoneNumber::SubTypeFax);
            break;
        case 6:
            phoneNumber->setContexts(QContactDetail::ContextHome);
            phoneNumber->setSubTypes(QContactPhoneNumber::SubTypePager);
            break;
        case 10:
            phoneNumber->setContexts(QContactDetail::ContextHome);
            phoneNumber->setSubTypes(QContactPhoneNumber::SubTypeCar);
            break;
        case 11:
            phoneNumber->setContexts(QContactDetail::ContextWork);
            phoneNumber->setSubTypes(QContactPhoneNumber::SubTypeLandline);
            break;
        case 14:
            phoneNumber->setContexts(QContactDetail::ContextOther);
            phoneNumber->setSubTypes(QContactPhoneNumber::SubTypeFax);
            break;
        case 18:
            phoneNumber->setContexts(QContactDetail::ContextWork);
            phoneNumber->setSubTypes(QContactPhoneNumber::SubTypeMobile);
            break;
        case 19:
            phoneNumber->setContexts(QContactDetail::ContextWork);
            phoneNumber->setSubTypes(QContactPhoneNumber::SubTypePager);
            break;
        case 20:
            phoneNumber->setContexts(QContactDetail::ContextHome);
            phoneNumber->setSubTypes(QContactPhoneNumber::SubTypeAssistant);
            break;
        case 21:
            phoneNumber->setContexts(QContactDetail::ContextHome);
            phoneNumber->setSubTypes(QContactPhoneNumber::SubTypeMessagingCapable);
            break;
        default:            //7,8,9,12,13,14,15,16,17
            phoneNumber->setContexts(QContactDetail::ContextOther);
        }

        map[QContactPhoneNumber::FieldNumber] = number;
        setDetailValues(map, phoneNumber);
        rtnList << phoneNumber;

    }
    return rtnList;
}


QList<QContactEmailAddress*> QContactABook::getEmailDetail(QtContactsJNI::Contacts & contact) const
{
    QList<QContactEmailAddress*> rtnList;


    QVariantMap map;

    int emailCount = contact.m_email.count();
    for(int i=0;i<emailCount;i++)
    {
        QContactEmailAddress *email = new QContactEmailAddress;

        QtContactsJNI::EmailData emailInfo = contact.m_email.takeFirst();
        QString emailId = emailInfo.m_email;

        if(emailId.isEmpty())
        {
            return rtnList;
        }
        switch(emailInfo.m_type)
        {
        case 1: //4
            email->setContexts(QContactDetail::ContextHome);
            break;
        case 2:
            email->setContexts(QContactDetail::ContextWork);
            break;
        default: //3,5
            email->setContexts(QContactDetail::ContextOther);
        }
        map[QContactEmailAddress::FieldEmailAddress] = emailId;
        setDetailValues(map, email);
        rtnList << email;
    }

    return rtnList;
}
//FIXME - Currently Service Provider Capabilities are Not Implemented
void QContactABook::getOnlineAccountAndPresenceDetails(QtContactsJNI::Contacts & contact,
                                                       QList<QContactOnlineAccount*>& onlineAccounts,
                                                       QList<QContactPresence*>& presences) const
{

    QVariantMap map;

    int onlineAccountCount = contact.m_onlineAccount.count();

    for(int i=0;i<onlineAccountCount;i++)
    {
        QContactOnlineAccount* onlineAccount = new QContactOnlineAccount;
        QContactPresence* contactPresence = new QContactPresence;

        QtContactsJNI::OnlineAccount onlineAccountInfo = contact.m_onlineAccount.takeFirst();
        if(onlineAccountInfo.m_acoountUri.isEmpty())
        {
            return;
        }
        switch(onlineAccountInfo.m_type)
        {
        case 1:
            onlineAccount->setContexts(QContactDetail::ContextHome);
            break;
        case 2:
            onlineAccount->setContexts(QContactDetail::ContextWork);
            break;
        default: //3,0
            onlineAccount->setContexts(QContactDetail::ContextOther);
        }


        QString serviceProvider;

        switch(onlineAccountInfo.m_protocol)
        {

        case 1:
            serviceProvider = "MSN";
            break;
        case 2:
            serviceProvider = "YAHOO";
            break;
        case 3:
            serviceProvider = "SKYPE";
            break;
        case 4:
            serviceProvider = "QQ";
            break;
        case 5:
            serviceProvider = "GTALK";
            break;
        case 6:
            serviceProvider = "ICQ";
            break;
        case 7:
            serviceProvider = "JABBER";
            break;
        case 8:
            serviceProvider = "NETMEETING";
            break;
        case -1:
            serviceProvider = onlineAccountInfo.m_customProtocol;
            break;
        default: //0
            serviceProvider = "AIM";

        }
        map[QContactOnlineAccount::FieldAccountUri] = onlineAccountInfo.m_acoountUri;
        map[QContactOnlineAccount::FieldServiceProvider] = serviceProvider;
        setDetailValues(map, onlineAccount);
        onlineAccounts << onlineAccount;

        QString presenceStatus;
        switch(onlineAccountInfo.m_presence)
        {
        case 0:
            presenceStatus = "OFFLINE";
            break;
        case 1:
            presenceStatus = "INVISIBLE";
            break;
        case 2:
            presenceStatus = "AWAY";
            break;
        case 3:
            presenceStatus = "IDLE";
            break;
        case 4:
            presenceStatus = "DO NOT DISTURB";
            break;
        default: //5
            presenceStatus = "AVAILABLE";
        }

        map.clear();
        map[QContactDetail::FieldLinkedDetailUris] = onlineAccountInfo.m_acoountUri;
        if(!onlineAccountInfo.m_status.isEmpty())
        {
            map[QContactPresence::FieldCustomMessage] = onlineAccountInfo.m_status;
        }
        map[QContactPresence::FieldPresenceStateText] = presenceStatus;
        QString timeStamp;
        timeStamp.setNum(onlineAccountInfo.m_timeStamp);
        map[QContactPresence::FieldTimestamp] = timeStamp;
        setDetailValues(map, contactPresence);
        presences << contactPresence;

    }

}

QContactNote* QContactABook::getNoteDetail(QtContactsJNI::Contacts & contact) const
{
    QContactNote* rtn = new QContactNote;
    QVariantMap map;
    if(contact.m_note.isEmpty())
    {
        return rtn;
    }
    map[QContactNote::FieldNote] =contact.m_note;

    setDetailValues(map, rtn);
    return rtn;
}

QList<QContactAddress*> QContactABook::getAddressDetail(QtContactsJNI::Contacts & contact) const
{
    QList<QContactAddress*> rtnList;

    QStringList addressFields;

    addressFields << QContactAddress::FieldPostOfficeBox
            << QContactAddress::FieldStreet
            << QContactAddress::FieldLocality
            << QContactAddress::FieldRegion
            << QContactAddress::FieldPostcode
            << QContactAddress::FieldCountry;

    QVariantMap map;
    int numberCount = contact.m_address.count();

    for(int i=0;i<numberCount;i++)
    {
        QContactAddress *address = new QContactAddress;
        QtContactsJNI::AddressData addressInfo = contact.m_address.takeFirst();


        if(addressInfo.m_pobox.isEmpty()&&addressInfo.m_street.isEmpty()&&addressInfo.m_city.isEmpty()&&addressInfo.m_region.isEmpty()&&addressInfo.m_postCode.isEmpty()&&addressInfo.m_country.isEmpty())
        {
            return rtnList;
        }
        switch(addressInfo.m_type)
        {
        case 1:
            address->setContexts(QContactDetail::ContextHome);
            break;
        case 2:
            address->setContexts(QContactDetail::ContextWork);
            break;
        case 3:
        case 4:
            address->setContexts(QContactDetail::ContextOther);
            break;
        }
        if(!addressInfo.m_pobox.isEmpty())
            map[addressFields[0]] = addressInfo.m_pobox;
        if(!addressInfo.m_street.isEmpty())
            map[addressFields[1]] = addressInfo.m_street;
        if(!addressInfo.m_city.isEmpty())
            map[addressFields[2]] = addressInfo.m_city;
        if(!addressInfo.m_region.isEmpty())
            map[addressFields[3]] = addressInfo.m_region;
        if(!addressInfo.m_postCode.isEmpty())
            map[addressFields[4]] = addressInfo.m_postCode;
        if(!addressInfo.m_country.isEmpty())
            map[addressFields[5]] = addressInfo.m_postCode;

        setDetailValues(map, address);
        rtnList << address;
    }
    return rtnList;
}

QList<QContactOrganization*> QContactABook::getOrganizationDetail(QtContactsJNI::Contacts & contact) const
{
    QList<QContactOrganization*> rtnList;
    QVariantMap map;

    int numberCount = contact.m_organization.count();
    for(int i=0;i<numberCount;i++)
    {
        QContactOrganization *organize = new QContactOrganization;
        QtContactsJNI::OrganizationalData orgInfo = contact.m_organization.takeFirst();
        if(orgInfo.m_organization.isEmpty()&&orgInfo.m_title.isEmpty())
        {
            return rtnList;
        }
        switch(orgInfo.m_type)
        {
        case 1:
            organize->setContexts(QContactDetail::ContextWork);
            break;
        case 2:
        case 3:
            organize->setContexts(QContactDetail::ContextOther);
            break;
        }

        map[QContactOrganization::FieldName] = orgInfo.m_organization;
        map[QContactOrganization::FieldTitle] = orgInfo.m_title;
        setDetailValues(map, organize);
        rtnList << organize;
    }
    return rtnList;
}

QList<QContactUrl*> QContactABook::getUrlDetail(QtContactsJNI::Contacts & contact) const
{

    QList<QContactUrl*> rtnList;
    QVariantMap map;
    int numberCount = contact.m_url.count();
    for(int i=0;i<numberCount;i++)
    {
        QContactUrl* url = new QContactUrl;
        QString urlInformation = contact.m_url.takeFirst();
        if(urlInformation.isEmpty())
        {
            return rtnList;
        }
        url->setContexts(QContactDetail::ContextHome);
        map[QContactUrl::FieldUrl] = urlInformation;
        setDetailValues(map, url);
        rtnList << url;
    }
    return rtnList;
}

QContactBirthday* QContactABook::getBirthdayDetail(QtContactsJNI::Contacts & contact) const
{
    QContactBirthday* rtn = new QContactBirthday;
    QVariantMap map;
    if(contact.m_birthday.isEmpty())
    {
        return rtn;
    }
    QStringList temp = contact.m_birthday.split(":", QString::SkipEmptyParts);
    QString sdate = temp.takeFirst();
    QString  smonth = temp.takeFirst();
    QString  syear = temp.takeFirst();
    bool ok;
    int date = sdate.toInt(&ok,10);
    int month = smonth.toInt(&ok,10);
    int year = syear.toInt(&ok,10);
    QDate birthday(year,month,date);
    map[QContactBirthday::FieldBirthday] = birthday;
    setDetailValues(map, rtn);
    return rtn;
}

QContactAnniversary* QContactABook::getAnniVersaryDetail(QtContactsJNI::Contacts & contact) const
{
    QContactAnniversary* rtn = new QContactAnniversary;
    QVariantMap map;
    if(contact.m_anniverasary.isEmpty())
    {
        return rtn;
    }
    QStringList temp = contact.m_anniverasary.split(":", QString::SkipEmptyParts);
    QString sdate = temp.takeFirst();
    QString  smonth = temp.takeFirst();
    QString  syear = temp.takeFirst();
    bool ok;
    int date = sdate.toInt(&ok,10);
    int month = smonth.toInt(&ok,10);
    int year = syear.toInt(&ok,10);
    QDate anniversaryDay(year,month,date);
    map[QContactAnniversary::FieldOriginalDate] =anniversaryDay;
    setDetailValues(map, rtn);
    return rtn;
}

QContactNickname* QContactABook::getNicknameDetail(QtContactsJNI::Contacts & contact) const
{
    QContactNickname* rtn = new QContactNickname;
    QVariantMap map;
    if(contact.m_nickName.isEmpty())
    {
        return rtn;
    }
    map[QContactNickname::FieldNickname] = contact.m_nickName;
    setDetailValues(map, rtn);
    return rtn;
}

QContactGuid* QContactABook::getGuidDetail(QtContactsJNI::Contacts & contact) const
{
    QContactGuid* rtn = new QContactGuid;
    QVariantMap map;
    if(contact.m_id.isEmpty())
    {
        return rtn;
    }
    map[QContactGuid::FieldGuid] = contact.m_id;

    setDetailValues(map, rtn);
    return rtn;
}

void QContactABook::setEmailDetail(QtContactsJNI::Contacts & contact, const QContactEmailAddress& detail)
{
    QString paramValues;
    QVariantMap vm = detail.variantValues();
    QMapIterator<QString, QVariant> i(vm);
    while (i.hasNext()) {
        i.next();
        QString key = i.key();
        QtContactsJNI::EmailData emailInfo;
        if (key == QContactDetail::FieldDetailUri)
        {
            continue;
        }
        emailInfo.m_type = 3;
        if (key == QContactDetail::FieldContext)
        {
            paramValues = i.value().toString().toUpper();
            continue;
        }
        else
        {
            emailInfo.m_email = i.value().toString();
        }
        if(paramValues == "Home")
        {
            emailInfo.m_type = 1;
        }
        else if(paramValues == "Work")
        {
            emailInfo.m_type = 2;
        }
        contact.m_email.append(emailInfo);
    }
}

void QContactABook::setNameDetail(QtContactsJNI::Contacts & contact, const QContactName& detail)
{
    QStringList attrValues;
    {
        QtContactsJNI::NameData names;
        QStringList supportedDetailValues;
        supportedDetailValues << QContactName::FieldLastName << QContactName::FieldFirstName << QContactName::FieldMiddleName<<QContactName::FieldPrefix<<QContactName::FieldSuffix;
        foreach(QString key, supportedDetailValues){
            if(key == QContactName::FieldFirstName)
            {
                names.m_firstName = detail.value(key);
                if(names.m_firstName.isEmpty())
                    names.m_firstName = "";
            }
            else if(key == QContactName::FieldLastName)
            {
                names.m_lastName = detail.value(key);
                if(names.m_lastName.isEmpty())
                    names.m_lastName = "";
            }
            else if(key == QContactName::FieldMiddleName)
            {
                names.m_middleName = detail.value(key);
                if(names.m_middleName.isEmpty())
                    names.m_middleName = "";
            }
            else if(key == QContactName::FieldPrefix)
            {
                names.m_prefix = detail.value(key);
                if(names.m_prefix.isEmpty())
                    names.m_prefix = "";
            }
            else if(key == QContactName::FieldSuffix)
            {
                names.m_suffix = detail.value(key);
                if(names.m_suffix.isEmpty())
                    names.m_suffix = "";
            }
            contact.m_names = names;
            attrValues << detail.value(key);
        }
        if (attrValues[1].isEmpty()){
            attrValues[1] = detail.customLabel();
        }
    }
    contact.m_displayName = attrValues[1];

}

void QContactABook::setPhoneDetail(QtContactsJNI::Contacts & contact, const QContactPhoneNumber& detail)
{
    QVariantMap vm = detail.variantValues();
    QMapIterator<QString, QVariant> i(vm);
    while (i.hasNext()) {
        i.next();
        const QString key = i.key();
        QtContactsJNI::PhoneNumber phoneNumberInfo;
        if (key == QContactDetail::FieldDetailUri)
        {
            continue;
        }
        phoneNumberInfo.m_type = 7;
        if (key == QContactDetail::FieldContext ||
            key == QContactPhoneNumber::FieldSubTypes){
            QString value = i.value().toString();
            if (value == QContactPhoneNumber::SubTypeMobile)
            {
                phoneNumberInfo.m_type = 1;
                continue;
            }
            else if( value == QContactPhoneNumber::SubTypePager)
            {
                phoneNumberInfo.m_type = 6;
                continue;
            }
            else
            {
                phoneNumberInfo.m_type= 7;
                continue;
            }
        } else
        {
            phoneNumberInfo.m_number=i.value().toString();
        }
        contact.m_phonenumber.append(phoneNumberInfo);
    }
}

void QContactABook::setAddressDetail(QtContactsJNI::Contacts & contact, const QContactAddress& detail)
{
    const uint nAddressElems = 7;
    QStringList adrAttrValues,
    paramValues;
    foreach(QString c, detail.contexts())
    {
        paramValues << c.toUpper();
    }
    for (uint i = 0; i < nAddressElems; ++i)
    {
        adrAttrValues << "";
    }
    QVariantMap vm = detail.variantValues();
    QMapIterator<QString, QVariant> i(vm);
    while (i.hasNext()) {
        i.next();
        int index = -1;
        QString key = i.key();
        if (key == QContactAddress::FieldPostOfficeBox)
        {
            index = 0;
        }
        else if (key == QContactAddress::FieldStreet)
        {
            index = 1;
        }
        else if (key == QContactAddress::FieldLocality)
        {
            index = 2;
        }
        else if (key == QContactAddress::FieldRegion)
        {
            index = 3;
        }
        else if (key == QContactAddress::FieldPostcode)
        {
            index = 4;
        }
        else if (key == QContactAddress::FieldCountry)
        {
            index = 5;
        }
        else if (key == QContactDetail::FieldContext)
        {
            index = 6;
            continue;
        }
        else if (key == QContactDetail::FieldDetailUri)
        {
            continue;
        }
        else {
            QANDROID_DEBUG << "Address contains an invalid field:" << key;
            return;
        }
        if (index != -1)
        {
            adrAttrValues[index] = i.value().toString();
        }
    }
    QtContactsJNI::AddressData addressInfo;
    QString temp;
    temp = adrAttrValues[0];
    if(temp.isEmpty())
        addressInfo.m_pobox ="";
    else
        addressInfo.m_pobox = temp;
    temp.clear();
    temp = adrAttrValues[1];
    if(temp.isEmpty())
        addressInfo.m_street = "";
    else
        addressInfo.m_street = temp;
    temp.clear();
    temp = adrAttrValues[2];
    if(temp.isEmpty())
        addressInfo.m_city = "";
    else
        addressInfo.m_city = temp;
    temp.clear();
    temp = adrAttrValues[3];
    if(temp.isEmpty())
        addressInfo.m_region = "";
    else
        addressInfo.m_region = temp;
    temp.clear();
    temp = adrAttrValues[4];
    if(temp.isEmpty())
        addressInfo.m_postCode = "";
    else
        addressInfo.m_postCode = temp;
    temp.clear();
    temp = adrAttrValues[5];
    if(temp.isEmpty())
        addressInfo.m_country = "";
    else
        addressInfo.m_country = temp;
    temp.clear();
    temp = adrAttrValues[6];
    if(temp.isEmpty())
        addressInfo.m_type = 3;
    else
    {
        if(temp == "Home")
        {
            addressInfo.m_type =1;
        }
        else if (temp == "Work")
        {
            addressInfo.m_type = 2;
        }
        else
        {
            addressInfo.m_type = 3;
        }
    }
    contact.m_address.append(addressInfo);

}
//TODO - Need to save time also
void QContactABook::setBirthdayDetail(QtContactsJNI::Contacts & contact, const QContactBirthday& detail)
{
    QString attrValues;
    QString androidFormat;
    attrValues = detail.value(QContactBirthday::FieldBirthday);
    QStringList temp = attrValues.split("-");
    int length = temp.count();
    if(length >=3)
    {
        for(int i=0;i<length;i++)
        {
            androidFormat.append(temp.takeFirst());
            if(i<2)
            {
                androidFormat.append(":");
            }
        }
        contact.m_birthday.append(androidFormat);
    }
    else
    {
        QANDROID_DEBUG<<"YOUR BIRTHDAY IS IN WRONG FROMAT";
        return;
    }
}
//TODO - Need to save time also
void QContactABook::setAnniversaryDetail(QtContactsJNI::Contacts & contact, const QContactAnniversary& detail)
{
    QString attrValues;
    QString androidFormat;
    attrValues = detail.value(QContactAnniversary::FieldOriginalDate);
    QStringList temp = attrValues.split("-");
    int length = temp.count();
    if(length >=3)
    {
        for(int i=0;i<length;i++)
        {
            androidFormat.append(temp.takeFirst());
            if(i<2)
            {
                androidFormat.append(":");
            }
        }
        contact.m_birthday.append(androidFormat);
    }
    else
    {
        QANDROID_DEBUG<<"YOUR ANNIVERSARY IS IN WRONG FROMAT";
        return;
    }
}

void QContactABook::setOrganizationDetail(QtContactsJNI::Contacts & contact, const QContactOrganization& detail)
{
    QString attrValues,title;
    attrValues = detail.value(QContactOrganization::FieldName);
    QtContactsJNI::OrganizationalData organizationInfo;
    if(attrValues.isEmpty())
    {
       organizationInfo.m_organization = "";
    }
    else
    {
        organizationInfo.m_organization = attrValues;
    }
    title = detail.value(QContactOrganization::FieldTitle);
    if(title.isEmpty())
    {
        organizationInfo.m_title = "";
    }
    else
    {
        organizationInfo.m_title = title;
    }
    organizationInfo.m_type = 1;
    contact.m_organization.append(organizationInfo);

}

void QContactABook::setNicknameDetail(QtContactsJNI::Contacts & contact, const QContactNickname& detail)
{
    contact.m_nickName = detail.value(QContactNickname::FieldNickname);
}

void QContactABook::setNoteDetail(QtContactsJNI::Contacts & contact, const QContactNote& detail)
{
    contact.m_note = detail.value(QContactNote::FieldNote);
}

void QContactABook::setUrlDetail(QtContactsJNI::Contacts & contact, const QContactUrl& detail)
{
    QString attrValues;
    QVariantMap vm = detail.variantValues();
    QMapIterator<QString, QVariant> i(vm);
    while (i.hasNext()) {
        i.next();
        const QString key = i.key();
        if (key == QContactDetail::FieldDetailUri)
        {
            continue;
        }
        if(key == QContactDetail::FieldContext)
        {
            continue;
        }
        attrValues = i.value().toString();
        contact.m_url.append(attrValues);
    }
}

/*NOTE: Online details comes from Telepathy or can be added manually by the user.
 *      OnlineDetails coming from Telepathy/Roster contacts can't be saved.
 */
void QContactABook::setOnlineAccountDetail(QtContactsJNI::Contacts & contact, const QContactOnlineAccount& detail) const
{
    Q_UNUSED(contact);
    Q_UNUSED(detail);
}
