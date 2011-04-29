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

#ifndef TST_QGEOMANEUVER_H
#define TST_QGEOMANEUVER_H


#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QMetaType>
#include <QSignalSpy>

#include "../qgeocoordinate/qlocationtestutils_p.h"
#include <qgeomaneuver.h>
#include <qgeocoordinate.h>

QTM_USE_NAMESPACE

class tst_QGeoManeuver : public QObject
{
    Q_OBJECT

public:
    tst_QGeoManeuver();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    //Start unit test for QGeoRouteManeuver
    void constructor();
    void copy_constructor();
    void destructor();
    void direction();
    void direction_data();
    void distanceToNextInstruction();
    void instructionText();
    void position();
    void position_data();
    void timeToNextInstruction();
    void waypoint();
    void waypoint_data();
    void isValid();
    void operators();
    //End Unit Test for QGeoRouteManeuver

private:
    QGeoManeuver *qgeomaneuver;
    QGeoCoordinate *qgeocoordinate;

};

Q_DECLARE_METATYPE( QList<double>);
Q_DECLARE_METATYPE (QGeoManeuver::InstructionDirection);

#endif // TST_QGEOMANEUVER_H

