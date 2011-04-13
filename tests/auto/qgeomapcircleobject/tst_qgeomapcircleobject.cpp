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
#include <QMetaType>
#include <QStyleOptionGraphicsItem>
#include <qgeomapcircleobject.h>
#include <qgeocoordinate.h>
#include <qgeoboundingcircle.h>
#include <qgeomappixmapobject.h>
#include <qgraphicsgeomap.h>
#include <qgeoboundingbox.h>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QGeoCoordinate)
Q_DECLARE_METATYPE(QGeoBoundingCircle)
Q_DECLARE_METATYPE(QGeoBoundingBox)
Q_DECLARE_METATYPE(QBrush)
Q_DECLARE_METATYPE(QPen)

class tst_QGeoMapCircleObject: public QObject
{
Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void qgeomapcircleobject_data();
    void qgeomapcircleobject();
    void brush_data();
    void brush();
    void center_data();
    void center();
    void circle_data();
    void circle();
    void pen_data();
    void pen();
    void radius_data();
    void radius();
    void zvalue_data();
    void zvalue();
    void boundingBox();
    void contains_data();
    void contains();
    void isSelected();
    void isVisible();

    // QTMOBILITY-1255: Changing z value of map object causes it to break insertion order
    void qtmobility1255();
    // QTMOBILITY-1199: Result of QGraphicsGeoMap::mapObjectsInViewport is zoom level dependent
    void qtmobility1199();

private:
    TestHelper *m_helper;

};

// This will be called before the first test function is executed.
// It is only called once.
void tst_QGeoMapCircleObject::initTestCase()
{
    qRegisterMetaType<QGeoCoordinate> ();
}

// This will be called after the last test function is executed.
// It is only called once.
void tst_QGeoMapCircleObject::cleanupTestCase()
{
}

// This will be called before each test function is executed.
void tst_QGeoMapCircleObject::init()
{
    m_helper = new TestHelper();

    QGraphicsGeoMap* map = m_helper->map();

    if (!map)
        QFAIL("Could not create map!");
}

// This will be called after every test function.
void tst_QGeoMapCircleObject::cleanup()
{
    delete (m_helper);
    m_helper = 0;
}

void tst_QGeoMapCircleObject::qgeomapcircleobject_data()
{
}

void tst_QGeoMapCircleObject::qgeomapcircleobject()
{
    QGeoMapCircleObject* object = new QGeoMapCircleObject();

    //check initialization
    QCOMPARE(object->type(), QGeoMapObject::CircleType);
    QCOMPARE(object->brush(), QBrush());
    QCOMPARE(object->center(), QGeoCoordinate());
    QCOMPARE(object->circle(), QGeoBoundingCircle());
    QCOMPARE(object->zValue(), 0);
    QCOMPARE(object->isSelected(),false);
    QCOMPARE(object->isVisible(),true);
    QCOMPARE(object->boundingBox(),QGeoBoundingBox());
    QCOMPARE(object->contains(QGeoCoordinate()),false);

    QPen pen(Qt::black);
    pen.setCosmetic(true);
    QCOMPARE(object->pen(), pen);
    //TODO: is this value really correct ?
    QCOMPARE(object->radius(), -1.0);

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

void tst_QGeoMapCircleObject::brush_data()
{
    QTest::addColumn<QBrush>("brush");
    QTest::newRow("white") << QBrush(Qt::white);
    QTest::newRow("blue") << QBrush(Qt::blue);
}

// public QBrush brush() const
void tst_QGeoMapCircleObject::brush()
{
    QFETCH(QBrush, brush);

    QGeoMapCircleObject* object = new QGeoMapCircleObject();

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    QSignalSpy spy0(object, SIGNAL(brushChanged(QBrush const&)));
    QSignalSpy spy1(object, SIGNAL(centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy2(object, SIGNAL(penChanged(QPen const&)));
    QSignalSpy spy3(object, SIGNAL(radiusChanged(qreal)));

    object->setBrush(brush);

    QCOMPARE(object->brush(), brush);

    QCOMPARE(spy0.count(), 1);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);

}

void tst_QGeoMapCircleObject::center_data()
{
    QTest::addColumn<QGeoCoordinate>("center");
    QTest::newRow("52.5, 13.0, 0") << QGeoCoordinate(52.5, 13.0, 0);
    QTest::newRow("-52.5, 13.0, 0") << QGeoCoordinate(-52.5, 13.0, 0);
    QTest::newRow("52.5, -13.0, 0") << QGeoCoordinate(52.5, -13.0, 0);
    QTest::newRow("-52.5, -13.0, 0") << QGeoCoordinate(-52.5, -13.0, 0);
}

// public QGeoCoordinate center() const

void tst_QGeoMapCircleObject::center()
{
    QFETCH(QGeoCoordinate, center);

    QGeoMapCircleObject* object = new QGeoMapCircleObject(QGeoCoordinate(10.0, 10.0, 0.0), 1000);

    QCOMPARE(object->center(), QGeoCoordinate(10.0,10.0,0.0));

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    QSignalSpy spy0(object, SIGNAL(brushChanged(QBrush const&)));
    QSignalSpy spy1(object, SIGNAL(centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy2(object, SIGNAL(penChanged(QPen const&)));
    QSignalSpy spy3(object, SIGNAL(radiusChanged(qreal)));

    map->setCenter(center);

    //check if circle is not there
    QPointF point = map->coordinateToScreenPosition(center);

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),0);

    object->setCenter(center);

    QCOMPARE(object->center(), center);

    //check if circle is there

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),1);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);

}

