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
#include <qgeosatelliteinfo.h>

#include <QMetaType>
#include <QObject>
#include <QDebug>
#include <QTest>

#include <float.h>
#include <limits.h>

QTM_USE_NAMESPACE
Q_DECLARE_METATYPE(QGeoSatelliteInfo)
Q_DECLARE_METATYPE(QGeoSatelliteInfo::Attribute)

QByteArray tst_qgeosatelliteinfo_debug;

void tst_qgeosatelliteinfo_messageHandler(QtMsgType type, const char *msg)
{
    switch(type) {
        case QtDebugMsg :
            tst_qgeosatelliteinfo_debug = QByteArray(msg);
            break;
        default:
            break;
    }
}


QList<qreal> tst_qgeosatelliteinfo_qrealTestValues()
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

QList<int> tst_qgeosatelliteinfo_intTestValues()
{
    QList<int> values;
    values << INT_MIN << -100 << 0 << 100 << INT_MAX;
    return values;
}

QList<QGeoSatelliteInfo::Attribute> tst_qgeosatelliteinfo_getAttributes()
{
    QList<QGeoSatelliteInfo::Attribute> attributes;
    attributes << QGeoSatelliteInfo::Elevation
            << QGeoSatelliteInfo::Azimuth;
    return attributes;
}


class tst_QGeoSatelliteInfo : public QObject
{
    Q_OBJECT

private:
    QGeoSatelliteInfo updateWithAttribute(QGeoSatelliteInfo::Attribute attribute, qreal value)
    {
        QGeoSatelliteInfo info;
        info.setAttribute(attribute, value);
        return info;
    }

    void addTestData_update()
    {
        QTest::addColumn<QGeoSatelliteInfo>("info");

        QList<int> intValues = tst_qgeosatelliteinfo_intTestValues();
        for (int i=0; i<intValues.count(); i++) {
            QGeoSatelliteInfo info;
            info.setPrnNumber(intValues[i]);
            QTest::newRow("prn") << info;
        }

        for (int i=0; i<intValues.count(); i++) {
            QGeoSatelliteInfo info;
            info.setSignalStrength(intValues[i]);
            QTest::newRow("signal strength") << info;
        }

        QList<QGeoSatelliteInfo::Attribute> attributes = tst_qgeosatelliteinfo_getAttributes();
        QList<qreal> qrealValues = tst_qgeosatelliteinfo_qrealTestValues();
        for (int i=0; i<attributes.count(); i++) {
            QTest::newRow(qPrintable(QString("Attribute %1 = %2").arg(attributes[i]).arg(qrealValues[i])))
                    << updateWithAttribute(attributes[i], qrealValues[i]);
        }
    }

private slots:
    void constructor()
    {
        QGeoSatelliteInfo info;
        QCOMPARE(info.prnNumber(), -1);
        QCOMPARE(info.signalStrength(), -1);
        QList<QGeoSatelliteInfo::Attribute> attributes = tst_qgeosatelliteinfo_getAttributes();
        for (int i=0; i<attributes.count(); i++)
            QCOMPARE(info.attribute(attributes[i]), qreal(-1.0));
    }

    void constructor_copy()
    {
        QFETCH(QGeoSatelliteInfo, info);

        QCOMPARE(QGeoSatelliteInfo(info), info);
    }

    void constructor_copy_data()
    {
        addTestData_update();
    }

    void operator_comparison()
    {
        QFETCH(QGeoSatelliteInfo, info);

        QVERIFY(info == info);
        QCOMPARE(info != info, false);
        QCOMPARE(info == QGeoSatelliteInfo(), false);
        QCOMPARE(info != QGeoSatelliteInfo(), true);

        QVERIFY(QGeoSatelliteInfo() == QGeoSatelliteInfo());
    }

    void operator_comparison_data()
    {
        addTestData_update();
    }

    void operator_assign()
    {
        QFETCH(QGeoSatelliteInfo, info);

        QGeoSatelliteInfo info2 = info;
        QCOMPARE(info2, info);
    }

    void operator_assign_data()
    {
        addTestData_update();
    }

    void setPrnNumber()
    {
        QFETCH(int, prn);

        QGeoSatelliteInfo info;
        QCOMPARE(info.prnNumber(), -1);

        info.setPrnNumber(prn);
        QCOMPARE(info.prnNumber(), prn);
    }

    void setPrnNumber_data()
    {
        QTest::addColumn<int>("prn");

        QList<int> intValues = tst_qgeosatelliteinfo_intTestValues();
        for (int i=0; i<intValues.count(); i++)
            QTest::newRow(qPrintable(QString("%1").arg(intValues[i]))) << intValues[i];
    }

    void setSignalStrength()
    {
        QFETCH(int, signal);

        QGeoSatelliteInfo info;
        QCOMPARE(info.signalStrength(), -1);

        info.setSignalStrength(signal);
        QCOMPARE(info.signalStrength(), signal);
    }

