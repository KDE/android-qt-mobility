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
#include <qservicemanager.h>
#include <qservicecontext.h>
#include <qabstractsecuritysession.h>
#include <qserviceinterfacedescriptor_p.h>
#include "../../sampleserviceplugin/sampleserviceplugin.h"
#include "../qsfwtestutil.h"

#include <QtTest/QtTest>
#include <QtCore>
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include <QPair>

#define QTRY_COMPARE(a,e)                       \
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
    QVERIFY(a)

#define PRINT_ERR(a) qPrintable(QString("error = %1").arg(a.error()))

typedef QList<QtMobility::QServiceInterfaceDescriptor> ServiceInterfaceDescriptorList;
Q_DECLARE_METATYPE(QtMobility::QServiceFilter)
Q_DECLARE_METATYPE(QtMobility::QServiceInterfaceDescriptor)
Q_DECLARE_METATYPE(ServiceInterfaceDescriptorList)

Q_DECLARE_METATYPE(QSet<QString>)
Q_DECLARE_METATYPE(QList<QByteArray>)
Q_DECLARE_METATYPE(QtMobility::QService::Scope)

QTM_BEGIN_NAMESPACE
typedef QHash<QtMobility::QServiceInterfaceDescriptor::Attribute, QVariant> DescriptorAttributes;

inline uint qHash(const QtMobility::QServiceInterfaceDescriptor &desc)
{
    return qHash(desc.serviceName()) + qHash(desc.interfaceName()) + desc.majorVersion() * 7 + desc.minorVersion() * 7;
}
QTM_END_NAMESPACE

QTM_USE_NAMESPACE
static DescriptorAttributes defaultDescriptorAttributes()
{
    DescriptorAttributes props;
    //props[QServiceInterfaceDescriptor::Capabilities] = QStringList();
    props[QServiceInterfaceDescriptor::Location] = "";
    props[QServiceInterfaceDescriptor::ServiceDescription] = "";
    props[QServiceInterfaceDescriptor::InterfaceDescription] = "";
    return props;
}
static const DescriptorAttributes DEFAULT_DESCRIPTOR_PROPERTIES = defaultDescriptorAttributes();

static QStringList validPluginFiles()
{
    // these are the plugins under tests/ which point to real service plugins
    // that can be used for testing (i.e. plugins that can be loaded, invoked)
    QStringList files;
    files << "plugins/tst_sfw_sampleserviceplugin" << "plugins/tst_sfw_sampleserviceplugin2";
    return files;
}
static const QStringList VALID_PLUGIN_FILES = validPluginFiles();

// Helper function for debugging. Useful e.g. for checking what is difference between
// two descriptors (in addition to attributes printed below, the \
// QServiceInterfaceDescriptor::== operator also compares
// attributes.
static void printDescriptor (QServiceInterfaceDescriptor &desc) {
    qDebug("***QServiceInterfaceDescriptor printed:");
    qDebug() << "***majorVersion:" << desc.majorVersion();
    qDebug() << "***minorVersion:" << desc.minorVersion();
    qDebug() << "***interfaceName:" << desc.interfaceName();
    qDebug() << "***serviceName:" << desc.serviceName();
    qDebug() << "***customAttributes:" << desc.customAttributes();
    qDebug() << "***isValid(): " << desc.isValid();
    qDebug() << "***scope (user:0, system:1): " << desc.scope();
}

class MySecuritySession : public QAbstractSecuritySession
{
public:
    MySecuritySession() {}
    virtual ~MySecuritySession() {}

    virtual bool isAllowed(const QStringList&) { return true; }
};

class MyServiceContext : public QServiceContext
{
public:
    MyServiceContext() {}
    ~MyServiceContext() {}

    virtual void notify(ContextType, const QVariant&) {}
};


class ServicesListener : public QObject
{
    Q_OBJECT
public slots:
    void serviceAdded(const QString &name , QService::Scope scope) {
        params.append(qMakePair(name, scope));
    }
    void serviceRemoved(const QString &name, QService::Scope scope) {
        params.append(qMakePair(name, scope));
    }
public:
    QList<QPair<QString, QService::Scope> > params;
};


class tst_QServiceManager: public QObject
{
    Q_OBJECT

private:
    QString xmlTestDataPath(const QString &xmlFileName)
    {
        // On Symbian applicationDirPath returns application's private directory
        return QCoreApplication::applicationDirPath() + "/plugins/xmldata/" + xmlFileName;
    }

    QByteArray createServiceXml(const QString &serviceName, const QByteArray &interfaceXml, const QString &path, const QString &description = QString()) const
    {
        QString xml = "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n";
        xml += "<service>\n";
        xml += "<name>" + serviceName + "</name>\n";
        xml += "<filepath>" + path + "</filepath>\n";
        xml += "<description>" + description + "</description>\n";
        xml += interfaceXml;
        xml += "</service>\n";
        return xml.toLatin1();
    }

    QByteArray createServiceXml(const QString &serviceName, const QList<QServiceInterfaceDescriptor> &descriptors) const
    {
        Q_ASSERT(descriptors.count() > 0);
        return createServiceXml(serviceName, createInterfaceXml(descriptors),
                descriptors[0].attribute(QServiceInterfaceDescriptor::Location).toString(),
                descriptors[0].attribute(QServiceInterfaceDescriptor::ServiceDescription).toString());
    }

    QByteArray createInterfaceXml(const QList<QServiceInterfaceDescriptor> &descriptors) const
    {
        QByteArray interfacesXml;
        foreach (const QServiceInterfaceDescriptor &desc, descriptors) {
            QString version = QString("%1.%2").arg(desc.majorVersion()).arg(desc.minorVersion());
            interfacesXml += createInterfaceXml(desc.interfaceName(), version,
                    desc.attribute(QServiceInterfaceDescriptor::InterfaceDescription).toString());
        }
        return interfacesXml;
    }

    QByteArray createInterfaceXml(const QString &name, const QString &version = "1.0", const QString &description = QString()) const
    {
       QString xml = "<interface>\n";
        xml += "<name>" + name + "</name>\n";
        xml += "<version>" + version + "</version>\n";
        xml += " <description>" + description + "</description>\n";
        xml += "</interface>\n";
        return xml.toLatin1();

    }

    QServiceInterfaceDescriptor createDescriptor(const QString &interfaceName, int major, int minor, const QString &serviceName, const DescriptorAttributes &attributes = DescriptorAttributes(), QService::Scope scope = QService::UserScope) const
    {
        QString version = QString("%1.%2").arg(major).arg(minor);

        QServiceInterfaceDescriptorPrivate *priv = new QServiceInterfaceDescriptorPrivate;
        priv->serviceName = serviceName;
        priv->interfaceName = interfaceName;
        priv->major = major;
        priv->minor = minor;
        priv->scope = scope;

        priv->attributes = attributes;
        foreach (QServiceInterfaceDescriptor::Attribute key, DEFAULT_DESCRIPTOR_PROPERTIES.keys()) {
            if (!priv->attributes.contains(key))
                priv->attributes[key] = DEFAULT_DESCRIPTOR_PROPERTIES[key];
        }

        QServiceInterfaceDescriptor desc;
        QServiceInterfaceDescriptorPrivate::setPrivate(&desc, priv);
        return desc;
    }

