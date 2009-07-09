/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "../qlocationtestutils_p.h"

#include <qgeocoordinate.h>
#include <qtest.h>

#include <QMetaType>
#include <QDebug>

#include <float.h>

Q_DECLARE_METATYPE(QGeoCoordinate)
Q_DECLARE_METATYPE(QGeoCoordinate::CoordinateFormat)
Q_DECLARE_METATYPE(QGeoCoordinate::CoordinateType)

static const QGeoCoordinate BRISBANE(-27.46758, 153.027892);
static const QGeoCoordinate MELBOURNE(-37.814251, 144.963169);
static const QGeoCoordinate LONDON(51.500152, -0.126236);
static const QGeoCoordinate NEW_YORK(40.71453, -74.00713);
static const QGeoCoordinate NORTH_POLE(90, 0);
static const QGeoCoordinate SOUTH_POLE(-90, 0);

static const QChar DEGREES_SYMB(0x00B0);


QByteArray tst_qgeocoordinate_debug;

void tst_qgeocoordinate_messageHandler(QtMsgType type, const char *msg)
{
    switch(type) {
        case QtDebugMsg :
            tst_qgeocoordinate_debug = QByteArray(msg);
            break;
        default:
            break;
    }
}


class tst_QGeoCoordinate : public QObject
{
    Q_OBJECT

private:
    enum TestDataType {
        Latitude,
        Longitude,
        Altitude
    };

private slots:
    void constructor()
    {
        QGeoCoordinate c;
        QVERIFY(!c.isValid());
        QCOMPARE(c, QGeoCoordinate());
    }

    void constructor_lat_long()
    {
        QFETCH(double, latitude);
        QFETCH(double, longitude);
        QFETCH(QGeoCoordinate::CoordinateType, type);

        QGeoCoordinate c(latitude, longitude);
        QCOMPARE(c.type(), type);
        if (type != QGeoCoordinate::InvalidCoordinate) {
            QVERIFY(c.isValid());
            QCOMPARE(c.latitude(), latitude);
            QCOMPARE(c.longitude(), longitude);
        } else {
            QVERIFY(!c.isValid());
            QVERIFY(c.latitude() != latitude);
            QVERIFY(c.longitude() != longitude);
        }
    }

    void constructor_lat_long_data()
    {
        QTest::addColumn<double>("latitude");
        QTest::addColumn<double>("longitude");
        QTest::addColumn<QGeoCoordinate::CoordinateType>("type");

        QTest::newRow("both zero") << 0.0 << 0.0 << QGeoCoordinate::Coordinate2D;
        QTest::newRow("both negative") << -1.0 << -1.0 << QGeoCoordinate::Coordinate2D;
        QTest::newRow("both positive") << 1.0 << 1.0 << QGeoCoordinate::Coordinate2D;
        QTest::newRow("latitude negative") << -1.0 << 1.0 << QGeoCoordinate::Coordinate2D;
        QTest::newRow("longitude negative") << 1.0 << -1.0 << QGeoCoordinate::Coordinate2D;

        QTest::newRow("both too high") << 90.1 << 180.1 << QGeoCoordinate::InvalidCoordinate;
        QTest::newRow("latitude too high") << 90.1 << 0.1 << QGeoCoordinate::InvalidCoordinate;
        QTest::newRow("longitude too high") << 0.1 << 180.1 << QGeoCoordinate::InvalidCoordinate;

        QTest::newRow("both too low") << -90.1 << -180.1 << QGeoCoordinate::InvalidCoordinate;
        QTest::newRow("latitude too low") << -90.1 << 0.1 << QGeoCoordinate::InvalidCoordinate;
        QTest::newRow("longitude too low") << 0.1 << -180.1 << QGeoCoordinate::InvalidCoordinate;

        QTest::newRow("both not too high") << 90.0 << 180.0 << QGeoCoordinate::Coordinate2D;
        QTest::newRow("both not too low") << -90.0 << -180.0 << QGeoCoordinate::Coordinate2D;

        QTest::newRow("latitude too high and longitude too low") << 90.1 << -180.1 << QGeoCoordinate::InvalidCoordinate;
        QTest::newRow("latitude too low and longitude too high") << -90.1 << 180.1 << QGeoCoordinate::InvalidCoordinate;
    }

