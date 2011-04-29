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

//TESTED_COMPONENT=src/serviceframework

#include <QCoreApplication>
#include "qservicemanager.h"
#include "qservicefilter.h"
#include <QTimer>
#include <QMetaObject>
#include <QMetaMethod>
#include <QtTest/QtTest>
#include <qservice.h>
#include <qremoteserviceregister.h>

#ifndef QT_NO_DBUS
#include <QtDBus/QtDBus>
#if QT_VERSION < 0x040800
inline QDBusArgument &operator<<(QDBusArgument &arg, const QVariantHash &map)
{
    arg.beginMap(QVariant::String, qMetaTypeId<QDBusVariant>());
    QVariantHash::ConstIterator it = map.constBegin();
    QVariantHash::ConstIterator end = map.constEnd();
    for ( ; it != end; ++it) {
        arg.beginMapEntry();
        arg << it.key() << QDBusVariant(it.value());
        arg.endMapEntry();
    }
    arg.endMap();
    return arg;
}
#endif
#endif


#define QTRY_VERIFY(a)                       \
    for (int _i = 0; _i < 5000; _i += 100) {    \
        if (a) break;                  \
        QTest::qWait(100);                      \
    }                                           \
    QVERIFY(a)

QTM_USE_NAMESPACE
Q_DECLARE_METATYPE(QServiceFilter);
Q_DECLARE_METATYPE(QVariant);
Q_DECLARE_METATYPE(QList<QString>);
Q_DECLARE_METATYPE(QVariantHash);

class tst_QServiceManager_IPC: public QObject
{
    Q_OBJECT
public:

protected slots:
    void ipcError(QService::UnrecoverableIPCError error);
    
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    void verifySharedServiceObject(); //rough count
    void verifySharedMethods();
    void verifySharedMethods_data();
    void verifySharedProperties();
    void verifySharedProperties_data();

    void verifyUniqueServiceObject(); //rough count
    void verifyUniqueMethods();
    void verifyUniqueMethods_data();

    void verifyUniqueProperties();
    void verifyUniqueProperties_data();

    void verifyUniqueClassInfo();
    void verifyUniqueClassInfo_data();

    void verifyUniqueEnumerator();
    void verifyUniqueEnumerator_data();

    void sharedTestService();
    void uniqueTestService();
    
    void testInvokableFunctions();
    void testSlotInvokation();
    void testSignalling();

    void verifyServiceClass();
    void verifyFailures();


    void testIpcFailure();


private:
    QObject* serviceUnique;
    QObject* serviceUniqueOther;
    QObject* serviceShared;
    QObject* serviceSharedOther;
    QObject* miscTest;
    QServiceManager* manager;
    bool verbose;
    bool ipcfailure;
};

void tst_QServiceManager_IPC::initTestCase()
{
    //verbose = true;
    ipcfailure = false;
    verbose = false;
    serviceUnique = 0;
    serviceUniqueOther = 0;
    serviceSharedOther = 0;
    serviceShared = 0;
    serviceSharedOther = 0;
    miscTest = 0;
    qRegisterMetaType<QServiceFilter>("QServiceFilter");
    qRegisterMetaTypeStreamOperators<QServiceFilter>("QServiceFilter");
    qRegisterMetaType<QList<QString> >("QList<QString>");
    qRegisterMetaTypeStreamOperators<QList<QString> >("QList<QString>");
    qRegisterMetaTypeStreamOperators<QVariantHash>("QVariantHash");
    qRegisterMetaType<QVariantHash>("QVariantHash");
#ifndef QT_NO_DBUS
    qDBusRegisterMetaType<QVariantHash>();
#endif


    QServiceManager* manager = new QServiceManager(this);

    // Symbian has auto registration
#ifndef Q_OS_SYMBIAN
    const QString path = QCoreApplication::applicationDirPath() + "/xmldata/ipcexampleservice.xml";    
    bool r = manager->addService(path);
    if (!r)
        qWarning() << "Cannot register IPCExampleService" << path;
#endif

    // D-Bus auto registration
#ifndef QT_NO_DBUS
    const QString &file = QDir::homePath() + "/.local/share/dbus-1/services/" +
                                             "com.nokia.qt.ipcunittest.service";
    QFile data(file);
    if (data.open(QFile::WriteOnly)) {
        QTextStream out(&data);
        out << "[D-BUS Service]\n"
            << "Name=com.nokia.qtmobility.sfw.IPCExampleService" << '\n'
            << "Exec=" << QFileInfo("./qt_sfw_example_ipc_unittest").absoluteFilePath();
        data.close();
    }
#endif

    //test that the service is installed
    QList<QServiceInterfaceDescriptor> list = manager->findInterfaces("IPCExampleService");
    QVERIFY2(list.count() == 5,"unit test specific IPCExampleService not registered/found" );
    QServiceInterfaceDescriptor d;
    foreach(d, list){
        if(d.majorVersion() == 3 && d.minorVersion() == 5){
            serviceUnique = manager->loadInterface(d);
            serviceUniqueOther = manager->loadInterface(d);
        }
        if(d.majorVersion() == 3 && d.minorVersion() == 4){
            serviceShared = manager->loadInterface(d);
            serviceSharedOther = manager->loadInterface(d);
        }
        if(d.majorVersion() == 3 && d.minorVersion() == 8){
            miscTest = manager->loadInterface(d);
        }

    }

    QString errorCode = "Cannot find service. Error: %1";
    errorCode = errorCode.arg(manager->error());
    QVERIFY2(serviceUnique,errorCode.toLatin1());
    QVERIFY2(serviceUniqueOther,errorCode.toLatin1());
    QVERIFY2(serviceShared,errorCode.toLatin1());
    QVERIFY2(serviceSharedOther,errorCode.toLatin1());

    // all objects come from the same service, just need to connect to 1 signal
    connect(serviceSharedOther, SIGNAL(errorUnrecoverableIPCFault(QService::UnrecoverableIPCError)), this, SLOT(ipcError(QService::UnrecoverableIPCError)));
}

void tst_QServiceManager_IPC::ipcError(QService::UnrecoverableIPCError err)
{  
    Q_UNUSED(err);
    ipcfailure = true;  
}

