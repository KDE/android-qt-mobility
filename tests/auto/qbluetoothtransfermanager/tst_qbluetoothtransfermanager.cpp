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
#include <QMap>
#include <QTextStream>

#include <qbluetoothtransferrequest.h>
#include <qbluetoothtransfermanager.h>
#include <qbluetoothtransferreply.h>
#include <qbluetoothaddress.h>
#include <qbluetoothlocaldevice.h>

#include <qbluetoothdeviceinfo.h>
#include <qbluetoothserviceinfo.h>
#include <qbluetoothservicediscoveryagent.h>
#include <btclient.h>


QTM_USE_NAMESPACE

typedef QMap<int,QVariant> tst_QBluetoothTransferManager_QParameterMap;
Q_DECLARE_METATYPE(tst_QBluetoothTransferManager_QParameterMap)

#ifdef Q_OS_SYMBIAN
char BTADDRESS[] = "00:09:DD:50:93:DD";
static const QString testfile("c:\\data\\testdata.txt");
#else
char BTADDRESS[] = "00:00:00:00:00:00";
#endif

static const int MaxConnectTime = 60 * 1000;   // 1 minute in ms

class tst_QBluetoothTransferManager : public QObject
{
    Q_OBJECT

public:
    tst_QBluetoothTransferManager();
    ~tst_QBluetoothTransferManager();

private slots:
    void initTestCase();

    void tst_construction();

    void tst_put_data();
    void tst_put();

    void tst_putAbort_data();
    void tst_putAbort();

    void tst_attribute_data();
    void tst_attribute();

    void tst_operation_data();
    void tst_operation();

    void tst_manager_data();
    void tst_manager();

public slots:
    void serviceDiscovered(const QBluetoothServiceInfo &info);
    void finished();
    void error(QBluetoothServiceDiscoveryAgent::Error error);
private:
    bool done_discovery;
};

tst_QBluetoothTransferManager::tst_QBluetoothTransferManager()
{
}

tst_QBluetoothTransferManager::~tst_QBluetoothTransferManager()
{
}

void tst_QBluetoothTransferManager::initTestCase()
{
    // start Bluetooth if not started
    QBluetoothLocalDevice *device = new QBluetoothLocalDevice();
    device->powerOn();
    delete device;

#ifndef Q_OS_SYMBIAN
    // Go find an echo server for BTADDRESS
    QBluetoothServiceDiscoveryAgent *sda = new QBluetoothServiceDiscoveryAgent(this);
    connect(sda, SIGNAL(serviceDiscovered(QBluetoothServiceInfo)), this, SLOT(serviceDiscovered(QBluetoothServiceInfo)));
    connect(sda, SIGNAL(error(QBluetoothServiceDiscoveryAgent::Error)), this, SLOT(error(QBluetoothServiceDiscoveryAgent::Error)));
    connect(sda, SIGNAL(finished()), this, SLOT(finished()));

    qDebug() << "Starting discovery";
    done_discovery = false;
    memset(BTADDRESS, 0, 18);

    sda->setUuidFilter(QBluetoothUuid(QString(ECHO_SERVICE_UUID)));
    sda->start(QBluetoothServiceDiscoveryAgent::MinimalDiscovery);

    int connectTime = MaxConnectTime;
    while (!done_discovery) {
        QTest::qWait(1000);
        connectTime -= 1000;
    }

    sda->stop();

    if (BTADDRESS[0] == 0) {
        QFAIL("Unable to find test service");
    }
    delete sda;
#endif
}
void tst_QBluetoothTransferManager::error(QBluetoothServiceDiscoveryAgent::Error error)
{
    qDebug() << "Received error" << error;
//    done_discovery = true;
}

void tst_QBluetoothTransferManager::finished()
{
    qDebug() << "Finished";
    done_discovery = true;
}

void tst_QBluetoothTransferManager::serviceDiscovered(const QBluetoothServiceInfo &info)
{
    qDebug() << "Found: " << info.device().name() << info.serviceUuid();
    strcpy(BTADDRESS, info.device().address().toString().toAscii());
    done_discovery = true;
}

void tst_QBluetoothTransferManager::tst_construction()
{
    QBluetoothTransferManager *manager = new QBluetoothTransferManager();

    QVERIFY(manager);
    delete manager;
}