    void constructor_lat_long_alt()
    {
        QFETCH(double, latitude);
        QFETCH(double, longitude);
        QFETCH(double, altitude);
        QFETCH(QGeoCoordinate::CoordinateType, type);

        QGeoCoordinate c(latitude, longitude, altitude);
        QCOMPARE(c.type(), type);
        if (type != QGeoCoordinate::InvalidCoordinate) {
            QCOMPARE(c.latitude(), latitude);
            QCOMPARE(c.longitude(), longitude);
            QCOMPARE(c.altitude(), altitude);
        } else {
            QVERIFY(!c.isValid());
            QVERIFY(c.latitude() != latitude);
            QVERIFY(c.longitude() != longitude);
            QVERIFY(c.altitude() != altitude);
        }
    }

    void constructor_lat_long_alt_data()
    {
        QTest::addColumn<double>("latitude");
        QTest::addColumn<double>("longitude");
        QTest::addColumn<double>("altitude");
        QTest::addColumn<QGeoCoordinate::CoordinateType>("type");

        QTest::newRow("all zero") << 0.0 << 0.0 << 0.0 << QGeoCoordinate::Coordinate3D;
        QTest::newRow("all negative") << -1.0 << -1.0 << -2.0 << QGeoCoordinate::Coordinate3D;
        QTest::newRow("all positive") << 1.0 << 1.0 << 4.0 << QGeoCoordinate::Coordinate3D;

        QTest::newRow("latitude negative") << -1.0 << 1.0 << 1.0 << QGeoCoordinate::Coordinate3D;
        QTest::newRow("longitude negative") << 1.0 << -1.0  << 1.0 << QGeoCoordinate::Coordinate3D;
        QTest::newRow("altitude negative") << 1.0 << 1.0  << -1.0 << QGeoCoordinate::Coordinate3D;

        QTest::newRow("altitude not too high") << 1.0 << 1.0  << DBL_MAX << QGeoCoordinate::Coordinate3D;
        QTest::newRow("altitude not too low") << 1.0 << 1.0  << DBL_MIN << QGeoCoordinate::Coordinate3D;

        QTest::newRow("all not too high") << 90.0 << 180.0  << DBL_MAX << QGeoCoordinate::Coordinate3D;
        QTest::newRow("all not too low") << -90.0 << -180.0  << DBL_MIN << QGeoCoordinate::Coordinate3D;

        QTest::newRow("all too high") << 90.1 << 180.1 << DBL_MAX << QGeoCoordinate::InvalidCoordinate;
        QTest::newRow("all too low") << -90.1 << -180.1 << DBL_MIN << QGeoCoordinate::InvalidCoordinate;
    }

    void copy_constructor()
    {
        QFETCH(QGeoCoordinate, c);

        QGeoCoordinate copy(c);
        QCOMPARE(copy.type(), c.type());
        if (c.type() != QGeoCoordinate::InvalidCoordinate) {
            QCOMPARE(copy.latitude(), c.latitude());
            QCOMPARE(copy.longitude(), c.longitude());
            if (c.type() == QGeoCoordinate::Coordinate3D)
                QCOMPARE(copy.altitude(), c.altitude());
        }
    }

    void copy_constructor_data()
    {
        QTest::addColumn<QGeoCoordinate>("c");

        QTest::newRow("no argument") << QGeoCoordinate();
        QTest::newRow("latitude, longitude arguments all zero") << QGeoCoordinate(0.0, 0.0);

        QTest::newRow("latitude, longitude arguments not too high") << QGeoCoordinate(90.0, 180.0);
        QTest::newRow("latitude, longitude arguments not too low") << QGeoCoordinate(-90.0, -180.0);
        QTest::newRow("latitude, longitude arguments too high") << QGeoCoordinate(90.1, 180.1);
        QTest::newRow("latitude, longitude arguments too low") << QGeoCoordinate(-90.1, -180.1);

        QTest::newRow("latitude, longitude, altitude arguments all zero") << QGeoCoordinate(0.0, 0.0, 0.0);
        QTest::newRow("latitude, longitude, altitude arguments not too high values") << QGeoCoordinate(90.0, 180.0, DBL_MAX);
        QTest::newRow("latitude, longitude, altitude arguments not too low values") << QGeoCoordinate(-90.0, -180.0, DBL_MIN);

        QTest::newRow("latitude, longitude, altitude arguments too high latitude & longitude") << QGeoCoordinate(90.1, 180.1, DBL_MAX);
        QTest::newRow("latitude, longitude, altitude arguments too low latitude & longitude") << QGeoCoordinate(-90.1, -180.1, DBL_MAX);
    }

