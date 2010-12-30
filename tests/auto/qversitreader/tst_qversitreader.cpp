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

//TESTED_COMPONENT=src/versit

#include "tst_qversitreader.h"
#include "qversitreader.h"
#include "qversitproperty.h"
#ifdef QTM_BUILD_UNITTESTS
#include "qversitreader_p.h"
#include "versitutils_p.h"
#endif
#include <QtTest/QtTest>
#include <QSignalSpy>

// Copied from tst_qcontactmanager.cpp
// Waits until __expr is true and fails if it doesn't happen within 5s.
#ifndef QTRY_VERIFY
#define QTRY_VERIFY(__expr) \
        do { \
        const int __step = 50; \
        const int __timeout = 5000; \
        if (!(__expr)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && !(__expr); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QVERIFY(__expr); \
    } while(0)
#endif

// This says "NOKIA" in Katakana encoded with UTF-8
const QByteArray KATAKANA_NOKIA("\xe3\x83\x8e\xe3\x82\xad\xe3\x82\xa2");

const static QByteArray SAMPLE_GIF_BASE64(QByteArray(
        "R0lGODlhEgASAIAAAAAAAP///yH5BAEAAAEALAAAAAASABIAAAIdjI+py+0G"
        "wEtxUmlPzRDnzYGfN3KBaKGT6rDmGxQAOw=="));

const static QByteArray SAMPLE_GIF(QByteArray::fromBase64(SAMPLE_GIF_BASE64));

Q_DECLARE_METATYPE(QVersitDocument::VersitType);
Q_DECLARE_METATYPE(QVersitProperty);

QTM_USE_NAMESPACE

void tst_QVersitReader::init()
{
    mInputDevice = new QBuffer;
    mInputDevice->open(QBuffer::ReadWrite);
    mReader = new QVersitReader;
#ifdef QT_BUILD_INTERNAL    
    mReaderPrivate = new QVersitReaderPrivate;
#endif
    mSignalCatcher = new SignalCatcher;
    connect(mReader, SIGNAL(stateChanged(QVersitReader::State)),
            mSignalCatcher, SLOT(stateChanged(QVersitReader::State)));
    connect(mReader, SIGNAL(resultsAvailable()),
            mSignalCatcher, SLOT(resultsAvailable()));
    mAsciiCodec = QTextCodec::codecForName("ISO 8859-1");
}

void tst_QVersitReader::cleanup()
{
#ifdef QT_BUILD_INTERNAL
    delete mReaderPrivate;
#endif
    delete mReader;
    delete mInputDevice;
    delete mSignalCatcher;
}

void tst_QVersitReader::testDevice()
{
    // No device
    QVERIFY(mReader->device() == NULL);

    // Device has been set
    mReader->setDevice(mInputDevice);
    QVERIFY(mReader->device() == mInputDevice);

    delete mInputDevice;
    QVERIFY(mReader->device() == NULL);

    mInputDevice = new QBuffer;
    mInputDevice->open(QBuffer::ReadWrite);

    QVERIFY(mReader->device() == NULL);
    mReader->setDevice(mInputDevice);
    QVERIFY(mReader->device() == mInputDevice);
}

void tst_QVersitReader::testNullDevice()
{
    QVersitReader vr;
    QVERIFY(vr.device() == NULL);
    QVERIFY(vr.startReading() == false);
    QVERIFY(vr.error() == QVersitReader::IOError);

    vr.setDevice(NULL);
    QVERIFY(vr.device() == NULL);
    QVERIFY(vr.startReading() == false);
    QVERIFY(vr.error() == QVersitReader::IOError);

    QFile f("does not exist or else");
    vr.setDevice(&f);
    QVERIFY(vr.device() == &f);
    QVERIFY(vr.startReading() == false);
    QVERIFY(vr.error() == QVersitReader::IOError);

}

void tst_QVersitReader::testDefaultCodec()
{
    QVERIFY(mReader->defaultCodec() == 0);
    mReader->setDefaultCodec(QTextCodec::codecForName("UTF-16BE"));
    QVERIFY(mReader->defaultCodec() == QTextCodec::codecForName("UTF-16BE"));
}

void tst_QVersitReader::testValidateUtf8()
{
    QFETCH(QByteArray, bytes);
    QFETCH(bool, isValid);
    QCOMPARE(VersitUtils::isValidUtf8(bytes), isValid);
}

void tst_QVersitReader::testValidateUtf8_data()
{
    // These test cases are taken from
    // http://www.cl.cam.ac.uk/~mgk25/ucs/examples/UTF-8-test.txt
    // See that page for a description of what they test
    QTest::addColumn<QByteArray>("bytes");
    QTest::addColumn<bool>("isValid");
    // The first 18 are marked as "valid" according to the above page
    QTest::newRow("1") << QByteArray("\xce\xba\xe1\xbd\xb9\xcf\x83\xce\xbc\xce\xb5") << true;
    QTest::newRow("2") << QByteArray("\x00") << true;
    QTest::newRow("3") << QByteArray("\xc2\x80") << true;
    QTest::newRow("4") << QByteArray("\xe0\xa0\x80") << true;
    QTest::newRow("5") << QByteArray("\xf0\x90\x80\x80") << true;
    // We treat 5 and 6 byte characters as invalid as per RFC3629
    QTest::newRow("6") << QByteArray("\xf8\x88\x80\x80\x80") << false;
    QTest::newRow("7") << QByteArray("\xfc\x84\x80\x80\x80\x80") << false;
    QTest::newRow("8") << QByteArray("\x7f") << true;
    QTest::newRow("9") << QByteArray("\xdf\xbf") << true;
    QTest::newRow("10") << QByteArray("\xef\xbf\xbd") << true;
    QTest::newRow("11") << QByteArray("\xf4\x8f\xbf\xbf") << true;
    // We treat 5 and 6 byte characters as invalid as per RFC3629
    QTest::newRow("12") << QByteArray("\xfb\xbf\xbf\xbf\xbf") << false;
    QTest::newRow("13") << QByteArray("\xfd\xbf\xbf\xbf\xbf\xbf") << false;
    QTest::newRow("14") << QByteArray("\xed\x9f\xbf") << true;
    QTest::newRow("15") << QByteArray("\xee\x80\x80") << true;
    QTest::newRow("16") << QByteArray("\xef\xbf\xbd") << true;
    QTest::newRow("17") << QByteArray("\xf4\x8f\xbf\xbf") << true;
    QTest::newRow("18") << QByteArray("\xf4\x90\x80\x80") << false; // outside the range

    // The rest are marked as "invalid" according to the above page
    QTest::newRow("19") << QByteArray("\x80") << false;
    QTest::newRow("20") << QByteArray("\xbf") << false;
    QTest::newRow("21") << QByteArray("\x80\xbf") << false;
    QTest::newRow("22") << QByteArray("\x80\xbf\x80") << false;
    QTest::newRow("23") << QByteArray("\x80\xbf\x80\xbf") << false;
    QTest::newRow("24") << QByteArray("\x80\xbf\x80\xbf\x80") << false;
    QTest::newRow("25") << QByteArray("\x80\xbf\x80\xbf\x80\xbf") << false;
    QTest::newRow("26") << QByteArray("\x80\xbf\x80\xbf\x80\xbf\x80") << false;
    QTest::newRow("27") << QByteArray("\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d"
            "\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1"
            "\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5"
            "\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf") << false;
    QTest::newRow("28") << QByteArray("\xc0\x20\xc1\x20\xc2\x20\xc3\x20\xc4\x20\xc5\x20\xc6\x20"
            "\xc7\x20\xc8\x20\xc9\x20\xca\x20\xcb\x20\xcc\x20\xcd\x20\xce\x20\xcf\x20\xd0\x20"
            "\xd1\x20\xd2\x20\xd3\x20\xd4\x20\xd5\x20\xd6\x20\xd7\x20\xd8\x20\xd9\x20\xda\x20"
            "\xdb\x20\xdc\x20\xdd\x20\xde\x20\xdf\x20") << false;
    QTest::newRow("29") << QByteArray("\xe0\x20\xe1\x20\xe2\x20\xe3\x20\xe4\x20\xe5\x20\xe6\x20"
            "\xe7\x20\xe8\x20\xe9\x20\xea\x20\xeb\x20\xec\x20\xed\x20\xee\x20\xef\x20") << false;
    QTest::newRow("30") << QByteArray("\xf0\x20\xf1\x20\xf2\x20\xf3\x20\xf4\x20\xf5\x20\xf6\x20"
            "\xf7\x20") << false;
    QTest::newRow("31") << QByteArray("\xf8\x20\xf9\x20\xfa\x20\xfb\x20") << false;
    QTest::newRow("32") << QByteArray("\xfc\x20\xfd\x20") << false;
    QTest::newRow("33") << QByteArray("\xc0") << false;
    QTest::newRow("34") << QByteArray("\xe0\x80") << false;
    QTest::newRow("35") << QByteArray("\xf0\x80\x80") << false;
    QTest::newRow("36") << QByteArray("\xf8\x80\x80\x80") << false;
    QTest::newRow("37") << QByteArray("\xfc\x80\x80\x80\x80") << false;
    QTest::newRow("38") << QByteArray("\xdf") << false;
    QTest::newRow("39") << QByteArray("\xef\xbf") << false;
    QTest::newRow("40") << QByteArray("\xf7\xbf\xbf") << false;
    QTest::newRow("41") << QByteArray("\xfb\xbf\xbf\xbf") << false;
    QTest::newRow("42") << QByteArray("\xfd\xbf\xbf\xbf\xbf") << false;
    QTest::newRow("43") << QByteArray("\xc0\xe0\x80\xf0\x80\x80\xf8\x80\x80\x80\xfc\x80\x80\x80"
            "\x80\xdf\xef\xbf\xf7\xbf\xbf\xfb\xbf\xbf\xbf\xfd\xbf\xbf\xbf\xbf") << false;
    QTest::newRow("44") << QByteArray("\xfe") << false;
    QTest::newRow("45") << QByteArray("\xff") << false;
    QTest::newRow("46") << QByteArray("\xfe\xfe\xff\xff") << false;
    QTest::newRow("47") << QByteArray("\xc0\xaf") << false;
    QTest::newRow("48") << QByteArray("\xe0\x80\xaf") << false;
    QTest::newRow("49") << QByteArray("\xf0\x80\x80\xaf") << false;
    QTest::newRow("50") << QByteArray("\xf8\x80\x80\x80\xaf") << false;
    QTest::newRow("51") << QByteArray("\xfc\x80\x80\x80\x80\xaf") << false;
    QTest::newRow("52") << QByteArray("\xc1\xbf") << false;
    QTest::newRow("53") << QByteArray("\xe0\x9f\xbf") << false;
    QTest::newRow("54") << QByteArray("\xf0\x8f\xbf\xbf") << false;
    QTest::newRow("55") << QByteArray("\xf8\x87\xbf\xbf\xbf") << false;
    QTest::newRow("56") << QByteArray("\xfc\x83\xbf\xbf\xbf\xbf") << false;
    QTest::newRow("57") << QByteArray("\xc0\x80") << false;
    QTest::newRow("58") << QByteArray("\xe0\x80\x80") << false;
    QTest::newRow("59") << QByteArray("\xf0\x80\x80\x80") << false;
    QTest::newRow("60") << QByteArray("\xf8\x80\x80\x80\x80") << false;
    QTest::newRow("61") << QByteArray("\xfc\x80\x80\x80\x80\x80") << false;
    QTest::newRow("62") << QByteArray("\xed\xa0\x80") << false;
    QTest::newRow("63") << QByteArray("\xed\xad\xbf") << false;
    QTest::newRow("64") << QByteArray("\xed\xae\x80") << false;
    QTest::newRow("65") << QByteArray("\xed\xaf\xbf") << false;
    QTest::newRow("66") << QByteArray("\xed\xb0\x80") << false;
    QTest::newRow("67") << QByteArray("\xed\xbe\x80") << false;
    QTest::newRow("68") << QByteArray("\xed\xbf\xbf") << false;
    QTest::newRow("69") << QByteArray("\xed\xa0\x80\xed\xb0\x80") << false;
    QTest::newRow("70") << QByteArray("\xed\xa0\x80\xed\xbf\xbf") << false;
    QTest::newRow("71") << QByteArray("\xed\xad\xbf\xed\xb0\x80") << false;
    QTest::newRow("72") << QByteArray("\xed\xad\xbf\xed\xbf\xbf") << false;
    QTest::newRow("73") << QByteArray("\xed\xae\x80\xed\xb0\x80") << false;
    QTest::newRow("74") << QByteArray("\xed\xae\x80\xed\xbf\xbf") << false;
    QTest::newRow("75") << QByteArray("\xed\xaf\xbf\xed\xb0\x80") << false;
    QTest::newRow("76") << QByteArray("\xed\xaf\xbf\xed\xbf\xbf") << false;
    QTest::newRow("77") << QByteArray("\xef\xbf\xbe") << false;
    QTest::newRow("78") << QByteArray("\xef\xbf\xbf") << false;

    // My own tests
    // 0x110000 is the first one outside the Unicode range
    QTest::newRow("79") << QByteArray("\xf4\x90\x80\x80") << false;
    // a 3 byte sequence followed by a single byte
    QTest::newRow("80") << QByteArray("\xef\xbf\xbd\x20") << true;
    // a 4 byte sequence followed by a single byte
    QTest::newRow("81") << QByteArray("\xf4\x8f\xbf\xbf\x20") << true;
}

void tst_QVersitReader::testDetectCodec()
{
    QFETCH(QByteArray, bytes);
    QFETCH(QString, expectedFnValue);

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("ISO 8859-1"));
    mInputDevice->close();
    mInputDevice->setData(bytes);
    mInputDevice->open(QBuffer::ReadOnly);
    mInputDevice->seek(0);
    mReader->setDevice(mInputDevice);
    QVERIFY(mReader->defaultCodec() == 0);
    QVERIFY2(mReader->startReading(), QString::number(mReader->error()).toAscii().data());
    QVERIFY2(mReader->waitForFinished(), QString::number(mReader->error()).toAscii().data());
    QCOMPARE(mReader->state(), QVersitReader::FinishedState);
    QCOMPARE(mReader->error(), QVersitReader::NoError);
    QVERIFY(mReader->defaultCodec() == 0); // shouldn't change
    QList<QVersitDocument> results = mReader->results();
    QCOMPARE(results.count(),1);
    QVersitDocument document = results.first();
    QCOMPARE(document.properties().size(), 1);
    QVersitProperty property = document.properties().first();
    QCOMPARE(property.name(), QLatin1String("FN"));
    QCOMPARE(property.value(), expectedFnValue);
}

