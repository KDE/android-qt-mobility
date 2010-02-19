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

#include <QtTest/QtTest>
#include <QObject>

#include <qtcontacts.h>

#ifdef SYMBIANSIM_BACKEND_USE_ETEL_TESTSERVER
#include <etelmm_etel_test_server.h>
#else
#include <etelmm.h>
#endif
#include <mmtsy_names.h>

QTM_USE_NAMESPACE

#define QCOMPARE_WITH_RETURN_VALUE(actual, expected) \
    if (!QTest::qCompare(actual, expected, #actual, #expected, __FILE__, __LINE__))\
        return false;

#define QVERIFY_WITH_RETURN_VALUE(statement) \
    if (!QTest::qVerify((statement), #statement, "", __FILE__, __LINE__))\
        return false;

#ifndef QTRY_COMPARE
#define QTRY_COMPARE(__expr, __expected) \
    do { \
        const int __step = 50; \
        const int __timeout = 5000; \
        if ((__expr) != (__expected)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && ((__expr) != (__expected)); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QCOMPARE(__expr, __expected); \
    } while(0)
#endif


//TESTED_CLASS=
//TESTED_FILES=

/*!
*/
class tst_QContactManagerSymbianSim : public QObject
{
Q_OBJECT

public:
    tst_QContactManagerSymbianSim();
    virtual ~tst_QContactManagerSymbianSim();

public slots:
    void init();
    void cleanup();
    void initTestCase();
    void cleanupTestCase();

private slots:
    /* Test cases that take no data */
    void hasFeature();
    void supportedContactTypes();
    void detailDefinitions();

    /* Test cases that need data */
    void addContact_data();
    void addContact();
    void updateContactDetail_data();
    void updateContactDetail();
    void fetchContactReq();
    void localIdFetchReq();
    void saveContactReq();
    void removeContactReq();
    void detailDefinitionFetchReq();
    void notSupportedRequests();
    
private:
    void getEtelStoreInfoL(const TDesC &phonebook, TDes8 &infoPckg) const;
    void parseDetails(QContact &contact, QStringList details, QList<QContactDetail> &parsedDetails);
    bool isContactSupported(QContact contact);
    bool compareDetails(QContact contact, QList<QContactDetail> expectedDetails);
    QContact createContact(QString name, QString number);
    QContact saveContact(QString name, QString number);

private:
    QContactManager* m_cm;
    RMobilePhoneBookStore::TMobilePhoneBookInfoV5 m_etelStoreInfo;
    RMobilePhoneBookStore::TMobilePhoneBookInfoV5Pckg m_etelStoreInfoPckg;
};

tst_QContactManagerSymbianSim::tst_QContactManagerSymbianSim() :
    m_etelStoreInfoPckg( m_etelStoreInfo )
{
	qRegisterMetaType<QContactAbstractRequest::State>("QContactAbstractRequest::State");
}

tst_QContactManagerSymbianSim::~tst_QContactManagerSymbianSim()
{
}

void tst_QContactManagerSymbianSim::init()
{
    
}

void tst_QContactManagerSymbianSim::cleanup()
{
    // remove all contacts
    QList<QContactLocalId> ids = m_cm->contactIds();
    m_cm->removeContacts(&ids, 0);   
}

void tst_QContactManagerSymbianSim::initTestCase()
{
    m_cm = QContactManager::fromUri("qtcontacts:symbiansim");
    QVERIFY(m_cm);
    TRAPD(err, getEtelStoreInfoL(KETelIccAdnPhoneBook, m_etelStoreInfoPckg));
    QCOMPARE(err, KErrNone);
}

void tst_QContactManagerSymbianSim::cleanupTestCase()
{
    delete m_cm;
    m_cm = 0;
}

void tst_QContactManagerSymbianSim::getEtelStoreInfoL(const TDesC &phonebook, TDes8 &infoPckg) const
{
    RTelServer etelServer;
    User::LeaveIfError(etelServer.Connect());
    CleanupClosePushL(etelServer);
    User::LeaveIfError(etelServer.LoadPhoneModule(KMmTsyModuleName));
 
    RMobilePhone etelPhone;
    RTelServer::TPhoneInfo info;
    User::LeaveIfError(etelServer.GetPhoneInfo(0, info));
    User::LeaveIfError(etelPhone.Open(etelServer, info.iName));
    CleanupClosePushL(etelPhone);

    //check what information can be saved to the Etel store
    RMobilePhoneBookStore etelStore;
    User::LeaveIfError(etelStore.Open(etelPhone, phonebook));
    CleanupClosePushL(etelStore);
    TRequestStatus requestStatus;
    etelStore.GetInfo(requestStatus, infoPckg);
    User::WaitForRequest(requestStatus);
    User::LeaveIfError(requestStatus.Int());

    CleanupStack::PopAndDestroy(&etelStore);
    CleanupStack::PopAndDestroy(&etelPhone);
    CleanupStack::PopAndDestroy(&etelServer);
}

void tst_QContactManagerSymbianSim::hasFeature()
{
    // TODO: Groups may be supported by some SIM cards?
    QVERIFY(!m_cm->hasFeature(QContactManager::Groups));
    QVERIFY(!m_cm->hasFeature(QContactManager::Groups, QContactType::TypeContact));
    QVERIFY(!m_cm->hasFeature(QContactManager::Groups, QContactType::TypeGroup));

    QVERIFY(!m_cm->hasFeature(QContactManager::ActionPreferences));
    QVERIFY(!m_cm->hasFeature(QContactManager::ActionPreferences, QContactType::TypeContact));
    QVERIFY(!m_cm->hasFeature(QContactManager::ActionPreferences, QContactType::TypeGroup));

    QVERIFY(!m_cm->hasFeature(QContactManager::MutableDefinitions));
    QVERIFY(!m_cm->hasFeature(QContactManager::MutableDefinitions, QContactType::TypeContact));
    QVERIFY(!m_cm->hasFeature(QContactManager::MutableDefinitions, QContactType::TypeGroup));

    QVERIFY(!m_cm->hasFeature(QContactManager::Relationships));
    QVERIFY(!m_cm->hasFeature(QContactManager::Relationships, QContactType::TypeContact));
    QVERIFY(!m_cm->hasFeature(QContactManager::Relationships, QContactType::TypeGroup));

    QVERIFY(!m_cm->hasFeature(QContactManager::ArbitraryRelationshipTypes));
    QVERIFY(!m_cm->hasFeature(QContactManager::ArbitraryRelationshipTypes, QContactType::TypeContact));
    QVERIFY(!m_cm->hasFeature(QContactManager::ArbitraryRelationshipTypes, QContactType::TypeGroup));

    QVERIFY(!m_cm->hasFeature(QContactManager::RelationshipOrdering));
    QVERIFY(!m_cm->hasFeature(QContactManager::RelationshipOrdering, QContactType::TypeContact));
    QVERIFY(!m_cm->hasFeature(QContactManager::RelationshipOrdering, QContactType::TypeGroup));

    // TODO: self contact may be supported? (so called "own number store")
    QVERIFY(!m_cm->hasFeature(QContactManager::SelfContact));
    QVERIFY(!m_cm->hasFeature(QContactManager::SelfContact, QContactType::TypeContact));
    QVERIFY(!m_cm->hasFeature(QContactManager::SelfContact, QContactType::TypeGroup));

    QVERIFY(!m_cm->hasFeature(QContactManager::Anonymous));
    QVERIFY(!m_cm->hasFeature(QContactManager::Anonymous, QContactType::TypeContact));
    QVERIFY(!m_cm->hasFeature(QContactManager::Anonymous, QContactType::TypeGroup));

    QVERIFY(!m_cm->hasFeature(QContactManager::ChangeLogs));
    QVERIFY(!m_cm->hasFeature(QContactManager::ChangeLogs, QContactType::TypeContact));
    QVERIFY(!m_cm->hasFeature(QContactManager::ChangeLogs, QContactType::TypeGroup));
}

void tst_QContactManagerSymbianSim::supportedContactTypes()
{
    QStringList types = m_cm->supportedContactTypes();
    QVERIFY(types.count() > 0);
    // Assuming that contact type of TypeContact is always supported
    QVERIFY(types.contains(QContactType::TypeContact));
}

void tst_QContactManagerSymbianSim::detailDefinitions()
{
    QMap<QString, QContactDetailDefinition> detailDefinitions = m_cm->detailDefinitions();
    QCOMPARE(m_cm->error(), QContactManager::NoError);

    QVERIFY(detailDefinitions.count() >= 2);
    QVERIFY(detailDefinitions.count() <= 7);

    // check that at least definitions for name and phone number exist
    QVERIFY(detailDefinitions.contains(QContactName::DefinitionName));
    QVERIFY(detailDefinitions.contains(QContactPhoneNumber::DefinitionName));
    QVERIFY(detailDefinitions.contains(QContactDisplayLabel::DefinitionName));
    QVERIFY(detailDefinitions.contains(QContactType::DefinitionName));
    QVERIFY(detailDefinitions.contains(QContactSyncTarget::DefinitionName));

    // Dynamic definitions (that depend on SIM card type)
    if(detailDefinitions.count() == 7) {
        QVERIFY(detailDefinitions.contains(QContactNickname::DefinitionName));
        QVERIFY(detailDefinitions.contains(QContactEmailAddress::DefinitionName));
    }
}

void tst_QContactManagerSymbianSim::addContact_data()
{
    // A string list containing the detail fields in format <detail definition name>:<field name>:<value>
    // For example first name: Name:First:James
    QTest::addColumn<int>("expectedResult"); // 1 = pass, 0 = fail, -1 = depends on the SIM card
    QTest::addColumn<QString>("expectedDisplayLabel");
    QTest::addColumn<QStringList>("details"); // format is <detail definition name>:<field name>:<value>
    QString unnamedLabel("Unnamed");
    QString es = QString();
    QString tooLongText("James Hunt the 12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890th");

    // TODO: what name field to use for a sim contact name?
    // Note: With the current implementation the value must not contain a ':' character
    QTest::newRow("custom label")
        << 1 // expected to pass
        << "James"
        << (QStringList()
            << "Name:CustomLabel:James");

    QTest::newRow("custom label 2")
        << 1 // expected to pass
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt");

    QTest::newRow("2 custom labels")
        << 0 // expected to fail. Custom label is unique.
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James"
            << "Name:CustomLabel:James Hunt");

    QTest::newRow("too long custom label")
        << 1 // expected to pass. Note: too long display label is truncated
        << tooLongText
        << (QStringList()
            << (QString("Name:CustomLabel:").append(tooLongText)));

    QTest::newRow("custom label and nick name")
        << -1 // Depends on SIM card support (some cards support second name)
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "Nickname:Nickname:Hunt the Shunt");

    QTest::newRow("custom label and too long nick name")
        << -1 // Depends on SIM card support (some cards support second name)
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << (QString("Nickname:Nickname:").append(tooLongText)));

    QTest::newRow("phone number")
        << 1
        << unnamedLabel
        << (QStringList()
            << "PhoneNumber:PhoneNumber:+44752222222");

    QTest::newRow("custom label and phone number")
        << 1
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "PhoneNumber:PhoneNumber:+44752222222");

    QTest::newRow("custom label and funny (but legal) phone number")
        << 1
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "PhoneNumber:PhoneNumber:+0123456789*#p");

    QTest::newRow("custom label and illegal phone number 1")
        << 0 // illegal characters in the phone number, should fail
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "PhoneNumber:PhoneNumber:+44(75)2222222");

    QTest::newRow("custom label and illegal phone number 2")
        << 0 // illegal characters in the phone number, should fail
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "PhoneNumber:PhoneNumber:asdfqwer");

    QTest::newRow("custom label and too long phone number")
        << 0 // long enough phone number to fail on any SIM card
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "PhoneNumber:PhoneNumber:1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");

    QTest::newRow("custom label and multiple phone numbers")
        << -1 // some SIM cards support multiple phone numbers
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "PhoneNumber:PhoneNumber:+44752222222"
            << "PhoneNumber:PhoneNumber:+44751111111");

    QTest::newRow("custom label and multiple phone numbers, one phone number too long")
        << 0 // Long enough additional phone number to fail on any SIM card
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "PhoneNumber:PhoneNumber:+44752222222"
            << "PhoneNumber:PhoneNumber:1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");

    QTest::newRow("custom label and email")
        << -1 // some SIM cards support e-mail address
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "EmailAddress:EmailAddress:james@mclaren.com");

    QTest::newRow("custom label and multiple emails")
        << 0 // some SIM cards support multiple e-mail addresses, but not this many
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "EmailAddress:EmailAddress:james@march.com"
            << "EmailAddress:EmailAddress:james@hesketh.com"
            << "EmailAddress:EmailAddress:james@mclaren.com"
            << "EmailAddress:EmailAddress:james.hunt@bbc.co.uk");

    QTest::newRow("custom label and too long email")
        << 0 // long enough e-mail to fail on any SIM card
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "EmailAddress:EmailAddress:james.hunt.the12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890th@mclaren.com");

    QTest::newRow("non-supported field")
        << 0 // expected to fail
        << es
        << (QStringList()
            << "Name:IllegalNameDetailFieldName:James");

    QTest::newRow("non-supported detail")
        << 0 // expected to fail
        << es
        << (QStringList()
            << "NotSupportedDetailDefinitionName:IllegalFieldName:FieldValue");

    QTest::newRow("empty, non-supported detail")
        << 0 // expected to fail, since no valid details provided
        << es
        << (QStringList()
            << "NotSupportedDetailDefinitionName:IllegalFieldName:");
}

