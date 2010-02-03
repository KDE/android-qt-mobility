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

#include "qtcontacts.h"
#include "qcontactmanagerdataholder.h" //QContactManagerDataHolder

//TESTED_CLASS=
//TESTED_FILES=

QTM_USE_NAMESPACE
class tst_QContactDetails : public QObject
{
Q_OBJECT

public:
    tst_QContactDetails();
    virtual ~tst_QContactDetails();

private:
    QContactManagerDataHolder managerDataHolder;

public slots:
    void init();
    void cleanup();

private slots:
    // leaf class testing
    void address();
    void anniversary();
    void avatar();
    void birthday();
    void displayLabel();
    void emailAddress();
    void family();
    void gender();
    void geolocation();
    void guid();
    void name();
    void nickname();
    void note();
    void onlineAccount();
    void organization();
    void phoneNumber();
    void syncTarget();
    void timestamp();
    void type();
    void url();

    // custom definition testing
    void custom();
};

tst_QContactDetails::tst_QContactDetails()
{
}

tst_QContactDetails::~tst_QContactDetails()
{
}

void tst_QContactDetails::init()
{
}

void tst_QContactDetails::cleanup()
{
}

void tst_QContactDetails::address()
{
    QContact c;
    QContactAddress a1, a2;

    QVERIFY(a1.isEmpty());
    QVERIFY(a2.isEmpty());

    // test property set
    QCOMPARE(a1.street(), QString());
    a1.setStreet("68 Imaginary Avenue");
    a1.setCountry("Australia");
    QCOMPARE(a1.street(), QString("68 Imaginary Avenue"));
    a1.setStreet("1234");
    QCOMPARE(a1.street(), QString("1234"));
    a1.setPostOfficeBox("PO Box 5678");
    QCOMPARE(a1.postOfficeBox(), QString("PO Box 5678"));


    // Sub types
    a1.setSubTypes(QContactAddress::SubTypeDomestic);
    QCOMPARE(a1.subTypes(), QStringList() << QLatin1String(QContactAddress::SubTypeDomestic));

    QStringList sl;
    sl << QLatin1String(QContactAddress::SubTypeParcel) << QLatin1String(QContactAddress::SubTypePostal);
    a1.setSubTypes(sl);
    QCOMPARE(a1.subTypes(), sl);

    // test property add
    QVERIFY(c.saveDetail(&a1));
    QCOMPARE(c.details(QContactAddress::DefinitionName).count(), 1);
    QCOMPARE(QContactAddress(c.details(QContactAddress::DefinitionName).value(0)).street(), a1.street());
    a2.setStreet("Test");
    QVERIFY(c.saveDetail(&a2));
    QCOMPARE(c.details(QContactAddress::DefinitionName).count(), 2);

    // test property update
    a1.setValue("label","label1");
    a1.setStreet("12345");
    QVERIFY(c.saveDetail(&a1));

    // test property remove
    QVERIFY(c.removeDetail(&a1));
    QCOMPARE(c.details(QContactAddress::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&a2));
    QVERIFY(!c.removeDetail(&a2)); // cannot remove twice
    QCOMPARE(c.details(QContactAddress::DefinitionName).count(), 0);

    QContactAddress a3;

    // set street
    a3.setStreet("Test");
    QCOMPARE(a3.street(), QString("Test"));
    a3.setStreet("");

    // set locality
    a3.setLocality("Test");
    QCOMPARE(a3.locality(), QString("Test"));
    a3.setLocality("");

    // set region
    a3.setRegion("Test");
    QCOMPARE(a3.region(), QString("Test"));
    a3.setRegion("");

    // set postcode
    a3.setPostcode("Test");
    QCOMPARE(a3.postcode(), QString("Test"));
    a3.setPostcode("");

    // set country
    a3.setCountry("Test");
    QCOMPARE(a3.country(), QString("Test"));
    a3.setCountry("");

    // subtypes
    a3.setSubTypes(QContactAddress::SubTypeDomestic);
    QCOMPARE(a3.subTypes(), QStringList(QContactAddress::SubTypeDomestic));
    a3.setSubTypes(QStringList() << QContactAddress::SubTypeInternational << QContactAddress::SubTypePostal);
    QCOMPARE(a3.subTypes(), QStringList() << QContactAddress::SubTypeInternational << QContactAddress::SubTypePostal);
}

void tst_QContactDetails::anniversary()
{
    QContact c;
    QContactAnniversary a1, a2;
    QDate currDate = QDate::currentDate();

    // test property set
    a1.setCalendarId("1234");
    QCOMPARE(a1.calendarId(), QString("1234"));
    QCOMPARE(a1.value(QContactAnniversary::FieldCalendarId), QString("1234"));
    a1.setEvent("4321");
    QCOMPARE(a1.event(), QString("4321"));
    QCOMPARE(a1.value(QContactAnniversary::FieldEvent), QString("4321"));
    a1.setOriginalDate(currDate);
    QCOMPARE(a1.originalDate(), currDate);
    QCOMPARE(a1.value<QDate>(QContactAnniversary::FieldOriginalDate), currDate);
    a1.setSubType(QContactAnniversary::SubTypeWedding);
    QCOMPARE(a1.subType(), QString(QLatin1String(QContactAnniversary::SubTypeWedding)));
    QCOMPARE(a1.value(QContactAnniversary::FieldSubType), QString(QLatin1String(QContactAnniversary::SubTypeWedding)));

    // test property add
    QVERIFY(c.saveDetail(&a1));
    QCOMPARE(c.details(QContactAnniversary::DefinitionName).count(), 1);
    QCOMPARE(QContactAnniversary(c.details(QContactAnniversary::DefinitionName).value(0)).event(), a1.event());

    // test property update
    a1.setValue("label","label1");
    a1.setCalendarId("12345");
    QVERIFY(c.saveDetail(&a1));
    QCOMPARE(c.details(QContactAnniversary::DefinitionName).value(0).value("label"), QString("label1"));
    QCOMPARE(c.details(QContactAnniversary::DefinitionName).value(0).value(QContactAnniversary::FieldCalendarId), QString("12345"));

    // test property remove
    QVERIFY(c.removeDetail(&a1));
    QCOMPARE(c.details(QContactAnniversary::DefinitionName).count(), 0);
    QVERIFY(c.saveDetail(&a2));
    QCOMPARE(c.details(QContactAnniversary::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&a2));
    QCOMPARE(c.details(QContactAnniversary::DefinitionName).count(), 0);
    QVERIFY(c.removeDetail(&a2) == false);
    QCOMPARE(c.details(QContactAnniversary::DefinitionName).count(), 0);
}

