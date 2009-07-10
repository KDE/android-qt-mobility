/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QObject>
#include <QList>
#include <QtDBus>
#include <QDBusConnection>
#include <QDBusError>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDBusPendingCallWatcher>
#include <QDBusObjectPath>
#include <QDBusPendingCall>

#include <NetworkManager/NetworkManager.h>

#include <qnmdbushelper_p.h>
#include "qnetworkmanagerservice_p.h"

//Q_DECLARE_METATYPE(QList<uint>)

static QDBusConnection dbusConnection = QDBusConnection::systemBus();
//static QDBusInterface iface(NM_DBUS_SERVICE, NM_DBUS_PATH, NM_DBUS_INTERFACE, dbusConnection);

class QNetworkManagerInterfacePrivate
{
public:
    QDBusInterface *connectionInterface;
};

QNetworkManagerInterface::QNetworkManagerInterface(QObject *parent)
        : QObject(parent)
{
    d = new QNetworkManagerInterfacePrivate();
    d->connectionInterface = new QDBusInterface(NM_DBUS_SERVICE,
                                                NM_DBUS_PATH,
                                                NM_DBUS_INTERFACE,
                                                dbusConnection);
    if (!d->connectionInterface->isValid()) {
        qWarning() << "Could not find NetworkManager";
        return;
    }
    nmDBusHelper = new QNmDBusHelper;
    connect(nmDBusHelper, SIGNAL(pathForPropertiesChanged(const QString &,QMap<QString,QVariant>)),
                    this,SIGNAL(propertiesChanged( const QString &, QMap<QString,QVariant>)));
    connect(nmDBusHelper,SIGNAL(pathForStateChanged(const QString &, quint32)),
            this, SIGNAL(stateChanged(const QString&, quint32)));

}

QNetworkManagerInterface::~QNetworkManagerInterface()
{
    delete d->connectionInterface;
    delete d;
}

bool QNetworkManagerInterface::setConnections()
{
    bool allOk = false;
    if (!dbusConnection.connect(NM_DBUS_SERVICE,
                                  NM_DBUS_PATH,
                                  NM_DBUS_INTERFACE,
                                  "PropertiesChanged",
                                nmDBusHelper,SLOT(slotPropertiesChanged( QMap<QString,QVariant>)))) {
        allOk = true;
    }
    if (!dbusConnection.connect(NM_DBUS_SERVICE,
                                  NM_DBUS_PATH,
                                  NM_DBUS_INTERFACE,
                                  "DeviceAdded",
                                this,SIGNAL(deviceAdded(QDBusObjectPath)))) {
        allOk = true;
    }
    if (!dbusConnection.connect(NM_DBUS_SERVICE,
                                  NM_DBUS_PATH,
                                  NM_DBUS_INTERFACE,
                                  "DeviceRemoved",
                                  this,SIGNAL(deviceRemoved(QDBusObjectPath)))) {
        allOk = true;
    }

    return allOk;
}

QDBusInterface *QNetworkManagerInterface::connectionInterface()  const
{
    return d->connectionInterface;
}

QList <QDBusObjectPath> QNetworkManagerInterface::getDevices() const
{
    QDBusReply<QList<QDBusObjectPath> > reply =  d->connectionInterface->call("GetDevices");
    return reply.value();
}

void QNetworkManagerInterface::activateConnection( const QString &serviceName,
                                                  QDBusObjectPath connectionPath,
                                                  QDBusObjectPath devicePath,
                                                  QDBusObjectPath /*specificObject*/)
{
    QDBusPendingCall pendingCall = d->connectionInterface->asyncCall("ActivateConnection",
                                                                    QVariant(serviceName),
                                                                    QVariant::fromValue(connectionPath),
                                                                    QVariant::fromValue(devicePath),
                                                                    QVariant::fromValue(connectionPath));

   QDBusPendingCallWatcher *callWatcher = new QDBusPendingCallWatcher(pendingCall, this);
   connect(callWatcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                    this, SIGNAL(activationFinished(QDBusPendingCallWatcher*)));
}

void QNetworkManagerInterface::deactivateConnection(QDBusObjectPath connectionPath)  const
{
    d->connectionInterface->call("DeactivateConnection", QVariant::fromValue(connectionPath));
}