/*
 * Tests if different contacts can be saved to SIM card.
 * Steps:
 * 1. Parse contact details from test parameters
 * 2. Determine the expected result
 * 3.1 (if expected to pass) Save contact, verify result and remove contact
 * 3.2 (if expected to fail) Check that saving a contact fails
 */
void tst_QContactManagerSymbianSim::addContact()
{
    // Make debugging easier by getting the test case name
    QString tescaseName = QTest::currentDataTag();

    QFETCH(int, expectedResult);
    QFETCH(QString, expectedDisplayLabel);
    QFETCH(QStringList, details);    

    QContact contact;
    QList<QContactDetail> expectedDetails;

    // 1. Parse details and add them to the contact
    parseDetails(contact, details, expectedDetails);

    // 2. Determine the expected result
    if (expectedResult == -1) {
        // Unknown expected result, so we need to check what details the SIM
        // card supports
        if (isContactSupported(contact)) {
            expectedResult = 1; 
        } else {
            expectedResult = 0;
        }
    }

    // Get the contact count for verification purposes
    QList<QContactLocalId> idsBefore = m_cm->contactIds();
    QCOMPARE(m_cm->error(), QContactManager::NoError);

    // 3.1 (if expected to pass) Save contact, verify result and remove contact
    if (expectedResult)
    {
        // verify contact can be saved
        QVERIFY(m_cm->saveContact(&contact));
        QCOMPARE(m_cm->error(), QContactManager::NoError);
        QList<QContactLocalId> idsAfterSave = m_cm->contactIds();
        QCOMPARE(idsAfterSave.count(), idsBefore.count() + 1);

        // verify contact id
        QVERIFY(contact.id() != QContactId());

        // verify that the details were saved as expected
        QVERIFY(compareDetails(contact, expectedDetails));

        // verify display label, allow truncating to the max text length
        QCOMPARE(contact.displayLabel(), expectedDisplayLabel.left(m_etelStoreInfo.iMaxTextLength));

        // TODO: verify that no extra details were added?
        //?QCOMPARE(contact.details().count(), detailsUnderTest.count() + 2);

        // verify contact removal
        QVERIFY(m_cm->removeContact(contact.localId()));
        QCOMPARE(m_cm->error(), QContactManager::NoError);

    // 3.2 (if expected to fail) Check that saving a contact fails
    } else {
        // verify that the contact cannot be saved
        QVERIFY(!m_cm->saveContact(&contact));
        // TODO: what is the expected error code? does it depend on the case?

        // verify contact id is untouched 
        QVERIFY(contact.id() == QContactId());
    }

    QList<QContactLocalId> idsAfterRemove = m_cm->contactIds();
    QCOMPARE(idsAfterRemove.count(), idsBefore.count());
}

