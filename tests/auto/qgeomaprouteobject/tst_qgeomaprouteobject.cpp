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

#include "testhelper.h"
#include <QtTest/QtTest>
#include "qgeomaprouteobject.h"
#include "qgeocoordinate.h"
#include "qgraphicsgeomap.h"
#include "qgeoboundingbox.h"
#include "qgeoroutesegment.h"

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QGeoRoute)
Q_DECLARE_METATYPE(QGeoCoordinate)
Q_DECLARE_METATYPE(QPen)
Q_DECLARE_METATYPE(quint32)

class tst_QGeoMapRouteObject: public QObject
{
Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void qgeomaprouteobject_data();
    void qgeomaprouteobject();

    void detailLevel_data();
    void detailLevel();
    void pen_data();
    void pen();
    void route_data();
    void route();
    void zvalue_data();
    void zvalue();
    void boundingBox();
    void contains_data();
    void contains();
    void isSelected();
    void isVisible();

private:
    TestHelper *m_helper;
};

// This will be called before the first test function is executed.
// It is only called once.
void tst_QGeoMapRouteObject::initTestCase()
{
    qRegisterMetaType<QGeoRoute> ();
}

// This will be called after the last test function is executed.
// It is only called once.
void tst_QGeoMapRouteObject::cleanupTestCase()
{
}

// This will be called before each test function is executed.
void tst_QGeoMapRouteObject::init()
{
    m_helper = new TestHelper();
    QGraphicsGeoMap* map = m_helper->map();

    if (!map)
        QFAIL("Could not create map!");
}

// This will be called after every test function.
void tst_QGeoMapRouteObject::cleanup()
{
    delete (m_helper);
    m_helper = 0;
}

void tst_QGeoMapRouteObject::qgeomaprouteobject_data()
{
}

void tst_QGeoMapRouteObject::qgeomaprouteobject()
{

    QGeoMapRouteObject* object = new QGeoMapRouteObject();

    QCOMPARE(object->type(), QGeoMapObject::RouteType);

    QCOMPARE((int)object->detailLevel(), 6);
    QPen pen(Qt::black);
    pen.setCosmetic(true);
    QCOMPARE(object->pen(), pen);
    QCOMPARE(object->route().path(),QGeoRoute().path());
    QCOMPARE(object->zValue(), 0);
    QCOMPARE(object->isSelected(),false);
    QCOMPARE(object->isVisible(),true);
    QCOMPARE(object->boundingBox(),QGeoBoundingBox());
    QCOMPARE(object->contains(QGeoCoordinate()),false);

    //check if can be added to map

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    QVERIFY2(object->mapData(),"no map data set");

    map->removeMapObject(object);

    QVERIFY2(!object->mapData(),"no map data still set");

    delete (object);
}

void tst_QGeoMapRouteObject::detailLevel_data()
{

    QTest::addColumn<int>("detailLevel");
    QTest::newRow("8") << 8;
    QTest::newRow("10") << 10;
    QTest::newRow("12") << 12;

}

// public quint32 detailLevel() const
void tst_QGeoMapRouteObject::detailLevel()
{
    QFETCH(int, detailLevel);

    QGeoCoordinate center(10, 20);

    QGeoRoute route;

    QGeoRouteSegment segment;

    QList<QGeoCoordinate> path;

    path << center;

    QGraphicsGeoMap* map = m_helper->map();

    map->setCenter(center);

    QPointF point = map->coordinateToScreenPosition(center);

    QPointF diff1(detailLevel / 2, detailLevel / 2);

    point += diff1;

    QGeoCoordinate coord = map->screenPositionToCoordinate(point);

    path << coord;

    QPointF diff2(-detailLevel / 2, detailLevel / 2);

    point += diff2;

    coord = map->screenPositionToCoordinate(point);

    path << coord;

    QPointF diff3(0, detailLevel / 2);

    point += diff3;

    coord = map->screenPositionToCoordinate(point);

    path << coord;

    segment.setPath(path);
    route.setFirstRouteSegment(segment);
    route.setPath(path);

    QGeoMapRouteObject* object = new QGeoMapRouteObject();

    object->setRoute(route);

    QPen p;

    p.setWidth(2);

    object->setPen(p);

    object->setDetailLevel(1);

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    QSignalSpy spy0(object, SIGNAL(detailLevelChanged(quint32)));
    QSignalSpy spy1(object, SIGNAL(penChanged(QPen const&)));
    QSignalSpy spy2(object, SIGNAL(routeChanged(QGeoRoute const&)));

    //check if we have the point
    map->setCenter(path.at(1));

    point = map->coordinateToScreenPosition(path.at(1));

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),1);

    object->setDetailLevel(detailLevel+1);

    QCOMPARE(object->detailLevel(), (quint32)detailLevel+1);

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),0);

    QCOMPARE(spy0.count(), 1);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);

}