    void deleteTestDatabasesAndWaitUntilDone()
    {
        QSfwTestUtil::removeTempUserDb();
        QSfwTestUtil::removeTempSystemDb();

        QTRY_VERIFY(!QFile::exists(QSfwTestUtil::tempUserDbDir()));
        QTRY_VERIFY(!QFile::exists(QSfwTestUtil::tempSystemDbDir()));
    }

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();

    void constructor();
    void constructor_scope();
    void constructor_scope_data();

    void findServices();
    void findServices_data();

    void findServices_scope();
    void findServices_scope_data();

    void findInterfaces_filter();
    void findInterfaces_filter_data();

    void findInterfaces_scope();
    void findInterfaces_scope_data();

    void loadInterface_string();

    void loadInterface_descriptor();
    void loadInterface_descriptor_data();

    void loadInterface_testLoadedObjectAttributes();

    void loadLocalTypedInterface();

    void addService();
    void addService_data();

    void addService_testInvalidServiceXml();
    void addService_testPluginLoading();
    void addService_testPluginLoading_data();
    void addService_testInstallService();

    void removeService();

    void setInterfaceDefault_strings();
    void setInterfaceDefault_strings_multipleInterfaces();

    void setInterfaceDefault_descriptor();
    void setInterfaceDefault_descriptor_data();

    void interfaceDefault();

    void serviceAdded();
    void serviceAdded_data();

    void serviceRemoved();
    void serviceRemoved_data();
};

void tst_QServiceManager::initTestCase()
{
    qRegisterMetaType<QService::Scope>("QService::Scope");

    QSfwTestUtil::setupTempUserDb();
    QSfwTestUtil::setupTempSystemDb();
#if defined(Q_OS_SYMBIAN)
    QSfwTestUtil::removeDatabases_symbian();
#endif
}

void tst_QServiceManager::init()
{
#if defined(Q_OS_SYMBIAN)
    // Wait a millisecond so that QServiceManagers are destroyed and release
    // the database file (otherwise QFile::remove will get a permission denied -->
    // in next case, the isEmpty() check fails).
    QTest::qWait(1);
#endif
    QSfwTestUtil::removeTempUserDb();
    QSfwTestUtil::removeTempSystemDb();
#if defined(Q_OS_SYMBIAN)
    QSfwTestUtil::removeDatabases_symbian();
#endif
    QSettings settings("com.nokia.qt.serviceframework.tests", "SampleServicePlugin");
    settings.setValue("installed", false);
}

void tst_QServiceManager::cleanupTestCase()
{
    QSfwTestUtil::removeTempUserDb();
    QSfwTestUtil::removeTempSystemDb();
#if defined(Q_OS_SYMBIAN)
    QSfwTestUtil::removeDatabases_symbian();
#endif
    //use QEventLopp::DeferredDeletion
    //QServiceManager::loadInterface makes use of deleteLater() when
    //cleaning up service objects and their respective QPluginLoader
    //we want to force the testcase to run the cleanup code
    QCoreApplication::processEvents(QEventLoop::AllEvents|QEventLoop::DeferredDeletion);
}

void tst_QServiceManager::constructor()
{
    QObject o;
    QServiceManager mgr(&o);
    QCOMPARE(mgr.scope(), QService::UserScope);
    QCOMPARE(mgr.parent(), &o);
}

void tst_QServiceManager::constructor_scope()
{
    QFETCH(QService::Scope, scope);

    QObject o;
    QServiceManager mgr(scope, &o);
    QCOMPARE(mgr.scope(), scope);
    QCOMPARE(mgr.parent(), &o);
}

void tst_QServiceManager::constructor_scope_data()
{
    QTest::addColumn<QService::Scope>("scope");

    QTest::newRow("user") << QService::UserScope;
    QTest::newRow("system") << QService::SystemScope;
}

void tst_QServiceManager::findServices()
{
    QFETCH(QList<QByteArray>, xmlBlocks);
    QFETCH(QStringList, interfaceNames);
    QFETCH(QSet<QString>, searchByInterfaceResult);
    QFETCH(QSet<QString>, searchAllResult);

    QServiceManager mgr;
    QServiceFilter wildcardFilter;

    // Check that nothing is found neither with default search or interface-search
    QVERIFY(mgr.findServices().isEmpty());
    foreach (const QString &interface, interfaceNames)
        QVERIFY(mgr.findServices(interface).isEmpty());
    QCOMPARE(mgr.findInterfaces(wildcardFilter).count(), 0);

    // Add all services from the xmlBlocks list
    foreach (const QByteArray &xml, xmlBlocks) {
        QBuffer buffer;
        buffer.setData(xml);
        QVERIFY2(mgr.addService(&buffer), PRINT_ERR(mgr));
    }
    // Check that all services are found with default search
    QCOMPARE(mgr.findServices().toSet(), searchAllResult);
    // Check that all services are found based on interface search
    foreach (const QString &interface, interfaceNames)
        QCOMPARE(mgr.findServices(interface).toSet(), searchByInterfaceResult);

    // Check that nothing is found with empty interface
    QCOMPARE(mgr.findServices("com.invalid.interface") , QStringList());
}

void tst_QServiceManager::findServices_data()
{
    QTest::addColumn< QList<QByteArray> >("xmlBlocks");
    QTest::addColumn<QStringList>("interfaceNames");
    QTest::addColumn< QSet<QString> >("searchByInterfaceResult");
    QTest::addColumn< QSet<QString> >("searchAllResult");

    QStringList interfaces;
    interfaces << "com.nokia.qt.TestInterfaceA";
    interfaces << "com.nokia.qt.TestInterfaceB";
    QByteArray interfacesXml;
    for (int i=0; i<interfaces.count(); i++)
        interfacesXml += "\n" + createInterfaceXml(interfaces[i]);

    QTest::newRow("one service")
            << (QList<QByteArray>() << createServiceXml("SomeTestService", interfacesXml, VALID_PLUGIN_FILES.first()))
            << interfaces
            << (QSet<QString>() << "SomeTestService")
            << (QSet<QString>() << "SomeTestService");

    QTest::newRow("multiple services with same interfaces")
            << (QList<QByteArray>() << createServiceXml("SomeTestService", interfacesXml, VALID_PLUGIN_FILES[0])
                                    << createServiceXml("SomeSimilarTestService", interfacesXml, VALID_PLUGIN_FILES[1]))
            << interfaces
            << (QSet<QString>() << "SomeTestService" << "SomeSimilarTestService")
            << (QSet<QString>() << "SomeTestService" << "SomeSimilarTestService");

    QStringList interfaces2;
    interfaces2 << "com.nokia.qt.TestInterfaceY";
    interfaces2 << "com.nokia.qt.TestInterfaceZ";
    QByteArray interfacesXml2;
    for (int i=0; i<interfaces2.count(); i++)
        interfacesXml2 += "\n" + createInterfaceXml(interfaces2[i]);
    QTest::newRow("multiple services with different interfaces")
            << (QList<QByteArray>() << createServiceXml("SomeTestService", interfacesXml, VALID_PLUGIN_FILES[0])
                                    << createServiceXml("TestServiceWithOtherInterfaces", interfacesXml2, VALID_PLUGIN_FILES[1]))
            << interfaces2
            << (QSet<QString>() << "TestServiceWithOtherInterfaces")
            << (QSet<QString>() << "SomeTestService" << "TestServiceWithOtherInterfaces");
}