void tst_QContactManagerSymbianSim::updateContactDetail_data()
{
    // The initial contact details,
    // for example: <"Name:First:James">; <"PhoneNumber:PhoneNumber:1234567890">
    QTest::addColumn<QStringList>("initialDetails");
    // The updated contact details,
    // for example: <"Name:First:James">; <"PhoneNumber:PhoneNumber:0987654321">
    QTest::addColumn<QStringList>("updatedDetails");

    QString es = QString();

    QTest::newRow("update custom label")
        << (QStringList()
            << "Name:CustomLabel:James")
        << (QStringList()
            << "Name:CustomLabel:James Hunt");

    QTest::newRow("add phone number detail")
        << (QStringList()
            << "Name:CustomLabel:James")
        << (QStringList()
            << "Name:CustomLabel:James"
            << "PhoneNumber:PhoneNumber:+44752222222");

    QTest::newRow("update phone number detail")
        << (QStringList()
            << "Name:CustomLabel:James"
            << "PhoneNumber:PhoneNumber:+44751111111")
        << (QStringList()
            << "Name:CustomLabel:James"
            << "PhoneNumber:PhoneNumber:+44752222222");

    QTest::newRow("remove phone number detail")
        << (QStringList()
            << "Name:CustomLabel:James"
            << "PhoneNumber:PhoneNumber:+44751111111")
        << (QStringList()
            << "Name:CustomLabel:James");

    QTest::newRow("add e-mail detail")
        << (QStringList()
            << "Name:CustomLabel:James")
        << (QStringList()
            << "Name:CustomLabel:James"
            << "EmailAddress:EmailAddress:james@hesketh.com");

    QTest::newRow("update e-mail detail")
        << (QStringList()
            << "Name:CustomLabel:James"
            << "EmailAddress:EmailAddress:james@march.com")
        << (QStringList()
            << "Name:CustomLabel:James"
            << "EmailAddress:EmailAddress:james@hesketh.com");

    QTest::newRow("remove e-mail detail")
        << (QStringList()
            << "Name:CustomLabel:James"
            << "EmailAddress:EmailAddress:james@march.com")
        << (QStringList()
            << "Name:CustomLabel:James");

    QTest::newRow("add nickname detail")
        << (QStringList()
            << "Name:CustomLabel:James")
        << (QStringList()
            << "Name:CustomLabel:James"
            << "Nickname:Nickname:Hunt the Shunt");

    QTest::newRow("update nickname detail")
        << (QStringList()
            << "Name:CustomLabel:James"
            << "Nickname:Nickname:James")
        << (QStringList()
            << "Name:CustomLabel:James"
            << "Nickname:Nickname:Hunt the Shunt");

    QTest::newRow("remove nickname detail")
        << (QStringList()
            << "Name:CustomLabel:James"
            << "Nickname:Nickname:James")
        << (QStringList()
            << "Name:CustomLabel:James");
}

