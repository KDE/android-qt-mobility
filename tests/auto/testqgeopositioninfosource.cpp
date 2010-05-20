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

#include <QTest>
#include <QMetaType>
#include <QSignalSpy>
#include <QDebug>
#include <QTimer>

#include <limits.h>

#include <qnumeric.h>
#include <qgeopositioninfosource.h>
#include <qgeopositioninfo.h>

#include "testqgeopositioninfosource_p.h"
#include "qlocationtestutils_p.h"


Q_DECLARE_METATYPE(QGeoPositionInfoSource::PositioningMethod)
Q_DECLARE_METATYPE(QGeoPositionInfoSource::PositioningMethods)
Q_DECLARE_METATYPE(QGeoPositionInfo)

#define MAX_WAITING_TIME 50000

// Must provide a valid source, unless testing the source
// returned by QGeoPositionInfoSource::createDefaultSource() on a system
// that has no default source
#define CHECK_SOURCE_VALID { \
        if (!m_source) { \
            if (m_testingDefaultSource && QGeoPositionInfoSource::createDefaultSource(0) == 0) \
                QSKIP("No default position source on this system", SkipAll); \
            else \
                QFAIL("createTestSource() must return a valid source!"); \
        } \
    }

class MyPositionSource : public QGeoPositionInfoSource
{
    Q_OBJECT
public:
    MyPositionSource(QObject *parent = 0)
            : QGeoPositionInfoSource(parent) {
    }

    QGeoPositionInfo lastKnownPosition(bool /*fromSatellitePositioningMethodsOnly = false*/) const {
        return QGeoPositionInfo();
    }

    void setSupportedPositioningMethods(PositioningMethods methods) {
        m_methods = methods;
    }

    virtual PositioningMethods supportedPositioningMethods() const {
        return m_methods;
    }
    virtual int minimumUpdateInterval() const {
        return 0;
    }

    virtual void startUpdates() {}
    virtual void stopUpdates() {}

    virtual void requestUpdate(int) {}

private:
    PositioningMethods m_methods;
};

class DefaultSourceTest : public TestQGeoPositionInfoSource
{
    Q_OBJECT
protected:
    QGeoPositionInfoSource *createTestSource() {
        return QGeoPositionInfoSource::createDefaultSource(0);
    }
};


TestQGeoPositionInfoSource::TestQGeoPositionInfoSource(QObject *parent)
        : QObject(parent)
{
    m_testingDefaultSource = false;
}

TestQGeoPositionInfoSource *TestQGeoPositionInfoSource::createDefaultSourceTest()
{
    DefaultSourceTest *test = new DefaultSourceTest;
    test->m_testingDefaultSource = true;
    return test;
}

void TestQGeoPositionInfoSource::test_slot1()
{
}

void TestQGeoPositionInfoSource::test_slot2()
{
    m_testSlot2Called = true;
}

void TestQGeoPositionInfoSource::base_initTestCase()
{
    qRegisterMetaType<QGeoPositionInfo>();
}

void TestQGeoPositionInfoSource::base_init()
{
    m_source = createTestSource();
    m_testSlot2Called = false;
}

void TestQGeoPositionInfoSource::base_cleanup()
{
    delete m_source;
    m_source = 0;
}

void TestQGeoPositionInfoSource::base_cleanupTestCase()
{
}

void TestQGeoPositionInfoSource::initTestCase()
{
    base_initTestCase();
}

void TestQGeoPositionInfoSource::init()
{
    base_init();
}

void TestQGeoPositionInfoSource::cleanup()
{
    base_cleanup();
}

void TestQGeoPositionInfoSource::cleanupTestCase()
{
    base_cleanupTestCase();
}

// TC_ID_3_x_1
void TestQGeoPositionInfoSource::constructor_withParent()
{
    //QLocationTestUtils::uheap_mark();
    QObject *parent = new QObject();
    new MyPositionSource(parent);
    delete parent;
    //QLocationTestUtils::uheap_mark_end();
}

// TC_ID_3_x_2
void TestQGeoPositionInfoSource::constructor_noParent()
{
    //QLocationTestUtils::uheap_mark();
    MyPositionSource *obj = new MyPositionSource();
    delete obj;
    //QLocationTestUtils::uheap_mark_end();
}

void TestQGeoPositionInfoSource::updateInterval()
{
    MyPositionSource s;
    QCOMPARE(s.updateInterval(), 0);
}

