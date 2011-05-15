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

#ifndef QSYSTEMINFO_LINUX_P_H
#define QSYSTEMINFO_LINUX_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qmobilityglobal.h"
#include "qsysteminfo_linux_common_p.h"

#if !defined(QT_NO_NETWORKMANAGER)
#include "qnetworkmanagerservice_linux_p.h"
#endif // QT_NO_NETWORKMANAGER

QTM_BEGIN_NAMESPACE

class QSystemInfoPrivate : public QSystemInfoLinuxCommonPrivate
{
    Q_OBJECT

public:
    QSystemInfoPrivate(QSystemInfoLinuxCommonPrivate *parent = 0);
    virtual ~QSystemInfoPrivate();

    QStringList availableLanguages() const;
};

class QSystemNetworkInfoPrivate : public QSystemNetworkInfoLinuxCommonPrivate
{
    Q_OBJECT

public:
    QSystemNetworkInfoPrivate(QSystemNetworkInfoLinuxCommonPrivate *parent = 0);
    virtual ~QSystemNetworkInfoPrivate();

    int networkSignalStrength(QSystemNetworkInfo::NetworkMode mode);
    QString networkName(QSystemNetworkInfo::NetworkMode mode);
    QSystemNetworkInfo::NetworkMode currentMode();
    QSystemNetworkInfo::NetworkStatus networkStatus(QSystemNetworkInfo::NetworkMode mode);

#if !defined(QT_NO_NETWORKMANAGER)
private Q_SLOTS:
    void nmPropertiesChanged(const QString &path, QMap<QString, QVariant> map);
    void nmAPPropertiesChanged(const QString &path, QMap<QString, QVariant> map);

private:
    QNetworkManagerInterface *iface;
    QNetworkManagerInterfaceDeviceWired *devWiredIface;
    QNetworkManagerInterfaceDeviceWireless *devWirelessIface;
    QNetworkManagerInterfaceAccessPoint *accessPointIface;
    QNetworkManagerInterfaceDeviceGsm *devGsmIface;
    QMap<QString, QString> activePaths;

    void setupNmConnections();
    void updateActivePaths();
    inline QSystemNetworkInfo::NetworkMode deviceTypeToMode(quint32 type);
#endif // QT_NO_NETWORKMANAGER
};

class QSystemDisplayInfoPrivate : public QSystemDisplayInfoLinuxCommonPrivate
{
    Q_OBJECT

public:
    QSystemDisplayInfoPrivate(QSystemDisplayInfoLinuxCommonPrivate *parent = 0);
    virtual ~QSystemDisplayInfoPrivate();
};

class QSystemDeviceInfoPrivate : public QSystemDeviceInfoLinuxCommonPrivate
{
    Q_OBJECT

public:
    QSystemDeviceInfoPrivate(QSystemDeviceInfoLinuxCommonPrivate *parent = 0);
    ~QSystemDeviceInfoPrivate();

    bool isDeviceLocked();
    bool isKeyboardFlippedOpen(); //1.2
    bool keypadLightOn(QSystemDeviceInfo::KeypadType type); //1.2
    bool vibrationActive(); //1.2
    int messageRingtoneVolume(); //1.2
    int voiceRingtoneVolume(); //1.2
    QString imei();
    QString imsi();
    QString model();
    QString productName();

    QSystemDeviceInfo::LockTypeFlags lockStatus(); //1.2
    QSystemDeviceInfo::Profile currentProfile();
    QSystemDeviceInfo::SimStatus simStatus();
    QSystemDeviceInfo::ThermalState currentThermalState();
};

class QSystemScreenSaverPrivate : public QObject
{
    Q_OBJECT

public:
    QSystemScreenSaverPrivate(QObject *parent = 0);
    ~QSystemScreenSaverPrivate();

    bool screenSaverInhibited();
    bool setScreenSaverInhibit();
    bool isScreenLockEnabled();
    bool isScreenSaverActive();
    void setScreenSaverInhibited(bool on);

private:
    bool gnomeIsRunning;
    bool kdeIsRunning;
    bool meegoIsRunning;
    bool screenSaverIsInhibited;
    bool screenSaverSecure;
    uint currentPid;
    QString screenPath;
    QString settingsPath;

    void whichWMRunning();
#if defined(Q_WS_X11)
     int changeTimeout(int timeout);
#endif // Q_WS_X11
};

class QSystemBatteryInfoPrivate : public QSystemBatteryInfoLinuxCommonPrivate
{
    Q_OBJECT
public:
    QSystemBatteryInfoPrivate(QSystemBatteryInfoLinuxCommonPrivate *parent = 0);
    ~QSystemBatteryInfoPrivate();
};

QTM_END_NAMESPACE

#endif // QSYSTEMSINFO_LINUX_P_H