void tst_QContactDetails::avatar()
{
    QContact c;
    QContactAvatar a1, a2;

    // test property set
    a1.setAvatar("1234");
    QCOMPARE(a1.avatar(), QString("1234"));
    QCOMPARE(a1.value(QContactAvatar::FieldAvatar), QString("1234"));
    a1.setSubType(QContactAvatar::SubTypeAudioRingtone);
    QCOMPARE(a1.subType(), QString(QLatin1String(QContactAvatar::SubTypeAudioRingtone)));
    QCOMPARE(a1.value(QContactAvatar::FieldSubType), QString(QLatin1String(QContactAvatar::SubTypeAudioRingtone)));

    a1.setSubType(QContactAvatar::SubTypeImage);
    
    //pixmap
    uchar pixDataRGB[] = {255, 0, 0, 0, 0, 255, 0, 0, 255, 255, 0, 0}; // Red, Blue, Red, Blue
    QImage img(pixDataRGB, 2, 2, 6, QImage::Format_RGB888); // 2 pixels width, 2 pixels height, 6 bytes per line, RGB888 format
    QImage scaled = img.scaled(100, 100); // Scale image to show results better
    QPixmap pix = QPixmap::fromImage(scaled); // Create pixmap from image
    a1.setPixmap(pix);

    // test property add
    QVERIFY(c.saveDetail(&a1));
    QCOMPARE(c.details(QContactAvatar::DefinitionName).count(), 1);
    QCOMPARE(QContactAvatar(c.details(QContactAvatar::DefinitionName).value(0)).avatar(), a1.avatar());
    QCOMPARE(a1.pixmap(), pix);

    // test property update
    a1.setValue("label","label1");
    a1.setAvatar("12345");
    QVERIFY(c.saveDetail(&a1));
    QCOMPARE(c.details(QContactAvatar::DefinitionName).value(0).value("label"), QString("label1"));
    QCOMPARE(c.details(QContactAvatar::DefinitionName).value(0).value(QContactAvatar::FieldAvatar), QString("12345"));

    // test property remove
    QVERIFY(c.removeDetail(&a1));
    QCOMPARE(c.details(QContactAvatar::DefinitionName).count(), 0);
    QVERIFY(c.saveDetail(&a2));
    QCOMPARE(c.details(QContactAvatar::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&a2));
    QCOMPARE(c.details(QContactAvatar::DefinitionName).count(), 0);
    QVERIFY(c.removeDetail(&a2) == false);
    QCOMPARE(c.details(QContactAvatar::DefinitionName).count(), 0);
}

void tst_QContactDetails::birthday()
{
    QContact c;
    QContactBirthday b1, b2;
    QDate currDate = QDate::currentDate();

    // test property set
    b1.setDate(currDate);
    QCOMPARE(b1.date(), currDate);
    QCOMPARE(b1.value<QDate>(QContactBirthday::FieldBirthday), currDate);

    // test property add
    QVERIFY(c.saveDetail(&b1));
    QCOMPARE(c.details(QContactBirthday::DefinitionName).count(), 1);
    QCOMPARE(QContactBirthday(c.details(QContactBirthday::DefinitionName).value(0)).date(), b1.date());

    // test property update
    b1.setValue("label","label1");
    b1.setDate(currDate.addDays(3));
    QVERIFY(c.saveDetail(&b1));
    QCOMPARE(c.details(QContactBirthday::DefinitionName).value(0).value("label"), QString("label1"));
    QCOMPARE(c.details(QContactBirthday::DefinitionName).value(0).value<QDate>(QContactBirthday::FieldBirthday), currDate.addDays(3));

    // test property remove
    QVERIFY(c.removeDetail(&b1));
    QCOMPARE(c.details(QContactBirthday::DefinitionName).count(), 0);
    QVERIFY(c.saveDetail(&b2));
    QCOMPARE(c.details(QContactBirthday::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&b2));
    QCOMPARE(c.details(QContactBirthday::DefinitionName).count(), 0);
    QVERIFY(c.removeDetail(&b2) == false);
    QCOMPARE(c.details(QContactBirthday::DefinitionName).count(), 0);
}

