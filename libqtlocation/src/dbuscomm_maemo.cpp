/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "dbuscomm_maemo_p.h"

#include <iostream>
using namespace std;

QTM_BEGIN_NAMESPACE

const QString DBusComm::positioningdService   = QString("com.nokia.positioningd.client");
const QString DBusComm::positioningdPath      = QString("/com/nokia/positioningd/client");
const QString DBusComm::positioningdInterface = QString("com.nokia.positioningd.client");

DBusComm::DBusComm()
{
}


int DBusComm::init()
{
    if (!QDBusConnection::sessionBus().isConnected()) {
        cerr << "Cannot connect to the D-BUS session bus.\n";
        return -1;
    }

    // Application auto-start by dbus may take a while, so try
    // connecting a few times.

    int cnt = 6; 
    do {
        cout << "Connecting to positioning daemon..." << endl;
        positioningdProxy = new QDBusInterface(positioningdService,
                                               positioningdPath,
                                               positioningdInterface,
                                               QDBusConnection::sessionBus());
        usleep(500000);
        cnt--;
    } while (cnt && (positioningdProxy->isValid() == false));

    if (positioningdProxy->isValid() == false) {
        cerr << "DBus connection to positioning daemon failed.\n";
        return -1;
    }

    if (createUniqueName() == false) { // set myService, myPath 
        return -1;
    }

    dbusServer = new DBusServer(&serverObj, this);
    QDBusConnection::sessionBus().registerObject(myPath, &serverObj);
    if (!QDBusConnection::sessionBus().registerService(myService)) {
        cerr << qPrintable(QDBusConnection::sessionBus().lastError().message()) << endl;
        return -1;
    }

    sendDBusRegister();

    return 0;
}


void DBusComm::receivePositionUpdate(const QGeoPositionInfo &update)
{
    emit receivedPositionUpdate(update);
}


void DBusComm::receiveSatellitesInView(const QList<QGeoSatelliteInfo> &info)
{
    emit receivedSatellitesInView(info);
}


void DBusComm::receiveSatellitesInUse(const QList<QGeoSatelliteInfo> &info)
{
    emit receivedSatellitesInUse(info);
}


void DBusComm::receiveSettings(const QGeoPositionInfoSource::PositioningMethod methods,
                              const int interval)
{
    Q_UNUSED(methods)
    cout << "Interval confirmed to be :" << interval << "\n";
    // FIXME save these

    return;
}


bool DBusComm::sendDBusRegister()
{
#if 1
    QDBusReply<int> reply;
    reply = positioningdProxy->call("registerListener",
                                    myService.toAscii().constData(),
                                    myPath.toAscii().constData());
    if (reply.isValid()) {
        clientId = reply.value();
        cout << "Register client ID: " << clientId << endl;
    } else {
        cerr << endl << "DBus error:\n";
        cerr << reply.error().name().toAscii().constData() << endl;
        cerr << reply.error().message().toAscii().constData() << endl;
        return false;
    }
#else
    QList<QVariant> args;
    QDBusMessage message = QDBusMessage::createMethodCall(positioningdService, positioningdPath,
                                                          positioningdInterface, "registerListener");
    args << myService.toAscii().constData() << myPath.toAscii().constData();
    message.setArguments(args);
    QDBusPendingCall pending = QDBusConnection::systemBus().asyncCall(message, 10000);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pending, this);
    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     this, SLOT(registerDone(QDBusPendingCallWatcher*)));
    
    cout << "DBusComm::sendDBusRegister() Register client\n";
#endif
    return true;
}


void DBusComm::registerDone(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<int> reply = *call;
    if (reply.isValid()) {
        clientId = reply.value();
        cout << "Register client ID: " << clientId << endl;
    } 
    else {
        std::cerr << "DBusComm::register Dbus Error:" << qPrintable(reply.error().message()) << "\n";
    }
}


bool DBusComm::sendConfigRequest(Command command, QGeoPositionInfoSource::PositioningMethods method,
                                 int interval) const
{
    QDBusReply<int> reply; 
    reply = positioningdProxy->call("configSession", clientId, command, int(method), interval);
    
    //cout << "sessionConfigRequest cmd: cmd:" << command << " method: ";
    //cout << method << " interval: " << interval << "\n";
    
    if (reply.isValid()) {
        int n = reply.value();
        cout << "sessionConfigRequest:Reply: " << n << endl;
    } else {
        cerr << endl << "DBus error:\n";
        cerr << reply.error().name().toAscii().constData() << endl;
        cerr << reply.error().message().toAscii().constData() << endl;
        return false;
    }

    return true;
}


QGeoPositionInfo& DBusComm::requestLastKnownPosition(bool satelliteMethodOnly)
{
    QDBusReply<QByteArray> reply; 
    reply = positioningdProxy->call("latestPosition", satelliteMethodOnly);
    static QGeoPositionInfo update;

    if (reply.isValid()) {
        cout << "requestLastKnownPosition(): received update\n";
        QByteArray message = reply.value();
        QDataStream stream(message);
        stream >> update;
    } else {
        cerr << endl << "DBus error:\n";
        cerr << reply.error().name().toAscii().constData() << endl;
        cerr << reply.error().message().toAscii().constData() << endl;
        update = QGeoPositionInfo();
    }

    return update;
} 


bool DBusComm::createUniqueName()
{
    QFile uuidfile("/proc/sys/kernel/random/uuid");
    if (!uuidfile.open(QIODevice::ReadOnly)) {
        cerr << "UUID file failed.";
        return false;
    }

    QTextStream in(&uuidfile);
    QString uuid = 'I' + in.readLine();
    uuid.replace('-', 'I');
    myService   = "com.nokia.qlocation." + uuid;
    myPath      = "/com/nokia/qlocation/" + uuid;

    return true;
}

#include "moc_dbuscomm_maemo_p.cpp"
QTM_END_NAMESPACE

