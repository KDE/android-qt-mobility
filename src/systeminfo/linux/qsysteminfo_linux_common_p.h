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
#ifndef QSYSTEMINFO_LINUX_COMMON_P_H
#define QSYSTEMINFO_LINUX_COMMON_P_H


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


#include <QObject>
#include <QSize>
#include <QHash>
#include <QTimer>
#include <QVariantMap>

#include "qsysteminfo.h"
#include "qsystemdeviceinfo.h"
#include "qsystemdisplayinfo.h"
#include "qsystemnetworkinfo.h"
#include "qsystemscreensaver.h"
#include "qsystemstorageinfo.h"
#include "qsystembatteryinfo.h"

#include <qmobilityglobal.h>

#if !defined(QT_NO_DBUS)
#include "qhalservice_linux_p.h"
#include "qdevicekitservice_linux_p.h"

#if !defined(QT_NO_CONNMAN)
#include "qconnmanservice_linux_p.h"
#include "qofonoservice_linux_p.h"
#endif
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE
class QStringList;
class QTimer;
QT_END_NAMESPACE

QTM_BEGIN_NAMESPACE

class QSystemNetworkInfo;
class QSystemInfoLinuxCommonPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemInfoLinuxCommonPrivate(QObject *parent = 0);
    virtual ~QSystemInfoLinuxCommonPrivate();
    QString currentLanguage() const;

    QString version(QSystemInfo::Version,  const QString &/*parameter*/ = QString());
    QString currentCountryCode() const;

    bool fmTransmitterAvailable();
    virtual bool hasFeatureSupported(QSystemInfo::Feature feature);
    bool hasSysFeature(const QString &featureStr);

Q_SIGNALS:
    void currentLanguageChanged(const QString &);

private:
    QTimer *langTimer;
    QString langCached;

protected Q_SLOTS:
    void startLanguagePolling();

protected:
#if !defined(QT_NO_DBUS)
    bool hasHalDeviceFeature(const QString &param);
    bool hasHalUsbFeature(qint32 usbClass);
    QHalInterface halIface;
#endif
};

class QNetworkManagerInterface;
class QNetworkManagerInterfaceDeviceWired;
class QNetworkManagerInterfaceDeviceWireless;
class QNetworkManagerInterfaceAccessPoint;

class QSystemNetworkInfoLinuxCommonPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemNetworkInfoLinuxCommonPrivate(QObject *parent = 0);
    virtual ~QSystemNetworkInfoLinuxCommonPrivate();

    QSystemNetworkInfo::NetworkStatus networkStatus(QSystemNetworkInfo::NetworkMode mode);
    qint32 networkSignalStrength(QSystemNetworkInfo::NetworkMode mode);
    qint32 cellId();
    int locationAreaCode();

    QString currentMobileCountryCode();
    QString currentMobileNetworkCode();

    QString homeMobileCountryCode();
    QString homeMobileNetworkCode();

    QString networkName(QSystemNetworkInfo::NetworkMode mode);
    QString macAddress(QSystemNetworkInfo::NetworkMode mode);

    QNetworkInterface interfaceForMode(QSystemNetworkInfo::NetworkMode mode);
    QSystemNetworkInfo::NetworkMode currentMode();

#if !defined(QT_NO_CONNMAN)
    QSystemNetworkInfo::NetworkStatus getOfonoStatus(QSystemNetworkInfo::NetworkMode mode);
#endif
//public Q_SLOTS:
//    void getPrimaryMode();

Q_SIGNALS:
   void networkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus);
   void networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int);
   void currentMobileCountryCodeChanged(const QString &);
   void currentMobileNetworkCodeChanged(const QString &);
   void networkNameChanged(QSystemNetworkInfo::NetworkMode, const QString &);
   void networkModeChanged(QSystemNetworkInfo::NetworkMode);

   void cellIdChanged(int); //1.2

protected:
#if !defined(QT_NO_DBUS)
    int getBluetoothRssi();
    QString getBluetoothInfo(const QString &file);
    bool isDefaultInterface(const QString &device);

#if !defined(QT_NO_CONNMAN)
    QConnmanManagerInterface *connmanManager;
    QOfonoManagerInterface *ofonoManager;
    void initConnman();
    void initOfono();
    void initModem(const QString &path);
    QString modeToTechnology(QSystemNetworkInfo::NetworkMode mode);
    QSystemNetworkInfo::NetworkStatus stateToStatus(const QString &state);
    QSystemNetworkInfo::NetworkMode typeToMode(const QString &type);
    QSystemNetworkInfo::NetworkMode ofonoTechToMode(const QString &tech);
    QSystemNetworkInfo::NetworkStatus ofonoStatusToStatus(const QString &state);

    QStringList knownModems;
