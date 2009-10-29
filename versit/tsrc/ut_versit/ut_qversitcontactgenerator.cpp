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
#include "ut_qversitcontactgenerator.h"
#include "qversitcontactgenerator.h"
#include "qversitcontactgenerator_p.h"
#include <qversitproperty.h>
#include <qversitdocument.h>
#include <QtTest/QtTest>
#include <qcontact.h>
#include <qcontactdetail.h>
#include <qcontactname.h>
#include <qcontactaddress.h>
#include <qcontactphonenumber.h>
#include <qcontactemailaddress.h>
#include <qcontacturl.h>
#include <qcontactguid.h>
#include <qcontactorganization.h>
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
#include <QDir>




QString imageAndAudioClipPath(QString::fromAscii("random98354_dir76583_ut_versit_photo"));

void UT_QVersitContactGenerator::initTestCase()
{
    // Create the directory to store the image
    QDir dir;
    if (!dir.exists(imageAndAudioClipPath)) {
        dir.mkdir(imageAndAudioClipPath);
    }
}

void UT_QVersitContactGenerator::cleanupTestCase()
{
    QDir dir;

    if (dir.exists(imageAndAudioClipPath)) {
        dir.cd(imageAndAudioClipPath);
        // remove all the files first
        QStringList allFiles;
        allFiles << QString::fromAscii("*");
        QStringList fileList = dir.entryList(allFiles, QDir::Files);
        foreach (QString file, fileList) {
            dir.remove(file);
        }
        dir.cdUp();
        dir.rmdir(imageAndAudioClipPath);
    }
}

void UT_QVersitContactGenerator::init()
{    
    mGenerator = new QVersitContactGenerator();
    mGeneratorPrivate = new QVersitContactGeneratorPrivate();
}

void UT_QVersitContactGenerator::cleanup()
{
    delete mGeneratorPrivate;
    delete mGenerator;
}

void UT_QVersitContactGenerator::testName()
{
    QVersitDocument document;
    QVersitProperty nameProperty;
    QStringList val;
    val.append(QString::fromAscii("Homer"));//FirstName
    val.append(QString::fromAscii("Simpson"));//LastName
    val.append(QString::fromAscii("BellyBoy"));//GivenName
    val.append(QString::fromAscii("Dr"));//PreFix
    val.append(QString::fromAscii("MSc"));//Suffix
    nameProperty.setName(QString::fromAscii(versitNameId));
    nameProperty.setValue(val.join(QString::fromAscii(";")).toAscii());
    document.addProperty(nameProperty);        
    QContact contact = mGenerator->generateContact(document);    
    QContactName name = (QContactName)contact.detail(QContactName::DefinitionName);
    QCOMPARE(name.last(),val[0]);
    QCOMPARE(name.first(),val[1]);
    QCOMPARE(name.middle(),val[2]);
    QCOMPARE(name.prefix(),val[3]);
    QCOMPARE(name.suffix(),val[4]);

    // Multiple Names , first one will be picked and rest will be discarded
    nameProperty = QVersitProperty();
    QStringList val_2;
    val_2.append(QString::fromAscii("FakeHomer"));//FirstName
    val_2.append(QString::fromAscii("FakeSimpson"));//LastName
    val_2.append(QString::fromAscii("FakeBellyBoy"));//GivenName
    val_2.append(QString::fromAscii("FakeDr"));//PreFix
    val_2.append(QString::fromAscii("FakeMSc"));//Suffix
    nameProperty.setName(QString::fromAscii(versitNameId));
    nameProperty.setValue(val_2.join(QString::fromAscii(";")).toAscii());
    document.addProperty(nameProperty);
    contact = mGenerator->generateContact(document);
    QList<QContactDetail> names = contact.details(QContactName::DefinitionName);
    QCOMPARE(names.count(),1);
    // val_2 should be discarded , so check for val
    name = (QContactName)names[0];
    QCOMPARE(name.last(),val[0]);
    QCOMPARE(name.first(),val[1]);
    QCOMPARE(name.middle(),val[2]);
    QCOMPARE(name.prefix(),val[3]);
    QCOMPARE(name.suffix(),val[4]);
}

void UT_QVersitContactGenerator::testAddress()
{
    QContact contact;
    QVersitDocument document;
    QVersitProperty property;
    property.setName(QString::fromAscii(versitAddressId)); 
    
    // Empty value for the address
    document = createDocumentWithProperty(property);
    contact = mGenerator->generateContact(document);
    QContactAddress address = 
        static_cast<QContactAddress>(contact.detail(QContactAddress::DefinitionName));
    QCOMPARE(address.postOfficeBox(),QString());
    QCOMPARE(address.street(),QString());
    QCOMPARE(address.locality(),QString());
    QCOMPARE(address.region(),QString());
    QCOMPARE(address.postcode(),QString());
    QCOMPARE(address.country(),QString());
    
    // Address with just seprators
    property.setValue(QByteArray(";;;;;;"));
    document = createDocumentWithProperty(property);
    contact = mGenerator->generateContact(document);
    address = static_cast<QContactAddress>(contact.detail(QContactAddress::DefinitionName));
    QCOMPARE(address.postOfficeBox(),QString());
    QCOMPARE(address.street(),QString());
    QCOMPARE(address.locality(),QString());
    QCOMPARE(address.region(),QString());
    QCOMPARE(address.postcode(),QString());
    QCOMPARE(address.country(),QString());
    
    // Address with some fields missing
    property.setValue(QByteArray(";;My Street;My Town;;12345;"));
    document = createDocumentWithProperty(property);
    contact = mGenerator->generateContact(document);
    address = static_cast<QContactAddress>(contact.detail(QContactAddress::DefinitionName));
    QCOMPARE(address.postOfficeBox(),QString());
    QCOMPARE(address.street(),QString::fromAscii("My Street"));
    QCOMPARE(address.locality(),QString::fromAscii("My Town"));
    QCOMPARE(address.region(),QString());
    QCOMPARE(address.postcode(),QString::fromAscii("12345"));
    QCOMPARE(address.country(),QString());
    
    // Address with all the fields filled
    property.setValue(QByteArray("PO Box;E;My Street;My Town;My State;12345;My Country"));
    document = createDocumentWithProperty(property);
    contact = mGenerator->generateContact(document);
    address = static_cast<QContactAddress>(contact.detail(QContactAddress::DefinitionName));
    QCOMPARE(address.postOfficeBox(),QString::fromAscii("PO Box"));
    QCOMPARE(address.street(),QString::fromAscii("My Street"));
    QCOMPARE(address.locality(),QString::fromAscii("My Town"));
    QCOMPARE(address.region(),QString::fromAscii("My State"));
    QCOMPARE(address.postcode(),QString::fromAscii("12345"));
    QCOMPARE(address.country(),QString::fromAscii("My Country"));
    
    // Address with TYPE parameters coverted to contexts and subtypes
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii("HOME"));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii("WORK"));    
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii("DOM"));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii("INTL"));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii("POSTAL"));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii("PARCEL"));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii("X-EXTENSION"));
    document = createDocumentWithProperty(property);
    contact = mGenerator->generateContact(document);
    address = static_cast<QContactAddress>(contact.detail(QContactAddress::DefinitionName));
    QStringList contexts = address.contexts();
    QVERIFY(contexts.contains(QContactDetail::ContextHome));   
    QVERIFY(contexts.contains(QContactDetail::ContextWork));
    QStringList subTypes = address.subTypes();
    QVERIFY(subTypes.contains(QContactAddress::SubTypeDomestic));
    QVERIFY(subTypes.contains(QContactAddress::SubTypeInternational));
    QVERIFY(subTypes.contains(QContactAddress::SubTypePostal));
    QVERIFY(subTypes.contains(QContactAddress::SubTypeParcel));
}

