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
#include <qgeopositioninfo.h>

#include <QMetaType>
#include <QObject>
#include <QDebug>
#include <QTest>

#include <float.h>

QTM_USE_NAMESPACE
Q_DECLARE_METATYPE(QGeoCoordinate)
Q_DECLARE_METATYPE(QGeoPositionInfo)
Q_DECLARE_METATYPE(QGeoPositionInfo::Attribute)

QByteArray tst_qgeopositioninfo_debug;

void tst_qgeopositioninfo_messageHandler(QtMsgType type, const char *msg)
{
    switch(type) {
        case QtDebugMsg :
            tst_qgeopositioninfo_debug = QByteArray(msg);
            break;
        default:
            break;
    }
}

QList<qreal> tst_qgeopositioninfo_qrealTestValues()
{
    QList<qreal> values;

// the following platforms use float for qreal
#if !defined(QT_NO_FPU) && !defined(QT_ARCH_ARM) && !defined(QT_ARCH_WINDOWSCE) && !defined(QT_ARCH_SYMBIAN) 
    if (qreal(DBL_MIN) == DBL_MIN)
        values << DBL_MIN;
#endif

    values << FLT_MIN;
    values << -1.0 << 0.0 << 1.0;
    values << FLT_MAX;

// the following platforms use float for qreal
#if !defined(QT_NO_FPU) && !defined(QT_ARCH_ARM) && !defined(QT_ARCH_WINDOWSCE) && !defined(QT_ARCH_SYMBIAN) 
    if (qreal(DBL_MAX) == DBL_MAX)
        values << DBL_MAX;
#endif

    return values;
}

QList<QGeoPositionInfo::Attribute> tst_qgeopositioninfo_getAttributes()
{
    QList<QGeoPositionInfo::Attribute> attributes;
    attributes << QGeoPositionInfo::Direction
            << QGeoPositionInfo::GroundSpeed
            << QGeoPositionInfo::VerticalSpeed
            << QGeoPositionInfo::MagneticVariation
            << QGeoPositionInfo::HorizontalAccuracy
            << QGeoPositionInfo::VerticalAccuracy;
    return attributes;
}


class tst_QGeoPositionInfo : public QObject
{
    Q_OBJECT

private:
    QGeoPositionInfo infoWithAttribute(QGeoPositionInfo::Attribute attribute, qreal value)
    {
        QGeoPositionInfo info;
        info.setAttribute(attribute, value);
        return info;
    }

    void addTestData_info()
    {
        QTest::addColumn<QGeoPositionInfo>("info");

        QTest::newRow("invalid") << QGeoPositionInfo();

        QTest::newRow("coord") << QGeoPositionInfo(QGeoCoordinate(-27.3422,150.2342), QDateTime());
        QTest::newRow("datetime") << QGeoPositionInfo(QGeoCoordinate(), QDateTime::currentDateTime());

        QList<QGeoPositionInfo::Attribute> attributes = tst_qgeopositioninfo_getAttributes();
        QList<qreal> values = tst_qgeopositioninfo_qrealTestValues();
        for (int i=0; i<attributes.count(); i++) {
            for (int j=0; j<values.count(); j++) {
                QTest::newRow(qPrintable(QString("Attribute %1 = %2").arg(attributes[i]).arg(values[j])))
                        << infoWithAttribute(attributes[i], values[j]);
            }
        }
    }

private slots:
    void constructor()
    {
        QGeoPositionInfo info;
        QVERIFY(!info.isValid());
        QVERIFY(!info.coordinate().isValid());
        QVERIFY(info.timestamp().isNull());
    }

    void constructor_coord_dateTime()
    {
        QFETCH(QGeoCoordinate, coord);
        QFETCH(QDateTime, dateTime);
        QFETCH(bool, valid);

        QGeoPositionInfo info(coord, dateTime);
        QCOMPARE(info.coordinate(), coord);
        QCOMPARE(info.timestamp(), dateTime);
        QCOMPARE(info.isValid(), valid);
    }

    void constructor_coord_dateTime_data()
    {
        QTest::addColumn<QGeoCoordinate>("coord");
        QTest::addColumn<QDateTime>("dateTime");
        QTest::addColumn<bool>("valid");

        QTest::newRow("both null") << QGeoCoordinate() << QDateTime() << false;
        QTest::newRow("both valid") << QGeoCoordinate(1,1) << QDateTime::currentDateTime() << true;
        QTest::newRow("valid coord") << QGeoCoordinate(1,1) << QDateTime() << false;
        QTest::newRow("valid datetime") << QGeoCoordinate() << QDateTime::currentDateTime() << false;
        QTest::newRow("valid time but not date == invalid")
                << QGeoCoordinate() << QDateTime(QDate(), QTime::currentTime()) << false;
        QTest::newRow("valid date but not time == valid due to QDateTime constructor")
                << QGeoCoordinate() << QDateTime(QDate::currentDate(), QTime()) << false;
    }

    void constructor_copy()
    {
        QFETCH(QGeoPositionInfo, info);

        QCOMPARE(QGeoPositionInfo(info), info);
    }

    void constructor_copy_data()
    {
        addTestData_info();
    }

    void operator_assign()
    {
        QFETCH(QGeoPositionInfo, info);

        QGeoPositionInfo info2 = info;
        QCOMPARE(info2, info);
    }

    void operator_assign_data()
    {
        addTestData_info();
    }

    void operator_equals()
    {
        QFETCH(QGeoPositionInfo, info);

        QVERIFY(info == info);
        if (info.isValid())
            QCOMPARE(info == QGeoPositionInfo(), false);
    }

    void operator_equals_data()
    {
        addTestData_info();
    }

