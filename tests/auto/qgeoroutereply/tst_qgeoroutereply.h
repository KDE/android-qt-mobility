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

#ifndef TST_QGEOROUTEREPLY_H
#define TST_QGEOROUTEREPLY_H

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QMetaType>
#include <QSignalSpy>

#include <qgeocoordinate.h>
#include <qgeorouterequest.h>
#include <qgeoroutereply.h>
#include <qgeoroute.h>

#include "../qgeocoordinate/qlocationtestutils_p.h"

QTM_USE_NAMESPACE
class SubRouteReply :public QGeoRouteReply
{
    Q_OBJECT
public:
    SubRouteReply(QGeoRouteRequest request):QGeoRouteReply(request) {}
    void callSetError(QGeoRouteReply::Error error, QString msg) {setError(error,msg);}
    void callSetFinished(bool finished) {setFinished(finished);}
    void callSetRoutes(const QList<QGeoRoute> & routes ) {setRoutes(routes);}
};

class tst_QGeoRouteReply :public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    //Start Unit Test for QGeoRouteReply
private slots:
    void constructor();
    void constructor_error();
    void constructor_error_data();
    void destructor();
    void destructor_data();
    void routes();
    void finished();
    void abort();
    void error();
    void error_data();
    void request();
    //End Unit Test for QGeoRouteReply


private:
    QGeoCoordinate *qgeocoordinate1;
    QGeoCoordinate *qgeocoordinate2;
    QGeoCoordinate *qgeocoordinate3;
    QGeoRouteRequest *qgeorouterequest;
    QSignalSpy *signalerror;
    QSignalSpy *signalfinished;
    QList<QGeoCoordinate> waypoints;
    SubRouteReply* reply;
};

Q_DECLARE_METATYPE( QList<QGeoRoute>);
Q_DECLARE_METATYPE( QList<QGeoCoordinate>);
Q_DECLARE_METATYPE( QList<double>);
Q_DECLARE_METATYPE( QGeoRouteReply::Error);

#endif // TST_QGEOROUTEREPLY_H

