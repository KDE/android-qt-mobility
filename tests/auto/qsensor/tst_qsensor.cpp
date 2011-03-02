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

//TESTED_COMPONENT=src/sensors

#include <QObject>
#include <QTest>
#include <QDebug>
#include <QSettings>
#include <QFile>

#include "qsensor.h"
#include "test_sensor.h"
#include "test_sensor2.h"
#include "test_sensorimpl.h"
#include "test_backends.h"

// The unit test needs to change the behaviour of the library. It does this
// through an exported but undocumented function.
QTM_BEGIN_NAMESPACE
Q_SENSORS_EXPORT void sensors_unit_test_hook(int index);
bool operator==(const qoutputrange &orl1, const qoutputrange &orl2)
{
    return (orl1.minimum == orl2.minimum &&
            orl1.maximum == orl2.maximum &&
            orl1.accuracy == orl2.accuracy);
}
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

namespace QTest {
    template<> char *toString(const qoutputrangelist &orl)
    {
        QStringList list;
        foreach (const qoutputrange &item, orl) {
            list << QString("%1-%2%3%4").arg(item.minimum).arg(item.maximum).arg(QString::fromWCharArray(L"\u00B1")).arg(item.accuracy);
        }
        QString ret = QString("qoutputrangelist: (%1)").arg(list.join("), ("));
        return qstrdup(ret.toLatin1().data());
    }
    template<> char *toString(const QList<QByteArray> &data)
    {
        QStringList list;
        foreach (const QByteArray &str, data) {
            list << QString::fromLatin1(str);
        }
        QString ret = QString("QList<QByteArray>: (%1)").arg(list.join("), ("));
        return qstrdup(ret.toLatin1().data());
    }
}


class MyFilter : public TestSensorFilter { bool filter(TestSensorReading *) { return false; } };

class ModFilter : public TestSensorFilter
{
    bool filter(TestSensorReading *reading)
    {
        reading->setTest(3);
        return true;
    }
};

class MyFactory : public QSensorBackendFactory
{
    QSensorBackend *createBackend(QSensor * /*sensor*/)
    {
        return 0;
    }
};

/*
    Unit test for QSensor class.
*/
class tst_QSensor : public QObject
{
    Q_OBJECT

public:
    tst_QSensor()
    {
        sensors_unit_test_hook(0); // change some flags the library uses
    }

private slots:
    void initTestCase()
    {
        QSettings settings(QLatin1String("Nokia"), QLatin1String("Sensors"));
        settings.clear();
    }

    void cleanupTestCase()
    {
        QSettings settings(QLatin1String("Nokia"), QLatin1String("Sensors"));
        settings.clear();

#ifdef WAIT_AT_END
        QFile _stdin;
        _stdin.open(1, QIODevice::ReadOnly);
        _stdin.readLine();
#endif
    }

    // This test MUST be first
    void testRecursiveLoadPlugins()
    {
        TestSensor sensor;

        // This confirms that legacy static plugins can still be registered
        QTest::ignoreMessage(QtWarningMsg, "Loaded the LegacySensorPlugin ");

        // The logic for the test is in test_sensorplugin.cpp (which warns and aborts if the test fails)
        (void)QSensor::sensorTypes();

        // Checking that the availableSensorsChanged() signal was not emitted too many times while loading plugins.
        QCOMPARE(sensor.sensorsChangedEmitted, 1);
    }

    void testTypeRegistered()
    {
        QList<QByteArray> expected;
        expected << TestSensor::type << TestSensor2::type;
        QList<QByteArray> actual = QSensor::sensorTypes();
        qSort(actual); // The actual list is not in a defined order
        QCOMPARE(actual, expected);
    }

    void testSensorRegistered()
    {
        QList<QByteArray> expected;
        expected << "test sensor 2" << "test sensor 3" << testsensorimpl::id;
        QList<QByteArray> actual = QSensor::sensorsForType(TestSensor::type);
        qSort(actual); // The actual list is not in a defined order
        QCOMPARE(actual, expected);
    }

    void testSensorDefault()
    {
        QByteArray expected = testsensorimpl::id;
        QByteArray actual = QSensor::defaultSensorForType(TestSensor::type);
        QCOMPARE(actual, expected);
    }