/*
 * Tests if SIM contacts can be changed. I.e. add, update and remove contact
 * details. Steps:
 * 1. Parse details from test data
 * 2. Add a contact with initial details
 * 3. Modify the contact (save with updated details)
 * 4. Remove the contact
 */
void tst_QContactManagerSymbianSim::updateContactDetail()
{
    QString tescaseName = QTest::currentDataTag();

    QFETCH(QStringList, initialDetails);
    QFETCH(QStringList, updatedDetails);

    // 1. Parse details
    QContact contact;
    QList<QContactDetail> parsedDetails;
    parseDetails(contact, initialDetails, parsedDetails);

    // 2. Save contact and verify result
    if (!isContactSupported(contact)) {
        QSKIP("The contact cannot be saved onto the SIM card", SkipSingle);
    }
    QVERIFY(m_cm->saveContact(&contact));
    QCOMPARE(m_cm->error(), QContactManager::NoError);
    QVERIFY(compareDetails(contact, parsedDetails));

    // 3. Update contact detail and verify result
    foreach (QContactDetail detail, parsedDetails) {
        QContactDetail savedDetail = contact.detail(detail.definitionName());
        QVERIFY(contact.removeDetail(&savedDetail));
    }
    parseDetails(contact, updatedDetails, parsedDetails);
    if (!isContactSupported(contact)) {
        QVERIFY(m_cm->removeContact(contact.localId()));
        QSKIP("The contact cannot be saved onto the SIM card", SkipSingle);
    }
    QVERIFY(m_cm->saveContact(&contact));
    QCOMPARE(m_cm->error(), QContactManager::NoError);
    QVERIFY(compareDetails(contact, parsedDetails));

    // 4. Remove the contact
    QVERIFY(m_cm->removeContact(contact.localId()));
}

