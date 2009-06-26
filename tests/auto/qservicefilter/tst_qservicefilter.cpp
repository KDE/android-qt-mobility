/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QtTest/QtTest>
#include <QtCore>
#include <qservicefilter.h>

class tst_QServiceFilter: public QObject
{
    Q_OBJECT
    
private slots:
    void initTestCase();
    void cleanupTestCase();
    void versionMatching();
    void versionMatching_data();
    void setInterface();
    void testAssignmentOperator();
    void testConstructor();
#ifndef QT_NO_DATASTREAM
    void streamTest();
#endif 
    void testCustomProperty();
    
};

void tst_QServiceFilter::initTestCase()
{
}

void tst_QServiceFilter::versionMatching_data()
{
    QTest::addColumn<QString>("version");
    QTest::addColumn<int>("majorV");
    QTest::addColumn<int>("minorV");
    QTest::addColumn<int>("rule");

    //invalid cases
    QTest::newRow("versionMatching_data():Invalid 1") << "" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 3") << "01.3" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 4") << "1.03" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 5") << "x.y" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 6") << "23" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 7") << "sdfsfs" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 8") << "%#5346" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 9") << ".66" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 10") << "1.3.4" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 11") << "1.a" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 12") << "b.1" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 13") << "3." << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 14") << "-1" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 16") << ".x" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 17") << "x." << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 18") << "1.  0" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 19") << "1  .0" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 20") << "1  0" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 21") << "1 . 0" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 22") << " 1.5" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 23") << "1.5 " << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 24") << " 1.5 " << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 25") << "1.5 1.6" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 26") << "-1.0" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 27") << "1.-1" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 28") << "-5.-1" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 29") << "4,8" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 30") << "   " << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;

   
    //valid cases 
    QTest::newRow("versionMatching_data():ValidMin 1") << "1.0" << 1 << 0 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():ValidMin 2") << "1.00" << 1 << 0 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():ValidMin 3") << "99.99" << 99 << 99 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():ValidMin 4") << "2.3" << 2 << 3 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():ValidMin 5") << "10.3" << 10 << 3 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():ValidMin 6") << "5.10" << 5 << 10 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():ValidMin 7") << "10.10" << 10 << 10 << (int)QServiceFilter::MinimumVersionMatch;

    QTest::newRow("versionMatching_data():ValidMin 8") << "0.3" << 0 << 3 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():ValidMin 10") << "0.0" << 0 << 0 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():ValidMin 11") << "00.00" << 0 << 0 << (int)QServiceFilter::MinimumVersionMatch;

    QTest::newRow("versionMatching_data():ValidExact 1") << "1.0" << 1 << 0 << (int)QServiceFilter::ExactVersionMatch;
    QTest::newRow("versionMatching_data():ValidExact 2") << "1.00" << 1 << 0 << (int)QServiceFilter::ExactVersionMatch;
    QTest::newRow("versionMatching_data():ValidExact 3") << "99.99" << 99 << 99 << (int)QServiceFilter::ExactVersionMatch;
    QTest::newRow("versionMatching_data():ValidExact 4") << "2.3" << 2 << 3 << (int)QServiceFilter::ExactVersionMatch;
    QTest::newRow("versionMatching_data():ValidExact 5") << "10.3" << 10 << 3 << (int)QServiceFilter::ExactVersionMatch;
    QTest::newRow("versionMatching_data():ValidExact 6") << "5.10" << 5 << 10 << (int)QServiceFilter::ExactVersionMatch;
    QTest::newRow("versionMatching_data():ValidExact 7") << "10.10" << 10 << 10 << (int)QServiceFilter::ExactVersionMatch;

    QTest::newRow("versionMatching_data():ValidExact 8") << "0.3" << 0 << 3 << (int)QServiceFilter::ExactVersionMatch;
    QTest::newRow("versionMatching_data():ValidExact 10") << "0.0" << 0 << 0 << (int)QServiceFilter::ExactVersionMatch;
    QTest::newRow("versionMatching_data():ValidExact 11") << "00.00" << 0 << 0 << (int)QServiceFilter::ExactVersionMatch;
}