void tst_QServiceManager_IPC::cleanupTestCase()
{
#ifndef QT_NO_DBUS
    const QString &file = QDir::homePath() + "/.local/share/dbus-1/services/" +
                                             "com.nokia.qt.ipcunittest.service";
    QFile::remove(file);
#endif

    if (serviceUnique) {
        delete serviceUnique;
    }

    if (serviceUniqueOther) {
        delete serviceUniqueOther;
    }

    if (serviceShared) {
        delete serviceShared;
    }

    if (serviceSharedOther) {
        delete serviceSharedOther;
    }

    // clean up the unit, don't leave it registered
    QServiceManager m;
    m.removeService("IPCExampleService");
}

void tst_QServiceManager_IPC::init()
{
}

void tst_QServiceManager_IPC::cleanup()
{
}

void tst_QServiceManager_IPC::sharedTestService()
{
    const QMetaObject *meta = serviceShared->metaObject(); 
    const QMetaObject *metaOther = serviceSharedOther->metaObject(); 

    // test changes on the property 'value'
    QMetaProperty prop = meta->property(1);
    QMetaProperty propOther = metaOther->property(1);

    // check that the property values are the same before and after an edit
    QCOMPARE(prop.read(serviceShared), propOther.read(serviceSharedOther));
    prop.write(serviceShared, "Shared");
    QCOMPARE(prop.read(serviceShared), propOther.read(serviceSharedOther));
    prop.reset(serviceShared);
    
    // test changes through a method call
    uint hash, hashOther;
    QMetaObject::invokeMethod(serviceShared, "setConfirmationHash", 
                              Q_ARG(uint, 0));
    QMetaObject::invokeMethod(serviceShared, "slotConfirmation", 
                              Q_RETURN_ARG(uint, hash));
    QMetaObject::invokeMethod(serviceSharedOther, "setConfirmationHash", 
                              Q_ARG(uint, 0));
    QMetaObject::invokeMethod(serviceSharedOther, "slotConfirmation", 
                              Q_RETURN_ARG(uint, hashOther));
    QCOMPARE(hash, (uint)0);
    QCOMPARE(hashOther, (uint)0);
    
    // check that the method values are the same after an edit
    QMetaObject::invokeMethod(serviceShared, "setConfirmationHash", 
                              Q_ARG(uint, 1));
    QMetaObject::invokeMethod(serviceShared, "slotConfirmation", 
                              Q_RETURN_ARG(uint, hash));
    QMetaObject::invokeMethod(serviceSharedOther, "slotConfirmation", 
                              Q_RETURN_ARG(uint, hashOther));
    QCOMPARE(hash, (uint)1);
    QCOMPARE(hashOther, (uint)1);
}

void tst_QServiceManager_IPC::uniqueTestService()
{
    const QMetaObject *meta = serviceUnique->metaObject(); 
    const QMetaObject *metaOther = serviceUniqueOther->metaObject(); 

    // test changes on the property 'value'
    QMetaProperty prop = meta->property(1);
    QMetaProperty propOther = metaOther->property(1);

    // check that the property values are not the same after an edit
    QCOMPARE(prop.read(serviceUnique), propOther.read(serviceUniqueOther));
    prop.write(serviceUnique, "Unique");
    QVERIFY(prop.read(serviceUnique) != propOther.read(serviceUniqueOther));
    prop.reset(serviceUnique);

    // test changes through a method call
    uint hash, hashOther;
    QMetaObject::invokeMethod(serviceUnique, "setConfirmationHash", 
                              Q_ARG(uint, 0));
    QMetaObject::invokeMethod(serviceUnique, "slotConfirmation", 
                              Q_RETURN_ARG(uint, hash));
    QMetaObject::invokeMethod(serviceUniqueOther, "setConfirmationHash", 
                              Q_ARG(uint, 0));
    QMetaObject::invokeMethod(serviceUniqueOther, "slotConfirmation", 
                              Q_RETURN_ARG(uint, hashOther));
    QCOMPARE(hash, (uint)0);
    QCOMPARE(hashOther, (uint)0);
    
    // check that the method values are not the same after an edit
    QMetaObject::invokeMethod(serviceUnique, "setConfirmationHash", 
                              Q_ARG(uint, 1));
    QMetaObject::invokeMethod(serviceUnique, "slotConfirmation", 
                              Q_RETURN_ARG(uint, hash));
    QMetaObject::invokeMethod(serviceUniqueOther, "slotConfirmation", 
                              Q_RETURN_ARG(uint, hashOther));
    QCOMPARE(hash, (uint)1);
    QCOMPARE(hashOther, (uint)0);
}