    void testBadDefaultFromConfig()
    {
        QSettings settings(QLatin1String("Nokia"), QLatin1String("Sensors"));
        settings.setValue(QString(QLatin1String("Default/%1")).arg(QString::fromLatin1(TestSensor::type)), QByteArray("bogus id"));
        settings.sync();

        QByteArray expected = testsensorimpl::id;
        QByteArray actual = QSensor::defaultSensorForType(TestSensor::type);
        QCOMPARE(actual, expected);
    }

    void testGoodDefaultFromConfig()
    {
        QSettings settings(QLatin1String("Nokia"), QLatin1String("Sensors"));
        settings.setValue(QString(QLatin1String("Default/%1")).arg(QString::fromLatin1(TestSensor::type)), QByteArray(testsensorimpl::id));
        settings.sync();

        QByteArray expected = testsensorimpl::id;
        QByteArray actual = QSensor::defaultSensorForType(TestSensor::type);
        QCOMPARE(actual, expected);

        settings.clear();
    }

    void testNoSensorsForType()
    {
        QList<QByteArray> expected;
        QList<QByteArray> actual = QSensor::sensorsForType("bogus type");
        QCOMPARE(actual, expected);
    }

    void testNoDefaultForType()
    {
        QByteArray expected;
        QByteArray actual = QSensor::defaultSensorForType("bogus type");
        QCOMPARE(actual, expected);
    }

    void testCreation()
    {
        TestSensor sensor;
        sensor.connectToBackend();
        QByteArray expected = testsensorimpl::id;
        QByteArray actual = sensor.identifier();
        QCOMPARE(actual, expected);
    }

    void testSetIdentifierFail()
    {
        TestSensor sensor;
        sensor.setIdentifier(testsensorimpl::id);
        sensor.connectToBackend();
        QVERIFY(sensor.isConnectedToBackend());
        QByteArray expected = testsensorimpl::id;
        QByteArray actual = sensor.identifier();
        QCOMPARE(actual, expected);

        QTest::ignoreMessage(QtWarningMsg, "ERROR: Cannot call QSensor::setIdentifier while connected to a backend! ");
        sensor.setIdentifier("dummy.accelerometer");
        expected = testsensorimpl::id;
        actual = sensor.identifier();
        QCOMPARE(actual, expected);
    }

    void testBadDefaultCreation()
    {
        QSettings settings(QLatin1String("Nokia"), QLatin1String("Sensors"));
        settings.setValue(QString(QLatin1String("Default/%1")).arg(QString::fromLatin1(TestSensor::type)), QByteArray("test sensor 2"));
        settings.sync();

        TestSensor sensor;
        QTest::ignoreMessage(QtWarningMsg, "Can't create backend \"test sensor 2\" ");
        sensor.connectToBackend();
        QByteArray expected = testsensorimpl::id;
        QByteArray actual = sensor.identifier();
        QCOMPARE(actual, expected);

        settings.clear();
    }

    void testBadCreation()
    {
        QSensor sensor("bogus type");
        sensor.connectToBackend();
        QByteArray expected; // should be null
        QByteArray actual = sensor.identifier();
        QCOMPARE(actual, expected);
    }

    void testTimestamp()
    {
        TestSensor sensor;
        sensor.connectToBackend();
        QVERIFY(sensor.reading() != 0);
        quint64 timestamp = sensor.reading()->timestamp();
        QVERIFY(timestamp == qtimestamp());
        sensor.setProperty("doThis", "setOne");
        sensor.start();
        timestamp = sensor.reading()->timestamp();
        QVERIFY(timestamp == 1);
    }

    void testStart()
    {
        TestSensor sensor;
        sensor.start();
        QVERIFY(sensor.isActive());
        sensor.start();
        QVERIFY(sensor.isActive());
    }

    void testBadStart()
    {
        QSensor sensor("bogus type");
        sensor.start();
        QVERIFY(!sensor.isActive());
    }

    void testStop()
    {
        TestSensor sensor;
        sensor.stop();
        QVERIFY(!sensor.isActive());
        sensor.start();
        QVERIFY(sensor.isActive());
        sensor.stop();
        QVERIFY(!sensor.isActive());
    }