    void setSignalStrength_data()
    {
        QTest::addColumn<int>("signal");

        QList<int> intValues = tst_qgeosatelliteinfo_intTestValues();
        for (int i=0; i<intValues.count(); i++)
            QTest::newRow(qPrintable(QString("%1").arg(intValues[i]))) << intValues[i];
    }

    void attribute()
    {
        QFETCH(QGeoSatelliteInfo::Attribute, attribute);
        QFETCH(qreal, value);

        QGeoSatelliteInfo u;
        QCOMPARE(u.attribute(attribute), qreal(-1.0));

        u.setAttribute(attribute, value);
        QCOMPARE(u.attribute(attribute), value);

        u.removeAttribute(attribute);
        QCOMPARE(u.attribute(attribute), qreal(-1.0));
    }

    void attribute_data()
    {
        QTest::addColumn<QGeoSatelliteInfo::Attribute>("attribute");
        QTest::addColumn<qreal>("value");

        QList<QGeoSatelliteInfo::Attribute> props;
        props << QGeoSatelliteInfo::Elevation
              << QGeoSatelliteInfo::Azimuth;
        for (int i=0; i<props.count(); i++) {
            QTest::newRow(QTest::toString("attribute " + props[i])) << props[i] << qreal(-1.0);
            QTest::newRow(QTest::toString("attribute " + props[i])) << props[i] << qreal(0.0);
            QTest::newRow(QTest::toString("attribute " + props[i])) << props[i] << qreal(1.0);
        }
    }

    void hasAttribute()
    {
        QFETCH(QGeoSatelliteInfo::Attribute, attribute);
        QFETCH(qreal, value);

        QGeoSatelliteInfo u;
        QVERIFY(!u.hasAttribute(attribute));

        u.setAttribute(attribute, value);
        QVERIFY(u.hasAttribute(attribute));

        u.removeAttribute(attribute);
        QVERIFY(!u.hasAttribute(attribute));
    }

    void hasAttribute_data()
    {
        attribute_data();
    }

    void removeAttribute()
    {
        QFETCH(QGeoSatelliteInfo::Attribute, attribute);
        QFETCH(qreal, value);

        QGeoSatelliteInfo u;
        QVERIFY(!u.hasAttribute(attribute));

        u.setAttribute(attribute, value);
        QVERIFY(u.hasAttribute(attribute));

        u.removeAttribute(attribute);
        QVERIFY(!u.hasAttribute(attribute));

        u.setAttribute(attribute, value);
        QVERIFY(u.hasAttribute(attribute));
    }

    void removeAttribute_data()
    {
        attribute_data();
    }

    void datastream()
    {
        QFETCH(QGeoSatelliteInfo, info);

        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out << info;

        QDataStream in(&ba, QIODevice::ReadOnly);
        QGeoSatelliteInfo inInfo;
        in >> inInfo;
        QCOMPARE(inInfo, info);
    }

    void datastream_data()
    {
        addTestData_update();
    }

    void debug()
    {
        QFETCH(QGeoSatelliteInfo, info);
        QFETCH(QByteArray, debugString);

        qInstallMsgHandler(tst_qgeosatelliteinfo_messageHandler);
        qDebug() << info;
        qInstallMsgHandler(0);
        QCOMPARE(QString(tst_qgeosatelliteinfo_debug), QString(debugString));
    }

    void debug_data()
    {
        QTest::addColumn<QGeoSatelliteInfo>("info");
        QTest::addColumn<QByteArray>("debugString");

        QGeoSatelliteInfo info;

        QTest::newRow("uninitialized") << info
                << QByteArray("QGeoSatelliteInfo(PRN=-1, signal-strength=-1)");

        info = QGeoSatelliteInfo();
        info.setPrnNumber(1);
        QTest::newRow("with PRN") << info
                << QByteArray("QGeoSatelliteInfo(PRN=1, signal-strength=-1)");

        info = QGeoSatelliteInfo();
        info.setSignalStrength(1);
        QTest::newRow("with PRN") << info
                << QByteArray("QGeoSatelliteInfo(PRN=-1, signal-strength=1)");

        info = QGeoSatelliteInfo();
        info.setAttribute(QGeoSatelliteInfo::Elevation, 1.1);
        QTest::newRow("with Elevation") << info
                << QByteArray("QGeoSatelliteInfo(PRN=-1, signal-strength=-1, Elevation=1.1)");

        info = QGeoSatelliteInfo();
        info.setAttribute(QGeoSatelliteInfo::Azimuth, 1.1);
        QTest::newRow("with Azimuth") << info
                << QByteArray("QGeoSatelliteInfo(PRN=-1, signal-strength=-1, Azimuth=1.1)");
    }
};


QTEST_MAIN(tst_QGeoSatelliteInfo)
#include "tst_qgeosatelliteinfo.moc"