void UT_QVersitContactGenerator::testOrganization()
{
    QContact contact;
    QVersitDocument document;
    QVersitProperty property;
    
    // ORG: Empty value for the organization
    property.setName(QString::fromAscii(versitOrganizationId));
    document = createDocumentWithProperty(property);
    contact = mGenerator->generateContact(document);
    QContactOrganization org = 
        static_cast<QContactOrganization>(contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(org.name(),QString());
    QCOMPARE(org.department(),QString());

    // ORG: Organization with one seprator
    property.setValue(QByteArray(";"));
    document = createDocumentWithProperty(property);
    contact = mGenerator->generateContact(document);
    org = static_cast<QContactOrganization>(contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(org.name(),QString::fromAscii(""));
    QCOMPARE(org.department(),QString::fromAscii(""));
    
    // ORG: Organization with just separators
    property.setValue(QByteArray(";;;"));
    document = createDocumentWithProperty(property);
    contact = mGenerator->generateContact(document);
    org = static_cast<QContactOrganization>(contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(org.name(),QString::fromAscii(""));
    QCOMPARE(org.department(),QString::fromAscii(";;"));
    
    // ORG: Organization with one Organizational Unit
    property.setValue(QByteArray("Nokia;R&D"));
    document = createDocumentWithProperty(property);
    contact = mGenerator->generateContact(document);
    org = static_cast<QContactOrganization>(contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(org.name(),QString::fromAscii("Nokia"));
    QCOMPARE(org.department(),QString::fromAscii("R&D"));
    
    // ORG: Organization with more Organizational Units
    property.setValue(QByteArray("ABC, Inc.;North American Division;Devices;Marketing"));
    document = createDocumentWithProperty(property);
    contact = mGenerator->generateContact(document);
    org = static_cast<QContactOrganization>(contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(org.name(),QString::fromAscii("ABC, Inc."));
    QCOMPARE(org.department(),QString::fromAscii("North American Division;Devices;Marketing"));

    // TITLE
    property.setName(QString::fromAscii(versitTitleId));
    QByteArray titleValue("Hacker");
    property.setValue(titleValue);
    document = createDocumentWithProperty(property);
    contact = mGenerator->generateContact(document);
    org = static_cast<QContactOrganization>(contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(org.title(),QString::fromAscii(titleValue));

    // ORG
    property.setName(QString::fromAscii(versitOrganizationId));
    property.setValue(QByteArray("Nokia;R&D"));
    document = createDocumentWithProperty(property);
    document.addProperty(property);
    contact = mGenerator->generateContact(document);
    org = static_cast<QContactOrganization>(contact.detail(QContactOrganization::DefinitionName));    
    QCOMPARE(org.department(),QString::fromAscii("R&D"));
    QCOMPARE(org.name(),QString::fromAscii("Nokia"));

    // ORG and LOGO
    property.setName(QString::fromAscii(versitLogoId));
    QByteArray logo = "R0lGODlhEgASAIAAAAAAAP///yH5BAEAAAEALAAAAAASABIAAAIdjI+py+0G";
    property.setValue(logo.toBase64());
    property.addParameter(QString::fromAscii(versitType),
                          QString::fromAscii(versitFormatGif));
    property.addParameter(QString::fromAscii(versitEncoding),
                          QString::fromAscii(versitEncodingBase64));
    document.addProperty(property);
    mGenerator->setImagePath(imageAndAudioClipPath);
    contact = mGenerator->generateContact(document);        
    QList<QContactDetail> details = contact.details(QContactOrganization::DefinitionName);
    QVERIFY(!details.isEmpty());
    QCOMPARE(details.size(), 3);
    QString expectedFileName;
    foreach(const QContactDetail& d,details){
        QContactOrganization o = (QContactOrganization)d;
        if(!o.name().isEmpty()){
            expectedFileName = o.name() + QString::fromAscii("_") + o.department();
        }else if(!o.logo().isEmpty()){
            QString fileName = o.logo();
            QVERIFY(fileName.endsWith((QString::fromAscii(versitFormatGif).toLower())));
            QVERIFY(fileName.contains(imageAndAudioClipPath + QString::fromAscii("/")));
             //                         +
            QFile file(fileName);
            QVERIFY(file.open( QIODevice::ReadOnly ));
            QByteArray content = file.readAll();
            QCOMPARE(content,logo);
            file.close();
            break;
        }
    }

    // ROLE
    property.setName(QString::fromAscii(versitRoleId));
    QByteArray roleValue("Very important manager and proud of it");
    property.setValue(roleValue);
    document = createDocumentWithProperty(property);
    contact = mGenerator->generateContact(document);
    org = static_cast<QContactOrganization>(contact.detail(QContactOrganization::DefinitionName));
    QVERIFY(org.isEmpty());

}

void UT_QVersitContactGenerator::testTel()
{
    QVersitDocument document;
    QVersitProperty property;
    property.setName(QString::fromAscii(versitPhoneId));
    QByteArray value("+35850486321");
    property.setValue(value);
    
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii(versitVoiceId));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii(versitCellId));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii(versitModemId));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii(versitCarId));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii(versitVideoId));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii(versitFaxId));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii(versitBbsId));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii(versitPagerId));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii(versitContextHomeId));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii(versitContextWorkId));

    document.addProperty(property);
    QContact contact = mGenerator->generateContact(document);
    const QContactPhoneNumber& phone = 
        static_cast<QContactPhoneNumber>(
            contact.detail(QContactPhoneNumber::DefinitionName));
    QCOMPARE(phone.number(),QString(QString::fromAscii(value)));

    const QStringList subTypes = phone.subTypes();
    QCOMPARE(subTypes.count(),8);
    QVERIFY(subTypes.contains(QContactPhoneNumber::SubTypeVoice));
    QVERIFY(subTypes.contains(QContactPhoneNumber::SubTypeMobile));
    QVERIFY(subTypes.contains(QContactPhoneNumber::SubTypeModem));
    QVERIFY(subTypes.contains(QContactPhoneNumber::SubTypeCar));
    QVERIFY(subTypes.contains(QContactPhoneNumber::SubTypeVideo));
    QVERIFY(subTypes.contains(QContactPhoneNumber::SubTypeFacsimile));
    QVERIFY(subTypes.contains(QContactPhoneNumber::SubTypeBulletinBoardSystem));
    QVERIFY(subTypes.contains(QContactPhoneNumber::SubTypePager));
    
    const QStringList contexts = phone.contexts();
    QCOMPARE(contexts.count(),2);
    QVERIFY(contexts.contains(QContactDetail::ContextWork));
    QVERIFY(contexts.contains(QContactDetail::ContextHome));
}