    void testMetaData()
    {
        TestSensor sensor;

        {
            bool actual = sensor.isConnectedToBackend();
            bool expected = false;
            QCOMPARE(actual, expected);
        }

        sensor.connectToBackend();

        {
            bool actual = sensor.isConnectedToBackend();
            bool expected = true;
            QCOMPARE(actual, expected);
        }

        {
            QString actual = sensor.description();
            QString expected = "sensor description";
            QCOMPARE(actual, expected);
        }

        {
            qoutputrangelist actual = sensor.outputRanges();
            qoutputrangelist expected;
            qoutputrange r; r.minimum = 0; r.maximum = 1; r.accuracy = 0.5;
            expected << r;
            r.minimum = 0; r.maximum = 2; r.accuracy = 1;
            expected << r;
            QCOMPARE(actual, expected);
        }

        {
            int actual = sensor.outputRange();
            int expected = -1;
            QCOMPARE(actual, expected);

            sensor.setOutputRange(0);

            actual = sensor.outputRange();
            expected = 0;
            QCOMPARE(actual, expected);
        }

        {
            qrangelist actual = sensor.availableDataRates();
            qrangelist expected = qrangelist() << qrange(100,100);
            QCOMPARE(actual, expected);
        }

        {
            TestSensor sensor;
            sensor.setProperty("doThis", "rates");
            sensor.connectToBackend();
            qrangelist actual = sensor.availableDataRates();
            qrangelist expected = qrangelist() << qrange(100,100);
            QCOMPARE(actual, expected);
        }

        {
            TestSensor sensor;
            sensor.setProperty("doThis", "rates(0)");
            QTest::ignoreMessage(QtWarningMsg, "ERROR: Cannot call QSensorBackend::setDataRates with 0 ");
            sensor.connectToBackend();
        }

        {
            TestSensor sensor;
            sensor.setProperty("doThis", "rates(nodef)");
            QTest::ignoreMessage(QtWarningMsg, "ERROR: Cannot call QSensorBackend::setDataRates with an invalid sensor ");
            sensor.connectToBackend();
        }

        {
            int actual = sensor.dataRate();
            int expected = 0;
            QCOMPARE(actual, expected);

            sensor.setDataRate(100);

            actual = sensor.dataRate();
            expected = 100;
            QCOMPARE(actual, expected);
        }

        // Test the generic accessor functions
        TestSensorReading *reading = sensor.reading();
        QCOMPARE(reading->valueCount(), 1);
        reading->setTest(1);
        QCOMPARE(reading->test(), reading->value(0).toInt());
    }

    void testFilter()
    {
        TestSensor sensor;
        sensor.connectToBackend();

        QList<QSensorFilter*> actual = sensor.filters();
        QList<QSensorFilter*> expected = QList<QSensorFilter*>();
        QCOMPARE(actual, expected);

        QTest::ignoreMessage(QtWarningMsg, "addFilter: passed a null filter! ");
        sensor.addFilter(0);

        QTest::ignoreMessage(QtWarningMsg, "removeFilter: passed a null filter! ");
        sensor.removeFilter(0);

        MyFilter *filter = new MyFilter;
        sensor.addFilter(filter);

        actual = sensor.filters();
        expected = QList<QSensorFilter*>() << filter;
        QCOMPARE(actual, expected);

        MyFilter *filter2 = new MyFilter;
        sensor.addFilter(filter2);

        actual = sensor.filters();
        expected = QList<QSensorFilter*>() << filter << filter2;
        QCOMPARE(actual, expected);

        delete filter2;

        actual = sensor.filters();
        expected = QList<QSensorFilter*>() << filter;
        QCOMPARE(actual, expected);

        sensor.removeFilter(filter);

        actual = sensor.filters();
        expected = QList<QSensorFilter*>();
        QCOMPARE(actual, expected);

        delete filter;
    }

    void testFilter2()
    {
        TestSensor sensor;
        sensor.setProperty("doThis", "setOne");
        TestSensorFilter *filter1 = new ModFilter;
        TestSensorFilter *filter2 = new MyFilter;
        sensor.addFilter(filter1);
        sensor.start();
        QCOMPARE(sensor.reading()->test(), 3);
        sensor.stop();
        sensor.reading()->setTest(1);
        sensor.addFilter(filter2);
        sensor.start();
        QCOMPARE(sensor.reading()->test(), 1);
        sensor.stop();
        delete filter1;
        delete filter2;
    }

