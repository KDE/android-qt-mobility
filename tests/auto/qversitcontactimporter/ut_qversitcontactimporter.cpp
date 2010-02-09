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

#include "qversitdefs_p.h"
#include "ut_qversitcontactimporter.h"
#include "qversitcontactimporter.h"
#include "qversitcontactimporter_p.h"
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

QTM_BEGIN_NAMESPACE
class MyQVersitContactImporterPropertyHandler : public QVersitContactImporterPropertyHandler
{
public:
    MyQVersitContactImporterPropertyHandler()
        : mPreProcess(false)
    {
    }

    bool preProcessProperty(const QVersitDocument& document,
                            const QVersitProperty& property,
                            int contactIndex,
                            QContact* contact)
    {
        Q_UNUSED(document)
        Q_UNUSED(contact)
        Q_UNUSED(contactIndex);
        mPreProcessedProperties.append(property);
        return mPreProcess;
    }

    bool postProcessProperty(const QVersitDocument& document,
                             const QVersitProperty& property,
                             bool alreadyProcessed,
                             int contactIndex,
                             QContact* contact)
    {
        Q_UNUSED(document)
        Q_UNUSED(contact)
        Q_UNUSED(contactIndex)
        if (!alreadyProcessed)
            mUnknownProperties.append(property);
        else
            mPostProcessedProperties.append(property);
        return false;
    }

    void clear()
    {
        mPreProcess = false;
        mPropertyNamesToProcess.clear();
        mUnknownProperties.clear();
        mPreProcessedProperties.clear();
        mPostProcessedProperties.clear();
    }

    // a hook to control what preProcess returns:
    bool mPreProcess;
    QStringList mPropertyNamesToProcess;
    QList<QVersitProperty> mUnknownProperties;
    QList<QVersitProperty> mPreProcessedProperties;
    QList<QVersitProperty> mPostProcessedProperties;
};

class MyQVersitResourceHandler : public QVersitResourceHandler
{
public:
    MyQVersitResourceHandler() : mIndex(0)
    {
    }

    bool saveResource(const QByteArray& contents, const QVersitProperty& property,
                      QString* location)
    {
        Q_UNUSED(property);
        *location = QString::number(mIndex++);
        mObjects.insert(*location, contents);
        return true;
    }

    bool loadResource(const QString &location, QByteArray *contents, QString *mimeType)
    {
        Q_UNUSED(location)
        Q_UNUSED(contents)
        Q_UNUSED(mimeType)
        return false;
    }

    void clear()
    {
        mIndex = 0;
        mObjects.clear();
    }

    int mIndex;
    QMap<QString, QByteArray> mObjects;
};

const static QByteArray SAMPLE_GIF(QByteArray::fromBase64(
        "R0lGODlhEgASAIAAAAAAAP///yH5BAEAAAEALAAAAAASABIAAAIdjI+py+0G"
        "wEtxUmlPzRDnzYGfN3KBaKGT6rDmGxQAOw=="));

QTM_END_NAMESPACE

QTM_USE_NAMESPACE

void UT_QVersitContactImporter::init()
{
    mImporter = new QVersitContactImporter();
    mImporterPrivate = new QVersitContactImporterPrivate();
    mResourceHandler = new MyQVersitResourceHandler();
    mImporter->setResourceHandler(mResourceHandler);
    mPropertyHandler = new MyQVersitContactImporterPropertyHandler();
    mImporter->setPropertyHandler(mPropertyHandler);
}

void UT_QVersitContactImporter::cleanup()
{
    QVERIFY(mImporter->propertyHandler() == mPropertyHandler);
    mImporter->setPropertyHandler(0);
    delete mPropertyHandler;
    QVERIFY(mImporter->resourceHandler() == mResourceHandler);
    mImporter->setResourceHandler(0);
    delete mResourceHandler;
    delete mImporter;
    delete mImporterPrivate;
}

void UT_QVersitContactImporter::testName()
{
    QVersitDocument document;
    QVersitProperty nameProperty;
    QStringList value;
    value.append(QString::fromAscii("John"));//FirstName
    value.append(QString::fromAscii("Citizen"));//LastName
    value.append(QString::fromAscii("Anonymous"));//GivenName
    value.append(QString::fromAscii("Dr"));//PreFix
    value.append(QString::fromAscii("MSc"));//Suffix
    nameProperty.setName(QString::fromAscii("N"));
    nameProperty.setValue(value.join(QString::fromAscii(";")));
    document.addProperty(nameProperty);
    QList<QVersitDocument> documentList;
    documentList.append(document);
    QContact contact = mImporter->importContacts(documentList).first();
    QContactName name = (QContactName)contact.detail(QContactName::DefinitionName);
    QCOMPARE(name.lastName(),value[0]);
    QCOMPARE(name.firstName(),value[1]);
    QCOMPARE(name.middleName(),value[2]);
    QCOMPARE(name.prefix(),value[3]);
    QCOMPARE(name.suffix(),value[4]);

    // Multiple names, first one will be picked and rest will be discarded
    nameProperty = QVersitProperty();
    QStringList anotherValue;
    anotherValue.append(QString::fromAscii("FakeJohn"));//FirstName
    anotherValue.append(QString::fromAscii("FakeCitizen"));//LastName
    anotherValue.append(QString::fromAscii("FakeAnonymous"));//GivenName
    anotherValue.append(QString::fromAscii("FakeDr"));//PreFix
    anotherValue.append(QString::fromAscii("FakeMSc"));//Suffix
    nameProperty.setName(QString::fromAscii("N"));
    nameProperty.setValue(anotherValue.join(QString::fromAscii(";")));
    document.addProperty(nameProperty);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    QList<QContactDetail> names = contact.details(QContactName::DefinitionName);
    QCOMPARE(names.count(),1);
    // anotherValue should be discarded, so check for value
    name = (QContactName)names[0];
    QCOMPARE(name.lastName(),value[0]);
    QCOMPARE(name.firstName(),value[1]);
    QCOMPARE(name.middleName(),value[2]);
    QCOMPARE(name.prefix(),value[3]);
    QCOMPARE(name.suffix(),value[4]);
}