void tst_QServiceManager_IPC::verifySharedServiceObject()
{
    QVERIFY(serviceShared != 0);
    const QMetaObject* mo = serviceShared->metaObject();
    QCOMPARE(mo->className(), "SharedTestService");
    QVERIFY(mo->superClass());
    QCOMPARE(mo->superClass()->className(), "QObject");
    QCOMPARE(mo->methodCount()-mo-> methodOffset(), 19);
    QCOMPARE(mo->methodCount(), 23); //20 meta functions available
    //actual function presence will be tested later
    
//    for(int i = 0; i < mo->methodCount(); i++){
//        qDebug() << "Methods" << i << mo->method(i).signature();
//    }
    
    //test properties
    QCOMPARE(mo->propertyCount()-mo->propertyOffset(), 1);
    QCOMPARE(mo->propertyCount(), 2);
    
    QCOMPARE(mo->enumeratorCount()-mo->enumeratorOffset(), 0);
    QCOMPARE(mo->enumeratorCount(), 0);
    
    QCOMPARE(mo->classInfoCount()-mo->classInfoOffset(), 0);
    QCOMPARE(mo->classInfoCount(), 0);
    
    if (verbose) {
        qDebug() << "ServiceObject class: " << mo->className() << mo->superClass() << mo->superClass()->className();
        qDebug() << "------------------- Meta Methods -----------";
        qDebug() << "Methods:" << mo->methodCount()- mo->methodOffset() << "(" << mo->methodCount() << ")";
        for (int i=0; i< mo->methodCount(); i++) {
            QMetaMethod method = mo->method(i);
            QString type;
            switch(method.methodType()) {
            case QMetaMethod::Signal:
                type = "signal"; break;
            case QMetaMethod::Slot:
                type = "slot"; break;
            case QMetaMethod::Constructor:
                type = "constrcutor"; break;
            case QMetaMethod::Method:
                type = "method"; break;
            }
            qDebug() << "    " << i << "." << method.signature() << type;
        }

        qDebug() << "------------------- Meta Properties --------";
        qDebug() << "Properties:" << mo->propertyCount()- mo->propertyOffset() << "(" << mo->propertyCount() << ")";
        for(int i=0; i< mo->propertyCount(); i++) {
            QMetaProperty property = mo->property(i);
            QString info = "Readable: %1 Resettable: %2 Writeable: %3 Designable: %4 Scriptable: %5 User: %6 Stored: %7 Constant: %8 Final: %9 HasNotify: %10 EnumType: %11 FlagType: %12";
            info = info.arg(property.isReadable()).arg(property.isResettable()).arg(property.isWritable());
            info = info.arg(property.isDesignable()).arg(property.isScriptable()).arg(property.isUser());
            info = info.arg(property.isStored()).arg(property.isConstant()).arg(property.isFinal());
            info = info.arg(property.hasNotifySignal()).arg(property.isEnumType()).arg(property.isFlagType());

            qDebug() << "    " << i << "." << property.name() << "Type:" << property.typeName() << info;
        }

        qDebug() << "------------------- Meta Enumerators --------";
        qDebug() << "Enums:" << mo->enumeratorCount()- mo->enumeratorOffset() << "(" << mo->enumeratorCount() << ")";

        qDebug() << "------------------- Meta class info ---------";
        qDebug() << "ClassInfos:" << mo->classInfoCount()- mo->classInfoOffset() << "(" << mo->classInfoCount() << ")";
    }
}

void tst_QServiceManager_IPC::verifySharedMethods_data()
{
    QTest::addColumn<QByteArray>("signature");
    QTest::addColumn<int>("metaMethodType");
    QTest::addColumn<QByteArray>("returnType");

    //list of all slots, signals and invokable functions
    QTest::newRow("signalWithIntParam(int)") 
        << QByteArray("signalWithIntParam(int)") <<  (int)( QMetaMethod::Signal) << QByteArray("");
    QTest::newRow("signalWithVariousParam(QVariant,QString,QServiceFilter)") 
        << QByteArray("signalWithVariousParam(QVariant,QString,QServiceFilter)") <<  (int)( QMetaMethod::Signal) << QByteArray("");
    QTest::newRow("valueChanged()") 
        << QByteArray("valueChanged()") <<  (int)( QMetaMethod::Signal) << QByteArray("");
    QTest::newRow("triggerSignalWithIntParam()") 
        << QByteArray("triggerSignalWithIntParam()") <<  (int)( QMetaMethod::Slot) << QByteArray("");
    QTest::newRow("triggerSignalWithVariousParam()") 
        << QByteArray("triggerSignalWithVariousParam()") <<  (int)( QMetaMethod::Slot) << QByteArray("");
    QTest::newRow("triggerSignalWithIntParamExecute()") 
        << QByteArray("triggerSignalWithIntParamExecute()") <<  (int)( QMetaMethod::Slot) << QByteArray("");
    QTest::newRow("triggerSignalWithVariousParamExecute()") 
        << QByteArray("triggerSignalWithVariousParamExecute()") <<  (int)( QMetaMethod::Slot) << QByteArray("");
    QTest::newRow("testSlot()") 
        << QByteArray("testSlot()") <<  (int)( QMetaMethod::Slot) << QByteArray("");
    QTest::newRow("testSlotWithArgs(QByteArray,int,QVariant)") 
        << QByteArray("testSlotWithArgs(QByteArray,int,QVariant)") <<  (int)( QMetaMethod::Slot) << QByteArray("");
    QTest::newRow("testSlotWithCustomArg(QServiceFilter)") 
        << QByteArray("testSlotWithCustomArg(QServiceFilter)") <<  (int)( QMetaMethod::Slot) << QByteArray("");
    QTest::newRow("testSlotWidthComplexArg(QVariantHash)")
            << QByteArray("testSlotWithComplexArg(QVariantHash)") <<  (int)( QMetaMethod::Slot) << QByteArray("");

    //QServiceInterfaceDescriptor has not been declared as meta type
    QTest::newRow("testSlotWithUnknownArg(QServiceInterfaceDescriptor)") 
        << QByteArray("testSlotWithUnknownArg(QServiceInterfaceDescriptor)") <<  (int)( QMetaMethod::Slot) << QByteArray("");
    QTest::newRow("testFunctionWithReturnValue(int)") 
        << QByteArray("testFunctionWithReturnValue(int)") <<  (int)( QMetaMethod::Method) << QByteArray("QString");
    QTest::newRow("testFunctionWithVariantReturnValue(QVariant)") 
        << QByteArray("testFunctionWithVariantReturnValue(QVariant)") <<  (int)( QMetaMethod::Method) << QByteArray("QVariant");
    QTest::newRow("testFunctionWithCustomReturnValue()") 
        << QByteArray("testFunctionWithCustomReturnValue()") <<  (int)( QMetaMethod::Method) << QByteArray("QServiceFilter");
    QTest::newRow("slotConfirmation()")
        << QByteArray("slotConfirmation()") <<  (int)( QMetaMethod::Method) << QByteArray("uint");
    QTest::newRow("setConfirmationHash(uint)")
        << QByteArray("setConfirmationHash(uint)") <<  (int)( QMetaMethod::Method) << QByteArray("");
}

void tst_QServiceManager_IPC::verifySharedMethods()
{
    QVERIFY(serviceShared);
    QFETCH(QByteArray, signature);
    QFETCH(int, metaMethodType);
    QFETCH(QByteArray, returnType);

    const QMetaObject* meta = serviceShared->metaObject();
    const int index = meta->indexOfMethod(signature.constData());
    QVERIFY(index>=0);
    QMetaMethod method = meta->method(index);
    QCOMPARE(metaMethodType, (int)method.methodType());
    QCOMPARE(returnType, QByteArray(method.typeName()));
}