void tst_QServiceFilter::versionMatching()
{
    QFETCH(QString, version);
    QFETCH(int, majorV);
    QFETCH(int, minorV);
    QFETCH(int, rule);

    QServiceFilter filter;
    QCOMPARE(filter.interfaceMajorVersion(), -1);
    QCOMPARE(filter.interfaceMinorVersion(), -1);
    QCOMPARE(filter.versionMatchRule(), QServiceFilter::MinimumVersionMatch);

    filter.setInterface("com.nokia.qt.test",version, (QServiceFilter::VersionMatchRule)rule);
    QCOMPARE(filter.interfaceMajorVersion(), majorV);
    QCOMPARE(filter.interfaceMinorVersion(), minorV);
    QCOMPARE(filter.versionMatchRule(), (QServiceFilter::VersionMatchRule)rule);
}


void tst_QServiceFilter::setInterface()
{
    //don't separate this out into test_data() function as we test the behavior
    //on the same filter object
    QServiceFilter filter;
    QCOMPARE(filter.interfaceMajorVersion(), -1);
    QCOMPARE(filter.interfaceMinorVersion(), -1);
    QCOMPARE(filter.versionMatchRule(), QServiceFilter::MinimumVersionMatch);
    QCOMPARE(filter.serviceName(), QString());
    QCOMPARE(filter.interfaceName(), QString());

    filter.setInterface("com.nokia.qt.text", "1.0", QServiceFilter::ExactVersionMatch);

    QCOMPARE(filter.interfaceMajorVersion(), 1);
    QCOMPARE(filter.interfaceMinorVersion(), 0);
    QCOMPARE(filter.versionMatchRule(), QServiceFilter::ExactVersionMatch);
    QCOMPARE(filter.serviceName(), QString());
    QCOMPARE(filter.interfaceName(), QString("com.nokia.qt.text"));

    filter.setInterface("com.nokia.qt.text", "1.5", QServiceFilter::MinimumVersionMatch);

    QCOMPARE(filter.interfaceMajorVersion(), 1);
    QCOMPARE(filter.interfaceMinorVersion(), 5);
    QCOMPARE(filter.versionMatchRule(), QServiceFilter::MinimumVersionMatch);
    QCOMPARE(filter.serviceName(), QString());
    QCOMPARE(filter.interfaceName(), QString("com.nokia.qt.text"));

    //invalid version tag -> ignore the call
    filter.setInterface("com.nokia.qt.label", "f.0", QServiceFilter::ExactVersionMatch);

    QCOMPARE(filter.interfaceMajorVersion(), 1);
    QCOMPARE(filter.interfaceMinorVersion(), 5);
    QCOMPARE(filter.versionMatchRule(), QServiceFilter::MinimumVersionMatch); //default
    QCOMPARE(filter.serviceName(), QString());
    QCOMPARE(filter.interfaceName(), QString("com.nokia.qt.text"));

    //empty version tag -> reset version
    filter.setInterface("com.nokia.qt.label", "", QServiceFilter::ExactVersionMatch);

    QCOMPARE(filter.interfaceMajorVersion(), -1);
    QCOMPARE(filter.interfaceMinorVersion(), -1);
    QCOMPARE(filter.versionMatchRule(), QServiceFilter::ExactVersionMatch); //default
    QCOMPARE(filter.serviceName(), QString());
    QCOMPARE(filter.interfaceName(), QString("com.nokia.qt.label"));

    //empty.interfaceName() tag -> ignore the call
    filter.setInterface("", "4.5", QServiceFilter::MinimumVersionMatch);

    QCOMPARE(filter.interfaceMajorVersion(), -1);
    QCOMPARE(filter.interfaceMinorVersion(), -1);
    QCOMPARE(filter.versionMatchRule(), QServiceFilter::ExactVersionMatch); //default
    QCOMPARE(filter.serviceName(), QString());
    QCOMPARE(filter.interfaceName(), QString("com.nokia.qt.label"));
    
    //set a valid 
    filter.setInterface("com.nokia.qt.valid", "4.77", QServiceFilter::ExactVersionMatch);

    QCOMPARE(filter.interfaceMajorVersion(), 4);
    QCOMPARE(filter.interfaceMinorVersion(), 77);
    QCOMPARE(filter.versionMatchRule(), QServiceFilter::ExactVersionMatch);
    QCOMPARE(filter.serviceName(), QString());
    QCOMPARE(filter.interfaceName(), QString("com.nokia.qt.valid"));

    filter.setServiceName("myService");
    QCOMPARE(filter.interfaceMajorVersion(), 4);
    QCOMPARE(filter.interfaceMinorVersion(), 77);
    QCOMPARE(filter.versionMatchRule(), QServiceFilter::ExactVersionMatch);
    QCOMPARE(filter.serviceName(), QString("myService"));
    QCOMPARE(filter.interfaceName(), QString("com.nokia.qt.valid"));

    //test default constructed version and matching rule
    filter.setInterface("com.nokia.qt.valid2");
    QCOMPARE(filter.interfaceMajorVersion(), -1);
    QCOMPARE(filter.interfaceMinorVersion(), -1);
    QCOMPARE(filter.versionMatchRule(), QServiceFilter::MinimumVersionMatch);
    QCOMPARE(filter.serviceName(), QString("myService"));
    QCOMPARE(filter.interfaceName(), QString("com.nokia.qt.valid2"));
}

