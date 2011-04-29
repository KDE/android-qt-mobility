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

#ifndef QSYSTEMNETWORKINFO_H
#define QSYSTEMNETWORKINFO_H

#include "qmobilityglobal.h"

#include <QtCore/qobject.h>
#include <QtNetwork/qnetworkinterface.h>

QT_BEGIN_HEADER
QTM_BEGIN_NAMESPACE

class QSystemNetworkInfoPrivate;

class Q_SYSINFO_EXPORT QSystemNetworkInfo : public QObject
{
    Q_OBJECT

    Q_ENUMS(NetworkStatus)
    Q_ENUMS(NetworkMode)
    Q_ENUMS(CellDataTechnology)

    Q_PROPERTY(int cellId READ cellId NOTIFY cellIdChanged)
    Q_PROPERTY(int locationAreaCode READ locationAreaCode)
    Q_PROPERTY(QString currentMobileCountryCode READ currentMobileCountryCode NOTIFY currentMobileCountryCodeChanged)
    Q_PROPERTY(QString currentMobileNetworkCode READ currentMobileNetworkCode NOTIFY currentMobileNetworkCodeChanged)
    Q_PROPERTY(QString homeMobileCountryCode READ homeMobileCountryCode CONSTANT)
    Q_PROPERTY(QString homeMobileNetworkCode READ homeMobileNetworkCode CONSTANT)
    Q_PROPERTY(QSystemNetworkInfo::NetworkMode currentMode READ currentMode CONSTANT)
    Q_PROPERTY(QSystemNetworkInfo::CellDataTechnology cellDataTechnology READ cellDataTechnology NOTIFY cellDataTechnologyChanged)

public:
    explicit QSystemNetworkInfo(QObject *parent = 0);
    ~QSystemNetworkInfo();

    enum NetworkStatus {
        UndefinedStatus = 0,
        NoNetworkAvailable,
        EmergencyOnly,
        Searching,
        Busy,
        Connected,
        HomeNetwork,
        Denied,
        Roaming
    };

    enum NetworkMode {
        UnknownMode = 0,
        GsmMode,
        CdmaMode,
        WcdmaMode, //umts
        WlanMode,
        EthernetMode,
        BluetoothMode,
        WimaxMode,
        LteMode //1.2
    };

    enum CellDataTechnology {
        UnknownDataTechnology = 0,
        GprsDataTechnology,
        EdgeDataTechnology,
        UmtsDataTechnology,
        HspaDataTechnology,
    }; //1.2

    Q_INVOKABLE QSystemNetworkInfo::NetworkStatus networkStatus(QSystemNetworkInfo::NetworkMode mode);
    Q_INVOKABLE static int networkSignalStrength(QSystemNetworkInfo::NetworkMode mode);
    Q_INVOKABLE QString macAddress(QSystemNetworkInfo::NetworkMode mode);
    Q_INVOKABLE QSystemNetworkInfo::NetworkMode currentMode();

    int cellId();
    int locationAreaCode();

    QString currentMobileCountryCode();
    QString currentMobileNetworkCode();
    QString homeMobileCountryCode();
    QString homeMobileNetworkCode();
    Q_INVOKABLE static QString networkName(QSystemNetworkInfo::NetworkMode mode);
    Q_INVOKABLE QNetworkInterface interfaceForMode(QSystemNetworkInfo::NetworkMode mode);
    QSystemNetworkInfo::CellDataTechnology cellDataTechnology();

Q_SIGNALS:
    void networkStatusChanged(QSystemNetworkInfo::NetworkMode mode, QSystemNetworkInfo::NetworkStatus status);
    void networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode mode, int strength);
    void currentMobileCountryCodeChanged(const QString &mcc);
    void currentMobileNetworkCodeChanged(const QString &mnc);
    void networkNameChanged(QSystemNetworkInfo::NetworkMode mode, const QString &name);
    void networkModeChanged(QSystemNetworkInfo::NetworkMode mode);
    void cellIdChanged(int cellId); //1.2
    void cellDataTechnologyChanged(QSystemNetworkInfo::CellDataTechnology cellTech); //1.2

protected:
    virtual void connectNotify(const char *signal);
    virtual void disconnectNotify(const char *signal);

private:
    QSystemNetworkInfoPrivate *d;
};

QTM_END_NAMESPACE
QT_END_HEADER

#endif // QSYSTEMNETWORKINFO_H
