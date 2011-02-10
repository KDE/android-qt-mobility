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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qnetworksession.h>
#include <qnetworkconfigmanager.h>

#include "qgeoserviceprovider.h"
#include "qgeopositioninfosource.h"
#include "qgeoroutereply.h"

#include "mapswidget.h"
#include "marker.h"
#include "searchdialog.h"
#include "markerdialog.h"
#include "navigatedialog.h"
#include "navigator.h"

using namespace QtMobility;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();

public slots:
    void initialize();

private slots:
    void showSearchDialog();
    void showNavigateDialog();
    void showMarkerDialog(Marker *marker);
    void goToMyLocation();

    void updateMyPosition(QGeoPositionInfo info);
    void disableTracking();

    void showErrorMessage(QGeoSearchReply::Error err, QString msg);
    void showErrorMessage(QGeoRouteReply::Error err, QString msg);

    void openNetworkSession();

private:
    QGeoServiceProvider *serviceProvider;
    MapsWidget *mapsWidget;
    MarkerManager *markerManager;
    QGeoPositionInfoSource *positionSource;
    Navigator *lastNavigator;

    QNetworkSession *session;
    QNetworkConfigurationManager *netConfigManager;

    bool tracking;
    bool firstUpdate;
};

#endif // MAINWINDOW_H
