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

#include "qsysteminfocommon_p.h"
#include "qsysteminfo_android_p.h"

#include <QStringList>
#include <QDir>
#include <QRegExp>
#include <QTimer>
#include <QList>
#include <mntent.h>
#include <paths.h>

extern "C"
{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <linux/msdos_fs.h>
#include <sys/statfs.h>
#include <fcntl.h>
#include <sys/ioctl.h>
}

#include <math.h>

QTM_BEGIN_NAMESPACE

//systeminfo
QSystemInfoPrivate::QSystemInfoPrivate(QObject *parent)
    :QObject(parent)
{
    QtSystemInfoJni::registerGeneralInfoupdates(this);
}
QSystemInfoPrivate:: ~QSystemInfoPrivate()
{
}

QString QSystemInfoPrivate::currentLanguage() const
{
    return QtSystemInfoJni::currentLanguage();
}
QStringList QSystemInfoPrivate::availableLanguages() const
{
    return QtSystemInfoJni::availableLanguages();
}
QString QSystemInfoPrivate::currentCountryCode() const
{
    return QtSystemInfoJni::currentCountryCode();
}

QString QSystemInfoPrivate::version(QSystemInfo::Version type, const QString &parameter)
{
    Q_UNUSED(parameter);
    switch (type)
    {
        case QSystemInfo::Os:
        {
           return QtSystemInfoJni::os();
        }
        case QSystemInfo::QtCore:
        {
            return qVersion();
        }
        case QSystemInfo::Firmware:
        {
            QString  strvalue;
            const QString versionPath = QLatin1String("/proc/version");
            QFile versionFile(versionPath);
            if (!versionFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qDebug() << "File not opened";
            } else {

                strvalue = QLatin1String(versionFile.readAll().trimmed());
                strvalue = strvalue.split(QLatin1String(" ")).at(2);
                versionFile.close();
            }
            return strvalue;

        }
        default:
          return QString();
    }
}

bool QSystemInfoPrivate::hasFeatureSupported(QSystemInfo::Feature feature)
{
    static const QList<int> featuresSupported=QtSystemInfoJni::featuresSupported();
    if(featuresSupported.contains(feature))
    {
        return true;
    }
    return false;
}

void QSystemInfoPrivate::onLanguageChanged(QString &language)
{
    emit currentLanguageChanged(language);
}


//qnetworkinfo
QSystemNetworkInfoPrivate::QSystemNetworkInfoPrivate(QObject *parent)
        :QObject(parent)
{
    QtSystemInfoJni::registerNetworkInfoUpdates(this);
}

QSystemNetworkInfoPrivate::~QSystemNetworkInfoPrivate()
{
}

QSystemNetworkInfo::NetworkStatus QSystemNetworkInfoPrivate::networkStatus(QSystemNetworkInfo::NetworkMode mode)
{
    QSystemNetworkInfo::NetworkStatus status=QSystemNetworkInfo::UndefinedStatus;
    switch(mode)
    {
        case QSystemNetworkInfo::GsmMode:
        case QSystemNetworkInfo::CdmaMode:
                status= (QSystemNetworkInfo::NetworkStatus)QtSystemInfoJni::serviceStatus(mode);
                break;
        case QSystemNetworkInfo::WcdmaMode:
                 status= (QSystemNetworkInfo::NetworkStatus)QtSystemInfoJni::networkStatus(mode);
                 break;
        case QSystemNetworkInfo::WlanMode:
                 status= (QSystemNetworkInfo::NetworkStatus)QtSystemInfoJni::wifiStatus();
                 break;

        case QSystemNetworkInfo::BluetoothMode:
                status= (QSystemNetworkInfo::NetworkStatus)QtSystemInfoJni::bluetoothStatus();
                break;
#warning FIXME !
//        case QSystemNetworkInfo::WimaxMode:
//        case QSystemNetworkInfo::LteMode:
        default:
               status= QSystemNetworkInfo::UndefinedStatus;
               break;
    };
    return status;
}
int QSystemNetworkInfoPrivate::networkSignalStrength(QSystemNetworkInfo::NetworkMode mode)
{
    int strength=-1;
    switch(mode)
    {
        case QSystemNetworkInfo::WlanMode:
                strength= QtSystemInfoJni::wifiStrength();
            break;
        case QSystemNetworkInfo::WcdmaMode:
        case QSystemNetworkInfo::GsmMode:
        case QSystemNetworkInfo::CdmaMode:
                strength= (QSystemNetworkInfo::UndefinedStatus!=networkStatus(mode))?
                          QtSystemInfoJni::phoneStrength():-1;
            break;
        default:
            strength=-1;
            break;
    };
    return strength;
}
int QSystemNetworkInfoPrivate::cellId()
{
    return QtSystemInfoJni::cellId();
}
int QSystemNetworkInfoPrivate::locationAreaCode()
{
    return QtSystemInfoJni::locationAreaCode();
}

