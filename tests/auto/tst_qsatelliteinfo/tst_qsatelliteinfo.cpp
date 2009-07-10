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
#include <qsatelliteinfo.h>

#include <QMetaType>
#include <QObject>
#include <QDebug>
#include <QTest>

#include <float.h>
#include <limits.h>

Q_DECLARE_METATYPE(QSatelliteInfo)
Q_DECLARE_METATYPE(QSatelliteInfo::Property)

QList<qreal> tst_qsatelliteinfo_qrealTestValues()
{
    QList<qreal> values;
    if (qreal(DBL_MIN) == DBL_MIN)
        values << DBL_MIN;

    values << FLT_MIN;
    values << -1.0 << 0.0 << 1.0;
    values << FLT_MAX;

    if (qreal(DBL_MAX) == DBL_MAX)
        values << DBL_MAX;
    return values;
}

QList<int> tst_qsatelliteinfo_intTestValues()
{
    QList<int> values;
    values << INT_MIN << -100 << 0 << 100 << INT_MAX;
    return values;
}

QList<QSatelliteInfo::Property> tst_qsatelliteinfo_getProperties()
{
    QList<QSatelliteInfo::Property> properties;
    properties << QSatelliteInfo::Elevation
            << QSatelliteInfo::Azimuth;
    return properties;
}


class tst_QSatelliteInfo : public QObject
{
    Q_OBJECT

private:
    QSatelliteInfo updateWithProperty(QSatelliteInfo::Property property, qreal value)
    {
        QSatelliteInfo info;
        info.setProperty(property, value);
        return info;
    }

    void addTestData_update()
    {
        QTest::addColumn<QSatelliteInfo>("info");

        QList<int> intValues = tst_qsatelliteinfo_intTestValues();
        for (int i=0; i<intValues.count(); i++) {
            QSatelliteInfo info;
            info.setPrnNumber(intValues[i]);
            QTest::newRow("prn") << info;
        }

        for (int i=0; i<intValues.count(); i++) {
            QSatelliteInfo info;
            info.setSignalStrength(intValues[i]);
            QTest::newRow("signal strength") << info;
        }

        QList<QSatelliteInfo::Property> properties = tst_qsatelliteinfo_getProperties();
        QList<qreal> qrealValues = tst_qsatelliteinfo_qrealTestValues();
        for (int i=0; i<properties.count(); i++) {
            QTest::newRow(qPrintable(QString("Property %1 = %2").arg(properties[i]).arg(qrealValues[i])))
                    << updateWithProperty(properties[i], qrealValues[i]);
        }
    }

private slots:
    void constructor()
    {
        QSatelliteInfo info;
        QCOMPARE(info.prnNumber(), -1);
        QCOMPARE(info.signalStrength(), -1);
        QList<QSatelliteInfo::Property> properties = tst_qsatelliteinfo_getProperties();
        for (int i=0; i<properties.count(); i++)
            QCOMPARE(info.property(properties[i]), qreal(-1.0));
    }

    void constructor_copy()
    {
        QFETCH(QSatelliteInfo, info);

        QCOMPARE(QSatelliteInfo(info), info);
    }

    void constructor_copy_data()
    {
        addTestData_update();
    }

    void operator_comparison()
    {
        QFETCH(QSatelliteInfo, info);

        QVERIFY(info == info);
        QCOMPARE(info != info, false);
        QCOMPARE(info == QSatelliteInfo(), false);
        QCOMPARE(info != QSatelliteInfo(), true);

        QVERIFY(QSatelliteInfo() == QSatelliteInfo());
    }

    void operator_comparison_data()
    {
        addTestData_update();
    }

    void operator_assign()
    {
        QFETCH(QSatelliteInfo, info);

        QSatelliteInfo info2 = info;
        QCOMPARE(info2, info);
    }

    void operator_assign_data()
    {
        addTestData_update();
    }

    void setPrnNumber()
    {
        QFETCH(int, prn);

        QSatelliteInfo info;
        QCOMPARE(info.prnNumber(), -1);

        info.setPrnNumber(prn);
        QCOMPARE(info.prnNumber(), prn);
    }

    void setPrnNumber_data()
    {
        QTest::addColumn<int>("prn");

        QList<int> intValues = tst_qsatelliteinfo_intTestValues();
        for (int i=0; i<intValues.count(); i++)
            QTest::newRow(qPrintable(QString("%1").arg(intValues[i]))) << intValues[i];
    }

    void setSignalStrength()
    {
        QFETCH(int, signal);

        QSatelliteInfo info;
        QCOMPARE(info.signalStrength(), -1);

        info.setSignalStrength(signal);
        QCOMPARE(info.signalStrength(), signal);
    }

    void setSignalStrength_data()
    {
        QTest::addColumn<int>("signal");

        QList<int> intValues = tst_qsatelliteinfo_intTestValues();
        for (int i=0; i<intValues.count(); i++)
            QTest::newRow(qPrintable(QString("%1").arg(intValues[i]))) << intValues[i];
    }

    void property()
    {
        QFETCH(QSatelliteInfo::Property, property);
        QFETCH(qreal, value);

        QSatelliteInfo u;
        QCOMPARE(u.property(property), qreal(-1.0));

        u.setProperty(property, value);
        QCOMPARE(u.property(property), value);

        u.removeProperty(property);
        QCOMPARE(u.property(property), qreal(-1.0));
    }

    void property_data()
    {
        QTest::addColumn<QSatelliteInfo::Property>("property");
        QTest::addColumn<qreal>("value");

        QList<QSatelliteInfo::Property> props;
        props << QSatelliteInfo::Elevation
              << QSatelliteInfo::Azimuth;
        for (int i=0; i<props.count(); i++) {
            QTest::newRow(QTest::toString("property " + props[i])) << props[i] << qreal(-1.0);
            QTest::newRow(QTest::toString("property " + props[i])) << props[i] << qreal(0.0);
            QTest::newRow(QTest::toString("property " + props[i])) << props[i] << qreal(1.0);
        }
    }

    void hasProperty()
    {
        QFETCH(QSatelliteInfo::Property, property);
        QFETCH(qreal, value);

        QSatelliteInfo u;
        QVERIFY(!u.hasProperty(property));

        u.setProperty(property, value);
        QVERIFY(u.hasProperty(property));

        u.removeProperty(property);
        QVERIFY(!u.hasProperty(property));
    }

    void hasProperty_data()
    {
        property_data();
    }

    void removeProperty()
    {
        QFETCH(QSatelliteInfo::Property, property);
        QFETCH(qreal, value);

        QSatelliteInfo u;
        QVERIFY(!u.hasProperty(property));

        u.setProperty(property, value);
        QVERIFY(u.hasProperty(property));

        u.removeProperty(property);
        QVERIFY(!u.hasProperty(property));

        u.setProperty(property, value);
        QVERIFY(u.hasProperty(property));
    }

    void removeProperty_data()
    {
        property_data();
    }
};


QTEST_MAIN(tst_QSatelliteInfo)
#include "tst_qsatelliteinfo.moc"