    void destructor()
    {
        QGeoCoordinate *coordinate;

        QLocationTestUtils::uheap_mark();
        coordinate = new QGeoCoordinate();
        delete coordinate;
        QLocationTestUtils::uheap_mark_end();

        QLocationTestUtils::uheap_mark();
        coordinate = new QGeoCoordinate(0.0, 0.0);
        delete coordinate;
        QLocationTestUtils::uheap_mark_end();

        QLocationTestUtils::uheap_mark();
        coordinate = new QGeoCoordinate(0.0, 0.0, 0.0);
        delete coordinate;
        QLocationTestUtils::uheap_mark_end();

        QLocationTestUtils::uheap_mark();
        coordinate = new QGeoCoordinate(90.0, 180.0);
        delete coordinate;
        QLocationTestUtils::uheap_mark_end();

        QLocationTestUtils::uheap_mark();
        coordinate = new QGeoCoordinate(-90.0, -180.0);
        delete coordinate;
        QLocationTestUtils::uheap_mark_end();

        QLocationTestUtils::uheap_mark();
        coordinate = new QGeoCoordinate(90.1, 180.1);
        delete coordinate;
        QLocationTestUtils::uheap_mark_end();

        QLocationTestUtils::uheap_mark();
        coordinate = new QGeoCoordinate(-90.1, -180.1);
        delete coordinate;
        QLocationTestUtils::uheap_mark_end();
    }

    void destructor2()
    {
        QFETCH(QGeoCoordinate, c);
        QLocationTestUtils::uheap_mark();
        QGeoCoordinate *coordinate = new QGeoCoordinate(c);
        delete coordinate;
        QLocationTestUtils::uheap_mark_end();
    }

    void destructor2_data()
    {
        copy_constructor_data();
    }

    void assign()
    {
        QFETCH(QGeoCoordinate, c);

        QGeoCoordinate c1 = c;
        QCOMPARE(c.type(), c1.type());
        if (c.isValid()) {
            QCOMPARE(c.latitude(), c.latitude());
            QCOMPARE(c.longitude(), c.longitude());
            if (c.type() == QGeoCoordinate::Coordinate3D)
                QCOMPARE(c.altitude(), c.altitude());
        }
    }

    void assign_data()
    {
        copy_constructor_data();
    }

    void comparison()
    {
        QFETCH(QGeoCoordinate, c1);
        QFETCH(QGeoCoordinate, c2);
        QFETCH(bool, result);

        QCOMPARE(c1 == c2, result);
    }

    void comparison_data()
    {
        QTest::addColumn<QGeoCoordinate>("c1");
        QTest::addColumn<QGeoCoordinate>("c2");
        QTest::addColumn<bool>("result");

        QTest::newRow("Invalid != BRISBANE")
                << QGeoCoordinate(-190,-1000) << BRISBANE << false;
        QTest::newRow("BRISBANE != MELBOURNE")
                << BRISBANE << MELBOURNE << false;
        QTest::newRow("equal")
                << BRISBANE << BRISBANE << true;
        QTest::newRow("LONDON != uninitialized data")
                << LONDON << QGeoCoordinate() << false;
        QTest::newRow("uninitialized data == uninitialized data")
                << QGeoCoordinate() << QGeoCoordinate() << true;
        QTest::newRow("invalid == same invalid")
                << QGeoCoordinate(-190,-1000) << QGeoCoordinate(-190,-1000) << true;
        QTest::newRow("invalid == different invalid")
                << QGeoCoordinate(-190,-1000) << QGeoCoordinate(190,1000) << true;
        QTest::newRow("valid != another valid")
                << QGeoCoordinate(-90,-180) << QGeoCoordinate(-45,+45) << false;
        QTest::newRow("valid == same valid")
                << QGeoCoordinate(-90,-180) << QGeoCoordinate(-90,-180) << true;
    }

    void type()
    {
        QFETCH(QGeoCoordinate, c);
        QFETCH(QGeoCoordinate::CoordinateType, type);

        QCOMPARE(c.type(), type);
    }