QString QSystemNetworkInfoPrivate::currentMobileCountryCode()
{
    return QtSystemInfoJni::currentMobileCountryCode();
}

QString QSystemNetworkInfoPrivate::currentMobileNetworkCode()
{
    return QtSystemInfoJni::currentMobileNetworkCode();
}

QString QSystemNetworkInfoPrivate::homeMobileCountryCode()
{
    return QtSystemInfoJni::currentMobileCountryCode();
}

QString QSystemNetworkInfoPrivate::homeMobileNetworkCode()
{
    return QtSystemInfoJni::currentMobileNetworkCode();
}

QString QSystemNetworkInfoPrivate::networkName(QSystemNetworkInfo::NetworkMode mode)
{
    QString netname;
    netname=QtSystemInfoJni::networkName(mode);
    return netname;
} //signal

QSystemNetworkInfo::CellDataTechnology QSystemNetworkInfoPrivate::cellDataTechnology()
{
#warning FIXME !
    return QSystemNetworkInfo::UnknownDataTechnology;
}

QString QSystemNetworkInfoPrivate::macAddress(QSystemNetworkInfo::NetworkMode mode)
{
    return QtSystemInfoJni::macAddress(mode);
}

QNetworkInterface QSystemNetworkInfoPrivate::interfaceForMode(QSystemNetworkInfo::NetworkMode)
{
    return QNetworkInterface ();
}

QSystemNetworkInfo::NetworkMode QSystemNetworkInfoPrivate::currentMode()
{
    return (QSystemNetworkInfo::NetworkMode)QtSystemInfoJni::currentMode();
}

void QSystemNetworkInfoPrivate::onPhoneStrengthChanged(int strength)
{
    emit networkSignalStrengthChanged(QSystemNetworkInfo::GsmMode,strength);
}

void QSystemNetworkInfoPrivate::onWifiStrengthChanged(int strength)
{
    emit networkSignalStrengthChanged(QSystemNetworkInfo::WlanMode,strength);
}

void QSystemNetworkInfoPrivate::onNetworkStatusChanged(int status, int mode)
{
    emit networkStatusChanged((QSystemNetworkInfo::NetworkMode)mode
                              ,(QSystemNetworkInfo::NetworkStatus)status);
}

void QSystemNetworkInfoPrivate::onNetworkNameChanged(int mode, QString &name)
{
    emit networkNameChanged((QSystemNetworkInfo::NetworkMode)mode
                            ,name);
}

void QSystemNetworkInfoPrivate::onCellIdChanged(int cellID)
{
    emit cellIdChanged(cellID);
}

//display info
QSystemDisplayInfoPrivate::QSystemDisplayInfoPrivate(QObject *parent)
    :QObject(parent)
{
    QtSystemInfoJni::initDisplay();
}

QSystemDisplayInfoPrivate::~QSystemDisplayInfoPrivate()
{
}

int QSystemDisplayInfoPrivate::displayBrightness(int)
{
    return QtSystemInfoJni::displayBrightness();
}

int QSystemDisplayInfoPrivate::colorDepth(int)
{
    return (int)QtSystemInfoJni::colorDepth();
}

