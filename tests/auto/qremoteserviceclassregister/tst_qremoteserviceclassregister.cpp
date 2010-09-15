/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

//TESTED_COMPONENT=src/serviceframework

#include <qservicemanager.h>
#include <qremoteserviceclassregister.h>

#include <QtTest/QtTest>
#include <QtCore>

QTM_USE_NAMESPACE

/*#define QTRY_COMPARE(a,e)                       \
    for (int _i = 0; _i < 5000; _i += 100) {    \
        if ((a) == (e)) break;                  \
        QTest::qWait(100);                      \
    }                                           \
    QCOMPARE(a, e)

#define QTRY_VERIFY(a)                       \
    for (int _i = 0; _i < 5000; _i += 100) {    \
        if (a) break;                  \
        QTest::qWait(100);                      \
    }                                           \
    QVERIFY(a)*/

class UniqueService : public QObject
{
    Q_OBJECT
    Q_SERVICE(UniqueService, "RemoteServiceClasRegisterService", "com.nokia.qt.iremoteserviceclassregistertest", "2.1");

public:
    UniqueService() {}
    static int counter;
};

class SharedService : public QObject
{
    Q_OBJECT
    Q_SERVICE(SharedService,  "RemoteServiceClasRegisterService","com.nokia.qt.iremoteserviceclassregistertest", "2.2");

public:
    SharedService() { }
    static int counter;
};

class DefaultService : public QObject
{
    Q_OBJECT
    Q_SERVICE(DefaultService, "RemoteServiceClasRegisterService", "com.nokia.qt.iremoteserviceclassregistertest", "2.3");

public:
    DefaultService() { }
    static int counter;
};

class DuplicateService : public QObject
{
    Q_OBJECT
    Q_SERVICE(DuplicateService, "RemoteServiceClasRegisterService",  "com.nokia.qt.iremoteserviceclassregistertest", "2.1");

public:
    DuplicateService() { }
    static int counter;
};


class tst_QRemoteServiceClassRegister : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void testRegistration();
};

void tst_QRemoteServiceClassRegister::initTestCase()
{
    bool result = QRemoteServiceClassRegister::registerType<UniqueService>(QRemoteServiceClassRegister::UniqueInstance);
    QVERIFY(result);
    result = QRemoteServiceClassRegister::registerType<DuplicateService>(QRemoteServiceClassRegister::SharedInstance);
    QVERIFY(!result);
    result = QRemoteServiceClassRegister::registerType<SharedService>(QRemoteServiceClassRegister::SharedInstance);
    QVERIFY(result);
    result = QRemoteServiceClassRegister::registerType<DefaultService>(); //-> defaults to Unique
    QVERIFY(result);
}

void tst_QRemoteServiceClassRegister::init()
{
}

void tst_QRemoteServiceClassRegister::cleanupTestCase()
{
    //use QEventLopp::DeferredDeletion
    //QServiceManager::loadInterface makes use of deleteLater() when
    //cleaning up service objects and their respective QPluginLoader
    //we want to force the testcase to run the cleanup code
    QCoreApplication::processEvents(QEventLoop::AllEvents|QEventLoop::DeferredDeletion);
}

void tst_QRemoteServiceClassRegister::testRegistration()
{
    QSet<QByteArray> reference;
    reference.insert("com.nokia.qt.iremoteserviceclassregistertest-2.1");
    reference.insert("com.nokia.qt.iremoteserviceclassregistertest-2.2");
    reference.insert("com.nokia.qt.iremoteserviceclassregistertest-2.3");

    QRemoteServiceClassRegister reg;
    QList<QRemoteServiceIdentifier> idents = reg.types();
    QVERIFY(idents.count() == 3);
    QSet<QByteArray> discoveredValues;
    foreach(QRemoteServiceIdentifier type, idents) {
        discoveredValues.insert(type.iface+'-'+type.version);
        const QMetaObject* meta = reg.metaObject(type);
        if (type.version == QByteArray("2.1"))
            QCOMPARE(meta->className(), UniqueService::staticMetaObject.className());
        else if (type.version == QByteArray("2.2"))
            QCOMPARE(meta->className(), SharedService::staticMetaObject.className());
        else if (type.version == QByteArray("2.3"))
            QCOMPARE(meta->className(), DefaultService::staticMetaObject.className());
        else {
            QString msg = type.iface+'-'+type.version;
            QFAIL("Unknown type: " + msg.toLatin1());
        }
    }

    QVERIFY(discoveredValues==reference);
}

QTEST_MAIN(tst_QRemoteServiceClassRegister)
#include "tst_qremoteserviceclassregister.moc"