    void type_data()
    {
        QTest::addColumn<QGeoCoordinate>("c");
        QTest::addColumn<QGeoCoordinate::CoordinateType>("type");

        QGeoCoordinate c;

        QTest::newRow("no values set")  << c << QGeoCoordinate::InvalidCoordinate;

        c.setAltitude(1.0);
        QTest::newRow("only altitude is set")  << c << QGeoCoordinate::InvalidCoordinate;

        c.setLongitude(1.0);
        QTest::newRow("only latitude and altitude is set") << c << QGeoCoordinate::InvalidCoordinate;

        c.setLatitude(-1.0);
        QTest::newRow("all valid: 3D Coordinate") << c << QGeoCoordinate::Coordinate3D;

        c.setLatitude(-90.1);
        QTest::newRow("too low latitude and valid longitude") << c << QGeoCoordinate::InvalidCoordinate;

        c.setLongitude(-180.1);
        c.setLatitude(90.0);
        QTest::newRow("valid latitude and too low longitude") << c << QGeoCoordinate::InvalidCoordinate;

        c.setLatitude(90.1);
        c.setLongitude(-180.0);
        QTest::newRow("too high latitude and valid longitude") << c << QGeoCoordinate::InvalidCoordinate;

        c.setLatitude(-90.0);
        c.setLongitude(180.1);
        QTest::newRow("valid latitude and too high longitude") << c << QGeoCoordinate::InvalidCoordinate;
    }

    void valid()
    {
        QFETCH(QGeoCoordinate, c);
        QCOMPARE(c.isValid(), c.type() != QGeoCoordinate::InvalidCoordinate);
    }

    void valid_data()
    {
        type_data();
    }

    void addDataValues(TestDataType type)
    {
        QTest::addColumn<double>("value");
        QTest::addColumn<bool>("valid");

        QTest::newRow("negative") << -1.0 << true;
        QTest::newRow("zero") << 0.0 << true;
        QTest::newRow("positive") << 1.0 << true;

        switch (type) {
            case Latitude:
                QTest::newRow("too low") << -90.1 << false;
                QTest::newRow("not too low") << -90.0 << true;
                QTest::newRow("not too hight") << 90.0 << true;
                QTest::newRow("too high") << 90.1;
                break;
            case Longitude:
                QTest::newRow("too low") << -180.1 << false;
                QTest::newRow("not too low") << -180.0 << true;
                QTest::newRow("not too hight") << 180.0 << true;
                QTest::newRow("too high") << 180.1;
                break;
            case Altitude:
                break;
        }
    }

    void latitude()
    {
        QFETCH(double, value);
        QGeoCoordinate c;
        c.setLatitude(value);
        QCOMPARE(c.latitude(), value);

        QGeoCoordinate c2 = c;
        QCOMPARE(c.latitude(), value);
        QCOMPARE(c2, c);
    }
    void latitude_data() { addDataValues(Latitude); }

    void longitude()
    {
        QFETCH(double, value);
        QGeoCoordinate c;
        c.setLongitude(value);
        QCOMPARE(c.longitude(), value);

        QGeoCoordinate c2 = c;
        QCOMPARE(c.longitude(), value);
        QCOMPARE(c2, c);
    }
    void longitude_data() { addDataValues(Longitude); }

    void altitude()
    {
        QFETCH(double, value);
        QGeoCoordinate c;
        c.setAltitude(value);
        QCOMPARE(c.altitude(), value);

        QGeoCoordinate c2 = c;
        QCOMPARE(c.altitude(), value);
        QCOMPARE(c2, c);
    }
    void altitude_data() { addDataValues(Altitude); }

    void distanceTo()
    {
        QFETCH(QGeoCoordinate, c1);
        QFETCH(QGeoCoordinate, c2);
        QFETCH(double, distance);

        QCOMPARE(qRound(c1.distanceTo(c2)), qRound(distance));
    }

    void distanceTo_data()
    {
        QTest::addColumn<QGeoCoordinate>("c1");
        QTest::addColumn<QGeoCoordinate>("c2");
        QTest::addColumn<double>("distance");

        QTest::newRow("invalid coord 1")
                << QGeoCoordinate() << BRISBANE << 0.0;
        QTest::newRow("invalid coord 2")
                << BRISBANE << QGeoCoordinate() << 0.0;
        QTest::newRow("brisbane -> melbourne")
                << BRISBANE << MELBOURNE << 1374820.162;
        QTest::newRow("london -> new york")
                << LONDON << NEW_YORK << 5570538.499;
        QTest::newRow("north pole -> south pole")
                << NORTH_POLE << SOUTH_POLE << 20015109.42;
    }