QSystemDisplayInfo::DisplayOrientation QSystemDisplayInfoPrivate::orientation(int)
{
    return (QSystemDisplayInfo::DisplayOrientation)QtSystemInfoJni::orientation();
}
float QSystemDisplayInfoPrivate::contrast(int)
{
    return 0;
}
int QSystemDisplayInfoPrivate::getDPIWidth(int)
{
    return QtSystemInfoJni::getDPIWidth();
}
int QSystemDisplayInfoPrivate::getDPIHeight(int)
{
    return QtSystemInfoJni::getDPIHeight();
}
int QSystemDisplayInfoPrivate::physicalHeight(int)
{
    return QtSystemInfoJni::physicalHeight();
}
int QSystemDisplayInfoPrivate::physicalWidth(int)
{
    return QtSystemInfoJni::physicalWidth();
}

QSystemDisplayInfo::BacklightState QSystemDisplayInfoPrivate::backlightStatus(int)
{
    return (QSystemDisplayInfo::BacklightState)QtSystemInfoJni::backLightStatus();
}


//storage
QList<StorageInfo> QSystemStorageInfoPrivate::storageInfo=QList<StorageInfo> ();

QSystemStorageInfoPrivate::QSystemStorageInfoPrivate(QObject *parent)
    :QObject(parent)
{
    QtSystemInfoJni::registerStorageInfoUpdates (this);
}

QSystemStorageInfoPrivate::~QSystemStorageInfoPrivate()
{
}
qlonglong QSystemStorageInfoPrivate::totalDiskSpace(const QString &driveVolume)
{
    QList<StorageInfo>::iterator it;
    qlonglong totalDiskSpace=0;
    for (it = storageInfo.begin(); it != storageInfo.end();it++)
    {
        StorageInfo info=(StorageInfo)*it;
        if(info.m_logicalDrive==driveVolume)
        {
           totalDiskSpace= info.m_totalDiskSpace;
        }
    }
    return totalDiskSpace;
}
qlonglong QSystemStorageInfoPrivate::availableDiskSpace(const QString &driveVolume)
{
    QList<StorageInfo>::iterator it;
    qlonglong availableDiskSpace=0;
    for (it = storageInfo.begin(); it != storageInfo.end();it++)
    {
        StorageInfo info=(StorageInfo)*it;
        if(info.m_logicalDrive==driveVolume)
        {
           availableDiskSpace= info.m_availableDiskSpace;
        }
    }
    return availableDiskSpace;
}

QStringList QSystemStorageInfoPrivate::logicalDrives()
{
    storageInfo.clear();
    df_main ();
    QStringList logicalDrives;
    QList<StorageInfo>::iterator it;
    for (it = storageInfo.begin(); it != storageInfo.end();it++)
    {
        StorageInfo info=(StorageInfo)*it;
        logicalDrives.append(info.m_logicalDrive);
    }
    return logicalDrives;
}

//license of bsd android
void QSystemStorageInfoPrivate::df(char *s, int always)
{
    struct statfs st;

    if (statfs(s, &st) < 0)
    {
        fprintf(stderr, "%s: %s\n", s, strerror(errno));
        return;
    }
    else
    {
        if (st.f_blocks == 0 && !always)
            return;
        if(st.f_type==TMPFS_MAGIC)
        {
            return;
        }

        QString logicalDrive=tr(s);
        qlonglong totalSize=((qlonglong)st.f_blocks * (qlonglong)st.f_bsize) / 1024*1000;
        qlonglong available=((qlonglong)st.f_bfree * (qlonglong)st.f_bsize) / 1024*1000;
        QSystemStorageInfo::DriveType driveType=QSystemStorageInfo::DriveType ();
        QString uri;
        int fd = open(s, O_RDONLY);
        if (fd >= 0) {
            int result = ioctl(fd, VFAT_IOCTL_GET_VOLUME_ID);
            uri.append(QString::number(result));
            close(fd);
        }

        if(logicalDrive.startsWith(tr("/mnt")))
        {
            driveType=QSystemStorageInfo::RemovableDrive;
        }
        else
            if( (logicalDrive.startsWith(tr("/system")))
                ||(logicalDrive.startsWith(tr("/data"))) )
        {
            driveType=QSystemStorageInfo::InternalDrive;
        }
        else
        {
            driveType=QSystemStorageInfo::NoDrive;
        }
        StorageInfo info(logicalDrive,totalSize,available,driveType,uri);
        storageInfo.append(info);
    }
}