void tst_QGeoMapRouteObject::pen_data()
{

    QTest::addColumn<QPen>("pen");

    QPen pen1(Qt::blue);

    pen1.setWidth(5);

    QTest::newRow("blue,5") << pen1;

    QPen pen2(Qt::white);

    pen2.setWidth(10);

    QTest::newRow("white,10") << pen2;

    QPen pen3(Qt::black);

    pen3.setWidth(15);

    QTest::newRow("black,15") << pen3;

}

// public QPen pen() const
void tst_QGeoMapRouteObject::pen()
{
    QFETCH(QPen, pen);

    QGeoMapRouteObject* object = new QGeoMapRouteObject();

    QGeoRoute route;

    QGeoRouteSegment segment;

    QList<QGeoCoordinate> path;

    path << QGeoCoordinate(10, 10) << QGeoCoordinate(10, 20) << QGeoCoordinate(10, 30);

    segment.setPath(path);

    route.setFirstRouteSegment(segment);

    object->setRoute(route);

    QPen p;

    p.setWidth(1);

    object->setPen(p);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    QSignalSpy spy0(object, SIGNAL(detailLevelChanged(quint32)));
    QSignalSpy spy1(object, SIGNAL(penChanged(QPen const&)));
    QSignalSpy spy2(object, SIGNAL(routeChanged(QGeoRoute const&)));

    //check if object is not there

    map->setCenter(path.at(1));

    QPointF point = map->coordinateToScreenPosition(path.at(1));

    QPointF diff(pen.width() / 2 - 1, pen.width() / 2 - 1);

    point += diff;

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),0);

    object->setPen(pen);

    QCOMPARE(object->pen(), pen);

    //check if object is there

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),1);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 0);

}

void tst_QGeoMapRouteObject::route_data()
{

    QTest::addColumn<QGeoRoute>("route");
    QGeoRoute route1;
    QGeoRouteSegment segment1;
    QList<QGeoCoordinate> list1;
    list1 << QGeoCoordinate(10, 10) << QGeoCoordinate(10, 20) << QGeoCoordinate(10, 30);
    segment1.setPath(list1);
    route1.setFirstRouteSegment(segment1);
    route1.setPath(list1);
    QTest::newRow("(10,10)-(10,20)-(10,30)") << route1;
    QGeoRoute route2;
    QGeoRouteSegment segment2;
    QList<QGeoCoordinate> list2;
    list2 << QGeoCoordinate(-10, 10) << QGeoCoordinate(-5, 10) << QGeoCoordinate(-2, 10);
    segment2.setPath(list2);
    route2.setFirstRouteSegment(segment2);
    route2.setPath(list2);
    QTest::newRow("(-10,10)-(-5,10)-(-2,10)") << route2;
    QGeoRoute route3;
    QGeoRouteSegment segment3;
    QList<QGeoCoordinate> list3;
    list3 << QGeoCoordinate(-10, 10) << QGeoCoordinate(-5, 10) << QGeoCoordinate(2, 10);
    segment3.setPath(list3);
    route3.setFirstRouteSegment(segment3);
    route3.setPath(list3);
    QTest::newRow("(-10,10)-(-5,10)-(2,10)") << route3;
}

// public QGeoRoute route() const
void tst_QGeoMapRouteObject::route()
{
    QFETCH(QGeoRoute, route);

    QGeoMapRouteObject* object = new QGeoMapRouteObject();

    QPen pen;

    pen.setWidth(10);

    object->setPen(pen);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    QSignalSpy spy0(object, SIGNAL(detailLevelChanged(quint32)));
    QSignalSpy spy1(object, SIGNAL(penChanged(QPen const&)));
    QSignalSpy spy2(object, SIGNAL(routeChanged(QGeoRoute const&)));

    //check if object is not there

    map->setCenter(route.path().at(1));

    QPointF point = map->coordinateToScreenPosition(route.path().at(1));

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),0);

    object->setRoute(route);

    QCOMPARE(object->route(), route);

    //check if object is there

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),1);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 1);

}

void tst_QGeoMapRouteObject::zvalue_data()
{
    QTest::addColumn<int>("zValue1");
    QTest::addColumn<int>("zValue2");
    QTest::addColumn<int>("zValue3");
    QTest::newRow("1,2,3") << 1 << 2 << 3;
    QTest::newRow("3,2,1") << 3 << 2 << 1;
    QTest::newRow("2,1,3") << 2 << 1 << 3;
}

