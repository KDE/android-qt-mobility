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

#include "qversitdefs.h"
#include "qversitcontactgenerator_p.h"
#include <qversitdocument.h>
#include <qversitproperty.h>
#include <qcontact.h>
#include <qcontactdetail.h>
#include <qcontactname.h>
#include <qcontactphonenumber.h>
#include <qcontactaddress.h>
#include <qcontactemailaddress.h>
#include <qcontactorganization.h>
#include <qcontacturl.h>
#include <qcontactguid.h>
#include <qcontacttimestamp.h>
#include <qcontactanniversary.h>
#include <qcontactbirthday.h>
#include <qcontactgender.h>
#include <qcontactnickname.h>
#include <qcontactavatar.h>
#include <qcontactgeolocation.h>
#include <qcontactnote.h>
#include <qcontactonlineaccount.h>
#include <qcontactfamily.h>
#include <QHash>
#include <QFile>

/*!
 * Constructor.
 */
QVersitContactGeneratorPrivate::QVersitContactGeneratorPrivate()
{
    mDetailMappings.insert(QString::fromAscii(versitEmailId),
        QPair<QString,QString>(
            QContactEmailAddress::DefinitionName,
            QContactEmailAddress::FieldEmailAddress));
    mDetailMappings.insert(QString::fromAscii(versitUrlId),
        QPair<QString,QString>(
            QContactUrl::DefinitionName,
            QContactUrl::FieldUrl));
    mDetailMappings.insert(QString::fromAscii(versitUidId),
        QPair<QString,QString>(
            QContactGuid::DefinitionName,
            QContactGuid::FieldGuid));
    mDetailMappings.insert(QString::fromAscii(versitNoteId),
        QPair<QString,QString>(
            QContactNote::DefinitionName,
            QContactNote::FieldNote));
    mDetailMappings.insert(QString::fromAscii(versitGenderId),
        QPair<QString,QString>(
            QContactGender::DefinitionName,
            QContactGender::FieldGender));

    mContextMappings.insert(
        QString::fromAscii(versitContextWorkId),QContactDetail::ContextWork);
    mContextMappings.insert(
        QString::fromAscii(versitContextHomeId),QContactDetail::ContextHome);
    
    mSubTypeMappings.insert(
        QString::fromAscii(versitDomesticId),QContactAddress::SubTypeDomestic);
    mSubTypeMappings.insert(
        QString::fromAscii(versitInternationalId),QContactAddress::SubTypeInternational);
    mSubTypeMappings.insert(
        QString::fromAscii(versitPostalId),QContactAddress::SubTypePostal);
    mSubTypeMappings.insert(
        QString::fromAscii(versitParcelId),QContactAddress::SubTypeParcel);
    mSubTypeMappings.insert(
        QString::fromAscii(versitVoiceId),QContactPhoneNumber::SubTypeVoice);
    mSubTypeMappings.insert(
        QString::fromAscii(versitCellId),QContactPhoneNumber::SubTypeMobile);
    mSubTypeMappings.insert(
        QString::fromAscii(versitModemId),QContactPhoneNumber::SubTypeModem);
    mSubTypeMappings.insert(
        QString::fromAscii(versitCarId),QContactPhoneNumber::SubTypeCar);
    mSubTypeMappings.insert(
        QString::fromAscii(versitVideoId),QContactPhoneNumber::SubTypeVideo);
    mSubTypeMappings.insert(
        QString::fromAscii(versitFaxId),QContactPhoneNumber::SubTypeFacsimile);
    mSubTypeMappings.insert(
        QString::fromAscii(versitBbsId),QContactPhoneNumber::SubTypeBulletinBoardSystem);
    mSubTypeMappings.insert(
        QString::fromAscii(versitPagerId),QContactPhoneNumber::SubTypePager);
    mSubTypeMappings.insert(
        QString::fromAscii(versitSipSubTypeId),QContactOnlineAccount::SubTypeSip);
    mSubTypeMappings.insert(
        QString::fromAscii(versitSwisId),QContactOnlineAccount::SubTypeShareVideo);
    mSubTypeMappings.insert(
        QString::fromAscii(versitVoipId),QContactOnlineAccount::SubTypeInternet);

    mTypeFileExtensionMappings.insert(
        QString::fromAscii(versitFormatWave),QString::fromAscii(versitWAVEExtenId));
}

/*!
 * Destructor.
 */
QVersitContactGeneratorPrivate::~QVersitContactGeneratorPrivate()
{
}

/*!
 * Generates a QContact from \a versitDocument.
 */