void tst_QContactDetails::displayLabel()
{
    QContactDisplayLabel d1;
    QContact c;

    QVERIFY(d1.label().isEmpty());
    QVERIFY(d1.value(QContactDisplayLabel::FieldLabel).isEmpty());
    d1.setValue(QContactDisplayLabel::FieldLabel, "Test");
    QVERIFY(d1.value(QContactDisplayLabel::FieldLabel) == QString("Test"));
    QVERIFY(d1.label() == QString("Test"));

    QContactDisplayLabel d2;
    d2.setValue(QContactDisplayLabel::FieldLabel, "Test 2");

    // test property add [== fail]
    QVERIFY(!c.saveDetail(&d2));
    QVERIFY(d2.accessConstraints() & QContactDetail::ReadOnly);
    QCOMPARE(c.details(QContactDisplayLabel::DefinitionName).count(), 1);

    // test property update [== fail]
    d1 = c.detail<QContactDisplayLabel>();
    QVERIFY(!c.saveDetail(&d1));
    QVERIFY(d1.accessConstraints() & QContactDetail::ReadOnly);

    // test property remove
    QVERIFY(!c.removeDetail(&d1)); // cannot remove display label
    QCOMPARE(c.details<QContactDisplayLabel>().count(), 1);
}

void tst_QContactDetails::emailAddress()
{
    QContact c;
    QContactEmailAddress e1, e2;

    // test property set
    e1.setEmailAddress("1234");
    QCOMPARE(e1.emailAddress(), QString("1234"));
    QCOMPARE(e1.value(QContactEmailAddress::FieldEmailAddress), QString("1234"));

    // test property add
    QVERIFY(c.saveDetail(&e1));
    QCOMPARE(c.details(QContactEmailAddress::DefinitionName).count(), 1);
    QCOMPARE(QContactEmailAddress(c.details(QContactEmailAddress::DefinitionName).value(0)).emailAddress(), e1.emailAddress());

    // test property update
    e1.setValue("label","label1");
    e1.setEmailAddress("12345");
    QVERIFY(c.saveDetail(&e1));
    QCOMPARE(c.details(QContactEmailAddress::DefinitionName).value(0).value("label"), QString("label1"));
    QCOMPARE(c.details(QContactEmailAddress::DefinitionName).value(0).value(QContactEmailAddress::FieldEmailAddress), QString("12345"));

    // test property remove
    QVERIFY(c.removeDetail(&e1));
    QCOMPARE(c.details(QContactEmailAddress::DefinitionName).count(), 0);
    QVERIFY(c.saveDetail(&e2));
    QCOMPARE(c.details(QContactEmailAddress::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&e2));
    QCOMPARE(c.details(QContactEmailAddress::DefinitionName).count(), 0);
    QVERIFY(c.removeDetail(&e2) == false);
    QCOMPARE(c.details(QContactEmailAddress::DefinitionName).count(), 0);
}

void tst_QContactDetails::family()
{
    QContact c;
    QContactFamily f1, f2;

    // test property set
    f1.setSpouse("1234");
    QCOMPARE(f1.spouse(), QString("1234"));
    QCOMPARE(f1.value(QContactFamily::FieldSpouse), QString("1234"));

    // test property add
    QVERIFY(c.saveDetail(&f1));
    QCOMPARE(c.details(QContactFamily::DefinitionName).count(), 1);
    QCOMPARE(QContactFamily(c.details(QContactFamily::DefinitionName).value(0)).spouse(), f1.spouse());

    // test property update
    f1.setValue("label","label1");
    f1.setSpouse("12345");
    f1.setChildren(QStringList("54321"));
    QVERIFY(c.saveDetail(&f1));
    QCOMPARE(c.details(QContactFamily::DefinitionName).value(0).value("label"), QString("label1"));
    QCOMPARE(c.details(QContactFamily::DefinitionName).value(0).value(QContactFamily::FieldSpouse), QString("12345"));
    QCOMPARE(c.details(QContactFamily::DefinitionName).value(0).value<QStringList>(QContactFamily::FieldChildren), QStringList("54321"));

    // test property remove
    f2.setSpouse("1111");
    QVERIFY(c.removeDetail(&f1));
    QCOMPARE(c.details(QContactFamily::DefinitionName).count(), 0);
    QVERIFY(c.saveDetail(&f2));
    QCOMPARE(c.details(QContactFamily::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&f2));
    QCOMPARE(c.details(QContactFamily::DefinitionName).count(), 0);
    QVERIFY(c.removeDetail(&f2) == false);
    QCOMPARE(c.details(QContactFamily::DefinitionName).count(), 0);
}

void tst_QContactDetails::gender()
{
    QContact c;
    QContactGender g1, g2;

    // test property set
    g1.setGender("1234");
    QCOMPARE(g1.gender(), QString("1234"));
    QCOMPARE(g1.value(QContactGender::FieldGender), QString("1234"));

    // test property add
    QVERIFY(c.saveDetail(&g1));
    QCOMPARE(c.details(QContactGender::DefinitionName).count(), 1);
    QCOMPARE(QContactGender(c.details(QContactGender::DefinitionName).value(0)).gender(), g1.gender());

    // test property update
    g1.setValue("label","label1");
    g1.setGender("12345");
    QVERIFY(c.saveDetail(&g1));
    QCOMPARE(c.details(QContactGender::DefinitionName).value(0).value("label"), QString("label1"));
    QCOMPARE(c.details(QContactGender::DefinitionName).value(0).value(QContactGender::FieldGender), QString("12345"));

    // test property remove
    QVERIFY(c.removeDetail(&g1));
    QCOMPARE(c.details(QContactGender::DefinitionName).count(), 0);
    QVERIFY(c.saveDetail(&g2));
    QCOMPARE(c.details(QContactGender::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&g2));
    QCOMPARE(c.details(QContactGender::DefinitionName).count(), 0);
    QVERIFY(c.removeDetail(&g2) == false);
    QCOMPARE(c.details(QContactGender::DefinitionName).count(), 0);
}