void tst_QGeoMapCircleObject::circle_data()
{
    QTest::addColumn<QGeoBoundingCircle>("circle");
    QTest::newRow("52.5, 13.0, 2000") << QGeoBoundingCircle(QGeoCoordinate(52.5, 13.0, 0), 2000);
    QTest::newRow("-52.5, 13.0, 3000") << QGeoBoundingCircle(QGeoCoordinate(-52.5, 13.0, 0), 3000);
    QTest::newRow("52.5, -13.0, 4000") << QGeoBoundingCircle(QGeoCoordinate(52.5, -13.0, 0), 4000);
    QTest::newRow("-52.5, -13.0, 5000")
        << QGeoBoundingCircle(QGeoCoordinate(-52.5, -13.0, 0), 5000);

}

// public QGeoBoundingCircle circle() const
void tst_QGeoMapCircleObject::circle()
{
    QFETCH(QGeoBoundingCircle, circle);

    QGeoMapCircleObject* object = new QGeoMapCircleObject(QGeoCoordinate(10.0, 10.0, 0.0), 1000);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    QSignalSpy spy0(object, SIGNAL(brushChanged(QBrush const&)));
    QSignalSpy spy1(object, SIGNAL(centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy2(object, SIGNAL(penChanged(QPen const&)));
    QSignalSpy spy3(object, SIGNAL(radiusChanged(qreal)));

    //check if object is not there

    map->setCenter(circle.center());

    QPointF point = map->coordinateToScreenPosition(circle.center());

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),0);

    object->setCircle(circle);

    QCOMPARE(object->circle(), circle);

    //check if object is there

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),1);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 1);

}

void tst_QGeoMapCircleObject::pen_data()
{
    QTest::addColumn<QPen>("pen");
    QTest::newRow("blue") << QPen(Qt::white);
    QPen pen;

    pen.setColor(Qt::white);
    QTest::newRow("white") << QPen(Qt::white);
}