void tst_QServiceFilter::testAssignmentOperator()
{
    QServiceFilter emptyFilter;
    QServiceFilter tempFilter(emptyFilter);
   

    //assign empty filter to empty filter 
    QCOMPARE(emptyFilter.interfaceMajorVersion(), -1);
    QCOMPARE(emptyFilter.interfaceMinorVersion(), -1);
    QCOMPARE(emptyFilter.versionMatchRule(), QServiceFilter::MinimumVersionMatch);
    QCOMPARE(emptyFilter.serviceName(), QString(""));
    QCOMPARE(emptyFilter.interfaceName(), QString(""));
    QCOMPARE(emptyFilter.customProperty("key1"), QString());

    QCOMPARE(tempFilter.interfaceMajorVersion(), -1);
    QCOMPARE(tempFilter.interfaceMinorVersion(), -1);
    QCOMPARE(tempFilter.versionMatchRule(), QServiceFilter::MinimumVersionMatch);
    QCOMPARE(tempFilter.serviceName(), QString(""));
    QCOMPARE(tempFilter.interfaceName(), QString(""));
    QCOMPARE(tempFilter.customProperty("key1"), QString());

    tempFilter = emptyFilter;
    
    QCOMPARE(tempFilter.interfaceMajorVersion(), -1);
    QCOMPARE(tempFilter.interfaceMinorVersion(), -1);
    QCOMPARE(tempFilter.versionMatchRule(), QServiceFilter::MinimumVersionMatch);
    QCOMPARE(tempFilter.serviceName(), QString(""));
    QCOMPARE(tempFilter.interfaceName(), QString(""));
    QCOMPARE(tempFilter.customProperty("key1"), QString());
   
    //assign filter to new filter via constructor
    tempFilter.setInterface("com.nokia.qt.valid", "4.77", QServiceFilter::ExactVersionMatch);
    tempFilter.setServiceName("ServiceName");
    tempFilter.setCustomProperty("key1", "value1");
    QCOMPARE(tempFilter.interfaceMajorVersion(), 4);
    QCOMPARE(tempFilter.interfaceMinorVersion(), 77);
    QCOMPARE(tempFilter.versionMatchRule(), QServiceFilter::ExactVersionMatch);
    QCOMPARE(tempFilter.serviceName(), QString("ServiceName"));
    QCOMPARE(tempFilter.interfaceName(), QString("com.nokia.qt.valid"));
    QCOMPARE(tempFilter.customProperty("key1"), QString("value1"));

    QServiceFilter constructFilter(tempFilter);
    QCOMPARE(constructFilter.interfaceMajorVersion(), 4);
    QCOMPARE(constructFilter.interfaceMinorVersion(), 77);
    QCOMPARE(constructFilter.versionMatchRule(), QServiceFilter::ExactVersionMatch);
    QCOMPARE(constructFilter.serviceName(), QString("ServiceName"));
    QCOMPARE(constructFilter.interfaceName(), QString("com.nokia.qt.valid"));
    QCOMPARE(constructFilter.customProperty("key1"), QString("value1"));

    //ensure that we don't have any potential references between tempFilter and 
    //constructedFilter
    tempFilter.setServiceName("NewServiceName");
    tempFilter.setInterface("com.nokia.qt.valid2", "5.88", QServiceFilter::MinimumVersionMatch);
    tempFilter.setCustomProperty("key2", "value2");
    QCOMPARE(tempFilter.interfaceMajorVersion(), 5);
    QCOMPARE(tempFilter.interfaceMinorVersion(), 88);
    QCOMPARE(tempFilter.versionMatchRule(), QServiceFilter::MinimumVersionMatch);
    QCOMPARE(tempFilter.serviceName(), QString("NewServiceName"));
    QCOMPARE(tempFilter.interfaceName(), QString("com.nokia.qt.valid2"));
    QCOMPARE(tempFilter.customProperty("key1"), QString("value1"));
    QCOMPARE(tempFilter.customProperty("key2"), QString("value2"));
    QCOMPARE(constructFilter.interfaceMajorVersion(), 4);
    QCOMPARE(constructFilter.interfaceMinorVersion(), 77);
    QCOMPARE(constructFilter.versionMatchRule(), QServiceFilter::ExactVersionMatch);
    QCOMPARE(constructFilter.serviceName(), QString("ServiceName"));
    QCOMPARE(constructFilter.interfaceName(), QString("com.nokia.qt.valid"));
    QCOMPARE(constructFilter.customProperty("key1"), QString("value1"));
    QCOMPARE(constructFilter.customProperty("key2"), QString());

    //assign empty filter to filter with values
    constructFilter = emptyFilter;
    QCOMPARE(constructFilter.interfaceMajorVersion(), -1);
    QCOMPARE(constructFilter.interfaceMinorVersion(), -1);
    QCOMPARE(constructFilter.versionMatchRule(), QServiceFilter::MinimumVersionMatch);
    QCOMPARE(constructFilter.serviceName(), QString(""));
    QCOMPARE(constructFilter.interfaceName(), QString(""));
    QCOMPARE(constructFilter.customProperty("key1"), QString());
    QCOMPARE(constructFilter.customProperty("key2"), QString());
}