void tst_QContactDetails::geolocation()
{
    QContact c;
    QContactGeoLocation g1, g2;

    // test property set
    g1.setLabel("1234");
    QCOMPARE(g1.label(), QString("1234"));
    QCOMPARE(g1.value(QContactGeoLocation::FieldLabel), QString("1234"));
    g1.setAccuracy(3.2);
    QCOMPARE(g1.accuracy(), 3.2);
    QCOMPARE(g1.variantValue(QContactGeoLocation::FieldAccuracy), QVariant(3.2));
    g1.setAltitude(3.3);
    QCOMPARE(g1.altitude(), 3.3);
    QCOMPARE(g1.variantValue(QContactGeoLocation::FieldAltitude), QVariant(3.3));
    g1.setAltitudeAccuracy(3.4);
    QCOMPARE(g1.altitudeAccuracy(), 3.4);
    QCOMPARE(g1.variantValue(QContactGeoLocation::FieldAltitudeAccuracy), QVariant(3.4));
    g1.setHeading(3.5);
    QCOMPARE(g1.heading(), 3.5);
    QCOMPARE(g1.variantValue(QContactGeoLocation::FieldHeading), QVariant(3.5));
    g1.setLatitude(3.6);
    QCOMPARE(g1.latitude(), 3.6);
    QCOMPARE(g1.variantValue(QContactGeoLocation::FieldLatitude), QVariant(3.6));
    g1.setLongitude(3.7);
    QCOMPARE(g1.longitude(), 3.7);
    QCOMPARE(g1.variantValue(QContactGeoLocation::FieldLongitude), QVariant(3.7));
    QDateTime current = QDateTime::currentDateTime();
    g1.setTimestamp(current);
    QCOMPARE(g1.timestamp(), current);
    QCOMPARE(g1.variantValue(QContactGeoLocation::FieldTimestamp), QVariant(current));
    g1.setSpeed(3.8);
    QCOMPARE(g1.speed(), 3.8);
    QCOMPARE(g1.variantValue(QContactGeoLocation::FieldSpeed), QVariant(3.8));

    // test property add
    QVERIFY(c.saveDetail(&g1));
    QCOMPARE(c.details(QContactGeoLocation::DefinitionName).count(), 1);
    QCOMPARE(QContactGeoLocation(c.details(QContactGeoLocation::DefinitionName).value(0)).label(), g1.label());

    // test property update
    g1.setLabel("12345");
    QVERIFY(c.saveDetail(&g1));
    QCOMPARE(c.details(QContactGeoLocation::DefinitionName).value(0).value(QContactGeoLocation::FieldLabel), QString("12345"));

    // test property remove
    QVERIFY(c.removeDetail(&g1));
    QCOMPARE(c.details(QContactGeoLocation::DefinitionName).count(), 0);
    QVERIFY(c.saveDetail(&g2));
    QCOMPARE(c.details(QContactGeoLocation::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&g2));
    QCOMPARE(c.details(QContactGeoLocation::DefinitionName).count(), 0);
    QVERIFY(c.removeDetail(&g2) == false);
    QCOMPARE(c.details(QContactGeoLocation::DefinitionName).count(), 0);
}

void tst_QContactDetails::guid()
{
    QContact c;
    QContactGuid g1, g2;

    // test property set
    g1.setGuid("1234");
    QCOMPARE(g1.guid(), QString("1234"));
    QCOMPARE(g1.value(QContactGuid::FieldGuid), QString("1234"));

    // test property add
    QVERIFY(c.saveDetail(&g1));
    QCOMPARE(c.details(QContactGuid::DefinitionName).count(), 1);
    QCOMPARE(QContactGuid(c.details(QContactGuid::DefinitionName).value(0)).guid(), g1.guid());

    // test property update
    g1.setValue("label","label1");
    g1.setGuid("12345");
    QVERIFY(c.saveDetail(&g1));
    QCOMPARE(c.details(QContactGuid::DefinitionName).value(0).value("label"), QString("label1"));
    QCOMPARE(c.details(QContactGuid::DefinitionName).value(0).value(QContactGuid::FieldGuid), QString("12345"));

    // test property remove
    QVERIFY(c.removeDetail(&g1));
    QCOMPARE(c.details(QContactGuid::DefinitionName).count(), 0);
    QVERIFY(c.saveDetail(&g2));
    QCOMPARE(c.details(QContactGuid::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&g2));
    QCOMPARE(c.details(QContactGuid::DefinitionName).count(), 0);
    QVERIFY(c.removeDetail(&g2) == false);
    QCOMPARE(c.details(QContactGuid::DefinitionName).count(), 0);
}