    void azimuthTo()
    {
        QFETCH(QGeoCoordinate, c1);
        QFETCH(QGeoCoordinate, c2);
        QFETCH(double, azimuth);

        QCOMPARE(QString::number(c1.azimuthTo(c2)), QString::number(azimuth));
    }

    void azimuthTo_data()
    {
        QTest::addColumn<QGeoCoordinate>("c1");
        QTest::addColumn<QGeoCoordinate>("c2");
        QTest::addColumn<double>("azimuth");

        QTest::newRow("invalid coord 1")
                << QGeoCoordinate() << BRISBANE << 0.0;
        QTest::newRow("invalid coord 2")
                << BRISBANE << QGeoCoordinate() << 0.0;
        QTest::newRow("brisbane -> melbourne")
                << BRISBANE << MELBOURNE << 211.1717;
        QTest::newRow("london -> new york")
                << LONDON << NEW_YORK << 288.3389;
        QTest::newRow("north pole -> south pole")
                << NORTH_POLE << SOUTH_POLE << 180.0;
    }

    void degreesToString()
    {
        QFETCH(QGeoCoordinate, coord);
        QFETCH(QGeoCoordinate::CoordinateFormat, format);
        QFETCH(QString, string);

        QCOMPARE(coord.toString(format), string);
    }