void tst_QServiceManager_IPC::verifySharedProperties_data()
{
    QTest::addColumn<QByteArray>("signature");
    QTest::addColumn<QString>("typeName");
    QTest::addColumn<qint16>("flags");
    QTest::addColumn<QVariant>("defaultValue");
    QTest::addColumn<QVariant>("writeValue");
    QTest::addColumn<QVariant>("expectedReturnValue");

    //
    //    bit order:
    //        0 isWritable - 1 isUser - 2 isStored - 3 isScriptable 
    //        4 isResettable - 5 isReadable - 6 isFlagType - 7 isFinal
    //        8 isEnumType - 9 isDesignable - 10 isConstant - 11 hasNotifySgnal
    //
    //        for more details see verifyProperties()
    //

    QTest::newRow("value property") << QByteArray("value")
            << QString("QString") << qint16(0x0A3D) << QVariant(QString("FFF")) << QVariant(QString("GGG")) << QVariant(QString("GGG"));
}

void tst_QServiceManager_IPC::verifySharedProperties()
{
    QVERIFY(serviceShared);
    QFETCH(QByteArray, signature);
    QFETCH(QString, typeName);
    QFETCH(qint16, flags);
    QFETCH(QVariant, defaultValue);
    QFETCH(QVariant, writeValue);
    QFETCH(QVariant, expectedReturnValue);

    const QMetaObject* meta = serviceShared->metaObject();
    const int index = meta->indexOfProperty(signature.constData());
    QVERIFY(index>=0);
    QMetaProperty property = meta->property(index);
    QVERIFY(property.isValid());
    QCOMPARE(QString(property.typeName()), typeName);

    QVERIFY( property.isWritable() == (bool) (flags & 0x0001) );
    QVERIFY( property.isUser() == (bool) (flags & 0x0002) );
    QVERIFY( property.isStored() == (bool) (flags & 0x0004) );
    QVERIFY( property.isScriptable() == (bool) (flags & 0x0008) );
    QVERIFY( property.isResettable() == (bool) (flags & 0x0010) );
    QVERIFY( property.isReadable() == (bool) (flags & 0x0020) );
    QVERIFY( property.isFlagType() == (bool) (flags & 0x0040) );
    QVERIFY( property.isFinal() == (bool) (flags & 0x0080) );
    QVERIFY( property.isEnumType() == (bool) (flags & 0x0100) );
    QVERIFY( property.isDesignable() == (bool) (flags & 0x0200) );
    QVERIFY( property.isConstant() == (bool) (flags & 0x0400) );
    QVERIFY( property.hasNotifySignal() == (bool) (flags & 0x0800) );

    if (property.isReadable()) {
        QCOMPARE(defaultValue, serviceShared->property(signature));
        if (property.isWritable()) {
            serviceShared->setProperty(signature, writeValue);
            QCOMPARE(expectedReturnValue, serviceShared->property(signature));
            if (property.isResettable()) {
                property.reset(serviceShared);
                QCOMPARE(defaultValue, serviceShared->property(signature));
            }
            serviceShared->setProperty(signature, defaultValue);
            QCOMPARE(defaultValue, serviceShared->property(signature));
        }
    }
}

void tst_QServiceManager_IPC::verifyUniqueServiceObject()
{
    QVERIFY(serviceUnique != 0);
    const QMetaObject* mo = serviceUnique->metaObject();
    QCOMPARE(mo->className(), "UniqueTestService");
    QVERIFY(mo->superClass());
    QCOMPARE(mo->superClass()->className(), "QObject");
    // TODO adding the ipc failure signal seems to break these    
    QCOMPARE(mo->methodCount()-mo-> methodOffset(), 22); // 17+1 added signal for error signal added by library
    QCOMPARE(mo->methodCount(), 26); //21 meta functions available + 1 signal
    //actual function presence will be tested later
    
//    for(int i = 0; i < mo->methodCount(); i++){
//        qDebug() << "Methods" << i << mo->method(i).signature();
//    }

    //test properties
    QCOMPARE(mo->propertyCount()-mo->propertyOffset(), 3);
    QCOMPARE(mo->propertyCount(), 4);

    QCOMPARE(mo->enumeratorCount()-mo->enumeratorOffset(), 3);
    QCOMPARE(mo->enumeratorCount(), 3);
    
    QCOMPARE(mo->classInfoCount()-mo->classInfoOffset(), 2);
    QCOMPARE(mo->classInfoCount(), 2);
    
    if (verbose) {
        qDebug() << "ServiceObject class: " << mo->className() << mo->superClass() << mo->superClass()->className();
        qDebug() << "------------------- Meta Methods -----------";
        qDebug() << "Methods:" << mo->methodCount()- mo->methodOffset() << "(" << mo->methodCount() << ")";
        for (int i=0; i< mo->methodCount(); i++) {
            QMetaMethod method = mo->method(i);
            QString type;
            switch(method.methodType()) {
            case QMetaMethod::Signal:
                type = "signal"; break;
            case QMetaMethod::Slot:
                type = "slot"; break;
            case QMetaMethod::Constructor:
                type = "constrcutor"; break;
            case QMetaMethod::Method:
                type = "method"; break;
            }
            qDebug() << "    " << i << "." << method.signature() << type;
        }

        qDebug() << "------------------- Meta Properties --------";
        qDebug() << "Properties:" << mo->propertyCount()- mo->propertyOffset() << "(" << mo->propertyCount() << ")";
        for(int i=0; i< mo->propertyCount(); i++) {
            QMetaProperty property = mo->property(i);
            QString info = "Readable: %1 Resettable: %2 Writeable: %3 Designable: %4 Scriptable: %5 User: %6 Stored: %7 Constant: %8 Final: %9 HasNotify: %10 EnumType: %11 FlagType: %12";
            info = info.arg(property.isReadable()).arg(property.isResettable()).arg(property.isWritable());
            info = info.arg(property.isDesignable()).arg(property.isScriptable()).arg(property.isUser());
            info = info.arg(property.isStored()).arg(property.isConstant()).arg(property.isFinal());
            info = info.arg(property.hasNotifySignal()).arg(property.isEnumType()).arg(property.isFlagType());

            qDebug() << "    " << i << "." << property.name() << "Type:" << property.typeName() << info;
        }

        qDebug() << "------------------- Meta Enumerators --------";
        qDebug() << "Enums:" << mo->enumeratorCount()- mo->enumeratorOffset() << "(" << mo->enumeratorCount() << ")";
        for(int i=0; i< mo->enumeratorCount(); i++) {
            QMetaEnum e = mo->enumerator(i);
            qDebug() << "    " << i << "." << e.name() << "Scope:" << e.scope() << "KeyCount: " << e.keyCount();
            for(int j = 0; j<e.keyCount(); j++)
                qDebug() << "         " << e.key(j) << " - " << e.value(j);
        }

        qDebug() << "------------------- Meta class info ---------";
        qDebug() << "ClassInfos:" << mo->classInfoCount()- mo->classInfoOffset() << "(" << mo->classInfoCount() << ")";
        for(int i=0; i< mo->classInfoCount(); i++) {
            QMetaClassInfo info = mo->classInfo(i);
            qDebug() << "    " << i << "." << info.name() << "Value:" << info.value();
        }
    }
}