void tst_QVersitReader::testDetectCodec_data()
{
    QTest::addColumn<QByteArray>("bytes");
    QTest::addColumn<QString>("expectedFnValue");

    const QString& documentString =
        QLatin1String("BEGIN:VCARD\r\nVERSION:2.1\r\nFN:John\r\nEND:VCARD\r\n");
    {
        const QByteArray& document =
            "\xef\xbb\xbf" + documentString.toUtf8();
        QTest::newRow("UTF-8 with BOM") << document << QString::fromAscii("John");
    }
    {
        const QByteArray& document =
            QTextCodec::codecForName("UTF-16BE")->fromUnicode(documentString);
        QTest::newRow("UTF-16BE with BOM") << document << QString::fromAscii("John");
    }
    {
        const QByteArray& document =
            QTextCodec::codecForName("UTF-16LE")->fromUnicode(documentString);
        QTest::newRow("UTF-16LE with BOM") << document << QString::fromAscii("John");
    }
    {
        const QByteArray& document =
            VersitUtils::encode(documentString.toAscii(), QTextCodec::codecForName("UTF-16BE"));
        QTest::newRow("UTF-16BE without BOM") << document << QString::fromAscii("John");
    }
    {
        const QByteArray& document =
            VersitUtils::encode(documentString.toAscii(), QTextCodec::codecForName("UTF-16LE"));
        QTest::newRow("UTF-16LE without BOM") << document << QString::fromAscii("John");
    }
    {
        const QByteArray& document =
            QTextCodec::codecForName("UTF-32BE")->fromUnicode(documentString);
        QTest::newRow("UTF-32BE with BOM") << document << QString::fromAscii("John");
    }
    {
        const QByteArray& document =
            QTextCodec::codecForName("UTF-32LE")->fromUnicode(documentString);
        QTest::newRow("UTF-32LE with BOM") << document << QString::fromAscii("John");
    }
    {
        const QByteArray& document = documentString.toUtf8();
        QTest::newRow("Plain ASCII") << document << QString::fromAscii("John");
    }
    {
        const QByteArray& document = "BEGIN:VCARD\r\nVERSION:2.1\r\nFN:"
                                    + KATAKANA_NOKIA
                                    + "\r\nEND:VCARD\r\n";
        QTest::newRow("Non-ASCII UTF-8") << document << QString::fromUtf8(KATAKANA_NOKIA);
    }
    {
        // some Scandinavian characters, note that "\xe4\xe4" is invalid UTF-8, as is "\xf6n"
        const QByteArray& document =
            "BEGIN:VCARD\r\nVERSION:2.1\r\n"
            "FN:P\xe4\xe4kk\xf6nen\r\n"
            "END:VCARD\r\n";
        QTest::newRow("Non-ASCII Latin-1") << document
            << QString::fromLatin1("P\xe4\xe4kk\xf6nen");
    }
    {
        // as above, but quoted-printable
        const QByteArray& document =
            "BEGIN:VCARD\r\nVERSION:2.1\r\n"
            "FN;ENCODING=QUOTED-PRINTABLE:P=E4=E4kk=F6nen\r\n"
            "END:VCARD\r\n";
        QTest::newRow("Non-ASCII Latin-1 QP") << document
            << QString::fromLatin1("P\xe4\xe4kk\xf6nen");
    }
}