// public QPen pen() const
void tst_QGeoMapCircleObject::pen()
{
    QFETCH(QPen, pen);

    QGeoMapCircleObject* object = new QGeoMapCircleObject();

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    QSignalSpy spy0(object, SIGNAL(brushChanged(QBrush const&)));
    QSignalSpy spy1(object, SIGNAL(centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy2(object, SIGNAL(penChanged(QPen const&)));
    QSignalSpy spy3(object, SIGNAL(radiusChanged(qreal)));

    object->setPen(pen);

    pen.setCosmetic(true);
    //according to specs it should be cosmetic
    QCOMPARE(object->pen(), pen);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 1);
    QCOMPARE(spy3.count(), 0);

}

void tst_QGeoMapCircleObject::radius_data()
{
    QTest::addColumn<QGeoCoordinate>("radiusTo");
    QTest::newRow("10.5, 10.0, 0") << QGeoCoordinate(10.5, 10.0, 0);
    QTest::newRow("-10.5, 10.0, 0") << QGeoCoordinate(-10.5, 10.0, 0);
    QTest::newRow("10.5, -10.0, 0") << QGeoCoordinate(10.5, -10.0, 0);
    QTest::newRow("-10.5, -10.0, 0") << QGeoCoordinate(-10.5, -10.0, 0);
}

// public qreal radius() const
void tst_QGeoMapCircleObject::radius()
{
    QFETCH(QGeoCoordinate, radiusTo);

    QGeoCoordinate center(1.0, 1.0, 0);

    QGeoMapCircleObject* object = new QGeoMapCircleObject(center, 10);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    QSignalSpy spy0(object, SIGNAL(brushChanged(QBrush const&)));
    QSignalSpy spy1(object, SIGNAL(centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy2(object, SIGNAL(penChanged(QPen const&)));
    QSignalSpy spy3(object, SIGNAL(radiusChanged(qreal)));

    map->setCenter(radiusTo);

    QPointF point = map->coordinateToScreenPosition(radiusTo);

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),0);

    //make a bit larger, this is not a accuracy test
    qreal radius = radiusTo.distanceTo(center) * 1.2;

    object->setRadius(radius);

    QCOMPARE(object->radius(), radius);

    //now it is bigger
    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),1);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 1);

}

void tst_QGeoMapCircleObject::zvalue_data()
{
    QTest::addColumn<int>("zValue1");
    QTest::addColumn<int>("zValue2");
    QTest::addColumn<int>("zValue3");
    QTest::addColumn<int>("topIdx");
    QTest::newRow("1,2,3") << 1 << 2 << 3 << 2;
    QTest::newRow("1,3,2") << 1 << 3 << 2 << 1;
    QTest::newRow("3,2,1") << 3 << 2 << 1 << 0;
    QTest::newRow("2,1,3") << 2 << 1 << 3 << 2;
}

// public int zValue() const
void tst_QGeoMapCircleObject::zvalue()
{

    QFETCH(int, zValue1);
    QFETCH(int, zValue2);
    QFETCH(int, zValue3);
    QFETCH(int, topIdx);

    QGeoCoordinate center(1.0, 1.0, 0);

    QGeoMapCircleObject* object1 = new QGeoMapCircleObject(center, 1000);
    QGeoMapCircleObject* object2 = new QGeoMapCircleObject(center, 1000);
    QGeoMapCircleObject* object3 = new QGeoMapCircleObject(center, 1000);

    QList<QColor> colors;
    colors << Qt::blue << Qt::red << Qt::green;
    object1->setBrush(QBrush(colors.at(0)));
    object2->setBrush(QBrush(colors.at(1)));
    object3->setBrush(QBrush(colors.at(2)));

    QGraphicsGeoMap* map = m_helper->map();
    map->setCenter(center);
    map->setZoomLevel(14.0);

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

    QPixmap *px[2];
    QPainter *p[2];
    for (int i=0; i < 2; i++) {
        px[i] = new QPixmap(map->size().toSize());
        p[i] = new QPainter(px[i]);
    }
    QStyleOptionGraphicsItem style;
    style.rect = QRect(QPoint(0,0), map->size().toSize());

    QTest::qWait(10);
    map->paint(p[0], &style, 0);

    QPointF point = map->coordinateToScreenPosition(center);
    qDebug("center = (%f, %f)", point.x(), point.y());

    QRgb col = px[0]->toImage().pixel(int(point.x()), int(point.y()));
    QCOMPARE(qRed(col), colors.at(2).red());
    QCOMPARE(qGreen(col), colors.at(2).green());
    QCOMPARE(qBlue(col), colors.at(2).blue());

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),3);

    QVERIFY(map->mapObjectsAtScreenPosition(point).at(0)!=object2);
    QVERIFY(map->mapObjectsAtScreenPosition(point).at(0)!=object3);
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

    QTest::qWait(10);
    map->paint(p[1], &style, 0);

    col = px[1]->toImage().pixel(int(point.x()), int(point.y()));
    QCOMPARE(qRed(col), colors.at(topIdx).red());
    QCOMPARE(qGreen(col), colors.at(topIdx).green());
    QCOMPARE(qBlue(col), colors.at(topIdx).blue());

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),3);

    QVERIFY(map->mapObjectsAtScreenPosition(point).at(zValue1-1)==object1);
    QVERIFY(map->mapObjectsAtScreenPosition(point).at(zValue2-1)==object2);
    QVERIFY(map->mapObjectsAtScreenPosition(point).at(zValue3-1)==object3);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 1);

    for (int i=0; i < 2; i++) {
        delete p[i];
        delete px[i];
    }

}