    void operator_notEquals()
    {
        QFETCH(QGeoPositionInfo, info);

        QCOMPARE(info != info, false);
        if (info.isValid())
            QCOMPARE(info != QGeoPositionInfo(), true);
    }

    void operator_notEquals_data()
    {
        addTestData_info();
    }

    void setDateTime()
    {
        QFETCH(QDateTime, dateTime);

        QGeoPositionInfo info;
        info.setTimestamp(dateTime);
        QCOMPARE(info.timestamp(), dateTime);
    }

    void setDateTime_data()
    {
        QTest::addColumn<QDateTime>("dateTime");
        QTest::newRow("invalid") << QDateTime();
        QTest::newRow("now") << QDateTime::currentDateTime();
    }

    void dateTime()
    {
        QGeoPositionInfo info;
        QVERIFY(info.timestamp().isNull());
    }

    void setCoordinate()
    {

        QFETCH(QGeoCoordinate, coord);

        QGeoPositionInfo info;
        info.setCoordinate(coord);
        QCOMPARE(info.coordinate(), coord);
    }

    void setCoordinate_data()
    {
        QTest::addColumn<QGeoCoordinate>("coord");

        QTest::newRow("invalid") << QGeoCoordinate();
        QTest::newRow("valid") << QGeoCoordinate(30,30);
    }

    void attribute()
    {
        QFETCH(QGeoPositionInfo::Attribute, attribute);
        QFETCH(qreal, value);

        QGeoPositionInfo info;
        QCOMPARE(info.attribute(attribute), qreal(-1.0));

        info.setAttribute(attribute, value);
        QCOMPARE(info.attribute(attribute), value);

        info.removeAttribute(attribute);
        QCOMPARE(info.attribute(attribute), qreal(-1.0));
    }

    void attribute_data()
    {
        QTest::addColumn<QGeoPositionInfo::Attribute>("attribute");
        QTest::addColumn<qreal>("value");

        QList<QGeoPositionInfo::Attribute> attributes = tst_qgeopositioninfo_getAttributes();
        QList<qreal> values = tst_qgeopositioninfo_qrealTestValues();
        for (int i=0; i<attributes.count(); i++) {
            for (int j=0; j<values.count(); j++) {
                QTest::newRow(qPrintable(QString("Attribute %1 = %2").arg(attributes[i]).arg(values[j])))
                        << attributes[i] << values[j];
            }
        }
    }

    void hasAttribute()
    {
        QFETCH(QGeoPositionInfo::Attribute, attribute);
        QFETCH(qreal, value);

        QGeoPositionInfo info;
        QVERIFY(!info.hasAttribute(attribute));

        info.setAttribute(attribute, value);
        QVERIFY(info.hasAttribute(attribute));

        info.removeAttribute(attribute);
        QVERIFY(!info.hasAttribute(attribute));
    }

    void hasAttribute_data()
    {
        attribute_data();
    }

    void removeAttribute()
    {
        QFETCH(QGeoPositionInfo::Attribute, attribute);
        QFETCH(qreal, value);

        QGeoPositionInfo info;
        QVERIFY(!info.hasAttribute(attribute));

        info.setAttribute(attribute, value);
        QVERIFY(info.hasAttribute(attribute));

        info.removeAttribute(attribute);
        QVERIFY(!info.hasAttribute(attribute));

        info.setAttribute(attribute, value);
        QVERIFY(info.hasAttribute(attribute));
    }

    void removeAttribute_data()
    {
        attribute_data();
    }

    void datastream()
    {
        QFETCH(QGeoPositionInfo, info);

        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out << info;

        QDataStream in(&ba, QIODevice::ReadOnly);
        QGeoPositionInfo inInfo;
        in >> inInfo;
        QCOMPARE(inInfo, info);
    }

    void datastream_data()
    {
        addTestData_info();
    }

    void debug()
    {
        QFETCH(QGeoPositionInfo, info);
        QFETCH(QByteArray, debugStringEnd);

        qInstallMsgHandler(tst_qgeopositioninfo_messageHandler);
        qDebug() << info;
        qInstallMsgHandler(0);

        // use endsWith() so we don't depend on QDateTime's debug() implementation
        QVERIFY(tst_qgeopositioninfo_debug.endsWith(debugStringEnd));
    }

    void debug_data()
    {
        QTest::addColumn<QGeoPositionInfo>("info");
        QTest::addColumn<QByteArray>("debugStringEnd");

        QTest::newRow("no values") << QGeoPositionInfo()
                << QString("QGeoCoordinate(?, ?))").toLatin1();

        QGeoCoordinate coord(1, 1);
        QTest::newRow("coord, time") << QGeoPositionInfo(coord, QDateTime::currentDateTime())
                << QByteArray("QGeoCoordinate(1, 1))");

        QGeoPositionInfo info;
        info.setAttribute(QGeoPositionInfo::Direction, 1.1);
        info.setAttribute(QGeoPositionInfo::GroundSpeed, 2.1);
        info.setAttribute(QGeoPositionInfo::VerticalSpeed, 3.1);
        info.setAttribute(QGeoPositionInfo::MagneticVariation, 4.1);
        info.setAttribute(QGeoPositionInfo::HorizontalAccuracy, 5.1);
        info.setAttribute(QGeoPositionInfo::VerticalAccuracy, 6.1);
        QTest::newRow("all attributes") << info
                << QByteArray("QGeoCoordinate(?, ?), Direction=1.1, GroundSpeed=2.1, VerticalSpeed=3.1, MagneticVariation=4.1, HorizontalAccuracy=5.1, VerticalAccuracy=6.1)");
    }
};


QTEST_MAIN(tst_QGeoPositionInfo)
#include "tst_qgeopositioninfo.moc"