void tst_QBluetoothTransferManager::tst_put_data()
{
    QTest::addColumn<QBluetoothAddress>("address");
    QTest::addColumn<QMap<int, QVariant> >("parameters");

    QMap<int, QVariant> inparameters;
    inparameters.insert((int)QBluetoothTransferRequest::DescriptionAttribute, "Desciption");
    inparameters.insert((int)QBluetoothTransferRequest::LengthAttribute, QVariant(1024));
    inparameters.insert((int)QBluetoothTransferRequest::TypeAttribute, "OPP");

    QTest::newRow("0x000000 COD") << QBluetoothAddress(BTADDRESS) << inparameters;
}

void tst_QBluetoothTransferManager::tst_put()
{
    QFETCH(QBluetoothAddress, address);
    QFETCH(tst_QBluetoothTransferManager_QParameterMap, parameters);

    QBluetoothTransferRequest transferRequest(address);

    foreach (int key, parameters.keys()) {
        transferRequest.setAttribute((QBluetoothTransferRequest::Attribute)key, parameters[key]);
    }

    QBluetoothTransferManager manager;

#ifdef Q_OS_SYMBIAN
    {
        QFile fileToWrite(testfile);
         if (!fileToWrite.open(QIODevice::WriteOnly | QIODevice::Text))
             return;

         QTextStream out(&fileToWrite);
         out << "This_is_testdata!!!";
         fileToWrite.close();
    }

    QFile file(testfile);
    if (!file.exists())
        return;

    QBluetoothTransferReply *reply = manager.put(transferRequest, &file);
    int connectTime = MaxConnectTime;

    QSignalSpy finishedSpy(&manager, SIGNAL(finished(QBluetoothTransferReply*)));
    while (finishedSpy.count() == 0 && connectTime > 0) {
        QTest::qWait(1000);
        connectTime -= 1000;
    }
    QCOMPARE(finishedSpy.count(), 1);

    int error = reply->error();
    qDebug()<<"QtMobilityBtTester::SendData reply->error ="<<error;
    QVERIFY(error == QBluetoothTransferReply::NoError);

    if (error != QBluetoothTransferReply::NoError) {
        qDebug() << "Failed to send file";
    }
    delete reply;
    file.close();
#endif
}

void tst_QBluetoothTransferManager::tst_putAbort_data()
{
    QTest::addColumn<QBluetoothAddress>("address");
    QTest::addColumn<QMap<int, QVariant> >("parameters");

    QMap<int, QVariant> inparameters;
    inparameters.insert((int)QBluetoothTransferRequest::DescriptionAttribute, "Desciption");
    inparameters.insert((int)QBluetoothTransferRequest::LengthAttribute, QVariant(1024));
    inparameters.insert((int)QBluetoothTransferRequest::TypeAttribute, "OPP");

    QTest::newRow("0x000000 COD") << QBluetoothAddress(BTADDRESS) << inparameters;
}

void tst_QBluetoothTransferManager::tst_putAbort()
{
    QFETCH(QBluetoothAddress, address);
    QFETCH(tst_QBluetoothTransferManager_QParameterMap, parameters);

    QBluetoothTransferRequest transferRequest(address);

    foreach (int key, parameters.keys()) {
        transferRequest.setAttribute((QBluetoothTransferRequest::Attribute)key, parameters[key]);
    }

    QBluetoothTransferManager manager;

#ifdef Q_OS_SYMBIAN
    {
        QFile fileToWrite(testfile);
         if (!fileToWrite.open(QIODevice::WriteOnly | QIODevice::Text))
             return;

         QTextStream out(&fileToWrite);
         out << "This_is_testdata!!!";
         fileToWrite.close();
    }

    QFile file(testfile);
    if (!file.exists())
        return;

    QBluetoothTransferReply *reply = manager.put(transferRequest, &file);
    reply->abort();

    int error = reply->error();
    qDebug()<<"QtMobilityBtTester::SendData reply->error ="<<error;
    QVERIFY(error == QBluetoothTransferReply::NoError);

    if (error != QBluetoothTransferReply::NoError) {
        qDebug() << "Failed to send file";
    }
    delete reply;
    file.close();
#endif
}