void tst_QServiceManager::findServices_scope()
{
#if defined(Q_OS_SYMBIAN)
    QSKIP("There is no difference between user and system scope in symbian", SkipAll);
#endif
    QFETCH(QService::Scope, scope_add);
    QFETCH(QService::Scope, scope_find);
    QFETCH(bool, expectFound);

    QByteArray xml = createServiceXml("SomeTestService",
            createInterfaceXml("com.nokia.qt.TestInterface"), VALID_PLUGIN_FILES[0]);
    QBuffer buffer(&xml);

    QServiceManager mgrUser(QService::UserScope);
    QServiceManager mgrSystem(QService::SystemScope);

    QServiceManager &mgrAdd = scope_add == QService::UserScope ? mgrUser : mgrSystem;
    QServiceManager &mgrFind = scope_find == QService::UserScope ? mgrUser : mgrSystem;

    QVERIFY2(mgrAdd.addService(&buffer), PRINT_ERR(mgrAdd));
    QStringList result = mgrFind.findServices();
    QCOMPARE(!result.isEmpty(), expectFound);
}

void tst_QServiceManager::findServices_scope_data()
{
    QTest::addColumn<QService::Scope>("scope_add");
    QTest::addColumn<QService::Scope>("scope_find");
    QTest::addColumn<bool>("expectFound");

    QTest::newRow("user scope")
            << QService::UserScope << QService::UserScope << true;
    QTest::newRow("system scope")
            << QService::SystemScope << QService::SystemScope << true;

    QTest::newRow("user scope - add, system scope - find")
            << QService::UserScope << QService::SystemScope << false;
    QTest::newRow("system scope - add, user scope - find")
            << QService::SystemScope << QService::UserScope << true;
}

void tst_QServiceManager::findInterfaces_filter()
{
    QFETCH(QByteArray, xml);
    QFETCH(QServiceFilter, filter);
    QFETCH(QList<QServiceInterfaceDescriptor>, expectedInterfaces);

    QServiceManager mgr;

    QBuffer buffer(&xml);
    QVERIFY2(mgr.addService(&buffer), PRINT_ERR(mgr));

    QList<QServiceInterfaceDescriptor> result = mgr.findInterfaces(filter);
    QCOMPARE(result.toSet(), expectedInterfaces.toSet());
}

void tst_QServiceManager::findInterfaces_filter_data()
{
    QTest::addColumn<QByteArray>("xml");
    QTest::addColumn<QServiceFilter>("filter");
    QTest::addColumn<ServiceInterfaceDescriptorList>("expectedInterfaces");

    QString serviceName = "SomeTestService";
    DescriptorAttributes attributes;
    attributes[QServiceInterfaceDescriptor::Location] = VALID_PLUGIN_FILES.first();

    QList<QServiceInterfaceDescriptor> descriptors;
    descriptors << createDescriptor("com.nokia.qt.TestInterfaceA", 1, 0, serviceName, attributes);
    descriptors << createDescriptor("com.nokia.qt.TestInterfaceB", 1, 0, serviceName, attributes);
    descriptors << createDescriptor("com.nokia.qt.TestInterfaceB", 2, 0, serviceName, attributes);
    descriptors << createDescriptor("com.nokia.qt.TestInterfaceB", 2, 3, serviceName, attributes);

    QByteArray serviceXml = createServiceXml(serviceName, descriptors);
    QServiceFilter filter;

    QTest::newRow("empty/wildcard filter")
            << serviceXml
            << QServiceFilter()
            << descriptors;

    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceA");
    QTest::newRow("by interface name (A)")
            << serviceXml
            << filter
            << descriptors.mid(0, 1);

    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB");
    QTest::newRow("by interface name (B)")
            << serviceXml
            << filter
            << descriptors.mid(1);

    filter = QServiceFilter();
    filter.setServiceName(serviceName);
    QTest::newRow("by service name, should find all")
            << serviceXml
            << filter
            << descriptors;

    filter = QServiceFilter();
    filter.setInterface("com.invalid.interface");
    QTest::newRow("by non-existing interface name")
            << serviceXml
            << filter
            << ServiceInterfaceDescriptorList();

    filter = QServiceFilter();
    filter.setServiceName("InvalidServiceName");
    QTest::newRow("by non-existing service name")
            << serviceXml
            << filter
            << ServiceInterfaceDescriptorList();

    //version lookup testing for existing interface
    //valid from first version onwards
    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "1.0");
    QTest::newRow("by version name 1.0 DefaultMatch, should find all B interfaces")
            << serviceXml
            << filter
            << descriptors.mid(1);

    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "1.0", QServiceFilter::MinimumVersionMatch);
    QTest::newRow("by version name 1.0 MinimumMatch, should find all B interfaces")
            << serviceXml
            << filter
            << descriptors.mid(1);

    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "1.0", QServiceFilter::ExactVersionMatch);
    QTest::newRow("by version name 1.0 ExactMatch, find B 1.0 only")
            << serviceXml
            << filter
            << descriptors.mid(1, 1);

    //valid with exact version match
    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "2.0");
    QTest::newRow("by version name 2.0 DefaultMatch, find B 2.0+")
            << serviceXml
            << filter
            << descriptors.mid(2);

    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "2.0", QServiceFilter::MinimumVersionMatch);
    QTest::newRow("by version name 2.0 MinimumMatch, find B 2.0+")
            << serviceXml
            << filter
            << descriptors.mid(2);

    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "2.0", QServiceFilter::ExactVersionMatch);
    QTest::newRow("by version name 2.0 ExactMatch, find B 2.0")
            << serviceXml
            << filter
            << descriptors.mid(2, 1);

    //valid but not exact version match
    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "1.9");
    QTest::newRow("by version name 1.9 DefaultMatch, find B 1.9+")
            << serviceXml
            << filter
            << descriptors.mid(2);

    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "1.9", QServiceFilter::MinimumVersionMatch);
    QTest::newRow("by version name 1.9 MinimumMatch, find B 1.9+")
            << serviceXml
            << filter
            << descriptors.mid(2);

    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "1.9", QServiceFilter::ExactVersionMatch);
    QTest::newRow("by version name 1.9 ExactMatch")
            << serviceXml
            << filter
            << ServiceInterfaceDescriptorList();

    //version doesn't exist yet
    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "3.9");
    QTest::newRow("by version name 3.9 DefaultMatch")
            << serviceXml
            << filter
            << ServiceInterfaceDescriptorList();

    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "3.9", QServiceFilter::MinimumVersionMatch);
    QTest::newRow("by version name 3.9 MinimumMatch")
            << serviceXml
            << filter
            << ServiceInterfaceDescriptorList();

    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "3.9", QServiceFilter::ExactVersionMatch);
    QTest::newRow("by version name 3.9 ExactMatch")
            << serviceXml
            << filter
            << ServiceInterfaceDescriptorList();

    //invalid version tag 1 -> match anything
    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "x3.9");
    QTest::newRow("by version name x3.9 DefaultMatch")
            << serviceXml<< filter
            << descriptors;

    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "x3.9", QServiceFilter::MinimumVersionMatch);
    QTest::newRow("by version name x3.9 MinimumMatch")
            << serviceXml
            << filter
            << descriptors;

    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "x3.9", QServiceFilter::ExactVersionMatch);
    QTest::newRow("by version name x3.9 ExactMatch")
            << serviceXml
            << filter
            << descriptors;

    //envalid/empty version tag
    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "");
    QTest::newRow("by empty version string DefaultMatch")
            << serviceXml
            << filter
            << descriptors.mid(1);

    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "", QServiceFilter::MinimumVersionMatch);
    QTest::newRow("by empty version string MinimumMatch")
            << serviceXml
            << filter
            << descriptors.mid(1);

    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "", QServiceFilter::ExactVersionMatch); //what's the result of this?
    QTest::newRow("by empty version string ExactMatch")
            << serviceXml
            << filter
            << descriptors.mid(1);

    //invalid version tag 2
    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "abc");
    QTest::newRow("by version name abc DefaultMatch")
            << serviceXml<< filter
            << descriptors;

    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "abc", QServiceFilter::MinimumVersionMatch);
    QTest::newRow("by version name abc MinimumMatch")
            << serviceXml<< filter
            << descriptors;

    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "abc", QServiceFilter::ExactVersionMatch);
    QTest::newRow("by version name abc ExactMatch")
            << serviceXml
            << filter
            << descriptors;
}

