#include<iostream>
#include <QPixmap>
#include <QEventLoop>
#include <QBuffer>
#include <QImageReader>

#include"contactsjni.h"
#include "qcontactandroidbackend_p.h"
#include "qcontactabook_p.h"

QContactABook::QContactABook(QObject* parent) :QObject(parent)
{
    initAddressBook();
}
QContactABook::~QContactABook()
{

}
void QContactABook::initAddressBook()
{
    m_contactsMap = QtContactsJNI::initAndroidContacts();
    QANDROID_DEBUG<<"Address Book Initilized and Mapping Contacts is Done";

}
void QContactABook::callInitPhoneBook()
{
    initAddressBook();
    _contactsChanged(m_contactsMap.keys());
}
//TODO Use native filters
QList<QContactLocalId> QContactABook::contactIds(const QContactFilter& filter,
                                                 const QList<QContactSortOrder>& sortOrders,
                                                 QContactManager::Error* error) const
{
    Q_UNUSED(sortOrders)
    Q_UNUSED(filter);
    *error = QContactManager::NoError;
    return m_contactsMap.keys();
}

QContact* QContactABook::getQContact(const QContactLocalId& contactId, QContactManager::Error* error) const
{
    QContact *qtContact=NULL;
    QtContactsJNI::MyContacts contact = m_contactsMap.value(contactId);
    QANDROID_DEBUG<<"Converting Android contact into QContact";
    qtContact = convert(contact);
    QContactId cId;
    cId.setLocalId(contactId);
    qtContact->setId(cId);
    return qtContact;
}

//TO-DO
bool QContactABook::removeContact(const QContactLocalId& contactId, QContactManager::Error* error)
{
    return true;
}

//TO-DO
bool QContactABook::saveContact(QContact* contact, QContactManager::Error* error)
{
    return true;
}

const QString QContactABook::getDisplayName(const QContact& contact) const
{
    QContactLocalId lid = contact.localId();
    if(lid)
    {
        QtContactsJNI::MyContacts androidContact = m_contactsMap.value(lid);
        return androidContact.m_displayName;
    }
    else
        return QString();
}

//FIX ME
QContactLocalId QContactABook::selfContactId(QContactManager::Error* errors) const
{
    QContactLocalId id;
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
        QANDROID_DEBUG << "Set" << i.key() << i.value();
        detail->setValue(i.key(), i.value());
    }

    if (detail->isEmpty())
    {
        return false;
    }
    return true;

}

QContact* QContactABook::convert(QtContactsJNI::MyContacts & contact) const
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
    return contactData;
}

