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

#include "ut_qversitreader.h"
#include "qversitreader.h"
#include "qversitreader_p.h"
#include <QtTest/QtTest>

QTM_USE_NAMESPACE

void UT_QVersitReader::init()
{
    mReadingDoneCalled = false;
    mExpectedDocumentCount = 0;
    mInputDevice = new QBuffer;
    mInputDevice->open(QBuffer::ReadWrite);
    mReader = new QVersitReader;
    connect(mReader,SIGNAL(readingDone()),this,SLOT(readingDone()),Qt::DirectConnection);
    mReaderPrivate = new QVersitReaderPrivate;
}

void UT_QVersitReader::cleanup()
{   
    delete mReaderPrivate;
    delete mReader;
    delete mInputDevice;
}

void UT_QVersitReader::readingDone()
{
    QCOMPARE(mReader->result().count(),mExpectedDocumentCount);
    mReadingDoneCalled = true;
}

void UT_QVersitReader::testDevice()
{
    // No device
    QVERIFY(mReader->device() == NULL);    
    
    // Device has been set
    mReader->setDevice(mInputDevice);
    QVERIFY(mReader->device() == mInputDevice);
}

void UT_QVersitReader::testReading()
{
    // No I/O device set
    QVERIFY(!mReader->readAll());
    
    // Device set, no data
    mReader->setDevice(mInputDevice);
    mInputDevice->open(QBuffer::ReadWrite);
    QVERIFY(!mReader->readAll());
    QCOMPARE(mReader->result().count(),0);

    // Device set, one document
    const QByteArray& oneDocument = 
        "BEGIN:VCARD\r\nVERSION:2.1\r\nFN:John\r\nEND:VCARD\r\n";
    mInputDevice->write(oneDocument);
    mInputDevice->seek(0);
    QVERIFY(mReader->readAll());
    QCOMPARE(mReader->result().count(),1);

    // Two documents
    const QByteArray& twoDocuments =
        "BEGIN:VCARD\r\nFN:Jenny\r\nEND:VCARD\r\nBEGIN:VCARD\r\nFN:Jake\r\nEND:VCARD\r\n";
    delete mInputDevice;
    mInputDevice = 0;
    mInputDevice = new QBuffer;
    mInputDevice->open(QBuffer::ReadWrite);
    mInputDevice->write(twoDocuments);
    mInputDevice->seek(0);
    QVERIFY(mReader->readAll());
    QCOMPARE(mReader->result().count(),2);

    // Valid documents and a grouped document between them
    const QByteArray& validDocumentsAndGroupedDocument =
"BEGIN:VCARD\r\nFN:Jenny\r\nEND:VCARD\r\n\
BEGIN:VCARD\r\n\
X-GROUPING:pub gang\r\n\
BEGIN:VCARD\r\nFN:Jeremy\r\nEND:VCARD\r\n\
BEGIN:VCARD\r\nFN:Jeffery\r\nEND:VCARD\r\n\
END:VCARD\r\n\
BEGIN:VCARD\r\nFN:Jake\r\nEND:VCARD\r\n\
BEGIN:VCARD\r\nFN:James\r\nEND:VCARD\r\n\
BEGIN:VCARD\r\nFN:Jane\r\nEND:VCARD\r\n";
    delete mInputDevice;
    mInputDevice = 0;
    mInputDevice = new QBuffer;
    mInputDevice->open(QBuffer::ReadWrite);
    mInputDevice->write(validDocumentsAndGroupedDocument);
    mInputDevice->seek(0);
    QVERIFY(mReader->readAll());
    QCOMPARE(mReader->result().count(),4);

    // Asynchronous reading
    QVERIFY(!mReadingDoneCalled);
    delete mInputDevice;
    mInputDevice = 0;
    mInputDevice = new QBuffer;
    mInputDevice->open(QBuffer::ReadWrite);
    mInputDevice->write(oneDocument);
    mInputDevice->seek(0);
    mExpectedDocumentCount = 1;
    QVERIFY(mReader->startReading());
    delete mReader; // waits for the thread to finish
    mReader = 0;
    QVERIFY(mReadingDoneCalled);
}

void UT_QVersitReader::testResult()
{
    QCOMPARE(mReader->result().count(),0);
}