void tst_QServiceManager::findInterfaces_scope()
{
#if defined(Q_OS_SYMBIAN)
    QSKIP("There is no difference between user and system scope in symbian", SkipAll);
#endif
    QFETCH(QService::Scope, scope_add);
    QFETCH(QService::Scope, scope_find);
    QFETCH(bool, expectFound);

    QByteArray xml = createServiceXml("SomeTestService",
            createInterfaceXml("com.nokia.qt.TestInterface"), VALID_PLUGIN_FILES[0]);
    QBuffer buffer(&xml);

    QServiceManager mgrUser(QService::UserScope);
    QServiceManager mgrSystem(QService::SystemScope);

    QServiceManager &mgrAdd = scope_add == QService::UserScope ? mgrUser : mgrSystem;
    QServiceManager &mgrFind = scope_find == QService::UserScope ? mgrUser : mgrSystem;

    QList<QServiceInterfaceDescriptor> result = mgrFind.findInterfaces(QString());
    QVERIFY(result.isEmpty());

    QVERIFY2(mgrAdd.addService(&buffer), PRINT_ERR(mgrAdd));
    result = mgrFind.findInterfaces("SomeTestService");
    QCOMPARE(!result.isEmpty(), expectFound);

    result = mgrFind.findInterfaces(QString());
    if (expectFound)
        QVERIFY(result.count() == 1);
    else
        QVERIFY(result.isEmpty());

    result = mgrFind.findInterfaces("NonExistingService");
    QVERIFY(result.isEmpty());
}

void tst_QServiceManager::findInterfaces_scope_data()
{
    findServices_scope_data();
}


void tst_QServiceManager::loadInterface_string()
{
    // The sampleservice.xml and sampleservice2.xml services in
    // tests/sampleserviceplugin and tests/sampleserviceplugin2 implement a
    // common interface, "com.nokia.qt.TestInterfaceA". If both are
    // registered, loadInterface(QString) should return the correct one
    // depending on which is set as the default.

    // Real servicenames and classnames
    QString serviceA = "SampleService";
    QString serviceAClassName = "SampleServicePluginClass";
    QString serviceB = "SampleService2";
    QString serviceBClassName = "SampleServicePluginClass2";

    QObject *obj = 0;
    QServiceManager mgr;
    QString commonInterface = "com.nokia.qt.TestInterfaceA";

    // Add first service. Adds the service described in
    // c/Private/<uid3 of this executable>/plugins/xmldata/sampleservice.xml
    QVERIFY2(mgr.addService(xmlTestDataPath("sampleservice.xml")), PRINT_ERR(mgr));
    obj = mgr.loadInterface(commonInterface, 0, 0);
    QVERIFY(obj != 0);
    QCOMPARE(QString(obj->metaObject()->className()), serviceAClassName);
    delete obj;
    QCoreApplication::processEvents(QEventLoop::AllEvents|QEventLoop::DeferredDeletion);

    // Add first service. Adds the service described in
    // c/Private/<uid3 of this executable>/plugins/xmldata/sampleservice2.xml
    QVERIFY2(mgr.addService(xmlTestDataPath("sampleservice2.xml")), PRINT_ERR(mgr));

    // if first service is set as default, it should be returned
    QVERIFY(mgr.setInterfaceDefault(serviceA, commonInterface));
    obj = mgr.loadInterface(commonInterface, 0, 0);
    QVERIFY(obj != 0);
    QCOMPARE(QString(obj->metaObject()->className()), serviceAClassName);
    delete obj;
    QCoreApplication::processEvents(QEventLoop::AllEvents|QEventLoop::DeferredDeletion);

    // if second service is set as default, it should be returned
    QVERIFY(mgr.setInterfaceDefault(serviceB, commonInterface));
    obj = mgr.loadInterface(commonInterface, 0, 0);
    QVERIFY(obj != 0);
    QCOMPARE(QString(obj->metaObject()->className()), serviceBClassName);
    delete obj;
    QCoreApplication::processEvents(QEventLoop::AllEvents|QEventLoop::DeferredDeletion);
}

void tst_QServiceManager::loadInterface_descriptor()
{
    QFETCH(QServiceInterfaceDescriptor, descriptor);
    QFETCH(QString, className);

    QObject* obj;
    {
        QServiceManager mgr;
        MySecuritySession session;
        MyServiceContext context;
        obj = mgr.loadInterface(descriptor, &context, &session);
        QVERIFY(obj != 0);
        QCOMPARE(className, QString(obj->metaObject()->className()));
    }

    QVERIFY(obj != 0);

    delete obj;
    QCoreApplication::processEvents(QEventLoop::AllEvents|QEventLoop::DeferredDeletion);
}

void tst_QServiceManager::loadInterface_descriptor_data()
{
    QTest::addColumn<QServiceInterfaceDescriptor>("descriptor");
    QTest::addColumn<QString>("className");

    QLibrary lib;
    QServiceInterfaceDescriptor descriptor;
    QServiceInterfaceDescriptorPrivate *priv = new QServiceInterfaceDescriptorPrivate;
    priv->interfaceName = "com.nokia.qt.TestInterfaceA";    // needed by service plugin implementation

    lib.setFileName(QCoreApplication::applicationDirPath() + "/plugins/tst_sfw_sampleserviceplugin");
    QVERIFY(lib.load());
    QVERIFY(lib.unload());
#if defined (Q_OS_SYMBIAN)
    priv->attributes[QServiceInterfaceDescriptor::Location] = "plugins/" + lib.fileName();
#else
    priv->attributes[QServiceInterfaceDescriptor::Location] =  lib.fileName();
#endif
    QServiceInterfaceDescriptorPrivate::setPrivate(&descriptor, priv);
    QTest::newRow("tst_sfw_sampleserviceplugin")
            << descriptor
            << "SampleServicePluginClass";

    lib.setFileName(QCoreApplication::applicationDirPath() + "/plugins/tst_sfw_testservice2plugin");
    QVERIFY(lib.load());
    QVERIFY(lib.unload());

#if defined(Q_OS_SYMBIAN)
    priv->attributes[QServiceInterfaceDescriptor::Location] =  "plugins/" + lib.fileName();
#else
    priv->attributes[QServiceInterfaceDescriptor::Location] =  lib.fileName();
#endif
    QServiceInterfaceDescriptorPrivate::setPrivate(&descriptor, priv);
    QTest::newRow("tst_sfw_sampleserviceplugin2")
            << descriptor
            << "TestService";
}