void tst_QServiceManager_IPC::verifyUniqueMethods_data()
{
    QTest::addColumn<QByteArray>("signature");
    QTest::addColumn<int>("metaMethodType");
    QTest::addColumn<QByteArray>("returnType");

    //list of all slots, signals and invokable functions
    QTest::newRow("signalWithIntParam(int)") 
        << QByteArray("signalWithIntParam(int)") <<  (int)( QMetaMethod::Signal) << QByteArray("");
    QTest::newRow("signalWithVariousParam(QVariant,QString,QServiceFilter,QVariant)") 
        << QByteArray("signalWithVariousParam(QVariant,QString,QServiceFilter,QVariant)") <<  (int)( QMetaMethod::Signal) << QByteArray("");
    QTest::newRow("valueChanged()") 
        << QByteArray("valueChanged()") <<  (int)( QMetaMethod::Signal) << QByteArray("");
    QTest::newRow("priorityChanged()") 
        << QByteArray("priorityChanged()") <<  (int)( QMetaMethod::Signal) << QByteArray("");
    QTest::newRow("triggerSignalWithIntParam()") 
        << QByteArray("triggerSignalWithIntParam()") <<  (int)( QMetaMethod::Slot) << QByteArray("");
    QTest::newRow("triggerSignalWithVariousParam()") 
        << QByteArray("triggerSignalWithVariousParam()") <<  (int)( QMetaMethod::Slot) << QByteArray("");
    QTest::newRow("triggerSignalWithIntParamExecute()") 
        << QByteArray("triggerSignalWithIntParamExecute()") <<  (int)( QMetaMethod::Slot) << QByteArray("");
    QTest::newRow("triggerSignalWithVariousParamExecute()") 
        << QByteArray("triggerSignalWithVariousParamExecute()") <<  (int)( QMetaMethod::Slot) << QByteArray("");
    QTest::newRow("testSlot()") 
        << QByteArray("testSlot()") <<  (int)( QMetaMethod::Slot) << QByteArray("");
    QTest::newRow("testSlotWithArgs(QByteArray,int,QVariant)") 
        << QByteArray("testSlotWithArgs(QByteArray,int,QVariant)") <<  (int)( QMetaMethod::Slot) << QByteArray("");
    QTest::newRow("testSlotWithCustomArg(QServiceFilter)") 
        << QByteArray("testSlotWithCustomArg(QServiceFilter)") <<  (int)( QMetaMethod::Slot) << QByteArray("");

    //QServiceInterfaceDescriptor has not been declared as meta type
    QTest::newRow("testSlotWithUnknownArg(QServiceInterfaceDescriptor)") 
        << QByteArray("testSlotWithUnknownArg(QServiceInterfaceDescriptor)") <<  (int)( QMetaMethod::Slot) << QByteArray("");
    QTest::newRow("testFunctionWithReturnValue(int)") 
        << QByteArray("testFunctionWithReturnValue(int)") <<  (int)( QMetaMethod::Method) << QByteArray("QString");
    QTest::newRow("testFunctionWithVariantReturnValue(QVariant)") 
        << QByteArray("testFunctionWithVariantReturnValue(QVariant)") <<  (int)( QMetaMethod::Method) << QByteArray("QVariant");
    QTest::newRow("testFunctionWithCustomReturnValue()") 
        << QByteArray("testFunctionWithCustomReturnValue()") <<  (int)( QMetaMethod::Method) << QByteArray("QServiceFilter");
    QTest::newRow("slotConfirmation()")
        << QByteArray("slotConfirmation()") <<  (int)( QMetaMethod::Method) << QByteArray("uint");
    QTest::newRow("setConfirmationHash(uint)")
        << QByteArray("setConfirmationHash(uint)") <<  (int)( QMetaMethod::Method) << QByteArray("");
}

void tst_QServiceManager_IPC::verifyUniqueMethods()
{
    QVERIFY(serviceUnique);
    QFETCH(QByteArray, signature);
    QFETCH(int, metaMethodType);
    QFETCH(QByteArray, returnType);

    const QMetaObject* meta = serviceUnique->metaObject();
    const int index = meta->indexOfMethod(signature.constData());
    QVERIFY(index>=0);
    QMetaMethod method = meta->method(index);
    QCOMPARE(metaMethodType, (int)method.methodType());
    QCOMPARE(returnType, QByteArray(method.typeName()));
}