// public int zValue() const
void tst_QGeoMapRouteObject::zvalue()
{

    QFETCH(int, zValue1);
    QFETCH(int, zValue2);
    QFETCH(int, zValue3);

    QList<QGeoCoordinate> path;
    QGeoRouteSegment segment;

    path << QGeoCoordinate(2.0, -1.0, 0);
    path << QGeoCoordinate(2.0, 1.0, 0);
    path << QGeoCoordinate(2.0, 2.0, 0);

    QGeoRoute route;
    segment.setPath(path);
    route.setFirstRouteSegment(segment);

    QPen pen;
    pen.setWidth(1);

    QGeoMapRouteObject* object1 = new QGeoMapRouteObject();
    object1->setRoute(route);
    object1->setPen(pen);
    QGeoMapRouteObject* object2 = new QGeoMapRouteObject();
    object2->setRoute(route);
    object2->setPen(pen);
    QGeoMapRouteObject* object3 = new QGeoMapRouteObject();
    object3->setRoute(route);
    object3->setPen(pen);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object1);
    map->addMapObject(object2);
    map->addMapObject(object3);

    QList<QGeoMapObject *> list = map->mapObjects();

    QCOMPARE(list.count(),3);

    QVERIFY(list.at(0)==object1);
    QVERIFY(list.at(1)==object2);
    QVERIFY(list.at(2)==object3);

    QSignalSpy spy0(object1, SIGNAL(selectedChanged(bool)));
    QSignalSpy spy1(object1, SIGNAL(visibleChanged(bool)));
    QSignalSpy spy2(object1, SIGNAL(zValueChanged(int)));

    map->setCenter(path.at(1));

    QPointF point = map->coordinateToScreenPosition(path.at(1));


    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),3);

    QVERIFY(map->mapObjectsAtScreenPosition(point).at(0)==object1);
    QVERIFY(map->mapObjectsAtScreenPosition(point).at(1)==object2);
    QVERIFY(map->mapObjectsAtScreenPosition(point).at(2)==object3);

    object1->setZValue(zValue1);
    object2->setZValue(zValue2);
    object3->setZValue(zValue3);

    QCOMPARE(object1->zValue(), zValue1);
    QCOMPARE(object2->zValue(), zValue2);
    QCOMPARE(object3->zValue(), zValue3);
    //check if object is there

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),3);

    QVERIFY(map->mapObjectsAtScreenPosition(point).at(zValue1-1)==object1);
    QVERIFY(map->mapObjectsAtScreenPosition(point).at(zValue2-1)==object2);
    QVERIFY(map->mapObjectsAtScreenPosition(point).at(zValue3-1)==object3);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 1);

}

// public bool isVisible() const
void tst_QGeoMapRouteObject::isVisible()
{
    QList<QGeoCoordinate> path;

    path << QGeoCoordinate(2.0, -1.0, 0);
    path << QGeoCoordinate(2.0, 1.0, 0);
    path << QGeoCoordinate(2.0, 2.0, 0);

    QGeoRoute route;
    QGeoRouteSegment segment;
    segment.setPath(path);
    route.setFirstRouteSegment(segment);

    QPen pen;
    pen.setWidth(1);

    QGeoMapRouteObject* object = new QGeoMapRouteObject();
    object->setRoute(route);
    object->setPen(pen);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    QSignalSpy spy0(object, SIGNAL(selectedChanged(bool)));
    QSignalSpy spy1(object, SIGNAL(visibleChanged(bool)));
    QSignalSpy spy2(object, SIGNAL(zValueChanged(int)));

    map->setCenter(path.at(1));

    QPointF point = map->coordinateToScreenPosition(path.at(1));


    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),1);

    object->setVisible(false);

    QCOMPARE(object->isVisible(), false);

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),0);

    object->setVisible(true);

    QCOMPARE(object->isVisible(), true);


    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),1);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 2);
    QCOMPARE(spy2.count(), 0);

}