void TestQGeoPositionInfoSource::setPreferredPositioningMethods()
{
    QFETCH(int, supported);
    QFETCH(int, preferred);
    QFETCH(int, resulting);

    MyPositionSource s;
    s.setSupportedPositioningMethods(
        static_cast<QGeoPositionInfoSource::PositioningMethods>(supported));
    s.setPreferredPositioningMethods(
        static_cast<QGeoPositionInfoSource::PositioningMethods>(preferred));
    QCOMPARE(s.preferredPositioningMethods(),
             static_cast<QGeoPositionInfoSource::PositioningMethods>(resulting));
}

void TestQGeoPositionInfoSource::setPreferredPositioningMethods_data()
{
    QTest::addColumn<int>("supported");
    QTest::addColumn<int>("preferred");
    QTest::addColumn<int>("resulting");

    QTest::newRow("Sat supported, Sat preferred")
    << int(QGeoPositionInfoSource::SatellitePositioningMethods)
    << int(QGeoPositionInfoSource::SatellitePositioningMethods)
    << int(QGeoPositionInfoSource::SatellitePositioningMethods);
    QTest::newRow("Sat supported, Non-Sat prefered")
    << int(QGeoPositionInfoSource::SatellitePositioningMethods)
    << int(QGeoPositionInfoSource::NonSatellitePositioningMethods)
    << int(QGeoPositionInfoSource::SatellitePositioningMethods);
    QTest::newRow("Sat supported, All prefered")
    << int(QGeoPositionInfoSource::SatellitePositioningMethods)
    << int(QGeoPositionInfoSource::AllPositioningMethods)
    << int(QGeoPositionInfoSource::SatellitePositioningMethods);

    QTest::newRow("Non-Sat supported, Sat preferred")
    << int(QGeoPositionInfoSource::NonSatellitePositioningMethods)
    << int(QGeoPositionInfoSource::SatellitePositioningMethods)
    << int(QGeoPositionInfoSource::NonSatellitePositioningMethods);
    QTest::newRow("Non-Sat supported, Non-Sat prefered")
    << int(QGeoPositionInfoSource::NonSatellitePositioningMethods)
    << int(QGeoPositionInfoSource::NonSatellitePositioningMethods)
    << int(QGeoPositionInfoSource::NonSatellitePositioningMethods);
    QTest::newRow("Non-Sat supported, All prefered")
    << int(QGeoPositionInfoSource::NonSatellitePositioningMethods)
    << int(QGeoPositionInfoSource::AllPositioningMethods)
    << int(QGeoPositionInfoSource::NonSatellitePositioningMethods);

    QTest::newRow("All supported, Sat preferred")
    << int(QGeoPositionInfoSource::AllPositioningMethods)
    << int(QGeoPositionInfoSource::SatellitePositioningMethods)
    << int(QGeoPositionInfoSource::SatellitePositioningMethods);
    QTest::newRow("All supported, Non-Sat prefered")
    << int(QGeoPositionInfoSource::AllPositioningMethods)
    << int(QGeoPositionInfoSource::NonSatellitePositioningMethods)
    << int(QGeoPositionInfoSource::NonSatellitePositioningMethods);
    QTest::newRow("All supported, All prefered")
    << int(QGeoPositionInfoSource::AllPositioningMethods)
    << int(QGeoPositionInfoSource::AllPositioningMethods)
    << int(QGeoPositionInfoSource::AllPositioningMethods);
}

void TestQGeoPositionInfoSource::preferredPositioningMethods()
{
    MyPositionSource s;
    QCOMPARE(s.preferredPositioningMethods(), 0);
}

//TC_ID_3_x_1 : Create a position source with the given parent that reads from the system's default
// sources of location data
void TestQGeoPositionInfoSource::createDefaultSource()
{
    //QLocationTestUtils::uheap_mark();
    QObject *parent = new QObject;

    QGeoPositionInfoSource *source = QGeoPositionInfoSource::createDefaultSource(parent);
#if defined(Q_OS_SYMBIAN)
    QVERIFY(source != 0);
#elif defined(Q_OS_WINCE)
    QVERIFY(source != 0);
#elif defined(Q_WS_MAEMO_5)
    QVERIFY(source != 0);
#else 
    QVERIFY(source == 0);
#endif
    delete parent;
    //QLocationTestUtils::uheap_mark_end();
}

