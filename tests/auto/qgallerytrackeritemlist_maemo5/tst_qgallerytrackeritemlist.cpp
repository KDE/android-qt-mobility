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

//TESTED_COMPONENT=src/documentgallery

#include <QtTest/QtTest>

#include <maemo5/qgallerytrackeritemlist_p.h>
#include <maemo5/qgallerytrackerlistcolumn_p.h>

#include <qgalleryresource.h>

#include <QtDBus/qdbusabstractadaptor.h>
#include <QtDBus/qdbusmetatype.h>

Q_DECLARE_METATYPE(QVector<QStringList>)

QTM_USE_NAMESPACE

class QtTestQueryAdaptor;

class tst_QGalleryTrackerItemList : public QObject
{
    Q_OBJECT
public:
    tst_QGalleryTrackerItemList()
        : m_itemType(QLatin1String("itemType"))
        , m_character(QLatin1String("character"))
        , m_number(QLatin1String("number"))
        , m_argument(QLatin1String("argument"))
        , m_composite(QLatin1String("composite"))
        , m_alias(QLatin1String("alias"))
        , m_image(QLatin1String("image"))
        , m_connection(QDBusConnection::sessionBus())
        , m_queryAdaptor(0) {}

public Q_SLOTS:
    void initTestCase();
    void cleanup();

private Q_SLOTS:
//private:
    void query();
    void refresh();
    void reset();
    void removeItem();
    void insertItem();
    void replaceFirstItem();
    void replaceLastItem();
    void replaceMiddleItem();

private:
    void populateArguments(
            QGalleryTrackerItemListArguments *arguments, const QString &argument);

    const QString m_itemType;
    const QString m_character;
    const QString m_number;
    const QString m_argument;
    const QString m_composite;
    const QString m_alias;
    const QString m_image;
    QDBusConnection m_connection;
    QtTestQueryAdaptor *m_queryAdaptor;
    QGalleryDBusInterfacePointer m_queryInterface;
};

class QtTestProgressMonitor : public QObject
{
    Q_OBJECT
public:
    QtTestProgressMonitor() : m_currentProgress(0), m_maximumProgress(1) {}

    bool isFinished() const { return m_currentProgress == m_maximumProgress; }

public Q_SLOTS:
    void progressChanged(int current, int maximum)
    {
        m_currentProgress = current;
        m_maximumProgress = maximum;
    }

private:
    int m_currentProgress;
    int m_maximumProgress;
};

class QtTestQueryAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.nokia.QtTest.QGalleryTrackerItemList.Query")
public:
    QtTestQueryAdaptor(QObject *object) : QDBusAbstractAdaptor(object)
    {
        qMemSet(m_counts, 0, sizeof(m_counts));
    }

public:
    int count(char id) const { return m_counts[id - 'a']; }
    void setCount(char id, int count) { m_counts[id - 'a'] = count; }

    void clear() { qMemSet(m_counts, 0, sizeof(m_counts)); }

public Q_SLOTS:
    QVector<QStringList> GetResults(const QString &argument, int offset, int limit)
    {
        QVector<QStringList> results;

        for (int c = 0; c < 26; ++c) {
            const QString character = QChar(QLatin1Char('a' + c));

            for (int i = offset; i < m_counts[c] && limit > 0; ++i, --limit) {
                QStringList result;

                QString number = QString(QLatin1String("%1")).arg(i, 3, 10, QLatin1Char('0'));

                QString id = character + QLatin1Char('-') + number;
                QString url
                        = QLatin1String("file:///")
                        + character
                        + QLatin1Char('/')
                        + number;
                QString type = QLatin1String("type") + character;

                result.append(id);
                result.append(url);
                result.append(type);
                result.append(character);
                result.append(number);
                result.append(argument);

                results.append(result);
            }

            offset = qMax(0, offset - m_counts[c]);
        }

        return results;
    }

private:
    int m_counts[26];
};

class QtTestStringValueColumn : public QGalleryTrackerValueColumn
{
public:
    QVariant toVariant(const QString &string) const { return string; }
    QString toString(const QVariant &variant) const { return variant.toString(); }
};

