/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QCoreApplication>
#include <QStringList>
#include "rfcommclient.h"
#include <qbluetoothdeviceinfo.h>
//#include <qbluetoothlocaldevice.h>
//#include <QtTest/QtTest>


 
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);    
    RfCommClient client;
    QBluetoothLocalDevice localDevice;
    MyThread mythread;
    
    QObject::connect(&client, SIGNAL(done()), &app, SLOT(quit()));

    QString address;
    QString port;
    QStringList args = QCoreApplication::arguments();
    
    if(args.length() >= 2){
        address = args.at(1);
        if(args.length() >= 3){
            port = args.at(2);
        }
    }

       // use previous value for client, stored earlier
//    if(address.isEmpty()){
//        QSettings settings("QtDF", "bttennis");
//        address = settings.value("lastclient").toString();
//    }

    // hard-code address and port number if not provided
    if(address.isEmpty()){
        address = "6C:9B:02:0C:91:D3";  // "J C7-2"
        port = QString("20");
    }
            
    if(!address.isEmpty()){
        qDebug() << "Connecting to" << address << port;
        QBluetoothDeviceInfo device = QBluetoothDeviceInfo(QBluetoothAddress(address), "", 
                QBluetoothDeviceInfo::MiscellaneousDevice);
        QBluetoothServiceInfo service;
        if (!port.isEmpty()) {
            QBluetoothServiceInfo::Sequence protocolDescriptorList;
            QBluetoothServiceInfo::Sequence protocol;
            protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::Rfcomm))
                     << QVariant::fromValue(port.toUShort());
            protocolDescriptorList.append(QVariant::fromValue(protocol));
            service.setAttribute(QBluetoothServiceInfo::ProtocolDescriptorList,
                                 protocolDescriptorList);
            qDebug() << "port" << port.toUShort() << service.protocolServiceMultiplexer();
        }
        else {
            service.setServiceUuid(QBluetoothUuid(serviceUuid));
        }
        service.setDevice(device);
        // delay so that server is in waiting state
        qDebug() << "Starting sleep";
        mythread.sleep(10);  // seconds
        qDebug() << "Finished sleeping";
        client.startClient(service);
    } else {
        qDebug() << "failed because address and/or port is missing " << address << port;
    }

    
    return app.exec();
}