void tst_QVersitReader::testReading()
{
    // No I/O device set
    QVERIFY(!mReader->startReading());
    QCOMPARE(mReader->error(), QVersitReader::IOError);

    // Device set, no data
    mReader->setDevice(mInputDevice);
    mInputDevice->open(QBuffer::ReadOnly);
    QVERIFY2(mReader->startReading(), QString::number(mReader->error()).toAscii().data());
    QVERIFY2(mReader->waitForFinished(), QString::number(mReader->error()).toAscii().data());
    QList<QVersitDocument> results(mReader->results());
    QCOMPARE(mReader->state(), QVersitReader::FinishedState);
    QCOMPARE(mReader->error(), QVersitReader::NoError);
    QCOMPARE(results.count(),0);

    // Device set, one document
    const QByteArray& oneDocument =
        "BEGIN:VCARD\r\nVERSION:2.1\r\nFN:John\r\nEND:VCARD\r\n";
    mInputDevice->close();
    mInputDevice->setData(oneDocument);
    mInputDevice->open(QBuffer::ReadOnly);
    mInputDevice->seek(0);
    QVERIFY2(mReader->startReading(), QString::number(mReader->error()).toAscii().data());
    QVERIFY2(mReader->waitForFinished(), QString::number(mReader->error()).toAscii().data());
    results = mReader->results();
    QCOMPARE(mReader->state(), QVersitReader::FinishedState);
    QCOMPARE(mReader->error(), QVersitReader::NoError);
    QCOMPARE(results.count(),1);

    // vCard 4.0
    const QByteArray& vcard40 =
        "BEGIN:VCARD\r\nVERSION:4.0\r\nFN:John\r\nEND:VCARD\r\n";
    mInputDevice->close();
    mInputDevice->setData(vcard40);
    mInputDevice->open(QBuffer::ReadOnly);
    mInputDevice->seek(0);
    QVERIFY2(mReader->startReading(), QString::number(mReader->error()).toAscii().data());
    QVERIFY2(mReader->waitForFinished(), QString::number(mReader->error()).toAscii().data());
    results = mReader->results();
    QCOMPARE(mReader->state(), QVersitReader::FinishedState);
    QCOMPARE(mReader->error(), QVersitReader::NoError);
    QCOMPARE(results.count(),1);

    // Wide charset with no byte-order mark
    QTextCodec* codec = QTextCodec::codecForName("UTF-16BE");
    QTextCodec::ConverterState converterState(QTextCodec::IgnoreHeader);
    QString document = QLatin1String("BEGIN:VCARD\r\nVERSION:2.1\r\nFN:John\r\nEND:VCARD\r\n");
    const QByteArray& wideDocument =
        codec->fromUnicode(document.data(), document.length(), &converterState);
    mInputDevice->close();
    mInputDevice->setData(wideDocument);
    mInputDevice->open(QBuffer::ReadOnly);
    mInputDevice->seek(0);
    mReader->setDefaultCodec(codec);
    QVERIFY2(mReader->startReading(), QString::number(mReader->error()).toAscii().data());
    QVERIFY2(mReader->waitForFinished(), QString::number(mReader->error()).toAscii().data());
    results = mReader->results();
    QCOMPARE(mReader->state(), QVersitReader::FinishedState);
    QCOMPARE(mReader->error(), QVersitReader::NoError);
    QCOMPARE(mReader->results().count(),1);
    mReader->setDefaultCodec(NULL);

    // Two documents
    const QByteArray& twoDocuments =
        " \r\n BEGIN:VCARD\r\nFN:Jenny\r\nEND:VCARD\r\nBEGIN:VCARD\r\nFN:Jake\r\nEND:VCARD\r\n";
    mInputDevice->close();
    mInputDevice->setData(twoDocuments);
    mInputDevice->open(QBuffer::ReadOnly);
    mInputDevice->seek(0);
    QVERIFY2(mReader->startReading(), QString::number(mReader->error()).toAscii().data());
    QVERIFY2(mReader->waitForFinished(), QString::number(mReader->error()).toAscii().data());
    results = mReader->results();
    QCOMPARE(mReader->state(), QVersitReader::FinishedState);
    QCOMPARE(mReader->error(), QVersitReader::NoError);
    QCOMPARE(results.count(),2);

    // Valid documents and a grouped document between them
    const QByteArray& validDocumentsAndGroupedDocument =
"BEGIN:VCARD\r\nFN:Jenny\r\nEND:VCARD\r\n"
"BEGIN:VCARD\r\nX-GROUPING:pub gang\r\nBEGIN:VCARD\r\nFN:Jeremy\r\nEND:VCARD\r\nBEGIN:VCARD\r\nFN:Jeffery\r\nEND:VCARD\r\nEND:VCARD\r\n"
"BEGIN:VCARD\r\nFN:Jake\r\nEND:VCARD\r\n"
"BEGIN:VCARD\r\nFN:James\r\nEND:VCARD\r\n"
"BEGIN:VCARD\r\nFN:Jane\r\nEND:VCARD\r\n";
    mInputDevice->close();
    mInputDevice->setData(validDocumentsAndGroupedDocument);
    mInputDevice->open(QBuffer::ReadWrite);
    mInputDevice->seek(0);
    QVERIFY2(mReader->startReading(), QString::number(mReader->error()).toAscii().data());
    QVERIFY2(mReader->waitForFinished(), QString::number(mReader->error()).toAscii().data());
    results = mReader->results();
    QCOMPARE(mReader->state(), QVersitReader::FinishedState);
    QCOMPARE(mReader->error(), QVersitReader::NoError);
    QCOMPARE(results.count(),5);

    qApp->processEvents(); // clean up before we start sniffing signals

    // Asynchronous reading
    mInputDevice->close();
    mInputDevice->setData(twoDocuments);
    mInputDevice->open(QBuffer::ReadWrite);
    mInputDevice->seek(0);
    mSignalCatcher->mStateChanges.clear();
    mSignalCatcher->mResultsCount = 0;
    QVERIFY2(mReader->startReading(), QString::number(mReader->error()).toAscii().data());
    QTRY_VERIFY(mSignalCatcher->mStateChanges.count() >= 2);
    QCOMPARE(mSignalCatcher->mStateChanges.at(0), QVersitReader::ActiveState);
    QCOMPARE(mSignalCatcher->mStateChanges.at(1), QVersitReader::FinishedState);
    QVERIFY(mSignalCatcher->mResultsCount >= 2);
    QCOMPARE(mReader->results().size(), 2);
    QCOMPARE(mReader->error(), QVersitReader::NoError);

    // Cancelling
    mInputDevice->close();
    mInputDevice->setData(twoDocuments);
    mInputDevice->open(QBuffer::ReadOnly);
    mInputDevice->seek(0);
    mSignalCatcher->mStateChanges.clear();
    mSignalCatcher->mResultsCount = 0;
    QVERIFY2(mReader->startReading(), QString::number(mReader->error()).toAscii().data());
    mReader->cancel();
    mReader->waitForFinished();
    QTRY_VERIFY(mSignalCatcher->mStateChanges.count() >= 2);
    QCOMPARE(mSignalCatcher->mStateChanges.at(0), QVersitReader::ActiveState);
    QVersitReader::State state(mSignalCatcher->mStateChanges.at(1));
    // It's possible that it finishes before it cancels.
    QVERIFY(state == QVersitReader::CanceledState
            || state == QVersitReader::FinishedState);
}

void tst_QVersitReader::testResult()
{
    QCOMPARE(mReader->results().count(),0);
}

void tst_QVersitReader::testParseNextVersitProperty()
{
#ifndef QT_BUILD_INTERNAL
    QSKIP("Testing private API", SkipSingle);
#else
    QFETCH(QVersitDocument::VersitType, documentType);
    QFETCH(QByteArray, input);
    QFETCH(QVersitProperty, expectedProperty);

    QBuffer buffer(&input);
    buffer.open(QIODevice::ReadOnly);
    LineReader lineReader(&buffer, mAsciiCodec);
    QVersitProperty property = mReaderPrivate->parseNextVersitProperty(documentType, &lineReader);
    if (property != expectedProperty) {
        // compare each part of the property separately for easier debugging
        QCOMPARE(property.groups(), expectedProperty.groups());
        QCOMPARE(property.name(), expectedProperty.name());
        QCOMPARE(property.valueType(), expectedProperty.valueType());

        // QVariant doesn't support == on QVersitDocuments - do it manually
        if (property.variantValue().userType() == qMetaTypeId<QVersitDocument>()) {
            QVERIFY(expectedProperty.variantValue().userType() == qMetaTypeId<QVersitDocument>());
            QCOMPARE(property.value<QVersitDocument>(), expectedProperty.value<QVersitDocument>());
        }
        else
            QCOMPARE(property.variantValue(), expectedProperty.variantValue());

        // Don't check parameters because the reader can add random parameters of its own (like CHARSET)
        // QCOMPARE(property.parameters(), expectedProperty.parameters());
    }
#endif
}