class QtTestIdentityColumn : public QGalleryTrackerCompositeColumn
{
public:
    QtTestIdentityColumn(int column) : m_column(column) {}

    QVariant value(QVector<QVariant>::const_iterator row) const { return *(row + m_column); }

private:
    const int m_column;
};

class QtTestCompositeColumn : public QGalleryTrackerCompositeColumn
{
public:
    QtTestCompositeColumn(int columnA, int columnB) : m_columnA(columnA), m_columnB(columnB) {}

    QVariant value(QVector<QVariant>::const_iterator row) const {
        return (row + m_columnA)->toString() + QLatin1Char('|') + (row + m_columnB)->toString(); }

private:
    const int m_columnA;
    const int m_columnB;
};

void tst_QGalleryTrackerItemList::initTestCase()
{
    const QString service
            = QLatin1String("com.nokia.QtTest.QGalleryTrackerItemList")
              + QString::number(QCoreApplication::applicationPid());
    const QString path = QLatin1String("/");
    const char *interface = "com.nokia.QtTest.QGalleryTrackerItemList.Query";

    qDBusRegisterMetaType<QVector<QStringList> >();

    m_queryAdaptor = new QtTestQueryAdaptor(this);

    m_connection.registerService(service);
    m_connection.registerObject(path, this);

    m_queryInterface = new QGalleryDBusInterface(service, path, interface);

    QVERIFY(m_queryInterface->isValid());
}

void tst_QGalleryTrackerItemList::cleanup()
{
    m_queryAdaptor->clear();
}

void tst_QGalleryTrackerItemList::populateArguments(
        QGalleryTrackerItemListArguments *arguments, const QString &argument)
{
    arguments->idColumn = new QtTestIdentityColumn(0);
    arguments->urlColumn = new QtTestIdentityColumn(1);
    arguments->typeColumn = new QtTestIdentityColumn(2);
    arguments->updateMask = 0x01;
    arguments->identityWidth = 1;
    arguments->valueOffset = 2;
    arguments->queryInterface = m_queryInterface;
    arguments->queryMethod = QLatin1String("GetResults");
    arguments->queryArguments = QVariantList()
            << argument;
    arguments->propertyNames = QStringList()
            << m_itemType
            << m_character
            << m_number
            << m_argument
            << m_composite
            << m_alias;
    arguments->fieldNames = QStringList();
    arguments->propertyAttributes = QVector<QGalleryProperty::Attributes>()
            << (QGalleryProperty::CanRead | QGalleryProperty::CanFilter)
            << (QGalleryProperty::CanRead | QGalleryProperty::CanFilter | QGalleryProperty::CanSort)
            << (QGalleryProperty::CanRead | QGalleryProperty::CanFilter | QGalleryProperty::CanSort)
            << (QGalleryProperty::CanRead | QGalleryProperty::CanSort)
            << (QGalleryProperty::CanRead)
            << (QGalleryProperty::CanRead | QGalleryProperty::CanFilter | QGalleryProperty::CanSort);
    arguments->propertyTypes = QVector<QVariant::Type>()
                               << QVariant::String
                               << QVariant::Char
                               << QVariant::Int
                               << QVariant::String
                               << QVariant::String
                               << QVariant::Int;
    arguments->valueColumns = QVector<QGalleryTrackerValueColumn *>()
            << new QtTestStringValueColumn
            << new QtTestStringValueColumn
            << new QtTestStringValueColumn
            << new QtTestStringValueColumn;
    arguments->compositeColumns = QVector<QGalleryTrackerCompositeColumn *>()
            << new QtTestCompositeColumn(3, 4);
    arguments->aliasColumns = QVector<int>()
            << 4;
    arguments->sortCriteria = QVector<QGalleryTrackerSortCriteria>();
    arguments->resourceKeys = QVector<int>()
            << 2
            << 3;
}

