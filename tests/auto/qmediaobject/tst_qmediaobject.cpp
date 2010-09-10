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

//TESTED_COMPONENT=src/multimedia

#include "tst_qmediaobject.h"

QT_USE_NAMESPACE

void tst_QMediaObject::propertyWatch()
{
    QtTestMediaObject object;
    object.setNotifyInterval(0);

    QEventLoop loop;
    connect(&object, SIGNAL(aChanged(int)), &QTestEventLoop::instance(), SLOT(exitLoop()));
    connect(&object, SIGNAL(bChanged(int)), &QTestEventLoop::instance(), SLOT(exitLoop()));
    connect(&object, SIGNAL(cChanged(int)), &QTestEventLoop::instance(), SLOT(exitLoop()));

    QSignalSpy aSpy(&object, SIGNAL(aChanged(int)));
    QSignalSpy bSpy(&object, SIGNAL(bChanged(int)));
    QSignalSpy cSpy(&object, SIGNAL(cChanged(int)));

    QTestEventLoop::instance().enterLoop(1);

    QCOMPARE(aSpy.count(), 0);
    QCOMPARE(bSpy.count(), 0);
    QCOMPARE(cSpy.count(), 0);

    int aCount = 0;
    int bCount = 0;
    int cCount = 0;

    object.addPropertyWatch("a");

    QTestEventLoop::instance().enterLoop(1);

    QVERIFY(aSpy.count() > aCount);
    QCOMPARE(bSpy.count(), 0);
    QCOMPARE(cSpy.count(), 0);
    QCOMPARE(aSpy.last().value(0).toInt(), 0);

    aCount = aSpy.count();

    object.setA(54);
    object.setB(342);
    object.setC(233);

    QTestEventLoop::instance().enterLoop(1);

    QVERIFY(aSpy.count() > aCount);
    QCOMPARE(bSpy.count(), 0);
    QCOMPARE(cSpy.count(), 0);
    QCOMPARE(aSpy.last().value(0).toInt(), 54);

    aCount = aSpy.count();

    object.addPropertyWatch("b");
    object.addPropertyWatch("d");
    object.removePropertyWatch("e");
    object.setA(43);
    object.setB(235);
    object.setC(90);

    QTestEventLoop::instance().enterLoop(1);

    QVERIFY(aSpy.count() > aCount);
    QVERIFY(bSpy.count() > bCount);
    QCOMPARE(cSpy.count(), 0);
    QCOMPARE(aSpy.last().value(0).toInt(), 43);
    QCOMPARE(bSpy.last().value(0).toInt(), 235);

    aCount = aSpy.count();
    bCount = bSpy.count();

    object.removePropertyWatch("a");
    object.addPropertyWatch("c");
    object.addPropertyWatch("e");

    QTestEventLoop::instance().enterLoop(1);

    QCOMPARE(aSpy.count(), aCount);
    QVERIFY(bSpy.count() > bCount);
    QVERIFY(cSpy.count() > cCount);
    QCOMPARE(bSpy.last().value(0).toInt(), 235);
    QCOMPARE(cSpy.last().value(0).toInt(), 90);

    bCount = bSpy.count();
    cCount = cSpy.count();

    object.setA(435);
    object.setC(9845);

    QTestEventLoop::instance().enterLoop(1);

    QCOMPARE(aSpy.count(), aCount);
    QVERIFY(bSpy.count() > bCount);
    QVERIFY(cSpy.count() > cCount);
    QCOMPARE(bSpy.last().value(0).toInt(), 235);
    QCOMPARE(cSpy.last().value(0).toInt(), 9845);

    bCount = bSpy.count();
    cCount = cSpy.count();

    object.setA(8432);
    object.setB(324);
    object.setC(443);
    object.removePropertyWatch("c");
    object.removePropertyWatch("d");

    QTestEventLoop::instance().enterLoop(1);

    QCOMPARE(aSpy.count(), aCount);
    QVERIFY(bSpy.count() > bCount);
    QCOMPARE(cSpy.count(), cCount);
    QCOMPARE(bSpy.last().value(0).toInt(), 324);
    QCOMPARE(cSpy.last().value(0).toInt(), 9845);

    bCount = bSpy.count();

    object.removePropertyWatch("b");

    QTestEventLoop::instance().enterLoop(1);

    QCOMPARE(aSpy.count(), aCount);
    QCOMPARE(bSpy.count(), bCount);
    QCOMPARE(cSpy.count(), cCount);
}

void tst_QMediaObject::setupNotifyTests()
{
    QTest::addColumn<int>("interval");
    QTest::addColumn<int>("count");

    QTest::newRow("single 750ms")
            << 750
            << 1;
    QTest::newRow("single 600ms")
            << 600
            << 1;
    QTest::newRow("x3 300ms")
            << 300
            << 3;
    QTest::newRow("x5 180ms")
            << 180
            << 5;
}

void tst_QMediaObject::notifySignals_data()
{
    setupNotifyTests();
}

void tst_QMediaObject::notifySignals()
{
    QFETCH(int, interval);
    QFETCH(int, count);

    QtTestMediaObject object;
    object.setNotifyInterval(interval);
    object.addPropertyWatch("a");

    QSignalSpy spy(&object, SIGNAL(aChanged(int)));

    QTestEventLoop::instance().enterLoop(1);

    QCOMPARE(spy.count(), count);
}

void tst_QMediaObject::notifyInterval_data()
{
    setupNotifyTests();
}