bool QNetworkManagerInterface::wirelessEnabled()  const
{
    return d->connectionInterface->property("WirelessEnabled").toBool();
}

bool QNetworkManagerInterface::wirelessHardwareEnabled()  const
{
    return d->connectionInterface->property("WirelessHardwareEnabled").toBool();
}

QList <QDBusObjectPath> QNetworkManagerInterface::activeConnections() const
{
    QVariant prop = d->connectionInterface->property("ActiveConnections");
    return prop.value<QList<QDBusObjectPath> >();
}

quint32 QNetworkManagerInterface::state()
{
    return d->connectionInterface->property("State").toUInt();
}

/////////////
class QNetworkManagerInterfaceAccessPointPrivate
{
public:
    QDBusInterface *connectionInterface;
    QString path;
};

QNetworkManagerInterfaceAccessPoint::QNetworkManagerInterfaceAccessPoint(const QString &dbusPathName, QObject *parent)
        : QObject(parent)
{
    d = new QNetworkManagerInterfaceAccessPointPrivate();
    d->path = dbusPathName;
    d->connectionInterface = new QDBusInterface(NM_DBUS_SERVICE,
                                                d->path,
                                                NM_DBUS_INTERFACE_ACCESS_POINT,
                                                dbusConnection);
    if (!d->connectionInterface->isValid()) {
        qWarning() << "Could not find InterfaceAccessPoint";
        return;
    }

}

QNetworkManagerInterfaceAccessPoint::~QNetworkManagerInterfaceAccessPoint()
{
    delete d->connectionInterface;
    delete d;
}

bool QNetworkManagerInterfaceAccessPoint::setConnections()
{
    bool allOk = false;
    nmDBusHelper = new QNmDBusHelper;
    connect(nmDBusHelper, SIGNAL(pathForPropertiesChanged(const QString &,QMap<QString,QVariant>)),
            this,SIGNAL(propertiesChanged( const QString &, QMap<QString,QVariant>)));

    if(dbusConnection.connect(NM_DBUS_SERVICE,
                              d->path,
                              NM_DBUS_INTERFACE_ACCESS_POINT,
                              "PropertiesChanged",
                              nmDBusHelper,SLOT(slotPropertiesChanged( QMap<QString,QVariant>))) ) {
        allOk = true;

    }
    return allOk;
}

QDBusInterface *QNetworkManagerInterfaceAccessPoint::connectionInterface() const
{
    return d->connectionInterface;
}

quint32 QNetworkManagerInterfaceAccessPoint::flags() const
{
    return d->connectionInterface->property("Flags").toUInt();
}

quint32 QNetworkManagerInterfaceAccessPoint::wpaFlags() const
{
    return d->connectionInterface->property("WpaFlags").toUInt();
}

quint32 QNetworkManagerInterfaceAccessPoint::rsnFlags() const
{
    return d->connectionInterface->property("RsnFlags").toUInt();
}

QString QNetworkManagerInterfaceAccessPoint::ssid() const
{
    return d->connectionInterface->property("Ssid").toString();
}

quint32 QNetworkManagerInterfaceAccessPoint::frequency() const
{
    return d->connectionInterface->property("Frequency").toUInt();
}

QString QNetworkManagerInterfaceAccessPoint::hwAddress() const
{
    return d->connectionInterface->property("HwAddress").toString();
}

quint32 QNetworkManagerInterfaceAccessPoint::mode() const
{
    return d->connectionInterface->property("Mode").toUInt();
}

quint32 QNetworkManagerInterfaceAccessPoint::maxBitrate() const
{
    return d->connectionInterface->property("MaxBitrate").toUInt();
}

quint32 QNetworkManagerInterfaceAccessPoint::strength() const
{
    return d->connectionInterface->property("Strength").toUInt();
}

/////////////
class QNetworkManagerInterfaceDevicePrivate
{
public:
    QDBusInterface *connectionInterface;
    QString path;
};