// public bool isSelected() const
void tst_QGeoMapRouteObject::isSelected()
{
#if 0

    QSignalSpy spy0(object, SIGNAL(selectedChanged(bool)));
    QSignalSpy spy1(object, SIGNAL(visibleChanged(bool)));
    QSignalSpy spy2(object, SIGNAL(zValueChanged(int)));

    QCOMPARE(object->isSelected(), isSelected);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

void tst_QGeoMapRouteObject::contains_data()
{

    QTest::addColumn<QGeoCoordinate>("coordinate");
    QTest::newRow("2.0,-1.0") << QGeoCoordinate(2.0, -1.0, 0);
    QTest::newRow("2.0,0.0") << QGeoCoordinate(2.0, 0.0, 0);
    QTest::newRow("2.0,1.0") << QGeoCoordinate(2.0, 1.0, 0);

}

// public bool contains(QGeoCoordinate const& coordinate) const
void tst_QGeoMapRouteObject::contains()
{
    QFETCH(QGeoCoordinate, coordinate);

    QList<QGeoCoordinate> path;

    path << QGeoCoordinate(2.0, -1.0, 0);
    path << QGeoCoordinate(2.0, 1.0, 0);
    path << QGeoCoordinate(2.0, 2.0, 0);

    QGeoRoute route;
    QGeoRouteSegment segment;
    segment.setPath(path);
    route.setFirstRouteSegment(segment);

    QPen pen;
    pen.setWidth(1);

    QGeoMapRouteObject* object = new QGeoMapRouteObject();

    object->setRoute(route);
    object->setPen(pen);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();


    QCOMPARE(list.size(),1);
    QVERIFY(list.at(0)==object);

    QSignalSpy spy0(object, SIGNAL(selectedChanged(bool)));
    QSignalSpy spy1(object, SIGNAL(visibleChanged(bool)));
    QSignalSpy spy2(object, SIGNAL(zValueChanged(int)));

    map->setCenter(coordinate);

    QPointF point = map->coordinateToScreenPosition(coordinate);

    bool contains = map->mapObjectsAtScreenPosition(point).size() == 1;

    QCOMPARE(object->contains(coordinate), contains);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);

}

// public QGeoBoundingBox boundingBox() const
void tst_QGeoMapRouteObject::boundingBox()
{

    QList<QGeoCoordinate> path;

    path << QGeoCoordinate(2.0, -1.0, 0);
    path << QGeoCoordinate(2.0, 1.0, 0);
    path << QGeoCoordinate(-2.0, 1.0, 0);
    path << QGeoCoordinate(-2.0, -1.0, 0);

    QGeoRoute route;
    QGeoRouteSegment segment;
    segment.setPath(path);
    route.setFirstRouteSegment(segment);
    route.setPath(path);

    QList<QGeoCoordinate> datelinePath;

    datelinePath << QGeoCoordinate(2.0, 179.0, 0);
    datelinePath << QGeoCoordinate(2.0, -179.0, 0);
    datelinePath << QGeoCoordinate(-2.0, -179.0, 0);
    datelinePath << QGeoCoordinate(-2.0, 179.0, 0);

    QGeoRoute datelineRoute;
    QGeoRouteSegment datelineSegment;
    datelineSegment.setPath(datelinePath);
    datelineRoute.setFirstRouteSegment(datelineSegment);
    datelineRoute.setPath(datelinePath);

    QGeoMapRouteObject* object = new QGeoMapRouteObject();
    object->setRoute(route);

    QGraphicsGeoMap* map = m_helper->map();

    map->setZoomLevel(1);
    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    QGeoBoundingBox box1 = object->boundingBox();

    QVERIFY2(box1.width()>0,"no bounding box");
    QVERIFY2(box1.height()>0,"no bounding box");
    
    double width = object->boundingBox().width();
    double height = object->boundingBox().height();

    double top = object->boundingBox().topLeft().latitude();
    double bottom = object->boundingBox().bottomRight().latitude();

    QVERIFY(object->boundingBox().topLeft().longitude() < object->boundingBox().bottomRight().longitude());

    object->setRoute(datelineRoute);

    QVERIFY2(object->boundingBox().width()!=0,"no bounding box");
    QVERIFY2(object->boundingBox().height()!=0,"no bounding box");

    QVERIFY(object->boundingBox().width() == width);
    QVERIFY(object->boundingBox().height() == height);

    QVERIFY(object->boundingBox().topLeft().latitude() == top);
    QVERIFY(object->boundingBox().bottomRight().latitude() == bottom);

    QVERIFY(object->boundingBox().topLeft().longitude() > object->boundingBox().bottomRight().longitude());

    object->setRoute(route);

    map->setZoomLevel(10);
    map->setCenter(QGeoCoordinate(30.0, 0.0, 0.0));

    list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    QGeoBoundingBox box2 = object->boundingBox();

    QVERIFY2(box2.width()>0,"no bounding box");
    QVERIFY2(box2.height()>0,"no bounding box");

    QVERIFY(box1 == box2);
}

QTEST_MAIN(tst_QGeoMapRouteObject)
#include "tst_qgeomaprouteobject.moc"