void TestQGeoPositionInfoSource::setUpdateInterval()
{
    CHECK_SOURCE_VALID;

    QFETCH(int, interval);
    QFETCH(int, expectedInterval);

    m_source->setUpdateInterval(interval);
    QCOMPARE(m_source->updateInterval(), expectedInterval);
}

void TestQGeoPositionInfoSource::setUpdateInterval_data()
{
    QTest::addColumn<int>("interval");
    QTest::addColumn<int>("expectedInterval");

    QGeoPositionInfoSource *source = createTestSource();
    int minUpdateInterval = source ? source->minimumUpdateInterval() : -1;
    if (source)
        delete source;

    QTest::newRow("0") << 0 << 0;
    
    if (minUpdateInterval > -1) {
        QTest::newRow("INT_MIN") << INT_MIN << minUpdateInterval;
        QTest::newRow("-1") << -1 << minUpdateInterval;
    }

    if (minUpdateInterval > 0) {
        QTest::newRow("more than minInterval") << minUpdateInterval + 1 << minUpdateInterval + 1;
        QTest::newRow("equal to minInterval") << minUpdateInterval << minUpdateInterval;
    }
    
    if (minUpdateInterval > 1) {
        QTest::newRow("less then minInterval") << minUpdateInterval - 1 << minUpdateInterval;
        QTest::newRow("in btw zero and minInterval") << 1 << minUpdateInterval;
    }

    // Fails on S60, should investigate
    //QTest::newRow("INT_MAX") << INT_MAX << INT_MAX;
}

void TestQGeoPositionInfoSource::lastKnownPosition()
{
    CHECK_SOURCE_VALID;

    QFETCH(int, positioningMethod);
    QFETCH(bool, lastKnownPositionArgument);
    QFETCH(bool, positionValid);

    QGeoPositionInfoSource::PositioningMethods method
            = static_cast<QGeoPositionInfoSource::PositioningMethods>(positioningMethod);

    if ((m_source->supportedPositioningMethods() & method) == 0)
        QSKIP("Not a supported positioning method for this position source", SkipSingle);

    m_source->setPreferredPositioningMethods(method);

    QSignalSpy spy(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy timeout(m_source, SIGNAL(updateTimeout()));
    int time_out = 7000;
    m_source->setUpdateInterval(time_out);
    m_source->startUpdates();

    // Use QEventLoop instead of qWait() to ensure we stop as soon as a
    // position is emitted (otherwise the lastKnownPosition() may have
    // changed by the time it is checked)
    QEventLoop loop;
    QTimer timer;
    timer.setInterval(9500);
    connect(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)),
            &loop, SLOT(quit()));
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    timer.start();
    loop.exec();

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337_ABORT;

    QVERIFY((spy.count() > 0) && (timeout.count() == 0));

    QList<QVariant> list = spy.takeFirst();
    QGeoPositionInfo info;
    info = list.at(0).value<QGeoPositionInfo>();
    QGeoPositionInfo lastPositioninfo;
    lastPositioninfo = m_source->lastKnownPosition(lastKnownPositionArgument);
 
    QCOMPARE(lastPositioninfo.isValid(), positionValid);

    if (positionValid) {
        QCOMPARE(info.coordinate(), lastPositioninfo.coordinate());
        QCOMPARE(info.timestamp(), lastPositioninfo.timestamp());

        QCOMPARE(info.hasAttribute(QGeoPositionInfo::HorizontalAccuracy),
                 lastPositioninfo.hasAttribute(QGeoPositionInfo::HorizontalAccuracy));

        if (info.hasAttribute(QGeoPositionInfo::HorizontalAccuracy)) {
            bool isNaN1 =  qIsNaN(info.attribute(QGeoPositionInfo::HorizontalAccuracy));
            bool isNaN2 =  qIsNaN(lastPositioninfo.attribute(QGeoPositionInfo::HorizontalAccuracy));
            QCOMPARE(isNaN1, isNaN2);
            if (!isNaN1) {
                QCOMPARE(qFuzzyCompare(info.attribute(QGeoPositionInfo::HorizontalAccuracy),
                                       lastPositioninfo.attribute(QGeoPositionInfo::HorizontalAccuracy)), TRUE);
            }
        }

        QCOMPARE(info.hasAttribute(QGeoPositionInfo::VerticalAccuracy),
                 lastPositioninfo.hasAttribute(QGeoPositionInfo::VerticalAccuracy));

        if (info.hasAttribute(QGeoPositionInfo::VerticalAccuracy)) {
            bool isNaN1 =  qIsNaN(info.attribute(QGeoPositionInfo::VerticalAccuracy));
            bool isNaN2 =  qIsNaN(lastPositioninfo.attribute(QGeoPositionInfo::VerticalAccuracy));
            QCOMPARE(isNaN1, isNaN2);
            if (!isNaN1) {
                QCOMPARE(qFuzzyCompare(info.attribute(QGeoPositionInfo::VerticalAccuracy),
                                       lastPositioninfo.attribute(QGeoPositionInfo::VerticalAccuracy)), TRUE);
            }
        }
    }

    m_source->stopUpdates();
}