QContact QVersitContactGeneratorPrivate::generateContact(const QVersitDocument& versitDocument)
{
    QContact contact;
    const QList<QVersitProperty> properties = versitDocument.properties();
    foreach (QVersitProperty property, properties) {
        QContactDetail* detail = 0;
        if (property.name() == QString::fromAscii(versitNameId)) {
            detail = createName(property,contact);
        } else if (property.name() == QString::fromAscii(versitPhoneId)) {
            detail = createPhone(property);
        } else if (property.name() == QString::fromAscii(versitAddressId)) {
            detail = createAddress(property);
        } else if (property.name() == QString::fromAscii(versitTitleId)){
            detail = createOrganization(property, versitDocument);
        } else if (property.name() == QString::fromAscii(versitOrganizationId)) {
            detail = createOrganization(property, versitDocument);
        } else if (property.name() == QString::fromAscii(versitRoleId)) {
            detail = createOrganization(property, versitDocument);
        } else if (property.name() == QString::fromAscii(versitLogoId)) {
            detail = createOrganization(property, versitDocument);
        } else if (property.name() == QString::fromAscii(versitRevisionId)) {
            detail = createTimeStamp(property);
        } else if (property.name() == QString::fromAscii(versitAnniversaryId)) {
            detail = createAnniversary(property);
        } else if (property.name() == QString::fromAscii(versitPhotoId)) {
            detail = createAvatar(property,versitDocument,QContactAvatar::SubTypeImage);
        } else if (property.name() == QString::fromAscii(versitBirthdayId)) {
            detail = createBirthday(property);
        } else if (property.name() == QString::fromAscii(versitNicknameId)||
                   property.name() == QString::fromAscii(versitNicknameXId)) {
            createNicknames(property,contact);
        } else if (property.name() == QString::fromAscii(versitGeoId)){
            detail = createGeoLocation(property);
        } else if (property.name() == QString::fromAscii(versitSipId)){
            detail = createOnlineAccount(property);
        } else if (property.name() == QString::fromAscii(versitSpouseId) ||
                   property.name() == QString::fromAscii(versitChildrenId)){
            detail = createFamily(property,contact);
        } else if (property.name() == QString::fromAscii(versitSoundId)){
            detail = createAvatar(property,versitDocument,QContactAvatar::SubTypeAudioRingtone);
        } else {
            detail = createNameValueDetail(property);
        }

        if (detail) {
            QStringList contexts(extractContexts(property));
            if (!contexts.empty())
                detail->setContexts(contexts);
            contact.saveDetail(detail);
            delete detail;
        }
    }
    return contact;
}

/*!
 * Creates a QContactName from \a property
 */
QContactDetail* QVersitContactGeneratorPrivate::createName(
    const QVersitProperty& property,const QContact& contact) const
{
    // Restrict only one name can exist, if multiple than choose first
    // and discard rest
    if( !contact.detail(QContactName::DefinitionName).isEmpty()){
        return 0;
    }
    QContactName* name = new QContactName();
    QList<QByteArray> values = property.value().split(';');
    name->setLast(takeFirst(values));
    name->setFirst(takeFirst(values));
    name->setMiddle(takeFirst(values));
    name->setPrefix(takeFirst(values));
    name->setSuffix(takeFirst(values));
    return name;
}

/*!
 * Creates a QContactPhoneNumber from \a property
 */
QContactDetail* QVersitContactGeneratorPrivate::createPhone(
    const QVersitProperty& property) const
{
    QContactPhoneNumber* phone = new QContactPhoneNumber();
    phone->setNumber(QString::fromAscii(property.value()));
    phone->setSubTypes(extractSubTypes(property));
    return phone;
}

/*!
 * Creates a QContactAddress from \a property
 */
QContactDetail* QVersitContactGeneratorPrivate::createAddress(
    const QVersitProperty& property) const
{
    QContactAddress* address = new QContactAddress();
    
    QList<QByteArray> addressParts = property.value().split(';');
    address->setPostOfficeBox(takeFirst(addressParts));
    // There is no setter for the Extended Address in QContactAddress:
    if (!addressParts.isEmpty())
        addressParts.removeFirst(); 
    address->setStreet(takeFirst(addressParts));
    address->setLocality(takeFirst(addressParts));
    address->setRegion(takeFirst(addressParts));
    address->setPostcode(takeFirst(addressParts));
    address->setCountry(takeFirst(addressParts));
    
    address->setSubTypes(extractSubTypes(property));
    
    return address;
}

/*!
 * Creates a QContactOrganization from \a property and adds it to the \a document
 */