#endif

#endif

private Q_SLOTS:
#if !defined(QT_NO_CONNMAN)
    void connmanPropertyChangedContext(const QString &path,const QString &item, const QDBusVariant &value);
    void connmanTechnologyPropertyChangedContext(const QString &path,const QString &item, const QDBusVariant &value);
    void connmanDevicePropertyChangedContext(const QString &path,const QString &item, const QDBusVariant &value);
    void connmanServicePropertyChangedContext(const QString &path,const QString &item, const QDBusVariant &value);

    void ofonoPropertyChangedContext(const QString &path,const QString &item, const QDBusVariant &value);
    void ofonoNetworkPropertyChangedContext(const QString &path,const QString &item, const QDBusVariant &value);
    void ofonoModemPropertyChangedContext(const QString &path,const QString &item, const QDBusVariant &value);
#endif

    QSystemNetworkInfo::NetworkStatus getBluetoothNetStatus();

    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);
};

class QSystemDisplayInfoLinuxCommonPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemDisplayInfoLinuxCommonPrivate(QObject *parent = 0);
    virtual ~QSystemDisplayInfoLinuxCommonPrivate();

    int displayBrightness(int screen);
    int colorDepth(int screen);

    QSystemDisplayInfo::DisplayOrientation orientation(int screen);
    float contrast(int /*screen*/) {return 0.0;};
    int getDPIWidth(int screen);
    int getDPIHeight(int screen);
    int physicalHeight(int screen);
    int physicalWidth(int screen);
    QSystemDisplayInfo::BacklightState backlightStatus(int screen); //1.2
Q_SIGNALS:
    void orientationChanged(QSystemDisplayInfo::DisplayOrientation newOrientation);

};

class QSystemStorageInfoLinuxCommonPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemStorageInfoLinuxCommonPrivate(QObject *parent = 0);
    virtual ~QSystemStorageInfoLinuxCommonPrivate();

    qint64 availableDiskSpace(const QString &driveVolume);
    qint64 totalDiskSpace(const QString &driveVolume);
    QStringList logicalDrives();
    QSystemStorageInfo::DriveType typeForDrive(const QString &driveVolume);

    QString uriForDrive(const QString &driveVolume);//1.2
    QSystemStorageInfo::StorageState getStorageState(const QString &volume);//1.2

Q_SIGNALS:
    void logicalDriveChanged(bool, const QString &);
    void storageStateChanged(const QString &vol, QSystemStorageInfo::StorageState state); //1.2

private:
    bool storageChanged;
     QMap<QString, QString> mountEntriesMap;
     QMap<QString, QSystemStorageInfo::StorageState> stateMap;
     void mountEntries();
     int mtabWatchA;
     int inotifyFD;
     void checkAvailableStorage();
     QString getUuid(const QString &vol);

     QTimer *storageTimer;

#if !defined(QT_NO_DBUS)
    QHalInterface *halIface;
    QHalDeviceInterface *halIfaceDevice;

    QUDisksInterface *udisksIface;
    QUDisksDeviceInterface *udisksDeviceIface;

private Q_SLOTS:
    void udisksDeviceChanged(const QDBusObjectPath &);
#endif

private Q_SLOTS:
    void deviceChanged();
    void inotifyActivated();
    void checkFilesystem();

protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);

};

class QSystemDeviceInfoLinuxCommonPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemDeviceInfoLinuxCommonPrivate(QObject *parent = 0);
    virtual ~QSystemDeviceInfoLinuxCommonPrivate();

    QString imei();
    QString imsi();
    QString manufacturer();
    QString model();
    QString productName();

    QSystemDeviceInfo::InputMethodFlags inputMethodType();

    int  batteryLevel() const ;

    QSystemDeviceInfo::SimStatus simStatus();
    bool isDeviceLocked() {return false;}
    QSystemDeviceInfo::Profile currentProfile() {return QSystemDeviceInfo::UnknownProfile;}

    QSystemDeviceInfo::PowerState currentPowerState();
    void setConnection();
    bool currentBluetoothPowerState();

    QSystemDeviceInfo::KeyboardTypeFlags keyboardTypes(); //1.2
    bool isWirelessKeyboardConnected(); //1.2
    bool isKeyboardFlippedOpen();//1.2

    void keyboardConnected(bool connect);//1.2
    bool keypadLightOn(QSystemDeviceInfo::KeypadType type); //1.2
    QUuid uniqueDeviceID(); //1.2
    QSystemDeviceInfo::LockTypeFlags lockStatus(); //1.2