void UT_QVersitContactGenerator::testEmail()
{
    QVersitProperty property;
    property.setName(QString::fromAscii(versitEmailId));
    QByteArray value("homer.simpson@burns-corporation.com");
    property.setValue(value);
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii(versitContextWorkId));    
    QVersitDocument document = createDocumentWithProperty(property);
    QContact contact = mGenerator->generateContact(document);
    QContactEmailAddress email =
        static_cast<QContactEmailAddress>(
            contact.detail(QContactEmailAddress::DefinitionName));
    QCOMPARE(email.emailAddress(),QString::fromAscii(value));
    const QStringList contexts = email.contexts();
    QCOMPARE(contexts.count(),1);
    QVERIFY(contexts.contains(QContactDetail::ContextWork)); 
}

void UT_QVersitContactGenerator::testUrl()
{
    QVersitProperty property;
    property.setName(QString::fromAscii(versitUrlId));
    QByteArray value("http://www.simpsonsmovie.com/homer.html");
    property.setValue(value);
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii(versitContextWorkId));    
    QVersitDocument document = createDocumentWithProperty(property);    
    QContact contact = mGenerator->generateContact(document);
    QContactUrl url =
        static_cast<QContactUrl>(
            contact.detail(QContactUrl::DefinitionName));
    QCOMPARE(url.url(),QString::fromAscii(value));
    const QStringList contexts = url.contexts();
    QCOMPARE(contexts.count(),1);
    QVERIFY(contexts.contains(QContactDetail::ContextWork));    
}

void UT_QVersitContactGenerator::testUid()
{
    QVersitProperty property;
    property.setName(QString::fromAscii(versitUidId));
    QByteArray value("unique identifier");
    property.setValue(value);
    QVersitDocument document = createDocumentWithProperty(property);
    QContact contact = mGenerator->generateContact(document);
    QContactGuid uid =
        static_cast<QContactGuid>(
            contact.detail(QContactGuid::DefinitionName));
    QCOMPARE(uid.guid(),QString::fromAscii(value));    
}

void UT_QVersitContactGenerator::testTimeStamp()
{
    // Simple date : ISO 8601 extended format
    QVersitProperty property;
    property.setName(QString::fromAscii(versitRevisionId));    
    QByteArray dateValue("1981-05-20");
    property.setValue(dateValue);
    QVersitDocument document = createDocumentWithProperty(property);
    QContact contact = mGenerator->generateContact(document);
    QContactTimestamp timeStamp =
        static_cast<QContactTimestamp>(
            contact.detail(QContactTimestamp::DefinitionName));    
    QCOMPARE(timeStamp.lastModified().date().toString(Qt::ISODate),QString::fromAscii(dateValue));

    // Date and Time : ISO 8601 extended format without utc offset
    QByteArray dateAndTimeValue("1981-05-20T23:55:55");
    property.setValue(dateAndTimeValue);
    document = createDocumentWithProperty(property);
    contact = mGenerator->generateContact(document);
    timeStamp =
        static_cast<QContactTimestamp>(
            contact.detail(QContactTimestamp::DefinitionName));    
    QCOMPARE(timeStamp.lastModified().toString(Qt::ISODate),QString::fromAscii(dateAndTimeValue));    

    // Date and Time : ISO 8601 extented format with utc offset
    QByteArray utcOffset = "Z";
    QByteArray dateAndTimeWithUtcValue = dateAndTimeValue+utcOffset;
    property.setValue(dateAndTimeWithUtcValue);
    document = createDocumentWithProperty(property);
    contact = mGenerator->generateContact(document);
    timeStamp =
        static_cast<QContactTimestamp>(
            contact.detail(QContactTimestamp::DefinitionName));
    QCOMPARE(timeStamp.lastModified().toString(Qt::ISODate),QString::fromAscii(dateAndTimeValue));
    QCOMPARE(timeStamp.lastModified().timeSpec(),Qt::UTC);

    // Date and Time : ISO 8601 in basic format without utc offset
    dateAndTimeValue = "19810520T235555";
    property.setValue(dateAndTimeValue);
    document = createDocumentWithProperty(property);
    contact = mGenerator->generateContact(document);
    timeStamp =
        static_cast<QContactTimestamp>(
            contact.detail(QContactTimestamp::DefinitionName));
    QCOMPARE(timeStamp.lastModified().toString(QString::fromAscii(versitDateTimeSpecIso8601Basic)),
                                                         QString::fromAscii(dateAndTimeValue));

    // Date and Time : ISO 8601 in basic format with utc offset
    dateAndTimeValue = "19810520T235555";
    dateAndTimeWithUtcValue = dateAndTimeValue+utcOffset;
    property.setValue(dateAndTimeWithUtcValue);
    document = createDocumentWithProperty(property);
    contact = mGenerator->generateContact(document);
    timeStamp =
        static_cast<QContactTimestamp>(
            contact.detail(QContactTimestamp::DefinitionName));
    QCOMPARE(timeStamp.lastModified().toString(QString::fromAscii(versitDateTimeSpecIso8601Basic)),
                                                         QString::fromAscii(dateAndTimeValue));
    QCOMPARE(timeStamp.lastModified().timeSpec(),Qt::UTC);
}