void tst_QContactDetails::name()
{
    QContact c;
    QContactName n1, n2;

    QVERIFY(n1.isEmpty());
    QVERIFY(n2.isEmpty());

    // test property set
    n1.setPrefix("Dr");
    n1.setFirstName("Freddy");
    n1.setMiddleName("William Preston");
    n1.setLastName("Gumboots");
    n1.setSuffix("Esquire");
    QCOMPARE(n1.prefix(), QString("Dr"));
    QCOMPARE(n1.firstName(), QString("Freddy"));
    QCOMPARE(n1.middleName(), QString("William Preston"));
    QCOMPARE(n1.lastName(), QString("Gumboots"));
    QCOMPARE(n1.suffix(), QString("Esquire"));

    // test property add
    QVERIFY(c.saveDetail(&n1));
    QCOMPARE(c.details(QContactName::DefinitionName).count(), 1);
    n2.setFirstName("Billy");
    n2.setLastName("Galoshes");
    QVERIFY(c.saveDetail(&n2));
    QCOMPARE(c.details(QContactName::DefinitionName).count(), 2);

    // test property update
    n1.setValue("label","label1");
    QVERIFY(c.saveDetail(&n1));

    // test property remove
    QVERIFY(c.removeDetail(&n1)); // remove
    QCOMPARE(c.details(QContactName::DefinitionName).count(), 1);
    QVERIFY(c.saveDetail(&n2));   // save but already added; count remains the same.
    QCOMPARE(c.details(QContactName::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&n2)); // remove it
    QCOMPARE(c.details(QContactName::DefinitionName).count(), 0);
    QVERIFY(!c.removeDetail(&n2));// remove now fails
    QVERIFY(c.saveDetail(&n1));   // save the original name
    QCOMPARE(c.details(QContactName::DefinitionName).count(), 1);
}

void tst_QContactDetails::nickname()
{
    QContact c;
    QContactNickname n1, n2;

    // test property set
    n1.setNickname("1234");
    QCOMPARE(n1.nickname(), QString("1234"));
    QCOMPARE(n1.value(QContactNickname::FieldNickname), QString("1234"));

    // test property add
    QVERIFY(c.saveDetail(&n1));
    QCOMPARE(c.details(QContactNickname::DefinitionName).count(), 1);
    QCOMPARE(QContactNickname(c.details(QContactNickname::DefinitionName).value(0)).nickname(), n1.nickname());

    // test property update
    n1.setValue("label","label1");
    n1.setNickname("12345");
    QVERIFY(c.saveDetail(&n1));
    QCOMPARE(c.details(QContactNickname::DefinitionName).value(0).value("label"), QString("label1"));
    QCOMPARE(c.details(QContactNickname::DefinitionName).value(0).value(QContactNickname::FieldNickname), QString("12345"));

    // test property remove
    QVERIFY(c.removeDetail(&n1));
    QCOMPARE(c.details(QContactNickname::DefinitionName).count(), 0);
    QVERIFY(c.saveDetail(&n2));
    QCOMPARE(c.details(QContactNickname::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&n2));
    QCOMPARE(c.details(QContactNickname::DefinitionName).count(), 0);
    QVERIFY(c.removeDetail(&n2) == false);
    QCOMPARE(c.details(QContactNickname::DefinitionName).count(), 0);
}

void tst_QContactDetails::onlineAccount()
{
    QContact c;
    QContactOnlineAccount o1, o2;

    // test property set
    o1.setAccountUri("test@nokia.com");
    QCOMPARE(o1.accountUri(), QString("test@nokia.com"));
    QCOMPARE(o1.value(QContactOnlineAccount::FieldAccountUri), QString("test@nokia.com"));
    o1.setNickname("test");
    QCOMPARE(o1.nickname(), QString("test"));
    QCOMPARE(o1.value(QContactOnlineAccount::FieldNickname), QString("test"));
    o1.setStatusMessage("Gone Fishing");
    QCOMPARE(o1.statusMessage(), QString("Gone Fishing"));
    QCOMPARE(o1.value(QContactOnlineAccount::FieldStatusMessage), QString("Gone Fishing"));
    o1.setPresence("Extended Away");
    QCOMPARE(o1.presence(), QString("Extended Away"));
    QCOMPARE(o1.value(QContactOnlineAccount::FieldPresence), QString("Extended Away"));

    // Sub types
    o1.setSubTypes(QContactOnlineAccount::SubTypeSip);
    QCOMPARE(o1.subTypes(), QStringList() << QLatin1String(QContactOnlineAccount::SubTypeSip));

    QStringList sl;
    sl << QLatin1String(QContactOnlineAccount::SubTypeImpp) << QLatin1String(QContactOnlineAccount::SubTypeVideoShare);
    o1.setSubTypes(sl);
    QCOMPARE(o1.subTypes(), sl);

    o1 = QContactOnlineAccount();
    o1.setAccountUri("test@nokia.com");

    // test property add
    QVERIFY(c.saveDetail(&o1));
    QCOMPARE(c.details(QContactOnlineAccount::DefinitionName).count(), 1);
    QCOMPARE(QContactOnlineAccount(c.details(QContactOnlineAccount::DefinitionName).value(0)).accountUri(), o1.accountUri());
    QCOMPARE(QContactOnlineAccount(c.details(QContactOnlineAccount::DefinitionName).value(0)).presence(), o1.presence());
    QCOMPARE(QContactOnlineAccount(c.details(QContactOnlineAccount::DefinitionName).value(0)).nickname(), o1.nickname());
    QCOMPARE(QContactOnlineAccount(c.details(QContactOnlineAccount::DefinitionName).value(0)).statusMessage(), o1.statusMessage());
    QCOMPARE(QContactOnlineAccount(c.details(QContactOnlineAccount::DefinitionName).value(0)).accountUri(), o1.accountUri());

    // test property update
    o1.setValue("label","label1");
    o1.setAccountUri("test2@nokia.com");
    QVERIFY(c.saveDetail(&o1));
    QCOMPARE(c.details(QContactOnlineAccount::DefinitionName).value(0).value("label"), QString("label1"));
    QCOMPARE(c.details(QContactOnlineAccount::DefinitionName).value(0).value(QContactOnlineAccount::FieldAccountUri), QString("test2@nokia.com"));

    // test property remove
    QVERIFY(c.removeDetail(&o1));
    QCOMPARE(c.details(QContactOnlineAccount::DefinitionName).count(), 0);
    QVERIFY(c.saveDetail(&o2));
    QCOMPARE(c.details(QContactOnlineAccount::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&o2));
    QCOMPARE(c.details(QContactOnlineAccount::DefinitionName).count(), 0);
    QVERIFY(c.removeDetail(&o2) == false);
    QCOMPARE(c.details(QContactOnlineAccount::DefinitionName).count(), 0);
}

