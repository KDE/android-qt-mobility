/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "routepresenter.h"

#include <QStringBuilder>
#include <qgeoboundingbox.h>
#include <qgeomaneuver.h>

RoutePresenter::RoutePresenter(QTreeWidget* treeWidget, const QGeoRouteReply* routeReply)
        : GeoPresenter(treeWidget), m_routeReply(routeReply)
{
}

void RoutePresenter::show()
{
    m_treeWidget->clear();

    QTreeWidgetItem* requestTop = new QTreeWidgetItem(m_treeWidget);
    requestTop->setText(0, "request");

    QTreeWidgetItem* modesItem = new QTreeWidgetItem(requestTop);
    modesItem->setText(0, "modes");
    showModes(modesItem, m_routeReply->request(), m_routeReply->request().travelModes());

    QTreeWidgetItem* waypointsItem = new QTreeWidgetItem(requestTop);
    waypointsItem->setText(0, "waypoints");
    waypointsItem->setText(1, QString().setNum(m_routeReply->request().waypoints().length()));
    showPoints(waypointsItem, m_routeReply->request().waypoints());

    QTreeWidgetItem* excludesItem = new QTreeWidgetItem(requestTop);
    excludesItem->setText(0, "exclude areas");
    excludesItem->setText(1, QString().setNum(m_routeReply->request().excludeAreas().length()));
    for (int i = 0; i < m_routeReply->request().excludeAreas().length(); ++i)
        showBoundingBox(excludesItem, m_routeReply->request().excludeAreas().at(i));

    QTreeWidgetItem* alternativesItem = new QTreeWidgetItem(requestTop);
    alternativesItem->setText(0, "alternatives");
    alternativesItem->setText(1, QString().setNum(m_routeReply->request().numberAlternativeRoutes()));

    showRoutes();
}

void RoutePresenter::showRoutes()
{
    QTreeWidgetItem* routeTop = new QTreeWidgetItem(m_treeWidget);
    routeTop->setText(0, "routes");

    QTreeWidgetItem* countItem = new QTreeWidgetItem(routeTop);
    countItem->setText(0, "count");
    countItem->setText(1, QString().setNum(m_routeReply->routes().size()));

    const QList<QGeoRoute> routes = m_routeReply->routes();
    for (int i = 0; i < routes.length(); ++i) {
        showRoute(routeTop, routes[i]);
    }
}

void RoutePresenter::showRoute(QTreeWidgetItem* top, const QGeoRoute& route)
{
    QTreeWidgetItem* routeItem = new QTreeWidgetItem(top);
    routeItem->setText(0, "route");

    QTreeWidgetItem* idItem = 0;
    if (!route.routeId().isEmpty()) {
        idItem = new QTreeWidgetItem(routeItem);
        idItem->setText(0, "id");
        idItem->setText(1, route.routeId());
    }

    QTreeWidgetItem* modeItem = new QTreeWidgetItem(routeItem);
    modeItem->setText(0, "mode");
    showModes(modeItem, route.request(), route.travelMode());

    QTreeWidgetItem* distanceItem = new QTreeWidgetItem(routeItem);
    distanceItem->setText(0, "distance");
    distanceItem->setText(1, QString().setNum(route.distance()));

    showBoundingBox(routeItem, route.bounds());

    QTreeWidgetItem* wayPointsItem = new QTreeWidgetItem(routeItem);
    QString overviewLabel = "overview";
    if (route.path().count() > 100)
        overviewLabel += "(100)";
    wayPointsItem->setText(0, overviewLabel);
    showPoints(wayPointsItem, route.path());

    QList<QGeoRouteSegment> segments;
    QGeoRouteSegment segment = route.firstRouteSegment();
    while (segment.isValid()) {
        segments << segment;
        segment = segment.nextRouteSegment();
    }

    QTreeWidgetItem* segmentsItem = new QTreeWidgetItem(routeItem);
    QString segmentsLabel = "segments";
    if (segments.length() > 100)
        segmentsLabel += "(100)";

    segmentsItem->setText(0, segmentsLabel);

    segmentsItem->setText(1, QString().setNum(segments.length()));
    for (int i = 0; i < segments.length() && i < 100; ++i) {
        showRouteSegment(segmentsItem, segments[i]);
    }
}

