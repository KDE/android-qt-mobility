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

#include <QPointF>
#include <QNetworkProxy>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "qmaptile.h"
#include "qrouterequest.h"
#include "qmapmarker.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow),
        geoNetworkManager("", "")
{
    ui->setupUi(this);

    QNetworkProxy proxy(QNetworkProxy::HttpProxy, "172.16.42.41", 8080);
    geoNetworkManager.setMapProxy(proxy);
    geoNetworkManager.setMapServer("maptile.mapplayer.maps.svc.ovi.com");

    qgv = new QGraphicsView(this);
    qgv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    qgv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    qgv->setVisible(true);
    qgv->setGeometry(QRect(0, 0, width(), height()));

    QGraphicsScene* scene = new QGraphicsScene(0, 0, width(), height());
    qgv->setScene(scene);

    mapView = new QMapView();
    qgv->scene()->addItem(mapView);
    mapView->setGeometry(0, 0, width(), height());

    QObject::connect(&geoNetworkManager, SIGNAL(finished(QRouteReply*)),
                     this, SLOT(routeReplyFinished(QRouteReply*)));
    QObject::connect(mapView, SIGNAL(mapClicked(QGeoCoordinate, QGraphicsSceneMouseEvent*)),
                     this, SLOT(mapClicked(QGeoCoordinate, QGraphicsSceneMouseEvent*)));
    QObject::connect(mapView, SIGNAL(zoomLevelChanged(quint16, quint16)),
                     this, SLOT(zoomLevelChanged(quint16, quint16)));

    //mapView->init(&geoNetworkManager, QPointF(13, 52.35));
    mapView->init(&geoNetworkManager, QGeoCoordinate(52.35, 13));

    slider = new QSlider(Qt::Vertical, this);
    slider->setMinimum(0);
    slider->setMaximum(18);
    slider->setSliderPosition(4);
    slider->setGeometry(10, 10, 30, 100);
    slider->setVisible(true);
    QObject::connect(slider, SIGNAL(sliderMoved(int)),
                     mapView, SLOT(setZoomLevel(int)));

    //QGraphicsProxyWidget* proxy = qgv->scene()->addWidget(slider);

    popupMenu = new QMenu(this);
    mnMarker = popupMenu->addAction("Add marker here");
    popupMenu->addSeparator();
    mnRoute = popupMenu->addAction("Add route");
    mnLine = popupMenu->addAction("Draw line");
    mnRect = popupMenu->addAction("Draw rectangle");
    mnPolygon = popupMenu->addAction("Draw polygon");
    popupMenu->addSeparator();
    mnDay = popupMenu->addAction("Normal daylight");
    mnSat = popupMenu->addAction("Satellite");
    mnTer = popupMenu->addAction("Terrain");
    QObject::connect(mnMarker, SIGNAL(triggered(bool)),
                     this, SLOT(addMarker(bool)));
    QObject::connect(mnLine, SIGNAL(triggered(bool)),
                     this, SLOT(drawLine(bool)));
    QObject::connect(mnRect, SIGNAL(triggered(bool)),
                     this, SLOT(drawRect(bool)));
    QObject::connect(mnPolygon, SIGNAL(triggered(bool)),
                     this, SLOT(drawPolygon(bool)));
    QObject::connect(mnRoute, SIGNAL(triggered(bool)),
                     this, SLOT(setRtFromTo(bool)));
    QObject::connect(mnDay, SIGNAL(triggered(bool)),
                     this, SLOT(setScheme(bool)));
    QObject::connect(mnSat, SIGNAL(triggered(bool)),
                     this, SLOT(setScheme(bool)));
    QObject::connect(mnTer, SIGNAL(triggered(bool)),
                     this, SLOT(setScheme(bool)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    qgv->resize(event->size());
    qgv->setSceneRect(0, 0, event->size().width(), event->size().height());
    mapView->resize(event->size());
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void MainWindow::routeReplyFinished(QRouteReply* reply)
{
    QList<QRoute> routes = reply->routes();

    if (routes.size() > 0) {
        QColor routeColor(Qt::blue);
        routeColor.setAlpha(127); //semi-transparent
        QPen pen(routeColor);
        pen.setWidth(7);
        pen.setCapStyle(Qt::RoundCap);
        mapView->addRoute(routes[0], pen);
    }
}

void MainWindow::mapClicked(QGeoCoordinate geoCoord, QGraphicsSceneMouseEvent* mouseEvent)
{
    if (mouseEvent->buttons() & Qt::RightButton) {
        lastClicked = geoCoord;
        popupMenu->popup(mouseEvent->screenPos());
    }
}

void MainWindow::setRtFromTo(bool /*checked*/)
{
    if (selectedMarkers.count() < 2)
        return;

    QGeoCoordinate from = selectedMarkers.first()->point();
    QGeoCoordinate to = selectedMarkers.last()->point();
    QRouteRequest request;
    request.setSource(from);
    request.setDestination(to);

    for (int i = 1; i < selectedMarkers.count() - 1; i++) {
        request.addStopOver(selectedMarkers[i]->point());
    }

    geoNetworkManager.get(request);
    selectedMarkers.clear();
}


void MainWindow::zoomLevelChanged(quint16 /*oldZoomLevel*/, quint16 newZoomLevel)
{
    slider->setSliderPosition(newZoomLevel);
}

void MainWindow::setScheme(bool /*checked*/)
{
    if (sender() == mnDay)
        mapView->setScheme(MapScheme(MapScheme::Normal_Day));
    else if (sender() == mnSat)
        mapView->setScheme(MapScheme(MapScheme::Satellite_Day));
    else if (sender() == mnTer)
        mapView->setScheme(MapScheme(MapScheme::Terrain_Day));
}

void MainWindow::addMarker(bool /*checked*/)
{
    selectedMarkers.append(mapView->addMarker(lastClicked,
                           QString::number(selectedMarkers.count() + 1)));
}

void MainWindow::drawLine(bool /*checked*/)
{
    for (int i = 0; i < selectedMarkers.count() - 1; i++) {
        const QMapMarker* m1 = selectedMarkers[i];
        const QMapMarker* m2 = selectedMarkers[i + 1];
        QPen pen(Qt::red);
        pen.setWidth(2);
        mapView->addLine(m1->point(), m2->point(), pen, 1);
    }

    selectedMarkers.clear();
}

void MainWindow::drawRect(bool /*checked*/)
{
    for (int i = 0; i < selectedMarkers.count() - 1; i++) {
        const QMapMarker* m1 = selectedMarkers[i];
        const QMapMarker* m2 = selectedMarkers[i + 1];
        QPen pen(Qt::white);
        pen.setWidth(2);
        QColor fill(Qt::black);
        fill.setAlpha(65);
        mapView->addRect(m1->point(), m2->point(), pen, QBrush(fill), 1);
    }

    selectedMarkers.clear();
}

void MainWindow::drawPolygon(bool /*checked*/)
{
    QList<QGeoCoordinate> coords;

    for (int i = 0; i < selectedMarkers.count(); i++) {
        coords.append(selectedMarkers[i]->point());
    }

    QPen pen(Qt::white);
    pen.setWidth(2);
    QColor fill(Qt::black);
    fill.setAlpha(65);
    mapView->addPolygon(coords, pen, QBrush(fill), 1);

    selectedMarkers.clear();
}