void tst_QServiceManager::loadInterface_testLoadedObjectAttributes()
{
    QLibrary lib(QCoreApplication::applicationDirPath() + "/plugins/tst_sfw_testservice2plugin");
    QVERIFY(lib.load());
    QVERIFY(lib.unload());

    QServiceInterfaceDescriptor descriptor;
    QServiceInterfaceDescriptorPrivate *priv = new QServiceInterfaceDescriptorPrivate;
    priv->interfaceName = "com.nokia.qt.TestInterfaceA";    // needed by service plugin implementation
#if defined(Q_OS_SYMBIAN)
    priv->attributes[QServiceInterfaceDescriptor::Location] =  "plugins/" + lib.fileName();
#else
    priv->attributes[QServiceInterfaceDescriptor::Location] =  lib.fileName();
#endif
    QServiceInterfaceDescriptorPrivate::setPrivate(&descriptor, priv);

    QServiceManager mgr;
    MySecuritySession session;
    MyServiceContext context;
    QObject *obj = mgr.loadInterface(descriptor, &context, &session);
    QVERIFY(obj != 0);

    bool invokeOk = false;
    QString name;

    // check attributes
    QMetaProperty nameProperty = obj->metaObject()->property(obj->metaObject()->indexOfProperty("name"));
    QVERIFY(nameProperty.isValid());
    QVERIFY(nameProperty.write(obj, "A service name"));
    QCOMPARE(nameProperty.read(obj), QVariant("A service name"));

    // check signals
    QVERIFY(obj->metaObject()->indexOfSignal("someSignal()") >= 0);
    QSignalSpy spy(obj, SIGNAL(someSignal()));
    invokeOk = QMetaObject::invokeMethod(obj, "someSignal");
    QVERIFY(invokeOk);
    QVERIFY(spy.count() == 1);

    // check slots
    invokeOk = QMetaObject::invokeMethod(obj, "callSlot");
    QVERIFY(invokeOk);
    invokeOk = QMetaObject::invokeMethod(obj, "callSlotAndSetName", Q_ARG(QString, "test string"));
    QVERIFY(invokeOk);
    invokeOk = QMetaObject::invokeMethod(obj, "callSlotAndReturnName", Q_RETURN_ARG(QString, name));
    QVERIFY(invokeOk);
    QCOMPARE(name, QString("test string"));

    // check invokables
    invokeOk = QMetaObject::invokeMethod(obj, "callInvokable");
    QVERIFY(invokeOk);

    // call method on a returned object
    QObject *embeddedObj = 0;
    int value = 0;
    invokeOk = QMetaObject::invokeMethod(obj, "embeddedTestService", Q_RETURN_ARG(QObject*, embeddedObj));
    QVERIFY(invokeOk);
    invokeOk = QMetaObject::invokeMethod(embeddedObj, "callWithInt", Q_RETURN_ARG(int, value), Q_ARG(int, 10));
    QVERIFY(invokeOk);
    QCOMPARE(value, 10);

    // call a method that is not invokable via meta system
    invokeOk = QMetaObject::invokeMethod(embeddedObj, "callNormalMethod");
    QVERIFY(!invokeOk);

    delete obj;
    QCoreApplication::processEvents(QEventLoop::AllEvents|QEventLoop::DeferredDeletion);
}

void tst_QServiceManager::loadLocalTypedInterface()
{
    //ensure the plugin exists 
    QLibrary lib(QCoreApplication::applicationDirPath() + "/plugins/tst_sfw_sampleserviceplugin");
    QCOMPARE(lib.load(), true);
    lib.unload();

    QServiceManager mgr;
    MySecuritySession session;
    MyServiceContext context;

    QServiceInterfaceDescriptor descriptor;
    QServiceInterfaceDescriptorPrivate *priv = new QServiceInterfaceDescriptorPrivate;
    priv->interfaceName = "com.nokia.qt.TestInterfaceA";    // needed by service plugin implementation
#if defined(Q_OS_SYMBIAN)
    priv->attributes[QServiceInterfaceDescriptor::Location] =  "plugins/" + lib.fileName();
#else
    priv->attributes[QServiceInterfaceDescriptor::Location] =  lib.fileName();
#endif
    QServiceInterfaceDescriptorPrivate::setPrivate(&descriptor, priv);

    //use manual descriptor -> avoid database involvement
    SampleServicePluginClass *plugin = 0;
    plugin = mgr.loadLocalTypedInterface<SampleServicePluginClass>(descriptor, &context, &session);

    QVERIFY(plugin != 0);
    QCOMPARE(plugin->context(), (QServiceContext *)&context);
    QCOMPARE(plugin->securitySession(), (QAbstractSecuritySession *)&session);

    delete plugin;
    plugin = 0;
    QCoreApplication::processEvents(QEventLoop::AllEvents|QEventLoop::DeferredDeletion);

    //use database descriptor
    QFile file1(xmlTestDataPath("sampleservice.xml"));
    QVERIFY(file1.exists());
    QVERIFY2(mgr.addService(&file1), PRINT_ERR(mgr));

    QCOMPARE(mgr.findServices("com.nokia.qt.TestInterfaceA"), QStringList("SampleService"));
    QCOMPARE(mgr.findServices("com.nokia.qt.TestInterfaceB"), QStringList("SampleService"));
    QCOMPARE(mgr.findServices("com.nokia.qt.TestInterfaceC"), QStringList("SampleService"));
    QList<QServiceInterfaceDescriptor> ifaces = mgr.findInterfaces("SampleService");
    QList<SampleServicePluginClass*> serviceObjects;
    QVERIFY(ifaces.count() == 3);
    for (int i = 0; i<ifaces.count(); i++) {
        plugin = mgr.loadLocalTypedInterface<SampleServicePluginClass>(ifaces.at(i), &context, &session);

        if (ifaces.at(i).interfaceName() == "com.nokia.qt.TestInterfaceC") {
            QVERIFY(plugin == 0);
        } else {
            QVERIFY(plugin != 0);
            QCOMPARE(plugin->context(), (QServiceContext *)&context);
            QCOMPARE(plugin->securitySession(), (QAbstractSecuritySession *)&session);
            plugin->testSlotOne();
            serviceObjects.append(plugin);
        }
    }

    //test for a bug where two service instances from same plugin
    //caused a crash when the first instance was deleted and 
    //the second instance called
    QVERIFY(serviceObjects.count() == 2);
    delete serviceObjects.takeFirst();
    QCoreApplication::processEvents(QEventLoop::AllEvents|QEventLoop::DeferredDeletion);

    plugin = serviceObjects.takeFirst();
    plugin->testSlotOne();
    delete plugin;
    QCoreApplication::processEvents(QEventLoop::AllEvents|QEventLoop::DeferredDeletion);


    //use default lookup
    plugin = mgr.loadLocalTypedInterface<SampleServicePluginClass>("com.nokia.qt.TestInterfaceA", &context, &session);
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->context(), (QServiceContext *)&context);
    QCOMPARE(plugin->securitySession(), (QAbstractSecuritySession *)&session);

    delete plugin;
    QCoreApplication::processEvents(QEventLoop::AllEvents|QEventLoop::DeferredDeletion);
    plugin = 0;

    //use totally wrong but QObject based template class type
    QFile *w = mgr.loadLocalTypedInterface<QFile>("com.nokia.qt.TestInterfaceA", &context, &session);
    QVERIFY(!w);

    //use non QObject based template class type
    //doesn't compile and should never compile
    //QString* s = mgr.loadLocalTypedInterface<QString>("com.nokia.qt.TestInterfaceA", &context, &session);
    //QVERIFY(!s);
}

