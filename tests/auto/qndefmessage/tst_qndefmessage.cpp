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

#include <QtTest/QtTest>

#include <QDebug>

#include <qndefrecord.h>
#include <qndefmessage.h>
#include <qndefnfctextrecord.h>
#include <qndefnfcurirecord.h>


QTM_USE_NAMESPACE

class tst_QNdefMessage : public QObject
{
    Q_OBJECT

public:
    tst_QNdefMessage();
    ~tst_QNdefMessage();

private slots:
    void tst_parse_data();
    void tst_parse();
};

tst_QNdefMessage::tst_QNdefMessage()
{
}

tst_QNdefMessage::~tst_QNdefMessage()
{
}

void tst_QNdefMessage::tst_parse_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<QNdefMessage>("message");
    QTest::addColumn<QVariantList>("expectedData");

    // empty message
    {
        QByteArray data;
        data.append(char(0xc0));   // MB=1, ME=1
        data.append(char(0));      // TYPE LENGTH
        data.append(char(0));      // PAYLOAD LENGTH 3
        data.append(char(0));      // PAYLOAD LENGTH 2
        data.append(char(0));      // PAYLOAD LENGTH 1
        data.append(char(0));      // PAYLOAD LENGTH 0
        QTest::newRow("empty") << data << QNdefMessage() << QVariantList();

        QNdefRecord record;
        record.setTypeNameFormat(QNdefRecord::Empty);
        QTest::newRow("empty record") << data
                                      << QNdefMessage(QList<QNdefRecord>() << record)
                                      << QVariantList();
    }

    // empty short message
    {
        QByteArray data;
        data.append(char(0xd0));   // MB=1, ME=1, SR=1
        data.append(char(0));      // TYPE LENGTH
        data.append(char(0));      // PAYLOAD LENGTH
        QTest::newRow("empty") << data << QNdefMessage() << QVariantList();

        QNdefRecord record;
        record.setTypeNameFormat(QNdefRecord::Empty);
        QTest::newRow("empty record") << data
                                      << QNdefMessage(QList<QNdefRecord>() << record)
                                      << QVariantList();
    }

    // unknown message
    {
        QByteArray type("type");
        QByteArray id("id");
        QByteArray payload("payload");

        QByteArray data;
        data.append(char(0xcd));                            // MB=1, ME=1, IL=1, TNF=5
        data.append(char(type.length()));                   // TYPE LENGTH
        data.append(char((payload.length() >> 24) & 0xff)); // PAYLOAD LENGTH 3
        data.append(char((payload.length() >> 16) & 0xff)); // PAYLOAD LENGTH 2
        data.append(char((payload.length() >> 8) & 0xff));  // PAYLOAD LENGTH 1
        data.append(char((payload.length() >> 0) & 0xff));  // PAYLOAD LENGTH 0
        data.append(char(id.length()));                     // ID LENGTH
        data.append(type);
        data.append(id);
        data.append(payload);

        QNdefRecord record;
        record.setTypeNameFormat(QNdefRecord::Unknown);
        record.setType(type);
        record.setId(id);
        record.setPayload(payload);
        QList<QNdefRecord> recordList;
        recordList.append(record);
        QTest::newRow("unknown") << data << QNdefMessage(recordList) << QVariantList();
    }

    // NFC-RTD Text
    {
        QByteArray type("T");
        QByteArray payload;
        payload.append(char(0x02));     // UTF-8, 2 byte language code
        payload.append("en");
        payload.append("Test String");

        QByteArray data;
        data.append(char(0xc1));            // MB=1, ME=1, IL=0, TNF=1
        data.append(char(type.length()));   // TYPE LENGTH
        data.append(char((payload.length() >> 24) & 0xff)); // PAYLOAD LENGTH 3
        data.append(char((payload.length() >> 16) & 0xff)); // PAYLOAD LENGTH 2
        data.append(char((payload.length() >> 8) & 0xff));  // PAYLOAD LENGTH 1
        data.append(char((payload.length() >> 0) & 0xff));  // PAYLOAD LENGTH 0
        data.append(type);
        data.append(payload);

        QNdefRecord record;
        record.setTypeNameFormat(QNdefRecord::NfcRtd);
        record.setType("T");
        record.setPayload("\002enTest String");
        QList<QNdefRecord> recordList;
        recordList.append(record);
        QTest::newRow("nfc-rtd text") << data << QNdefMessage(recordList)
                                      << (QVariantList() << QLatin1String("Test String")
                                                         << QLocale(QLatin1String("en")));
    }

    // NFC-RTD Text
    {
        QByteArray type("T");
        QByteArray payload;
        payload.append(char(0x02));     // UTF-8, 2 byte language code
        payload.append("ja");
        payload.append(QByteArray::fromHex("e38386e382b9e38388e69687e5ad97e58897"));

        QByteArray data;
        data.append(char(0xc1));            // MB=1, ME=1, IL=0, TNF=1
        data.append(char(type.length()));   // TYPE LENGTH
        data.append(char((payload.length() >> 24) & 0xff)); // PAYLOAD LENGTH 3
        data.append(char((payload.length() >> 16) & 0xff)); // PAYLOAD LENGTH 2
        data.append(char((payload.length() >> 8) & 0xff));  // PAYLOAD LENGTH 1
        data.append(char((payload.length() >> 0) & 0xff));  // PAYLOAD LENGTH 0
        data.append(type);
        data.append(payload);

        QNdefRecord record;
        record.setTypeNameFormat(QNdefRecord::NfcRtd);
        record.setType("T");
        record.setPayload("\002ja" + QByteArray::fromHex("e38386e382b9e38388e69687e5ad97e58897"));
        QList<QNdefRecord> recordList;
        recordList.append(record);
        QTest::newRow("nfc-rtd text ja")
            << data << QNdefMessage(recordList)
            << (QVariantList() << QString::fromUtf8("\343\203\206\343\202\271\343\203\210\346\226"
                                                    "\207\345\255\227\345\210\227")
                               << QLocale(QLatin1String("ja")));
    }

    // NFC-RTD URI
    {
        QByteArray type("U");
        QByteArray payload;
        payload.append(char(0x00));
        payload.append("http://qt.nokia.com/");

        QByteArray data;
        data.append(char(0xc1));
        data.append(char(type.length()));
        data.append(char((payload.length() >> 24) & 0xff)); // PAYLOAD LENGTH 3
        data.append(char((payload.length() >> 16) & 0xff)); // PAYLOAD LENGTH 2
        data.append(char((payload.length() >> 8) & 0xff));  // PAYLOAD LENGTH 1
        data.append(char((payload.length() >> 0) & 0xff));  // PAYLOAD LENGTH 0
        data.append(type);
        data.append(payload);

        QNdefRecord record;
        record.setTypeNameFormat(QNdefRecord::NfcRtd);
        record.setType("U");
        record.setPayload(QByteArray("\000http://qt.nokia.com/", 21));
        QList<QNdefRecord> recordList;
        recordList.append(record);
        QTest::newRow("nfc-rtd uri http://qt.nokia.com/")
            << data << QNdefMessage(recordList)
            << (QVariantList() << QUrl(QLatin1String("http://qt.nokia.com/")));
    }

    // NFC-RTD URI
    {
        QByteArray type("U");
        QByteArray payload;
        payload.append(char(0x03));
        payload.append("qt.nokia.com/");

        QByteArray data;
        data.append(char(0xc1));
        data.append(char(type.length()));
        data.append(char((payload.length() >> 24) & 0xff)); // PAYLOAD LENGTH 3
        data.append(char((payload.length() >> 16) & 0xff)); // PAYLOAD LENGTH 2
        data.append(char((payload.length() >> 8) & 0xff));  // PAYLOAD LENGTH 1
        data.append(char((payload.length() >> 0) & 0xff));  // PAYLOAD LENGTH 0
        data.append(type);
        data.append(payload);

        QNdefRecord record;
        record.setTypeNameFormat(QNdefRecord::NfcRtd);
        record.setType("U");
        record.setPayload(QByteArray("\003qt.nokia.com/", 14));
        QList<QNdefRecord> recordList;
        recordList.append(record);
        QTest::newRow("nfc-rtd uri abbrev http://qt.nokia.com/")
            << data << QNdefMessage(recordList)
            << (QVariantList() << QUrl(QLatin1String("http://qt.nokia.com/")));
    }

    // NFC-RTD URI
    {
        QByteArray type("U");
        QByteArray payload;
        payload.append(char(0x05));
        payload.append("+1234567890");

        QByteArray data;
        data.append(char(0xc1));
        data.append(char(type.length()));
        data.append(char((payload.length() >> 24) & 0xff)); // PAYLOAD LENGTH 3
        data.append(char((payload.length() >> 16) & 0xff)); // PAYLOAD LENGTH 2
        data.append(char((payload.length() >> 8) & 0xff));  // PAYLOAD LENGTH 1
        data.append(char((payload.length() >> 0) & 0xff));  // PAYLOAD LENGTH 0
        data.append(type);
        data.append(payload);

        QNdefRecord record;
        record.setTypeNameFormat(QNdefRecord::NfcRtd);
        record.setType("U");
        record.setPayload(QByteArray("\005+1234567890", 12));
        QList<QNdefRecord> recordList;
        recordList.append(record);
        QTest::newRow("nfc-rtd uri tel:+1234567890")
            << data << QNdefMessage(recordList)
            << (QVariantList() << QUrl(QLatin1String("tel:+1234567890")));
    }
}