void RoutePresenter::showRouteSegment(QTreeWidgetItem* routeItem, const QGeoRouteSegment &segment)
{
    QTreeWidgetItem* segmentItem = new QTreeWidgetItem(routeItem);
    segmentItem->setText(0, "segment");

    QTreeWidgetItem* durationItem = new QTreeWidgetItem(segmentItem);
    durationItem->setText(0, "duration");
    durationItem->setText(1, QString().setNum(segment.travelTime()));

    QTreeWidgetItem* distanceItem = new QTreeWidgetItem(segmentItem);
    distanceItem->setText(0, "distance");
    distanceItem->setText(1, QString().setNum(segment.distance()));

    // add back in when more qgeoinstruction classes are made available
    /*
        QString s = segment->turn();

        if (!s.isEmpty()) {
            propItem = new QTreeWidgetItem(maneuverItem);
            propItem->setText(0, "turn");
            propItem->setText(1, s);
        }

        s = segment->streetName();

        if (!s.isEmpty()) {
            propItem = new QTreeWidgetItem(maneuverItem);
            propItem->setText(0, "street name");
            propItem->setText(1, s);
        }

        s = segment->routeName();

        if (!s.isEmpty()) {
            propItem = new QTreeWidgetItem(maneuverItem);
            propItem->setText(0, "route name");
            propItem->setText(1, s);
        }

        s = segment->nextStreetName();

        if (!s.isEmpty()) {
            propItem = new QTreeWidgetItem(maneuverItem);
            propItem->setText(0, "next street name");
            propItem->setText(1, s);
        }

        s = segment->signPost();

        if (!s.isEmpty()) {
            propItem = new QTreeWidgetItem(maneuverItem);
            propItem->setText(0, "sign post");
            propItem->setText(1, s);
        }

        propItem = new QTreeWidgetItem(maneuverItem);
        propItem->setText(0, "traffic direction");
        propItem->setText(1, QString().setNum(segment->trafficDirection()));
    */
    QTreeWidgetItem* pathItem = new QTreeWidgetItem(segmentItem);
    pathItem->setText(0, "path");
    showPoints(pathItem, segment.path());

    if (!segment.maneuver().instructionText().isEmpty()) {
        QTreeWidgetItem* instructionItem = new QTreeWidgetItem(segmentItem);
        instructionItem->setText(0, "instruction");

        QTreeWidgetItem* positionItem = new QTreeWidgetItem(instructionItem);
        positionItem->setText(0, "position");
        QList<QGeoCoordinate> points;
        points.append(segment.maneuver().position());
        showPoints(positionItem, points);

        QTreeWidgetItem* instructionTextItem = new QTreeWidgetItem(instructionItem);
        instructionTextItem->setText(0, "text");
        instructionTextItem->setText(1, segment.maneuver().instructionText());
    }
}

void RoutePresenter::showPoints(QTreeWidgetItem* pointsItem, const QList<QGeoCoordinate>& points)
{
    for (int i = 0; i < points.count() && i < 100; ++i) {
        QTreeWidgetItem* point = new QTreeWidgetItem(pointsItem);
        point->setText(0, QString().setNum(i + 1));
        point->setText(1, formatGeoCoordinate(points[i]));
    }
}

//-------------------------------------------------------------------------------------------------
void GeoPresenter::showBoundingBox(QTreeWidgetItem* routeItem, const QGeoBoundingBox& box)
{
    QTreeWidgetItem* boxItem = new QTreeWidgetItem(routeItem);
    boxItem->setText(0, "bounding box");

    QTreeWidgetItem* nwItem = new QTreeWidgetItem(boxItem);
    nwItem->setText(0, "NW");
    nwItem->setText(1, formatGeoCoordinate(box.topLeft()));

    QTreeWidgetItem* seItem = new QTreeWidgetItem(boxItem);
    seItem->setText(0, "SE");
    seItem->setText(1, formatGeoCoordinate(box.bottomRight()));
}

QString GeoPresenter::formatGeoCoordinate(const QGeoCoordinate& coord)
{
    return QString().setNum(coord.latitude()) %
           ", " %
           QString().setNum(coord.longitude());
}