Q_SIGNALS:
    void batteryLevelChanged(int);
    void batteryStatusChanged(QSystemDeviceInfo::BatteryStatus );

    void powerStateChanged(QSystemDeviceInfo::PowerState);
    void currentProfileChanged(QSystemDeviceInfo::Profile);
    void bluetoothStateChanged(bool);

    void wirelessKeyboardConnected(bool connected);//1.2
    void keyboardFlipped(bool open);//1.2
    void deviceLocked(bool isLocked); // 1.2
    void lockStatusChanged(QSystemDeviceInfo::LockTypeFlags); //1.2

protected:
    bool btPowered;

#if !defined(QT_NO_DBUS)

    QHalInterface *halIface;
    QHalDeviceInterface *halIfaceDevice;
    QUDisksInterface *udisksIface;
    bool hasWirelessKeyboardConnected;
    bool connectedBtPower;
    bool connectedWirelessKeyboard;
    void connectBtPowered(const QString &str);
    void connectBtKeyboard(const QString &str);

    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);

private Q_SLOTS:
    virtual void halChanged(int,QVariantList);
    void bluezPropertyChanged(const QString&, QDBusVariant);
    virtual void upowerChanged();
    virtual void upowerDeviceChanged();
#endif

private:
    QSystemDeviceInfo::BatteryStatus currentBatStatus;
    void initBatteryStatus();
    int currentBatLevel;
    QSystemDeviceInfo::PowerState curPowerState;
};


class QSystemScreenSaverLinuxCommonPrivate : public QObject
{
    Q_OBJECT

public:
    QSystemScreenSaverLinuxCommonPrivate(QObject *parent = 0);
    virtual ~QSystemScreenSaverLinuxCommonPrivate();

//    bool screenSaverInhibited() {return false;}
//    bool setScreenSaverInhibit() {return false;}
//    bool isScreenLockEnabled() {return false;}
//    bool isScreenSaverActive() {return false;}
};


class QSystemBatteryInfoLinuxCommonPrivate : public QObject
{
    Q_OBJECT
public:
    explicit QSystemBatteryInfoLinuxCommonPrivate(QObject *parent = 0);
    ~QSystemBatteryInfoLinuxCommonPrivate();


    QSystemBatteryInfo::ChargerType chargerType() const;
    QSystemBatteryInfo::ChargingState chargingState() const;

    int nominalCapacity() const;
    int remainingCapacityPercent() const;
    int remainingCapacity() const;

    int voltage() const;
    int remainingChargingTime() const;
    int currentFlow() const;
    int remainingCapacityBars() const;
    int maxBars() const;
    QSystemBatteryInfo::BatteryStatus batteryStatus() const;
    QSystemBatteryInfo::EnergyUnit energyMeasurementUnit() const;
    bool batteryIsPresent;
    QSystemBatteryInfo::ChargerType curChargeType;

Q_SIGNALS:
    void batteryStatusChanged(QSystemBatteryInfo::BatteryStatus batteryStatus);


    void chargingStateChanged(QSystemBatteryInfo::ChargingState chargingState);
    void chargerTypeChanged(QSystemBatteryInfo::ChargerType chargerType);

    void nominalCapacityChanged(int);
    void remainingCapacityPercentChanged(int);
    void remainingCapacityChanged(int);
    void batteryCurrentFlowChanged(int);

    void currentFlowChanged(int);
    void cumulativeCurrentFlowChanged(int);
    void remainingCapacityBarsChanged(int);
    void remainingChargingTimeChanged(int);

protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);

#if !defined(QT_NO_DBUS)
    QHalInterface *halIface;
    QHalDeviceInterface *halIfaceDevice;
    QUDisksInterface *udisksIface;

    QSystemBatteryInfo::ChargerType currentChargerType();

private Q_SLOTS:
    void setConnection();
    virtual void halChanged(int,QVariantList);
    void getBatteryStats();
    void timeout();
#if !defined(Q_WS_MAEMO_6) && !defined(Q_WS_MAEMO_5)
    void uPowerPropertyChanged(const QString &, const QVariant &);
#endif
#endif
private:

    QSystemBatteryInfo::BatteryStatus currentBatStatus;
    QSystemBatteryInfo::ChargingState curChargeState;
    QVariantMap pMap;

    int currentBatLevelPercent;
    int currentVoltage;
    int dischargeRate;
    int capacity;
    int timeToFull;
    int remainingEnergy;
    int  batteryLevel() const ;
    QUPowerDeviceInterface *battery;



};

QTM_END_NAMESPACE

QT_END_HEADER

#endif /*QSYSTEMINFO_LINUX_COMMON_P_H*/

// End of file