void tst_QContactDetails::organization()
{
    QContact c;
    QContactOrganization o1, o2;

    // test property set
    o1.setName("organization one");
    QCOMPARE(o1.name(), QString("organization one"));
    QCOMPARE(o1.value(QContactOrganization::FieldName), QString("organization one"));

    o1.setDepartment(QStringList(QString("department one")));
    QCOMPARE(o1.department(), QStringList(QString("department one")));
    QCOMPARE(o1.value<QStringList>(QContactOrganization::FieldDepartment), QStringList(QString("department one")));

    o1.setLocation("location one");
    QCOMPARE(o1.location(), QString("location one"));
    QCOMPARE(o1.value(QContactOrganization::FieldLocation), QString("location one"));

    o1.setLogo("logo one");
    QCOMPARE(o1.logo(), QString("logo one"));
    QCOMPARE(o1.value(QContactOrganization::FieldLogo), QString("logo one"));

    o1.setTitle("title one");
    QCOMPARE(o1.title(), QString("title one"));
    QCOMPARE(o1.value(QContactOrganization::FieldTitle), QString("title one"));

    o1.setAssistantName("assistant one");
    QCOMPARE(o1.assistantName(), QString("assistant one"));
    QCOMPARE(o1.value(QContactOrganization::FieldAssistantName), QString("assistant one"));

    // test property add
    QVERIFY(c.saveDetail(&o1));
    QCOMPARE(c.details(QContactOrganization::DefinitionName).count(), 1);
    QCOMPARE(QContactOrganization(c.details(QContactOrganization::DefinitionName).value(0)).name(), o1.name());

    // test property update
    o1.setName("organization two");
    QVERIFY(c.saveDetail(&o1));
    QCOMPARE(c.details(QContactOrganization::DefinitionName).value(0).value(QContactOrganization::FieldName), QString("organization two"));

    // test property remove
    QVERIFY(c.removeDetail(&o1));
    QCOMPARE(c.details(QContactOrganization::DefinitionName).count(), 0);
    QVERIFY(c.saveDetail(&o2));
    QCOMPARE(c.details(QContactOrganization::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&o2));
    QCOMPARE(c.details(QContactOrganization::DefinitionName).count(), 0);
    QVERIFY(c.removeDetail(&o2) == false);
    QCOMPARE(c.details(QContactOrganization::DefinitionName).count(), 0);

    // organization-specific API testing
    o1.setDepartment(QStringList(QString("Imaginary Dept")));
    o1.setLocation("Utopia");
    o1.setLogo("logo.png");
    o1.setName("Utopian Megacorporation");
    o1.setTitle("Generic Employee");
    c.saveDetail(&o1);
    QVERIFY(c.detail(QContactOrganization::DefinitionName).value<QStringList>(QContactOrganization::FieldDepartment) == QStringList(QString("Imaginary Dept")));
    QVERIFY(c.detail(QContactOrganization::DefinitionName).value(QContactOrganization::FieldLocation) == QString("Utopia"));
    QVERIFY(c.detail(QContactOrganization::DefinitionName).value(QContactOrganization::FieldLogo) == QString("logo.png"));
    QVERIFY(c.detail(QContactOrganization::DefinitionName).value(QContactOrganization::FieldName) == QString("Utopian Megacorporation"));
    QVERIFY(c.detail(QContactOrganization::DefinitionName).value(QContactOrganization::FieldTitle) == QString("Generic Employee"));
}