QContactDetail* QVersitContactGeneratorPrivate::createOrganization(
     const QVersitProperty& property,
     const QVersitDocument& versitDocument) const
{
    QContactOrganization* org = new QContactOrganization;
    if (property.name() == QString::fromAscii(versitTitleId)) {
        org->setTitle(QString::fromAscii(property.value()));
    } else if (property.name() == QString::fromAscii(versitOrganizationId)) {
        QByteArray value = property.value();
        int firstSemic = value.indexOf(";");
        org->setName(QString::fromAscii(value.left(firstSemic)));
        QByteArray orgUnit = value.mid(firstSemic+1,value.size());
        org->setDepartment(QString::fromAscii(orgUnit));
    } else if (property.name() == QString::fromAscii(versitLogoId)) {
        QString path(mImagePath);
        if (!path.endsWith(QString::fromAscii("/")))
            path += QString::fromAscii("/");
        QString fileName = saveContentToFile(path,property);
        if (!fileName.isEmpty()) {
            org->setLogo(fileName);
        } else {
            delete org;
            org = 0;
        }
    } else {
        delete org;
        org = 0;
    }    
    return org;
}

/*!
 * Creates a QContactTimeStamp from \a property
 */
QContactDetail* QVersitContactGeneratorPrivate::createTimeStamp(
    const QVersitProperty& property) const
{
    QContactTimestamp* timeStamp = new QContactTimestamp();
    QByteArray value(property.value());
    bool utc = (value.endsWith("Z") || value.endsWith("z"));
    if (utc)
        value.chop(1); // take away z from end;
    QDateTime dateTime = parseDateTime(value,versitDateTimeSpecIso8601Basic);
    if (utc)
        dateTime.setTimeSpec(Qt::UTC);
    timeStamp->setLastModified(dateTime);
    return timeStamp;
}

/*!
 * Creates a QContactAnniversary from \a property
 */
QContactDetail* QVersitContactGeneratorPrivate::createAnniversary(
    const QVersitProperty& property) const
{
    QContactAnniversary* anniversary = new QContactAnniversary();
    QDateTime dateTime =
        parseDateTime(property.value(),versitDateSpecIso8601Basic);
    anniversary->setOriginalDate(dateTime.date());
    return anniversary;
}

/*!
 * Creates a QContactBirthday from \a property
 */
QContactDetail* QVersitContactGeneratorPrivate::createBirthday(
    const QVersitProperty& property) const
{
    QContactBirthday* bday = new QContactBirthday();
    QDateTime dateTime =
        parseDateTime(property.value(),versitDateSpecIso8601Basic);
    bday->setDate(dateTime.date());
    return bday;
}

/*!
 * Creates QContactNicknames from \a property and adds them to \a contact
 */
void QVersitContactGeneratorPrivate::createNicknames(
    const QVersitProperty& property,
    QContact& contact) const
{
    QList<QByteArray> values = property.value().split(',');
    foreach(QByteArray value,values) {
        QContactNickname* nickName = new QContactNickname();
        nickName->setNickname(QString::fromAscii(value));
        contact.saveDetail(nickName);
        delete nickName;
    }
}

/*!
 * Creates a QContactOnlineAccount from \a property
 */
QContactDetail* QVersitContactGeneratorPrivate::createOnlineAccount(
    const QVersitProperty& property) const
{    
    QContactOnlineAccount* onlineAccount = new QContactOnlineAccount();
    onlineAccount->setAccountUri(QString::fromAscii(property.value()));
    QStringList subTypes = extractSubTypes(property);
    if (subTypes.count() == 0)
        subTypes.append(QContactOnlineAccount::SubTypeSip);
    onlineAccount->setSubTypes(subTypes);
    return onlineAccount;
}

/*!
 * Creates a QContactAvatar from \a property
 */
QContactDetail* QVersitContactGeneratorPrivate::createAvatar(
    const QVersitProperty& property,
    const QVersitDocument& versitDocument,
    const QString& subType) const
{
    QString value(property.value());

    const QString valueParam =
        property.parameters().value(QString::fromAscii(versitValue));

    if (valueParam != QString::fromAscii("URL")) {
        QString pathAndName(mImagePath);
        if (subType == QContactAvatar::SubTypeAudioRingtone)
            pathAndName = mAudioClipPath;
        if (!pathAndName.endsWith(QString::fromAscii("/")))
            pathAndName += QString::fromAscii("/");
        pathAndName += getFirstAndLastName(versitDocument);
        value = saveContentToFile(pathAndName,property);
    }

    QContactAvatar* avatar = 0;
    if (!value.isEmpty()) {
        avatar = new QContactAvatar();
        avatar->setAvatar(value);
        avatar->setSubType(subType);
    }
    return avatar;
}

/*!
 * Creates a QContactGeolocation from \a property
 */
QContactDetail* QVersitContactGeneratorPrivate::createGeoLocation(
    const QVersitProperty& property) const
{
    QContactGeolocation* geo = new QContactGeolocation();
    QList<QByteArray> values = property.value().split(',');
    geo->setLongitude(takeFirst(values).toDouble());
    geo->setLatitude(takeFirst(values).toDouble());
    return geo;
}