void tst_QServiceManager_IPC::verifyUniqueProperties_data()
{
    QTest::addColumn<QByteArray>("signature");
    QTest::addColumn<QString>("typeName");
    QTest::addColumn<qint16>("flags");
    QTest::addColumn<QVariant>("defaultValue");
    QTest::addColumn<QVariant>("writeValue");
    QTest::addColumn<QVariant>("expectedReturnValue");

    //
    //    bit order:
    //        0 isWritable - 1 isUser - 2 isStored - 3 isScriptable 
    //        4 isResettable - 5 isReadable - 6 isFlagType - 7 isFinal
    //        8 isEnumType - 9 isDesignable - 10 isConstant - 11 hasNotifySgnal
    //
    //        for more details see verifyProperties()
    //

    QTest::newRow("value property") << QByteArray("value")
            << QString("QString") << qint16(0x0A3D) << QVariant(QString("FFF")) << QVariant(QString("GGG")) << QVariant(QString("GGG"));
#ifdef QT_NO_DBUS 
    // ENUMS DONT WORK OVER DBUS
    QTest::newRow("priority property") << QByteArray("priority")
            << QString("Priority") << qint16(0x0B2D) << QVariant((int)0) << QVariant("Low") << QVariant((int)1) ;
    QTest::newRow("serviceFlags property") << QByteArray("serviceFlags")
            << QString("ServiceFlag") << qint16(0x036D) << QVariant((int)4) << QVariant("FirstBit|ThirdBit") << QVariant((int)5);
#endif    
}

void tst_QServiceManager_IPC::verifyUniqueProperties()
{
    QVERIFY(serviceUnique);
    QFETCH(QByteArray, signature);
    QFETCH(QString, typeName);
    QFETCH(qint16, flags);
    QFETCH(QVariant, defaultValue);
    QFETCH(QVariant, writeValue);
    QFETCH(QVariant, expectedReturnValue);

    const QMetaObject* meta = serviceUnique->metaObject();
    const int index = meta->indexOfProperty(signature.constData());
    QVERIFY(index>=0);
    QMetaProperty property = meta->property(index);
    QVERIFY(property.isValid());
    QCOMPARE(QString(property.typeName()), typeName);

    QVERIFY( property.isWritable() == (bool) (flags & 0x0001) );
    QVERIFY( property.isUser() == (bool) (flags & 0x0002) );
    QVERIFY( property.isStored() == (bool) (flags & 0x0004) );
    QVERIFY( property.isScriptable() == (bool) (flags & 0x0008) );
    QVERIFY( property.isResettable() == (bool) (flags & 0x0010) );
    QVERIFY( property.isReadable() == (bool) (flags & 0x0020) );
    QVERIFY( property.isFlagType() == (bool) (flags & 0x0040) );
    QVERIFY( property.isFinal() == (bool) (flags & 0x0080) );
    QVERIFY( property.isEnumType() == (bool) (flags & 0x0100) );
    QVERIFY( property.isDesignable() == (bool) (flags & 0x0200) );
    QVERIFY( property.isConstant() == (bool) (flags & 0x0400) );
    QVERIFY( property.hasNotifySignal() == (bool) (flags & 0x0800) );

    if (property.isReadable()) {
        QCOMPARE(defaultValue, serviceUnique->property(signature));
        if (property.isWritable()) {
            serviceUnique->setProperty(signature, writeValue);
            QCOMPARE(expectedReturnValue, serviceUnique->property(signature));
            if (property.isResettable()) {
                property.reset(serviceUnique);
                QCOMPARE(defaultValue, serviceUnique->property(signature));
            }
            serviceUnique->setProperty(signature, defaultValue);
            QCOMPARE(defaultValue, serviceUnique->property(signature));
        }
    }
}

void tst_QServiceManager_IPC::verifyUniqueClassInfo_data()
{
    QTest::addColumn<QString>("classInfoKey");
    QTest::addColumn<QString>("classInfoValue");

    QTest::newRow("UniqueTestService") << QString("UniqueTestService") << QString("First test");
    QTest::newRow("Key") << QString("Key") << QString("Value");

}
void tst_QServiceManager_IPC::verifyUniqueClassInfo()
{
    QFETCH(QString, classInfoKey);
    QFETCH(QString, classInfoValue);

    const QMetaObject* meta = serviceUnique->metaObject();
    const int index = meta->indexOfClassInfo(classInfoKey.toAscii().constData());

    QMetaClassInfo info = meta->classInfo(index);
    QCOMPARE(classInfoKey, QString(info.name()));
    QCOMPARE(classInfoValue, QString(info.value()));

}

Q_DECLARE_METATYPE(QList<int> )
void tst_QServiceManager_IPC::verifyUniqueEnumerator_data()
{
    QTest::addColumn<QString>("enumName");
    QTest::addColumn<QString>("enumScope");
    QTest::addColumn<int>("enumKeyCount");
    QTest::addColumn<bool>("enumIsFlag");
    QTest::addColumn<QStringList>("enumKeyNames");
    QTest::addColumn<QList<int> >("enumKeyValues");


    QStringList keynames;
    keynames << "FirstBit" << "SecondBit" << "ThirdBit";
    QList<int> values;
    values << 1 << 2 << 4;
    QTest::newRow("ServiceFlag") << QString("ServiceFlag") << QString("UniqueTestService")
        << 3 << true << keynames << values;
    QTest::newRow("ServiceFlags") << QString("ServiceFlags") << QString("UniqueTestService")
        << 3 << true << keynames << values;

    keynames.clear();
    values.clear();

    keynames << "High"  << "Low" << "VeryLow" << "ExtremelyLow";
    values << 0 << 1 << 2 << 3 ;

    QTest::newRow("Priority") << QString("Priority") << QString("UniqueTestService")
        << 4 << false << keynames << values;
}

void tst_QServiceManager_IPC::verifyUniqueEnumerator()
{
    QFETCH(QString, enumName);
    QFETCH(QString, enumScope);
    QFETCH(int, enumKeyCount);
    QFETCH(bool, enumIsFlag);
    QFETCH(QStringList, enumKeyNames);
    QFETCH(QList<int>, enumKeyValues);

    const QMetaObject* meta = serviceUnique->metaObject();
    const int index = meta->indexOfEnumerator(enumName.toAscii().constData());
    QMetaEnum metaEnum = meta->enumerator(index);

    QVERIFY(metaEnum.isValid());
    QCOMPARE(enumScope, QString(metaEnum.scope()));
    QCOMPARE(enumKeyCount, metaEnum.keyCount());
    QCOMPARE(enumIsFlag, metaEnum.isFlag());
    QCOMPARE(enumKeyNames.count(), enumKeyValues.count());

    for (int i=0; i<enumKeyNames.count(); i++) {
        QCOMPARE(enumKeyNames.at(i), QString(metaEnum.valueToKey(enumKeyValues.at(i))));
        QCOMPARE(enumKeyValues.at(i), metaEnum.keyToValue(enumKeyNames.at(i).toAscii().constData()));
    }
}