void tst_QMediaObject::notifyInterval()
{
    QFETCH(int, interval);

    QtTestMediaObject object;
    QSignalSpy spy(&object, SIGNAL(notifyIntervalChanged(int)));

    object.setNotifyInterval(interval);
    QCOMPARE(object.notifyInterval(), interval);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.last().value(0).toInt(), interval);

    object.setNotifyInterval(interval);
    QCOMPARE(object.notifyInterval(), interval);
    QCOMPARE(spy.count(), 1);
}

void tst_QMediaObject::nullMetaDataControl()
{
    const QString titleKey(QLatin1String("Title"));
    const QString title(QLatin1String("Host of Seraphim"));

    QtTestMetaDataService service;
    service.hasMetaData = false;

    QtTestMediaObject object(&service);

    QSignalSpy spy(&object, SIGNAL(metaDataChanged()));

    QCOMPARE(object.isMetaDataAvailable(), false);

    QCOMPARE(object.metaData(QtMultimediaKit::Title).toString(), QString());
    QCOMPARE(object.extendedMetaData(titleKey).toString(), QString());
    QCOMPARE(object.availableMetaData(), QList<QtMultimediaKit::MetaData>());
    QCOMPARE(object.availableExtendedMetaData(), QStringList());
    QCOMPARE(spy.count(), 0);
}

void tst_QMediaObject::isMetaDataAvailable()
{
    QtTestMetaDataService service;
    service.metaData.setMetaDataAvailable(false);

    QtTestMediaObject object(&service);
    QCOMPARE(object.isMetaDataAvailable(), false);

    QSignalSpy spy(&object, SIGNAL(metaDataAvailableChanged(bool)));
    service.metaData.setMetaDataAvailable(true);

    QCOMPARE(object.isMetaDataAvailable(), true);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toBool(), true);

    service.metaData.setMetaDataAvailable(false);

    QCOMPARE(object.isMetaDataAvailable(), false);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.at(1).at(0).toBool(), false);
}

void tst_QMediaObject::metaDataChanged()
{
    QtTestMetaDataService service;
    QtTestMediaObject object(&service);

    QSignalSpy spy(&object, SIGNAL(metaDataChanged()));

    service.metaData.metaDataChanged();
    QCOMPARE(spy.count(), 1);

    service.metaData.metaDataChanged();
    QCOMPARE(spy.count(), 2);
}

void tst_QMediaObject::metaData_data()
{
    QTest::addColumn<QString>("artist");
    QTest::addColumn<QString>("title");
    QTest::addColumn<QString>("genre");

    QTest::newRow("")
            << QString::fromLatin1("Dead Can Dance")
            << QString::fromLatin1("Host of Seraphim")
            << QString::fromLatin1("Awesome");
}

void tst_QMediaObject::metaData()
{
    QFETCH(QString, artist);
    QFETCH(QString, title);
    QFETCH(QString, genre);

    QtTestMetaDataService service;
    service.metaData.populateMetaData();

    QtTestMediaObject object(&service);
    QVERIFY(object.availableMetaData().isEmpty());

    service.metaData.m_data.insert(QtMultimediaKit::AlbumArtist, artist);
    service.metaData.m_data.insert(QtMultimediaKit::Title, title);
    service.metaData.m_data.insert(QtMultimediaKit::Genre, genre);

    QCOMPARE(object.metaData(QtMultimediaKit::AlbumArtist).toString(), artist);
    QCOMPARE(object.metaData(QtMultimediaKit::Title).toString(), title);

    QList<QtMultimediaKit::MetaData> metaDataKeys = object.availableMetaData();
    QCOMPARE(metaDataKeys.size(), 3);
    QVERIFY(metaDataKeys.contains(QtMultimediaKit::AlbumArtist));
    QVERIFY(metaDataKeys.contains(QtMultimediaKit::Title));
    QVERIFY(metaDataKeys.contains(QtMultimediaKit::Genre));
}

void tst_QMediaObject::extendedMetaData()
{
    QFETCH(QString, artist);
    QFETCH(QString, title);
    QFETCH(QString, genre);

    QtTestMetaDataService service;
    QtTestMediaObject object(&service);
    QVERIFY(object.availableExtendedMetaData().isEmpty());

    service.metaData.m_extendedData.insert(QLatin1String("Artist"), artist);
    service.metaData.m_extendedData.insert(QLatin1String("Title"), title);
    service.metaData.m_extendedData.insert(QLatin1String("Genre"), genre);

    QCOMPARE(object.extendedMetaData(QLatin1String("Artist")).toString(), artist);
    QCOMPARE(object.extendedMetaData(QLatin1String("Title")).toString(), title);

    QStringList extendedKeys = object.availableExtendedMetaData();
    QCOMPARE(extendedKeys.size(), 3);
    QVERIFY(extendedKeys.contains(QLatin1String("Artist")));
    QVERIFY(extendedKeys.contains(QLatin1String("Title")));
    QVERIFY(extendedKeys.contains(QLatin1String("Genre")));
}

void tst_QMediaObject::availability()
{
    QtTestMediaObject nullObject(0);
    QCOMPARE(nullObject.isAvailable(), false);
    QCOMPARE(nullObject.availabilityError(), QtMultimediaKit::ServiceMissingError);

    QtTestMetaDataService service;
    QtTestMediaObject object(&service);
    QCOMPARE(object.isAvailable(), true);
    QCOMPARE(object.availabilityError(), QtMultimediaKit::NoError);
}
