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

#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include <qgeoroutingmanager.h>
#include <qgeosearchmanager.h>

#include <qgeoroutereply.h>
#include <qgeoroutereply.h>
#include <qgeosearchreply.h>
#include <qgeomaprouteobject.h>

#include "marker.h"

using namespace QtMobility;

class MapsWidget;

class Navigator : public QObject
{
    Q_OBJECT
public:
    Navigator(QGeoRoutingManager *routingManager, QGeoSearchManager *searchManager,
              MapsWidget *mapsWidget, const QString &address,
              const QGeoRouteRequest &requestTemplate);
    ~Navigator();

    void start();
    QGeoRoute route() const;

signals:
    void finished();
    void searchError(QGeoSearchReply::Error error, QString errorString);
    void routingError(QGeoRouteReply::Error error, QString errorString);

private slots:
    void on_addressSearchFinished();
    void on_routingFinished();

private:
    QString address;
    QGeoRouteRequest request;

    QGeoRoutingManager *routingManager;
    QGeoSearchManager *searchManager;
    MapsWidget *mapsWidget;

    QGeoSearchReply *addressReply;
    QGeoRouteReply *routeReply;

    QGeoMapRouteObject *routeObject;
    Marker *endMarker;
    Marker *startMarker;

    QGeoRoute firstRoute;
};

#endif // NAVIGATOR_H