void tst_QContactDetails::phoneNumber()
{
    QContact c;
    QContactPhoneNumber p1, p2;

    // test property set
    p1.setNumber("1234");
    QCOMPARE(p1.number(), QString("1234"));
    QCOMPARE(p1.value(QContactPhoneNumber::FieldNumber), QString("1234"));

    // Sub types
    p1.setSubTypes(QContactPhoneNumber::SubTypeCar);
    QCOMPARE(p1.subTypes(), QStringList() << QLatin1String(QContactPhoneNumber::SubTypeCar));

    QStringList sl;
    sl << QLatin1String(QContactPhoneNumber::SubTypeMobile) << QLatin1String(QContactPhoneNumber::SubTypeFacsimile);
    p1.setSubTypes(sl);
    QCOMPARE(p1.subTypes(), sl);

    // test property add
    QVERIFY(c.saveDetail(&p1));
    QCOMPARE(c.details(QContactPhoneNumber::DefinitionName).count(), 1);
    QCOMPARE(QContactPhoneNumber(c.details(QContactPhoneNumber::DefinitionName).value(0)).number(), p1.number());

    // test property update
    p1.setValue("label","label1");
    p1.setNumber("12345");
    QVERIFY(c.saveDetail(&p1));
    QCOMPARE(c.details(QContactPhoneNumber::DefinitionName).value(0).value("label"), QString("label1"));
    QCOMPARE(c.details(QContactPhoneNumber::DefinitionName).value(0).value(QContactPhoneNumber::FieldNumber), QString("12345"));

    p1.setSubTypes(QContactPhoneNumber::SubTypeDtmfMenu);
    c.saveDetail(&p1);
    QVERIFY(c.detail(QContactPhoneNumber::DefinitionName).variantValue(QContactPhoneNumber::FieldSubTypes).toStringList() == QStringList(QString(QLatin1String(QContactPhoneNumber::SubTypeDtmfMenu))));
    p1.setSubTypes(QStringList() << QContactPhoneNumber::SubTypeModem << QContactPhoneNumber::SubTypeFacsimile);
    c.saveDetail(&p1);
    QVERIFY(c.detail(QContactPhoneNumber::DefinitionName).variantValue(QContactPhoneNumber::FieldSubTypes).toStringList() == p1.subTypes());

    // test property remove
    QVERIFY(c.removeDetail(&p1));
    QCOMPARE(c.details(QContactPhoneNumber::DefinitionName).count(), 0);
    QVERIFY(c.saveDetail(&p2));
    QCOMPARE(c.details(QContactPhoneNumber::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&p2));
    QCOMPARE(c.details(QContactPhoneNumber::DefinitionName).count(), 0);
    QVERIFY(c.removeDetail(&p2) == false);
    QCOMPARE(c.details(QContactPhoneNumber::DefinitionName).count(), 0);
}

void tst_QContactDetails::syncTarget()
{
    QContact c;
    QContactSyncTarget s1, s2;

    // test property set
    s1.setSyncTarget("1234");
    QCOMPARE(s1.syncTarget(), QString("1234"));
    QCOMPARE(s1.value(QContactSyncTarget::FieldSyncTarget), QString("1234"));

    // test property add
    QVERIFY(c.saveDetail(&s1));
    QCOMPARE(c.details(QContactSyncTarget::DefinitionName).count(), 1);
    QCOMPARE(QContactSyncTarget(c.details(QContactSyncTarget::DefinitionName).value(0)).syncTarget(), s1.syncTarget());

    // test property update
    s1.setValue("label","label1");
    s1.setSyncTarget("12345");
    QVERIFY(c.saveDetail(&s1));
    QCOMPARE(c.details(QContactSyncTarget::DefinitionName).value(0).value("label"), QString("label1"));
    QCOMPARE(c.details(QContactSyncTarget::DefinitionName).value(0).value(QContactSyncTarget::FieldSyncTarget), QString("12345"));

    // test property remove
    QVERIFY(c.removeDetail(&s1));
    QCOMPARE(c.details(QContactSyncTarget::DefinitionName).count(), 0);
    QVERIFY(c.saveDetail(&s2));
    QCOMPARE(c.details(QContactSyncTarget::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&s2));
    QCOMPARE(c.details(QContactSyncTarget::DefinitionName).count(), 0);
    QVERIFY(c.removeDetail(&s2) == false);
    QCOMPARE(c.details(QContactSyncTarget::DefinitionName).count(), 0);
}

void tst_QContactDetails::timestamp()
{
    QContact c;
    QContactTimestamp t1, t2;
    QDateTime modified = QDateTime::currentDateTime();
    QDateTime created = modified.addSecs(-43);

    // test property set
    t1.setCreated(created);
    QCOMPARE(t1.created(), created);
    QCOMPARE(t1.variantValue(QContactTimestamp::FieldCreationTimestamp).toDateTime(), created);

    // test property add
    QVERIFY(c.saveDetail(&t1));
    QCOMPARE(c.details(QContactTimestamp::DefinitionName).count(), 1);
    QCOMPARE(QContactTimestamp(c.details(QContactTimestamp::DefinitionName).value(0)).created(), t1.created());

    // test property update
    t1.setValue("label","label1");
    t1.setLastModified(modified);
    QVERIFY(c.saveDetail(&t1));
    QCOMPARE(c.details(QContactTimestamp::DefinitionName).value(0).value("label"), QString("label1"));
    QCOMPARE(c.details(QContactTimestamp::DefinitionName).value(0).variantValue(QContactTimestamp::FieldCreationTimestamp).toDateTime(), created);
    QCOMPARE(c.details(QContactTimestamp::DefinitionName).value(0).variantValue(QContactTimestamp::FieldModificationTimestamp).toDateTime(), modified);

    // test property remove
    QVERIFY(c.removeDetail(&t1));
    QCOMPARE(c.details(QContactTimestamp::DefinitionName).count(), 0);
    t2.setCreated(created.addSecs(15));
    QVERIFY(c.saveDetail(&t2));
    QCOMPARE(c.details(QContactTimestamp::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&t2));
    QCOMPARE(c.details(QContactTimestamp::DefinitionName).count(), 0);
    QVERIFY(c.removeDetail(&t2) == false);
    QCOMPARE(c.details(QContactTimestamp::DefinitionName).count(), 0);
}