void tst_QVersitReader::testParseNextVersitProperty_data()
{
#ifdef QT_BUILD_INTERNAL
    QTest::addColumn<QVersitDocument::VersitType>("documentType");
    QTest::addColumn<QByteArray>("input");
    QTest::addColumn<QVersitProperty>("expectedProperty");

    {
        QVersitProperty expectedProperty;
        expectedProperty.setName(QLatin1String("BEGIN"));
        expectedProperty.setValue(QLatin1String("vcard"));
        QTest::newRow("begin")
            << QVersitDocument::VCard21Type
            << QByteArray("Begin:vcard\r\n")
            << expectedProperty;
    }

    {
        QVersitProperty expectedProperty;
        expectedProperty.setName(QLatin1String("VERSION"));
        expectedProperty.setValue(QLatin1String("2.1"));
        expectedProperty.setValueType(QVersitProperty::PlainType);
        QTest::newRow("version")
            << QVersitDocument::VCard21Type
            << QByteArray("VERSION:2.1\r\n")
            << expectedProperty;
    }

    {
        QVersitProperty expectedProperty;
        expectedProperty.setName(QLatin1String("FN"));
        expectedProperty.setValue(QLatin1String("John"));
        expectedProperty.setValueType(QVersitProperty::PlainType);
        QTest::newRow("fn")
            << QVersitDocument::VCard21Type
            << QByteArray("FN:John\r\n")
            << expectedProperty;
    }

    {
        // "NOTE:\;\,\:\\"
        QVersitProperty expectedProperty;
        expectedProperty.setName(QLatin1String("NOTE"));
        expectedProperty.setValue(QLatin1String("\\;\\,\\:\\\\"));
        expectedProperty.setValueType(QVersitProperty::PlainType);
        QTest::newRow("vcard21 note")
            << QVersitDocument::VCard21Type
            << QByteArray("NOTE:\\;\\,\\:\\\\\r\n")
            << expectedProperty;

        expectedProperty.setValue(QLatin1String(";,:\\"));
        QTest::newRow("vcard30 note")
            << QVersitDocument::VCard30Type
            << QByteArray("NOTE:\\;\\,\\:\\\\\r\n")
            << expectedProperty;
    }

    {
        // "N:foo\;bar;foo\,bar;foo\:bar;foo\\bar;foo\\\;bar"
        QVersitProperty expectedProperty;
        expectedProperty.setName(QLatin1String("N"));
        QStringList components;
        components << QLatin1String("foo;bar")
            << QLatin1String("foo\\,bar")
            << QLatin1String("foo\\:bar")
            << QLatin1String("foo\\\\bar")
            << QLatin1String("foo\\\\;bar");
        expectedProperty.setValue(components);
        expectedProperty.setValueType(QVersitProperty::CompoundType);
        QTest::newRow("vcard21 n")
            << QVersitDocument::VCard21Type
            << QByteArray("N:foo\\;bar;foo\\,bar;foo\\:bar;foo\\\\bar;foo\\\\\\;bar\r\n")
            << expectedProperty;

        components.clear();
        components << QLatin1String("foo;bar")
            << QLatin1String("foo,bar")
            << QLatin1String("foo:bar")
            << QLatin1String("foo\\bar")
            << QLatin1String("foo\\;bar");
        expectedProperty.setValue(components);
        QTest::newRow("vcard30 n")
            << QVersitDocument::VCard30Type
            << QByteArray("N:foo\\;bar;foo\\,bar;foo\\:bar;foo\\\\bar;foo\\\\\\;bar\r\n")
            << expectedProperty;
    }

    {
        QVersitProperty expectedProperty;
        expectedProperty.setName(QLatin1String("ADR"));
        expectedProperty.setValue(QStringList(QString()));
        expectedProperty.setValueType(QVersitProperty::CompoundType);
        QTest::newRow("empty structured")
            << QVersitDocument::VCard21Type
            << QByteArray("ADR:\r\n")
            << expectedProperty;
    }

    {
        QVersitProperty expectedProperty;
        expectedProperty.setName(QLatin1String("X-CHILDREN"));
        expectedProperty.setValue(QStringList() << QLatin1String("Child1") << QLatin1String("Child2"));
        expectedProperty.setValueType(QVersitProperty::ListType);
        QTest::newRow("children")
            << QVersitDocument::VCard21Type
            << QByteArray("X-CHILDREN:Child1,Child2\r\n")
            << expectedProperty;
    }

    {
        // "NICKNAME:foo\;bar,foo\,bar,foo\:bar,foo\\bar,foo\\\,bar"
        QVersitProperty expectedProperty;
        expectedProperty.setName(QLatin1String("NICKNAME"));
        QStringList components;
        components << QLatin1String("foo\\;bar")
            << QLatin1String("foo,bar")
            << QLatin1String("foo\\:bar")
            << QLatin1String("foo\\\\bar")
            << QLatin1String("foo\\\\,bar");
        expectedProperty.setValue(components);
        expectedProperty.setValueType(QVersitProperty::ListType);
        QTest::newRow("vcard21 nickname")
            << QVersitDocument::VCard21Type
            << QByteArray("NICKNAME:foo\\;bar,foo\\,bar,foo\\:bar,foo\\\\bar,foo\\\\\\,bar\r\n")
            << expectedProperty;

        components.clear();
        components << QLatin1String("foo;bar")
            << QLatin1String("foo,bar")
            << QLatin1String("foo:bar")
            << QLatin1String("foo\\bar")
            << QLatin1String("foo\\,bar");
        expectedProperty.setValue(components);
        QTest::newRow("vcard30 nickname")
            << QVersitDocument::VCard30Type
            << QByteArray("NICKNAME:foo\\;bar,foo\\,bar,foo\\:bar,foo\\\\bar,foo\\\\\\,bar\r\n")
            << expectedProperty;
    }

    {
        // "CATEGORIES:foo\;bar,foo\,bar,foo\:bar,foo\\bar,foo\\\,bar"
        QVersitProperty expectedProperty;
        expectedProperty.setName(QLatin1String("CATEGORIES"));
        QStringList components;
        components << QLatin1String("foo\\;bar")
            << QLatin1String("foo,bar")
            << QLatin1String("foo\\:bar")
            << QLatin1String("foo\\\\bar")
            << QLatin1String("foo\\\\,bar");
        expectedProperty.setValue(components);
        expectedProperty.setValueType(QVersitProperty::ListType);
        QTest::newRow("vcard21 categories")
            << QVersitDocument::VCard21Type
            << QByteArray("CATEGORIES:foo\\;bar,foo\\,bar,foo\\:bar,foo\\\\bar,foo\\\\\\,bar\r\n")
            << expectedProperty;

        components.clear();
        components << QLatin1String("foo;bar")
            << QLatin1String("foo,bar")
            << QLatin1String("foo:bar")
            << QLatin1String("foo\\bar")
            << QLatin1String("foo\\,bar");
        expectedProperty.setValue(components);
        QTest::newRow("vcard30 categories")
            << QVersitDocument::VCard30Type
            << QByteArray("CATEGORIES:foo\\;bar,foo\\,bar,foo\\:bar,foo\\\\bar,foo\\\\\\,bar\r\n")
            << expectedProperty;

        // "CATEGORIES:foobar\\,foobar\\\\,foo\\\\\,bar"
        components.clear();
        components << QLatin1String("foobar\\")
            << QLatin1String("foobar\\\\")
            << QLatin1String("foo\\\\,bar");
        expectedProperty.setValue(components);
        QTest::newRow("vcard30 unescaping")
            << QVersitDocument::VCard30Type
            << QByteArray("CATEGORIES:foobar\\\\,foobar\\\\\\\\,foo\\\\\\\\\\,bar")
            << expectedProperty;
    }

    {
        QVersitProperty expectedProperty;
        expectedProperty.setName(QLatin1String("ORG"));
        expectedProperty.setValue(QString::fromUtf8(KATAKANA_NOKIA));
        expectedProperty.setValueType(QVersitProperty::CompoundType);
        QTest::newRow("org utf8")
            << QVersitDocument::VCard21Type
            << QByteArray("ORG;CHARSET=UTF-8:" + KATAKANA_NOKIA + "\r\n")
            << expectedProperty;
    }

    {
        QVersitProperty expectedProperty;
        expectedProperty.setName(QLatin1String("ORG"));
        expectedProperty.setValue(QString::fromUtf8(KATAKANA_NOKIA));
        expectedProperty.setValueType(QVersitProperty::CompoundType);
        QTest::newRow("vcard21 org utf8 qp")
            << QVersitDocument::VCard21Type
            << QByteArray("ORG;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=E3=83=8E=E3=82=AD=E3=82=A2\r\n")
            << expectedProperty;
        QTest::newRow("vcard30 org utf8 qp")
            << QVersitDocument::VCard30Type
            << QByteArray("ORG;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=E3=83=8E=E3=82=AD=E3=82=A2\r\n")
            << expectedProperty;
    }

    {
        QVersitProperty expectedProperty;
        expectedProperty.setName(QLatin1String("PHOTO"));
        expectedProperty.setValue(SAMPLE_GIF);
        expectedProperty.setValueType(QVersitProperty::BinaryType);
        QTest::newRow("vcard21 photo1")
            << QVersitDocument::VCard21Type
            << QByteArray("PHOTO;ENCODING=BASE64:") + SAMPLE_GIF_BASE64 + QByteArray("\r\n\r\n")
            << expectedProperty;

        QTest::newRow("vcard30 photo1")
            << QVersitDocument::VCard30Type
            << QByteArray("PHOTO;ENCODING=B:") + SAMPLE_GIF_BASE64 + QByteArray("\r\n\r\n")
            << expectedProperty;

    // Again, but without the explicit "ENCODING" parameter
        QTest::newRow("vcard21 photo2")
            << QVersitDocument::VCard21Type
            << QByteArray("PHOTO;BASE64:") + SAMPLE_GIF_BASE64 + QByteArray("\r\n\r\n")
            << expectedProperty;

        QTest::newRow("vcard30 photo2")
            << QVersitDocument::VCard30Type
            << QByteArray("PHOTO;B:") + SAMPLE_GIF_BASE64 + QByteArray("\r\n\r\n")
            << expectedProperty;
    }

    {
        QVersitProperty expectedProperty;
        expectedProperty.setGroups(QStringList() << QLatin1String("HOME") << QLatin1String("Springfield"));
        expectedProperty.setName(QLatin1String("EMAIL"));
        expectedProperty.setValue(QLatin1String("john.citizen@example.com"));
        expectedProperty.setValueType(QVersitProperty::PlainType);
        QTest::newRow("email qp")
            << QVersitDocument::VCard21Type
            << QByteArray("HOME.Springfield.EMAIL;Encoding=Quoted-Printable:john.citizen=40exam=\r\nple.com\r\n")
            << expectedProperty;
    }

    {
        QVersitDocument subDocument;
        subDocument.setComponentType(QLatin1String("VCARD"));
        subDocument.setType(QVersitDocument::VCard21Type);
        QVersitProperty subProperty;
        subProperty.setName(QLatin1String("FN"));
        subProperty.setValue(QLatin1String("Jenny"));
        subDocument.addProperty(subProperty);

        QVersitProperty expectedProperty;
        expectedProperty.setName(QLatin1String("AGENT"));
        expectedProperty.setValue(QVariant::fromValue(subDocument));
        expectedProperty.setValueType(QVersitProperty::VersitDocumentType);
        QTest::newRow("agent")
            << QVersitDocument::VCard21Type
            << QByteArray("AGENT:\r\nBEGIN:VCARD\r\nFN:Jenny\r\nEND:VCARD\r\n\r\n")
            << expectedProperty;
    }
#endif
}

void tst_QVersitReader::testParseVersitDocument()
{
    QFETCH(QByteArray, vCard);
    QFETCH(bool, expectedSuccess);
    QFETCH(QVersitDocument, expectedDocument);

    QBuffer buffer(&vCard);
    buffer.open(QIODevice::ReadOnly);
    LineReader lineReader(&buffer, QTextCodec::codecForName("UTF-8"));

    mReader->setDevice(&buffer);
    QVERIFY2(mReader->startReading(), QString::number(mReader->error()).toAscii().data());
    QVERIFY2(mReader->waitForFinished(), QString::number(mReader->error()).toAscii().data());
    QCOMPARE(mReader->error(), expectedSuccess ? QVersitReader::NoError : QVersitReader::ParseError);
    if (expectedSuccess) {
        QList<QVersitDocument> documents = mReader->results();
        QCOMPARE(documents.size(), 1);
        QVersitDocument document = documents.at(0);
        if (document != expectedDocument) {
            qDebug() << "Expected: " << expectedDocument;
            qDebug() << "Actual: " << document;
            QCOMPARE(document, expectedDocument);
        }
    }
}