    void testStart2()
    {
        TestSensor sensor;
        sensor.connectToBackend();

        sensor.setProperty("doThis", "busy");
        sensor.start();
        QVERIFY(sensor.isBusy());
        QVERIFY(!sensor.isActive());
        sensor.stop();

        sensor.setProperty("doThis", "stop");
        sensor.start();
        QVERIFY(!sensor.isActive());
        sensor.stop();

        sensor.setProperty("doThis", "error");
        sensor.start();
        QVERIFY(sensor.error() == 1);
        // Yes, this is non-intuitive but the sensor
        // decides if an error is fatal or not.
        // In this case our test sensor is reporting a
        // non-fatal error so the sensor will start.
        QVERIFY(sensor.isActive());
        sensor.stop();

        sensor.setProperty("doThis", "setOne");
        sensor.start();
        QCOMPARE(sensor.reading()->timestamp(), qtimestamp(1));
        QCOMPARE(sensor.reading()->test(), 1);
        sensor.stop();

        sensor.setProperty("doThis", "setTwo");
        sensor.start();
        QCOMPARE(sensor.reading()->timestamp(), qtimestamp(2));
        QCOMPARE(sensor.reading()->test(), 2);
        sensor.stop();
    }

    void testSetBadDataRate()
    {
        TestSensor sensor;
        sensor.connectToBackend();

        QTest::ignoreMessage(QtWarningMsg, "setDataRate: 1 is not supported by the sensor. ");
        sensor.setDataRate(1);
        QCOMPARE(sensor.dataRate(), 0);

        QTest::ignoreMessage(QtWarningMsg, "setDataRate: 1000 is not supported by the sensor. ");
        sensor.setDataRate(1000);
        QCOMPARE(sensor.dataRate(), 0);
    }

    void testSetBadDataRateWhenNotConnected()
    {
        TestSensor sensor;
        sensor.setDataRate(0);
        QCOMPARE(sensor.dataRate(), 0);
        sensor.setDataRate(300);
        QCOMPARE(sensor.dataRate(), 300);
        sensor.setDataRate(350);
        QTest::ignoreMessage(QtWarningMsg, "setDataRate: 350 is not supported by the sensor. ");
        sensor.connectToBackend();
        QCOMPARE(sensor.dataRate(), 0);
    }

    void testSetBadOutputRange()
    {
        TestSensor sensor;
        sensor.connectToBackend();

        sensor.setOutputRange(-1);
        QCOMPARE(sensor.outputRange(), -1);
        QTest::ignoreMessage(QtWarningMsg, "setOutputRange: 300 is not supported by the sensor. ");
        sensor.setOutputRange(300);
        QCOMPARE(sensor.outputRange(), -1);
    }

    void testSetBadOutputRangeWhenNotConnected()
    {
        TestSensor sensor;
        sensor.setOutputRange(300);
        QCOMPARE(sensor.outputRange(), 300);
        sensor.setOutputRange(350);
        QTest::ignoreMessage(QtWarningMsg, "setOutputRange: 350 is not supported by the sensor. ");
        sensor.connectToBackend();
        QCOMPARE(sensor.outputRange(), -1);
        QTest::ignoreMessage(QtWarningMsg, "setOutputRange: -2 is not supported by the sensor. ");
        sensor.setOutputRange(-2);
        QCOMPARE(sensor.outputRange(), -1);
    }