QNetworkManagerInterfaceDevice::QNetworkManagerInterfaceDevice(const QString &deviceObjectPath, QObject *parent)
        : QObject(parent)
{
    d = new QNetworkManagerInterfaceDevicePrivate();
    d->path = deviceObjectPath;
    d->connectionInterface = new QDBusInterface(NM_DBUS_SERVICE,
                                                d->path,
                                                NM_DBUS_INTERFACE_DEVICE,
                                                dbusConnection);
    if (!d->connectionInterface->isValid()) {
        qWarning() << "Could not find NetworkManager";
        return;
    }
}

QNetworkManagerInterfaceDevice::~QNetworkManagerInterfaceDevice()
{
    delete d->connectionInterface;
    delete d;
}

bool QNetworkManagerInterfaceDevice::setConnections()
{
    bool allOk = false;
    nmDBusHelper = new QNmDBusHelper;
    connect(nmDBusHelper,SIGNAL(pathForStateChanged(const QString &, quint32)),
            this, SIGNAL(stateChanged(const QString&, quint32)));
    if(dbusConnection.connect(NM_DBUS_SERVICE,
                              d->path,
                              NM_DBUS_INTERFACE_DEVICE,
                              "StateChanged",
                              nmDBusHelper,SLOT(deviceStateChanged(quint32)))) {
        allOk = true;
    }
    return allOk;
}

QDBusInterface *QNetworkManagerInterfaceDevice::connectionInterface() const
{
    return d->connectionInterface;
}

QString QNetworkManagerInterfaceDevice::udi() const
{
    return d->connectionInterface->property("Udi").toString();
}

QNetworkInterface QNetworkManagerInterfaceDevice::interface() const
{
    return QNetworkInterface::interfaceFromName(d->connectionInterface->property("Interface").toString());
}

quint32 QNetworkManagerInterfaceDevice::ip4Address() const
{
    return d->connectionInterface->property("Ip4Address").toUInt();
}

quint32 QNetworkManagerInterfaceDevice::state() const
{
    return d->connectionInterface->property("State").toUInt();
}

quint32 QNetworkManagerInterfaceDevice::deviceType() const
{
    return d->connectionInterface->property("DeviceType").toUInt();
}

QDBusObjectPath QNetworkManagerInterfaceDevice::ip4config() const
{
    QVariant prop = d->connectionInterface->property("Ip4Config");
    return prop.value<QDBusObjectPath>();
}

/////////////
class QNetworkManagerInterfaceDeviceWiredPrivate
{
public:
    QDBusInterface *connectionInterface;
    QString path;
};

QNetworkManagerInterfaceDeviceWired::QNetworkManagerInterfaceDeviceWired(const QString &ifaceDevicePath, QObject *parent)
{
    d = new QNetworkManagerInterfaceDeviceWiredPrivate();
    d->path = ifaceDevicePath;
    d->connectionInterface = new QDBusInterface(NM_DBUS_SERVICE,
                                                d->path,
                                                NM_DBUS_INTERFACE_DEVICE_WIRED,
                                                dbusConnection, parent);
    if (!d->connectionInterface->isValid()) {
        qWarning() << "Could not find InterfaceDeviceWired";
        return;
    }
}

QNetworkManagerInterfaceDeviceWired::~QNetworkManagerInterfaceDeviceWired()
{
    delete d->connectionInterface;
    delete d;
}

bool QNetworkManagerInterfaceDeviceWired::setConnections()
{
    bool allOk = false;
    nmDBusHelper = new QNmDBusHelper;
    connect(nmDBusHelper, SIGNAL(pathForPropertiesChanged(const QString &,QMap<QString,QVariant>)),
            this,SIGNAL(propertiesChanged( const QString &, QMap<QString,QVariant>)));
    if(dbusConnection.connect(NM_DBUS_SERVICE,
                              d->path,
                              NM_DBUS_INTERFACE_DEVICE_WIRED,
                              "PropertiesChanged",
                              nmDBusHelper,SLOT(slotPropertiesChanged( QMap<QString,QVariant>))) )  {
        allOk = true;
    }
    return allOk;
}

QDBusInterface *QNetworkManagerInterfaceDeviceWired::connectionInterface() const
{
    return d->connectionInterface;
}

