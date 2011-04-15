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

#ifndef QBLUETOOTHSERVICEDISCOVERYAGENT_H
#define QBLUETOOTHSERVICEDISCOVERYAGENT_H

#include <qmobilityglobal.h>

#include <QObject>

#include <qbluetoothserviceinfo.h>
#include <qbluetoothuuid.h>

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QBluetoothAddress;
class QBluetoothServiceDiscoveryAgentPrivate;

class Q_CONNECTIVITY_EXPORT QBluetoothServiceDiscoveryAgent : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QBluetoothServiceDiscoveryAgent)

public:
    enum Error {
        NoError,
        DeviceDiscoveryError,
        UnknownError = 100
    };

    enum DiscoveryMode {
        MinimalDiscovery,
        FullDiscovery
    };

    QBluetoothServiceDiscoveryAgent(QObject *parent = 0);
    explicit QBluetoothServiceDiscoveryAgent(const QBluetoothAddress &remoteAddress, QObject *parent = 0);
    ~QBluetoothServiceDiscoveryAgent();

    bool isActive() const;

    Error error() const;
    QString errorString() const;

    QList<QBluetoothServiceInfo> discoveredServices() const;

    void setUuidFilter(const QList<QBluetoothUuid> &uuids);
    void setUuidFilter(const QBluetoothUuid &uuid);
    QList<QBluetoothUuid> uuidFilter() const;

public slots:
    void start(DiscoveryMode mode = MinimalDiscovery);
    void stop();
    void clear();

signals:
    void serviceDiscovered(const QBluetoothServiceInfo &info);
    void finished();
    void canceled();
    void error(QBluetoothServiceDiscoveryAgent::Error error);

private:
    QBluetoothServiceDiscoveryAgentPrivate *d_ptr;

    Q_PRIVATE_SLOT(d_func(), void _q_deviceDiscovered(const QBluetoothDeviceInfo &info))
    Q_PRIVATE_SLOT(d_func(), void _q_deviceDiscoveryFinished())
    Q_PRIVATE_SLOT(d_func(), void _q_serviceDiscoveryFinished())
#ifndef QT_NO_DBUS
    Q_PRIVATE_SLOT(d_func(), void _q_discoveredServices(QDBusPendingCallWatcher*))
    Q_PRIVATE_SLOT(d_func(), void _q_createdDevice(QDBusPendingCallWatcher*))
#endif
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif
