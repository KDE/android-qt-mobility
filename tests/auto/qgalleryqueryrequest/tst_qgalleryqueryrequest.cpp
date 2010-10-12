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

#include <qgalleryqueryrequest.h>

#include <qabstractgallery.h>
#include <qgalleryabstractresponse.h>
#include <qgalleryresultset.h>
#include <qgalleryresource.h>
#include <qgallerytype.h>

#include <QtTest/QtTest>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QGalleryResultSet*)

class tst_QGalleryQueryRequest : public QObject
{
    Q_OBJECT
public Q_SLOTS:
    void initTestCase();

private Q_SLOTS:
    void propertyNames();
    void sortPropertyNames();
    void autoUpdate();
    void offset();
    void limit();
    void rootType();
    void rootItem();
    void scope();
    void filter();
    void executeSynchronous();
    void executeAsynchronous();
    void noResponse();
};


class QtGalleryTestResponse : public QGalleryResultSet
{
    Q_OBJECT
public:
    QtGalleryTestResponse(
            const QStringList &propertyNames,
            int count,
            QGalleryAbstractRequest::State state,
            int error,
            const QString &errorString)
        : m_count(count)
        , m_currentIndex(-1)
        , m_propertyNames(propertyNames)
    {
        if (error != QGalleryAbstractRequest::NoError)
            QGalleryAbstractResponse::error(error, errorString);
        else if (state == QGalleryAbstractRequest::Finished)
            finish();
        else if (state == QGalleryAbstractRequest::Idle)
            finish(true);
    }

    int propertyKey(const QString &propertyName) const {
        return m_propertyNames.indexOf(propertyName); }
    QGalleryProperty::Attributes propertyAttributes(int) const {
        return QGalleryProperty::CanRead | QGalleryProperty::CanWrite; }
    QVariant::Type propertyType(int) const { return QVariant::String; }

    int itemCount() const { return m_count; }

    int currentIndex() const { return m_currentIndex; }

    bool fetch(int index)
    {
        emit currentIndexChanged(m_currentIndex = index);
        emit currentItemChanged();

        return isValid();
    }

    QVariant itemId() const { return isValid() ? QVariant(1) : QVariant(); }
    QUrl itemUrl() const { return isValid() ? QUrl("http://example.com") : QUrl(); }
    QString itemType() const { return isValid() ? QLatin1String("WebPage") : QString(); }

    QVariant metaData(int key) const { return isValid() ? m_metaData.value(key) : QVariant(); }
    bool setMetaData(int key, const QVariant &value)
    {
        if (isValid()) {
            m_metaData.insert(key, value);
            emit metaDataChanged(m_currentIndex, 1, QList<int>() << key);
            return true;
        } else {
            return false;
        }
    }

    void setCount(int count) { m_count = count; }

    using QGalleryAbstractResponse::finish;
    using QGalleryResultSet::itemsInserted;
    using QGalleryResultSet::itemsRemoved;
    using QGalleryResultSet::itemsMoved;
    using QGalleryResultSet::metaDataChanged;

private:
    int m_count;
    int m_currentIndex;
    QStringList m_propertyNames;
    QHash<int, QVariant> m_metaData;
};

class QtTestGallery : public QAbstractGallery
{
public:
    QtTestGallery()
        : m_count(1)
        , m_state(QGalleryAbstractRequest::Active)
        , m_error(QGalleryAbstractRequest::NoError)
    {}

    bool isRequestSupported(QGalleryAbstractRequest::RequestType type) const {
        return type == QGalleryAbstractRequest::QueryRequest; }

    void setState(QGalleryAbstractRequest::State state) { m_state = state; }
    void setError(int error, const QString &errorString) {
        m_error = error; m_errorString = errorString; }

    void setCount(int count) { m_count = count; }

protected:
    QGalleryAbstractResponse *createResponse(QGalleryAbstractRequest *request)
    {
        if (request->type() == QGalleryAbstractRequest::QueryRequest) {
            return new QtGalleryTestResponse(
                    static_cast<QGalleryQueryRequest *>(request)->propertyNames(),
                    m_count,
                    m_state,
                    m_error,
                    m_errorString);
        }
        return 0;
    }

private:
    int m_count;
    QGalleryAbstractRequest::State m_state;
    int m_error;
    QString m_errorString;
};


void tst_QGalleryQueryRequest::initTestCase()
{
    qRegisterMetaType<QGalleryResultSet*>();
}

void tst_QGalleryQueryRequest::propertyNames()
{
    const QGalleryProperty titleProperty = {"title", sizeof("title")};
    const QGalleryProperty artistProperty = {"artist", sizeof("artist")};

    const QStringList propertyNames = QStringList()
            << titleProperty
            << artistProperty.name()
            << QLatin1String("album")
            << QLatin1String("trackNumber");

    QGalleryQueryRequest request;

    QSignalSpy spy(&request, SIGNAL(propertyNamesChanged()));

    QCOMPARE(request.propertyNames(), QStringList());

    request.setPropertyNames(QStringList());
    QCOMPARE(request.propertyNames(), QStringList());
    QCOMPARE(spy.count(), 0);

    request.setPropertyNames(propertyNames);
    QCOMPARE(request.propertyNames(), propertyNames);
    QCOMPARE(spy.count(), 1);

    request.setPropertyNames(propertyNames);
    QCOMPARE(request.propertyNames(), propertyNames);
    QCOMPARE(spy.count(), 1);

    request.setPropertyNames(QStringList());
    QCOMPARE(request.propertyNames(), QStringList());
    QCOMPARE(spy.count(), 2);
}

void tst_QGalleryQueryRequest::sortPropertyNames()
{
    const QStringList propertyNames = QStringList()
            << QLatin1String("-rating")
            << QLatin1String("+duration");

    QGalleryQueryRequest request;

    QSignalSpy spy(&request, SIGNAL(sortPropertyNamesChanged()));

    QCOMPARE(request.sortPropertyNames(), QStringList());

    request.setSortPropertyNames(QStringList());
    QCOMPARE(request.sortPropertyNames(), QStringList());
    QCOMPARE(spy.count(), 0);

    request.setSortPropertyNames(propertyNames);
    QCOMPARE(request.sortPropertyNames(), propertyNames);
    QCOMPARE(spy.count(), 1);

    request.setSortPropertyNames(propertyNames);
    QCOMPARE(request.sortPropertyNames(), propertyNames);
    QCOMPARE(spy.count(), 1);

    request.setSortPropertyNames(QStringList());
    QCOMPARE(request.sortPropertyNames(), QStringList());
    QCOMPARE(spy.count(), 2);
}

void tst_QGalleryQueryRequest::autoUpdate()
{
    QGalleryQueryRequest request;

    QSignalSpy spy(&request, SIGNAL(autoUpdateChanged()));

    QCOMPARE(request.autoUpdate(), false);

    request.setAutoUpdate(false);
    QCOMPARE(request.autoUpdate(), false);
    QCOMPARE(spy.count(), 0);

    request.setAutoUpdate(true);
    QCOMPARE(request.autoUpdate(), true);
    QCOMPARE(spy.count(), 1);

    request.setAutoUpdate(true);
    QCOMPARE(request.autoUpdate(), true);
    QCOMPARE(spy.count(), 1);

    request.setAutoUpdate(false);
    QCOMPARE(request.autoUpdate(), false);
    QCOMPARE(spy.count(), 2);
}

void tst_QGalleryQueryRequest::offset()
{
    QGalleryQueryRequest request;

    QSignalSpy spy(&request, SIGNAL(offsetChanged()));

    QCOMPARE(request.offset(), 0);

    request.setOffset(0);
    QCOMPARE(request.offset(), 0);
    QCOMPARE(spy.count(), 0);

    request.setOffset(-45);
    QCOMPARE(request.offset(), 0);
    QCOMPARE(spy.count(), 0);

    request.setOffset(32);
    QCOMPARE(request.offset(), 32);
    QCOMPARE(spy.count(), 1);

    request.setOffset(32);
    QCOMPARE(request.offset(), 32);
    QCOMPARE(spy.count(), 1);

    request.setOffset(-45);
    QCOMPARE(request.offset(), 0);
    QCOMPARE(spy.count(), 2);
}

void tst_QGalleryQueryRequest::limit()
{
    QGalleryQueryRequest request;

    QSignalSpy spy(&request, SIGNAL(limitChanged()));

    QCOMPARE(request.limit(), 0);

    request.setLimit(0);
    QCOMPARE(request.limit(), 0);
    QCOMPARE(spy.count(), 0);

    request.setLimit(-21);
    QCOMPARE(request.limit(), 0);
    QCOMPARE(spy.count(), 0);

    request.setLimit(102);
    QCOMPARE(request.limit(), 102);
    QCOMPARE(spy.count(), 1);

    request.setLimit(102);
    QCOMPARE(request.limit(), 102);
    QCOMPARE(spy.count(), 1);

    request.setLimit(-21);
    QCOMPARE(request.limit(), 0);
    QCOMPARE(spy.count(), 2);
}

void tst_QGalleryQueryRequest::rootType()
{
    const QString itemType = QLatin1String("Audio");

    QGalleryQueryRequest request;

    QSignalSpy spy(&request, SIGNAL(rootTypeChanged()));

    QCOMPARE(request.rootType(), QString());

    request.setRootType(QString());
    QCOMPARE(request.rootType(), QString());
    QCOMPARE(spy.count(), 0);

    request.setRootType(itemType);
    QCOMPARE(request.rootType(), itemType);
    QCOMPARE(spy.count(), 1);

    request.setRootType(itemType);
    QCOMPARE(request.rootType(), itemType);
    QCOMPARE(spy.count(), 1);

    request.setRootType(QString());
    QCOMPARE(request.rootType(), QString());
    QCOMPARE(spy.count(), 2);
}

void tst_QGalleryQueryRequest::rootItem()
{
    QGalleryQueryRequest request;

    QSignalSpy spy(&request, SIGNAL(rootItemChanged()));

    QCOMPARE(request.rootItem(), QVariant());

    request.setRootItem(QVariant());
    QCOMPARE(request.rootItem(), QVariant());
    QCOMPARE(spy.count(), 0);

    request.setRootItem(76);
    QCOMPARE(request.rootItem(), QVariant(76));
    QCOMPARE(spy.count(), 1);

    request.setRootItem(76);
    QCOMPARE(request.rootItem(), QVariant(76));
    QCOMPARE(spy.count(), 1);

    request.setRootItem(QLatin1String("65"));
    QCOMPARE(request.rootItem(), QVariant(QLatin1String("65")));
    QCOMPARE(spy.count(), 2);

    request.setRootItem(QLatin1String("65"));
    QCOMPARE(request.rootItem(), QVariant(QLatin1String("65")));
    QCOMPARE(spy.count(), 2);

    request.setRootItem(QVariant());
    QCOMPARE(request.rootItem(), QVariant());
    QCOMPARE(spy.count(), 3);
}

void tst_QGalleryQueryRequest::scope()
{
    QGalleryQueryRequest request;

    QSignalSpy spy(&request, SIGNAL(scopeChanged()));

    QCOMPARE(request.scope(), QGalleryQueryRequest::AllDescendants);

    request.setScope(QGalleryQueryRequest::AllDescendants);
    QCOMPARE(request.scope(), QGalleryQueryRequest::AllDescendants);
    QCOMPARE(spy.count(), 0);

    request.setScope(QGalleryQueryRequest::DirectDescendants);
    QCOMPARE(request.scope(), QGalleryQueryRequest::DirectDescendants);
    QCOMPARE(spy.count(), 1);

    request.setScope(QGalleryQueryRequest::DirectDescendants);
    QCOMPARE(request.scope(), QGalleryQueryRequest::DirectDescendants);
    QCOMPARE(spy.count(), 1);

    request.setScope(QGalleryQueryRequest::AllDescendants);
    QCOMPARE(request.scope(), QGalleryQueryRequest::AllDescendants);
    QCOMPARE(spy.count(), 2);
}

void tst_QGalleryQueryRequest::filter()
{
    const QGalleryFilter filter = QGalleryMetaDataFilter(
            QLatin1String("rating"), 3, QGalleryFilter::GreaterThan);

    QGalleryQueryRequest request;

    QSignalSpy spy(&request, SIGNAL(filterChanged()));

    QCOMPARE(request.filter(), QGalleryFilter());

    request.setFilter(QGalleryFilter());
    QCOMPARE(request.filter(), QGalleryFilter());
    QCOMPARE(spy.count(), 0);

    request.setFilter(filter);
    QCOMPARE(request.filter(), filter);
    QCOMPARE(spy.count(), 1);

    request.setFilter(filter);
    QCOMPARE(request.filter(), filter);
    QCOMPARE(spy.count(), 1);

    request.setFilter(QGalleryFilter());
    QCOMPARE(request.filter(), QGalleryFilter());
    QCOMPARE(spy.count(), 2);
}


void tst_QGalleryQueryRequest::executeSynchronous()
{
    QtTestGallery gallery;
    gallery.setError(80, QString());

    QGalleryQueryRequest request(&gallery);
    QVERIFY(request.resultSet() == 0);

    request.setPropertyNames(QStringList()
            << QLatin1String("album")
            << QLatin1String("trackNumber"));

    QSignalSpy spy(&request, SIGNAL(resultSetChanged(QGalleryResultSet*)));

    request.execute();
    QCOMPARE(request.state(), QGalleryAbstractRequest::Error);
    QCOMPARE(request.error(), 80);
    QCOMPARE(spy.count(), 0);
    QVERIFY(qobject_cast<QtGalleryTestResponse *>(request.resultSet()) == 0);

    gallery.setState(QGalleryAbstractRequest::Finished);
    gallery.setError(QGalleryAbstractRequest::NoError, QString());
    gallery.setCount(10);
    request.execute();
    QCOMPARE(request.state(), QGalleryAbstractRequest::Finished);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(spy.count(), 1);
    QVERIFY(qobject_cast<QtGalleryTestResponse *>(request.resultSet()) != 0);
    QCOMPARE(spy.last().at(0).value<QGalleryResultSet*>(), request.resultSet());

    QCOMPARE(request.propertyKey(QLatin1String("title")), -1);
    QCOMPARE(request.propertyKey(QLatin1String("album")), 0);
    QCOMPARE(request.propertyKey(QLatin1String("trackNumber")), 1);

    QCOMPARE(request.propertyAttributes(0), QGalleryProperty::CanRead | QGalleryProperty::CanWrite);
    QCOMPARE(request.propertyType(0), QVariant::String);

    QCOMPARE(request.itemCount(), 10);
    QCOMPARE(request.currentIndex(), -1);
    QCOMPARE(request.itemId(), QVariant());
    QCOMPARE(request.itemUrl(), QUrl());
    QCOMPARE(request.itemType(), QString());
    QCOMPARE(request.metaData(1), QVariant());
    QCOMPARE(request.setMetaData(1, 12), false);
    QCOMPARE(request.metaData(1), QVariant());
    QCOMPARE(request.metaData(QLatin1String("trackNumber")), QVariant());
    QCOMPARE(request.setMetaData(QLatin1String("trackNumber"), 12), false);
    QCOMPARE(request.metaData(QLatin1String("trackNumber")), QVariant());
    QCOMPARE(request.resources(), QList<QGalleryResource>());

    QCOMPARE(request.seek(0, false), true);
    QCOMPARE(request.currentIndex(), 0);
    QCOMPARE(request.isValid(), true);
    QCOMPARE(request.seek(1, true), true);
    QCOMPARE(request.currentIndex(), 1);
    QCOMPARE(request.isValid(), true);
    QCOMPARE(request.seek(-1, true), true);
    QCOMPARE(request.currentIndex(), 0);
    QCOMPARE(request.isValid(), true);
    QCOMPARE(request.seek(-1, false), false);
    QCOMPARE(request.currentIndex(), -1);
    QCOMPARE(request.isValid(), false);
    QCOMPARE(request.last(), true);
    QCOMPARE(request.currentIndex(), 9);
    QCOMPARE(request.isValid(), true);
    QCOMPARE(request.next(), false);
    QCOMPARE(request.currentIndex(), 10);
    QCOMPARE(request.isValid(), false);
    QCOMPARE(request.previous(), true);
    QCOMPARE(request.currentIndex(), 9);
    QCOMPARE(request.isValid(), true);
    QCOMPARE(request.first(), true);
    QCOMPARE(request.currentIndex(), 0);
    QCOMPARE(request.previous(), false);
    QCOMPARE(request.currentIndex(), -1);
    QCOMPARE(request.isValid(), false);
    QCOMPARE(request.next(), true);
    QCOMPARE(request.currentIndex(), 0);
    QCOMPARE(request.isValid(), true);

    QCOMPARE(request.itemId(), QVariant(1));
    QCOMPARE(request.itemUrl(), QUrl(QLatin1String("http://example.com")));
    QCOMPARE(request.itemType(), QLatin1String("WebPage"));
    QCOMPARE(request.metaData(1), QVariant());
    QCOMPARE(request.setMetaData(1, 12), true);
    QCOMPARE(request.metaData(1), QVariant(12));
    QCOMPARE(request.metaData(QLatin1String("trackNumber")), QVariant(12));
    QCOMPARE(request.setMetaData(QLatin1String("trackNumber"), 5), true);
    QCOMPARE(request.metaData(QLatin1String("trackNumber")), QVariant(5));
    QCOMPARE(request.resources(), QList<QGalleryResource>()
            << QGalleryResource(QUrl(QLatin1String("http://example.com"))));

    QCOMPARE(request.previous(), false);
    QCOMPARE(request.currentIndex(), -1);

    QCOMPARE(request.itemId(), QVariant());
    QCOMPARE(request.itemUrl(), QUrl());
    QCOMPARE(request.itemType(), QString());
    QCOMPARE(request.metaData(1), QVariant());
    QCOMPARE(request.setMetaData(1, 12), false);
    QCOMPARE(request.metaData(1), QVariant());
    QCOMPARE(request.metaData(QLatin1String("trackNumber")), QVariant());
    QCOMPARE(request.setMetaData(QLatin1String("trackNumber"), 12), false);
    QCOMPARE(request.metaData(QLatin1String("trackNumber")), QVariant());
    QCOMPARE(request.resources(), QList<QGalleryResource>());

    request.clear();
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(spy.count(), 2);
    QVERIFY(request.resultSet() == 0);
    QCOMPARE(spy.last().at(0).value<QGalleryResultSet*>(), request.resultSet());
}