void UT_QVersitContactGenerator::testAnniversary()
{
    // Date : ISO 8601 extended format
    QVersitProperty property;
    property.setName(QString::fromAscii(versitAnniversaryId));
    QByteArray dateValue("1981-05-20");
    property.setValue(dateValue);
    QVersitDocument document = createDocumentWithProperty(property);
    QContact contact = mGenerator->generateContact(document);
    QContactAnniversary anniversary =
        static_cast<QContactAnniversary>(
            contact.detail(QContactAnniversary::DefinitionName));
    QCOMPARE(anniversary.originalDate().toString(Qt::ISODate),QString::fromAscii(dateValue));

    // Date : ISO 8601 in basic format
    dateValue = "19810520";
    property.setValue(dateValue);
    document = createDocumentWithProperty(property);
    contact = mGenerator->generateContact(document);
    anniversary =
        static_cast<QContactAnniversary>(
            contact.detail(QContactAnniversary::DefinitionName));
    QCOMPARE(anniversary.originalDate().toString(QString::fromAscii(versitDateSpecIso8601Basic)),
                                                          QString::fromAscii(dateValue));

}

void UT_QVersitContactGenerator::testBirthday()
{
    // Date : ISO 8601 extended format
    QVersitProperty property;
    property.setName(QString::fromAscii(versitBirthdayId));
    QByteArray dateValue("1981-05-20");
    property.setValue(dateValue);
    QVersitDocument document = createDocumentWithProperty(property);
    QContact contact = mGenerator->generateContact(document);
    QContactBirthday bday =
        static_cast<QContactBirthday>(
            contact.detail(QContactBirthday::DefinitionName));
    QCOMPARE(bday.date().toString(Qt::ISODate),QString::fromAscii(dateValue));

    // Date : ISO 8601 in basic format
    dateValue = "19810520";
    property.setValue(dateValue);
    document = createDocumentWithProperty(property);
    contact = mGenerator->generateContact(document);
    bday =
        static_cast<QContactBirthday>(
            contact.detail(QContactBirthday::DefinitionName));
    QCOMPARE(bday.date().toString(QString::fromAscii(versitDateSpecIso8601Basic)),
                                                          QString::fromAscii(dateValue));

}

void UT_QVersitContactGenerator::testGender()
{
    // Date : ISO 8601 extended format
    QVersitProperty property;
    property.setName(QString::fromAscii(versitGenderId));
    QByteArray val("Male");
    property.setValue(val);
    QVersitDocument document = createDocumentWithProperty(property);
    QContact contact = mGenerator->generateContact(document);
    QContactGender  gender =
        static_cast<QContactGender >(
            contact.detail(QContactGender ::DefinitionName));
    QCOMPARE(gender.gender(),QString::fromAscii(val));
}

void UT_QVersitContactGenerator::testNickname()
{
    // one value
    QVersitDocument document;
    QVersitProperty nameProperty;
    QString singleVal(QString::fromAscii("Homie"));
    nameProperty.setName(QString::fromAscii(versitNicknameId));
    nameProperty.setValue(singleVal.toAscii());
    document.addProperty(nameProperty);
    QContact contact = mGenerator->generateContact(document);
    QContactNickname nickName = (QContactNickname)contact.detail(QContactNickname::DefinitionName);
    QCOMPARE(nickName.nickname(),singleVal);

    // comma separated values should generate multiple nickname fields
    contact.clearDetails();
    document = QVersitDocument();
    QStringList multiVal;
    multiVal.append(QString::fromAscii("Homie"));
    multiVal.append(QString::fromAscii("SuperHero"));
    multiVal.append(QString::fromAscii("NukeSpecialist"));
    nameProperty.setName(QString::fromAscii(versitNicknameId));
    nameProperty.setValue(multiVal.join(QString::fromAscii(",")).toAscii());
    document.addProperty(nameProperty);
    contact = mGenerator->generateContact(document);
    QList<QContactDetail> nickNames = contact.details(QContactNickname::DefinitionName);
    QCOMPARE(nickNames.count(),3);
    nickName = static_cast<QContactNickname>(nickNames[0]);
    QCOMPARE(nickName.nickname(),QString::fromAscii("Homie"));
    nickName = static_cast<QContactNickname>(nickNames[1]);
    QCOMPARE(nickName.nickname(),QString::fromAscii("SuperHero"));
    nickName = static_cast<QContactNickname>(nickNames[2]);
    QCOMPARE(nickName.nickname(),QString::fromAscii("NukeSpecialist"));


    // X-NICKNAME
    document = QVersitDocument();
    nameProperty = QVersitProperty();
    nameProperty.setName(QString::fromAscii(versitNicknameXId));
    nameProperty.setValue(singleVal.toAscii());
    document.addProperty(nameProperty);
    contact = mGenerator->generateContact(document);
    nickName =
        static_cast<QContactNickname>(
            contact.detail(QContactNickname::DefinitionName));
    QCOMPARE(nickName.nickname(),singleVal);
}

