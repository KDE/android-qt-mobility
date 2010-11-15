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

#ifndef QSYSTEMDEVICEINFO_H
#define QSYSTEMDEVICEINFO_H

#include <QObject>
#include <QUuid>
#include "qmobilityglobal.h"

QT_BEGIN_HEADER
QTM_BEGIN_NAMESPACE

class QSystemDeviceInfoPrivate;

class  Q_SYSINFO_EXPORT QSystemDeviceInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Profile currentProfile READ currentProfile NOTIFY currentProfileChanged)
    Q_PROPERTY(PowerState currentPowerState READ currentPowerState NOTIFY powerStateChanged)
    Q_PROPERTY(SimStatus simStatus READ simStatus CONSTANT)
    Q_PROPERTY(BatteryStatus batteryStatus READ batteryStatus NOTIFY batteryStatusChanged)
    Q_PROPERTY(InputMethodFlags inputMethodType READ inputMethodType)

    Q_PROPERTY(QString imei READ imei CONSTANT)
    Q_PROPERTY(QString imsi READ imsi CONSTANT)
    Q_PROPERTY(QString manufacturer READ manufacturer CONSTANT)
    Q_PROPERTY(QString model READ model CONSTANT)
    Q_PROPERTY(QString productName READ productName CONSTANT)
    Q_PROPERTY(int batteryLevel READ batteryLevel NOTIFY batteryLevelChanged)
    Q_PROPERTY(bool isDeviceLocked READ isDeviceLocked NOTIFY deviceLocked)
    Q_PROPERTY(bool currentBluetoothPowerState READ currentBluetoothPowerState NOTIFY bluetoothStateChanged)

    Q_PROPERTY(KeyboardTypeFlags keyboardType READ keyboardType)//1.2
    Q_PROPERTY(bool isWirelessKeyboardConnected READ isWirelessKeyboardConnected NOTIFY wirelessKeyboardConnected)//1.2
    Q_PROPERTY(bool isKeyboardFlipOpen READ isKeyboardFlipOpen NOTIFY keyboardFlip)//1.2
    Q_PROPERTY(QSystemDeviceInfo::LockType typeOfLock READ typeOfLock NOTIFY lockChanged)
    Q_PROPERTY(QSystemDeviceInfo::PowerState currentPowerState READ currentPowerState NOTIFY powerStateChanged)

    Q_ENUMS(BatteryStatus)
    Q_ENUMS(PowerState)
    Q_FLAGS(InputMethod InputMethodFlags)
    Q_ENUMS(SimStatus)
    Q_ENUMS(Profile)
    Q_ENUMS(LockType)

    Q_FLAGS(KeyboardType KeyboardTypeFlags) //1.2

public:

    explicit QSystemDeviceInfo(QObject *parent = 0);
    virtual ~QSystemDeviceInfo();

    enum BatteryStatus {
        NoBatteryLevel = 0,
        BatteryCritical,
        BatteryVeryLow,
        BatteryLow,
        BatteryNormal
    };


    enum PowerState {
        UnknownPower = 0,
        BatteryPower,
        WallPower,
        WallPowerChargingBattery
    };


    enum InputMethod {
        Keys = 0x0000001,
        Keypad = 0x0000002,
        Keyboard = 0x0000004,
        SingleTouch = 0x0000008,
        MultiTouch = 0x0000010,
        Mouse = 0x0000020
    };
    Q_DECLARE_FLAGS(InputMethodFlags, InputMethod)

    enum Profile {
        UnknownProfile = 0,
        SilentProfile,
        NormalProfile,
        LoudProfile,
        VibProfile,
        OfflineProfile,
        PowersaveProfile,
        CustomProfile,
        BeepProfile
    };

    enum SimStatus {
        SimNotAvailable = 0,
        SingleSimAvailable,
        DualSimAvailable,
        SimLocked
    };

    enum KeyboardType {
        UnknownKeyboard = 0,
        SoftwareKeyboard= 0x0000001,
        ITUKeypad = 0x0000002,
        HalfQwertyKeyboard = 0x0000004,
        FullQwertyKeyboard = 0x0000008,
        WirelessKeyboard = 0x0000010
      };//1.2
    Q_DECLARE_FLAGS(KeyboardTypeFlags, KeyboardType)//1.2

    enum LockType {
        UnknownLock = 0,
        DeviceLocked,
        TouchAndKeyboardLocked
    }; //1.2

    QSystemDeviceInfo::InputMethodFlags inputMethodType();

    QString imei();
    QString imsi();
    QString manufacturer();
    QString model();
    QString productName();
    int batteryLevel() const;
    QSystemDeviceInfo::BatteryStatus batteryStatus();

    bool isDeviceLocked();
    QSystemDeviceInfo::SimStatus simStatus();
    QSystemDeviceInfo::Profile currentProfile();
    QSystemDeviceInfo::PowerState currentPowerState();

    bool currentBluetoothPowerState();

    QSystemDeviceInfo::KeyboardTypeFlags keyboardType(); //1.2
    bool isWirelessKeyboardConnected(); //1.2
    bool isKeyboardFlipOpen();//1.2

    bool keypadLightOn(); //1.2
    bool backLightOn(); //1.2
    QUuid hostId(); //1.2
    QSystemDeviceInfo::LockType typeOfLock(); //1.2
#ifdef TESTR
    QSystemDeviceInfoPrivate *priv;
#endif

Q_SIGNALS:
    void batteryLevelChanged(int level);
    void batteryStatusChanged(QSystemDeviceInfo::BatteryStatus batteryStatus);
    void powerStateChanged(QSystemDeviceInfo::PowerState powerState);
    void currentProfileChanged(QSystemDeviceInfo::Profile currentProfile);
    void bluetoothStateChanged(bool on);

    void wirelessKeyboardConnected(bool connected);//1.2
    void keyboardFlip(bool open);//1.2
    void deviceLocked(bool isLocked); // 1.2
    void lockChanged(QSystemDeviceInfo::LockType, bool); //1.2


private:
    QSystemDeviceInfoPrivate *d;
protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QSystemDeviceInfo::InputMethodFlags )


QTM_END_NAMESPACE

QT_END_HEADER

#endif // QSYSTEMDEVICEINFO_H