void tst_QGalleryQueryRequest::executeAsynchronous()
{
    QtTestGallery gallery;
    gallery.setState(QGalleryAbstractRequest::Active);

    QGalleryQueryRequest request(&gallery);
    QVERIFY(request.resultSet() == 0);

    QSignalSpy spy(&request, SIGNAL(resultSetChanged(QGalleryResultSet*)));

    request.execute();
    QCOMPARE(request.state(), QGalleryAbstractRequest::Active);
    QCOMPARE(spy.count(), 1);
    QVERIFY(qobject_cast<QtGalleryTestResponse *>(request.resultSet()) != 0);
    QCOMPARE(spy.last().at(0).value<QGalleryResultSet*>(), request.resultSet());

    qobject_cast<QtGalleryTestResponse *>(request.resultSet())->finish(false);
    QCOMPARE(request.state(), QGalleryAbstractRequest::Finished);
    QCOMPARE(spy.count(), 1);
    QVERIFY(qobject_cast<QtGalleryTestResponse *>(request.resultSet()) != 0);

    request.clear();
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(spy.count(), 2);
    QVERIFY(request.resultSet() == 0);
    QCOMPARE(spy.last().at(0).value<QGalleryResultSet*>(), request.resultSet());
}

void tst_QGalleryQueryRequest::noResponse()
{
    QGalleryQueryRequest request;

    QCOMPARE(request.propertyKey(QLatin1String("title")), -1);
    QCOMPARE(request.propertyKey(QLatin1String("album")), -1);
    QCOMPARE(request.propertyKey(QLatin1String("trackNumber")), -1);

    QCOMPARE(request.propertyAttributes(0), QGalleryProperty::Attributes());
    QCOMPARE(request.propertyType(0), QVariant::Invalid);

    QCOMPARE(request.itemCount(), 0);
    QCOMPARE(request.currentIndex(), -1);
    QCOMPARE(request.itemId(), QVariant());
    QCOMPARE(request.itemUrl(), QUrl());
    QCOMPARE(request.itemType(), QString());
    QCOMPARE(request.metaData(1), QVariant());
    QCOMPARE(request.setMetaData(1, QLatin1String("hello")), false);
    QCOMPARE(request.metaData(1), QVariant());
    QCOMPARE(request.metaData(QLatin1String("title")), QVariant());
    QCOMPARE(request.setMetaData(QLatin1String("title"), QLatin1String("hello")), false);
    QCOMPARE(request.metaData(QLatin1String("title")), QVariant());
    QCOMPARE(request.resources(), QList<QGalleryResource>());
    QCOMPARE(request.seek(0, false), false);
    QCOMPARE(request.seek(1, true), false);
    QCOMPARE(request.seek(-1, true), false);
    QCOMPARE(request.next(), false);
    QCOMPARE(request.previous(), false);
}

QTEST_MAIN(tst_QGalleryQueryRequest)

#include "tst_qgalleryqueryrequest.moc"