/*!
 * Creates a QContactFamily from \a property
 */
QContactDetail* QVersitContactGeneratorPrivate::createFamily(
    const QVersitProperty& property,
    const QContact& contact) const
{
    QString val = QString::fromAscii(property.value());
    QContactFamily family =
        static_cast<QContactFamily>(contact.detail(QContactFamily::DefinitionName));
    if (property.name() == QString::fromAscii(versitSpouseId)) {
        family.setSpouse(val);
    } else if (property.name() == QString::fromAscii(versitChildrenId)) {
        family.setChildren(val.split(QString::fromAscii(",")));
    }
    return new QContactDetail(family);
}

/*!
 * Creates a simple name-value contact detail.
 */
QContactDetail* QVersitContactGeneratorPrivate::createNameValueDetail(
    const QVersitProperty& property) const
{
    QContactDetail* detail = 0;
    QPair<QString,QString> nameAndValueType =
        mDetailMappings.value(property.name());
    if (nameAndValueType.first.length() > 0) {
        detail = new QContactDetail(nameAndValueType.first);
        detail->setValue(
            nameAndValueType.second,QString::fromAscii(property.value()));
    }
    return detail;
}

/*!
 * Extracts the list of contexts from \a types
 */
QStringList QVersitContactGeneratorPrivate::extractContexts(
    const QVersitProperty& property) const
{   
    QStringList types = 
        property.parameters().values(QString::fromAscii(versitType));
    QStringList contexts;
    foreach (QString type, types) {   
        QString value = mContextMappings.value(type);
        if (value.length() > 0)
            contexts.append(value);
    }
    return contexts;
}

/*!
 * Extracts the list of subtypes from \a property
 */
QStringList QVersitContactGeneratorPrivate::extractSubTypes(
    const QVersitProperty& property) const
{
    QStringList types = 
        property.parameters().values(QString::fromAscii(versitType));
    QStringList subTypes;
    foreach (QString type, types) {
        QString subType = mSubTypeMappings.value(type);
        if (subType.length() > 0)
            subTypes += subType;
    }
    return subTypes;
}

/*!
 * Takes the first value in \a list and converts it to a QString.
 * An empty QString is returned, if the list is empty.
 */
QString QVersitContactGeneratorPrivate::takeFirst(QList<QByteArray>& list) const
{
    QString first;
    if (!list.isEmpty())
        first = QString::fromAscii(list.takeFirst());
    return first; 
}

/*!
 * Parses a date and time from text
 */
QDateTime QVersitContactGeneratorPrivate::parseDateTime(
    const QByteArray& text,
    const QByteArray& format) const
{
    QDateTime dateTime;
    QString value(QString::fromAscii(text));
    if (text.contains("-")) {
        dateTime = QDateTime::fromString(value,Qt::ISODate);
    } else {
        dateTime = QDateTime::fromString(value, QString::fromAscii(format));
    }
    return dateTime;
}

/*!
 *
 */
QString QVersitContactGeneratorPrivate::saveContentToFile(
    const QString& pathAndName,
    const QVersitProperty& property) const
{
    QString encoding =
        property.parameters().value(QString::fromAscii(versitEncoding));
    QByteArray content = property.value();

    QString type =
        property.parameters().value(QString::fromAscii(versitType));
    // Use the type parameter value as it is, if not found in the mapping table
    QString extension = mTypeFileExtensionMappings.value(type,type);

    QString fileName(pathAndName);
    fileName += QString::number(qrand());
    fileName += QString::fromAscii(".");
    fileName += extension.toLower();

    QFile file(fileName);
    qint64 writeResult = -1;
    if (file.open(QIODevice::WriteOnly)) {
        if (encoding == QString::fromAscii(versitEncodingBase64) ||
            encoding == QString::fromAscii(versitEncodingBinary)) {
            writeResult = file.write(QByteArray::fromBase64(property.value()));
        } else {
            // default assumption
            // quoted-printable encoding is parsed already in the reader
            writeResult = file.write(property.value());
        }
    }
    file.close();
    return (writeResult > 0) ? fileName : QString();
}

QString QVersitContactGeneratorPrivate::getFirstAndLastName
        (const QVersitDocument& document) const
{
    QString name;
    const QList<QVersitProperty> properties = document.properties();
    foreach(const QVersitProperty& nameProperty, properties) {
        if (nameProperty.name() == QString::fromAscii(versitNameId)) {
            QList<QByteArray> values = nameProperty.value().split(';');
            name.append(takeFirst(values));
            name.append(takeFirst(values));
            break;
        }
    }
    return name;
}