// public bool isVisible() const
void tst_QGeoMapCircleObject::isVisible()
{

    QGeoCoordinate center(1.0, 1.0, 0);

    QGeoMapCircleObject* object = new QGeoMapCircleObject(center, 1000);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    QSignalSpy spy0(object, SIGNAL(selectedChanged(bool)));
    QSignalSpy spy1(object, SIGNAL(visibleChanged(bool)));
    QSignalSpy spy2(object, SIGNAL(zValueChanged(int)));

    map->setCenter(center);

    QPointF point = map->coordinateToScreenPosition(center);

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
void tst_QGeoMapCircleObject::isSelected()
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

void tst_QGeoMapCircleObject::contains_data()
{

    QTest::addColumn<QGeoCoordinate>("coordinate");
    QTest::newRow("10,10") << QGeoCoordinate(10, 10, 0);
    QTest::newRow("20,20") << QGeoCoordinate(20, 20, 0);
    QTest::newRow("0,0") << QGeoCoordinate(0, 0, 0);

}

// public bool contains(QGeoCoordinate const& coordinate) const
void tst_QGeoMapCircleObject::contains()
{
    QFETCH(QGeoCoordinate, coordinate);

    QGeoCoordinate center(0, 0, 0);

    QGeoMapCircleObject* object = new QGeoMapCircleObject(center, 1000);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

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
void tst_QGeoMapCircleObject::boundingBox()
{
    QGeoCoordinate center(0, 0, 0);

    QGeoMapCircleObject* object = new QGeoMapCircleObject(center, 1000);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    QVERIFY2(object->boundingBox().width()>0,"no bounding box");
    QVERIFY2(object->boundingBox().height()>0,"no bounding box");

    double width = object->boundingBox().width();
    double height = object->boundingBox().height();

    double top = object->boundingBox().topLeft().latitude();
    double bottom = object->boundingBox().bottomRight().latitude();

    QVERIFY(object->boundingBox().topLeft().longitude() < object->boundingBox().bottomRight().longitude());

    QGeoCoordinate dateline(0.0, 180.0, 0.0);

    object->setCenter(dateline);

    QVERIFY2(object->boundingBox().width()!=0,"no bounding box");
    QVERIFY2(object->boundingBox().height()!=0,"no bounding box");

    QCOMPARE(object->boundingBox().width(), width);
    QCOMPARE(object->boundingBox().height(), height);

    QVERIFY(object->boundingBox().topLeft().latitude() == top);
    QVERIFY(object->boundingBox().bottomRight().latitude() == bottom);

    QVERIFY(object->boundingBox().topLeft().longitude() > object->boundingBox().bottomRight().longitude());
}

// QTMOBILITY-1255: Changing z value of map object causes it to break insertion order
void tst_QGeoMapCircleObject::qtmobility1255()
{
    QGeoCoordinate center(0,0,0);

    QGeoMapCircleObject *outer = new QGeoMapCircleObject(center, 5000);
    outer->setZValue(5);
    outer->setBrush(QBrush(Qt::black));
    QGeoMapCircleObject *middle = new QGeoMapCircleObject(center, 3500);
    middle->setZValue(4);
    middle->setBrush(QBrush(Qt::red));
    QGeoMapCircleObject *inner = new QGeoMapCircleObject(center, 1000);
    inner->setZValue(5);
    inner->setBrush(QBrush(Qt::blue));

    QGraphicsGeoMap *map = m_helper->map();

    map->addMapObject(outer);
    map->addMapObject(middle);
    map->addMapObject(inner);
    map->setCenter(center);

    QList<QGeoMapObject*> list;
    list = map->mapObjects();
    QCOMPARE(list.size(), 3);

    QPointF pxCenter = map->coordinateToScreenPosition(center);
    list = map->mapObjectsAtScreenPosition(pxCenter);
    QCOMPARE(list.size(), 3);
    QVERIFY(list.at(0) == middle);
    QVERIFY(list.at(1) == outer);
    QVERIFY(list.at(2) == inner);

    middle->setZValue(5);
    list = map->mapObjectsAtScreenPosition(pxCenter);
    QCOMPARE(list.size(), 3);
    QVERIFY(list.at(0) == outer);
    QVERIFY(list.at(1) == middle);
    QVERIFY(list.at(2) == inner);

    middle->setZValue(6);
    list = map->mapObjectsAtScreenPosition(pxCenter);
    QCOMPARE(list.size(), 3);
    QVERIFY(list.at(0) == outer);
    QVERIFY(list.at(1) == inner);
    QVERIFY(list.at(2) == middle);
}


// QTMOBILITY-1199: Result of QGraphicsGeoMap::mapObjectsInViewport is zoom level dependent
void tst_QGeoMapCircleObject::qtmobility1199()
{
#if QTM_VERSION < 0x010200
    QSKIP("Mobility 1.1 fails qtmobility-1199", SkipAll);
#endif

    QGeoCoordinate seattle(47.609722,-122.333056,0);
    QGeoCoordinate seattle2(47.60981194,-122.33185897);
    QGeoCoordinate seattle3(47.60972200, -122.33332201);

    QGeoMapCircleObject *obj = new QGeoMapCircleObject(seattle3, 30);
    QGeoMapCircleObject *obj2 = new QGeoMapCircleObject(seattle2, 120);

    QPixmap pm(20,20);
    pm.fill(Qt::blue);
    QPainter p(&pm);
    p.setPen(QPen(Qt::red));
    p.drawEllipse(10,10,5,7);
    QGeoMapPixmapObject *pobj = new QGeoMapPixmapObject(seattle, QPoint(0,0),
                                                        pm);

    QGraphicsGeoMap *map = m_helper->map();
    map->setCenter(seattle);
    map->setZoomLevel(18.0);
    map->pan(10, 10);

    map->addMapObject(obj);
    map->addMapObject(obj2);
    map->addMapObject(pobj);

    QTest::qWait(10);

    for (qreal z = 18.0; z >= 0.0; z -= 1.0) {
        map->setZoomLevel(z);
        QTest::qWait(10);

        QPointF coord = map->coordinateToScreenPosition(seattle);
        QList<QGeoMapObject*> list;

        list = map->mapObjectsAtScreenPosition(coord);
        QCOMPARE(list.size(), 3);
        QVERIFY(list.at(0) == obj);
        QVERIFY(list.at(1) == obj2);
        QVERIFY(list.at(2) == pobj);

        list = map->mapObjectsInViewport();
        QCOMPARE(list.size(), 3);
        QVERIFY(list.at(0) == obj);
        QVERIFY(list.at(1) == obj2);
        QVERIFY(list.at(2) == pobj);
    }
}

QTEST_MAIN(tst_QGeoMapCircleObject)
#include "tst_qgeomapcircleobject.moc"