void UT_QVersitContactGenerator::testAvatarJpegStored()
{
    // JPEG image. Name property present. The image directory exist.
    // Test that the avatar detail is created and the image
    // is stored on the disk.

    QByteArray img =
"/9j/4AAQSkZJRgABAQAAAQABAAD/2wCEABALDA4MChAODQ4SERATGCgaGBYWGDEj"
"JR0oOjM9PDkzODdASFxOQERXRTc4UG1RV19iZ2hnPk1xeXBkeFxlZ2MBERISGBUY"
"LxoaL2NCOEJjY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2Nj"
"Y2NjY2NjY2NjY//AABEIAGAAPAMBIgACEQEDEQH/xAGiAAABBQEBAQEBAQAAAAAA"
"AAAAAQIDBAUGBwgJCgsQAAIBAwMCBAMFBQQEAAABfQECAwAEEQUSITFBBhNRYQci"
"cRQygZGhCCNCscEVUtHwJDNicoIJChYXGBkaJSYnKCkqNDU2Nzg5OkNERUZHSElK"
"U1RVVldYWVpjZGVmZ2hpanN0dXZ3eHl6g4SFhoeIiYqSk5SVlpeYmZqio6Slpqeo"
"qaqys7S1tre4ubrCw8TFxsfIycrS09TV1tfY2drh4uPk5ebn6Onq8fLz9PX29/j5"
"+gEAAwEBAQEBAQEBAQAAAAAAAAECAwQFBgcICQoLEQACAQIEBAMEBwUEBAABAncA"
"AQIDEQQFITEGEkFRB2FxEyIygQgUQpGhscEJIzNS8BVictEKFiQ04SXxFxgZGiYn"
"KCkqNTY3ODk6Q0RFRkdISUpTVFVWV1hZWmNkZWZnaGlqc3R1dnd4eXqCg4SFhoeI"
"iYqSk5SVlpeYmZqio6Slpqeoqaqys7S1tre4ubrCw8TFxsfIycrS09TV1tfY2dri"
"4+Tl5ufo6ery8/T19vf4+fr/2gAMAwEAAhEDEQA/AO/qC8uUs7SW4k+7GucDqT2A"
"9zU9ZWp/vbuONuUjXeVPcngH8MH86yrVFSg5voOKu7GfK19ePlneJNvdyMk+gUjg"
"e5zT4UvLfYYp4yy4ySrDcPf5sfpViivEeNrN3udKpxL9percMY2UxygZ2E5yPUHu"
"Kt1gzt5Ki4H3oTv47gdR+IyK3QQQCOhr1cLXdaF3ujCceVi0lLSV1kC1lX3y35J7"
"xL+hb/GtWuX8Sy3C3wRcbGjESkdcueh/75Nc+Jhz0nEqDtK5ehiublPMhjQRn7rS"
"MRu9wADxUby+UzJMhSRcfL1zngY9cmtaN/Ot4XtnVUODyufl7j2NUdSkA1S1ZF8x"
"oAzOo9CMAfXv+FctTBUYwve3mWqkmyN7a7aAs1sCpHKB/nx9On61f0xmfToS5y23"
"HPXj19/X3qE6pbbwZZJLfYC21wMOP1z9BzVixybbfjAkZnA9ATkV00KVOGtPqRKT"
"e5ZpKWkrpJFrF1jzHvoUTaNgVwD/ABckH8h/6FW1VLVkVrPcygsjqVPcfMKxrxcq"
"bSY4uzMSe5WKV0tw3mfx7ZGRQffHeoUmuEGFeNc8n5Ccn3JPNQRuFthI+eRvbAzy"
"eT/Ol844yIZMeuB/LrXmKHu8r1OtJbjp2uZCH81CyA7V2YByMc11WnuklhA0eduw"
"AZ6jHGK5L7SnA2y5JwB5TD+ldJottcW1qy3HyhnLJHkHYO/T3yfxrswseW6SMqtj"
"RpKWkrtMBao6rNGlsI2b53Zdqjkn5hV6sW7V7uWWOE/vzLkc8BUAOD7EnH4+1RNX"
"i0gW5mS20lux2IXhJyNoyV9sdx9Ki8wZxskJ9PLbP8q0/OXyt5BBzjZjnd/dx65q"
"e3sWN1svSW3x71RWICEHBHHXqP1ryMPGrUurbHVKaiZVmrDUIZpo28qLLMnXaMff"
"YdsHGB9a6wEEZHIquy2tlbtlY4oscgDGf8ar2F9ELGDf5iqEA3NGwGPrivVpQ5I2"
"ZzylzO5o0lIrK6hlIZT0IPBpa1JILqV1CxQ486Thc9FHdj9P54qGwt4orm5aNQMb"
"Yy2OWIGSSe5+apItp1Gcsf3gRQo/2eefzz+QpNPO5Z27NO/P0OP6UAS/ZoPP8/yU"
"83+/tGaJ7fzWRhI8bJnDJjoeo5H0qaikBnT2sIlhhwXeVjueQ7m2gZI56A8DHvWg"
"OBgVVuPkvrWU/d+aMn0LYx/LH41aJAGScAd6YFSWE2xM9svHV4R0YdyB2P8AOrSO"
"siK6EFWGQR3FUJ9TUgpZgTP/AH/4B+Pf8Kn05w+nwY/hQKfYjgj8xURqRlJxT1Q7"
"NK5kSLNd3SSAtFcyttQg8xp1I/LP4/hW9GixRrGgwqgAD2FZ+lx77macn7mIlHp0"
"JP6j8q06xwql7Pmk9XqVO17IKKKK6SBkkaSxskihlYYINYbQq880UpeXyZCoEjFg"
"BgEcH2IrerEBD3FzMpyJJSR+AC/+y1wY+VqWj6mlJe8Oq1o/+omXsszY/HB/rVWr"
"ejqfsruRgSSMw9x0B/SuTLk/aP0NKux//9k=";

    QStringList nameValues(QString::fromAscii("Homer")); // First name
    nameValues.append(QString::fromAscii("Simpson")); // Last name
    QByteArray name = nameValues.join(QString::fromAscii(";")).toAscii();
    QVersitDocument document =
        createDocumentWithNameAndPhoto(
            name,img,QString::fromAscii(versitFormatJpeg),QString::fromAscii(versitEncodingBase64));
    mGenerator->setImagePath(imageAndAudioClipPath);
    QContact contact = mGenerator->generateContact(document);
    QContactAvatar avatar =
        static_cast<QContactAvatar>(contact.detail(QContactAvatar::DefinitionName));
    QVERIFY(avatar.subType() == QContactAvatar::SubTypeImage);
    QString fileName = avatar.avatar();
    QVERIFY(fileName.contains(nameValues.at(0) + nameValues.at(1)));
    QVERIFY(fileName.endsWith(QString::fromAscii(versitFormatJpeg).toLower()));
    QFile file(fileName);
    QVERIFY(file.open(QIODevice::ReadOnly));
    QByteArray content = file.readAll();
    file.close();
    QCOMPARE(content,img);
}