#define TST_QSERVICEMANAGER_ADD_SERVICE(paramType, file) { \
    if (paramType == "QString") \
        QVERIFY2(mgr.addService(file->fileName()), PRINT_ERR(mgr)); \
    else if (paramType == "QIODevice") \
        QVERIFY2(mgr.addService(file), PRINT_ERR(mgr)); \
    else \
        QFAIL("tst_QServiceManager::addService(): Bad test parameter"); \
}

void tst_QServiceManager::addService()
{
    QFETCH(QString, paramType);

    QServiceManager mgr;

    QString commonInterface = "com.qt.serviceframework.tests.CommonInterface";
    QByteArray xmlA = createServiceXml("ServiceA", createInterfaceXml(commonInterface), VALID_PLUGIN_FILES[0]);
    QByteArray xmlB = createServiceXml("ServiceB", createInterfaceXml(commonInterface), VALID_PLUGIN_FILES[1]);

    QTemporaryFile *tempFileA = new QTemporaryFile(this);
    QVERIFY2(tempFileA->open(), "Can't open temp file A");
    tempFileA->write(xmlA);
    tempFileA->seek(0);
    QTemporaryFile *tempFileB = new QTemporaryFile(this);
    QVERIFY2(tempFileB->open(), "Can't open temp file B");
    tempFileB->write(xmlB);
    tempFileB->seek(0);

    TST_QSERVICEMANAGER_ADD_SERVICE(paramType, tempFileA);
    QCOMPARE(mgr.findServices(), QStringList("ServiceA"));

    // the service should be automatically set as the default for its
    // implemented interfaces since it was the first service added for them
    QCOMPARE(mgr.interfaceDefault(commonInterface).serviceName(), QString("ServiceA"));
    QCOMPARE(mgr.interfaceDefault(commonInterface).serviceName(), QString("ServiceA"));

    // add second service
    TST_QSERVICEMANAGER_ADD_SERVICE(paramType, tempFileB);
    QStringList result = mgr.findServices();
    QCOMPARE(result.count(), 2);
    QVERIFY(result.contains("ServiceA"));
    QVERIFY(result.contains("ServiceB"));

    // the default does not change once ServiceB is added
    QCOMPARE(mgr.interfaceDefault(commonInterface).serviceName(), QString("ServiceA"));
    QCOMPARE(mgr.interfaceDefault(commonInterface).serviceName(), QString("ServiceA"));

    delete tempFileA;
    delete tempFileB;
}

void tst_QServiceManager::addService_data()
{
    QTest::addColumn<QString>("paramType");

    QTest::newRow("string") << "QString";
    QTest::newRow("iodevice") << "QIODevice";
}

void tst_QServiceManager::addService_testInvalidServiceXml()
{
    QBuffer buffer;
    QServiceManager mgr;

    QVERIFY2(!mgr.addService(&buffer), PRINT_ERR(mgr));

    // a service with no interfaces
    QString xml = "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n";
    xml += "<service><name>SomeService</name><filepath>" + VALID_PLUGIN_FILES.first() + "</filepath>\n";
    xml += "</service>\n";
    buffer.close();
    buffer.setData(xml.toLatin1());
    QVERIFY(!mgr.addService(&buffer));

    QVERIFY2(mgr.findServices().isEmpty(), PRINT_ERR(mgr));
}

void tst_QServiceManager::addService_testPluginLoading()
{
    QFETCH(QString, pluginPath);
    QFETCH(bool, isAdded);

    QServiceManager mgr;

    QByteArray xml = createServiceXml("SomeService", createInterfaceXml("com.qt.serviceframework.Interface"), pluginPath);
    QBuffer buffer(&xml);
    QVERIFY2(mgr.addService(&buffer) == isAdded, PRINT_ERR(mgr));

    // the service should not be added if the service plugin cannot be loaded
    if (!isAdded)
        QVERIFY(mgr.findServices().isEmpty());
}

void tst_QServiceManager::addService_testPluginLoading_data()
{
    QTest::addColumn<QString>("pluginPath");
    QTest::addColumn<bool>("isAdded");

    QTest::newRow("valid path") << VALID_PLUGIN_FILES.first() << true;
    QTest::newRow("invalid path") << "no_such_plugin" << false;
}

void tst_QServiceManager::addService_testInstallService()
{
    QSettings settings("com.nokia.qt.serviceframework.tests", "SampleServicePlugin");
    QCOMPARE(settings.value("installed").toBool(), false);

    QServiceManager mgr;
    QVERIFY2(mgr.addService(xmlTestDataPath("sampleservice.xml")), PRINT_ERR(mgr));
    QCOMPARE(mgr.findServices(), QStringList("SampleService"));

    // test installService() was called on the plugin
    QCOMPARE(settings.value("installed").toBool(), true);
}

void tst_QServiceManager::removeService()
{
    QServiceManager mgr;

    QVERIFY(!mgr.removeService("NonExistentService"));

    QSettings settings("com.nokia.qt.serviceframework.tests", "SampleServicePlugin");
    QCOMPARE(settings.value("installed").toBool(), false);

    QVERIFY2(mgr.addService(xmlTestDataPath("sampleservice.xml")), PRINT_ERR(mgr));
    QCOMPARE(mgr.findServices("com.nokia.qt.TestInterfaceA"), QStringList("SampleService"));
    QCOMPARE(settings.value("installed").toBool(), true);

    QVERIFY(mgr.removeService("SampleService"));
    QVERIFY(mgr.findServices().isEmpty());
    QCOMPARE(mgr.findServices("com.nokia.qt.TestInterfaceA"), QStringList());
    QCOMPARE(settings.value("installed").toBool(), false);

    // add it again, should still work
    QVERIFY2(mgr.addService(xmlTestDataPath("sampleservice.xml")), PRINT_ERR(mgr));
    QCOMPARE(mgr.findServices("com.nokia.qt.TestInterfaceA"), QStringList("SampleService"));
    QCOMPARE(settings.value("installed").toBool(), true);
}