void tst_QVersitReader::testParseVersitDocument_data()
{
    QTest::addColumn<QByteArray>("vCard");
    QTest::addColumn<bool>("expectedSuccess");
    QTest::addColumn<QVersitDocument>("expectedDocument");

    {
    QVersitDocument expected(QVersitDocument::VCard21Type);
    expected.setComponentType(QLatin1String("VCARD"));
    QVersitProperty property;
    property.setName(QLatin1String("FN"));
    property.setValue(QLatin1String("John"));
    expected.addProperty(property);
    QTest::newRow("Basic vCard 2.1")
            << QByteArray(
                    "BEGIN:VCARD\r\n"
                    "VERSION:2.1\r\n"
                    "FN:John\r\n"
                    "END:VCARD\r\n")
            << true
            << expected;
    }

    {
    QVersitDocument expected(QVersitDocument::VCard21Type);
    expected.setComponentType(QLatin1String("VCARD"));
    QVersitProperty property;
    property.setName(QLatin1String("FN"));
    property.setValue(QLatin1String("John"));
    expected.addProperty(property);
    QVersitDocument agent(QVersitDocument::VCard21Type);
    agent.setComponentType(QLatin1String("VCARD"));
    property.setValue(QLatin1String("Jenny"));
    agent.addProperty(property);
    property.clear();
    property.setName(QLatin1String("AGENT"));
    property.setValue(QVariant::fromValue(agent));
    property.setValueType(QVersitProperty::VersitDocumentType);
    expected.addProperty(property);
    property.clear();
    property.setName(QLatin1String("EMAIL"));
    property.setValue(QLatin1String("john.citizen@example.com"));
    expected.addProperty(property);
    QTest::newRow("vCard 2.1 with Agent")
            << QByteArray(
                    "BEGIN:VCARD\r\n"
                    "VERSION:2.1\r\n"
                    "FN:John\r\n"
                    "AGENT:BEGIN:VCARD\r\n"
                    "FN:Jenny\r\n"
                    "END:VCARD\r\n"
                    "\r\n"
                    "EMAIL;ENCODING=QUOTED-PRINTABLE:john.citizen=40exam=\r\nple.com\r\n"
                    "END:VCARD\r\n")
            << true
            << expected;
    }

    {
    QVersitDocument expected(QVersitDocument::VCard30Type);
    expected.setComponentType(QLatin1String("VCARD"));
    QVersitProperty property;
    property.setName(QLatin1String("FN"));
    property.setValue(QLatin1String("John"));
    expected.addProperty(property);
    QVersitDocument agent(QVersitDocument::VCard30Type);
    agent.setComponentType(QLatin1String("VCARD"));
    property.setValue(QLatin1String("Jenny"));
    agent.addProperty(property);
    property.clear();
    property.setName(QLatin1String("AGENT"));
    property.setValue(QVariant::fromValue(agent));
    property.setValueType(QVersitProperty::VersitDocumentType);
    expected.addProperty(property);
    property.clear();
    property.setName(QLatin1String("EMAIL"));
    property.setValue(QLatin1String("john.citizen@example.com"));
    expected.addProperty(property);
    QTest::newRow("vCard 3.0 with Agent")
            << QByteArray(
                    "BEGIN:VCARD\r\n"
                    "VERSION:3.0\r\n"
                    "FN:John\r\n"
                    "AGENT:BEGIN\\:VCARD\\nFN\\:Jenny\\nEND\\:VCARD\\n\r\n"
                    "EMAIL:john.citizen@example.com\r\n"
                    "END:VCARD\r\n")
            << true
            << expected;
    }

    QTest::newRow("No BEGIN found")
            << QByteArray(
                    "VCARD\r\n"
                    "VERSION:2.1\r\n"
                    "FN:Nobody\r\n"
                    "END:VCARD\r\n")
            << false
            << QVersitDocument();

    QTest::newRow("Wrong card type")
            << QByteArray(
                    "BEGIN:VCAL\r\n"
                    "END:VCAL\r\n")
            << false
            << QVersitDocument();

    QTest::newRow("Wrong version")
            << QByteArray(
                    "BEGIN:VCARD\r\n"
                    "VERSION:1234\r\n"
                    "FN:Nobody\r\n"
                    "END:VCARD\r\n")
            << false
            << QVersitDocument();

    {
    QVersitDocument expected(QVersitDocument::VCard21Type);
    expected.setComponentType(QLatin1String("VCARD"));
    QVersitProperty property;
    property.setName(QLatin1String("FN"));
    property.setValue(QLatin1String("Nobody"));
    expected.addProperty(property);
    QTest::newRow("No trailing crlf")
            << QByteArray(
                    "BEGIN:VCARD\r\n"
                    "VERSION:2.1\r\n"
                    "FN:Nobody\r\n"
                    "END:VCARD")
            << true
            << expected;
    }

    QTest::newRow("No end")
            << QByteArray(
                    "BEGIN:VCARD\r\n"
                    "VERSION:2.1\r\n"
                    "FN:Nobody\r\n")
            << false
            << QVersitDocument();

    {
    QVersitDocument expected(QVersitDocument::VCard21Type);
    expected.setComponentType(QLatin1String("VCARD"));
    QVersitProperty property;
    property.setName(QLatin1String("X-EXAMPLES"));
    property.setValue(QLatin1String("Family vCard"));
    expected.addProperty(property);

    QVersitDocument nested1(QVersitDocument::VCard21Type);
    nested1.setComponentType(QLatin1String("VCARD"));
    property.setName(QLatin1String("FN"));
    property.setValue(QLatin1String("John"));
    nested1.addProperty(property);
    expected.addSubDocument(nested1);

    QVersitDocument nested2(QVersitDocument::VCard21Type);
    nested2.setComponentType(QLatin1String("VCARD"));
    property.setName(QLatin1String("FN"));
    property.setValue(QLatin1String("Jenny"));
    nested2.addProperty(property);
    expected.addSubDocument(nested2);

    QTest::newRow("Grouped vCard")
            << QByteArray(
                    "BEGIN:VCARD\r\n"
                    "VERSION:2.1\r\n"
                    "X-EXAMPLES:Family vCard\r\n"
                    "BEGIN:VCARD\r\n"
                    "VERSION:2.1\r\n"
                    "FN:John\r\n"
                    "END:VCARD\r\n"
                    "BEGIN:VCARD\r\n"
                    "VERSION:2.1\r\n"
                    "FN:Jenny\r\n"
                    "END:VCARD\r\n"
                    "END:VCARD")
            << true
            << expected;
    }

    {
        QVersitDocument expected(QVersitDocument::ICalendar20Type);
        expected.setComponentType(QLatin1String("VCALENDAR"));
        QVersitProperty property;
        property.setName(QLatin1String("PRODID"));
        property.setValue(QLatin1String("-//hacksw/handcal//NONSGML v1.0//EN"));
        expected.addProperty(property);
        QVersitDocument nested(QVersitDocument::ICalendar20Type);
        nested.setComponentType(QLatin1String("VEVENT"));
        property.setName(QLatin1String("DTSTART"));
        property.setValue(QLatin1String("19970714T170000Z"));
        nested.addProperty(property);
        property.setName(QLatin1String("DTEND"));
        property.setValue(QLatin1String("19970715T035959Z"));
        nested.addProperty(property);
        property.setName(QLatin1String("SUMMARY"));
        property.setValue(QLatin1String("Bastille Day Party"));
        nested.addProperty(property);
        expected.addSubDocument(nested);
        QTest::newRow("iCalendar sample from spec")
            << QByteArray(
                    "BEGIN:VCALENDAR\r\n"
                    "VERSION:2.0\r\n"
                    "PRODID:-//hacksw/handcal//NONSGML v1.0//EN\r\n"
                    "BEGIN:VEVENT\r\n"
                    "DTSTART:19970714T170000Z\r\n"
                    "DTEND:19970715T035959Z\r\n"
                    "SUMMARY:Bastille Day Party\r\n"
                    "END:VEVENT\r\n"
                    "END:VCALENDAR\r\n")
            << true
            << expected;
    }
}

void tst_QVersitReader::testDecodeQuotedPrintable()
{
#ifndef QT_BUILD_INTERNAL
    QSKIP("Testing private API", SkipSingle);
#else
    QFETCH(QByteArray, encoded);

    QFETCH(QByteArray, decoded);
    mReaderPrivate->decodeQuotedPrintable(&encoded);
    QCOMPARE(encoded, decoded);
#endif
}

void tst_QVersitReader::testDecodeQuotedPrintable_data()
{
#ifdef QT_BUILD_INTERNAL
    QTest::addColumn<QByteArray>("encoded");
    QTest::addColumn<QByteArray>("decoded");


    QTest::newRow("Soft line breaks")
            << QByteArray("This=\r\n is =\r\none line.")
            << QByteArray("This is one line.");

    QTest::newRow("Characters recommended to be encoded according to RFC 1521")
            << QByteArray("To be decoded: =0A=0D=21=22=23=24=3D=40=5B=5C=5D=5E=60=7B=7C=7D=7E")
            << QByteArray("To be decoded: \n\r!\"#$=@[\\]^`{|}~");

    QTest::newRow("Characters recommended to be encoded according to RFC 1521(lower case)")
            << QByteArray("To be decoded: =0a=0d=21=22=23=24=3d=40=5b=5c=5d=5e=60=7b=7c=7d=7e")
            << QByteArray("To be decoded: \n\r!\"#$=@[\\]^`{|}~");

    QTest::newRow("random characters encoded")
            << QByteArray("=45=6E=63=6F=64=65=64 =64=61=74=61")
            << QByteArray("Encoded data");

    QTest::newRow("short string1")
            << QByteArray("-=_")
            << QByteArray("-=_");

    QTest::newRow("short string2")
            << QByteArray("=0")
            << QByteArray("=0");

    QTest::newRow("short string2")
            << QByteArray("\r")
            << QByteArray("\r");

    QTest::newRow("short string2")
            << QByteArray("\n")
            << QByteArray("\n");

    QTest::newRow("short string2")
            << QByteArray("\n\r")
            << QByteArray("\n\r");

    QTest::newRow("White spaces")
            << QByteArray("=09=20")
            << QByteArray("\t ");
#endif
}
void tst_QVersitReader::testParamName()
{
#ifndef QT_BUILD_INTERNAL
    QSKIP("Testing private API", SkipSingle);
#else
    // Empty value
    QByteArray param;
    QCOMPARE(mReaderPrivate->paramName(param, mAsciiCodec),QString());

    // Only value present
    param = "WORK";
    QCOMPARE(mReaderPrivate->paramName(param, mAsciiCodec),
             QLatin1String("TYPE"));

    // The below tests intentionally use the misspelling TIPE to avoid the default behaviour of
    // returning TYPE when the name can't be parsed.
    // Both name and value, spaces after the name
    param = "TIPE \t =WORK";
    QCOMPARE(mReaderPrivate->paramName(param, mAsciiCodec),
             QLatin1String("TIPE"));

    // Both name and value, no spaces after the name
    param = "TIPE=WORK";
    QCOMPARE(mReaderPrivate->paramName(param, mAsciiCodec),
             QLatin1String("TIPE"));

    // Test wide character support.
    QTextCodec* codec = QTextCodec::codecForName("UTF-16BE");
    param = codec->fromUnicode(QLatin1String("TIPE=WORK"));
    QCOMPARE(mReaderPrivate->paramName(param, codec),
             QLatin1String("TIPE"));
#endif
}