void tst_QGalleryTrackerItemList::query()
{
    const QStringList propertyNames = QStringList()
            << m_itemType
            << m_character
            << m_number
            << m_argument
            << m_composite
            << m_alias;

    QGalleryTrackerItemListArguments arguments;

    populateArguments(&arguments, QLatin1String("query"));

    m_queryAdaptor->setCount('a', 16);

    QGalleryTrackerItemList list(arguments, false, 0, 32);
    QCOMPARE(list.propertyNames(), propertyNames);
    QCOMPARE(list.propertyKey(m_itemType), 2);
    QCOMPARE(list.propertyKey(m_alias), 7);
    QCOMPARE(list.propertyAttributes(2), (QGalleryProperty::CanRead | QGalleryProperty::CanFilter));
    QCOMPARE(list.propertyAttributes(7), (QGalleryProperty::CanRead | QGalleryProperty::CanFilter | QGalleryProperty::CanSort));
    QCOMPARE(list.propertyType(2), QVariant::String);
    QCOMPARE(list.propertyType(7), QVariant::Int);

    QSignalSpy insertSpy(&list, SIGNAL(itemsInserted(int,int)));
    QSignalSpy removeSpy(&list, SIGNAL(itemsRemoved(int,int)));
    QSignalSpy changeSpy(&list, SIGNAL(metaDataChanged(int,int,QList<int>)));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(list.itemCount(), 0);

    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.itemCount(), 16);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(0).toInt(),  0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(1).toInt(), 16);

    QCOMPARE(list.seek(0, false), true);
    QCOMPARE(list.currentIndex(), 0);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("a-000")));
    QCOMPARE(list.itemUrl(), QUrl(QLatin1String("file:///a/000")));
    QCOMPARE(list.itemType(), QLatin1String("typea"));
    QCOMPARE(list.metaData(0), QVariant());
    QCOMPARE(list.metaData(1), QVariant());
    QCOMPARE(list.metaData(2), QVariant(QLatin1String("typea")));
    QCOMPARE(list.metaData(3), QVariant(QLatin1String("a")));
    QCOMPARE(list.metaData(4), QVariant(QLatin1String("000")));
    QCOMPARE(list.metaData(5), QVariant(QLatin1String("query")));
    QCOMPARE(list.metaData(6), QVariant(QLatin1String("a|000")));
    QCOMPARE(list.metaData(7), QVariant(QLatin1String("000")));
    QCOMPARE(list.metaData(8), QVariant());
    {
        QMap<int, QVariant> attributes;
        attributes.insert(2, QLatin1String("typea"));
        attributes.insert(3, QLatin1String("a"));

        QCOMPARE(list.resources(), QList<QGalleryResource>()
                 << QGalleryResource(QUrl(QLatin1String("file:///a/000")), attributes));
    }

    QCOMPARE(list.seek(-1, true), false);
    QCOMPARE(list.currentIndex(), -1);
    QCOMPARE(list.itemId(), QVariant());
    QCOMPARE(list.itemUrl(), QUrl());
    QCOMPARE(list.itemType(), QString());
    QCOMPARE(list.metaData(0), QVariant());
    QCOMPARE(list.metaData(1), QVariant());
    QCOMPARE(list.metaData(2), QVariant());
    QCOMPARE(list.metaData(3), QVariant());
    QCOMPARE(list.metaData(4), QVariant());
    QCOMPARE(list.metaData(5), QVariant());
    QCOMPARE(list.metaData(6), QVariant());
    QCOMPARE(list.metaData(7), QVariant());
    QCOMPARE(list.metaData(8), QVariant());
    QCOMPARE(list.resources(), QList<QGalleryResource>());

    QCOMPARE(list.seek(15, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("a-015")));
    QCOMPARE(list.itemUrl(), QUrl(QLatin1String("file:///a/015")));
    QCOMPARE(list.itemType(), QLatin1String("typea"));
    QCOMPARE(list.metaData(0), QVariant());
    QCOMPARE(list.metaData(1), QVariant());
    QCOMPARE(list.metaData(2), QVariant(QLatin1String("typea")));
    QCOMPARE(list.metaData(3), QVariant(QLatin1String("a")));
    QCOMPARE(list.metaData(4), QVariant(QLatin1String("015")));
    QCOMPARE(list.metaData(5), QVariant(QLatin1String("query")));
    QCOMPARE(list.metaData(6), QVariant(QLatin1String("a|015")));
    QCOMPARE(list.metaData(7), QVariant(QLatin1String("015")));
    QCOMPARE(list.metaData(8), QVariant());
    {
        QMap<int, QVariant> attributes;
        attributes.insert(2, QLatin1String("typea"));
        attributes.insert(3, QLatin1String("a"));

        QCOMPARE(list.resources(), QList<QGalleryResource>()
                 << QGalleryResource(QUrl(QLatin1String("file:///a/015")), attributes));
    }

    QCOMPARE(list.seek(1, true), false);
    QCOMPARE(list.currentIndex(), 16);
    QCOMPARE(list.itemId(), QVariant());
    QCOMPARE(list.itemUrl(), QUrl());
    QCOMPARE(list.itemType(), QString());
    QCOMPARE(list.metaData(0), QVariant());
    QCOMPARE(list.metaData(1), QVariant());
    QCOMPARE(list.metaData(2), QVariant());
    QCOMPARE(list.metaData(3), QVariant());
    QCOMPARE(list.metaData(4), QVariant());
    QCOMPARE(list.metaData(5), QVariant());
    QCOMPARE(list.metaData(6), QVariant());
    QCOMPARE(list.metaData(7), QVariant());
    QCOMPARE(list.metaData(8), QVariant());
    QCOMPARE(list.resources(), QList<QGalleryResource>());
}