void tst_QNdefMessage::tst_parse()
{
    QFETCH(QByteArray, data);
    QFETCH(QNdefMessage, message);
    QFETCH(QVariantList, expectedData);

    QNdefMessage parsedMessage(data);

    QVERIFY(parsedMessage == message);
    QVERIFY(message == parsedMessage);

    const QList<QNdefRecord> &records = message.records();
    const QList<QNdefRecord> &parsedRecords = parsedMessage.records();

    for (int i = 0; i < records.count(); ++i) {
        const QNdefRecord &record = records.at(i);
        const QNdefRecord &parsedRecord = parsedRecords.at(i);

        // Test NDEF NFC Text
        {
            QNdefNfcTextRecord textRecord(record);
            QNdefNfcTextRecord parsedTextRecord(parsedRecord);

            QCOMPARE(textRecord.text(), parsedTextRecord.text());
            QCOMPARE(textRecord.locale(), parsedTextRecord.locale());

            if (expectedData.count() == 2) {
                QCOMPARE(parsedTextRecord.text(), expectedData.at(0).toString());
                QCOMPARE(parsedTextRecord.locale(), expectedData.at(1).toLocale());
            }
        }

        // Test NDEF NFC URI
        {
            QNdefNfcUriRecord uriRecord(record);
            QNdefNfcUriRecord parsedUriRecord(parsedRecord);

            QCOMPARE(uriRecord.uri(), parsedUriRecord.uri());

            if (expectedData.count() == 1)
                QCOMPARE(parsedUriRecord.uri(), expectedData.at(0).toUrl());
        }
    }
}

QTEST_MAIN(tst_QNdefMessage)

#include "tst_qndefmessage.moc"