QString QNetworkManagerInterfaceDeviceWired::hwAddress() const
{
    return d->connectionInterface->property("HwAddress").toString();
}

quint32 QNetworkManagerInterfaceDeviceWired::speed() const
{
    return d->connectionInterface->property("Speed").toUInt();
}

bool QNetworkManagerInterfaceDeviceWired::carrier() const
{
    return d->connectionInterface->property("Carrier").toBool();
}

/////////////
class QNetworkManagerInterfaceDeviceWirelessPrivate
{
public:
    QDBusInterface *connectionInterface;
    QString path;
};

QNetworkManagerInterfaceDeviceWireless::QNetworkManagerInterfaceDeviceWireless(const QString &ifaceDevicePath, QObject *parent)
{
    d = new QNetworkManagerInterfaceDeviceWirelessPrivate();
    d->path = ifaceDevicePath;
    d->connectionInterface = new QDBusInterface(NM_DBUS_SERVICE,
                                                d->path,
                                                NM_DBUS_INTERFACE_DEVICE_WIRELESS,
                                                dbusConnection, parent);
    if (!d->connectionInterface->isValid()) {
        qWarning() << "Could not find InterfaceDeviceWireless";
        return;
    }
}

QNetworkManagerInterfaceDeviceWireless::~QNetworkManagerInterfaceDeviceWireless()
{
    delete d->connectionInterface;
    delete d;
}

bool QNetworkManagerInterfaceDeviceWireless::setConnections()
{
    bool allOk = false;
    nmDBusHelper = new QNmDBusHelper;
    connect(nmDBusHelper, SIGNAL(pathForPropertiesChanged(const QString &,QMap<QString,QVariant>)),
            this,SIGNAL(propertiesChanged( const QString &, QMap<QString,QVariant>)));

    connect(nmDBusHelper, SIGNAL(pathForAccessPointAdded(const QString &,QDBusObjectPath)),
            this,SIGNAL(accessPointAdded(const QString &,QDBusObjectPath)));

    connect(nmDBusHelper, SIGNAL(pathForAccessPointRemoved(const QString &,QDBusObjectPath)),
            this,SIGNAL(accessPointRemoved(const QString &,QDBusObjectPath)));

    if(!dbusConnection.connect(NM_DBUS_SERVICE,
                              d->path,
                              NM_DBUS_INTERFACE_DEVICE_WIRELESS,
                              "AccessPointAdded",
                              nmDBusHelper, SLOT(slotAccessPointAdded( QDBusObjectPath )))) {
        allOk = true;
    }


    if(!dbusConnection.connect(NM_DBUS_SERVICE,
                              d->path,
                              NM_DBUS_INTERFACE_DEVICE_WIRELESS,
                              "AccessPointRemoved",
                              nmDBusHelper, SLOT(slotAccessPointRemoved( QDBusObjectPath )))) {
        allOk = true;
    }


    if(!dbusConnection.connect(NM_DBUS_SERVICE,
                              d->path,
                              NM_DBUS_INTERFACE_DEVICE_WIRELESS,
                              "PropertiesChanged",
                              nmDBusHelper,SLOT(slotPropertiesChanged( QMap<QString,QVariant>)))) {
        allOk = true;
    }

    return allOk;
}

QDBusInterface *QNetworkManagerInterfaceDeviceWireless::connectionInterface() const
{
    return d->connectionInterface;
}

QList <QDBusObjectPath> QNetworkManagerInterfaceDeviceWireless::getAccessPoints()
{
    QDBusReply<QList<QDBusObjectPath> > reply = d->connectionInterface->call("GetAccessPoints");
    return reply.value();
}

QString QNetworkManagerInterfaceDeviceWireless::hwAddress() const
{
    return d->connectionInterface->property("HwAddress").toString();
}

quint32 QNetworkManagerInterfaceDeviceWireless::mode() const
{
    return d->connectionInterface->property("Mode").toUInt();
}

quint32 QNetworkManagerInterfaceDeviceWireless::bitrate() const
{
    return d->connectionInterface->property("Bitrate").toUInt();
}

QDBusObjectPath QNetworkManagerInterfaceDeviceWireless::activeAccessPoint() const
{
    return d->connectionInterface->property("ActiveAccessPoint").value<QDBusObjectPath>();
}