void tst_QGalleryTrackerItemList::refresh()
{
    QGalleryTrackerItemListArguments arguments;

    populateArguments(&arguments, QLatin1String("query"));

    m_queryAdaptor->setCount('a', 16);

    QGalleryTrackerItemList list(arguments, true, 0, 32);

    QSignalSpy insertSpy(&list, SIGNAL(itemsInserted(int,int)));
    QSignalSpy removeSpy(&list, SIGNAL(itemsRemoved(int,int)));
    QSignalSpy changeSpy(&list, SIGNAL(metaDataChanged(int,int,QList<int>)));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(list.itemCount(), 0);

    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.itemCount(), 16);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(0).toInt(),  0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(1).toInt(), 16);

    QCOMPARE(list.seek(0, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("a-000")));

    QCOMPARE(list.seek(15, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("a-015")));

    list.refresh();
    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.itemCount(), 16);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);

    QCOMPARE(list.seek(0, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("a-000")));

    QCOMPARE(list.seek(15, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("a-015")));
}

void tst_QGalleryTrackerItemList::reset()
{
    QGalleryTrackerItemListArguments arguments;

    populateArguments(&arguments, QLatin1String("query"));

    m_queryAdaptor->setCount('a', 16);

    QGalleryTrackerItemList list(arguments, true, 0, 32);

    QSignalSpy insertSpy(&list, SIGNAL(itemsInserted(int,int)));
    QSignalSpy removeSpy(&list, SIGNAL(itemsRemoved(int,int)));
    QSignalSpy changeSpy(&list, SIGNAL(metaDataChanged(int,int,QList<int>)));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(list.itemCount(), 0);

    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.itemCount(), 16);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(0).toInt(),  0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(1).toInt(), 16);

    QCOMPARE(list.seek(0, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("a-000")));

    QCOMPARE(list.seek(15, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("a-015")));

    m_queryAdaptor->setCount('a', 0);
    m_queryAdaptor->setCount('b', 16);

    list.refresh();
    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.itemCount(), 16);
    QCOMPARE(insertSpy.count(), 2);
    QCOMPARE(removeSpy.count(), 1);
    QCOMPARE(changeSpy.count(), 0);
    QCOMPARE(removeSpy.at(removeSpy.count() - 1).value(0).toInt(),  0);
    QCOMPARE(removeSpy.at(removeSpy.count() - 1).value(1).toInt(), 16);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(0).toInt(),  0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(1).toInt(), 16);

    QCOMPARE(list.seek(0, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("b-000")));

    QCOMPARE(list.seek(15, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("b-015")));
}