void UT_QVersitReader::testSetVersionFromProperty()
{
    QVersitDocument document;

    // Some other property than VERSION
    QVersitProperty property;
    property.setName(QString::fromAscii("N"));
    QVERIFY(mReaderPrivate->setVersionFromProperty(document,property));
    
    // VERSION property with 2.1
    property.setName(QString::fromAscii("VERSION"));
    property.setValue(QByteArray("2.1"));
    QVERIFY(mReaderPrivate->setVersionFromProperty(document,property));
    QVERIFY(document.versitType() == QVersitDocument::VCard21);
    
    // VERSION property with 3.0
    property.setValue(QByteArray("3.0"));
    QVERIFY(mReaderPrivate->setVersionFromProperty(document,property));
    QVERIFY(document.versitType() == QVersitDocument::VCard30);

    // VERSION property with a not supported value
    property.setValue(QByteArray("4.0"));
    QVERIFY(!mReaderPrivate->setVersionFromProperty(document,property));
    
    // VERSION property with BASE64 encoded supported value
    property.setValue(QByteArray("2.1").toBase64());
    property.addParameter(QString::fromAscii("ENCODING"),QString::fromAscii("BASE64"));
    QVERIFY(mReaderPrivate->setVersionFromProperty(document,property));
    QVERIFY(document.versitType() == QVersitDocument::VCard21);
    
    // VERSION property with BASE64 encoded not supported value
    property.setValue(QByteArray("4.0").toBase64());
    QVERIFY(!mReaderPrivate->setVersionFromProperty(document,property));
}

void UT_QVersitReader::testParseNextVersitPropertyVCard21()
{
    QVersitDocument::VersitType type = QVersitDocument::VCard21;

    // Test a valid vCard 2.1 with properties having separate handling:
    // AGENT property, some property with parameter ENCODING=QUOTED-PRINTABLE 
    // and some other property without this parameter
    QByteArray vCard("Begin:vcard\r\n");
    vCard.append("VERSION:2.1\r\n");
    vCard.append("FN:John\r\n");
    vCard.append("PHOTO;ENCODING=BASE64: U\t XQgaX MgZ\t3Jl YXQh\r\n\r\n");
    vCard.append("HOME.Springfield.EMAIL;Encoding=Quoted-Printable:john.citizen=40exam=\r\nple.com\r\n");
    vCard.append("AGENT:\r\nBEGIN:VCARD\r\nFN:Jenny\r\nEND:VCARD\r\n\r\n");
    vCard.append("End:VCARD\r\n");

    QVersitProperty property = mReaderPrivate->parseNextVersitProperty(type,vCard);
    QCOMPARE(property.name(),QString::fromAscii("BEGIN"));
    QCOMPARE(property.value(),QByteArray("vcard"));
    
    property = mReaderPrivate->parseNextVersitProperty(type,vCard);
    QCOMPARE(property.name(),QString::fromAscii("VERSION"));
    QCOMPARE(property.value(),QByteArray("2.1"));
    
    property = mReaderPrivate->parseNextVersitProperty(type,vCard);
    QCOMPARE(property.name(),QString::fromAscii("FN"));
    QCOMPARE(property.value(),QByteArray("John"));
    
    property = mReaderPrivate->parseNextVersitProperty(type,vCard);
    QCOMPARE(property.name(),QString::fromAscii("PHOTO"));
    QCOMPARE(1,property.parameters().count());
    // Linear whitespaces (SPACEs and TABs) removed from the value:
    QCOMPARE(property.value(),QByteArray("UXQgaXMgZ3JlYXQh"));

    property = mReaderPrivate->parseNextVersitProperty(type,vCard);
    QStringList propertyGroup(QString::fromAscii("HOME"));
    propertyGroup.append(QString::fromAscii("Springfield"));
    QCOMPARE(property.groups(),propertyGroup);
    QCOMPARE(property.name(),QString::fromAscii("EMAIL"));
    QCOMPARE(0,property.parameters().count());
    QCOMPARE(property.value(),QByteArray("john.citizen@example.com"));
    
    property = mReaderPrivate->parseNextVersitProperty(type,vCard);
    QCOMPARE(property.name(),QString::fromAscii("AGENT"));
    QCOMPARE(property.value(),QByteArray());
    QCOMPARE(property.embeddedDocument().properties().count(),1);
    
    property = mReaderPrivate->parseNextVersitProperty(type,vCard);
    QCOMPARE(property.name(),QString::fromAscii("END"));
    QCOMPARE(property.value(),QByteArray("VCARD"));
    
    property = mReaderPrivate->parseNextVersitProperty(type,vCard);
    QCOMPARE(property.name(),QString());
    QCOMPARE(property.value(),QByteArray());
    
    // Simulate a situation where the document nesting level is exceeded
    // In practice this would mean a big number of nested AGENT properties
    mReaderPrivate->mDocumentNestingLevel = 20;
    QByteArray agentProperty("AGENT:BEGIN:VCARD\r\nN:Jenny\r\nEND:VCARD\r\n\r\n");
    property = mReaderPrivate->parseNextVersitProperty(type,agentProperty);
    QCOMPARE(property.name(),QString());
    QCOMPARE(property.embeddedDocument().properties().count(),0);
    QCOMPARE(property.value(),QByteArray());
}