/*!
 * Private helper function for parsing test data (creates QContactDetails from
 * string lists).
 */
void tst_QContactManagerSymbianSim::parseDetails(QContact &contact, QStringList details, QList<QContactDetail> &parsedDetails)
{
    parsedDetails.clear();
    foreach (QString detail, details) {
        // the expected format is <detail definition name>:<field name>:<value>
        QStringList detailParts = detail.split(QChar(':'), QString::KeepEmptyParts, Qt::CaseSensitive);
        QVERIFY(detailParts.count() == 3);
    
        // Use existing detail if available and would not cause an overwrite of
        // a field value
        QContactDetail contactDetail = QContactDetail(detailParts[0]);
        if (contact.details().contains(detailParts[0])
            && contact.detail(detailParts[0]).variantValues().key(detailParts[1]).isNull()) {
            contactDetail = contact.detail(detailParts[0]);
        }

        // Set the field value only if not empty (do not add empty fields)  
        if (!detailParts[2].isEmpty()) {
            QVERIFY(contactDetail.setValue(detailParts[1], detailParts[2]));
        }

        QVERIFY(contact.saveDetail(&contactDetail));
        parsedDetails.append(contactDetail);
    }
}

/*!
 * Private helper function for checking if the SIM backend supports the
 * contact. This can be used in cases where it depends on the SIM card features
 * if the contact details can be saved or not.
 */
bool tst_QContactManagerSymbianSim::isContactSupported(QContact contact)
{
    QMap<QString, QContactDetailDefinition> detailDefinitions = m_cm->detailDefinitions();

    if(!m_cm->supportedContactTypes().contains(contact.type()))
        return false;
        
    QList<QString> uniqueDetails = QList<QString>();

    foreach(QContactDetail detail, contact.details()) {
        QString definitionName = detail.definitionName();

        // TODO: should we save a contact that has empty, non-supported details?
        // The current implementation is to ignore empty details here which
        // means that the backend should also ignore the empty details, even
        // if the detail in question is not supported.
        if (detail.isEmpty()) {
            continue;
        }

        // check if the detail is supported by the SIM
        if (detailDefinitions.contains(detail.definitionName())) {

            QContactDetailDefinition detailDef = detailDefinitions.value(detail.definitionName());

            // If the detail is unique check that there are no duplicates
            if (detailDef.isUnique()) {
                if (uniqueDetails.contains(detail.definitionName())) {
                    return false;
                } else {
                    uniqueDetails.append(detail.definitionName());
                }
            }

            // check the fields of the detail
            foreach (QString fieldKey, detail.variantValues().keys()) {
                if (!detailDef.fields().contains(fieldKey)) {
                    return false;
                }
            }

        } else {
            qDebug() << "Detail" << definitionName << "Not supported";
            return false;
        }
    }

    return true;
}

/*
 * Private helper function for comparing QContact details to a well-known set
 * of QContactDetails.
 * \return true if all the expected contact details have a match in the \contact.
 */