void tst_QGalleryTrackerItemList::removeItem()
{
    QGalleryTrackerItemListArguments arguments;

    populateArguments(&arguments, QLatin1String("query"));

    m_queryAdaptor->setCount('a', 8);
    m_queryAdaptor->setCount('b', 2);
    m_queryAdaptor->setCount('c', 8);

    QGalleryTrackerItemList list(arguments, true, 0, 32);

    QSignalSpy insertSpy(&list, SIGNAL(itemsInserted(int,int)));
    QSignalSpy removeSpy(&list, SIGNAL(itemsRemoved(int,int)));
    QSignalSpy changeSpy(&list, SIGNAL(metaDataChanged(int,int,QList<int>)));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(list.itemCount(), 0);

    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.itemCount(), 18);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(0).toInt(),  0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(1).toInt(), 18);

    QCOMPARE(list.seek(7, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("a-007")));

    QCOMPARE(list.seek(8, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("b-000")));

    QCOMPARE(list.seek(10, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("c-000")));

    m_queryAdaptor->setCount('b', 0);

    list.refresh();
    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.itemCount(), 16);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(removeSpy.count(), 1);
    QCOMPARE(changeSpy.count(), 0);
    QCOMPARE(removeSpy.at(removeSpy.count() - 1).value(0).toInt(),  8);
    QCOMPARE(removeSpy.at(removeSpy.count() - 1).value(1).toInt(),  2);

    QCOMPARE(list.seek(7, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("a-007")));

    QCOMPARE(list.seek(8, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("c-000")));
}

void tst_QGalleryTrackerItemList::insertItem()
{
    QGalleryTrackerItemListArguments arguments;

    populateArguments(&arguments, QLatin1String("query"));

    m_queryAdaptor->setCount('a', 8);
    m_queryAdaptor->setCount('c', 8);

    QGalleryTrackerItemList list(arguments, true, 0, 32);

    QSignalSpy insertSpy(&list, SIGNAL(itemsInserted(int,int)));
    QSignalSpy removeSpy(&list, SIGNAL(itemsRemoved(int,int)));
    QSignalSpy changeSpy(&list, SIGNAL(metaDataChanged(int,int,QList<int>)));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(list.itemCount(), 0);

    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.itemCount(), 16);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(0).toInt(),  0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(1).toInt(), 16);

    QCOMPARE(list.seek(7, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("a-007")));

    QCOMPARE(list.seek(8, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("c-000")));

    m_queryAdaptor->setCount('b', 2);

    list.refresh();
    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.itemCount(), 18);
    QCOMPARE(insertSpy.count(), 2);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(0).toInt(),  8);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(1).toInt(),  2);

    QCOMPARE(list.seek(7, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("a-007")));

    QCOMPARE(list.seek(8, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("b-000")));

    QCOMPARE(list.seek(10, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("c-000")));
}

void tst_QGalleryTrackerItemList::replaceFirstItem()
{
    QGalleryTrackerItemListArguments arguments;

    populateArguments(&arguments, QLatin1String("query"));

    m_queryAdaptor->setCount('a', 1);
    m_queryAdaptor->setCount('c', 15);

    QGalleryTrackerItemList list(arguments, true, 0, 32);

    QSignalSpy insertSpy(&list, SIGNAL(itemsInserted(int,int)));
    QSignalSpy removeSpy(&list, SIGNAL(itemsRemoved(int,int)));
    QSignalSpy changeSpy(&list, SIGNAL(metaDataChanged(int,int,QList<int>)));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(list.itemCount(), 0);

    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.itemCount(), 16);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(0).toInt(), 0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(1).toInt(), 16);


    QCOMPARE(list.seek(0, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("a-000")));

    QCOMPARE(list.seek(1, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("c-000")));

    m_queryAdaptor->setCount('a', 0);
    m_queryAdaptor->setCount('b', 1);

    list.refresh();
    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.itemCount(), 16);
    QCOMPARE(insertSpy.count(), 2);
    QCOMPARE(removeSpy.count(), 1);
    QCOMPARE(changeSpy.count(), 0);
    QCOMPARE(removeSpy.at(removeSpy.count() - 1).value(0).toInt(),  0);
    QCOMPARE(removeSpy.at(removeSpy.count() - 1).value(1).toInt(),  1);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(0).toInt(),  0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(1).toInt(),  1);

    QCOMPARE(list.seek(0, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("b-000")));

    QCOMPARE(list.seek(1, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("c-000")));
}