quint32 QNetworkManagerInterfaceDeviceWireless::wirelessCapabilities() const
{
    return d->connectionInterface->property("WirelelessCapabilities").toUInt();
}

/////////////
class QNetworkManagerSettingsPrivate
{
public:
    QDBusInterface *connectionInterface;
    QString path;
};

QNetworkManagerSettings::QNetworkManagerSettings(const QString &settingsService, QObject *parent)
        : QObject(parent)
{
//    qWarning() << __PRETTY_FUNCTION__;
    d = new QNetworkManagerSettingsPrivate();
    d->path = settingsService;
    d->connectionInterface = new QDBusInterface(settingsService,
                                                NM_DBUS_PATH_SETTINGS,
                                                NM_DBUS_IFACE_SETTINGS,
                                                dbusConnection);
    if (!d->connectionInterface->isValid()) {
        qWarning() << "Could not find NetworkManagerSettings";
        return;
    }
}

QNetworkManagerSettings::~QNetworkManagerSettings()
{
    delete d->connectionInterface;
    delete d;
}

bool QNetworkManagerSettings::setConnections()
{
    bool allOk = false;

    if (!dbusConnection.connect(d->path, NM_DBUS_PATH_SETTINGS,
                           NM_DBUS_IFACE_SETTINGS, "NewConnection",
                           this, SIGNAL(newConnection(QDBusObjectPath)))) {
        allOk = true;
    }

    return allOk;
}

QList <QDBusObjectPath> QNetworkManagerSettings::listConnections()
{
    QDBusReply<QList<QDBusObjectPath> > reply = d->connectionInterface->call("ListConnections");
    return  reply.value();
}

QDBusInterface *QNetworkManagerSettings::connectionInterface() const
{
    return d->connectionInterface;
}


/////////////
class QNetworkManagerSettingsConnectionPrivate
{
public:
    QDBusInterface *connectionInterface;
    QString path;
    QString service;
    QNmSettingsMap settingsMap;
};

QNetworkManagerSettingsConnection::QNetworkManagerSettingsConnection(const QString &settingsService, const QString &connectionObjectPath, QObject *parent)
{
    qDBusRegisterMetaType<QNmSettingsMap>();
    d = new QNetworkManagerSettingsConnectionPrivate();
    d->path = connectionObjectPath;
    d->service = settingsService;
    d->connectionInterface = new QDBusInterface(settingsService,
                                                d->path,
                                                NM_DBUS_IFACE_SETTINGS_CONNECTION,
                                                dbusConnection, parent);
    if (!d->connectionInterface->isValid()) {
        qWarning() << "Could not find NetworkManagerSettingsConnection";
        return;
    }
    QDBusReply< QNmSettingsMap > rep = d->connectionInterface->call("GetSettings");
    d->settingsMap = rep.value();
}

QNetworkManagerSettingsConnection::~QNetworkManagerSettingsConnection()
{
    delete d->connectionInterface;
    delete d;
}

bool QNetworkManagerSettingsConnection::setConnections()
{
    bool allOk = false;
    if(!dbusConnection.connect(d->service, d->path,
                           NM_DBUS_IFACE_SETTINGS_CONNECTION, "NewConnection",
                           this, SIGNAL(updated(QNmSettingsMap)))) {
        allOk = true;
    }

    if (!dbusConnection.connect(d->service, d->path,
                           NM_DBUS_IFACE_SETTINGS_CONNECTION, "Removed",
                           this, SIGNAL(removed()))) {
        allOk = true;
    }

    return allOk;
}
//QNetworkManagerSettingsConnection::update(QNmSettingsMap map)
//{
//    d->connectionInterface->call("Update", QVariant::fromValue(map));
//}

QDBusInterface *QNetworkManagerSettingsConnection::connectionInterface() const
{
    return d->connectionInterface;
}

QNmSettingsMap QNetworkManagerSettingsConnection::getSettings()
{
    QDBusReply< QNmSettingsMap > rep = d->connectionInterface->call("GetSettings");
    d->settingsMap = rep.value();
    return d->settingsMap;
}

