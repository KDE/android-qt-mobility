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

#include "qbluetoothlocaldevice.h"
#include "qbluetoothaddress.h"

#include "bluez/manager_p.h"
#include "bluez/adapter_p.h"

QTM_BEGIN_NAMESPACE

class QBluetoothLocalDevicePrivate
{
public:
    OrgBluezAdapterInterface *adapter;
};

/*!
    Constructs a QBluetoothLocalDevice.
*/
QBluetoothLocalDevice::QBluetoothLocalDevice(QObject *parent)
:   QObject(parent)
{
    OrgBluezManagerInterface manager(QLatin1String("org.bluez"), QLatin1String("/"),
                                     QDBusConnection::systemBus());

    QDBusPendingReply<QDBusObjectPath> reply = manager.DefaultAdapter();
    reply.waitForFinished();
    if (reply.isError())
        return;

    OrgBluezAdapterInterface *adapter = new OrgBluezAdapterInterface(QLatin1String("org.bluez"),
                                                                     reply.value().path(),
                                                                     QDBusConnection::systemBus());

    this->d_ptr = new QBluetoothLocalDevicePrivate;
    this->d_ptr->adapter = adapter;    

}

QBluetoothLocalDevice::QBluetoothLocalDevice(const QBluetoothAddress &address, QObject *parent)
: QObject(parent)
{
    OrgBluezManagerInterface manager(QLatin1String("org.bluez"), QLatin1String("/"),
                                     QDBusConnection::systemBus());

    QDBusPendingReply<QList<QDBusObjectPath> > reply = manager.ListAdapters();
    reply.waitForFinished();
    if (reply.isError())
        return;


    foreach (const QDBusObjectPath &path, reply.value()) {
        OrgBluezAdapterInterface *adapter = new OrgBluezAdapterInterface(QLatin1String("org.bluez"),
                                                                         path.path(),
                                                                         QDBusConnection::systemBus());

        QDBusPendingReply<QVariantMap> reply = adapter->GetProperties();
        reply.waitForFinished();
        if (reply.isError())
            continue;

        QBluetoothAddress path_address(reply.value().value(QLatin1String("Address")).toString());

        if(path_address == address){
            this->d_ptr = new QBluetoothLocalDevicePrivate;
            this->d_ptr->adapter = adapter;
            break;
        }
        else {
            delete adapter;
        }
    }
}

QString QBluetoothLocalDevice::name() const
{    
    if (!d_ptr)
        return QString();

    QDBusPendingReply<QVariantMap> reply = d_ptr->adapter->GetProperties();
    reply.waitForFinished();
    if (reply.isError())
        return QString();

    return reply.value().value(QLatin1String("Name")).toString();
}

QBluetoothAddress QBluetoothLocalDevice::address() const
{
    if (!d_ptr)
        return QBluetoothAddress();

    QDBusPendingReply<QVariantMap> reply = d_ptr->adapter->GetProperties();
    reply.waitForFinished();
    if (reply.isError())
        return QBluetoothAddress();

    return QBluetoothAddress(reply.value().value(QLatin1String("Address")).toString());
}

void QBluetoothLocalDevice::powerOn()
{
    if (!d_ptr)
        return;

    d_ptr->adapter->SetProperty(QLatin1String("Powered"), QDBusVariant(QVariant::fromValue(true)));
}

void QBluetoothLocalDevice::setHostMode(QBluetoothLocalDevice::HostMode mode)
{
    if (!d_ptr)
        return;

    switch (mode) {
    case HostDiscoverable:
        d_ptr->adapter->SetProperty(QLatin1String("Powered"), QDBusVariant(QVariant::fromValue(true)));
        d_ptr->adapter->SetProperty(QLatin1String("Discoverable"),
                                QDBusVariant(QVariant::fromValue(true)));
        break;
    case HostConnectable:
        d_ptr->adapter->SetProperty(QLatin1String("Powered"), QDBusVariant(QVariant::fromValue(true)));
        d_ptr->adapter->SetProperty(QLatin1String("Discoverable"),
                                QDBusVariant(QVariant::fromValue(false)));
        break;
    case HostPoweredOff:
        d_ptr->adapter->SetProperty(QLatin1String("Powered"),
                                QDBusVariant(QVariant::fromValue(false)));
//        d->adapter->SetProperty(QLatin1String("Discoverable"),
//                                QDBusVariant(QVariant::fromValue(false)));
        break;
    }
}

QBluetoothLocalDevice::HostMode QBluetoothLocalDevice::hostMode() const
{
    if (!d_ptr)
        return HostPoweredOff;

    QDBusPendingReply<QVariantMap> reply = d_ptr->adapter->GetProperties();
    reply.waitForFinished();
    if (reply.isError())
        return HostPoweredOff;

    if (!reply.value().value(QLatin1String("Powered")).toBool())
        return HostPoweredOff;
    else if (reply.value().value(QLatin1String("Discoverable")).toBool())
        return HostDiscoverable;
    else if (reply.value().value(QLatin1String("Powered")).toBool())
        return HostConnectable;

    return HostPoweredOff;
}

QList<QBluetoothHostInfo> QBluetoothLocalDevice::allDevices()
{
    QList<QBluetoothHostInfo> localDevices;

    OrgBluezManagerInterface manager(QLatin1String("org.bluez"), QLatin1String("/"),
                                     QDBusConnection::systemBus());

    QDBusPendingReply<QList<QDBusObjectPath> > reply = manager.ListAdapters();
    reply.waitForFinished();
    if (reply.isError())
        return localDevices;


    foreach (const QDBusObjectPath &path, reply.value()) {
        QBluetoothHostInfo hostinfo;
        OrgBluezAdapterInterface adapter(QLatin1String("org.bluez"), path.path(),
                                         QDBusConnection::systemBus());

        QDBusPendingReply<QVariantMap> reply = adapter.GetProperties();
        reply.waitForFinished();
        if (reply.isError())
            continue;

        hostinfo.setAddress(QBluetoothAddress(reply.value().value(QLatin1String("Address")).toString()));
        hostinfo.setName(reply.value().value(QLatin1String("Name")).toString());

        localDevices.append(hostinfo);
    }

    return localDevices;
}


QTM_END_NAMESPACE