    void testEnumHandling()
    {
        {
            QAmbientLightReading reading;
            for (int i = 0; i <= 6; i++) {
                QAmbientLightReading::LightLevel setting = static_cast<QAmbientLightReading::LightLevel>(i);
                QAmbientLightReading::LightLevel expected = setting;
                if (i == 6)
                    expected = QAmbientLightReading::Undefined;
                reading.setLightLevel(setting);
                QCOMPARE(reading.lightLevel(), expected);
            }
        }

        {
            QOrientationReading reading;
            for (int i = 0; i <= 7; i++) {
                QOrientationReading::Orientation setting = static_cast<QOrientationReading::Orientation>(i);
                QOrientationReading::Orientation expected = setting;
                if (i == 7)
                    expected = QOrientationReading::Undefined;
                reading.setOrientation(setting);
                QCOMPARE(reading.orientation(), expected);
            }
        }

        {
            QTapReading reading;
            reading.setTapDirection(QTapReading::Undefined);
            QCOMPARE(reading.tapDirection(), QTapReading::Undefined);
            reading.setTapDirection(QTapReading::X_Pos);
            QCOMPARE(reading.tapDirection(), QTapReading::X_Pos);
            reading.setTapDirection(QTapReading::X_Neg);
            QCOMPARE(reading.tapDirection(), QTapReading::X_Neg);
            reading.setTapDirection(QTapReading::Y_Pos);
            QCOMPARE(reading.tapDirection(), QTapReading::Y_Pos);
            reading.setTapDirection(QTapReading::Y_Neg);
            QCOMPARE(reading.tapDirection(), QTapReading::Y_Neg);
            reading.setTapDirection(QTapReading::Z_Pos);
            QCOMPARE(reading.tapDirection(), QTapReading::Z_Pos);
            reading.setTapDirection(QTapReading::Z_Neg);
            QCOMPARE(reading.tapDirection(), QTapReading::Z_Neg);
            // Directions can be ORed together
            reading.setTapDirection(QTapReading::X_Both);
            QCOMPARE(reading.tapDirection(), QTapReading::X_Both);
            reading.setTapDirection(QTapReading::Y_Both);
            QCOMPARE(reading.tapDirection(), QTapReading::Y_Both);
            reading.setTapDirection(QTapReading::Z_Both);
            QCOMPARE(reading.tapDirection(), QTapReading::Z_Both);
            // You can't set just the Axis
            reading.setTapDirection(QTapReading::X);
            QCOMPARE(reading.tapDirection(), QTapReading::Undefined);
            reading.setTapDirection(QTapReading::Y);
            QCOMPARE(reading.tapDirection(), QTapReading::Undefined);
            reading.setTapDirection(QTapReading::Z);
            QCOMPARE(reading.tapDirection(), QTapReading::Undefined);
            reading.setTapDirection(static_cast<QTapReading::TapDirection>(0x1000));
            QCOMPARE(reading.tapDirection(), QTapReading::Undefined);
        }
    }

    void testDynamicDefaultsAndGenericHandling()
    {
        QByteArray expected;
        QByteArray actual;
        MyFactory factory;

        // The default for this type is null
        expected = QByteArray();
        actual = QSensor::defaultSensorForType("random");
        QCOMPARE(expected, actual);

        // Register a bogus backend
        QSensorManager::registerBackend("random", "generic.random", &factory);

        // The default for this type is the newly-registered backend
        expected = "generic.random";
        actual = QSensor::defaultSensorForType("random");
        QCOMPARE(expected, actual);

        // Register a non-generic bogus backend
        QSensorManager::registerBackend("random", "not.generic.random", &factory);

        // The default for this type is the newly-registered backend
        expected = "not.generic.random";
        actual = QSensor::defaultSensorForType("random");
        QCOMPARE(expected, actual);

        // Unregister the non-generic bogus backend
        QSensorManager::unregisterBackend("random", "not.generic.random");

        // The default for this type is the generic backend
        expected = "generic.random";
        actual = QSensor::defaultSensorForType("random");
        QCOMPARE(expected, actual);

        // Unregister a bogus backend
        QSensorManager::unregisterBackend("random", "generic.random");

        // The default for this type is null again
        expected = QByteArray();
        actual = QSensor::defaultSensorForType("random");
        QCOMPARE(expected, actual);


        // Now test out some more of the logic
        // Register 2 backends and unregister the first.
        QSensorManager::registerBackend("random", "random.1", &factory);
        expected = "random.1";
        actual = QSensor::defaultSensorForType("random");
        QCOMPARE(expected, actual);
        QSensorManager::registerBackend("random", "random.2", &factory);
        expected = "random.1";
        actual = QSensor::defaultSensorForType("random");
        QCOMPARE(expected, actual);
        QSensorManager::unregisterBackend("random", "random.1");
        expected = "random.2";
        actual = QSensor::defaultSensorForType("random");
        QCOMPARE(expected, actual);
        QSensorManager::unregisterBackend("random", "random.2");
        expected = QByteArray();
        actual = QSensor::defaultSensorForType("random");
        QCOMPARE(expected, actual);

        // Now stick a generic backend into the mix and ensure the correct thing happens
        QSensorManager::registerBackend("random", "random.1", &factory);
        expected = "random.1";
        actual = QSensor::defaultSensorForType("random");
        QCOMPARE(expected, actual);
        QSensorManager::registerBackend("random", "generic.random.2", &factory);
        expected = "random.1";
        actual = QSensor::defaultSensorForType("random");
        QCOMPARE(expected, actual);
        QSensorManager::registerBackend("random", "random.2", &factory);
        expected = "random.1";
        actual = QSensor::defaultSensorForType("random");
        QCOMPARE(expected, actual);
        QSensorManager::unregisterBackend("random", "random.1");
        expected = "random.2";
        actual = QSensor::defaultSensorForType("random");
        QCOMPARE(expected, actual);
        QSensorManager::unregisterBackend("random", "generic.random.2");
        expected = "random.2";
        actual = QSensor::defaultSensorForType("random");
        QCOMPARE(expected, actual);
        QSensorManager::unregisterBackend("random", "random.2");
        expected = QByteArray();
        actual = QSensor::defaultSensorForType("random");
        QCOMPARE(expected, actual);
    }

