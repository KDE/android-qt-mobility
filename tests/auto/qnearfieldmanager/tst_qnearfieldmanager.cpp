/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "../qconnectivitytestcommon.h"
#include "qnearfieldmanager_emulator_p.h"

#include <qnearfieldmanager.h>
#include <qndefnfctextrecord.h>
#include <qndefnfcurirecord.h>
#include <qndefmessage.h>
#include <qndefrecord.h>

Q_DECLARE_METATYPE(QNearFieldTarget*)
Q_DECLARE_METATYPE(QNearFieldTarget::Type)
Q_DECLARE_METATYPE(QNdefFilter)
Q_DECLARE_METATYPE(QNdefRecord::TypeNameFormat)

QTM_USE_NAMESPACE

class tst_QNearFieldManager : public QObject
{
    Q_OBJECT

public:
    tst_QNearFieldManager();

private slots:
    void targetDetected_data();
    void targetDetected();

    void unregisterNdefMessageHandler();

    void registerNdefMessageHandler();

    void registerNdefMessageHandler_type_data();
    void registerNdefMessageHandler_type();

    void registerNdefMessageHandler_filter_data();
    void registerNdefMessageHandler_filter();
};

tst_QNearFieldManager::tst_QNearFieldManager()
{
#ifndef Q_OS_SYMBIAN
    QDir::setCurrent(QLatin1String(SRCDIR));
#endif

    qRegisterMetaType<QNdefMessage>("QNdefMessage");
    qRegisterMetaType<QNearFieldTarget *>("QNearFieldTarget*");
}

void tst_QNearFieldManager::targetDetected_data()
{
    QTest::addColumn<QNearFieldTarget::Type>("type");

    QTest::newRow("AnyTarget") << QNearFieldTarget::AnyTarget;
    QTest::newRow("NfcTagType1") << QNearFieldTarget::NfcTagType1;
}

void tst_QNearFieldManager::targetDetected()
{
    QFETCH(QNearFieldTarget::Type, type);

    QNearFieldManagerPrivateImpl *emulatorBackend = new QNearFieldManagerPrivateImpl;
    QNearFieldManager manager(emulatorBackend, 0);

    QSignalSpy targetDetectedSpy(&manager, SIGNAL(targetDetected(QNearFieldTarget*)));
    QSignalSpy targetLostSpy(&manager, SIGNAL(targetLost(QNearFieldTarget*)));

    manager.startTargetDetection(type);

    QTRY_VERIFY(!targetDetectedSpy.isEmpty());

    QNearFieldTarget *target = targetDetectedSpy.first().at(0).value<QNearFieldTarget *>();

    QSignalSpy disconnectedSpy(target, SIGNAL(disconnected()));

    QVERIFY(target);

    QVERIFY(!target->uid().isEmpty());

    QTRY_VERIFY(!targetLostSpy.isEmpty());

    QNearFieldTarget *lostTarget = targetLostSpy.first().at(0).value<QNearFieldTarget *>();

    QCOMPARE(target, lostTarget);

    QVERIFY(!disconnectedSpy.isEmpty());

    manager.stopTargetDetection();
}

void tst_QNearFieldManager::unregisterNdefMessageHandler()
{
    QNearFieldManagerPrivateImpl *emulatorBackend = new QNearFieldManagerPrivateImpl;
    QNearFieldManager manager(emulatorBackend, 0);

    QVERIFY(!manager.unregisterNdefMessageHandler(-1));
    QVERIFY(!manager.unregisterNdefMessageHandler(0));
}

class MessageListener : public QObject
{
    Q_OBJECT

signals:
    void matchedNdefMessage(const QNdefMessage &message, QNearFieldTarget *target);
};

void tst_QNearFieldManager::registerNdefMessageHandler()
{
    QNearFieldManagerPrivateImpl *emulatorBackend = new QNearFieldManagerPrivateImpl;
    QNearFieldManager manager(emulatorBackend, 0);

    MessageListener listener;
    QSignalSpy messageSpy(&listener, SIGNAL(matchedNdefMessage(QNdefMessage,QNearFieldTarget*)));

    int id = manager.registerNdefMessageHandler(&listener,
                                                SIGNAL(matchedNdefMessage(QNdefMessage,QNearFieldTarget*)));

    QVERIFY(id != -1);

    QTRY_VERIFY(!messageSpy.isEmpty());

    const QNdefMessage message = messageSpy.first().at(0).value<QNdefMessage>();
    QNearFieldTarget *target = messageSpy.first().at(1).value<QNearFieldTarget *>();

    QVERIFY(target);

    QVERIFY(manager.unregisterNdefMessageHandler(id));
}