void tst_QBluetoothTransferManager::tst_attribute_data()
{
    QTest::addColumn<QBluetoothAddress>("address");
    QTest::addColumn<QMap<int, QVariant> >("parameters");

    QMap<int, QVariant> inparameters;
    inparameters.insert((int)QBluetoothTransferRequest::DescriptionAttribute, "Desciption");
    inparameters.insert((int)QBluetoothTransferRequest::LengthAttribute, QVariant(1024));
    inparameters.insert((int)QBluetoothTransferRequest::TypeAttribute, "OPP");

    QTest::newRow("0x000000 COD") << QBluetoothAddress(BTADDRESS) << inparameters;
}

void tst_QBluetoothTransferManager::tst_attribute()
{
    QFETCH(QBluetoothAddress, address);
    QFETCH(tst_QBluetoothTransferManager_QParameterMap, parameters);

    QBluetoothTransferRequest transferRequest(address);

    foreach (int key, parameters.keys()) {
        transferRequest.setAttribute((QBluetoothTransferRequest::Attribute)key, parameters[key]);
    }

    QBluetoothTransferManager manager;

#ifdef Q_OS_SYMBIAN
    {
        QFile fileToWrite(testfile);
         if (!fileToWrite.open(QIODevice::WriteOnly | QIODevice::Text))
             return;

         QTextStream out(&fileToWrite);
         out << "This_is_testdata!!!";
         fileToWrite.close();
    }

    QFile file(testfile);
    if (!file.exists())
        return;

    QBluetoothTransferReply *reply = manager.put(transferRequest, &file);
    reply->abort();

    QVERIFY(reply->attribute(QBluetoothTransferRequest::DescriptionAttribute) == QVariant("Desciption"));
    QVERIFY(reply->attribute(QBluetoothTransferRequest::LengthAttribute) == QVariant("1024"));
    QVERIFY(reply->attribute(QBluetoothTransferRequest::TypeAttribute) == QVariant("OPP"));

    delete reply;
    file.close();
#endif
}

void tst_QBluetoothTransferManager::tst_operation_data()
{
    QTest::addColumn<QBluetoothAddress>("address");

    QTest::newRow("0x000000 COD") << QBluetoothAddress(BTADDRESS);
}

void tst_QBluetoothTransferManager::tst_operation()
{
    QFETCH(QBluetoothAddress, address);

    QBluetoothTransferRequest transferRequest(address);
    QBluetoothTransferManager manager;
#ifdef Q_OS_SYMBIAN
    {
        QFile fileToWrite(testfile);
         if (!fileToWrite.open(QIODevice::WriteOnly | QIODevice::Text))
             return;

         QTextStream out(&fileToWrite);
         out << "This_is_testdata!!!";
         fileToWrite.close();
    }

    QFile file(testfile);
    if (!file.exists())
        return;

    QBluetoothTransferReply *reply = manager.put(transferRequest, &file);

    QVERIFY(reply->operation() == QBluetoothTransferManager::PutOperation);

    delete reply;
    file.close();
#endif
}

void tst_QBluetoothTransferManager::tst_manager_data()
{
    QTest::addColumn<QBluetoothAddress>("address");

    QTest::newRow("0x000000 COD") << QBluetoothAddress(BTADDRESS);
}

void tst_QBluetoothTransferManager::tst_manager()
{
    QFETCH(QBluetoothAddress, address);

    QBluetoothTransferRequest transferRequest(address);
    QBluetoothTransferManager manager;
#ifdef Q_OS_SYMBIAN
    {
        QFile fileToWrite(testfile);
         if (!fileToWrite.open(QIODevice::WriteOnly | QIODevice::Text))
             return;

         QTextStream out(&fileToWrite);
         out << "This_is_testdata!!!";
         fileToWrite.close();
    }

    QFile file(testfile);
    if (!file.exists())
        return;

    QBluetoothTransferReply *reply = manager.put(transferRequest, &file);

    QVERIFY(reply->manager() == &manager);

    delete reply;
    file.close();
#endif
}

QTEST_MAIN(tst_QBluetoothTransferManager)

#include "tst_qbluetoothtransfermanager.moc"