void QSystemStorageInfoPrivate::df_main()
{
        char s[2000];
        FILE *f = fopen("/proc/mounts", "r");

        while (fgets(s, 2000, f))
        {
            char *c, *e = s;

            for (c = s; *c; c++) {
                if (*c == ' ')
                {
                    e = c + 1;
                    break;
                }
            }

            for (c = e; *c; c++)
            {
                if (*c == ' ')
                {
                    *c = '\0';
                    break;
                }
            }

            df(e, 0);
        }

        fclose(f);

}
QSystemStorageInfo::DriveType QSystemStorageInfoPrivate::typeForDrive(const QString &driveVolume)
{
    QSystemStorageInfo::DriveType typeForDrive=QSystemStorageInfo::DriveType ();
    QList<StorageInfo>::iterator it;
    for (it = storageInfo.begin(); it != storageInfo.end();it++)
    {
        StorageInfo info=(StorageInfo)*it;
        if(info.m_logicalDrive==driveVolume)
        {
           typeForDrive= info.m_driveType;
        }
    }
    return typeForDrive;
}

QString QSystemStorageInfoPrivate::uriForDrive(const QString &driveVolume)
{
   QString uriForDrive=QString ();
    QList<StorageInfo>::iterator it;
    for (it = storageInfo.begin(); it != storageInfo.end();it++)
    {
        StorageInfo info=(StorageInfo)*it;
        if(info.m_logicalDrive==driveVolume)
        {
           uriForDrive= info.m_uri;
        }
    }
    return uriForDrive;
}

QSystemStorageInfo::StorageState QSystemStorageInfoPrivate::getStorageState(const QString &volume)
{
    QList<StorageInfo>::iterator it;
    QSystemStorageInfo::StorageState state=QSystemStorageInfo::UnknownStorageState;
    for (it = storageInfo.begin(); it != storageInfo.end();it++)
    {
        StorageInfo info=(StorageInfo)*it;
        if(info.m_logicalDrive==volume)
        {
            int percent=(info.m_availableDiskSpace*100)/info.m_totalDiskSpace;
            if(percent<=2)
            {
                state=QSystemStorageInfo::CriticalStorageState;
            }
            else if(percent<=10)
            {
                state=QSystemStorageInfo::VeryLowStorageState;
            }
            else if(percent<=40)
            {
                state=QSystemStorageInfo::LowStorageState;
            }
            else
            {
                state=QSystemStorageInfo::NormalStorageState;
            }
        }
    }
    return state;
}

 void QSystemStorageInfoPrivate::onStorageStateChanged()
 {
     QMap<QString,QSystemStorageInfo::StorageState> storageStateMap;
     QList<StorageInfo>::iterator it;
     for (it = storageInfo.begin(); it != storageInfo.end();it++)
     {
         StorageInfo info=(StorageInfo)*it;
         storageStateMap.insert(info.m_logicalDrive,getStorageState(info.m_logicalDrive));
     }
     //refresh the list
     logicalDrives();
     for (it = storageInfo.begin(); it != storageInfo.end();it++)
     {
         StorageInfo info=(StorageInfo)*it;
         QSystemStorageInfo::StorageState latestState=getStorageState(info.m_logicalDrive);
         if( storageStateMap.value(info.m_logicalDrive) != latestState )
         {
                emit storageStateChanged(info.m_logicalDrive,latestState);
         }
     }
 }

void QSystemStorageInfoPrivate::onLogicalDriveChanged(QString &drive,bool added)
{
    emit logicalDriveChanged(added,drive);
}