void tst_QContactDetails::type()
{
    QContact c;
    QContactType t1, t2;

    // test property set
    t1.setType(QContactType::TypeGroup);
    QCOMPARE(t1.type(), QString(QLatin1String(QContactType::TypeGroup)));
    QCOMPARE(t1.value(QContactType::FieldType), QString(QLatin1String(QContactType::TypeGroup)));

    // test property add
    QVERIFY(c.saveDetail(&t1));
    QCOMPARE(c.details(QContactType::DefinitionName).count(), 1);
    QCOMPARE(QContactType(c.details(QContactType::DefinitionName).value(0)).type(), t1.type());

    // test property update
    t1.setType(QContactType::TypeContact);
    QVERIFY(c.saveDetail(&t1));
    QCOMPARE(c.details(QContactType::DefinitionName).value(0).value(QContactType::FieldType), QString(QLatin1String(QContactType::TypeContact)));

    // test property remove
    QVERIFY(!c.removeDetail(&t1)); // cannot remove type
    QCOMPARE(c.details(QContactType::DefinitionName).count(), 1);
    t2.setType(QContactType::TypeGroup);
    QVERIFY(c.saveDetail(&t2)); // overwrites t1
    QCOMPARE(c.details(QContactType::DefinitionName).count(), 1);
    QVERIFY(!c.removeDetail(&t2)); // cannot remove type - "succeeds" but count remains unchanged
    QCOMPARE(c.details(QContactType::DefinitionName).count(), 1);
    QVERIFY(!c.removeDetail(&t2));
    QCOMPARE(c.details(QContactType::DefinitionName).count(), 1);
}

void tst_QContactDetails::url()
{
    QContact c;
    QContactUrl u1, u2;

    // test property set
    u1.setUrl("1234");
    QCOMPARE(u1.url(), QString("1234"));
    QCOMPARE(u1.value(QContactUrl::FieldUrl), QString("1234"));

    u1.setSubType(QContactUrl::SubTypeHomePage);
    QCOMPARE(u1.subType(), QString(QLatin1String(QContactUrl::SubTypeHomePage)));
    QCOMPARE(u1.value(QContactUrl::FieldSubType), QString(QLatin1String(QContactUrl::SubTypeHomePage)));

    // test property add
    QVERIFY(c.saveDetail(&u1));
    QCOMPARE(c.details(QContactUrl::DefinitionName).count(), 1);
    QCOMPARE(QContactUrl(c.details(QContactUrl::DefinitionName).value(0)).url(), u1.url());

    // test property update
    u1.setValue("label","label1");
    u1.setUrl("12345");
    QVERIFY(c.saveDetail(&u1));
    QCOMPARE(c.details(QContactUrl::DefinitionName).value(0).value("label"), QString("label1"));
    QCOMPARE(c.details(QContactUrl::DefinitionName).value(0).value(QContactUrl::FieldUrl), QString("12345"));

    // test property remove
    QVERIFY(c.removeDetail(&u1));
    QCOMPARE(c.details(QContactUrl::DefinitionName).count(), 0);
    QVERIFY(c.saveDetail(&u2));
    QCOMPARE(c.details(QContactUrl::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&u2));
    QCOMPARE(c.details(QContactUrl::DefinitionName).count(), 0);
    QVERIFY(c.removeDetail(&u2) == false);
    QCOMPARE(c.details(QContactUrl::DefinitionName).count(), 0);
}

void tst_QContactDetails::custom()
{
    QContact c;
    QContactDetail c1("mycustom"), c2("mycustom");

    // test property set
    c1.setValue("custom", "1234");
    QCOMPARE(c1.value("custom"), QString("1234"));

    // test property add
    QVERIFY(c.saveDetail(&c1));
    QCOMPARE(c.details("mycustom").count(), 1);
    QCOMPARE((c.details("mycustom").value(0)).value("custom"), c1.value("custom"));

    // test property update
    c1.setValue("label","label1");
    c1.setValue("custom", "12345");
    QVERIFY(c.saveDetail(&c1));

    // test property remove
    QVERIFY(c.removeDetail(&c1));
    QCOMPARE(c.details("mycustom").count(), 0);
    QVERIFY(c.saveDetail(&c2));
    QCOMPARE(c.details("mycustom").count(), 1);
    QVERIFY(c.removeDetail(&c2));
    QCOMPARE(c.details("mycustom").count(), 0);
    QVERIFY(c.removeDetail(&c2) == false);
    QCOMPARE(c.details("mycustom").count(), 0);
}

void tst_QContactDetails::note()
{
    QContact c;
    QContactNote n1, n2;

    // test property set
    n1.setNote("lorem ipsum");
    QCOMPARE(n1.note(), QString("lorem ipsum"));
    QCOMPARE(n1.value(QContactNote::FieldNote), QString("lorem ipsum"));

    // test property add
    QVERIFY(c.saveDetail(&n1));
    QCOMPARE(c.details(QContactNote::DefinitionName).count(), 1);
    QCOMPARE(QContactNote(c.details(QContactNote::DefinitionName).value(0)).note(), n1.note());

    // test property update
    n1.setValue("label","label1");
    n1.setNote("orange gypsum");
    QVERIFY(c.saveDetail(&n1));
    QCOMPARE(c.details(QContactNote::DefinitionName).value(0).value("label"), QString("label1"));
    QCOMPARE(c.details(QContactNote::DefinitionName).value(0).value(QContactNote::FieldNote), QString("orange gypsum"));

    // test property remove
    QVERIFY(c.removeDetail(&n1));
    QCOMPARE(c.details(QContactNote::DefinitionName).count(), 0);
    QVERIFY(c.saveDetail(&n2));
    QCOMPARE(c.details(QContactNote::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&n2));
    QCOMPARE(c.details(QContactNote::DefinitionName).count(), 0);
    QVERIFY(c.removeDetail(&n2) == false);
    QCOMPARE(c.details(QContactNote::DefinitionName).count(), 0);
}


QTEST_MAIN(tst_QContactDetails)
#include "tst_qcontactdetails.moc"