void tst_QVersitReader::testParamValue()
{
#ifndef QT_BUILD_INTERNAL
    QSKIP("Testing private API", SkipSingle);
#else
    // Empty value
    QByteArray param;
    QCOMPARE(mReaderPrivate->paramValue(param, mAsciiCodec),QString());

    // Only value present
    param = "WORK";
    QCOMPARE(mReaderPrivate->paramValue(param, mAsciiCodec),
             QLatin1String("WORK"));

    // Name and equals sign, but no value
    param = "TYPE=";
    QCOMPARE(mReaderPrivate->paramValue(param, mAsciiCodec),QString());

    // Name and equals sign, but value has only spaces
    param = "TYPE=  \t ";
    QCOMPARE(mReaderPrivate->paramValue(param, mAsciiCodec),QString());

    // Both name and value, spaces before the value
    param = "TYPE= \t  WORK";
    QCOMPARE(mReaderPrivate->paramValue(param, mAsciiCodec),
             QLatin1String("WORK"));

    // Both name and value, no spaces before the value
    param = "ENCODING=QUOTED-PRINTABLE";
    QCOMPARE(mReaderPrivate->paramValue(param, mAsciiCodec),
             QLatin1String("QUOTED-PRINTABLE"));

    // Test wide character support.
    QTextCodec* codec = QTextCodec::codecForName("UTF-16BE");
    param = codec->fromUnicode(QLatin1String("TYPE=WORK"));
    QCOMPARE(mReaderPrivate->paramValue(param, codec),
             QLatin1String("WORK"));
#endif
}

void tst_QVersitReader::testExtractPart()
{
#ifndef QT_BUILD_INTERNAL
    QSKIP("Testing private API", SkipSingle);
#else
    QByteArray originalStr;

    // Negative starting position
    QCOMPARE(mReaderPrivate->extractPart(originalStr,-1,1), QByteArray());

    // Empty original string
    QCOMPARE(mReaderPrivate->extractPart(originalStr,0,1), QByteArray());

    // Trimmed substring empty
    originalStr = " \t \t";
    QCOMPARE(mReaderPrivate->extractPart(originalStr,0,4), QByteArray());

    // The given substring length is greater than the original string length
    originalStr = "ENCODING=7BIT";
    QCOMPARE(mReaderPrivate->extractPart(originalStr,0,100), originalStr);

    // Non-empty substring, from the beginning
    originalStr = " TYPE=WORK ; X-PARAM=X-VALUE; ENCODING=8BIT";
    QCOMPARE(mReaderPrivate->extractPart(originalStr,0,11),
             QByteArray("TYPE=WORK"));

    // Non-empty substring, from the middle
    QCOMPARE(mReaderPrivate->extractPart(originalStr,12,16),
             QByteArray("X-PARAM=X-VALUE"));

    // Non-empty substring, from the middle to the end
    QCOMPARE(mReaderPrivate->extractPart(originalStr,29),
             QByteArray("ENCODING=8BIT"));
#endif
}

void tst_QVersitReader::testExtractParts()
{
#ifndef QT_BUILD_INTERNAL
    QSKIP("Testing private API", SkipSingle);
#else
    QList<QByteArray> parts;

    // Empty value
    QByteArray text;
    parts = mReaderPrivate->extractParts(text,";", mAsciiCodec);
    QVERIFY(parts.isEmpty());

    // Only separator
    text = ";";
    parts = mReaderPrivate->extractParts(text,";", mAsciiCodec);
    QVERIFY(parts.isEmpty());

    // One part
    text = "part";
    parts = mReaderPrivate->extractParts(text,";", mAsciiCodec);
    QCOMPARE(parts.count(),1);
    QCOMPARE(QLatin1String(parts[0]),QLatin1String("part"));

    // Separator in the beginning, one part
    text = ";part";
    parts = mReaderPrivate->extractParts(text,";", mAsciiCodec);
    QCOMPARE(parts.count(),1);
    QCOMPARE(QLatin1String(parts[0]),QLatin1String("part"));

    // Separator in the end, one part
    text = "part;";
    parts = mReaderPrivate->extractParts(text,";", mAsciiCodec);
    QCOMPARE(parts.count(),1);
    QCOMPARE(QLatin1String(parts[0]),QLatin1String("part"));

    // One part that contains escaped separator
    text = "part\\;";
    parts = mReaderPrivate->extractParts(text,";", mAsciiCodec);
    QCOMPARE(parts.count(),1);
    QCOMPARE(QLatin1String(parts[0]),QLatin1String("part\\;"));

    // Two parts
    text = "part1;part2";
    parts = mReaderPrivate->extractParts(text,";", mAsciiCodec);
    QCOMPARE(parts.count(),2);
    QCOMPARE(QLatin1String(parts[0]),QLatin1String("part1"));
    QCOMPARE(QLatin1String(parts[1]),QLatin1String("part2"));

    // Two parts that contain escaped separators
    text = "pa\\;rt1;par\\;t2";
    parts = mReaderPrivate->extractParts(text,";", mAsciiCodec);
    QCOMPARE(parts.count(),2);
    QCOMPARE(QLatin1String(parts[0]),QLatin1String("pa\\;rt1"));
    QCOMPARE(QLatin1String(parts[1]),QLatin1String("par\\;t2"));

    // Test wide character support
    QTextCodec* codec = QTextCodec::codecForName("UTF-16BE");
    text = codec->fromUnicode(QLatin1String("part1;part2"));
    parts = mReaderPrivate->extractParts(text,";", codec);
    QCOMPARE(parts.count(),2);
    QCOMPARE(codec->toUnicode(parts[0]),QLatin1String("part1"));
    QCOMPARE(codec->toUnicode(parts[1]),QLatin1String("part2"));
#endif
}

void tst_QVersitReader::testExtractPropertyGroupsAndName()
{
#ifndef QT_BUILD_INTERNAL
    QSKIP("Testing private API", SkipSingle);
#else
    QPair<QStringList,QString> groupsAndName;

    // Empty string
    LByteArray cursor(QByteArray(" "));
    groupsAndName = mReaderPrivate->extractPropertyGroupsAndName(&cursor, mAsciiCodec);
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QString());

    // No value -> returns empty string and no groups
    QByteArray property("TEL");
    cursor = property;
    groupsAndName = mReaderPrivate->extractPropertyGroupsAndName(&cursor, mAsciiCodec);
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QString());

    // Simple name and value
    property = "TEL:123";
    cursor = property;
    groupsAndName = mReaderPrivate->extractPropertyGroupsAndName(&cursor, mAsciiCodec);
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QLatin1String("TEL"));

    // One whitespace before colon
    property = "TEL :123";
    cursor = property;
    groupsAndName = mReaderPrivate->extractPropertyGroupsAndName(&cursor, mAsciiCodec);
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QLatin1String("TEL"));

    // Several whitespaces before colon
    property = "TEL \t  :123";
    cursor = property;
    groupsAndName = mReaderPrivate->extractPropertyGroupsAndName(&cursor, mAsciiCodec);
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QLatin1String("TEL"));

    // Name contains a group
    property = "group1.TEL:1234";
    cursor = property;
    groupsAndName = mReaderPrivate->extractPropertyGroupsAndName(&cursor, mAsciiCodec);
    QCOMPARE(groupsAndName.first.count(),1);
    QCOMPARE(groupsAndName.first.takeFirst(),QLatin1String("group1"));
    QCOMPARE(groupsAndName.second,QLatin1String("TEL"));

    // Name contains more than one group
    property = "group1.group2.TEL:12345";
    cursor = property;
    groupsAndName = mReaderPrivate->extractPropertyGroupsAndName(&cursor, mAsciiCodec);
    QCOMPARE(groupsAndName.first.count(),2);
    QCOMPARE(groupsAndName.first.takeFirst(),QLatin1String("group1"));
    QCOMPARE(groupsAndName.first.takeFirst(),QLatin1String("group2"));
    QCOMPARE(groupsAndName.second,QLatin1String("TEL"));
    QCOMPARE(cursor.toByteArray(), QByteArray(":12345"));

    // Property contains one parameter
    property = "TEL;WORK:123";
    cursor = property;
    groupsAndName = mReaderPrivate->extractPropertyGroupsAndName(&cursor, mAsciiCodec);
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QLatin1String("TEL"));

    // Property contains several parameters
    property = "EMAIL;INTERNET;ENCODING=QUOTED-PRINTABLE:user=40ovi.com";
    cursor = property;
    groupsAndName = mReaderPrivate->extractPropertyGroupsAndName(&cursor, mAsciiCodec);
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QLatin1String("EMAIL"));

    // Name contains an escaped semicolon
    property = "X-proper\\;ty:value";
    cursor = property;
    groupsAndName = mReaderPrivate->extractPropertyGroupsAndName(&cursor, mAsciiCodec);
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QLatin1String("X-proper\\;ty"));

    // Test wide character support
    QTextCodec* codec = QTextCodec::codecForName("UTF-16BE");
    property = codec->fromUnicode(QLatin1String("group1.group2.TEL;WORK:123"));
    cursor = property;
    groupsAndName = mReaderPrivate->extractPropertyGroupsAndName(&cursor, codec);
    QCOMPARE(groupsAndName.first.count(),2);
    QCOMPARE(groupsAndName.first.takeFirst(),QLatin1String("group1"));
    QCOMPARE(groupsAndName.first.takeFirst(),QLatin1String("group2"));
    QCOMPARE(groupsAndName.second,QLatin1String("TEL"));
    QCOMPARE(cursor.size(), 18); // ";WORK:123" in UTF16 is 18 bytes
