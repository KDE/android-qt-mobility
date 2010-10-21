/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qlocationconnection_simulator_p.h"
#include "qgeopositioninfosource_simulator_p.h"
#include "qlocationdata_simulator_p.h"
#include "mobilitysimulatorglobal.h"

#include <private/qsimulatordata_p.h>
#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtCore/QDataStream>

#include <QtNetwork/QLocalSocket>

QTM_BEGIN_NAMESPACE

using namespace QtSimulatorPrivate;

namespace Simulator
{
    LocationConnection::LocationConnection(MobilityConnection *mobilityCon)
        : QObject(mobilityCon)
        , mConnection(mobilityCon)
        , mInitialDataReceived(false)
    {
        qt_registerLocationTypes();
        mobilityCon->addMessageHandler(this);
    }


    void LocationConnection::getInitialData()
    {
        RemoteMetacall<void>::call(mConnection->sendSocket(), NoSync, "setRequestsLocationInfo");

        while (!mInitialDataReceived) {
            mConnection->receiveSocket()->waitForReadyRead(100);
            mConnection->onReadyRead();
        }
    }

    void LocationConnection::initialLocationDataSent()
    {
        mInitialDataReceived = true;
    }

    void LocationConnection::setLocationData(const QGeoPositionInfoData &data)
    {
        *qtPositionInfo() = data;
    }

    void LocationConnection::setSatelliteData(const QGeoSatelliteInfoData &data)
    {
        *qtSatelliteInfo() = data;
    }

#include "moc_qlocationconnection_simulator_p.cpp"

} // namespace


void ensureSimulatorConnection()
{
    using namespace Simulator;

    static bool connected = false;
    if (connected)
        return;

    connected = true;
    MobilityConnection *connection = MobilityConnection::instance();
    LocationConnection *locationConnection = new LocationConnection(connection);
    locationConnection->getInitialData();
}

QGeoPositionInfoData *qtPositionInfo()
{
    static QGeoPositionInfoData *positionInfo = 0;
    if (!positionInfo) {
        positionInfo = new QGeoPositionInfoData;
    }

    return positionInfo;
}

QGeoSatelliteInfoData *qtSatelliteInfo()
{
    static QGeoSatelliteInfoData *satelliteInfo;
    if (!satelliteInfo) {
        satelliteInfo = new QGeoSatelliteInfoData;
    }

    return satelliteInfo;
}


QTM_END_NAMESPACE