void UT_QVersitReader::testParseNextVersitPropertyVCard30()
{
    QVersitDocument::VersitType type = QVersitDocument::VCard30;

    // Test a valid vCard 3.0 with properties having separate handling:
    // AGENT property and some other property
    QByteArray vCard("Begin:vcard\r\n");
    vCard.append("VERSION:3.0\r\n");
    vCard.append("FN:John\r\n");
    vCard.append("TEL;TYPE=PREF;HOME:123\r\n");
    vCard.append("PHOTO;ENCODING=B:UXQgaXMgZ3JlYXQh\r\n");
    vCard.append("EMAIL:john.citizen@example.com\r\n");
    vCard.append("AGENT:BEGIN:VCARD\\nFN:Jenny\\nEND:VCARD\\n\r\n");
    vCard.append("End:VCARD\r\n");

    QVersitProperty property = mReaderPrivate->parseNextVersitProperty(type,vCard);
    QCOMPARE(property.name(),QString::fromAscii("BEGIN"));
    QCOMPARE(property.value(),QByteArray("vcard"));

    property = mReaderPrivate->parseNextVersitProperty(type,vCard);
    QCOMPARE(property.name(),QString::fromAscii("VERSION"));
    QCOMPARE(property.value(),QByteArray("3.0"));

    property = mReaderPrivate->parseNextVersitProperty(type,vCard);
    QCOMPARE(property.name(),QString::fromAscii("FN"));
    QCOMPARE(property.value(),QByteArray("John"));

    property = mReaderPrivate->parseNextVersitProperty(type,vCard);
    QCOMPARE(property.name(),QString::fromAscii("TEL"));
    QCOMPARE(property.value(),QByteArray("123"));
    QCOMPARE(property.parameters().count(), 2);

    property = mReaderPrivate->parseNextVersitProperty(type,vCard);
    QCOMPARE(property.name(),QString::fromAscii("PHOTO"));
    QCOMPARE(1,property.parameters().count());
    QCOMPARE(property.value(),QByteArray("UXQgaXMgZ3JlYXQh"));

    property = mReaderPrivate->parseNextVersitProperty(type,vCard);
    QCOMPARE(property.name(),QString::fromAscii("EMAIL"));
    QCOMPARE(0,property.parameters().count());
    QCOMPARE(property.value(),QByteArray("john.citizen@example.com"));

    property = mReaderPrivate->parseNextVersitProperty(type,vCard);
    QCOMPARE(property.name(),QString::fromAscii("AGENT"));
    QCOMPARE(property.value(),QByteArray());
    QCOMPARE(property.embeddedDocument().properties().count(),1);

    property = mReaderPrivate->parseNextVersitProperty(type,vCard);
    QCOMPARE(property.name(),QString::fromAscii("END"));
    QCOMPARE(property.value(),QByteArray("VCARD"));

    property = mReaderPrivate->parseNextVersitProperty(type,vCard);
    QCOMPARE(property.name(),QString());
    QCOMPARE(property.value(),QByteArray());

    // Simulate a situation where the document nesting level is exceeded
    // In practice this would mean a big number of nested AGENT properties
    mReaderPrivate->mDocumentNestingLevel = 20;
    QByteArray agentProperty("AGENT:BEGIN\\:VCARD\\nFN\\:Jenny\\nEND\\:VCARD\\n\r\n");
    property = mReaderPrivate->parseNextVersitProperty(type,agentProperty);
    QCOMPARE(property.name(),QString());
    QCOMPARE(property.embeddedDocument().properties().count(),0);
    QCOMPARE(property.value(),QByteArray());
}