//device
QSystemDeviceInfoPrivate::QSystemDeviceInfoPrivate(QObject *parent)
    :QObject(parent)
{
    QtSystemInfoJni::initDevice();
    QtSystemInfoJni::registerDeviceInfoUpdate(this);
    connect(&m_batteryinfo,SIGNAL(chargerTypeChanged ( QSystemBatteryInfo::ChargerType)),
            this,SLOT(powerStateChanged(QSystemBatteryInfo::ChargerType)));

    connect(&m_batteryinfo,SIGNAL(remainingCapacityPercentChanged(int)),
            this,SLOT(batteryLevelNotify(int)));

    connect(&m_batteryinfo,SIGNAL(batteryStatusChanged(QSystemBatteryInfo::BatteryStatus)),
            this,SLOT(batteryStatusNotify(QSystemBatteryInfo::BatteryStatus)),Qt::DirectConnection);
}
QSystemDeviceInfoPrivate::~QSystemDeviceInfoPrivate()
{
}

QSystemDeviceInfo::InputMethodFlags QSystemDeviceInfoPrivate::inputMethodType()
{
    return (QSystemDeviceInfo::InputMethodFlags)QtSystemInfoJni::inputMethodType();
}

// device
QString QSystemDeviceInfoPrivate::imei()
{
    return QtSystemInfoJni::imei();
}
QString QSystemDeviceInfoPrivate::imsi()
{
    return QtSystemInfoJni::imsi();
}
QString QSystemDeviceInfoPrivate::manufacturer()
{
    return QtSystemInfoJni::manufacturer();
}
QString QSystemDeviceInfoPrivate::model()
{
    return QtSystemInfoJni::model();
}
QString QSystemDeviceInfoPrivate::productName()
{
    return QtSystemInfoJni::productName();
}
QByteArray QSystemDeviceInfoPrivate::uniqueDeviceID() const
{
    return imei().toUtf8();
}
int QSystemDeviceInfoPrivate::batteryLevel() const
{
    return m_batteryinfo.remainingCapacityPercent();
}
QSystemDeviceInfo::BatteryStatus QSystemDeviceInfoPrivate::batteryStatus()
{
    QSystemDeviceInfo::BatteryStatus status;

    switch(m_batteryinfo.batteryStatus())
    {
        case 0:
        case '?':
            status=QSystemDeviceInfo::NoBatteryLevel;
            break;
        case 1:
            status=QSystemDeviceInfo::BatteryCritical;
            break;
        case 2:
            status=QSystemDeviceInfo::BatteryVeryLow;
            break;
        case 3:
            status=QSystemDeviceInfo::BatteryLow;
            break;
        case 4:
        case 5:
            status=QSystemDeviceInfo::BatteryNormal;
            break;
        default:
            status=QSystemDeviceInfo::NoBatteryLevel;
    }
    return status;
}


bool QSystemDeviceInfoPrivate::isDeviceLocked()
{
    return QtSystemInfoJni::isDeviceLocked();
}
void QSystemDeviceInfoPrivate::onDeviceLocked(bool isDeviceLocked)
{
    emit deviceLocked(isDeviceLocked);
}

int QSystemDeviceInfoPrivate::messageRingtoneVolume() const
{
#warning FIXME !
    return 100;
}
int QSystemDeviceInfoPrivate::voiceRingtoneVolume() const
{
#warning FIXME !
    return 100;
}
bool QSystemDeviceInfoPrivate::vibrationActive() const
{
#warning FIXME !
    return true;
}

QSystemDeviceInfo::SimStatus QSystemDeviceInfoPrivate::simStatus()
{
    return (QSystemDeviceInfo::SimStatus)QtSystemInfoJni::simStatus();
}
QSystemDeviceInfo::Profile QSystemDeviceInfoPrivate::currentProfile()
{
    return (QSystemDeviceInfo::Profile)(int)QtSystemInfoJni::getProfileState();
}

void QSystemDeviceInfoPrivate::onProfileChanged(ProfileMode mode)
{
    emit currentProfileChanged((QSystemDeviceInfo::Profile)(int)mode);
}