NMDeviceType QNetworkManagerSettingsConnection::getType()
{
    QNmSettingsMap::const_iterator i = d->settingsMap.find("connection");
    while (i != d->settingsMap.end() && i.key() == "connection") {
        QMap<QString,QVariant> innerMap = i.value();
        QMap<QString,QVariant>::const_iterator ii = innerMap.find("type");
        while (ii != innerMap.end() && ii.key() == "type") {
            QString devType = ii.value().toString();
            if (devType == "802-3-ethernet") {
                return 	DEVICE_TYPE_802_3_ETHERNET;
            }
            if (devType == "802-11-wireless") {
                return 	DEVICE_TYPE_802_11_WIRELESS;
            }
            ii++;
        }
        i++;
    }
    return 	DEVICE_TYPE_UNKNOWN;
}

bool QNetworkManagerSettingsConnection::isAutoConnect()
{
    QNmSettingsMap::const_iterator i = d->settingsMap.find("connection");
    while (i != d->settingsMap.end() && i.key() == "connection") {
        QMap<QString,QVariant> innerMap = i.value();
        QMap<QString,QVariant>::const_iterator ii = innerMap.find("autoconnect");
        while (ii != innerMap.end() && ii.key() == "autoconnect") {
            return ii.value().toBool();
            ii++;
        }
        i++;
    }
    return false;
}

quint64 QNetworkManagerSettingsConnection::getTimestamp()
{
    QNmSettingsMap::const_iterator i = d->settingsMap.find("connection");
    while (i != d->settingsMap.end() && i.key() == "connection") {
        QMap<QString,QVariant> innerMap = i.value();
        QMap<QString,QVariant>::const_iterator ii = innerMap.find("timestamp");
        while (ii != innerMap.end() && ii.key() == "timestamp") {
            return ii.value().toUInt();
            ii++;
        }
        i++;
    }
    return 	0;
}

QString QNetworkManagerSettingsConnection::getId()
{
    QNmSettingsMap::const_iterator i = d->settingsMap.find("connection");
    while (i != d->settingsMap.end() && i.key() == "connection") {
        QMap<QString,QVariant> innerMap = i.value();
        QMap<QString,QVariant>::const_iterator ii = innerMap.find("id");
        while (ii != innerMap.end() && ii.key() == "id") {
            return ii.value().toString();
            ii++;
        }
        i++;
    }
    return 	QString();
}

QString QNetworkManagerSettingsConnection::getUuid()
{
    QNmSettingsMap::const_iterator i = d->settingsMap.find("connection");
    while (i != d->settingsMap.end() && i.key() == "connection") {
        QMap<QString,QVariant> innerMap = i.value();
        QMap<QString,QVariant>::const_iterator ii = innerMap.find("uuid");
        while (ii != innerMap.end() && ii.key() == "uuid") {
            return ii.value().toString();
            ii++;
        }
        i++;
    }
    return 	QString();
}

QString QNetworkManagerSettingsConnection::getSsid()
{
    QNmSettingsMap::const_iterator i = d->settingsMap.find("802-11-wireless");
    while (i != d->settingsMap.end() && i.key() == "802-11-wireless") {
        QMap<QString,QVariant> innerMap = i.value();
        QMap<QString,QVariant>::const_iterator ii = innerMap.find("ssid");
        while (ii != innerMap.end() && ii.key() == "ssid") {
            return ii.value().toString();
            ii++;
        }
        i++;
    }
    return 	QString();
}

QString QNetworkManagerSettingsConnection::getMacAddress()
{
    if(getType() == DEVICE_TYPE_802_3_ETHERNET) {
        QNmSettingsMap::const_iterator i = d->settingsMap.find("802-3-ethernet");
        while (i != d->settingsMap.end() && i.key() == "802-3-ethernet") {
            QMap<QString,QVariant> innerMap = i.value();
            QMap<QString,QVariant>::const_iterator ii = innerMap.find("mac-address");
            while (ii != innerMap.end() && ii.key() == "mac-address") {
                return ii.value().toString();
                ii++;
            }
            i++;
        }
    }

    else if(getType() == DEVICE_TYPE_802_11_WIRELESS) {
        QNmSettingsMap::const_iterator i = d->settingsMap.find("802-11-wireless");
        while (i != d->settingsMap.end() && i.key() == "802-11-wireless") {
            QMap<QString,QVariant> innerMap = i.value();
            QMap<QString,QVariant>::const_iterator ii = innerMap.find("mac-address");
            while (ii != innerMap.end() && ii.key() == "mac-address") {
                return ii.value().toString();
                ii++;
            }
            i++;
        }
    }
    return 	QString();
}