void UT_QVersitReader::testParseVersitDocument()
{
    // Valid vCard 2.1
    const char validCard21[] =
"BEGIN:VCARD\r\n\
VERSION:2.1\r\n\
FN:John\r\n\
AGENT:BEGIN:VCARD\r\nN:Jenny\r\nEND:VCARD\r\n\r\n\
EMAIL;ENCODING=QUOTED-PRINTABLE:john.citizen=40exam=\r\nple.com\r\n\
END:VCARD\r\n";
    QByteArray vCard(validCard21);
    QVersitDocument document;
    QVERIFY(mReaderPrivate->parseVersitDocument(vCard,document));
    QCOMPARE(document.properties().count(),3);
    QCOMPARE(mReaderPrivate->mDocumentNestingLevel,0);

    // Valid vCard 3.0
    const char validCard30[] =
"BEGIN:VCARD\r\n\
VERSION:3.0\r\n\
FN:John\r\n\
AGENT:BEGIN\\:VCARD\\nN\\:Jenny\\nEND\\:VCARD\\n\r\n\
EMAIL:john.citizen@example.com\r\n\
END:VCARD\r\n";
    vCard = validCard30;
    document = QVersitDocument();
    QVERIFY(mReaderPrivate->parseVersitDocument(vCard,document));
    QCOMPARE(document.properties().count(),3);
    QCOMPARE(mReaderPrivate->mDocumentNestingLevel,0);
    
    // No BEGIN found
    const char beginMissing[] = 
"VCARD\r\n\
VERSION:2.1\r\n\
FN:Nobody\r\n\
END:VCARD\r\n";
    vCard = beginMissing;
    document = QVersitDocument();
    QVERIFY(mReaderPrivate->parseVersitDocument(vCard,document));
    QCOMPARE(document.properties().count(),0);
    QCOMPARE(mReaderPrivate->mDocumentNestingLevel,0);
    
    // Wrong card type
    const char wrongType[] = 
"BEGIN:VCAL\r\n\
END:VCAL\r\n";
    vCard = wrongType;
    document = QVersitDocument();
    QVERIFY(mReaderPrivate->parseVersitDocument(vCard,document));
    QCOMPARE(document.properties().count(),0);
    QCOMPARE(mReaderPrivate->mDocumentNestingLevel,0);
    
    // Wrong version
    const char wrongVersion[] = 
"BEGIN:VCARD\r\n\
VERSION:4.0\r\n\
FN:Nobody\r\n\
END:VCARD\r\n";
    vCard = wrongVersion;
    document = QVersitDocument();
    QVERIFY(!mReaderPrivate->parseVersitDocument(vCard,document));
    QCOMPARE(document.properties().count(),0);
    QCOMPARE(mReaderPrivate->mDocumentNestingLevel,0);

    // Grouped vCards are not supported. The whole vCard will be discarded.
    const char groupedCard[] =
"BEGIN:VCARD\r\n\
VERSION:2.1\r\n\
X-EXAMPLES:Family vCard\r\n\
BEGIN:VCARD\r\n\
VERSION:2.1\r\n\
N:Citizen;John\r\n\
TEL;CELL:1111\r\n\
EMAIL;ENCODING=QUOTED-PRINTABLE:john.citizen=40example.com\r\n\
END:VCARD\r\n\
BEGIN:VCARD\r\n\
VERSION:2.1\r\n\
N:Citizen;Jenny\r\n\
TEL;CELL:7777\r\n\
END:VCARD\r\n\
END:VCARD";
    vCard = groupedCard;
    document = QVersitDocument();
    QVERIFY(!mReaderPrivate->parseVersitDocument(vCard,document));
    QCOMPARE(mReaderPrivate->mDocumentNestingLevel, 0);
    QCOMPARE(mReaderPrivate->mVersitDocuments.count(), 0);
}

QTEST_MAIN(UT_QVersitReader)

