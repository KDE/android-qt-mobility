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

#include "ut_qvcard21writer.h"
#include "qvcard21writer_p.h"
#include "qversitproperty.h"
#include <QtTest/QtTest>
#include <QByteArray>

QTM_USE_NAMESPACE


class MyQVCard21Writer : public QVCard21Writer
{
public:
    // expose some protected functions as public
    QByteArray encodeVersitProperty(const QVersitProperty& property) {return QVCard21Writer::encodeVersitProperty(property);}
    QByteArray encodeParameters(const QMultiHash<QString,QString>& parameters) const {return QVCard21Writer::encodeParameters(parameters);}
    bool quotedPrintableEncode(const QVersitProperty& property, QByteArray& value) const {return QVCard21Writer::quotedPrintableEncode(property, value);}
    QByteArray encodeGroupsAndName(const QVersitProperty& property) const {return QVCard21Writer::encodeGroupsAndName(property);}
};

void UT_QVCard21Writer::init()
{
    mWriter = new MyQVCard21Writer;
}

void UT_QVCard21Writer::cleanup()
{
    delete mWriter;
}

void UT_QVCard21Writer::testEncodeVersitProperty()
{
    // No parameters
    QByteArray expectedResult = "FN:John Citizen\r\n";
    QVersitProperty property;
    property.setName(QString::fromAscii("FN"));
    property.setValue(QByteArray("John Citizen"));
    QCOMPARE(mWriter->encodeVersitProperty(property), expectedResult);

    // With parameter(s). No special characters in the value.
    // -> No need to Quoted-Printable encode the value.
    expectedResult = "TEL;HOME:123\r\n";

    property.setName(QString::fromAscii("TEL"));
    property.setValue(QByteArray("123"));
    property.addParameter(QString::fromAscii("TYPE"),QString::fromAscii("HOME"));
    QByteArray encodedProperty = mWriter->encodeVersitProperty(property);
    QCOMPARE(QString::fromAscii(encodedProperty), QString::fromAscii(expectedResult));
    QCOMPARE(mWriter->encodeVersitProperty(property), expectedResult);
    
    // With parameter(s). Special characters in the value.
    // -> The value needs to be Quoted-Printable encoded.
    expectedResult = "EMAIL;HOME;ENCODING=QUOTED-PRINTABLE:john.citizen=40example.com\r\n";
    property.setName(QString::fromAscii("EMAIL"));
    property.setValue(QByteArray("john.citizen@example.com"));
    QCOMPARE(QString::fromAscii(mWriter->encodeVersitProperty(property)),
             QString::fromAscii(expectedResult));
    
    // AGENT property with parameter
    expectedResult = 
"AGENT;X-PARAMETER=VALUE:\r\n\
BEGIN:VCARD\r\n\
VERSION:2.1\r\n\
FN:Secret Agent\r\n\
END:VCARD\r\n\
\r\n";
    property.setParameters(QMultiHash<QString,QString>());
    property.setName(QString::fromAscii("AGENT"));
    property.setValue(QByteArray());
    property.addParameter(QString::fromAscii("X-PARAMETER"),QString::fromAscii("VALUE"));
    QVersitDocument document;
    QVersitProperty embeddedProperty;
    embeddedProperty.setName(QString(QString::fromAscii("FN")));
    embeddedProperty.setValue(QByteArray("Secret Agent"));
    document.addProperty(embeddedProperty);
    property.setEmbeddedDocument(document);
    QCOMPARE(mWriter->encodeVersitProperty(property), expectedResult);

    // Value is base64 encoded.
    // Check that the extra folding and the line break are added
    QByteArray value = QByteArray("value").toBase64();
    expectedResult = "Springfield.HOUSE.PHOTO;ENCODING=BASE64:\r\n " + value + "\r\n\r\n";
    QStringList groups(QString::fromAscii("Springfield"));
    groups.append(QString::fromAscii("HOUSE"));
    property.setGroups(groups);
    property.setParameters(QMultiHash<QString,QString>());
    property.setName(QString::fromAscii("PHOTO"));
    property.setValue(value);
    property.addParameter(QString::fromAscii("ENCODING"),QString::fromAscii("BASE64"));
    QCOMPARE(QString::fromAscii(mWriter->encodeVersitProperty(property).data()),
             QString::fromAscii(expectedResult.data()));

}