    void testCreation2()
    {
        MyFactory factory;

        QSensorManager::registerBackend("random", "random.1", &factory);
        QSensorManager::registerBackend("random", "random.2", &factory);
        QSensor random("random");
        // This is a sensorlog, not a warning
        //QTest::ignoreMessage(QtWarningMsg, "no suitable backend found for requested identifier \"\" and type \"random\" ");
        random.connectToBackend();
        QVERIFY(!random.isConnectedToBackend());
        random.setIdentifier("random.3");
        // This is a sensorlog, not a warning
        //QTest::ignoreMessage(QtWarningMsg, "no backend with identifier \"random.3\" for type \"random\" ");
        random.connectToBackend();
        QVERIFY(!random.isConnectedToBackend());
        random.setIdentifier("random.1");
        random.connectToBackend();
        QVERIFY(!random.isConnectedToBackend());
        QSensorManager::unregisterBackend("random", "random.1");
        QSensorManager::unregisterBackend("random", "random.2");
    }

    void testSensorsChangedSignal()
    {
        TestSensor sensor;
        MyFactory factory;

        // Register a bogus backend
        sensor.sensorsChangedEmitted = 0;
        QSensorManager::registerBackend("a random type", "a random id", &factory);
        QCOMPARE(sensor.sensorsChangedEmitted, 1);

        // Register it again (creates a warning)
        sensor.sensorsChangedEmitted = 0;
        QTest::ignoreMessage(QtWarningMsg, "A backend with type \"a random type\" and identifier \"a random id\" has already been registered! ");
        QSensorManager::registerBackend("a random type", "a random id", &factory);
        QCOMPARE(sensor.sensorsChangedEmitted, 0);

        // Unregister a bogus backend
        sensor.sensorsChangedEmitted = 0;
        QSensorManager::unregisterBackend("a random type", "a random id");
        QCOMPARE(sensor.sensorsChangedEmitted, 1);

        // Unregister an unknown identifier
        sensor.sensorsChangedEmitted = 0;
        QTest::ignoreMessage(QtWarningMsg, "Identifier \"a random id\" is not registered ");
        QSensorManager::unregisterBackend(TestSensor::type, "a random id");
        QCOMPARE(sensor.sensorsChangedEmitted, 0);

        // Unregister for an unknown type
        sensor.sensorsChangedEmitted = 0;
        QTest::ignoreMessage(QtWarningMsg, "No backends of type \"foo\" are registered ");
        QSensorManager::unregisterBackend("foo", "bar");
        QCOMPARE(sensor.sensorsChangedEmitted, 0);

        // Make sure we've cleaned up the list of available types
        QList<QByteArray> expected;
        expected << TestSensor::type << TestSensor2::type;
        QList<QByteArray> actual = QSensor::sensorTypes();
        qSort(actual); // The actual list is not in a defined order
        QCOMPARE(actual, expected);
    }