void tst_QServiceManager::setInterfaceDefault_strings()
{
    QServiceManager mgr;
    QString interfaceName = "com.nokia.qt.serviceframework.tests.AnInterface";
    DescriptorAttributes attributes;
    QServiceInterfaceDescriptor descriptor;
    QByteArray xml;

    attributes[QServiceInterfaceDescriptor::Location] = VALID_PLUGIN_FILES[0];
    descriptor = createDescriptor(interfaceName, 1, 0, "ServiceA", attributes);
    xml = createServiceXml("ServiceA",
            createInterfaceXml(QList<QServiceInterfaceDescriptor>() << descriptor),
            attributes[QServiceInterfaceDescriptor::Location].toString());
    QBuffer buffer(&xml);

    // fails if the specified interface hasn't been registered
    QCOMPARE(mgr.setInterfaceDefault("ServiceA", interfaceName), false);

    // now it works
    QVERIFY2(mgr.addService(&buffer), PRINT_ERR(mgr));
    QCOMPARE(mgr.setInterfaceDefault("ServiceA", interfaceName), true);
    QCOMPARE(mgr.interfaceDefault(interfaceName), descriptor);

    // replace the default with another service
    attributes[QServiceInterfaceDescriptor::Location] = VALID_PLUGIN_FILES[1];
    descriptor = createDescriptor(interfaceName, 1, 0, "ServiceB", attributes);
    xml = createServiceXml("ServiceB",
            createInterfaceXml(QList<QServiceInterfaceDescriptor>() << descriptor),
            attributes[QServiceInterfaceDescriptor::Location].toString());
    buffer.close();
    buffer.setData(xml);
    QVERIFY2(mgr.addService(&buffer), PRINT_ERR(mgr));
    QCOMPARE(mgr.setInterfaceDefault("ServiceB", interfaceName), true);
    QCOMPARE(mgr.interfaceDefault(interfaceName), descriptor);

    // bad arguments
    QCOMPARE(mgr.setInterfaceDefault("", ""), false);
    QCOMPARE(mgr.setInterfaceDefault("blah", "blah"), false);
    QCOMPARE(mgr.setInterfaceDefault("SampleService", ""), false);
}

void tst_QServiceManager::setInterfaceDefault_strings_multipleInterfaces()
{
    QServiceManager mgr;
    QString interfaceName = "com.nokia.qt.serviceframework.tests.AnInterface";
    DescriptorAttributes attributes;
    QServiceInterfaceDescriptor descriptor;
    QByteArray xml;

    // if there are multiple interfaces, the default should be the latest version
    attributes[QServiceInterfaceDescriptor::Location] = VALID_PLUGIN_FILES[0];
    QList<QServiceInterfaceDescriptor> descriptorList;
    descriptorList << createDescriptor(interfaceName, 1, 0, "ServiceC", attributes)
                   << createDescriptor(interfaceName, 1, 8, "ServiceC", attributes)
                   << createDescriptor(interfaceName, 1, 3, "ServiceC", attributes);
    xml = createServiceXml("ServiceC", createInterfaceXml(descriptorList),
            attributes[QServiceInterfaceDescriptor::Location].toString());
    QBuffer buffer(&xml);
    QVERIFY2(mgr.addService(&buffer), PRINT_ERR(mgr));
    QCOMPARE(mgr.setInterfaceDefault("ServiceC", interfaceName), true);
    QCOMPARE(mgr.interfaceDefault(interfaceName), descriptorList[1]);
}

void tst_QServiceManager::setInterfaceDefault_descriptor()
{
    QFETCH(QService::Scope, scope_add);
    QFETCH(QService::Scope, scope_find);
    QFETCH(bool, expectFound);

    QServiceManager mgr(scope_add);
    QServiceInterfaceDescriptor desc;

    QString interfaceName = "com.nokia.qt.serviceframework.TestInterface";
    DescriptorAttributes attributes;
    attributes[QServiceInterfaceDescriptor::Location] = VALID_PLUGIN_FILES.first();

    QCOMPARE(mgr.setInterfaceDefault(desc), false);

    desc = createDescriptor(interfaceName, 1, 0, "SomeService", attributes,
            scope_add);

    // fails if the specified interface hasn't been registered
    QCOMPARE(mgr.setInterfaceDefault(desc), false);

    // now it works
    QByteArray xml = createServiceXml("SomeService",
            createInterfaceXml(QList<QServiceInterfaceDescriptor>() << desc), VALID_PLUGIN_FILES.first());
    QBuffer buffer(&xml);
    QVERIFY2(mgr.addService(&buffer), PRINT_ERR(mgr));
    QCOMPARE(mgr.setInterfaceDefault(desc), true);

    QCOMPARE(mgr.interfaceDefault(interfaceName), desc);

#if defined(Q_OS_SYMBIAN)
    QCOMPARE(mgr.interfaceDefault(interfaceName).isValid(), expectFound);
#else
    QServiceManager mgrWithOtherScope(scope_find);
    QCOMPARE(mgrWithOtherScope.interfaceDefault(interfaceName).isValid(), expectFound);
#endif
}

void tst_QServiceManager::setInterfaceDefault_descriptor_data()
{
    QTest::addColumn<QService::Scope>("scope_add");
    QTest::addColumn<QService::Scope>("scope_find");
    QTest::addColumn<bool>("expectFound");

#if defined(Q_OS_SYMBIAN)
    // Symbian implementation hard-codes user-scope for everything, do not test any system scope-stuff
    // because returned service interface descriptor is always in user-scope
    QTest::newRow("user scope")
                << QService::UserScope << QService::UserScope << true;
#else
    QTest::newRow("user scope")
            << QService::UserScope << QService::UserScope << true;
    QTest::newRow("system scope")
            << QService::SystemScope << QService::SystemScope << true;

    QTest::newRow("user scope - add, system scope - find")
            << QService::UserScope << QService::SystemScope << false;
    QTest::newRow("system scope - add, user scope - find")
            << QService::SystemScope << QService::UserScope << true;
#endif /* Q_OS_SYMBIAN */
}

void tst_QServiceManager::interfaceDefault()
{
    QServiceManager mgr;
    QVERIFY(!mgr.interfaceDefault("").isValid());
}