void tst_QServiceManager_IPC::testInvokableFunctions()
{
    // Test invokable method input with calculated return value
    QString temp;
    QString result;
    QString patternShared("%1 + 3 = %2");
    QString patternUnique("%1 x 3 = %2");
    for (int i = -10; i<10; i++) {
        result.clear(); temp.clear();
        QVERIFY(result.isEmpty());
        QVERIFY(temp.isEmpty());
    
        // Shared service
        QMetaObject::invokeMethod(serviceShared, "testFunctionWithReturnValue", 
                                  Q_RETURN_ARG(QString, result),
                                  Q_ARG(int, i));
        temp = patternShared.arg(i).arg(i+3);
        QCOMPARE(temp, result);

        result.clear(); temp.clear();
        QVERIFY(result.isEmpty());
        QVERIFY(temp.isEmpty());

        // Unique service
        QMetaObject::invokeMethod(serviceUnique, "testFunctionWithReturnValue", 
                                  Q_RETURN_ARG(QString, result), 
                                  Q_ARG(int, i));
        temp = patternUnique.arg(i).arg(i*3);
        QCOMPARE(temp, result);
    }

    // Test invokable method with QVariant return type
    QList<QVariant> variants;
    variants << QVariant("CANT BE NULL") << QVariant(6) << QVariant(QString("testString"));
    for(int i = 0; i<variants.count(); i++) {
        QVariant varResult;
        
        // Shared service
        QMetaObject::invokeMethod(serviceShared, "testFunctionWithVariantReturnValue",
                                  Q_RETURN_ARG(QVariant, varResult),
                                  Q_ARG(QVariant, variants.at(i)));
        QCOMPARE(variants.at(i), varResult);
        
        // Unique service
        QMetaObject::invokeMethod(serviceUnique, "testFunctionWithVariantReturnValue",
                                  Q_RETURN_ARG(QVariant, varResult),
                                  Q_ARG(QVariant, variants.at(i)));
        QCOMPARE(variants.at(i), varResult);
    }

    // Test invokable method with custom return type
    QServiceFilter f;
    // Shared service
    QMetaObject::invokeMethod(serviceShared, "testFunctionWithCustomReturnValue",
                              Q_RETURN_ARG(QServiceFilter, f));
    QCOMPARE(f.serviceName(), QString("MySharedService"));
    QCOMPARE(f.interfaceName(), QString("com.nokia.qt.ipcunittest"));
    QCOMPARE(f.majorVersion(), 6);
    QCOMPARE(f.minorVersion(), 2);
   
    // Unique service
    QMetaObject::invokeMethod(serviceUnique, "testFunctionWithCustomReturnValue",
                              Q_RETURN_ARG(QServiceFilter, f));
    QCOMPARE(f.serviceName(), QString("MyUniqueService"));
    QCOMPARE(f.interfaceName(), QString("com.nokia.qt.ipcunittest"));
    QCOMPARE(f.majorVersion(), 6);
    QCOMPARE(f.minorVersion(), 7);
    
    // Test invokable method with list return type
    QList<QString> list, retList;
    list << "1" << "2" << "3";
    QMetaObject::invokeMethod(serviceUnique, "testFunctionWithListReturn",
                              Q_RETURN_ARG(QList<QString>, retList));
    QCOMPARE(list, retList);
}

void tst_QServiceManager_IPC::testSignalling()
{
    // Test signalling for simple methods
    QSignalSpy spy(serviceUnique, SIGNAL(signalWithIntParam(int)));
    QMetaObject::invokeMethod(serviceUnique, "triggerSignalWithIntParam");
    QTRY_VERIFY(spy.count() > 0);
    QCOMPARE(spy.at(0).at(0).toInt(), 5);
    
    // Test signalling for property changes
    QCOMPARE(QString("FFF"), serviceUnique->property("value").toString());
    QSignalSpy propSpy(serviceUnique, SIGNAL(valueChanged()));
    
    serviceUnique->setProperty("value", QString("GGG"));
    QTRY_VERIFY(propSpy.count() == 1);
    propSpy.clear();
    serviceUnique->setProperty("value", QString("FFF"));
    QTRY_VERIFY(propSpy.count() == 1);
    QCOMPARE(QString("FFF"), serviceUnique->property("value").toString());
  
    // Test signalling with custom types
    QSignalSpy variousSpy(serviceUnique, SIGNAL(signalWithVariousParam(QVariant,QString,QServiceFilter,QVariant)));
    QMetaObject::invokeMethod(serviceUnique, "triggerSignalWithVariousParam");
   
    QTRY_VERIFY(variousSpy.count() == 1);
    QCOMPARE(variousSpy.at(0).count(), 4);
    QCOMPARE(variousSpy.at(0).at(0).value<QVariant>(), QVariant("CAN'T BE NULL"));
    QCOMPARE(variousSpy.at(0).at(1).toString(), QString("string-value"));

    QVariant vFilter = variousSpy.at(0).at(2);
    QServiceFilter filter;
    QVERIFY(vFilter.canConvert<QServiceFilter>());
    filter = vFilter.value<QServiceFilter>();
    QCOMPARE(filter.serviceName(), QString("MyService"));
    QCOMPARE(filter.interfaceName(), QString("com.nokia.qt.ipcunittest"));
    QCOMPARE(filter.majorVersion(), 6);
    QCOMPARE(filter.minorVersion(), 7);

    QCOMPARE(variousSpy.at(0).at(3).value<QVariant>(), QVariant(5));
}