QSystemDeviceInfo::PowerState QSystemDeviceInfoPrivate::currentPowerState()
{
    QSystemDeviceInfo::PowerState state;
    switch(m_batteryinfo.chargerType())
    {
        case QSystemBatteryInfo::UnknownCharger:
                state=QSystemDeviceInfo::UnknownPower;
                break;

        case QSystemBatteryInfo::NoCharger:
                state=QSystemDeviceInfo::BatteryPower;
                break;

        case QSystemBatteryInfo::WallCharger:
                 state=QSystemDeviceInfo::WallPowerChargingBattery;
                 break;
        default:
                 state=QSystemDeviceInfo::UnknownPower;
                 break;
    }
    return state;
}

QSystemDeviceInfo::ThermalState QSystemDeviceInfoPrivate::currentThermalState() const
{
#warning FIXME !
    return QSystemDeviceInfo::UnknownThermal;
}

void QSystemDeviceInfoPrivate::powerStateChanged(QSystemBatteryInfo::ChargerType chargerType)
{
    QSystemDeviceInfo::PowerState state;
    switch(chargerType)
    {
        case QSystemBatteryInfo::UnknownCharger:
                state=QSystemDeviceInfo::UnknownPower;
                break;

        case QSystemBatteryInfo::NoCharger:
                state=QSystemDeviceInfo::BatteryPower;
                break;

        case QSystemBatteryInfo::WallCharger:
                 state=QSystemDeviceInfo::WallPowerChargingBattery;
                 break;
        default:
                 state=QSystemDeviceInfo::UnknownPower;
                 break;
    }
    emit powerStateChanged(state);
}

void QSystemDeviceInfoPrivate::batteryLevelNotify(int level)
{
    emit batteryLevelChanged(level);
}

void QSystemDeviceInfoPrivate::batteryStatusNotify(QSystemBatteryInfo::BatteryStatus batteryStatus)
{
    QSystemDeviceInfo::BatteryStatus status=QSystemDeviceInfo::NoBatteryLevel;
    switch(batteryStatus)
    {
        case 0:
        case '?':
            status=QSystemDeviceInfo::NoBatteryLevel;
            break;
        case 1:
            status=QSystemDeviceInfo::BatteryCritical;
            break;
        case 2:
            status=QSystemDeviceInfo::BatteryVeryLow;
            break;
        case 3:
            status=QSystemDeviceInfo::BatteryLow;
            break;
        case 4:
        case 5:
            status=QSystemDeviceInfo::BatteryNormal;
            break;
        default:
            status=QSystemDeviceInfo::NoBatteryLevel;
    }
    emit batteryStatusChanged(status);
}

bool QSystemDeviceInfoPrivate::currentBluetoothPowerState()
{
    return QtSystemInfoJni::getBluetoothState();
}
void QSystemDeviceInfoPrivate::onBluetoothStateChanged(bool status)
{
    emit bluetoothStateChanged(status);
}

QSystemDeviceInfo::KeyboardTypeFlags QSystemDeviceInfoPrivate::keyboardTypes()
{
    return (QSystemDeviceInfo::KeyboardTypeFlags)QtSystemInfoJni::keyboardType();
}
bool QSystemDeviceInfoPrivate::isWirelessKeyboardConnected()
{
    return false;
}
bool QSystemDeviceInfoPrivate::isKeyboardFlippedOpen()
{
    return  QtSystemInfoJni::isKeyboardFlipOpened();
}

bool QSystemDeviceInfoPrivate::keypadLightOn(QSystemDeviceInfo::KeypadType)
{
    return false;
}

QSystemDeviceInfo::LockType QSystemDeviceInfoPrivate::lockStatus()
{
    return QSystemDeviceInfo::LockType ();
}

//screensaver
QSystemScreenSaverPrivate::QSystemScreenSaverPrivate(QObject *parent)
: QObject(parent),m_screenSaverSet(false)
{
    QtSystemInfoJni::initScreensaver();
}

bool QSystemScreenSaverPrivate::setScreenSaverInhibit()
{
    if(m_screenSaverSet)
    {
        return m_screenSaverSet;
    }
    m_screenSaverSet=true;
    QtSystemInfoJni::setScreenSaverInhibit();
    return m_screenSaverSet;
}