void TestQGeoPositionInfoSource::lastKnownPosition_data()
{
    QTest::addColumn<int>("positioningMethod");
    QTest::addColumn<bool>("lastKnownPositionArgument");
    QTest::addColumn<bool>("positionValid");

    QTest::newRow("satellite - false") << int(QGeoPositionInfoSource::SatellitePositioningMethods) << false << true;
    QTest::newRow("satellite - true") << int(QGeoPositionInfoSource::SatellitePositioningMethods) << true << true;
    QTest::newRow("nonsatellite - false") << int(QGeoPositionInfoSource::NonSatellitePositioningMethods) << false << false;
    QTest::newRow("nonsatellite - true") << int(QGeoPositionInfoSource::NonSatellitePositioningMethods) << true << true;
    QTest::newRow("all - false") << int(QGeoPositionInfoSource::AllPositioningMethods) << false << true;
    QTest::newRow("all - true") << int(QGeoPositionInfoSource::AllPositioningMethods) << true << true;
}

void TestQGeoPositionInfoSource::minimumUpdateInterval()
{
    CHECK_SOURCE_VALID;

    QVERIFY(m_source->minimumUpdateInterval() > 0);
}

//TC_ID_3_x_1
void TestQGeoPositionInfoSource::startUpdates_testIntervals()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spy(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy timeout(m_source, SIGNAL(updateTimeout()));
    m_source->setUpdateInterval(7000);
    int interval = m_source->updateInterval();

    m_source->startUpdates();

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_COMPARE_WITH_TIMEOUT(spy.count(), 1, 9500);
    for (int i = 0; i < 6; i++) {
        EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

        QTRY_VERIFY_WITH_TIMEOUT((spy.count() == 1) && (timeout.count() == 0), (interval*2));
        spy.clear();
    }

    m_source->stopUpdates();
}