bool tst_QContactManagerSymbianSim::compareDetails(QContact contact, QList<QContactDetail> expectedDetails)
{
    foreach (QContactDetail expectedDetail, expectedDetails) {
        QContactDetail actualDetail = contact.detail(expectedDetail.definitionName());
        QVERIFY_WITH_RETURN_VALUE(!actualDetail.isEmpty());

        // Allow truncating the custom label to the max text length
        if (expectedDetail.definitionName() == QContactName::DefinitionName) {
            QContactName nameDetail = static_cast<QContactName>(expectedDetail);
            nameDetail.setCustomLabel(nameDetail.customLabel().left(m_etelStoreInfo.iMaxTextLength));
            QCOMPARE_WITH_RETURN_VALUE(actualDetail, static_cast<QContactDetail>(nameDetail));
        // Allow truncating the nick name to the max text length
        } else if (expectedDetail.definitionName() == QContactNickname::DefinitionName) {
                QContactNickname nick = static_cast<QContactNickname>(expectedDetail);
                nick.setNickname(nick.nickname().left(m_etelStoreInfo.iMaxTextLength));
                QCOMPARE_WITH_RETURN_VALUE(actualDetail, static_cast<QContactDetail>(nick));
        } else {
            if (actualDetail != expectedDetail) {
                // FAIL! Make it easier to debug the output by
                // comparing the contact detail field contents
                foreach (QString key, expectedDetail.variantValues().keys()) {
                    QVariant value1 = actualDetail.value(key);
                    QVariant value2 = expectedDetail.value(key);
                    QCOMPARE_WITH_RETURN_VALUE(actualDetail.value(key), expectedDetail.value(key));
                }
            }
        }
    }
    return true;
}

QContact tst_QContactManagerSymbianSim::createContact(QString name, QString number)
{
    QContact c;
    
    QContactName n;
    n.setCustomLabel(name);
    c.saveDetail(&n);
    
    QContactPhoneNumber nb;
    nb.setNumber(number);
    c.saveDetail(&nb);

    return c;
}

QContact tst_QContactManagerSymbianSim::saveContact(QString name, QString number)
{
    QContact c;
    
    QContactName n;
    n.setCustomLabel(name);
    c.saveDetail(&n);
    
    QContactPhoneNumber nb;
    nb.setNumber(number);
    c.saveDetail(&nb);
    
    if (!m_cm->saveContact(&c)) {
        qWarning("*FATAL* could not save contact!");
    }
    
    return c;
}

void tst_QContactManagerSymbianSim::fetchContactReq()
{
    QContactFetchRequest req;
    req.setManager(m_cm);
    
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QContactAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));

    // Save some contacts
    QContact c1 = saveContact("a", "1234567");
    QContact c2 = saveContact("b", "7654321");
    QContact c3 = saveContact("c", "1111111");
    
    // Fetch the contacts
    QVERIFY(req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);    
    QVERIFY(req.waitForFinished(0));
    QVERIFY(req.state() == QContactAbstractRequest::FinishedState);
    QVERIFY(req.error() == QContactManager::NoError);
    QVERIFY(stateSpy.count() == 2);
    QVERIFY(resultSpy.count() == 1);
    QVERIFY(req.contacts().count() == 3);

    // Test cancelling
    stateSpy.clear();
    resultSpy.clear();
    QVERIFY(!req.cancel());
    QVERIFY(req.state() == QContactAbstractRequest::FinishedState);    
    QVERIFY(req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);
    QVERIFY(req.cancel());
    QVERIFY(!req.cancel());
    QVERIFY(stateSpy.count() == 2);
    QVERIFY(resultSpy.count() == 0);
    QVERIFY(req.state() == QContactAbstractRequest::CanceledState);
   
    // Remove all contacts
    QList<QContactLocalId> ids = m_cm->contactIds();
    m_cm->removeContacts(&ids, 0);    
    
    // Test fetching nothing
    stateSpy.clear();
    resultSpy.clear();
    QVERIFY(req.start());
    QVERIFY(!req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);
    QTRY_COMPARE(resultSpy.count(), 1);
    QVERIFY(req.state() == QContactAbstractRequest::FinishedState);
    QVERIFY(req.error() == QContactManager::DoesNotExistError);
    QVERIFY(stateSpy.count() == 2);
    QVERIFY(req.contacts().count() == 0);
}

void tst_QContactManagerSymbianSim::localIdFetchReq()
{
    QContactLocalIdFetchRequest req;
    req.setManager(m_cm);
    
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QContactAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));

    // Save some contacts
    QContact c1 = saveContact("a", "1234567");
    QContact c2 = saveContact("b", "7654321");
    QContact c3 = saveContact("c", "1111111");
    
    // Fetch the contacts
    QVERIFY(req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);
    QVERIFY(req.waitForFinished(0));
    QVERIFY(req.state() == QContactAbstractRequest::FinishedState);
    QVERIFY(req.error() == QContactManager::NoError);
    QVERIFY(stateSpy.count() == 2);
    QVERIFY(resultSpy.count() == 1);
    QVERIFY(req.ids().count() == 3);
    QVERIFY(req.ids().contains(c1.localId()));
    QVERIFY(req.ids().contains(c2.localId()));
    QVERIFY(req.ids().contains(c3.localId()));
    
    // Test cancelling
    stateSpy.clear();
    resultSpy.clear();
    QVERIFY(!req.cancel());
    QVERIFY(req.state() == QContactAbstractRequest::FinishedState);    
    QVERIFY(req.start());
    QVERIFY(!req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);
    QVERIFY(req.cancel());
    QVERIFY(!req.cancel());
    QVERIFY(stateSpy.count() == 2);
    QVERIFY(resultSpy.count() == 0);
    QVERIFY(req.state() == QContactAbstractRequest::CanceledState);    
    
    // Remove all contacts
    QList<QContactLocalId> ids = m_cm->contactIds();
    m_cm->removeContacts(&ids, 0);    
    
    // Start again
    stateSpy.clear();
    resultSpy.clear();
    QVERIFY(req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);
    QTRY_COMPARE(resultSpy.count(), 1);
    QVERIFY(req.state() == QContactAbstractRequest::FinishedState);
    QVERIFY(req.error() == QContactManager::DoesNotExistError);
    QVERIFY(stateSpy.count() == 2);
    QVERIFY(req.ids().count() == 0);    
}