void UT_QVCard21Writer::testEncodeParameters()
{
    QString typeParameterName(QString::fromAscii("TYPE"));
    QString encodingParameterName(QString::fromAscii("ENCODING"));

    // No parameters
    QMultiHash<QString,QString> parameters;
    QCOMPARE(QString::fromAscii(mWriter->encodeParameters(parameters)),
             QString());

    // One TYPE parameter
    parameters.insert(typeParameterName,QString::fromAscii("HOME"));
    QCOMPARE(QString::fromAscii(mWriter->encodeParameters(parameters)),
             QString::fromAscii(";HOME"));

    // Two TYPE parameters
    parameters.insert(typeParameterName,QString::fromAscii("VOICE"));
    QCOMPARE(QString::fromAscii(mWriter->encodeParameters(parameters)),
             QString::fromAscii(";VOICE;HOME"));

    // One ENCODING parameter
    parameters.clear();
    parameters.insert(encodingParameterName,QString::fromAscii("8BIT"));
    QCOMPARE(QString::fromAscii(mWriter->encodeParameters(parameters)),
             QString::fromAscii(";ENCODING=8BIT"));

    // Two parameters
    parameters.insert(QString::fromAscii("X-PARAM"),QString::fromAscii("VALUE"));
    QCOMPARE(QString::fromAscii(mWriter->encodeParameters(parameters)),
             QString::fromAscii(";X-PARAM=VALUE;ENCODING=8BIT"));
}

void UT_QVCard21Writer::testQuotedPrintableEncode()
{
    QByteArray encodedValue;
    
    // The property doesn't contain ENCODING parameter, 
    // no special characters in the encodedValue -> no need to use Quoted-Printable encode
    QVersitProperty property;
    property.setName(QString::fromAscii("N"));
    property.setValue(QByteArray("Citizen;John"));
    QVERIFY(!mWriter->quotedPrintableEncode(property,encodedValue));
    QVERIFY(encodedValue == property.value());
    
    // The property doesn't contain ENCODING parameter,
    // special characters in the encodedValue -> needs to be Quoted-Printable encoded
    property.setName(QString::fromAscii("EMAIL"));
    property.setValue(QByteArray("john.citizen@example.com"));
    QVERIFY(mWriter->quotedPrintableEncode(property,encodedValue));
    QCOMPARE(QString::fromAscii(encodedValue), QString::fromAscii("john.citizen=40example.com"));
    
    // The property contains ENCODING parameter
    // -> Value should not be Quoted-Printable encoded
    property.setName(QString::fromAscii("PHOTO"));
    property.setValue(QByteArray("the data").toBase64());
    property.addParameter(QString::fromAscii("ENCODING"),QString::fromAscii("BASE64"));
    QVERIFY(!mWriter->quotedPrintableEncode(property,encodedValue));
    QVERIFY(encodedValue == property.value());
}

void UT_QVCard21Writer::testEncodeGroupsAndName()
{
    QVersitProperty property;

    // No groups
    property.setName(QString::fromAscii("name"));
    QByteArray result("NAME");
    QCOMPARE(mWriter->encodeGroupsAndName(property),result);

    // One group
    property.setGroups(QStringList(QString::fromAscii("group")));
    result = "group.NAME";
    QCOMPARE(mWriter->encodeGroupsAndName(property),result);

    // Two groups
    QStringList groups(QString::fromAscii("group1"));
    groups.append(QString::fromAscii("group2"));
    property.setGroups(groups);
    result = "group1.group2.NAME";
    QCOMPARE(mWriter->encodeGroupsAndName(property),result);
}


QTEST_MAIN(UT_QVCard21Writer)