void TestQGeoPositionInfoSource::startUpdates_testIntervalChangesWhileRunning()
{
    // There are two ways of dealing with an interval change, and we have left it system dependent.
    // The interval can be changed will running or after the next update.
    // WinCE uses the first method, S60 uses the second method.

    // The minimum interval on the symbian emulator is 5000 msecs, which is why the times in
    // this test are as high as they are.

    CHECK_SOURCE_VALID;

    QSignalSpy spy(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy timeout(m_source, SIGNAL(updateTimeout()));
    m_source->setUpdateInterval(0);
    m_source->startUpdates();
    m_source->setUpdateInterval(0);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT(spy.count() > 0, 7000);
    QCOMPARE(timeout.count(), 0);
    spy.clear();

    m_source->setUpdateInterval(5000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;
    QTRY_VERIFY_WITH_TIMEOUT((spy.count() == 2) && (timeout.count() == 0), 15000);
    spy.clear();

    m_source->setUpdateInterval(10000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;
    QTRY_VERIFY_WITH_TIMEOUT((spy.count() == 2) && (timeout.count() == 0), 30000);
    spy.clear();

    m_source->setUpdateInterval(5000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;
    QTRY_VERIFY_WITH_TIMEOUT((spy.count() == 2) && (timeout.count() == 0), 15000);
    spy.clear();

    m_source->setUpdateInterval(5000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;
    QTRY_VERIFY_WITH_TIMEOUT((spy.count() == 2) && (timeout.count() == 0), 15000);
    spy.clear();

    m_source->setUpdateInterval(0);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;
    QTRY_VERIFY_WITH_TIMEOUT((spy.count() == 1) && (timeout.count() == 0), 7000);
    spy.clear();

    m_source->setUpdateInterval(0);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;
    QTRY_VERIFY_WITH_TIMEOUT((spy.count() == 1) && (timeout.count() == 0), 7000);
    spy.clear();

    m_source->stopUpdates();
}

//TC_ID_3_x_2
void TestQGeoPositionInfoSource::startUpdates_testDefaultInterval()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spy(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy timeout(m_source, SIGNAL(updateTimeout()));
    m_source->startUpdates();
    for (int i = 0; i < 3; i++) {
        EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

        QTRY_VERIFY_WITH_TIMEOUT((spy.count() > 0) && (timeout.count() == 0), 7000);
        spy.clear();
    }
    m_source->stopUpdates();
}

//TC_ID_3_x_3
void TestQGeoPositionInfoSource::startUpdates_testZeroInterval()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spy(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy timeout(m_source, SIGNAL(updateTimeout()));
    m_source->setUpdateInterval(0);
    m_source->startUpdates();
    for (int i = 0; i < 3; i++) {
        EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

        QTRY_VERIFY_WITH_TIMEOUT((spy.count() > 0) && (timeout.count() == 0), 7000);
        spy.clear();
    }
    m_source->stopUpdates();
}

void TestQGeoPositionInfoSource::startUpdates_moreThanOnce()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spy(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy timeout(m_source, SIGNAL(updateTimeout()));
    m_source->setUpdateInterval(0);
    m_source->startUpdates();

    m_source->startUpdates(); // check there is no crash

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spy.count() > 0) && (timeout.count() == 0), 7000);

    m_source->startUpdates(); // check there is no crash

    m_source->stopUpdates();
}

//TC_ID_3_x_1
void TestQGeoPositionInfoSource::stopUpdates()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spy(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy timeout(m_source, SIGNAL(updateTimeout()));
    m_source->setUpdateInterval(7000);
    m_source->startUpdates();
    for (int i = 0; i < 2; i++) {
        EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

        QTRY_VERIFY_WITH_TIMEOUT((spy.count() > 0) && (timeout.count() == 0), 9500);
        spy.clear();
    }
    m_source->stopUpdates();
    QTest::qWait(9500);
    QCOMPARE(spy.count(), 0);
    spy.clear();

    m_source->setUpdateInterval(0);
    m_source->startUpdates();
    m_source->stopUpdates();
    QTRY_COMPARE_WITH_TIMEOUT(spy.count(), 0, 9500);
}

//TC_ID_3_x_2
void TestQGeoPositionInfoSource::stopUpdates_withoutStart()
{
    CHECK_SOURCE_VALID;

    m_source->stopUpdates(); // check there is no crash
}

void TestQGeoPositionInfoSource::requestUpdate()
{
    CHECK_SOURCE_VALID;

    QFETCH(int, timeout);
    QSignalSpy spy(m_source, SIGNAL(updateTimeout()));
    m_source->requestUpdate(timeout);
    QTRY_COMPARE(spy.count(), 1);
}

void TestQGeoPositionInfoSource::requestUpdate_data()
{
    QTest::addColumn<int>("timeout");
    QTest::newRow("less than zero") << -1;          //TC_ID_3_x_7
}

// TC_ID_3_x_1 : Create position source and call requestUpdate with valid timeout value
void TestQGeoPositionInfoSource::requestUpdate_validTimeout()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyUpdate(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy spyTimeout(m_source, SIGNAL(updateTimeout()));

    m_source->requestUpdate(7000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyUpdate.count() > 0) && (spyTimeout.count() == 0), 7000);
}

void TestQGeoPositionInfoSource::requestUpdate_defaultTimeout()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyUpdate(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy spyTimeout(m_source, SIGNAL(updateTimeout()));

    m_source->requestUpdate(0);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    // S60 emulator fail
    QTRY_VERIFY_WITH_TIMEOUT((spyUpdate.count() > 0) && (spyTimeout.count() == 0), 7000);
}

// TC_ID_3_x_2 : Create position source and call requestUpdate with a timeout less than
// minimumupdateInterval
void TestQGeoPositionInfoSource::requestUpdate_timeoutLessThanMinimumInterval()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyTimeout(m_source, SIGNAL(updateTimeout()));
    m_source->requestUpdate(1);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_COMPARE_WITH_TIMEOUT(spyTimeout.count(), 1, 1000);
}

// TC_ID_3_x_3 : Call requestUpdate() with same value repeatedly
void TestQGeoPositionInfoSource::requestUpdate_repeatedCalls()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyUpdate(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy spyTimeout(m_source, SIGNAL(updateTimeout()));

    m_source->requestUpdate(7000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyUpdate.count() > 0) && (spyTimeout.count() == 0), 7000);
    spyUpdate.clear();
    m_source->requestUpdate(7000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyUpdate.count() > 0) && (spyTimeout.count() == 0), 7000);
}