    void degreesToString_data()
    {
        QTest::addColumn<QGeoCoordinate>("coord");
        QTest::addColumn<QGeoCoordinate::CoordinateFormat>("format");
        QTest::addColumn<QString>("string");

        QGeoCoordinate northEast(27.46758, 153.027892);
        QGeoCoordinate southEast(-27.46758, 153.027892);
        QGeoCoordinate northWest(27.46758, -153.027892);
        QGeoCoordinate southWest(-27.46758, -153.027892);

        QGeoCoordinate empty;
        QGeoCoordinate toohigh(90.1, 180.1);
        QGeoCoordinate toolow(-90.1, -180.1);
        QGeoCoordinate allzero(0.0, 0.0);

        QTest::newRow("empty, dd, no hemisphere")
                << empty << QGeoCoordinate::DecimalDegrees
                << QString("<Invalid coordinate>");
        QTest::newRow("empty, dd, hemisphere")
                << empty << QGeoCoordinate::DecimalDegreesWithHemisphere
                << QString("<Invalid coordinate>");
        QTest::newRow("empty, dm, no hemisphere")
                << empty << QGeoCoordinate::DegreesMinutes
                << QString("<Invalid coordinate>");
        QTest::newRow("empty, dm, hemisphere")
                << empty << QGeoCoordinate::DegreesMinutesWithHemisphere
                << QString("<Invalid coordinate>");
        QTest::newRow("empty, dms, no hemisphere")
                << empty << QGeoCoordinate::DegreesMinutesSeconds
                << QString("<Invalid coordinate>");
        QTest::newRow("empty, dms, hemisphere")
                << empty << QGeoCoordinate::DegreesMinutesSecondsWithHemisphere
                << QString("<Invalid coordinate>");

        QTest::newRow("too low, dd, no hemisphere")
                << toolow << QGeoCoordinate::DecimalDegrees
                << QString("<Invalid coordinate>");
        QTest::newRow("too low, dd, hemisphere")
                << toolow << QGeoCoordinate::DecimalDegreesWithHemisphere
                << QString("<Invalid coordinate>");
        QTest::newRow("too low, dm, no hemisphere")
                << toolow << QGeoCoordinate::DegreesMinutes
                << QString("<Invalid coordinate>");
        QTest::newRow("too low, dm, hemisphere")
                << toolow << QGeoCoordinate::DegreesMinutesWithHemisphere
                << QString("<Invalid coordinate>");
        QTest::newRow("too low, dms, no hemisphere")
                << toolow << QGeoCoordinate::DegreesMinutesSeconds
                << QString("<Invalid coordinate>");
        QTest::newRow("too low, dms, hemisphere")
                << toolow << QGeoCoordinate::DegreesMinutesSecondsWithHemisphere
                << QString("<Invalid coordinate>");

        QTest::newRow("too high, dd, no hemisphere")
                << toohigh << QGeoCoordinate::DecimalDegrees
                << QString("<Invalid coordinate>");
        QTest::newRow("too high, dd, hemisphere")
                << toohigh << QGeoCoordinate::DecimalDegreesWithHemisphere
                << QString("<Invalid coordinate>");
        QTest::newRow("too high, dm, no hemisphere")
                << toohigh << QGeoCoordinate::DegreesMinutes
                << QString("<Invalid coordinate>");
        QTest::newRow("too high, dm, hemisphere")
                << toohigh << QGeoCoordinate::DegreesMinutesWithHemisphere
                << QString("<Invalid coordinate>");
        QTest::newRow("too high, dms, no hemisphere")
                << toohigh << QGeoCoordinate::DegreesMinutesSeconds
                << QString("<Invalid coordinate>");
        QTest::newRow("too high, dms, hemisphere")
                << toohigh << QGeoCoordinate::DegreesMinutesSecondsWithHemisphere
                << QString("<Invalid coordinate>");

        QTest::newRow("allzero, dd, no hemisphere")
                << allzero << QGeoCoordinate::DecimalDegrees
                << QString("0.00000%1, 0.00000%1").arg(DEGREES_SYMB);
        QTest::newRow("allzero, dd, hemisphere")
                << allzero << QGeoCoordinate::DecimalDegreesWithHemisphere
                << QString("0.00000%1, 0.00000%1").arg(DEGREES_SYMB);
        QTest::newRow("allzero, dm, no hemisphere")
                << allzero << QGeoCoordinate::DegreesMinutes
                << QString("0%1 0.000', 0%1 0.000'").arg(DEGREES_SYMB);
        QTest::newRow("allzero, dm, hemisphere")
                << allzero << QGeoCoordinate::DegreesMinutesWithHemisphere
                << QString("0%1 0.000', 0%1 0.000'").arg(DEGREES_SYMB);
        QTest::newRow("allzero, dms, no hemisphere")
                << allzero << QGeoCoordinate::DegreesMinutesSeconds
                << QString("0%1 0' 0.0\", 0%1 0' 0.0\"").arg(DEGREES_SYMB);
        QTest::newRow("allzero, dms, hemisphere")
                << allzero << QGeoCoordinate::DegreesMinutesSecondsWithHemisphere
                << QString("0%1 0' 0.0\", 0%1 0' 0.0\"").arg(DEGREES_SYMB);

        QTest::newRow("NE, dd, no hemisphere")
                << northEast << QGeoCoordinate::DecimalDegrees
                << QString("27.46758%1, 153.02789%1").arg(DEGREES_SYMB);
        QTest::newRow("NE, dd, hemisphere")
                << northEast << QGeoCoordinate::DecimalDegreesWithHemisphere
                << QString("27.46758%1 N, 153.02789%1 E").arg(DEGREES_SYMB);
        QTest::newRow("NE, dm, no hemisphere")
                << northEast << QGeoCoordinate::DegreesMinutes
                << QString("27%1 28.055', 153%1 1.674'").arg(DEGREES_SYMB);
        QTest::newRow("NE, dm, hemisphere")
                << northEast << QGeoCoordinate::DegreesMinutesWithHemisphere
                << QString("27%1 28.055' N, 153%1 1.674' E").arg(DEGREES_SYMB);
        QTest::newRow("NE, dms, no hemisphere")
                << northEast << QGeoCoordinate::DegreesMinutesSeconds
                << QString("27%1 28' 3.3\", 153%1 1' 40.4\"").arg(DEGREES_SYMB);
        QTest::newRow("NE, dms, hemisphere")
                << northEast << QGeoCoordinate::DegreesMinutesSecondsWithHemisphere
                << QString("27%1 28' 3.3\" N, 153%1 1' 40.4\" E").arg(DEGREES_SYMB);

        QTest::newRow("SE, dd, no hemisphere")
                << southEast << QGeoCoordinate::DecimalDegrees
                << QString("-27.46758%1, 153.02789%1").arg(DEGREES_SYMB);
        QTest::newRow("SE, dd, hemisphere")
                << southEast << QGeoCoordinate::DecimalDegreesWithHemisphere
                << QString("27.46758%1 S, 153.02789%1 E").arg(DEGREES_SYMB);
        QTest::newRow("SE, dm, no hemisphere")
                << southEast << QGeoCoordinate::DegreesMinutes
                << QString("-27%1 28.055', 153%1 1.674'").arg(DEGREES_SYMB);
        QTest::newRow("SE, dm, hemisphere")
                << southEast << QGeoCoordinate::DegreesMinutesWithHemisphere
                << QString("27%1 28.055' S, 153%1 1.674' E").arg(DEGREES_SYMB);
        QTest::newRow("SE, dms, no hemisphere")
                << southEast << QGeoCoordinate::DegreesMinutesSeconds
                << QString("-27%1 28' 3.3\", 153%1 1' 40.4\"").arg(DEGREES_SYMB);
        QTest::newRow("SE, dms, hemisphere")
                << southEast << QGeoCoordinate::DegreesMinutesSecondsWithHemisphere
                << QString("27%1 28' 3.3\" S, 153%1 1' 40.4\" E").arg(DEGREES_SYMB);

        QTest::newRow("NE, dd, no hemisphere")
                << northEast << QGeoCoordinate::DecimalDegrees
                << QString("27.46758%1, 153.02789%1").arg(DEGREES_SYMB);
        QTest::newRow("NE, dd, hemisphere")
                << northEast << QGeoCoordinate::DecimalDegreesWithHemisphere
                << QString("27.46758%1 N, 153.02789%1 E").arg(DEGREES_SYMB);
        QTest::newRow("NE, dm, no hemisphere")
                << northEast << QGeoCoordinate::DegreesMinutes
                << QString("27%1 28.055', 153%1 1.674'").arg(DEGREES_SYMB);
        QTest::newRow("NE, dm, hemisphere")
                << northEast << QGeoCoordinate::DegreesMinutesWithHemisphere
                << QString("27%1 28.055' N, 153%1 1.674' E").arg(DEGREES_SYMB);
        QTest::newRow("NE, dms, no hemisphere")
                << northEast << QGeoCoordinate::DegreesMinutesSeconds
                << QString("27%1 28' 3.3\", 153%1 1' 40.4\"").arg(DEGREES_SYMB);
        QTest::newRow("NE, dms, hemisphere")
                << northEast << QGeoCoordinate::DegreesMinutesSecondsWithHemisphere
                << QString("27%1 28' 3.3\" N, 153%1 1' 40.4\" E").arg(DEGREES_SYMB);

        QTest::newRow("SE, dd, no hemisphere")
                << southEast << QGeoCoordinate::DecimalDegrees
                << QString("-27.46758%1, 153.02789%1").arg(DEGREES_SYMB);
        QTest::newRow("SE, dd, hemisphere")
                << southEast << QGeoCoordinate::DecimalDegreesWithHemisphere
                << QString("27.46758%1 S, 153.02789%1 E").arg(DEGREES_SYMB);
        QTest::newRow("SE, dm, no hemisphere")
                << southEast << QGeoCoordinate::DegreesMinutes
                << QString("-27%1 28.055', 153%1 1.674'").arg(DEGREES_SYMB);
        QTest::newRow("SE, dm, hemisphere")
                << southEast << QGeoCoordinate::DegreesMinutesWithHemisphere
                << QString("27%1 28.055' S, 153%1 1.674' E").arg(DEGREES_SYMB);
        QTest::newRow("SE, dms, no hemisphere")
                << southEast << QGeoCoordinate::DegreesMinutesSeconds
                << QString("-27%1 28' 3.3\", 153%1 1' 40.4\"").arg(DEGREES_SYMB);
        QTest::newRow("SE, dms, hemisphere")
                << southEast << QGeoCoordinate::DegreesMinutesSecondsWithHemisphere
                << QString("27%1 28' 3.3\" S, 153%1 1' 40.4\" E").arg(DEGREES_SYMB);

        QTest::newRow("NW, dd, no hemisphere")
                << northWest << QGeoCoordinate::DecimalDegrees
                << QString("27.46758%1, -153.02789%1").arg(DEGREES_SYMB);
        QTest::newRow("NW, dd, hemisphere")
                << northWest << QGeoCoordinate::DecimalDegreesWithHemisphere
                << QString("27.46758%1 N, 153.02789%1 W").arg(DEGREES_SYMB);
        QTest::newRow("NW, dm, no hemisphere")
                << northWest << QGeoCoordinate::DegreesMinutes
                << QString("27%1 28.055', -153%1 1.674'").arg(DEGREES_SYMB);
        QTest::newRow("NW, dm, hemisphere")
                << northWest << QGeoCoordinate::DegreesMinutesWithHemisphere
                << QString("27%1 28.055' N, 153%1 1.674' W").arg(DEGREES_SYMB);
        QTest::newRow("NW, dms, no hemisphere")
                << northWest << QGeoCoordinate::DegreesMinutesSeconds
                << QString("27%1 28' 3.3\", -153%1 1' 40.4\"").arg(DEGREES_SYMB);
        QTest::newRow("NW, dms, hemisphere")
                << northWest << QGeoCoordinate::DegreesMinutesSecondsWithHemisphere
                << QString("27%1 28' 3.3\" N, 153%1 1' 40.4\" W").arg(DEGREES_SYMB);

        QTest::newRow("SW, dd, no hemisphere")
                << southWest << QGeoCoordinate::DecimalDegrees
                << QString("-27.46758%1, -153.02789%1").arg(DEGREES_SYMB);
        QTest::newRow("SW, dd, hemisphere")
                << southWest << QGeoCoordinate::DecimalDegreesWithHemisphere
                << QString("27.46758%1 S, 153.02789%1 W").arg(DEGREES_SYMB);
        QTest::newRow("SW, dm, no hemisphere")
                << southWest << QGeoCoordinate::DegreesMinutes
                << QString("-27%1 28.055', -153%1 1.674'").arg(DEGREES_SYMB);
        QTest::newRow("SW, dm, hemisphere")
                << southWest << QGeoCoordinate::DegreesMinutesWithHemisphere
                << QString("27%1 28.055' S, 153%1 1.674' W").arg(DEGREES_SYMB);
        QTest::newRow("SW, dms, no hemisphere")
                << southWest << QGeoCoordinate::DegreesMinutesSeconds
                << QString("-27%1 28' 3.3\", -153%1 1' 40.4\"").arg(DEGREES_SYMB);
        QTest::newRow("SW, dms, hemisphere")
                << southWest << QGeoCoordinate::DegreesMinutesSecondsWithHemisphere
                << QString("27%1 28' 3.3\" S, 153%1 1' 40.4\" W").arg(DEGREES_SYMB);
    }