void tst_QContactManagerSymbianSim::saveContactReq()
{
    QContactSaveRequest req;
    req.setManager(m_cm);
    
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QContactAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));

    // Create some contacts
    QContact c1 = createContact("Keeppu", "47474747");
    QContact c2 = createContact("Rilli", "74747474");
    
    // Test cancel
    QList<QContact> contacts;
    contacts << c1 << c2;
    req.setContacts(contacts);    
    QVERIFY(!req.cancel());
    QVERIFY(req.start());
    QVERIFY(!req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);
    QVERIFY(req.cancel());
    QVERIFY(!req.cancel());
    QVERIFY(req.state() == QContactAbstractRequest::CanceledState);
    QVERIFY(stateSpy.count() == 2);
    QVERIFY(resultSpy.count() == 0);
    QVERIFY(m_cm->contactIds().count() == 0);
    
    // Test save
    stateSpy.clear();
    req.setContacts(contacts); 
    QVERIFY(req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);
    QTRY_COMPARE(resultSpy.count(), 1);
    QVERIFY(req.state() == QContactAbstractRequest::FinishedState);
    QVERIFY(req.error() == QContactManager::NoError);
    QVERIFY(req.errorMap().count() == 0);
    QVERIFY(stateSpy.count() == 2);
    QVERIFY(resultSpy.count() == 1);
    QVERIFY(req.contacts().count() == 2);
    QContact c = req.contacts().at(0);
    QVERIFY(c.id().managerUri() == m_cm->managerUri());
    QVERIFY(c.localId() != QContactLocalId(0));
    QVERIFY(c.detail<QContactName>().firstName() == c1.detail<QContactName>().firstName());
    QVERIFY(c.detail<QContactPhoneNumber>().number() == c1.detail<QContactPhoneNumber>().number());
    QVERIFY(m_cm->contactIds().count() == 2);
    
    // Test saving again
    c1 = req.contacts().at(0);
    c2 = req.contacts().at(1);

    QContactName name = c1.detail<QContactName>();
    name.setCustomLabel("Keeputin");
    c1.saveDetail(&name);
    QContactPhoneNumber number = c1.detail<QContactPhoneNumber>();
    c1.removeDetail(&number);
    
    contacts.clear();
    contacts << c1 << c2;
    req.setContacts(contacts);
    
    stateSpy.clear();
    resultSpy.clear();
    QVERIFY(req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);
    QVERIFY(req.waitForFinished(0));
    QVERIFY(req.state() == QContactAbstractRequest::FinishedState);
    QVERIFY(req.error() == QContactManager::NoError);
    QVERIFY(req.errorMap().count() == 0);
    QVERIFY(stateSpy.count() == 2);
    QVERIFY(resultSpy.count() == 1);
    QVERIFY(req.contacts().count() == 2);
    QVERIFY(req.contacts().at(0).localId() == c1.localId());
    QVERIFY(req.contacts().at(1).localId() == c2.localId());
    c = req.contacts().at(0);
    QVERIFY(c.details(QContactPhoneNumber::DefinitionName).count() == 0);
    QVERIFY(c.detail<QContactName>().customLabel() == c1.detail<QContactName>().customLabel());
    QVERIFY(m_cm->contactIds().count() == 2);
    c = m_cm->contact(c1.localId(), QStringList());
    QVERIFY(c.details(QContactPhoneNumber::DefinitionName).count() == 0);
    QVERIFY(c.detail<QContactName>().customLabel() == c1.detail<QContactName>().customLabel());
}

