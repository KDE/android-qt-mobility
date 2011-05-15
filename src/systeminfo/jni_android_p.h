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

#ifndef JNI_ANDROID_P_H
#define JNI_ANDROID_P_H
#include <jni.h>
#include <QString>
#include <QStringList>

struct BatteryInfo
{
    char m_batteryStatus;
    int m_chargerType;
    int m_chargingState;
    int m_maxBars;
    int m_remainingCapacityBars;
    int m_remainingCapacityPercent;
    int m_voltage;

    BatteryInfo():m_batteryStatus('?'),m_chargerType(-1),m_chargingState(-1),m_maxBars(0)
            ,m_remainingCapacityBars(0),m_remainingCapacityPercent(0),m_voltage(0)
    {}
};

enum ProfileMode
{
    SilentProfile=1,
    NormalProfile=2,
    VibProfile=4,
    UnknownProfile=0
};

class BatteryInfoUpdateListner
{
public:
    BatteryInfoUpdateListner ()
    {

    }
    virtual ~ BatteryInfoUpdateListner ()
    {

    }
    virtual void onBatteryInfoUpdate (BatteryInfo info)=0;
};

class DeviceUpdatelistener
{
public:
    DeviceUpdatelistener ()
    {

    }
    virtual ~ DeviceUpdatelistener ()
    {

    }
    virtual void onBluetoothStateChanged (bool status)=0;
    virtual void onProfileChanged (ProfileMode mode)=0;
    virtual void onDeviceLocked (bool isDeviceLocked)=0;
};

class GeneralInfoUpdateListner
{
public:
    GeneralInfoUpdateListner ()
    {

    }
    virtual ~ GeneralInfoUpdateListner()
    {

    }
    virtual void onLanguageChanged(QString &language)=0;
};


class StorageInfoUpdateListner
{
public:
    StorageInfoUpdateListner ()
    {

    }
    virtual ~ StorageInfoUpdateListner()
    {

    }
    virtual void onStorageStateChanged ()=0;
    virtual void onLogicalDriveChanged (QString &drive,bool added)=0;
};

class NetworkInfoUpdateListener
{
public :
        NetworkInfoUpdateListener ()
        {

        }
        virtual ~ NetworkInfoUpdateListener ()
        {

        }
        virtual void onPhoneStrengthChanged (int strength)=0;
        virtual void onWifiStrengthChanged (int strength)=0;
        virtual void onNetworkStatusChanged (int status,int mode)=0;
        virtual void onNetworkNameChanged (int mode,QString &name)=0;
        virtual void onCellIdChanged(int cellID)=0;

};

namespace QtSystemInfoJni
{
    BatteryInfo getBatteryInfo ();
    void unregisterBatteryInfoUpdates();
    void registerBatteryInfoUpdates (BatteryInfoUpdateListner * listner);
    //screensaver
    void initScreensaver ();
    void setScreenSaverInhibit ();
    void disableScreenSaverInhbit ();
    //device
    void initDevice ();
    void exitDevice();
    bool getBluetoothState();
    void registerDeviceInfoUpdate (DeviceUpdatelistener * listener);
    ProfileMode getProfileState ();
    QString imei ();
    QString imsi ();
    bool isDeviceLocked ();
    bool isKeyboardFlipOpened ();
    int inputMethodType ();
    int lockStatus ();
    QString manufacturer ();
    QString model ();
    QString productName ();
    int simStatus ();
    int keyboardType ();
    //systeminfo
    QStringList availableLanguages ();
    void registerGeneralInfoupdates(GeneralInfoUpdateListner * listener);
    void unregisterGeneralInfoupdates();
    QString currentCountryCode ();
    QString currentLanguage ();
    QString os ();
    QList<int> featuresSupported ();
    //display
    void initDisplay();
    void exitDisplay();
    int getDPIHeight ();
    int getDPIWidth ();
    int physicalHeight ();
    int physicalWidth ();
    int orientation ();
    int backLightStatus ();
    int displayBrightness ();
    float colorDepth ();
    //storage
    void registerStorageInfoUpdates(StorageInfoUpdateListner *listner);
    void unregisterStorageInfoUpdates ();
    //network
    QString networkName (int mode);
    void registerNetworkInfoUpdates (NetworkInfoUpdateListener *listener);
    void unregisterNetworkInfoUpdates ();
    QString macAddress (int mode);
    int wifiStrength ();
    int phoneStrength ();
    int serviceStatus(int mode);
    int cellId ();
    QString currentMobileCountryCode ();
    QString currentMobileNetworkCode ();
    int locationAreaCode ();
    int currentMode ();
    int networkStatus(int mode);
    int wifiStatus ();
    int bluetoothStatus ();
}
#endif // JNI_ANDROID_P_H