void tst_QNearFieldManager::registerNdefMessageHandler_type_data()
{
    QTest::addColumn<QNdefRecord::TypeNameFormat>("typeNameFormat");
    QTest::addColumn<QByteArray>("type");

    QTest::newRow("Image") << QNdefRecord::Mime << QByteArray("image/png");
    QTest::newRow("URI") << QNdefRecord::NfcRtd << QByteArray("U");
    QTest::newRow("Text") << QNdefRecord::NfcRtd << QByteArray("T");
}

void tst_QNearFieldManager::registerNdefMessageHandler_type()
{
    QFETCH(QNdefRecord::TypeNameFormat, typeNameFormat);
    QFETCH(QByteArray, type);

    QNearFieldManagerPrivateImpl *emulatorBackend = new QNearFieldManagerPrivateImpl;
    QNearFieldManager manager(emulatorBackend, 0);

    MessageListener listener;
    QSignalSpy messageSpy(&listener, SIGNAL(matchedNdefMessage(QNdefMessage,QNearFieldTarget*)));

    int id = manager.registerNdefMessageHandler(typeNameFormat, type, &listener,
                                                SIGNAL(matchedNdefMessage(QNdefMessage,QNearFieldTarget*)));

    QVERIFY(id != -1);

    QTRY_VERIFY(!messageSpy.isEmpty());

    const QNdefMessage message = messageSpy.first().at(0).value<QNdefMessage>();

    bool hasRecord = false;
    foreach (const QNdefRecord &record, message) {
        if (record.typeNameFormat() == typeNameFormat && record.type() == type) {
            hasRecord = true;
            break;
        }
    }

    QVERIFY(hasRecord);

    QNearFieldTarget *target = messageSpy.first().at(1).value<QNearFieldTarget *>();

    QVERIFY(target);
}

void tst_QNearFieldManager::registerNdefMessageHandler_filter_data()
{
    QTest::addColumn<QNdefFilter>("filter");

    QNdefFilter filter;

    QTest::newRow("Empty") << filter;

    filter.clear();
    filter.setOrderMatch(true);
    filter.appendRecord(QNdefRecord::Mime, "image/png");
    filter.appendRecord<QNdefNfcTextRecord>(2, 10);
    filter.appendRecord<QNdefNfcUriRecord>(1, 1);
    QTest::newRow("Image + Multiple Text + URI") << filter;

    filter.clear();
    filter.setOrderMatch(true);
    filter.appendRecord<QNdefNfcTextRecord>(1, 1);
    filter.appendRecord<QNdefNfcUriRecord>(1, 1);
    QTest::newRow("Text + URI") << filter;

    QNdefFilter::Record record;

    filter.clear();
    filter.setOrderMatch(false);
    filter.appendRecord<QNdefNfcUriRecord>(1, 1);
    record.typeNameFormat = QNdefRecord::NfcRtd;
    record.type = "T";
    record.minimum = 1;
    record.maximum = 1;
    filter.appendRecord(record);
    QTest::newRow("Unordered Text + URI") << filter;
}

void tst_QNearFieldManager::registerNdefMessageHandler_filter()
{
    QFETCH(QNdefFilter, filter);

    QNearFieldManagerPrivateImpl *emulatorBackend = new QNearFieldManagerPrivateImpl;
    QNearFieldManager manager(emulatorBackend, 0);

    MessageListener listener;
    QSignalSpy messageSpy(&listener, SIGNAL(matchedNdefMessage(QNdefMessage,QNearFieldTarget*)));

    int id = manager.registerNdefMessageHandler(filter, &listener,
                                                SIGNAL(matchedNdefMessage(QNdefMessage,QNearFieldTarget*)));

    QVERIFY(id != -1);

    QTRY_VERIFY(!messageSpy.isEmpty());

    const QNdefMessage message = messageSpy.first().at(0).value<QNdefMessage>();

    QNearFieldTarget *target = messageSpy.first().at(1).value<QNearFieldTarget *>();

    QVERIFY(target);
}

QTEST_MAIN(tst_QNearFieldManager);

#include "tst_qnearfieldmanager.moc"