#endif
}

void tst_QVersitReader::testExtractVCard21PropertyParams()
{
#ifndef QT_BUILD_INTERNAL
    QSKIP("Testing private API", SkipSingle);
#else
    // No parameters
    LByteArray cursor(QByteArray(":123"));
    QCOMPARE(mReaderPrivate->extractVCard21PropertyParams(&cursor, mAsciiCodec).count(), 0);

    // "Empty" parameter
    cursor = QByteArray(";:123");
    QCOMPARE(mReaderPrivate->extractVCard21PropertyParams(&cursor, mAsciiCodec).count(), 0);

    // Semicolon found, but no value for the property
    cursor = QByteArray(";TYPE=X-TYPE");
    QCOMPARE(mReaderPrivate->extractVCard21PropertyParams(&cursor, mAsciiCodec).count(), 0);

    // The property name contains an escaped semicolon, no parameters
    cursor = QByteArray(":value");
    QCOMPARE(mReaderPrivate->extractVCard21PropertyParams(&cursor, mAsciiCodec).count(), 0);

    // The property value contains a semicolon, no parameters
    cursor = QByteArray(":va;lue");
    QCOMPARE(mReaderPrivate->extractVCard21PropertyParams(&cursor, mAsciiCodec).count(), 0);

    // One parameter
    cursor = QByteArray(";HOME:123");
    QMultiHash<QString,QString> params = mReaderPrivate->extractVCard21PropertyParams(&cursor,
                                                                                   mAsciiCodec);
    QCOMPARE(1, params.count());
    QCOMPARE(1, params.values(QLatin1String("TYPE")).count());
    QCOMPARE(params.values(QLatin1String("TYPE"))[0],QLatin1String("HOME"));

    // Two parameters of the same type
    cursor = QByteArray(";HOME;VOICE:123");
    params = mReaderPrivate->extractVCard21PropertyParams(&cursor, mAsciiCodec);
    QCOMPARE(2, params.count());
    QCOMPARE(2, params.values(QLatin1String("TYPE")).count());
    QCOMPARE(params.values(QLatin1String("TYPE"))[0],QLatin1String("HOME"));
    QCOMPARE(params.values(QLatin1String("TYPE"))[1],QLatin1String("VOICE"));

    // Two parameters, several empty parameters (extra semicolons)
    cursor = QByteArray(";;;;HOME;;;;;VOICE;;;:123");
    params = mReaderPrivate->extractVCard21PropertyParams(&cursor, mAsciiCodec);
    QCOMPARE(2, params.count());
    QCOMPARE(2, params.values(QLatin1String("TYPE")).count());
    QCOMPARE(params.values(QLatin1String("TYPE"))[0],QLatin1String("HOME"));
    QCOMPARE(params.values(QLatin1String("TYPE"))[1],QLatin1String("VOICE"));

    // Two parameters with different types
    cursor = QByteArray(";INTERNET;ENCODING=QUOTED-PRINTABLE:user=40ovi.com");
    params.clear();
    params = mReaderPrivate->extractVCard21PropertyParams(&cursor, mAsciiCodec);
    QCOMPARE(2, params.count());
    QList<QString> typeParams = params.values(QLatin1String("TYPE"));
    QCOMPARE(1, typeParams.count());
    QCOMPARE(typeParams[0],QLatin1String("INTERNET"));
    QList<QString> encodingParams = params.values(QLatin1String("ENCODING"));
    QCOMPARE(1, encodingParams.count());
    QCOMPARE(encodingParams[0],QLatin1String("QUOTED-PRINTABLE"));

    // Test wide character support.
    QTextCodec* codec = QTextCodec::codecForName("UTF-16BE");
    QByteArray data = VersitUtils::encode(";HOME;CHARSET=UTF-16:123", codec);
    cursor = data;
    params = mReaderPrivate->extractVCard21PropertyParams(&cursor, codec);
    QCOMPARE(2, params.count());
    typeParams = params.values(QLatin1String("TYPE"));
    QCOMPARE(1, typeParams.count());
    QCOMPARE(typeParams[0],QLatin1String("HOME"));
    encodingParams = params.values(QLatin1String("CHARSET"));
    QCOMPARE(1, encodingParams.count());
    QCOMPARE(encodingParams[0],QLatin1String("UTF-16"));
#endif
}

void tst_QVersitReader::testExtractVCard30PropertyParams()
{
#ifndef QT_BUILD_INTERNAL
    QSKIP("Testing private API", SkipSingle);
#else
    // No parameters
    LByteArray cursor(QByteArray(":123"));
    QCOMPARE(mReaderPrivate->extractVCard30PropertyParams(&cursor, mAsciiCodec).count(), 0);

    // One parameter
    cursor = QByteArray(";TYPE=HOME:123");
    QMultiHash<QString,QString> params = mReaderPrivate->extractVCard30PropertyParams(&cursor,
                                                                                   mAsciiCodec);
    QCOMPARE(params.count(), 1);
    QCOMPARE(params.values(QLatin1String("TYPE")).count(), 1);
    QCOMPARE(params.values(QLatin1String("TYPE"))[0], QLatin1String("HOME"));

    // One parameter with an escaped semicolon
    cursor = QByteArray(";para\\;meter:value");
    params = mReaderPrivate->extractVCard30PropertyParams(&cursor, mAsciiCodec);
    QCOMPARE(params.count(), 1);
    QCOMPARE(params.values(QLatin1String("TYPE")).count(), 1);
    QCOMPARE(params.values(QLatin1String("TYPE"))[0], QLatin1String("para;meter"));

    // One parameter with and escaped comma in the name and the value
    cursor = QByteArray(";X-PA\\,RAM=VAL\\,UE:123");
    params = mReaderPrivate->extractVCard30PropertyParams(&cursor, mAsciiCodec);
    QCOMPARE(params.count(), 1);
    QCOMPARE(params.values(QLatin1String("X-PA,RAM")).count(), 1);
    QCOMPARE(params.values(QLatin1String("X-PA,RAM"))[0], QLatin1String("VAL,UE"));

    // Two parameters of the same type
    cursor = QByteArray(";TYPE=HOME,VOICE:123");
    params = mReaderPrivate->extractVCard30PropertyParams(&cursor, mAsciiCodec);
    QCOMPARE(params.count(), 2);
    QCOMPARE(params.values(QLatin1String("TYPE")).count(), 2);
    QVERIFY(params.values(QLatin1String("TYPE")).contains(QLatin1String("HOME")));
    QVERIFY(params.values(QLatin1String("TYPE")).contains(QLatin1String("VOICE")));

    // Two parameters of the same type in separate name-values
    cursor = QByteArray(";TYPE=HOME;TYPE=VOICE:123");
    params = mReaderPrivate->extractVCard30PropertyParams(&cursor, mAsciiCodec);
    QCOMPARE(params.count(), 2);
    QCOMPARE(params.values(QLatin1String("TYPE")).count(), 2);
    QVERIFY(params.values(QLatin1String("TYPE")).contains(QLatin1String("HOME")));
    QVERIFY(params.values(QLatin1String("TYPE")).contains(QLatin1String("VOICE")));

    // Three parameters of the same type
    cursor = QByteArray(";TYPE=PREF,HOME,VOICE:123");
    params = mReaderPrivate->extractVCard30PropertyParams(&cursor, mAsciiCodec);
    QCOMPARE(params.count(), 3);
    QCOMPARE(params.values(QLatin1String("TYPE")).count(), 3);
    QVERIFY(params.values(QLatin1String("TYPE")).contains(QLatin1String("PREF")));
    QVERIFY(params.values(QLatin1String("TYPE")).contains(QLatin1String("HOME")));
    QVERIFY(params.values(QLatin1String("TYPE")).contains(QLatin1String("VOICE")));

    // Two parameters with different types
    cursor = QByteArray(";TYPE=HOME;X-PARAM=X-VALUE:Home Street 1");
    params.clear();
    params = mReaderPrivate->extractVCard30PropertyParams(&cursor, mAsciiCodec);
    QCOMPARE(params.count(), 2);
    QList<QString> typeParams = params.values(QLatin1String("TYPE"));
    QCOMPARE(typeParams.count(), 1);
    QCOMPARE(typeParams[0],QLatin1String("HOME"));
    QList<QString> encodingParams = params.values(QLatin1String("X-PARAM"));
    QCOMPARE(encodingParams.count(), 1);
    QCOMPARE(encodingParams[0],QLatin1String("X-VALUE"));

    // Test wide character support.
    QTextCodec* codec = QTextCodec::codecForName("UTF-16BE");
    QByteArray data = VersitUtils::encode(";TIPE=HOME,VOICE;CHARSET=UTF-16:123", codec);
    cursor = data;
    params = mReaderPrivate->extractVCard30PropertyParams(&cursor, codec);
    QCOMPARE(params.count(), 3);
    typeParams = params.values(QLatin1String("TIPE"));
    QCOMPARE(params.values(QLatin1String("TIPE")).count(), 2);
    QVERIFY(params.values(QLatin1String("TIPE")).contains(QLatin1String("HOME")));
    QVERIFY(params.values(QLatin1String("TIPE")).contains(QLatin1String("VOICE")));
    encodingParams = params.values(QLatin1String("CHARSET"));
    QCOMPARE(1, encodingParams.count());
    QCOMPARE(encodingParams[0],QLatin1String("UTF-16"));
#endif
}