void tst_QServiceFilter::testConstructor()
{
    QServiceFilter tempFilter1("");
    QCOMPARE(tempFilter1.interfaceMajorVersion(), -1);
    QCOMPARE(tempFilter1.interfaceMinorVersion(), -1);
    QCOMPARE(tempFilter1.versionMatchRule(), QServiceFilter::MinimumVersionMatch);
    QCOMPARE(tempFilter1.serviceName(), QString());
    QCOMPARE(tempFilter1.interfaceName(), QString(""));
    QCOMPARE(tempFilter1.customProperty("key1"), QString());

    QServiceFilter tempFilter2("com.nokia.qt.test");
    QCOMPARE(tempFilter2.interfaceMajorVersion(), -1);
    QCOMPARE(tempFilter2.interfaceMinorVersion(), -1);
    QCOMPARE(tempFilter2.versionMatchRule(), QServiceFilter::MinimumVersionMatch);
    QCOMPARE(tempFilter2.serviceName(), QString());
    QCOMPARE(tempFilter2.interfaceName(), QString("com.nokia.qt.test"));
    QCOMPARE(tempFilter2.customProperty("key1"), QString());

    QServiceFilter tempFilter3("com.nokia.qt.test", "10.5");
    QCOMPARE(tempFilter3.interfaceMajorVersion(), 10);
    QCOMPARE(tempFilter3.interfaceMinorVersion(), 5);
    QCOMPARE(tempFilter3.versionMatchRule(), QServiceFilter::MinimumVersionMatch);
    QCOMPARE(tempFilter3.serviceName(), QString());
    QCOMPARE(tempFilter3.interfaceName(), QString("com.nokia.qt.test"));
    QCOMPARE(tempFilter3.customProperty("key1"), QString());

    QServiceFilter tempFilter4("com.nokia.qt.test", "11.7", QServiceFilter::ExactVersionMatch);
    QCOMPARE(tempFilter4.interfaceMajorVersion(), 11);
    QCOMPARE(tempFilter4.interfaceMinorVersion(), 7);
    QCOMPARE(tempFilter4.versionMatchRule(), QServiceFilter::ExactVersionMatch);
    QCOMPARE(tempFilter4.serviceName(), QString());
    QCOMPARE(tempFilter4.interfaceName(), QString("com.nokia.qt.test"));
    QCOMPARE(tempFilter4.customProperty("key1"), QString());
    
}