void TestQGeoPositionInfoSource::requestUpdate_overlappingCalls()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyUpdate(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy spyTimeout(m_source, SIGNAL(updateTimeout()));

    m_source->requestUpdate(7000);
    m_source->requestUpdate(7000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyUpdate.count() > 0) && (spyTimeout.count() == 0), 7000);
}

//TC_ID_3_x_4
void TestQGeoPositionInfoSource::requestUpdateAfterStartUpdates_ZeroInterval()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyUpdate(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy spyTimeout(m_source, SIGNAL(updateTimeout()));

    m_source->setUpdateInterval(0);
    m_source->startUpdates();

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyUpdate.count() > 0) && (spyTimeout.count() == 0), 7000);
    spyUpdate.clear();

    m_source->requestUpdate(7000);
    QTest::qWait(7000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QVERIFY((spyUpdate.count() > 0) && (spyTimeout.count() == 0));
    spyUpdate.clear();

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyUpdate.count() > 0) && (spyTimeout.count() == 0), MAX_WAITING_TIME);

    m_source->stopUpdates();
}

void TestQGeoPositionInfoSource::requestUpdateAfterStartUpdates_SmallInterval()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyUpdate(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy spyTimeout(m_source, SIGNAL(updateTimeout()));

    m_source->setUpdateInterval(10000);
    m_source->startUpdates();

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyUpdate.count() == 1) && (spyTimeout.count() == 0), 20000);
    spyUpdate.clear();

    m_source->requestUpdate(7000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyUpdate.count() == 1) && (spyTimeout.count() == 0), 7000);
    spyUpdate.clear();

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyUpdate.count() == 1) && (spyTimeout.count() == 0), 20000);

    m_source->stopUpdates();
}

void TestQGeoPositionInfoSource::requestUpdateBeforeStartUpdates_ZeroInterval()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyUpdate(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy spyTimeout(m_source, SIGNAL(updateTimeout()));

    m_source->requestUpdate(7000);

    m_source->setUpdateInterval(0);
    m_source->startUpdates();

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyUpdate.count() >= 2) && (spyTimeout.count() == 0), 14000);
    spyUpdate.clear();

    QTest::qWait(7000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QCOMPARE(spyTimeout.count(), 0);

    m_source->stopUpdates();
}

void TestQGeoPositionInfoSource::requestUpdateBeforeStartUpdates_SmallInterval()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyUpdate(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy spyTimeout(m_source, SIGNAL(updateTimeout()));

    m_source->requestUpdate(7000);

    m_source->setUpdateInterval(10000);
    m_source->startUpdates();

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyUpdate.count() > 0) && (spyTimeout.count() == 0), 7000);
    spyUpdate.clear();

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyUpdate.count() > 0) && (spyTimeout.count() == 0), 20000);

    m_source->stopUpdates();
}

void TestQGeoPositionInfoSource::removeSlotForRequestTimeout()
{
    CHECK_SOURCE_VALID;

    bool i = connect(m_source, SIGNAL(updateTimeout()), this, SLOT(test_slot1()));
    QVERIFY(i == true);
    i = connect(m_source, SIGNAL(updateTimeout()), this, SLOT(test_slot2()));
    QVERIFY(i == true);
    i = disconnect(m_source, SIGNAL(updateTimeout()), this, SLOT(test_slot1()));
    QVERIFY(i == true);

    m_source->requestUpdate(-1);
    QTRY_VERIFY_WITH_TIMEOUT((m_testSlot2Called == true), 1000);
}

void TestQGeoPositionInfoSource::removeSlotForPositionUpdated()
{
    CHECK_SOURCE_VALID;

    bool i = connect(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo &)), this, SLOT(test_slot1()));
    QVERIFY(i == true);
    i = connect(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo &)), this, SLOT(test_slot2()));
    QVERIFY(i == true);
    i = disconnect(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo &)), this, SLOT(test_slot1()));
    QVERIFY(i == true);

    m_source->requestUpdate(7000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((m_testSlot2Called == true), 7000);
}

#include "testqgeopositioninfosource.moc"