    void testSetActive()
    {
        TestSensor sensor;
        sensor.setActive(true);
        // doesn't start till the event loop is hit
        QVERIFY(!sensor.isActive());
        // hit the event loop
        QTest::qWait(0);
        QVERIFY(sensor.isActive());
        sensor.setActive(true);
        QVERIFY(sensor.isActive());
        // it does stop immediately
        sensor.setActive(false);
        QVERIFY(!sensor.isActive());
    }

    void testIsRegistered()
    {
        bool expected;
        bool actual;

        expected = true;
        actual = QSensorManager::isBackendRegistered(TestSensor::type, testsensorimpl::id);
        QCOMPARE(expected, actual);

        expected = false;
        actual = QSensorManager::isBackendRegistered(TestSensor::type, "random");
        QCOMPARE(expected, actual);

        expected = false;
        actual = QSensorManager::isBackendRegistered("random", "random");
        QCOMPARE(expected, actual);
    }

    void testAllTheInterfaces()
    {
        register_test_backends();

        TEST_SENSORINTERFACE(QAccelerometer, QAccelerometerReading, {
            QCOMPARE(reading->x(), 1.0);
            QCOMPARE(reading->y(), 1.0);
            QCOMPARE(reading->z(), 1.0);
        })

        TEST_SENSORINTERFACE(QAmbientLightSensor, QAmbientLightReading, {
            QCOMPARE(reading->lightLevel(), QAmbientLightReading::Twilight);
        })

        TEST_SENSORINTERFACE(QCompass, QCompassReading, {
            QCOMPARE(reading->azimuth(), 1.0);
            QCOMPARE(reading->calibrationLevel(), 1.0);
        })

        TEST_SENSORINTERFACE(QGyroscope, QGyroscopeReading, {
            QCOMPARE(reading->x(), 1.0);
            QCOMPARE(reading->y(), 1.0);
            QCOMPARE(reading->z(), 1.0);
        })

        TEST_SENSORINTERFACE(QLightSensor, QLightReading, {
            QCOMPARE(reading->lux(), 1.0);
        })

        TEST_SENSORINTERFACE(QMagnetometer, QMagnetometerReading, {
            QCOMPARE(reading->x(), 1.0);
            QCOMPARE(reading->y(), 1.0);
            QCOMPARE(reading->z(), 1.0);
            QCOMPARE(reading->calibrationLevel(), 1.0);
        })

        TEST_SENSORINTERFACE(QOrientationSensor, QOrientationReading, {
            QCOMPARE(reading->orientation(), QOrientationReading::LeftUp);
        })

        TEST_SENSORINTERFACE(QProximitySensor, QProximityReading, {
            QCOMPARE(reading->close(), true);
        })

        TEST_SENSORINTERFACE(QRotationSensor, QRotationReading, {
            QCOMPARE(reading->x(), 1.0);
            QCOMPARE(reading->y(), 1.0);
            QCOMPARE(reading->z(), 1.0);
        })

        TEST_SENSORINTERFACE(QTapSensor, QTapReading, {
            QCOMPARE(reading->tapDirection(), QTapReading::Z_Both);
            QCOMPARE(reading->isDoubleTap(), true);
        })

        unregister_test_backends();
    }

    void testReadingBC()
    {
        // QSensorReading changed in 1.0.1 due to QTMOBILITY-226
        // This test verifies that a backend built against the 1.0.0
        // version of qsensor.h still runs.
        TestSensor2 sensor;

        sensor.setProperty("doThis", "setOne");
        sensor.start();
        QCOMPARE(sensor.reading()->timestamp(), qtimestamp(1));
        QCOMPARE(sensor.reading()->test(), 1);
        sensor.stop();

        sensor.setProperty("doThis", "setTwo");
        sensor.start();
        QCOMPARE(sensor.reading()->timestamp(), qtimestamp(2));
        QCOMPARE(sensor.reading()->test(), 2);
        sensor.stop();
    }

    // This test must be LAST or it will interfere with the other tests
    void testLoadingPlugins()
    {
        // Go ahead and load the actual plugins (as a test that plugin loading works)
        sensors_unit_test_hook(1);

        // Hmm... There's no real way to tell if this worked or not.
        // If it doesn't work the unit test will probably crash.
        // That's what it did on Symbian before plugin loading was fixed.
    }
};

QTEST_MAIN(tst_QSensor)

#include "tst_qsensor.moc"