void tst_QContactManagerSymbianSim::removeContactReq()
{
    QContactRemoveRequest req;
    req.setManager(m_cm);
    
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QContactAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));
    
    // Save some contacts
    QContact c1 = saveContact("a", "1234567");
    QContact c2 = saveContact("b", "7654321");

    // Remove the contacts
    QList<QContactLocalId> ids;
    ids << c1.localId() << c2.localId();
    req.setContactIds(ids);
    QVERIFY(req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);
    QVERIFY(req.waitForFinished(0));
    QVERIFY(req.state() == QContactAbstractRequest::FinishedState);
    QVERIFY(req.error() == QContactManager::NoError);
    QVERIFY(req.errorMap().count() == 0);
    QVERIFY(stateSpy.count() == 2);
    QVERIFY(resultSpy.count() == 1);
    QVERIFY(m_cm->contactIds().count() == 0);
    
    // Test cancel
    stateSpy.clear();
    resultSpy.clear();
    QVERIFY(!req.cancel());
    QVERIFY(req.start());
    QVERIFY(!req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);
    QVERIFY(req.cancel());
    QVERIFY(!req.cancel());
    QVERIFY(req.state() == QContactAbstractRequest::CanceledState);
    QVERIFY(stateSpy.count() == 2);
    QVERIFY(resultSpy.count() == 0);
    
    // Remove same ones again
    stateSpy.clear();
    resultSpy.clear();
    QVERIFY(req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);
    QTRY_COMPARE(resultSpy.count(), 1);
    QVERIFY(req.state() == QContactAbstractRequest::FinishedState);
#ifndef __WINS__
    QWARN("This test fails in hardware!");
    QWARN("In hardware removing SIM contacts which do not exist the etel server will return KErrNone instead of KErrNotFound");
#endif
    QVERIFY(req.error() == QContactManager::DoesNotExistError);
    QVERIFY(req.errorMap().count() == 2);
    QVERIFY(req.errorMap().value(0) == QContactManager::DoesNotExistError);
    QVERIFY(req.errorMap().value(1) == QContactManager::DoesNotExistError);
}

void tst_QContactManagerSymbianSim::detailDefinitionFetchReq()
{
    QContactDetailDefinitionFetchRequest req;
    req.setManager(m_cm);
    
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QContactAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));

    // Fetch all
    req.setContactType(QContactType::TypeContact);
    QVERIFY(req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);
    QVERIFY(req.waitForFinished(0));
    QVERIFY(req.state() == QContactAbstractRequest::FinishedState);
    QVERIFY(req.error() == QContactManager::NoError);
    QVERIFY(stateSpy.count() == 2);
    QVERIFY(resultSpy.count() == 1);
    QVERIFY(req.definitions().count());
    QVERIFY(req.definitions() == m_cm->detailDefinitions(req.contactType()));
    
    // Test cancel
    stateSpy.clear();
    resultSpy.clear();
    QVERIFY(!req.cancel());
    QVERIFY(req.start());
    QVERIFY(!req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);
    QVERIFY(req.cancel());
    QVERIFY(!req.cancel());
    QVERIFY(req.state() == QContactAbstractRequest::CanceledState);
    QVERIFY(stateSpy.count() == 2);
    QVERIFY(resultSpy.count() == 0);

    // Fetch some defs
    stateSpy.clear();
    resultSpy.clear();
    req.setDefinitionNames(QStringList() << QContactPhoneNumber::DefinitionName << QContactNote::DefinitionName);
    QVERIFY(req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);
    QTRY_COMPARE(resultSpy.count(), 1);
    QVERIFY(req.state() == QContactAbstractRequest::FinishedState);
    QVERIFY(req.error() == QContactManager::DoesNotExistError);
    QVERIFY(req.errorMap().value(1) == QContactManager::DoesNotExistError);
    QVERIFY(stateSpy.count() == 2);
    QVERIFY(resultSpy.count() == 1);
    QVERIFY(req.definitions().count() == 1);
    QVERIFY(req.definitions().contains(QContactPhoneNumber::DefinitionName));
    
    // Fetch non-existing type
    stateSpy.clear();
    resultSpy.clear();
    req.setContactType(QContactType::TypeGroup);
    req.setDefinitionNames(QStringList());
    QVERIFY(req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);
    QVERIFY(req.waitForFinished(0));
    QVERIFY(req.state() == QContactAbstractRequest::FinishedState);
    QVERIFY(req.error() == QContactManager::NotSupportedError);
    QVERIFY(stateSpy.count() == 2);
    QVERIFY(resultSpy.count() == 1);
}

void tst_QContactManagerSymbianSim::notSupportedRequests()
{
    QVERIFY(!m_cm->hasFeature(QContactManager::Relationships));
    QContactRelationshipFetchRequest rfreq;
    rfreq.setManager(m_cm);
    QVERIFY(!rfreq.start());
    QContactRelationshipRemoveRequest rrreq;
    rrreq.setManager(m_cm);
    QVERIFY(!rrreq.start());
    
    QVERIFY(!m_cm->hasFeature(QContactManager::MutableDefinitions));
    QContactDetailDefinitionRemoveRequest ddrreq;
    ddrreq.setManager(m_cm);
    QVERIFY(!ddrreq.start());
    QContactDetailDefinitionSaveRequest ddsreq;
    ddsreq.setManager(m_cm);
    QVERIFY(!ddsreq.start());
}


QTEST_MAIN(tst_QContactManagerSymbianSim)
#include "tst_qcontactmanagersymbiansim.moc"