// check that it doesn't crash if the FN property comes before the N property.
void UT_QVersitContactImporter::testNameWithFormatted()
{
    QVersitDocument document;
    QVersitProperty fnProperty;
    fnProperty.setName(QString::fromAscii("FN"));
    fnProperty.setValue(QString::fromAscii("First Last"));
    document.addProperty(fnProperty);
    QVersitProperty nProperty;
    nProperty.setName(QString::fromAscii("N"));
    nProperty.setValue(QString::fromAscii("Last;First;Middle;Prefix;Suffix"));
    document.addProperty(nProperty);
    QList<QVersitDocument> documentList;
    documentList.append(document);
    QContact contact = mImporter->importContacts(documentList).first();
    QContactName name = static_cast<QContactName>(contact.detail(QContactName::DefinitionName));
    QCOMPARE(name.firstName(), QString::fromAscii("First"));
    QCOMPARE(name.lastName(), QString::fromAscii("Last"));
    QCOMPARE(name.middleName(), QString::fromAscii("Middle"));
    QCOMPARE(name.prefix(), QString::fromAscii("Prefix"));
    QCOMPARE(name.suffix(), QString::fromAscii("Suffix"));
    QCOMPARE(name.customLabel(), QString::fromAscii("First Last"));
}

void UT_QVersitContactImporter::testAddress()
{
    QVersitDocument document;
    QVersitProperty property;
    property.setName(QString::fromAscii("ADR"));
    
    // Empty value for the address
    document = createDocumentWithProperty(property);
    QList<QVersitDocument> documentList;
    documentList.append(document);
    QContact contact = mImporter->importContacts(documentList).first();
    QContactAddress address = 
        static_cast<QContactAddress>(contact.detail(QContactAddress::DefinitionName));
    QCOMPARE(address.postOfficeBox(),QString());
    QCOMPARE(address.street(),QString());
    QCOMPARE(address.locality(),QString());
    QCOMPARE(address.region(),QString());
    QCOMPARE(address.postcode(),QString());
    QCOMPARE(address.country(),QString());
    
    // Address with just seprators
    property.setValue(QString::fromAscii(";;;;;;"));
    document = createDocumentWithProperty(property);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    address = static_cast<QContactAddress>(contact.detail(QContactAddress::DefinitionName));
    QCOMPARE(address.postOfficeBox(),QString());
    QCOMPARE(address.street(),QString());
    QCOMPARE(address.locality(),QString());
    QCOMPARE(address.region(),QString());
    QCOMPARE(address.postcode(),QString());
    QCOMPARE(address.country(),QString());
    
    // Address with some fields missing
    property.setValue(QString::fromAscii(";;My Street;My Town;;12345;"));
    document = createDocumentWithProperty(property);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    address = static_cast<QContactAddress>(contact.detail(QContactAddress::DefinitionName));
    QCOMPARE(address.postOfficeBox(),QString());
    QCOMPARE(address.street(),QString::fromAscii("My Street"));
    QCOMPARE(address.locality(),QString::fromAscii("My Town"));
    QCOMPARE(address.region(),QString());
    QCOMPARE(address.postcode(),QString::fromAscii("12345"));
    QCOMPARE(address.country(),QString());
    
    // Address with all the fields filled
    property.setValue(QString::fromAscii("PO Box;E;My Street;My Town;My State;12345;My Country"));
    document = createDocumentWithProperty(property);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    address = static_cast<QContactAddress>(contact.detail(QContactAddress::DefinitionName));
    QCOMPARE(address.postOfficeBox(),QString::fromAscii("PO Box"));
    QCOMPARE(address.street(),QString::fromAscii("My Street"));
    QCOMPARE(address.locality(),QString::fromAscii("My Town"));
    QCOMPARE(address.region(),QString::fromAscii("My State"));
    QCOMPARE(address.postcode(),QString::fromAscii("12345"));
    QCOMPARE(address.country(),QString::fromAscii("My Country"));
    
    // Address with TYPE parameters converted to contexts and subtypes
    property.insertParameter(QString::fromAscii("TYPE"),QString::fromAscii("HOME"));
    property.insertParameter(QString::fromAscii("TYPE"),QString::fromAscii("WORK"));
    property.insertParameter(QString::fromAscii("TYPE"),QString::fromAscii("DOM"));
    property.insertParameter(QString::fromAscii("TYPE"),QString::fromAscii("INTL"));
    property.insertParameter(QString::fromAscii("TYPE"),QString::fromAscii("POSTAL"));
    property.insertParameter(QString::fromAscii("TYPE"),QString::fromAscii("PARCEL"));
    property.insertParameter(QString::fromAscii("TYPE"),QString::fromAscii("X-EXTENSION"));
    document = createDocumentWithProperty(property);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
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

void UT_QVersitContactImporter::testOrganizationName()
{
    QVersitDocument document;
    QVersitProperty property;

    // Empty value for the organization
    property.setName(QString::fromAscii("ORG"));
    document = createDocumentWithProperty(property);
    QList<QVersitDocument> documentList;
    documentList.append(document);
    QContact contact = mImporter->importContacts(documentList).first();
    QContactOrganization organization =
        static_cast<QContactOrganization>(
            contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(organization.name(),QString());
    QCOMPARE(organization.department().count(),0);

    // Organization without separators
    property.setValue(QString::fromAscii("Nokia"));
    document = createDocumentWithProperty(property);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    organization =
        static_cast<QContactOrganization>(
            contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(organization.name(),QString::fromAscii("Nokia"));
    QCOMPARE(organization.department().count(),0);

    // Organization with one separator
    property.setValue(QString::fromAscii(";"));
    document = createDocumentWithProperty(property);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    organization =
        static_cast<QContactOrganization>(
            contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(organization.name(),QString::fromAscii(""));
    QCOMPARE(organization.department().count(),0);

    // Organization with just separators
    property.setValue(QString::fromAscii(";;;"));
    document = createDocumentWithProperty(property);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    organization =
        static_cast<QContactOrganization>(
            contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(organization.name(),QString::fromAscii(""));
    QCOMPARE(organization.department().count(),0);

    // Organization with one Organizational Unit
    property.setValue(QString::fromAscii("Nokia;R&D"));
    document = createDocumentWithProperty(property);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    organization =
        static_cast<QContactOrganization>(
            contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(organization.name(),QString::fromAscii("Nokia"));
    QCOMPARE(organization.department().count(),1);
    QCOMPARE(organization.department().at(0),QString::fromAscii("R&D"));

    // Organization with organization name and semicolon
    property.setValue(QString::fromAscii("Nokia;"));
    document = createDocumentWithProperty(property);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    organization =
        static_cast<QContactOrganization>(
            contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(organization.name(),QString::fromAscii("Nokia"));
    QCOMPARE(organization.department().count(),0);

    // Organization with semicolon and department
    property.setValue(QString::fromAscii(";R&D"));
    document = createDocumentWithProperty(property);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    organization =
        static_cast<QContactOrganization>(
            contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(organization.name(),QString());
    QCOMPARE(organization.department().count(),1);
    QCOMPARE(organization.department().at(0),QString::fromAscii("R&D"));

    // Organization with more Organizational Units
    property.setValue(QString::fromAscii("Nokia;R&D;Devices;Qt"));
    document = createDocumentWithProperty(property);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    organization =
        static_cast<QContactOrganization>(
            contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(organization.name(),QString::fromAscii("Nokia"));
    QCOMPARE(organization.department().count(),3);
    QCOMPARE(organization.department().at(0),QString::fromAscii("R&D"));
    QCOMPARE(organization.department().at(1),QString::fromAscii("Devices"));
    QCOMPARE(organization.department().at(2),QString::fromAscii("Qt"));
}

void UT_QVersitContactImporter::testOrganizationTitle()
{
    QVersitDocument document;
    QVersitProperty property;

    // One title
    property.setName(QString::fromAscii("TITLE"));
    QString titleValue(QString::fromAscii("Developer"));
    property.setValue(titleValue);
    document = createDocumentWithProperty(property);
    QList<QVersitDocument> documentList;
    documentList.append(document);
    QContact contact = mImporter->importContacts(documentList).first();
    QList<QContactDetail> organizationDetails =
        contact.details(QContactOrganization::DefinitionName);
    QCOMPARE(organizationDetails.count(), 1);
    QContactOrganization organization =
        static_cast<QContactOrganization>(organizationDetails[0]);
    QCOMPARE(organization.title(),titleValue);

    // Two titles -> two QContactOrganizations created
    property.setName(QString::fromAscii("TITLE"));
    QString secondTitleValue(QString::fromAscii("Hacker"));
    property.setValue(secondTitleValue);
    document.addProperty(property);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    organizationDetails = contact.details(QContactOrganization::DefinitionName);
    QCOMPARE(organizationDetails.count(), 2);
    QContactOrganization firstOrganization =
        static_cast<QContactOrganization>(organizationDetails[0]);
    QCOMPARE(firstOrganization.title(),titleValue);
    QContactOrganization secondOrganization =
        static_cast<QContactOrganization>(organizationDetails[1]);
    QCOMPARE(secondOrganization.title(),secondTitleValue);

    // Two titles and one organization name -> two QContactOrganizations created
    property.setName(QString::fromAscii("ORG"));
    QString organizationName(QString::fromAscii("Nokia"));
    property.setValue(organizationName);
    document.addProperty(property);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    organizationDetails = contact.details(QContactOrganization::DefinitionName);
    QCOMPARE(organizationDetails.count(), 2);
    firstOrganization = static_cast<QContactOrganization>(organizationDetails[0]);
    QCOMPARE(firstOrganization.title(),titleValue);
    QCOMPARE(firstOrganization.name(),organizationName);
    secondOrganization = static_cast<QContactOrganization>(organizationDetails[1]);
    QCOMPARE(secondOrganization.title(),secondTitleValue);
    QCOMPARE(secondOrganization.name(),QString());
}

void UT_QVersitContactImporter::testOrganizationAssistant()
{
    QContact contact;
    QVersitDocument document;
    QVersitProperty property;
    property.setName(QString::fromAscii("X-ASSISTANT"));
    QString assistantValue(QString::fromAscii("Jenny"));
    property.setValue(assistantValue);
    document = createDocumentWithProperty(property);
    QList<QVersitDocument> documentList;
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    QContactOrganization organization =
        static_cast<QContactOrganization>(
            contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(organization.assistantName(), assistantValue);
}

void UT_QVersitContactImporter::testOrganizationLogo()
{
    QContact contact;
    QVersitDocument document;
    QVersitProperty property;
    QList<QVersitDocument> documentList;

    // Embedded LOGO
    property.setName(QString::fromAscii("LOGO"));
    QByteArray logo(QByteArray::fromBase64(
            "R0lGODlhEgASAIAAAAAAAP///yH5BAEAAAEALAAAAAASABIAAAIdjI+py+0G"));
    property.setValue(logo);
    property.insertParameter(QString::fromAscii("TYPE"),
                          QString::fromAscii("GIF"));
    document = createDocumentWithProperty(property);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    QContactOrganization organization =
        static_cast<QContactOrganization>(contact.detail(QContactOrganization::DefinitionName));
    QByteArray content = mResourceHandler->mObjects.value(organization.logo());
    QCOMPARE(content, logo);

    // LOGO as a URL
    property.setName(QString::fromAscii("LOGO"));
    QString logoUrl(QString::fromAscii("http://www.organization.org/logo.gif"));
    property.setValue(logoUrl);
    property.insertParameter(QString::fromAscii("VALUE"),QString::fromAscii("URL"));
    document = createDocumentWithProperty(property);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    organization =
        static_cast<QContactOrganization>(
            contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(organization.logo(),logoUrl);
}

void UT_QVersitContactImporter::testOrganizationRole()
{
    QContact contact;
    QVersitDocument document;
    QVersitProperty property;

    // Setting the role is not yet supported by QContactOrganization
    property.setName(QString::fromAscii("ROLE"));
    QString roleValue(QString::fromAscii("Very important manager and proud of it"));
    property.setValue(roleValue);
    document = createDocumentWithProperty(property);
    QList<QVersitDocument> documentList;
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    QContactOrganization organization =
        static_cast<QContactOrganization>(
            contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(organization.role(), roleValue);
}

void UT_QVersitContactImporter::testTel()
{
    QVersitDocument document;
    QVersitProperty property;
    property.setName(QString::fromAscii("TEL"));
    QString value(QString::fromAscii("+35850987654321"));
    property.setValue(value);   

    property.insertParameter(QString::fromAscii("TYPE"),QString::fromAscii("VOICE"));
    property.insertParameter(QString::fromAscii("TYPE"),QString::fromAscii("CELL"));
    property.insertParameter(QString::fromAscii("TYPE"),QString::fromAscii("MODEM"));
    property.insertParameter(QString::fromAscii("TYPE"),QString::fromAscii("CAR"));
    property.insertParameter(QString::fromAscii("TYPE"),QString::fromAscii("VIDEO"));
    property.insertParameter(QString::fromAscii("TYPE"),QString::fromAscii("FAX"));
    property.insertParameter(QString::fromAscii("TYPE"),QString::fromAscii("BBS"));
    property.insertParameter(QString::fromAscii("TYPE"),QString::fromAscii("PAGER"));
    property.insertParameter(QString::fromAscii("TYPE"),QString::fromAscii("HOME"));
    property.insertParameter(QString::fromAscii("TYPE"),QString::fromAscii("WORK"));

    document.addProperty(property);
    QList<QVersitDocument> documentList;
    documentList.append(document);
    QContact contact = mImporter->importContacts(documentList).first();
    const QContactPhoneNumber& phone = 
        static_cast<QContactPhoneNumber>(
            contact.detail(QContactPhoneNumber::DefinitionName));
    QCOMPARE(phone.number(),QString(value));

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

void UT_QVersitContactImporter::testEmail()
{
    QVersitProperty property;
    property.setName(QString::fromAscii("EMAIL"));
    QString value(QString::fromAscii("john.citizen@example.com"));
    property.setValue(value);
    property.insertParameter(QString::fromAscii("TYPE"),QString::fromAscii("WORK"));
    QVersitDocument document = createDocumentWithProperty(property);
    QList<QVersitDocument> documentList;
    documentList.append(document);
    QContact contact = mImporter->importContacts(documentList).first();
    QContactEmailAddress email =
        static_cast<QContactEmailAddress>(
            contact.detail(QContactEmailAddress::DefinitionName));
    QCOMPARE(email.emailAddress(),value);
    const QStringList contexts = email.contexts();
    QCOMPARE(contexts.count(),1);
    QVERIFY(contexts.contains(QContactDetail::ContextWork)); 
}

void UT_QVersitContactImporter::testUrl()
{
    QVersitProperty property;
    property.setName(QString::fromAscii("URL"));
    QString value(QString::fromAscii("http://example.com"));
    property.setValue(value);
    property.insertParameter(QString::fromAscii("TYPE"),QString::fromAscii("WORK"));
    QVersitDocument document = createDocumentWithProperty(property);
    QList<QVersitDocument> documentList;
    documentList.append(document);
    QContact contact = mImporter->importContacts(documentList).first();
    QContactUrl url =
        static_cast<QContactUrl>(
            contact.detail(QContactUrl::DefinitionName));
    QCOMPARE(url.url(),value);
    const QStringList contexts = url.contexts();
    QCOMPARE(contexts.count(),1);
    QVERIFY(contexts.contains(QContactDetail::ContextWork));    
}

void UT_QVersitContactImporter::testUid()
{
    QVersitProperty property;
    property.setName(QString::fromAscii("UID"));
    QString value(QString::fromAscii("unique identifier"));
    property.setValue(value);
    QVersitDocument document = createDocumentWithProperty(property);
    QList<QVersitDocument> documentList;
    documentList.append(document);
    QContact contact = mImporter->importContacts(documentList).first();
    QContactGuid uid =
        static_cast<QContactGuid>(
            contact.detail(QContactGuid::DefinitionName));
    QCOMPARE(uid.guid(),value);
}

void UT_QVersitContactImporter::testTimeStamp()
{
    // Simple date : ISO 8601 extended format
    QVersitProperty property;
    property.setName(QString::fromAscii("REV"));
    QString dateValue(QString::fromAscii("1981-05-20"));
    property.setValue(dateValue);
    QVersitDocument document = createDocumentWithProperty(property);
    QList<QVersitDocument> documentList;
    documentList.append(document);
    QContact contact = mImporter->importContacts(documentList).first();
    QContactTimestamp timeStamp =
        static_cast<QContactTimestamp>(
            contact.detail(QContactTimestamp::DefinitionName));    
    QCOMPARE(timeStamp.lastModified().date().toString(Qt::ISODate),dateValue);

    // Date and Time : ISO 8601 extended format without utc offset
    QString dateAndTimeValue(QString::fromAscii("1981-05-20T23:55:55"));
    property.setValue(dateAndTimeValue);
    document = createDocumentWithProperty(property);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    timeStamp =
        static_cast<QContactTimestamp>(
            contact.detail(QContactTimestamp::DefinitionName));    
    QCOMPARE(timeStamp.lastModified().toString(Qt::ISODate),dateAndTimeValue);

    // Date and Time : ISO 8601 extented format with utc offset
    QString utcOffset(QString::fromAscii("Z"));
    QString dateAndTimeWithUtcValue = dateAndTimeValue+utcOffset;
    property.setValue(dateAndTimeWithUtcValue);
    document = createDocumentWithProperty(property);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    timeStamp =
        static_cast<QContactTimestamp>(
            contact.detail(QContactTimestamp::DefinitionName));
    QCOMPARE(timeStamp.lastModified().toString(Qt::ISODate),dateAndTimeValue);
    QCOMPARE(timeStamp.lastModified().timeSpec(),Qt::UTC);

    // Date and Time : ISO 8601 in basic format without utc offset
    dateAndTimeValue = QString::fromAscii("19810520T235555");
    property.setValue(dateAndTimeValue);
    document = createDocumentWithProperty(property);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    timeStamp =
        static_cast<QContactTimestamp>(
            contact.detail(QContactTimestamp::DefinitionName));

    QCOMPARE(timeStamp.lastModified().toString(QString::fromAscii("yyyyMMddThhmmss")),
             dateAndTimeValue);

    // Date and Time : ISO 8601 in basic format with utc offset
    dateAndTimeValue = QString::fromAscii("19810520T235555");
    dateAndTimeWithUtcValue = dateAndTimeValue+utcOffset;
    property.setValue(dateAndTimeWithUtcValue);
    document = createDocumentWithProperty(property);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    timeStamp =
        static_cast<QContactTimestamp>(
            contact.detail(QContactTimestamp::DefinitionName));
    QCOMPARE(timeStamp.lastModified().toString(QString::fromAscii("yyyyMMddThhmmss")),
             dateAndTimeValue);
    QCOMPARE(timeStamp.lastModified().timeSpec(),Qt::UTC);
}

void UT_QVersitContactImporter::testAnniversary()
{
    // Date : ISO 8601 extended format
    QVersitProperty property;
    property.setName(QString::fromAscii("X-ANNIVERSARY"));
    QString dateValue(QString::fromAscii("1981-05-20"));
    property.setValue(dateValue);
    QVersitDocument document = createDocumentWithProperty(property);
    QList<QVersitDocument> documentList;
    documentList.append(document);
    QContact contact = mImporter->importContacts(documentList).first();
    QContactAnniversary anniversary =
        static_cast<QContactAnniversary>(
            contact.detail(QContactAnniversary::DefinitionName));
    QCOMPARE(anniversary.originalDate().toString(Qt::ISODate),dateValue);

    // Date : ISO 8601 in basic format
    dateValue = QString::fromAscii("19810520");
    property.setValue(dateValue);
    document = createDocumentWithProperty(property);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    anniversary =
        static_cast<QContactAnniversary>(
            contact.detail(QContactAnniversary::DefinitionName));
    QCOMPARE(anniversary.originalDate().toString(QString::fromAscii("yyyyMMdd")),
             dateValue);

}

void UT_QVersitContactImporter::testBirthday()
{
    // Date : ISO 8601 extended format
    QVersitProperty property;
    property.setName(QString::fromAscii("BDAY"));
    QString dateValue(QString::fromAscii("1981-05-20"));
    property.setValue(dateValue);
    QVersitDocument document = createDocumentWithProperty(property);
    QList<QVersitDocument> documentList;
    documentList.append(document);
    QContact contact = mImporter->importContacts(documentList).first();
    QContactBirthday bday =
        static_cast<QContactBirthday>(
            contact.detail(QContactBirthday::DefinitionName));
    QCOMPARE(bday.date().toString(Qt::ISODate),
             dateValue);

    // Date : ISO 8601 in basic format
    dateValue = QString::fromAscii("19810520");
    property.setValue(dateValue);
    document = createDocumentWithProperty(property);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    bday =
        static_cast<QContactBirthday>(
            contact.detail(QContactBirthday::DefinitionName));
    QCOMPARE(bday.date().toString(QString::fromAscii("yyyyMMdd")),
             dateValue);

}

void UT_QVersitContactImporter::testGender()
{
    // Date : ISO 8601 extended format
    QVersitProperty property;
    property.setName(QString::fromAscii("X-GENDER"));
    QString val(QString::fromAscii("Male"));
    property.setValue(val);
    QVersitDocument document = createDocumentWithProperty(property);
    QList<QVersitDocument> documentList;
    documentList.append(document);
    QContact contact = mImporter->importContacts(documentList).first();
    QContactGender  gender =
        static_cast<QContactGender >(
            contact.detail(QContactGender ::DefinitionName));
    QCOMPARE(gender.gender(),val);
}

void UT_QVersitContactImporter::testNickname()
{
    // one value
    QVersitDocument document;
    QVersitProperty nameProperty;
    QString singleVal(QString::fromAscii("Homie"));
    nameProperty.setName(QString::fromAscii("NICKNAME"));
    nameProperty.setValue(singleVal);
    document.addProperty(nameProperty);
    QList<QVersitDocument> documentList;
    documentList.append(document);
    QContact contact = mImporter->importContacts(documentList).first();
    QContactNickname nickName = (QContactNickname)contact.detail(QContactNickname::DefinitionName);
    QCOMPARE(nickName.nickname(),singleVal);

    // comma separated values should generate multiple nickname fields
    contact.clearDetails();
    document = QVersitDocument();
    QStringList multiVal;
    multiVal.append(QString::fromAscii("Homie"));
    multiVal.append(QString::fromAscii("SuperHero"));
    multiVal.append(QString::fromAscii("NukeSpecialist"));
    nameProperty.setName(QString::fromAscii("NICKNAME"));
    nameProperty.setValue(multiVal.join(QString::fromAscii(",")));
    document.addProperty(nameProperty);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
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
    nameProperty.setName(QString::fromAscii("X-NICKNAME"));
    nameProperty.setValue(singleVal);
    document.addProperty(nameProperty);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    nickName =
        static_cast<QContactNickname>(
            contact.detail(QContactNickname::DefinitionName));
    QCOMPARE(nickName.nickname(),singleVal);
}

void UT_QVersitContactImporter::testAvatarStored()
{
    QByteArray gif(SAMPLE_GIF);
    QStringList nameValues(QString::fromAscii("John")); // First name
    nameValues.append(QString::fromAscii("Citizen")); // Last name
    QString name = nameValues.join(QString::fromAscii(";"));
    QVersitDocument document = createDocumentWithNameAndPhoto(name, gif, QLatin1String("GIF"));
    QList<QVersitDocument> documentList;
    documentList.append(document);
    QContact contact = mImporter->importContacts(documentList).first();
    QContactDetail detail = contact.detail(QContactAvatar::DefinitionName);
    QVERIFY(!detail.isEmpty());
    QContactAvatar avatar = static_cast<QContactAvatar>(detail);
    QVERIFY(avatar.subType() == QContactAvatar::SubTypeImage);
    QByteArray content = mResourceHandler->mObjects.value(avatar.avatar());
    QCOMPARE(content, gif);
    QPixmap pixmap(avatar.pixmap());
    QPixmap expectedPixmap;
    expectedPixmap.loadFromData(gif);
    QCOMPARE(pixmap, expectedPixmap);

    // Without the resource handler, the pixmap should still be set.
    mImporter->setResourceHandler(0);
    contact = mImporter->importContacts(documentList).first();
    avatar = contact.detail<QContactAvatar>();
    QVERIFY(avatar.subType() == QContactAvatar::SubTypeImage);
    QVERIFY(avatar.avatar().isEmpty());
    pixmap = avatar.pixmap();
    QCOMPARE(pixmap, expectedPixmap);

    // Empty photo.  The avatar should not be added to the QContact.
    QVersitProperty property;
    property.setName(QLatin1String("PHOTO"));
    property.setValue(QByteArray());
    document.clear();
    document.addProperty(property);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    QCOMPARE(contact.details(QContactAvatar::DefinitionName).size(), 0);

    mImporter->setResourceHandler(mResourceHandler);
}

void UT_QVersitContactImporter::testAvatarUrl()
{
    QVersitProperty property;
    property.setName(QLatin1String("PHOTO"));
    QString value(QLatin1String("http://example.com/example.jpg"));
    property.setValue(value);
    property.insertParameter(QLatin1String("VALUE"), QLatin1String("URL"));

    QVersitDocument document;
    document.addProperty(property);
    QList<QVersitDocument> documentList;
    documentList.append(document);

    QContact contact = mImporter->importContacts(documentList).first();
    QContactAvatar avatar =
        static_cast<QContactAvatar>(contact.detail(QContactAvatar::DefinitionName));
    QCOMPARE(avatar.avatar(), QLatin1String("http://example.com/example.jpg"));
    QVERIFY(avatar.subType() == QContactAvatar::SubTypeImage);


    // A URL disguised inside a QByteArray.
    document.clear();
    property.clear();
    property.setName(QLatin1String("PHOTO"));
    property.setValue(QByteArray("http://example.com/example.jpg"));
    property.insertParameter(QLatin1String("VALUE"), QLatin1String("URL"));
    property.insertParameter(QLatin1String("CHARSET"), QLatin1String("UTF-8"));
    document.addProperty(property);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    avatar =
        static_cast<QContactAvatar>(contact.detail(QContactAvatar::DefinitionName));
    QCOMPARE(avatar.avatar(), QLatin1String("http://example.com/example.jpg"));
    QVERIFY(avatar.subType() == QContactAvatar::SubTypeImage);
}

void UT_QVersitContactImporter::testAvatarInvalid()
{
    // An avatar that's a QVersitDocument?  It shouldn't work.
    QVersitDocument document;
    QVersitProperty property;
    property.setName(QLatin1String("PHOTO"));
    QVersitDocument nestedDocument;
    property.setValue(QVariant::fromValue(nestedDocument));
    property.insertParameter(QLatin1String("VALUE"), QLatin1String("URL"));
    document.addProperty(property);
    QList<QVersitDocument> list;
    list.append(document);
    QContact contact = mImporter->importContacts(list).first();
    QCOMPARE(contact.details(QContactAvatar::DefinitionName).size(), 0);

    document.clear();
    property.clear();
    list.clear();
    property.setName(QLatin1String("PHOTO"));
    property.setValue(QVariant::fromValue(nestedDocument));
    document.addProperty(property);
    list.append(document);
    contact = mImporter->importContacts(list).first();
    QCOMPARE(contact.details(QContactAvatar::DefinitionName).size(), 0);
}

void UT_QVersitContactImporter::testGeo()
{
    // some positive values
    QVersitDocument document;
    QVersitProperty nameProperty;
    QStringList val;    
    val.append(QString::fromAscii("18.53"));// Longtitude
    val.append(QString::fromAscii("45.32")); // Latitude
    nameProperty.setName(QString::fromAscii("GEO"));
    nameProperty.setValue(val.join(QString::fromAscii(",")));
    document.addProperty(nameProperty);
    QList<QVersitDocument> documentList;
    documentList.append(document);
    QContact contact = mImporter->importContacts(documentList).first();
    QContactGeoLocation geo = (QContactGeoLocation)contact.detail(QContactGeoLocation::DefinitionName);
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
    nameProperty.setName(QString::fromAscii("GEO"));
    nameProperty.setValue(val.join(QString::fromAscii(",")));
    document.addProperty(nameProperty);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    geo = (QContactGeolocation)contact.detail(QContactGeolocation::DefinitionName);
    str.setNum(geo.longitude(),'.',2);
    QCOMPARE(str,val[0]);
    str.setNum(geo.latitude(),'.',2);
    QCOMPARE(str,val[1]);
}

void UT_QVersitContactImporter::testNote()
{
    // single line value
    QVersitDocument document;
    QVersitProperty nameProperty;
    QString val(QString::fromAscii("I will not sleep at my work -John"));
    nameProperty.setName(QString::fromAscii("NOTE"));
    nameProperty.setValue(val);
    document.addProperty(nameProperty);
    QList<QVersitDocument> documentList;
    documentList.append(document);
    QContact contact = mImporter->importContacts(documentList).first();
    QContactNote note = (QContactNote)contact.detail(QContactNote::DefinitionName);
    QCOMPARE(note.note(),val);

    // Multiline value and quoted printable encoding
    document = QVersitDocument();
    nameProperty = QVersitProperty();
    val = QString::fromAscii("My Dad acts like he belongs,=0D=0AHe belongs in the zoo.=0D=0A");
    nameProperty.setName(QString::fromAscii("NOTE"));
    nameProperty.setValue(val);
    QMultiHash<QString,QString> params;
    params.insert(QString::fromAscii("QUOTED-PRINTABLE"),val);
    nameProperty.setParameters(params);
    document.addProperty(nameProperty);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    note = (QContactNote)contact.detail(QContactNote::DefinitionName);
    QCOMPARE(note.note(),val);
}

void UT_QVersitContactImporter::testLabel()
{
    QVersitDocument document;
    QVersitProperty nameProperty;
    QString val(QString::fromAscii("John Citizen"));
    nameProperty.setName(QString::fromAscii("FN"));
    nameProperty.setValue(val);
    document.addProperty(nameProperty);
    QList<QVersitDocument> documentList;
    documentList.append(document);
    QContact contact = mImporter->importContacts(documentList).first();
    QContactName name =
            (QContactName)contact.detail(QContactName::DefinitionName);
    QCOMPARE(name.customLabel(),val);
}

void UT_QVersitContactImporter::testOnlineAccount()
{
    QString accountUri(QString::fromAscii("sip:john.citizen@example.com"));

    // Plain X-SIP, no TYPE ->
    QVersitDocument document;
    QVersitProperty property;
    property.setName(QString::fromAscii("X-SIP"));
    property.setValue(accountUri);
    document.addProperty(property);
    QList<QVersitDocument> documentList;
    documentList.append(document);
    QContact contact = mImporter->importContacts(documentList).first();
    QContactOnlineAccount onlineAccount =
         static_cast<QContactOnlineAccount>(
             contact.detail(QContactOnlineAccount::DefinitionName));
    QCOMPARE(onlineAccount.accountUri(),accountUri);
    QStringList subTypes = onlineAccount.subTypes();
    QCOMPARE(subTypes.count(),1);
    QVERIFY(subTypes.first() == QContactOnlineAccount::SubTypeSip);

    // X-SIP;SWIS
    document = QVersitDocument();
    property = QVersitProperty();
    property.setName(QString::fromAscii("X-SIP"));
    property.setValue(accountUri);
    QMultiHash<QString,QString> params;
    params.insert(QString::fromAscii("TYPE"),QString::fromAscii("SWIS"));
    property.setParameters(params);
    document.addProperty(property);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    onlineAccount =
         static_cast<QContactOnlineAccount>(
             contact.detail(QContactOnlineAccount::DefinitionName));
    QCOMPARE(onlineAccount.accountUri(),accountUri);
    subTypes = onlineAccount.subTypes();
    QCOMPARE(subTypes.count(),1);
    QVERIFY(subTypes.first() == QContactOnlineAccount::SubTypeVideoShare);

    // X-SIP;VOIP
    document = QVersitDocument();
    property = QVersitProperty();
    property.setName(QString::fromAscii("X-SIP"));
    property.setValue(accountUri);
    params.clear();
    params.insert(QString::fromAscii("TYPE"),QString::fromAscii("VOIP"));
    property.setParameters(params);
    document.addProperty(property);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    onlineAccount =
         static_cast<QContactOnlineAccount>(
             contact.detail(QContactOnlineAccount::DefinitionName));
    QCOMPARE(onlineAccount.accountUri(),accountUri);
    subTypes = onlineAccount.subTypes();
    QCOMPARE(subTypes.count(),1);
    QVERIFY(subTypes.first() == QContactOnlineAccount::SubTypeSipVoip);

    // X-IMPP
    document = QVersitDocument();
    property = QVersitProperty();
    property.setName(QString::fromAscii("X-IMPP"));
    property.setValue(accountUri);
    document.addProperty(property);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    onlineAccount =
         static_cast<QContactOnlineAccount>(
             contact.detail(QContactOnlineAccount::DefinitionName));
    QCOMPARE(onlineAccount.accountUri(),accountUri);
    subTypes = onlineAccount.subTypes();
    QCOMPARE(subTypes.count(),1);
    QVERIFY(subTypes.first() == QContactOnlineAccount::SubTypeImpp);

    // IMPP
    document = QVersitDocument();
    property = QVersitProperty();
    property.setName(QString::fromAscii("IMPP"));
    property.setValue(accountUri);
    document.addProperty(property);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    onlineAccount =
         static_cast<QContactOnlineAccount>(
             contact.detail(QContactOnlineAccount::DefinitionName));
    QCOMPARE(onlineAccount.accountUri(),accountUri);
    subTypes = onlineAccount.subTypes();
    QCOMPARE(subTypes.count(),1);
    QVERIFY(subTypes.first() == QContactOnlineAccount::SubTypeImpp);
}

void UT_QVersitContactImporter::testFamily()
{
    // Interesting : kid but no wife :)
    QVersitDocument document;
    QVersitProperty nameProperty;
    QString val(QString::fromAscii("Jane")); // one is enough
    nameProperty.setName(QString::fromAscii("X-CHILDREN"));
    nameProperty.setValue(val);
    document.addProperty(nameProperty);
    QList<QVersitDocument> documentList;
    documentList.append(document);
    QContact contact = mImporter->importContacts(documentList).first();
    QContactFamily family = (QContactFamily)contact.detail(QContactFamily::DefinitionName);
    QStringList children = family.children();
    QCOMPARE(children.count(),1); // ensure no other kids in list
    QCOMPARE(family.spouse(),QString()); // make sure no wife
    QCOMPARE(children[0],val); // ensure it is your kid

    // Critical : wife but no kids , happy hours
    document = QVersitDocument();
    nameProperty = QVersitProperty();
    nameProperty.setName(QString::fromAscii("X-SPOUSE"));
    val = QString::fromAscii("Jenny");
    nameProperty.setValue(val);
    document.addProperty(nameProperty);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    family = (QContactFamily)contact.detail(QContactFamily::DefinitionName);
    children = family.children();
    QCOMPARE(children.count(),0); // list should be empty as you know
    QCOMPARE(family.spouse(),val); // make sure thats your wife:(

    // Hopeless : couple of kids and wife
    document = QVersitDocument();
    // Add kids first
    nameProperty = QVersitProperty();
    nameProperty.setName(QString::fromAscii("X-CHILDREN"));
    QStringList kidsVal;
    kidsVal.append(QString::fromAscii("James"));
    kidsVal.append(QString::fromAscii("Jake"));
    kidsVal.append(QString::fromAscii("Jane"));
    nameProperty.setValue(kidsVal.join(QString::fromAscii(",")));
    document.addProperty(nameProperty);
    // Add wife next
    val = QString::fromAscii("Jenny");
    nameProperty = QVersitProperty();
    nameProperty.setName(QString::fromAscii("X-SPOUSE"));
    nameProperty.setValue(val);
    document.addProperty(nameProperty);
    documentList.clear();
    documentList.append(document);
    contact = mImporter->importContacts(documentList).first();
    family = (QContactFamily)contact.detail(QContactFamily::DefinitionName);
    children = family.children();
    QCOMPARE(children.count(),3); // too late , count them now.
    // painfull but ensure they are your kids
    QCOMPARE(children.join(QString::fromAscii(",")),kidsVal.join(QString::fromAscii(",")));
    QCOMPARE(family.spouse(),val); // make sure thats your wife:(
}

void UT_QVersitContactImporter::testSound()
{
    QVersitDocument document;
    QVersitProperty nameProperty;
    nameProperty.setName(QString::fromAscii("N"));
    nameProperty.setValue(QString::fromAscii("Citizen;John;;;"));
    document.addProperty(nameProperty);
    nameProperty = QVersitProperty();
    QVersitProperty soundProperty;
    QMultiHash<QString,QString> param;
    param.insert(QString::fromAscii("TYPE"),QString::fromAscii("WAVE"));
    soundProperty.setName(QString::fromAscii("SOUND"));
    QByteArray val("111110000011111");
    soundProperty.setValue(val);
    soundProperty.setParameters(param);
    document.addProperty(soundProperty);
    QList<QVersitDocument> documents;
    documents.append(document);
    QContact contact = mImporter->importContacts(documents).first();
    QContactAvatar avatar = (QContactAvatar)contact.detail(QContactAvatar::DefinitionName);
    QCOMPARE(avatar.value(QContactAvatar::FieldSubType),QContactAvatar::SubTypeAudioRingtone.operator QString());
    QVERIFY(!avatar.hasValue(QContactAvatar::FieldAvatarPixmap));
    QByteArray content = mResourceHandler->mObjects.value(avatar.avatar());
    QCOMPARE(content, val);
}

void UT_QVersitContactImporter::testPropertyHandler()
{
    QVersitDocument document;
    QVersitProperty property;

    // No unconverted properties, no converted properties either
    QList<QVersitDocument> documents;
    documents.append(document);
    mImporter->importContacts(documents);
    QCOMPARE(mPropertyHandler->mUnknownProperties.size(), 0);
    QCOMPARE(mPropertyHandler->mPreProcessedProperties.size(), 0);
    QCOMPARE(mPropertyHandler->mPostProcessedProperties.size(), 0);

    // No unconverted properties, one converted property
    mPropertyHandler->clear();
    property.setName(QString::fromAscii("N"));
    property.setValue(QString::fromAscii("Citizen;John;Q;;"));
    document.addProperty(property);
    documents.clear();
    documents.append(document);
    QContact contact = mImporter->importContacts(documents).first();
    QCOMPARE(mPropertyHandler->mUnknownProperties.size(), 0);
    QCOMPARE(mPropertyHandler->mPreProcessedProperties.size(), 1);
    QCOMPARE(mPropertyHandler->mPostProcessedProperties.size(), 1);

    // Set the handler to override handling of the property
    mPropertyHandler->clear();
    mPropertyHandler->mPreProcess = true;
    document = QVersitDocument();
    property.setName(QString::fromAscii("N"));
    property.setValue(QString::fromAscii("Citizen;John;Q;;"));
    document.addProperty(property);
    documents.clear();
    documents.append(document);
    contact = mImporter->importContacts(documents).first();
    QCOMPARE(mPropertyHandler->mUnknownProperties.size(), 0);
    QCOMPARE(mPropertyHandler->mPreProcessedProperties.size(), 1);
    QCOMPARE(mPropertyHandler->mPostProcessedProperties.size(), 0);
    QContactDetail nameDetail = contact.detail(QContactName::DefinitionName);
    QVERIFY(nameDetail.isEmpty());

    // One unknown property
    mPropertyHandler->clear();
    property.setName(QString::fromAscii("X-EXTENSION-1"));
    property.setValue(QString::fromAscii("extension value 1"));
    document.addProperty(property);
    documents.clear();
    documents.append(document);
    mImporter->importContacts(documents);
    QList<QVersitProperty> unknownProperties = mPropertyHandler->mUnknownProperties;
    QCOMPARE(unknownProperties.count(), 1);
    QCOMPARE(unknownProperties[0].name(), QString::fromAscii("X-EXTENSION-1"));
    QCOMPARE(unknownProperties[0].value(), QString::fromAscii("extension value 1"));

    // Two unknown properties
    mPropertyHandler->clear();
    property.setName(QString::fromAscii("X-EXTENSION-2"));
    property.setValue(QString::fromAscii("extension value 2"));
    document.addProperty(property);
    documents.clear();
    documents.append(document);
    mImporter->importContacts(documents);
    unknownProperties = mPropertyHandler->mUnknownProperties;
    QCOMPARE(unknownProperties.count(), 2);
    QCOMPARE(unknownProperties[0].name(), QString::fromAscii("X-EXTENSION-1"));
    QCOMPARE(unknownProperties[0].value(), QString::fromAscii("extension value 1"));
    QCOMPARE(unknownProperties[1].name(), QString::fromAscii("X-EXTENSION-2"));
    QCOMPARE(unknownProperties[1].value(), QString::fromAscii("extension value 2"));
}

QVersitDocument UT_QVersitContactImporter::createDocumentWithProperty(
    const QVersitProperty& property)
{
    QVersitDocument document;
    document.addProperty(property);
    return document;
}

QVersitDocument UT_QVersitContactImporter::createDocumentWithNameAndPhoto(
    const QString& name,
    QByteArray image,
    const QString& imageType)
{
    QVersitDocument document;

    QVersitProperty nameProperty;
    nameProperty.setName(QString::fromAscii("N"));
    nameProperty.setValue(name);
    document.addProperty(nameProperty);

    QVersitProperty property;
    property.setName(QString::fromAscii("PHOTO"));
    property.setValue(image);
    if (imageType != QString()) {
        property.insertParameter(QString::fromAscii("TYPE"), imageType);
    }
    document.addProperty(property);

    return document;
}

QTEST_MAIN(UT_QVersitContactImporter)