void UT_QVersitContactGenerator::testAvatarGifStored()
{
    QByteArray img =
"R0lGODlhEgASAIAAAAAAAP///yH5BAEAAAEALAAAAAASABIAAAIdjI+py+0G"
"wEtxUmlPzRDnzYGfN3KBaKGT6rDmGxQAOw==";
    QStringList nameValues(QString::fromAscii("Homer")); // First name
    nameValues.append(QString::fromAscii("Simpson")); // Last name
    QByteArray name = nameValues.join(QString::fromAscii(";")).toAscii();
    QVersitDocument document =
        createDocumentWithNameAndPhoto(
            name,img,QString::fromAscii(versitFormatGif),QString::fromAscii(versitEncodingBinary));
    mGenerator->setImagePath(imageAndAudioClipPath);
    QString ext = QString::fromAscii(versitGIFExtenId);
    ext = ext.toLower();    
    QContact contact = mGenerator->generateContact(document);
    QContactDetail detail = contact.detail(QContactAvatar::DefinitionName);
    QVERIFY(!detail.isEmpty());
    // Take out the random part from the original file name
    QContactAvatar avatar = static_cast<QContactAvatar>(detail);
    QString fileName = avatar.avatar();
    QVERIFY(avatar.subType() == QContactAvatar::SubTypeImage);
    QDir dir;
    QVERIFY(dir.exists(avatar.avatar()));
    QVERIFY(fileName.contains(nameValues.at(0) + nameValues.at(1)));
    QVERIFY(fileName.endsWith(ext));
    QFile file(fileName);
    QVERIFY(file.open(QIODevice::ReadOnly));
    QByteArray content = file.readAll();
    QCOMPARE(content,img);
}

void UT_QVersitContactGenerator::testAvatarJpegTwoContactsWithSameName()
{
    // Create 2 contacts with the same name.
    // Test that the generated photo files have different names
    QByteArray img = "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";
    QStringList nameValues(QString::fromAscii("Homer")); // First name
    nameValues.append(QString::fromAscii("Simpson")); // Last name
    QByteArray name = nameValues.join(QString::fromAscii(";")).toAscii();
    QVersitDocument document1 =
        createDocumentWithNameAndPhoto(
            name,img,QString::fromAscii(versitFormatJpeg),QString::fromAscii(versitEncodingBase64));
    QVersitDocument document2 =
        createDocumentWithNameAndPhoto(
            name,img,QString::fromAscii(versitFormatJpeg),QString::fromAscii(versitEncodingBinary));

    mGenerator->setImagePath(imageAndAudioClipPath);
    QContact contact1 = mGenerator->generateContact(document1);
    QContact contact2 = mGenerator->generateContact(document2);
    QContactAvatar avatar1 =
        static_cast<QContactAvatar>(contact1.detail(QContactAvatar::DefinitionName));
    QContactAvatar avatar2 =
        static_cast<QContactAvatar>(contact2.detail(QContactAvatar::DefinitionName));
    QVERIFY(avatar1.subType() == QContactAvatar::SubTypeImage);
    QVERIFY(avatar2.subType() == QContactAvatar::SubTypeImage);
    QVERIFY(avatar1.avatar() != avatar2.avatar());
}

void UT_QVersitContactGenerator::testAvatarJpegNonexistentPath()
{
    // JPEG image. Name property present. The image path doesn't exist.
    // Test that the avatar detail is not created and there is no image
    // stored on the disk.
    QByteArray img = "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";
    QStringList nameValues(QString::fromAscii("Homer")); // First name
    nameValues.append(QString::fromAscii("Simpson")); // Last name
    QByteArray name = nameValues.join(QString::fromAscii(";")).toAscii();
    QVersitDocument document =
        createDocumentWithNameAndPhoto(
            name,img,QString::fromAscii(versitFormatJpeg),QString::fromAscii(versitEncodingBase64));

    mGenerator->setImagePath(QString::fromAscii("some_nonexistent/path/anywhere543"));
    QContact contact = mGenerator->generateContact(document);

    QContactDetail detail = contact.detail(QContactAvatar::DefinitionName);
    QVERIFY(detail.isEmpty());

    QDir dir(mGenerator->imagePath());
    QVERIFY(!dir.exists());
}

void UT_QVersitContactGenerator::testAvatarUrl()
{
    QVersitProperty property;
    property.setName(QString::fromAscii(versitPhotoId));
    QByteArray value("file:///jgpublic.");
    property.setValue(value);
    property.addParameter(
        QString::fromAscii(versitValue),QString::fromAscii("URL"));

    QVersitDocument document;
    document.addProperty(property);

    QContact contact = mGenerator->generateContact(document);
    QContactAvatar avatar =
        static_cast<QContactAvatar>(contact.detail(QContactAvatar::DefinitionName));
    QCOMPARE(avatar.avatar(), QString::fromAscii("file:///jgpublic."));
    QVERIFY(avatar.subType() == QContactAvatar::SubTypeImage);
}