void RoutePresenter::showModes(QTreeWidgetItem* top,
                               const QGeoRouteRequest &request,
                               QGeoRouteRequest::TravelModes travelModes)
{
    QTreeWidgetItem* optimizationItem = new QTreeWidgetItem(top);
    optimizationItem->setText(0, "optimization");

    QGeoRouteRequest::RouteOptimizations optimization = request.routeOptimization();

    if (optimization.testFlag(QGeoRouteRequest::ShortestRoute)) {
        QTreeWidgetItem* shortestRouteItem = new QTreeWidgetItem(optimizationItem);
        shortestRouteItem->setText(1, "ShortestRoute");
    }
    if (optimization.testFlag(QGeoRouteRequest::FastestRoute)) {
        QTreeWidgetItem* fastestRouteItem = new QTreeWidgetItem(optimizationItem);
        fastestRouteItem->setText(1, "FastestRoute");
    }
    if (optimization.testFlag(QGeoRouteRequest::MostEconomicRoute)) {
        QTreeWidgetItem* economicRouteItem = new QTreeWidgetItem(optimizationItem);
        economicRouteItem->setText(1, "MostEconomicRoute");
    }
    if (optimization.testFlag(QGeoRouteRequest::MostScenicRoute)) {
        QTreeWidgetItem* scenicRouteItem = new QTreeWidgetItem(optimizationItem);
        scenicRouteItem->setText(1, "MostScenicRoute");
    }

    QTreeWidgetItem* travelModeItem = new QTreeWidgetItem(top);
    travelModeItem->setText(0, "travel mode");

    if (travelModes.testFlag(QGeoRouteRequest::CarTravel)) {
        QTreeWidgetItem* carTravelItem = new QTreeWidgetItem(travelModeItem);
        carTravelItem->setText(1, "CarTravel");
    }
    if (travelModes.testFlag(QGeoRouteRequest::PedestrianTravel)) {
        QTreeWidgetItem* pedestrianTravelItem = new QTreeWidgetItem(travelModeItem);
        pedestrianTravelItem->setText(1, "PedestrianTravel");
    }
    if (travelModes.testFlag(QGeoRouteRequest::PublicTransitTravel)) {
        QTreeWidgetItem* transitTravelItem = new QTreeWidgetItem(travelModeItem);
        transitTravelItem->setText(1, "PublicTransitTravel");
    }
    if (travelModes.testFlag(QGeoRouteRequest::BicycleTravel)) {
        QTreeWidgetItem* bicycleTravelItem = new QTreeWidgetItem(travelModeItem);
        bicycleTravelItem->setText(1, "BicycleTravel");
    }
    if (travelModes.testFlag(QGeoRouteRequest::TruckTravel)) {
        QTreeWidgetItem* truckTravelItem = new QTreeWidgetItem(travelModeItem);
        truckTravelItem->setText(1, "TruckTravel");
    }

    QTreeWidgetItem* featuresItem = new QTreeWidgetItem(top);
    featuresItem->setText(0, "features");

    QList<QGeoRouteRequest::FeatureType> featureTypeList
    = request.featureTypes();

    for (int i = 0; i < featureTypeList.size(); ++i) {
        QGeoRouteRequest::FeatureWeight weight
        = request.featureWeight(featureTypeList.at(i));

        if (weight == QGeoRouteRequest::NeutralFeatureWeight)
            continue;

        QString labelString = "";
        switch (weight) {
            case QGeoRouteRequest::PreferFeatureWeight:
                labelString = "Prefer";
                break;
            case QGeoRouteRequest::AvoidFeatureWeight:
                labelString = "Avoid";
                break;
            case QGeoRouteRequest::DisallowFeatureWeight:
                labelString = "Disallow";
                break;
        }

        if (labelString.isEmpty())
            continue;

        labelString += " ";

        switch (featureTypeList.at(i)) {
            case QGeoRouteRequest::TollFeature:
                labelString += "tolls";
                break;
            case QGeoRouteRequest::HighwayFeature:
                labelString += "highways";
                break;
            case QGeoRouteRequest::PublicTransitFeature:
                labelString += "public transit";
                break;
            case QGeoRouteRequest::FerryFeature:
                labelString += "ferries";
                break;
            case QGeoRouteRequest::TunnelFeature:
                labelString += "tunnels";
                break;
            case QGeoRouteRequest::DirtRoadFeature:
                labelString += "dirt roads";
                break;
            case QGeoRouteRequest::ParksFeature:
                labelString += "parks";
                break;
            case QGeoRouteRequest::MotorPoolLaneFeature:
                labelString += "motor pool lanes";
                break;
            default:
                labelString = "";
                break;
        }

        if (labelString.isEmpty())
            continue;

        QTreeWidgetItem* item = new QTreeWidgetItem(featuresItem);
        item->setText(1, labelString);
    }
}