#ifndef QT_NO_DATASTREAM
void tst_QServiceFilter::streamTest()
{
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::ReadWrite);
    QDataStream stream(&buffer);

    QServiceFilter emptyFilter;
    QCOMPARE(emptyFilter.interfaceMajorVersion(), -1);
    QCOMPARE(emptyFilter.interfaceMinorVersion(), -1);
    QCOMPARE(emptyFilter.versionMatchRule(), QServiceFilter::MinimumVersionMatch);
    QCOMPARE(emptyFilter.serviceName(), QString(""));
    QCOMPARE(emptyFilter.interfaceName(), QString(""));
    QCOMPARE(emptyFilter.customProperty("key1"), QString());

    buffer.seek(0);
    stream << emptyFilter;
    
    QServiceFilter tempFilter;
    QCOMPARE(tempFilter.interfaceMajorVersion(), -1);
    QCOMPARE(tempFilter.interfaceMinorVersion(), -1);
    QCOMPARE(tempFilter.versionMatchRule(), QServiceFilter::MinimumVersionMatch);
    QCOMPARE(tempFilter.serviceName(), QString(""));
    QCOMPARE(tempFilter.interfaceName(), QString(""));
    QCOMPARE(tempFilter.customProperty("key1"), QString());

    buffer.seek(0);
    stream >> tempFilter;
    
    QCOMPARE(tempFilter.interfaceMajorVersion(), -1);
    QCOMPARE(tempFilter.interfaceMinorVersion(), -1);
    QCOMPARE(tempFilter.versionMatchRule(), QServiceFilter::MinimumVersionMatch);
    QCOMPARE(tempFilter.serviceName(), QString(""));
    QCOMPARE(tempFilter.interfaceName(), QString(""));
    QCOMPARE(tempFilter.customProperty("key1"), QString());
   
    //assign filter to new filter via constructor
    tempFilter.setInterface("com.nokia.qt.valid", "4.77", QServiceFilter::ExactVersionMatch);
    tempFilter.setServiceName("ServiceName");
    tempFilter.setCustomProperty("key1", "value1");
    QCOMPARE(tempFilter.interfaceMajorVersion(), 4);
    QCOMPARE(tempFilter.interfaceMinorVersion(), 77);
    QCOMPARE(tempFilter.versionMatchRule(), QServiceFilter::ExactVersionMatch);
    QCOMPARE(tempFilter.serviceName(), QString("ServiceName"));
    QCOMPARE(tempFilter.interfaceName(), QString("com.nokia.qt.valid"));
    QCOMPARE(tempFilter.customProperty("key1"), QString("value1"));
    buffer.seek(0);
    stream << tempFilter;

    QServiceFilter constructFilter;
    buffer.seek(0);
    stream >> constructFilter;
    QCOMPARE(constructFilter.interfaceMajorVersion(), 4);
    QCOMPARE(constructFilter.interfaceMinorVersion(), 77);
    QCOMPARE(constructFilter.versionMatchRule(), QServiceFilter::ExactVersionMatch);
    QCOMPARE(constructFilter.serviceName(), QString("ServiceName"));
    QCOMPARE(constructFilter.interfaceName(), QString("com.nokia.qt.valid"));
    QCOMPARE(constructFilter.customProperty("key1"), QString("value1"));

    //assign empty filter to filter with values

    buffer.seek(0);
    stream << emptyFilter;
    buffer.seek(0);
    stream >> constructFilter;
    QCOMPARE(constructFilter.interfaceMajorVersion(), -1);
    QCOMPARE(constructFilter.interfaceMinorVersion(), -1);
    QCOMPARE(constructFilter.versionMatchRule(), QServiceFilter::MinimumVersionMatch);
    QCOMPARE(constructFilter.serviceName(), QString(""));
    QCOMPARE(constructFilter.interfaceName(), QString(""));
    QCOMPARE(constructFilter.customProperty("key1"), QString());
}
#endif 

void tst_QServiceFilter::testCustomProperty()
{
    //default constructor
    QServiceFilter emptyFilter;
    QCOMPARE(emptyFilter.customProperty("key1"), QString());

    QServiceFilter filter("com.nokia.qt.testinterface", "4.5");
    QCOMPARE(filter.customProperty("key1"), QString());
    filter.setCustomProperty("key1", "newValue");
    QCOMPARE(filter.customProperty("key1"), QString("newValue"));
    filter.setCustomProperty("key1", "revisedValue");
    QCOMPARE(filter.customProperty("key1"), QString("revisedValue"));
    filter.setCustomProperty("key2", "Value");
    QCOMPARE(filter.customProperty("key1"), QString("revisedValue"));
    QCOMPARE(filter.customProperty("key2"), QString("Value"));
    filter.setCustomProperty("key1", QString());
    QCOMPARE(filter.customProperty("key1"), QString());
    QCOMPARE(filter.customProperty("key2"), QString("Value"));
}

void tst_QServiceFilter::cleanupTestCase()
{
}

QTEST_MAIN(tst_QServiceFilter)
#include "tst_qservicefilter.moc"