void UT_QVersitContactGenerator::testAvatarEncoding()
{
    // Tests only quoted-printable encoding.
    // Assumes that this is the only other encoding supported in PHOTO.
    // In versit property the image is already decoded by the reader.
    QByteArray img =
"R0lGODlhEgASAIAAAAA"
"AAP///yH5BAEAAAEALAAAAAASABIAAAIdjI+py+0G"
"wEtxUmlPzRDnzYGfN3KBaKGT6rDmGxQAOw==";
    QStringList nameValues(QString::fromAscii("Homer")); // First name
    nameValues.append(QString::fromAscii("Simpson")); // Last name
    QByteArray name = nameValues.join(QString::fromAscii(";")).toAscii();
    QVersitDocument document =
        createDocumentWithNameAndPhoto(
            name,img,QString::fromAscii(versitFormatGif),
            QString::fromAscii(versitEncodingQuotedPrintable));

    mGenerator->setImagePath(imageAndAudioClipPath);
    QContact contact = mGenerator->generateContact(document);

    QContactDetail detail = contact.detail(QContactAvatar::DefinitionName);
    QVERIFY(!detail.isEmpty());
    QContactAvatar avatar = static_cast<QContactAvatar>(detail);
    QVERIFY(avatar.subType() == QContactAvatar::SubTypeImage);
    QDir dir;
    QVERIFY(dir.exists(avatar.avatar()));
}

void UT_QVersitContactGenerator::testGeo()
{
    // some positive values
    QVersitDocument document;
    QVersitProperty nameProperty;
    QStringList val;    
    val.append(QString::fromAscii("18.53"));// Longtitude
    val.append(QString::fromAscii("45.32")); // Latitude
    nameProperty.setName(QString::fromAscii(versitGeoId));
    nameProperty.setValue(val.join(QString::fromAscii(",")).toAscii());
    document.addProperty(nameProperty);
    QContact contact = mGenerator->generateContact(document);
    QContactGeolocation geo = (QContactGeolocation)contact.detail(QContactGeolocation::DefinitionName);
    QString str;
    str.setNum(geo.longitude(),'.',2);
    QCOMPARE(str,val[0]);
    str.setNum(geo.latitude(),'.',2);
    QCOMPARE(str,val[1]);

    // some negative values
    document = QVersitDocument();
    nameProperty = QVersitProperty();
    val.append(QString::fromAscii("18.53"));// Longtitude
    val.append(QString::fromAscii("-45.32")); // Latitude
    nameProperty.setName(QString::fromAscii(versitGeoId));
    nameProperty.setValue(val.join(QString::fromAscii(",")).toAscii());
    document.addProperty(nameProperty);
    contact = mGenerator->generateContact(document);
    geo = (QContactGeolocation)contact.detail(QContactGeolocation::DefinitionName);
    str.setNum(geo.longitude(),'.',2);
    QCOMPARE(str,val[0]);
    str.setNum(geo.latitude(),'.',2);
    QCOMPARE(str,val[1]);
}

void UT_QVersitContactGenerator::testNote()
{
    // single line value
    QVersitDocument document;
    QVersitProperty nameProperty;
    QByteArray val("I will not sleep at my work -Bart");
    nameProperty.setName(QString::fromAscii(versitNoteId));
    nameProperty.setValue(val);
    document.addProperty(nameProperty);
    QContact contact = mGenerator->generateContact(document);
    QContactNote note = (QContactNote)contact.detail(QContactNote::DefinitionName);
    QCOMPARE(note.note(),QString::fromAscii(val));

    // Multiline value and quoted printable encoding
    document = QVersitDocument();
    nameProperty = QVersitProperty();
    val = QByteArray("My Dad acts like he belongs,=0D=0AHe belongs in the zoo.=0D=0A");
    nameProperty.setName(QString::fromAscii(versitNoteId));
    nameProperty.setValue(val);
    QMultiHash<QString,QString> params;
    params.insert(QString::fromAscii("QUOTED-PRINTABLE"),QString::fromAscii(val));
    nameProperty.setParameters(params);
    document.addProperty(nameProperty);
    contact = mGenerator->generateContact(document);
    note = (QContactNote)contact.detail(QContactNote::DefinitionName);
    QCOMPARE(note.note(),QString::fromAscii(val));
}

void UT_QVersitContactGenerator::testOnlineAccount()
{
    // no subtype
    QVersitDocument document;
    QVersitProperty nameProperty;
    QByteArray val("sip:homer@wiserthanever.com");
    nameProperty.setName(QString::fromAscii(versitSipId));
    nameProperty.setValue(val);
    document.addProperty(nameProperty);
    QContact contact = mGenerator->generateContact(document);
    QContactOnlineAccount online = (QContactOnlineAccount)contact.detail(QContactOnlineAccount::DefinitionName);
    QCOMPARE(online.accountUri(),QString::fromAscii(val));

    // with supported subtype : SIP
    document = QVersitDocument();
    nameProperty = QVersitProperty();
    nameProperty.setName(QString::fromAscii(versitSipId));
    nameProperty.setValue(val);
    QString subTypeVal = QString::fromAscii("SIP");
    QMultiHash<QString,QString> params;
    params.insert(QString::fromAscii(versitType),subTypeVal);
    nameProperty.setParameters(params);
    document.addProperty(nameProperty);
    contact = mGenerator->generateContact(document);
    online = (QContactOnlineAccount)contact.detail(QContactOnlineAccount::DefinitionName);
    QCOMPARE(online.accountUri(),QString::fromAscii(val));
    QStringList subTypes = online.subTypes();
    QCOMPARE(subTypes.count(),1);
    QCOMPARE(online.subTypes().at(0),QContactOnlineAccount::SubTypeSip.operator QString());

    // with supported subtype : VideoSharing
    document = QVersitDocument();
    nameProperty = QVersitProperty();
    nameProperty.setName(QString::fromAscii(versitSipId));
    nameProperty.setValue(val);
    subTypeVal = QString::fromAscii("SWIS");
    params.clear();
    params.insert(QString::fromAscii(versitType),subTypeVal);
    nameProperty.setParameters(params);
    document.addProperty(nameProperty);
    contact = mGenerator->generateContact(document);
    online = (QContactOnlineAccount)contact.detail(QContactOnlineAccount::DefinitionName);
    QCOMPARE(online.accountUri(),QString::fromAscii(val));
    subTypes = online.subTypes();    
    QCOMPARE(subTypes.count(),1);
    QCOMPARE(online.subTypes().at(0),QContactOnlineAccount::SubTypeShareVideo.operator QString());

    // with unsupported subtype : POC
    document = QVersitDocument();
    nameProperty = QVersitProperty();
    nameProperty.setName(QString::fromAscii(versitSipId));
    nameProperty.setValue(val);
    subTypeVal = QString::fromAscii("POC");
    params.clear();
    params.insert(QString::fromAscii(versitType),subTypeVal);
    nameProperty.setParameters(params);
    document.addProperty(nameProperty);
    contact = mGenerator->generateContact(document);
    online = (QContactOnlineAccount)contact.detail(QContactOnlineAccount::DefinitionName);
    QCOMPARE(online.isEmpty(),true);
}