void tst_QServiceManager_IPC::testSlotInvokation()
{
    QObject *service = serviceUnique;

    // check the success of our invokable slot by using a hash of the method and its parameters
    uint hash = 1;
    uint expectedHash = 0;
    QMetaObject::invokeMethod(service, "setConfirmationHash",
                              Q_ARG(uint, 0));
    QMetaObject::invokeMethod(service, "slotConfirmation",
                              Q_RETURN_ARG(uint, hash));
    QCOMPARE(hash, (uint)0);

    // Test invokable slot with no arguments
    QMetaObject::invokeMethod(service, "testSlot");
    QMetaObject::invokeMethod(service, "slotConfirmation",
                               Q_RETURN_ARG(uint, hash));
    expectedHash = qHash(QString("testSlot()"));
    QCOMPARE(hash, expectedHash);

    // Test invokable slot with various arguments
    QVariant test("CANT BE NULL");
    QByteArray d = "array";
    int num = 5;
    QString output = QString("%1, %2, %3, %4");
    output = output.arg(d.constData()).arg(num).arg(test.toString()).arg(test.isValid());
    expectedHash = qHash(output);
    QMetaObject::invokeMethod(service, "testSlotWithArgs",
                              Q_ARG(QByteArray, d), Q_ARG(int, num), Q_ARG(QVariant, test));
    QMetaObject::invokeMethod(service, "slotConfirmation",
                              Q_RETURN_ARG(uint, hash));
    QCOMPARE(hash, expectedHash);
    
    // Test failed invokable slot since QServiceInterfaceDescriptor is not a registered meta type
    // by checking that the service didn't set the hash to -1 due to being called
    QServiceInterfaceDescriptor desc;
    QMetaObject::invokeMethod(service, "testSlotWithUnknownArg",
                              Q_ARG(QServiceInterfaceDescriptor, desc));
    QMetaObject::invokeMethod(service, "slotConfirmation",
                              Q_RETURN_ARG(uint, hash));
    QVERIFY(hash != 1);
    
    // Test slot function with custom argument
    QServiceFilter f("com.myInterface" , "4.5");
    f.setServiceName("MyService");
    output = QString("%1: %2 - %3.%4");
    output = output.arg(f.serviceName()).arg(f.interfaceName())
            .arg(f.majorVersion()).arg(f.minorVersion());
    expectedHash = qHash(output); 
    QMetaObject::invokeMethod(service, "testSlotWithCustomArg",
                              Q_ARG(QServiceFilter, f));
    QMetaObject::invokeMethod(service, "slotConfirmation",
                              Q_RETURN_ARG(uint, hash));
    QCOMPARE(hash, expectedHash);
    
    // Test invokable method with a QList argument
    QList<QString> list;
    list << "one" << "two" << "three";
    output = "";
    for (int i=0; i<list.size(); i++) {
        output += list[i];
        if (i<list.size()-1)
            output += ", ";
    }
    expectedHash = qHash(output);
    QMetaObject::invokeMethod(serviceUnique, "testSlotWithListArg",
                              Q_ARG(QList<QString>, list));
    QMetaObject::invokeMethod(service, "slotConfirmation",
                              Q_RETURN_ARG(uint, hash));
    QCOMPARE(hash, expectedHash);

    // Test slot with QHash<QString, QVariant>
    QHash<QString, QVariant> hashv;
    hashv["test"] = QVariant::fromValue(1);
    hashv["test2"] = QVariant::fromValue(QString("test2 string"));
    hashv["test3"] = QVariant::fromValue(true);
    QHashIterator<QString, QVariant> i(hashv);
    output.clear();
    while (i.hasNext()) {
        i.next();
        output += i.key();
        output += "=";
        output += i.value().toString();
        output += ", ";
    }
    expectedHash = qHash(output);

    QMetaObject::invokeMethod(serviceUnique, "testSlotWithComplexArg",
                              Q_ARG(QVariantHash, hashv));
    QMetaObject::invokeMethod(service, "slotConfirmation",
                              Q_RETURN_ARG(uint, hash));
    QCOMPARE(hash, expectedHash);


}

void tst_QServiceManager_IPC::verifyServiceClass()
{
    QRemoteServiceRegister *registerObject = new QRemoteServiceRegister();

    QVERIFY2(registerObject->quitOnLastInstanceClosed() == true, "should default to true, default is to shutdown");
    registerObject->setQuitOnLastInstanceClosed(false);
    QVERIFY2(registerObject->quitOnLastInstanceClosed() == false, "must transition to false");
    registerObject->setQuitOnLastInstanceClosed(true);
    QVERIFY2(registerObject->quitOnLastInstanceClosed() == true, "must transition back to true");

    delete registerObject;
}

void tst_QServiceManager_IPC::testIpcFailure()
    {

    // test deleting an object doesn't trigger an IPC fault
    ipcfailure = false;
    delete serviceShared;
    QVERIFY2(!ipcfailure, "Deleting an object should not cause an IPC failure message");
    serviceShared = 0;

    ipcfailure = false;   
    QMetaObject::invokeMethod(serviceUnique, "testIpcFailure");
    int i = 0;
    while (!ipcfailure && i++ < 50)
        QTest::qWait(50);
    
    QVERIFY(ipcfailure);
  
    // TODO restart the connection
    //initTestCase();
}

void tst_QServiceManager_IPC::verifyFailures()
{
    bool result;

    QServiceManager* manager = new QServiceManager(this);
    QList<QServiceInterfaceDescriptor> list = manager->findInterfaces("IPCExampleService");
    QServiceInterfaceDescriptor d;
    foreach(d, list){
        if(d.majorVersion() == 3 && d.minorVersion() == 6){
            QObject *o = manager->loadInterface(d);
            QVERIFY2(o == 0, "Failure to allocate remote object returns null");
        }
        if(d.majorVersion() == 3 && d.minorVersion() == 7){
            QObject *o = manager->loadInterface(d);
            QVERIFY2(o == 0, "Failure to allocate remote object returns null");
        }
    }

    QMetaObject::invokeMethod(miscTest, "addTwice",
                              Q_RETURN_ARG(bool, result));
    QVERIFY2(result, "Added the same service twice, returned different entries");

    QMetaObject::invokeMethod(miscTest, "getInvalidEntry",
                              Q_RETURN_ARG(bool, result));
    QVERIFY2(result, "Invalid entry returns invalid meta data");

    delete manager;

}



QTEST_MAIN(tst_QServiceManager_IPC);
#include "tst_qservicemanager_ipc.moc"