void tst_QServiceManager::serviceAdded()
{
    QFETCH(QByteArray, xml);
    QFETCH(QString, serviceName);
    QFETCH(QService::Scope, scope_modify);
    QFETCH(QService::Scope, scope_listen);
    QFETCH(bool, expectSignal);

    QBuffer buffer;
    buffer.setData(xml);
    QServiceManager mgr_modify(scope_modify);
    QServiceManager mgr_listen(scope_listen);
    
    // ensure mgr.connectNotify() is called
    ServicesListener *listener = new ServicesListener;
    connect(&mgr_listen, SIGNAL(serviceAdded(QString,QService::Scope)),
            listener, SLOT(serviceAdded(QString,QService::Scope)));

    QSignalSpy spyAdd(&mgr_listen, SIGNAL(serviceAdded(QString,QService::Scope)));
    QVERIFY2(mgr_modify.addService(&buffer), PRINT_ERR(mgr_modify));

    if (!expectSignal) {
        QTest::qWait(2000);
        QCOMPARE(spyAdd.count(), 0);
    } else {
        QTRY_COMPARE(spyAdd.count(), 1);
    }

    if (expectSignal) {
        QCOMPARE(spyAdd.at(0).at(0).toString(), serviceName);
        QCOMPARE( listener->params.at(0).second , scope_modify);
    }
    listener->params.clear();

    // Pause between file changes so they are detected separately
    QTest::qWait(2000);

    QSignalSpy spyRemove(&mgr_listen, SIGNAL(serviceRemoved(QString,QService::Scope)));
    QVERIFY(mgr_modify.removeService(serviceName));
    
    if (!expectSignal) {
        QTest::qWait(2000);
        QCOMPARE(spyRemove.count(), 0);
    } else {
        QTRY_COMPARE(spyRemove.count(), 1);
    }

#if !defined (Q_OS_WIN) && !defined (Q_OS_SYMBIAN)
    // on win and symbian, cannot delete the database while it is in use
    // try it again after deleting the database
    deleteTestDatabasesAndWaitUntilDone();
#else
    // Pause between file changes so they are detected separately
    QTest::qWait(2000);
#endif

    spyAdd.clear();
    buffer.seek(0);
    QVERIFY2(mgr_modify.addService(&buffer), PRINT_ERR(mgr_modify));
    if (!expectSignal) {
        QTest::qWait(2000);
        QCOMPARE(spyAdd.count(), 0);
    } else {
        QTRY_COMPARE(spyAdd.count(), 1);
    }
    if (expectSignal) {
        QCOMPARE(spyAdd.at(0).at(0).toString(), serviceName);
        QCOMPARE(listener->params.at(0).second, scope_modify);
    }

    // ensure mgr.disconnectNotify() is called
    disconnect(&mgr_listen, SIGNAL(serviceRemoved(QString,QService::Scope)),
            listener, SLOT(serviceRemoved(QString,QService::Scope)));

    delete listener;
}

void tst_QServiceManager::serviceAdded_data()
{   
    QTest::addColumn<QByteArray>("xml");
    QTest::addColumn<QString>("serviceName");
    QTest::addColumn<QService::Scope>("scope_modify");
    QTest::addColumn<QService::Scope>("scope_listen");
    QTest::addColumn<bool>("expectSignal");

    QFile file1(xmlTestDataPath("sampleservice.xml"));
    QVERIFY(file1.open(QIODevice::ReadOnly));
    QFile file2(xmlTestDataPath("testserviceplugin.xml"));
    QVERIFY(file2.open(QIODevice::ReadOnly));

    QByteArray file1Data = file1.readAll();

#if defined (Q_OS_SYMBIAN)
    // Symbian implementation hard-codes (ignores) scopes for everything, do not test mixed-scope stuff
    QTest::newRow("SampleService, user scope") << file1Data << "SampleService"
            << QService::SystemScope << QService::SystemScope << true;
    QTest::newRow("TestService, user scope") << file2.readAll() << "TestService"
            << QService::SystemScope << QService::SystemScope << true;
#else
    QTest::newRow("SampleService, user scope") << file1Data << "SampleService"
            << QService::UserScope << QService::UserScope << true;
    QTest::newRow("TestService, user scope") << file2.readAll() << "TestService"
            << QService::UserScope << QService::UserScope << true;

    QTest::newRow("system scope") << file1Data << "SampleService"
            << QService::SystemScope << QService::SystemScope << true;
    QTest::newRow("modify as user, listen in system") << file1Data << "SampleService"
            << QService::UserScope << QService::SystemScope << false;
    QTest::newRow("modify as system, listen in user") << file1Data << "SampleService"
            << QService::SystemScope << QService::UserScope << true;
#endif /* Q_OS_SYMBIAN */
}

void tst_QServiceManager::serviceRemoved()
{
    QFETCH(QByteArray, xml);
    QFETCH(QString, serviceName);
    QFETCH(QService::Scope, scope_modify);
    QFETCH(QService::Scope, scope_listen);
    QFETCH(bool, expectSignal);

    QBuffer buffer;
    buffer.setData(xml);
    QServiceManager mgr_modify(scope_modify);
    QServiceManager mgr_listen(scope_listen);

    // ensure mgr.connectNotify() is called
    ServicesListener *listener = new ServicesListener;
    connect(&mgr_listen, SIGNAL(serviceRemoved(QString,QService::Scope)),
            listener, SLOT(serviceRemoved(QString,QService::Scope)));

    QSignalSpy spyAdd(&mgr_listen, SIGNAL(serviceAdded(QString,QService::Scope)));
    QVERIFY2(mgr_modify.addService(&buffer), PRINT_ERR(mgr_modify));

    if (!expectSignal) {
        QTest::qWait(2000);
        QCOMPARE(spyAdd.count(), 0);
    } else {
        QTRY_COMPARE(spyAdd.count(), 1);
    }

    // Pause between file changes so they are detected separately
    QTest::qWait(2000);


    QSignalSpy spyRemove(&mgr_listen, SIGNAL(serviceRemoved(QString,QService::Scope)));
    QVERIFY(mgr_modify.removeService(serviceName));

    if (!expectSignal) {
        QTest::qWait(2000);
        QCOMPARE(spyRemove.count(), 0);
    } else {
        QTRY_COMPARE(spyRemove.count(), 1);
    }
    if (expectSignal) {
        QCOMPARE(spyRemove.at(0).at(0).toString(), serviceName);
        QCOMPARE(listener->params.at(0).second , scope_modify);
    }
    listener->params.clear();

#if !defined (Q_OS_WIN) && !defined (Q_OS_SYMBIAN)
    // on win and symbian, cannot delete the database while it is in use
    // try it again after deleting the database
    deleteTestDatabasesAndWaitUntilDone();
#else
    // Pause between file changes so they are detected separately
    QTest::qWait(2000);
#endif
    spyAdd.clear();
    buffer.seek(0);
    QVERIFY2(mgr_modify.addService(&buffer), PRINT_ERR(mgr_modify));
    if (!expectSignal) {
        QTest::qWait(2000);
        QCOMPARE(spyAdd.count(), 0);
    } else {
        QTRY_COMPARE(spyAdd.count(), 1);
    }

    spyRemove.clear();

    // Pause between file changes so they are detected separately
    QTest::qWait(2000);

    QVERIFY(mgr_modify.removeService(serviceName));
    if (!expectSignal) {
        QTest::qWait(2000);
        QCOMPARE(spyRemove.count(), 0);
    } else {
        QTRY_COMPARE(spyRemove.count(), 1);
    }
    if (expectSignal) {
        QCOMPARE(spyRemove.at(0).at(0).toString(), serviceName);
        QCOMPARE(listener->params.at(0).second , scope_modify);
    }

    // ensure mgr.disconnectNotify() is called
    disconnect(&mgr_listen, SIGNAL(serviceRemoved(QString,QService::Scope)),
            listener, SLOT(serviceRemoved(QString,QService::Scope)));

    delete listener;
}

void tst_QServiceManager::serviceRemoved_data()
{
    serviceAdded_data();
}

QTEST_MAIN(tst_QServiceManager)

#include "tst_qservicemanager.moc"