void tst_QVersitReader::testExtractParams()
{
#ifndef QT_BUILD_INTERNAL
    QSKIP("Testing private API", SkipSingle);
#else
    LByteArray cursor;
    QByteArray data = ":123";
    cursor = data;
    QList<QByteArray> params = mReaderPrivate->extractParams(&cursor, mAsciiCodec);
    QCOMPARE(params.size(), 0);
    QVERIFY(cursor == QByteArray("123"));

    data = "a;b:123";
    cursor = data;
    params = mReaderPrivate->extractParams(&cursor, mAsciiCodec);
    QCOMPARE(params.size(), 2);
    QVERIFY(cursor == QByteArray("123"));
    QCOMPARE(params.at(0), QByteArray("a"));
    QCOMPARE(params.at(1), QByteArray("b"));

    QTextCodec* codec = QTextCodec::codecForName("UTF-16BE");
    data = VersitUtils::encode(":123", codec);
    cursor = data;
    params = mReaderPrivate->extractParams(&cursor, codec);
    QCOMPARE(params.size(), 0);
    QCOMPARE(cursor.size(), 6); // "123" takes up 6 bytes in UTF-16

    data = VersitUtils::encode("a;b:123", codec);
    cursor = data;
    params = mReaderPrivate->extractParams(&cursor, codec);
    QCOMPARE(params.size(), 2);
    QCOMPARE(cursor.size(), 6); // "123" takes up 6 bytes in UTF-16
#endif
}

Q_DECLARE_METATYPE(QList<QString>)

void tst_QVersitReader::testReadLine()
{
#ifndef QT_BUILD_INTERNAL
    QSKIP("Testing private API", SkipSingle);
#else
    QFETCH(QByteArray, codecName);
    QFETCH(QString, data);
    QFETCH(QList<QString>, expectedLines);

    QTextCodec* codec = QTextCodec::codecForName(codecName);
    QTextEncoder* encoder = codec->makeEncoder();
    encoder->fromUnicode(QString());

    QByteArray bytes(encoder->fromUnicode(data));

    mInputDevice->close();
    mInputDevice->setData(bytes);
    mInputDevice->open(QIODevice::ReadWrite);

    LineReader lineReader(mInputDevice, codec, 10);

    QByteArray testLine("test pushed line");
    // Check that all expected lines are read...
    foreach (QString expectedLine, expectedLines) {
        // (test push a line and read it)
        lineReader.pushLine(testLine);
        QVERIFY(!lineReader.atEnd());
        LByteArray line = lineReader.readLine();
        QCOMPARE(line.toByteArray(), testLine);


        QByteArray expectedBytes(encoder->fromUnicode(expectedLine));
        QVERIFY(!lineReader.atEnd());
        line = lineReader.readLine();
        if(line.toByteArray() != expectedBytes) {
            qDebug() << line.toByteArray();
            qDebug() << expectedBytes;
            QCOMPARE(line.toByteArray(), expectedBytes);
        }
        QCOMPARE(line.size(), expectedBytes.length());
    }

    // (test push a line to a line reader that's reached its end)
    lineReader.pushLine(testLine);
    QVERIFY(!lineReader.atEnd());
    LByteArray line = lineReader.readLine();
    QCOMPARE(line.toByteArray(), testLine);

    // ...And that there are no more lines
    line = lineReader.readLine();
    QVERIFY(line.isEmpty());
    QVERIFY(lineReader.atEnd());

    delete encoder;
#endif
}

void tst_QVersitReader::testReadLine_data()
{
#ifdef QT_BUILD_INTERNAL
    // Note: for this test, we set mLineReader to read 10 bytes at a time.  Lines of multiples of
    // 10 bytes are hence border cases.
    // Note: QVersitReaders' LineReader contains hacks that sniff for colons in the input to enable
    // a workaround for malformed vCards with badly wrapped lines (see the last test case)
    // For testing of normal wrapping behaviour, a colon must appear in every line.
    QTest::addColumn<QByteArray>("codecName");
    QTest::addColumn<QString>("data");
    QTest::addColumn<QList<QString> >("expectedLines");

    QList<QByteArray> codecNames;
    codecNames << "UTF-8" << "UTF-16";

    foreach (QByteArray codecName, codecNames) {
        QTest::newRow("empty " + codecName)
                << codecName
                << ""
                << QList<QString>();

        QTest::newRow("one line " + codecName)
                << codecName
                << "line:"
                << (QList<QString>() << QLatin1String("line:"));

        QTest::newRow("one ten-byte line " + codecName)
                << codecName
                << "10letters:"
                << (QList<QString>() << QLatin1String("10letters:"));

        QTest::newRow("one long line " + codecName)
                << codecName
                << "one:line longer than ten characters"
                << (QList<QString>() << QLatin1String("one:line longer than ten characters"));

        QTest::newRow("one terminated line " + codecName)
                << codecName
                << "one:line longer than ten characters\r\n"
                << (QList<QString>() << QLatin1String("one:line longer than ten characters"));

        QTest::newRow("two lines " + codecName)
                << codecName
                << "two:\r\nlines:"
                << (QList<QString>() << QLatin1String("two:") << QLatin1String("lines:"));

        QTest::newRow("two terminated lines " + codecName)
                << codecName
                << "two:\r\nlines:\r\n"
                << (QList<QString>() << QLatin1String("two:") << QLatin1String("lines:"));

        QTest::newRow("two long lines " + codecName)
                << codecName
                << "one:line longer than ten characters\r\nanother line:\r\n"
                << (QList<QString>() << QLatin1String("one:line longer than ten characters") << QLatin1String("another line:"));

        QTest::newRow("two full lines " + codecName)
                << codecName
                << "10letters:\r\n8letter:\r\n"
                << (QList<QString>() << QLatin1String("10letters:") << QLatin1String("8letter:"));

        QTest::newRow("a nine-byte line " + codecName)
                << codecName
                << "9letters:\r\nanother:line\r\n"
                << (QList<QString>() << QLatin1String("9letters:") << QLatin1String("another:line"));

        QTest::newRow("a blank line " + codecName)
                << codecName
                << "one:\r\n\r\ntwo:\r\n"
                << (QList<QString>() << QLatin1String("one:") << QLatin1String("two:"));

        QTest::newRow("folded lines " + codecName)
                << codecName
                << "fold:ed\r\n  line\r\nsecond: line\r\n"
                << (QList<QString>() << QLatin1String("fold:ed line") << QLatin1String("second: line"));

        QTest::newRow("multiply folded lines " + codecName)
                << codecName
                << "fo\r\n lded:\r\n  line\r\nseco\r\n\tnd:l\r\n ine\r\n"
                << (QList<QString>() << QLatin1String("folded: line") << QLatin1String("second:line"));

        QTest::newRow("fold hidden after a chunk " + codecName)
                << codecName
                << "8letter:\r\n  on one line\r\n"
                << (QList<QString>() << QLatin1String("8letter: on one line"));

        QTest::newRow("three mac lines " + codecName)
                << codecName
                << "one:\rtwo:\rthree:\r"
                << (QList<QString>() << QLatin1String("one:") << QLatin1String("two:") << QLatin1String("three:"));

        // Tests a workaround to parse a certain malformed vCard
        QTest::newRow("badly wrapped lines " + codecName)
                << codecName
                << "one:line\r\ntwo\r\nthree\r\n"
                << (QList<QString>() << QLatin1String("one:linetwothree"));
    }
#endif
}

void tst_QVersitReader::testByteArrayInput()
{
    delete mReader;
    const QByteArray& oneDocument =
        "BEGIN:VCARD\r\nVERSION:2.1\r\nFN:John\r\nEND:VCARD\r\n";

    mReader = new QVersitReader(oneDocument);
    QVERIFY(mReader->device() == 0);
    QVERIFY2(mReader->startReading(), QString::number(mReader->error()).toAscii().data());
    QVERIFY2(mReader->waitForFinished(), QString::number(mReader->error()).toAscii().data());
    QList<QVersitDocument> results = mReader->results();
    QCOMPARE(mReader->state(), QVersitReader::FinishedState);
    QCOMPARE(mReader->error(), QVersitReader::NoError);
    QCOMPARE(results.count(),1);
    QVersitDocument result = results.first();
    QCOMPARE(result.type(), QVersitDocument::VCard21Type);
    QList<QVersitProperty> properties = result.properties();
    QCOMPARE(properties.length(), 1);
    QCOMPARE(properties.first().name(), QLatin1String("FN"));
    QCOMPARE(properties.first().value(), QLatin1String("John"));
}

void tst_QVersitReader::testRemoveBackSlashEscaping()
{
#ifndef QT_BUILD_INTERNAL
    QSKIP("Testing private API", SkipSingle);
#else
    // Empty string
    QString input;
    QVersitReaderPrivate::removeBackSlashEscaping(&input);
    QCOMPARE(input,QString());

    // Nothing to escape in the string
    input = QLatin1String("Nothing to escape");
    QVersitReaderPrivate::removeBackSlashEscaping(&input);
    QCOMPARE(input,QLatin1String("Nothing to escape"));

    // Line break, semicolon, backslash and comma in the string
    input = QLatin1String("These should be unescaped \\n \\N \\; \\, \\\\");
    QVersitReaderPrivate::removeBackSlashEscaping(&input);
    QCOMPARE(input, QLatin1String("These should be unescaped \r\n \r\n ; , \\"));

    // Don't remove escaping within quotes
    input = QLatin1String("\"Quoted \\n \\N \\; \\,\"");
    QVersitReaderPrivate::removeBackSlashEscaping(&input);
    QCOMPARE(input, QLatin1String("\"Quoted \\n \\N \\; \\,\""));
#endif
}

QTEST_MAIN(tst_QVersitReader)