/////////////
class QNetworkManagerConnectionActivePrivate
{
public:
    QDBusInterface *connectionInterface;
    QString path;
};

QNetworkManagerConnectionActive::QNetworkManagerConnectionActive( const QString &activeConnectionObjectPath, QObject *parent)
{
    d = new QNetworkManagerConnectionActivePrivate();
    d->path = activeConnectionObjectPath;
    d->connectionInterface = new QDBusInterface(NM_DBUS_SERVICE,
                                                d->path,
                                                NM_DBUS_INTERFACE_ACTIVE_CONNECTION,
                                                dbusConnection, parent);
    if (!d->connectionInterface->isValid()) {
        qWarning() << "Could not find NetworkManagerSettingsConnection";
        return;
    }
}

QNetworkManagerConnectionActive::~QNetworkManagerConnectionActive()
{
    delete d->connectionInterface;
    delete d;
}

bool QNetworkManagerConnectionActive::setConnections()
{
    bool allOk = false;
    nmDBusHelper = new QNmDBusHelper;
    connect(nmDBusHelper, SIGNAL(pathForPropertiesChanged(const QString &,QMap<QString,QVariant>)),
            this,SIGNAL(propertiesChanged( const QString &, QMap<QString,QVariant>)));
    if(dbusConnection.connect(NM_DBUS_SERVICE,
                              d->path,
                              NM_DBUS_INTERFACE_ACTIVE_CONNECTION,
                              "PropertiesChanged",
                              nmDBusHelper,SLOT(slotPropertiesChanged( QMap<QString,QVariant>))) )  {
        allOk = true;
    }

    return allOk;
}

QDBusInterface *QNetworkManagerConnectionActive::connectionInterface() const
{
    return d->connectionInterface;
}

QString QNetworkManagerConnectionActive::serviceName() const
{
    return d->connectionInterface->property("ServiceName").toString();
}

QDBusObjectPath QNetworkManagerConnectionActive::connection() const
{
    QVariant prop = d->connectionInterface->property("Connection");
    return prop.value<QDBusObjectPath>();
}

QDBusObjectPath QNetworkManagerConnectionActive::specificObject() const
{
    QVariant prop = d->connectionInterface->property("SpecificObject");
    return prop.value<QDBusObjectPath>();
}

QList<QDBusObjectPath> QNetworkManagerConnectionActive::devices() const
{
    QVariant prop = d->connectionInterface->property("Devices");
    return prop.value<QList<QDBusObjectPath> >();
}

quint32 QNetworkManagerConnectionActive::state() const
{
    return d->connectionInterface->property("State").toUInt();
}

bool QNetworkManagerConnectionActive::defaultRoute() const
{
    return d->connectionInterface->property("Default").toBool();
}


////
class QNetworkManagerIp4ConfigPrivate
{
public:
    QDBusInterface *connectionInterface;
    QString path;
};

QNetworkManagerIp4Config::QNetworkManagerIp4Config( const QString &deviceObjectPath, QObject *parent)
{
    d = new QNetworkManagerIp4ConfigPrivate();
    d->path = deviceObjectPath;
    d->connectionInterface = new QDBusInterface(NM_DBUS_SERVICE,
                                                d->path,
                                                NM_DBUS_INTERFACE_IP4_CONFIG,
                                                dbusConnection, parent);
    if (!d->connectionInterface->isValid()) {
        qWarning() << "Could not find NetworkManagerIp4Config";
        return;
    }
}

QNetworkManagerIp4Config::~QNetworkManagerIp4Config()
{
    delete d->connectionInterface;
    delete d;
}

QStringList QNetworkManagerIp4Config::domains() const
{
    return d->connectionInterface->property("Domains").toStringList();
}