void tst_QGalleryTrackerItemList::replaceLastItem()
{
    QGalleryTrackerItemListArguments arguments;

    populateArguments(&arguments, QLatin1String("query"));

    m_queryAdaptor->setCount('a', 15);
    m_queryAdaptor->setCount('b', 1);

    QGalleryTrackerItemList list(arguments, true, 0, 32);

    QSignalSpy insertSpy(&list, SIGNAL(itemsInserted(int,int)));
    QSignalSpy removeSpy(&list, SIGNAL(itemsRemoved(int,int)));
    QSignalSpy changeSpy(&list, SIGNAL(metaDataChanged(int,int,QList<int>)));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(list.itemCount(), 0);

    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.itemCount(), 16);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(0).toInt(),  0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(1).toInt(), 16);

    QCOMPARE(list.seek(14, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("a-014")));

    QCOMPARE(list.seek(15, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("b-000")));

    m_queryAdaptor->setCount('b', 0);
    m_queryAdaptor->setCount('c', 1);

    list.refresh();
    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.itemCount(), 16);
    QCOMPARE(insertSpy.count(), 2);
    QCOMPARE(removeSpy.count(), 1);
    QCOMPARE(changeSpy.count(), 0);
    QCOMPARE(removeSpy.at(removeSpy.count() - 1).value(0).toInt(), 15);
    QCOMPARE(removeSpy.at(removeSpy.count() - 1).value(1).toInt(),  1);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(0).toInt(), 15);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(1).toInt(),  1);


    QCOMPARE(list.seek(14, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("a-014")));

    QCOMPARE(list.seek(15, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("c-000")));
}

void tst_QGalleryTrackerItemList::replaceMiddleItem()
{
    QGalleryTrackerItemListArguments arguments;

    populateArguments(&arguments, QLatin1String("query"));

    m_queryAdaptor->setCount('a', 8);
    m_queryAdaptor->setCount('b', 2);
    m_queryAdaptor->setCount('d', 6);

    QGalleryTrackerItemList list(arguments, true, 0, 32);

    QSignalSpy insertSpy(&list, SIGNAL(itemsInserted(int,int)));
    QSignalSpy removeSpy(&list, SIGNAL(itemsRemoved(int,int)));
    QSignalSpy changeSpy(&list, SIGNAL(metaDataChanged(int,int,QList<int>)));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(list.itemCount(), 0);

    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.itemCount(), 16);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(0).toInt(),  0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(1).toInt(), 16);

    QCOMPARE(list.seek(7, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("a-007")));

    QCOMPARE(list.seek(8, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("b-000")));

    QCOMPARE(list.seek(9, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("b-001")));

    QCOMPARE(list.seek(10, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("d-000")));

    m_queryAdaptor->setCount('b', 0);
    m_queryAdaptor->setCount('c', 2);

    list.refresh();
    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.itemCount(), 16);
    QCOMPARE(insertSpy.count(), 2);
    QCOMPARE(removeSpy.count(), 1);
    QCOMPARE(changeSpy.count(), 0);
    QCOMPARE(removeSpy.at(removeSpy.count() - 1).value(0).toInt(),  8);
    QCOMPARE(removeSpy.at(removeSpy.count() - 1).value(1).toInt(),  2);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(0).toInt(),  8);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(1).toInt(),  2);

    QCOMPARE(list.seek(7, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("a-007")));

    QCOMPARE(list.seek(8, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("c-000")));

    QCOMPARE(list.seek(9, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("c-001")));

    QCOMPARE(list.seek(10, false), true);
    QCOMPARE(list.itemId(), QVariant(QLatin1String("d-000")));
}


QTEST_MAIN(tst_QGalleryTrackerItemList)

#include "tst_qgallerytrackeritemlist.moc"