void UT_QVersitContactGenerator::testFamily()
{
    // Interesting : kid but no wife :)
    QVersitDocument document;
    QVersitProperty nameProperty;
    QByteArray val("Maggie"); // one is enough
    nameProperty.setName(QString::fromAscii(versitChildrenId));
    nameProperty.setValue(val);
    document.addProperty(nameProperty);
    QContact contact = mGenerator->generateContact(document);
    QContactFamily family = (QContactFamily)contact.detail(QContactFamily::DefinitionName);
    QStringList childrens = family.children();
    QCOMPARE(childrens.count(),1); // ensure no other kids in list
    QCOMPARE(family.spouse(),QString()); // make sure no wife
    QCOMPARE(childrens[0],QString::fromAscii(val)); // ensure it is your kid

    // Critical : wife but no kids , happy hours
    document = QVersitDocument();
    nameProperty = QVersitProperty();
    nameProperty.setName(QString::fromAscii(versitSpouseId));
    val = "March";
    nameProperty.setValue(val);
    document.addProperty(nameProperty);
    contact = mGenerator->generateContact(document);
    family = (QContactFamily)contact.detail(QContactFamily::DefinitionName);
    childrens = family.children();
    QCOMPARE(childrens.count(),0); // list should be empty as you know
    QCOMPARE(family.spouse(),QString::fromAscii(val)); // make sure thats your wife:(

    // Hopeless : couple of kids and wife
    document = QVersitDocument();
    // Add kids first
    nameProperty = QVersitProperty();
    nameProperty.setName(QString::fromAscii(versitChildrenId));
    QStringList kidsVal;
    kidsVal.append(QString::fromAscii("Bart"));
    kidsVal.append(QString::fromAscii("Liisa"));
    kidsVal.append(QString::fromAscii("Maggie"));
    nameProperty.setValue(kidsVal.join(QString::fromAscii(",")).toAscii());
    document.addProperty(nameProperty);
    // Add wife next
    val = "March";
    nameProperty = QVersitProperty();
    nameProperty.setName(QString::fromAscii(versitSpouseId));
    nameProperty.setValue(val);
    document.addProperty(nameProperty);
    contact = mGenerator->generateContact(document);
    family = (QContactFamily)contact.detail(QContactFamily::DefinitionName);
    childrens = family.children();
    QCOMPARE(childrens.count(),3); // too late , count them now.
    // painfull but ensure they are your kids
    QCOMPARE(childrens.join(QString::fromAscii(",")),kidsVal.join(QString::fromAscii(",")));
    QCOMPARE(family.spouse(),QString::fromAscii(val)); // make sure thats your wife:(
}

void UT_QVersitContactGenerator::testSound()
{
    QString path(imageAndAudioClipPath);
    mGenerator->setAudioClipPath(path);
    QVersitDocument document;
    QVersitProperty nameProperty;
    nameProperty.setName(QString::fromAscii(versitNameId));
    QStringList nameValues(QString::fromAscii("Homer")); // First name
    nameValues.append(QString::fromAscii("Simpson")); // Last name
    QByteArray name = nameValues.join(QString::fromAscii(";")).toAscii();
    nameProperty.setValue(name);
    document.addProperty(nameProperty);
    nameProperty = QVersitProperty();
    QMultiHash<QString,QString> param;
    param.insert(QString::fromAscii("TYPE"),QString::fromAscii("WAVE"));
    param.insert(QString::fromAscii("ENCODING"),QString::fromAscii("BASE64"));
    nameProperty.setName(QString::fromAscii(versitSoundId));
    QByteArray val("111110000011111");
    nameProperty.setValue(val.toBase64());
    nameProperty.setParameters(param);
    document.addProperty(nameProperty);
    QContact contact = mGenerator->generateContact(document);
    QContactAvatar avatar = (QContactAvatar)contact.detail(QContactAvatar::DefinitionName);
    QCOMPARE(avatar.value(QContactAvatar::FieldSubType),QContactAvatar::SubTypeAudioRingtone.operator QString());
    QDir dir(path);    
    QStringList type(QString::fromAscii("*.wav"));
    QStringList files = dir.entryList(type);
    QCOMPARE(files.count(),1);
    QString fileName = avatar.avatar();    
    QVERIFY(fileName.endsWith(QString::fromAscii(".wav")));
    QFile file(fileName);
    QVERIFY(file.open( QIODevice::ReadOnly ));
    QByteArray content = file.readAll();
    QCOMPARE(content,val);
}

QVersitDocument UT_QVersitContactGenerator::createDocumentWithProperty(
    const QVersitProperty& property)
{
    QVersitDocument document;
    document.addProperty(property);
    return document;
}

QVersitDocument UT_QVersitContactGenerator::createDocumentWithNameAndPhoto(
    const QByteArray& name,
    QByteArray image,
    const QString& imageType,
    const QString& encoding)
{
    QVersitDocument document;

    QVersitProperty nameProperty;
    nameProperty.setName(QString::fromAscii(versitNameId));
    nameProperty.setValue(name);
    document.addProperty(nameProperty);

    QVersitProperty property;
    property.setName(QString::fromAscii(versitPhotoId));
    if (encoding == QString::fromAscii(versitEncodingBase64) ||
        encoding == QString::fromAscii(versitEncodingBinary)){
        property.setValue(image.toBase64());
    } else {
        property.setValue(image);
    }
    if (imageType != QString()) {
        property.addParameter(QString::fromAscii(versitType), imageType);
    }
    if (encoding != QString()) {
        property.addParameter(QString::fromAscii(versitEncoding), encoding);
    }
    document.addProperty(property);

    return document;
}