QContactName* QContactABook::getNameDetail(QtContactsJNI::MyContacts & contact) const
{
    QContactName* rtn = new QContactName;
    QVariantMap map;
    if(contact.m_displayName.isEmpty())
    {
        return rtn;
    }
    map[QContactName::FieldCustomLabel] = contact.m_displayName;
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

QList<QContactPhoneNumber*> QContactABook::getPhoneDetail(QtContactsJNI::MyContacts & contact) const
{
    QList<QContactPhoneNumber*> rtnList;
    QVariantMap map;
    int numbercount = contact.m_phonenumber.count();
    for(int i=0;i<numbercount;i++)
    {
        QContactPhoneNumber* phoneNumber = new QContactPhoneNumber;
        QtContactsJNI::PhoneNumber phonenumberinfo = contact.m_phonenumber.takeFirst();
        QString number = phonenumberinfo.m_number;
        if(number.isEmpty())
        {
            return rtnList;
        }
        switch(phonenumberinfo.m_type)
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

QList<QContactEmailAddress*> QContactABook::getEmailDetail(QtContactsJNI::MyContacts & contact) const
{
    QList<QContactEmailAddress*> rtnList;
    QVariantMap map;
    int emailcount = contact.m_email.count();
    for(int i=0;i<emailcount;i++)
    {
        QContactEmailAddress *email = new QContactEmailAddress;

        QtContactsJNI::EmailData emailinfo = contact.m_email.takeFirst();
        QString emailid = emailinfo.m_email;

        if(emailid.isEmpty())
        {
            return rtnList;
        }
        switch(emailinfo.m_type)
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

        map[QContactEmailAddress::FieldEmailAddress] = emailid;
        setDetailValues(map, email);
        rtnList << email;
    }

    return rtnList;
}

QContactNote* QContactABook::getNoteDetail(QtContactsJNI::MyContacts & contact) const
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

QList<QContactAddress*> QContactABook::getAddressDetail(QtContactsJNI::MyContacts & contact) const
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
    int numbercount = contact.m_address.count();

    for(int i=0;i<numbercount;i++)
    {
        QContactAddress *address = new QContactAddress;
        QtContactsJNI::AddressData addressinfo = contact.m_address.takeFirst();
        if(addressinfo.m_pobox.isEmpty() &&
           addressinfo.m_street.isEmpty() &&
           addressinfo.m_city.isEmpty() &&
           addressinfo.m_region.isEmpty() &&
           addressinfo.m_postCode.isEmpty() &&
           addressinfo.m_country.isEmpty())
        {
            return rtnList;
        }

        switch(addressinfo.m_type)
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

        if(!addressinfo.m_pobox.isEmpty())
            map[addressFields[0]] = addressinfo.m_pobox;
        if(!addressinfo.m_street.isEmpty())
            map[addressFields[1]] = addressinfo.m_street;
        if(!addressinfo.m_city.isEmpty())
            map[addressFields[2]] = addressinfo.m_city;
        if(!addressinfo.m_region.isEmpty())
            map[addressFields[3]] = addressinfo.m_region;
        if(!addressinfo.m_postCode.isEmpty())
            map[addressFields[4]] = addressinfo.m_postCode;
        if(!addressinfo.m_country.isEmpty())
            map[addressFields[5]] = addressinfo.m_postCode;
        setDetailValues(map, address);
        rtnList << address;
    }
    return rtnList;
}

QList<QContactOrganization*> QContactABook::getOrganizationDetail(QtContactsJNI::MyContacts & contact) const
{
    QList<QContactOrganization*> rtnList;
    QVariantMap map;
    int numbercount = contact.m_organization.count();
    for(int i=0;i<numbercount;i++)
    {
        QContactOrganization *organize = new QContactOrganization;
        QtContactsJNI::OrganizationalData orginfo = contact.m_organization.takeFirst();
        if(orginfo.m_organization.isEmpty()&&orginfo.m_title.isEmpty())
        {
            return rtnList;
        }
        switch(orginfo.m_type)
        {
        case 1:
            organize->setContexts(QContactDetail::ContextWork);
            break;
        case 2:
        case 3:
            organize->setContexts(QContactDetail::ContextOther);
            break;
        }
        map[QContactOrganization::FieldName] = orginfo.m_organization;
        map[QContactOrganization::FieldTitle] = orginfo.m_title;
        setDetailValues(map, organize);
        rtnList << organize;
    }
    return rtnList;
}

QList<QContactUrl*> QContactABook::getUrlDetail(QtContactsJNI::MyContacts & contact) const
{
    QList<QContactUrl*> rtnList;
    QVariantMap map;
    int numbercount = contact.m_url.count();
    for(int i=0;i<numbercount;i++)
    {
        QContactUrl* url = new QContactUrl;
        QString urlinformation = contact.m_url.takeFirst();
        if(urlinformation.isEmpty())
        {
            return rtnList;
        }
        url->setContexts(QContactDetail::ContextHome);
        map[QContactUrl::FieldUrl] = urlinformation;
        setDetailValues(map, url);
        rtnList << url;
    }
    return rtnList;
}

QContactBirthday* QContactABook::getBirthdayDetail(QtContactsJNI::MyContacts & contact) const
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

QContactAnniversary* QContactABook::getAnniVersaryDetail(QtContactsJNI::MyContacts & contact) const
{
    QContactAnniversary* rtn = new QContactAnniversary;
    QVariantMap map;
    if(contact.m_anniverasay.isEmpty())
    {
        return rtn;
    }
    QStringList temp = contact.m_anniverasay.split(":", QString::SkipEmptyParts);
    QString sdate = temp.takeFirst();
    QString  smonth = temp.takeFirst();
    QString  syear = temp.takeFirst();
    bool ok;
    int date = sdate.toInt(&ok,10);
    int month = smonth.toInt(&ok,10);
    int year = syear.toInt(&ok,10);
    QDate anniversaryday(year,month,date);
    map[QContactAnniversary::FieldOriginalDate] =anniversaryday;
    setDetailValues(map, rtn);
    return rtn;
}

QContactNickname* QContactABook::getNicknameDetail(QtContactsJNI::MyContacts & contact) const
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

QContactGuid* QContactABook::getGuidDetail(QtContactsJNI::MyContacts & contact) const
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

