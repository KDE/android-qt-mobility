/*
Copyright (c) 2011 Elektrobit (EB), All rights reserved.
Contact: oss-devel@elektrobit.com

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are
met:
* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* Neither the name of the Elektrobit (EB) nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY Elektrobit (EB) ''AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Elektrobit (EB) BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef QSYSTEMINFO_ANDROID_H
#define QSYSTEMINFO_ANDROID_H


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
#include <QStringList>

#include "qmobilityglobal.h"
#include "qsysteminfo.h"
#include "jni_android_p.h"
QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QSystemInfoPrivate : public QObject,public GeneralInfoUpdateListner
{
    Q_OBJECT

public:
    QSystemInfoPrivate(QObject *parent = 0);
    virtual ~QSystemInfoPrivate();

    QString currentLanguage() const;
    QStringList availableLanguages() const;
    QString currentCountryCode() const;
    QString version(QSystemInfo::Version type, const QString &parameter = QString());
    bool hasFeatureSupported(QSystemInfo::Feature feature);
    virtual void onLanguageChanged(QString &language);
Q_SIGNALS:
    void currentLanguageChanged(const QString &);

};

//////// QSystemNetworkInfo
class QSystemNetworkInfoPrivate : public QObject,public NetworkInfoUpdateListener
{
    Q_OBJECT

public:

    QSystemNetworkInfoPrivate(QObject *parent = 0);
    virtual ~QSystemNetworkInfoPrivate();

    static QSystemNetworkInfo::NetworkStatus networkStatus(QSystemNetworkInfo::NetworkMode mode);
    static int networkSignalStrength(QSystemNetworkInfo::NetworkMode mode);
    static int cellId();
    static int locationAreaCode();

    static QString currentMobileCountryCode();
    static QString currentMobileNetworkCode();

    static QString homeMobileCountryCode();
    static QString homeMobileNetworkCode();

    static QString networkName(QSystemNetworkInfo::NetworkMode mode); //signal
    QString macAddress(QSystemNetworkInfo::NetworkMode mode);

    static QNetworkInterface interfaceForMode(QSystemNetworkInfo::NetworkMode mode);
    QSystemNetworkInfo::NetworkMode currentMode();

    virtual void onPhoneStrengthChanged(int strength);
    virtual void onWifiStrengthChanged(int strength);
    virtual void onNetworkStatusChanged(int status, int mode);
    virtual void onNetworkNameChanged(int mode, QString &name);
    virtual void onCellIdChanged(int cellID);

Q_SIGNALS:
    void networkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus);
    void networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode, int);
    void currentMobileCountryCodeChanged(const QString &);
    void currentMobileNetworkCodeChanged(const QString &);
    void networkNameChanged(QSystemNetworkInfo::NetworkMode,const QString &);
    void networkModeChanged(QSystemNetworkInfo::NetworkMode);
    void cellIdChanged(int);//1.2
};

//////// QSystemDisplayInfo
class QSystemDisplayInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemDisplayInfoPrivate(QObject *parent = 0);
    virtual ~QSystemDisplayInfoPrivate();

    static int displayBrightness(int screen);
    static int colorDepth(int screen);

    QSystemDisplayInfo::DisplayOrientation getOrientation(int screen);
    float contrast(int screen);
    int getDPIWidth(int screen);
    int getDPIHeight(int screen);
    int physicalHeight(int screen);
    int physicalWidth(int screen);
    QSystemDisplayInfo::BacklightState backlightStatus(int screen);

};

//////// QSystemStorageInfo

struct StorageInfo
{
    QString m_logicalDrive;
    qlonglong m_totalDiskSpace;
    qlonglong m_availableDiskSpace;
    QSystemStorageInfo::DriveType m_driveType;
    QString m_uri;

    StorageInfo(QString logicalDrive,qlonglong totalDiskSpace,qlonglong availableDiskSpace,
                QSystemStorageInfo::DriveType driveType,QString uri)
        :m_logicalDrive(logicalDrive),m_totalDiskSpace(totalDiskSpace)
            ,m_availableDiskSpace(availableDiskSpace)
            ,m_driveType(driveType),m_uri(uri)
    {

    }
};

class QSystemStorageInfoPrivate : public QObject,public StorageInfoUpdateListner
{
    Q_OBJECT

public:
    QSystemStorageInfoPrivate(QObject *parent = 0);
    virtual ~QSystemStorageInfoPrivate();
    qlonglong totalDiskSpace(const QString &driveVolume);
    qlonglong availableDiskSpace(const QString &driveVolume);
    static QStringList logicalDrives();
    QSystemStorageInfo::DriveType typeForDrive(const QString &driveVolume);

    QString uriForDrive(const QString &driveVolume);
    QSystemStorageInfo::StorageState getStorageState(const QString &volume);
    virtual void onStorageStateChanged();
    virtual void onLogicalDriveChanged(QString &drive,bool added);

Q_SIGNALS:
    void logicalDriveChanged(bool, const QString &);
    void storageStateChanged(const QString &vol, QSystemStorageInfo::StorageState state);

private:
    static void df (char*, int);
    static void df_main ();

private:
    QMap<QString, QString> mountEntriesMap;
    static QList<StorageInfo> storageInfo;

};

class QSystemDeviceInfoPrivate : public QObject,public DeviceUpdatelistener
{
    Q_OBJECT

public:

    QSystemDeviceInfoPrivate(QObject *parent = 0);
    virtual ~QSystemDeviceInfoPrivate();

    QSystemDeviceInfo::InputMethodFlags inputMethodType();

    // device
    static QString imei();
    static QString imsi();
    static QString manufacturer();
    static QString model();
    static QString productName();

    int batteryLevel() const;
    QSystemDeviceInfo::BatteryStatus batteryStatus();

    bool isDeviceLocked();
    static QSystemDeviceInfo::SimStatus simStatus();
    QSystemDeviceInfo::Profile currentProfile();

    QSystemDeviceInfo::PowerState currentPowerState();

    bool currentBluetoothPowerState();

    QSystemDeviceInfo::KeyboardTypeFlags keyboardType();
    bool isWirelessKeyboardConnected();
    bool isKeyboardFlipOpen();
    bool keypadLightOn(QSystemDeviceInfo::keypadType type);
    QUuid hostId();
    QSystemDeviceInfo::LockType lockStatus();

    virtual void onBluetoothStateChanged(bool status);
    virtual void onProfileChanged(ProfileMode mode);
    virtual void onDeviceLocked(bool isDeviceLocked);
Q_SIGNALS:
    void batteryLevelChanged(int);
    void batteryStatusChanged(QSystemDeviceInfo::BatteryStatus);
    void bluetoothStateChanged(bool);
    void currentProfileChanged(QSystemDeviceInfo::Profile);
    void powerStateChanged(QSystemDeviceInfo::PowerState);

    void wirelessKeyboardConnected(bool connected);//1.2
    void keyboardFlip(bool open);//1.2
    void deviceLocked(bool isLocked); // 1.2
    void lockStatusChanged(QSystemDeviceInfo::LockType); //1.2

public slots:
    void batteryStatusNotify(QSystemBatteryInfo::BatteryStatus);
    void batteryLevelNotify(int);
    void powerStateChanged(QSystemBatteryInfo::ChargerType);
protected:
    bool hasWirelessKeyboardConnected;
    QSystemBatteryInfo m_batteryinfo;

};

class QSystemScreenSaverPrivate : public QObject
{
    Q_OBJECT

private:
    bool m_screenSaverSet;
public:
    QSystemScreenSaverPrivate(QObject *parent = 0);
    virtual ~QSystemScreenSaverPrivate();

    bool screenSaverInhibited();
    bool setScreenSaverInhibit();
};

class QSystemBatteryInfoPrivate : public QObject,public BatteryInfoUpdateListner
{
    Q_OBJECT
public:
    explicit QSystemBatteryInfoPrivate(QObject *parent = 0);
    virtual ~QSystemBatteryInfoPrivate();


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
    int startCurrentMeasurement(int rate);

    virtual void onBatteryInfoUpdate (BatteryInfo info);
Q_SIGNALS:
    void batteryStatusChanged(QSystemBatteryInfo::BatteryStatus batteryStatus);
    void chargingStateChanged(QSystemBatteryInfo::ChargingState chargingState);
    void chargerTypeChanged(QSystemBatteryInfo::ChargerType chargerType);

    void remainingCapacityPercentChanged(int);
    void remainingCapacityBarsChanged(int);
    void voltageChanged(int);

    //not supported
    void nominalCapacityChanged(int);
    void remainingCapacityChanged(int);
    void batteryCurrentFlowChanged(int);
    void currentFlowChanged(int);
    void cumulativeCurrentFlowChanged(int);
    void remainingChargingTimeChanged(int);

private:
    int  batteryLevel() const ;
    char m_batteryStatus;
    int m_chargerType;
    int m_chargingState;
    int m_maxBars;
    int m_remainingCapacityBars;
    int m_remainingCapacityPercent;
    int m_voltage;
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif
// End of file