void QSystemScreenSaverPrivate::setScreenSaverInhibited(bool on)
{
#warning FIXME !
//    if(m_screenSaverSet)
//    {
//        return m_screenSaverSet;
//    }
//    m_screenSaverSet=true;
//    QtSystemInfoJni::setScreenSaverInhibit();
//    return m_screenSaverSet;
}

bool QSystemScreenSaverPrivate::screenSaverInhibited()
{
    return m_screenSaverSet;
}

QSystemScreenSaverPrivate::~QSystemScreenSaverPrivate()
{
    if(m_screenSaverSet)
        QtSystemInfoJni::disableScreenSaverInhbit();
}

QSystemBatteryInfoPrivate::QSystemBatteryInfoPrivate(QObject *parent)
    : QObject(parent)
{
    BatteryInfo info=QtSystemInfoJni::getBatteryInfo();
    m_batteryStatus=info.m_batteryStatus;
    m_chargerType=info.m_chargerType;
    m_chargingState=info.m_chargingState;
    m_maxBars=info.m_maxBars;
    m_remainingCapacityBars=info.m_remainingCapacityBars;
    m_remainingCapacityPercent=info.m_remainingCapacityPercent;
    m_voltage=info.m_voltage;
    QtSystemInfoJni::registerBatteryInfoUpdates(this);
}

QSystemBatteryInfoPrivate::~QSystemBatteryInfoPrivate()
{
}

void QSystemBatteryInfoPrivate::onBatteryInfoUpdate (BatteryInfo info)
{
    m_batteryStatus=info.m_batteryStatus;
    m_chargerType=info.m_chargerType;
    m_chargingState=info.m_chargingState;
    m_maxBars=info.m_maxBars;
    m_remainingCapacityBars=info.m_remainingCapacityBars;
    m_remainingCapacityPercent=info.m_remainingCapacityPercent;
    m_voltage=info.m_voltage;

    emit batteryStatusChanged ( (QSystemBatteryInfo::BatteryStatus) m_batteryStatus );
    emit chargerTypeChanged ( (QSystemBatteryInfo::ChargerType) m_chargerType );
    emit chargingStateChanged ((QSystemBatteryInfo::ChargingState) m_chargingState );
    emit remainingCapacityBarsChanged (m_remainingCapacityBars );
    emit remainingCapacityPercentChanged (m_remainingCapacityPercent );
    emit voltageChanged ( m_voltage );
}

QSystemBatteryInfo::BatteryStatus QSystemBatteryInfoPrivate::batteryStatus() const
{
    return (QSystemBatteryInfo::BatteryStatus)m_batteryStatus;
}

QSystemBatteryInfo::ChargerType QSystemBatteryInfoPrivate::chargerType () const
{
    return (QSystemBatteryInfo::ChargerType)m_chargerType;
}

QSystemBatteryInfo::ChargingState QSystemBatteryInfoPrivate::chargingState () const
{
    return  (QSystemBatteryInfo::ChargingState)m_chargingState;
}

int QSystemBatteryInfoPrivate::currentFlow () const
{
    return int();
}

QSystemBatteryInfo::EnergyUnit 	QSystemBatteryInfoPrivate::energyMeasurementUnit () const
{
    return QSystemBatteryInfo::EnergyUnit ();
}

int QSystemBatteryInfoPrivate::maxBars () const
{
    return m_maxBars;
}

int QSystemBatteryInfoPrivate::nominalCapacity () const
{
    return int ();
}

int QSystemBatteryInfoPrivate::remainingCapacity () const
{
    return int();
}

int QSystemBatteryInfoPrivate::remainingCapacityBars () const
{
    return m_remainingCapacityBars;
}

int QSystemBatteryInfoPrivate::remainingCapacityPercent () const
{
    return m_remainingCapacityPercent;
}

int QSystemBatteryInfoPrivate::remainingChargingTime () const
{
    return int ();
}

int QSystemBatteryInfoPrivate::startCurrentMeasurement ( int rate )
{
    Q_UNUSED(rate);
    return int ();
}

int QSystemBatteryInfoPrivate::voltage () const
{
    return m_voltage;
}

#include "moc_qsysteminfo_android_p.cpp"

QTM_END_NAMESPACE