    void datastream()
    {
        QFETCH(QGeoCoordinate, c);

        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out << c;

        QDataStream in(&ba, QIODevice::ReadOnly);
        QGeoCoordinate inCoord;
        in >> inCoord;
        QCOMPARE(inCoord, c);
    }

    void datastream_data()
    {
        QTest::addColumn<QGeoCoordinate>("c");

        QTest::newRow("invalid") << QGeoCoordinate();
        QTest::newRow("valid lat, long") << BRISBANE;
        QTest::newRow("valid lat, long, alt") << QGeoCoordinate(-1, -1, -1);
        QTest::newRow("valid lat, long, alt again") << QGeoCoordinate(1, 1, 1);
    }

    void debug()
    {
        QFETCH(QGeoCoordinate, c);
        QFETCH(QByteArray, debugString);

        qInstallMsgHandler(tst_qgeocoordinate_messageHandler);
        qDebug() << c;
        qInstallMsgHandler(0);
        QCOMPARE(tst_qgeocoordinate_debug, debugString);
    }

    void debug_data()
    {
        QTest::addColumn<QGeoCoordinate>("c");
        QTest::addColumn<QByteArray>("debugString");

        QTest::newRow("uninitialized") << QGeoCoordinate()
                << QByteArray("QGeoCoordinate(?, ?)");
        QTest::newRow("initialized without altitude") << BRISBANE
                << (QString("QGeoCoordinate(%1, %2)").arg(BRISBANE.latitude())
                        .arg(BRISBANE.longitude())).toLatin1();
        QTest::newRow("invalid initialization") << QGeoCoordinate(-100,-200)
                << QByteArray("QGeoCoordinate(?, ?)");
        QTest::newRow("initialized with altitude") << QGeoCoordinate(1,2,3)
                << QByteArray("QGeoCoordinate(1, 2, 3)");
    }
};

QTEST_MAIN(tst_QGeoCoordinate)
#include "tst_qgeocoordinate.moc"
