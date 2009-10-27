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
#ifndef QSYSTEMINFO_S60_H
#define QSYSTEMINFO_S60_H


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

#include "qsysinfoglobal.h"
#include "qsysteminfo.h"

#include <f32file.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

#include "telephonyinfo_s60.h"

//////// QSystemInfo
class QSystemInfoPrivate : public QObject
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

Q_SIGNALS:
    void currentLanguageChanged(const QString &);

private:
    QLocale::Language TLanguageToQLocale(TLanguage language) const;
};

//////// QSystemNetworkInfo
class QSystemNetworkInfoPrivate : public QObject, public MTelephonyInfoObserver
{
    Q_OBJECT

public:

    QSystemNetworkInfoPrivate(QObject *parent = 0);
    virtual ~QSystemNetworkInfoPrivate();

    QSystemNetworkInfo::NetworkStatus networkStatus(QSystemNetworkInfo::NetworkMode mode);
    static int networkSignalStrength(QSystemNetworkInfo::NetworkMode mode);
    static int cellId();
    static int locationAreaCode();

    static QString currentMobileCountryCode();
    static QString currentMobileNetworkCode();

    static QString homeMobileCountryCode();
    static QString homeMobileNetworkCode();

    static QString networkName(QSystemNetworkInfo::NetworkMode mode); //signal
    QString macAddress(QSystemNetworkInfo::NetworkMode mode);

    QNetworkInterface interfaceForMode(QSystemNetworkInfo::NetworkMode mode);

Q_SIGNALS:
    void networkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus);
    void networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode, int);
    void currentMobileCountryCodeChanged(const QString &);
    void currentMobileNetworkCodeChanged(const QString &);
    void networkNameChanged(QSystemNetworkInfo::NetworkMode,const QString &);
    void networkModeChanged(QSystemNetworkInfo::NetworkMode);

protected:  //from MTelephonyInfoObserver
    void batteryLevelChanged(){};
    void powerStateChanged(){};

    void countryCodeChanged();
    void networkCodeChanged();
    void networkNameChanged();

    void cellNetworkSignalStrengthChanged();
    void cellNetworkStatusChanged();
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
};


//////// QSystemStorageInfo
class QSystemStorageInfoPrivate : public QObject
{
    Q_OBJECT

public:
    QSystemStorageInfoPrivate(QObject *parent = 0);
    virtual ~QSystemStorageInfoPrivate();
    qlonglong totalDiskSpace(const QString &driveVolume);
    qlonglong availableDiskSpace(const QString &driveVolume);
    static QStringList logicalDrives();
    QSystemStorageInfo::DriveType typeForDrive(const QString &driveVolume);

private:
    RFs iFs;
};

//////// QSystemDeviceInfo
class DeviceInfo;

class QSystemDeviceInfoPrivate : public QObject, public MTelephonyInfoObserver
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
    QSystemDeviceInfo::SimStatus simStatus();
    QSystemDeviceInfo::Profile currentProfile();

    QSystemDeviceInfo::PowerState currentPowerState();

Q_SIGNALS:
    void batteryLevelChanged(int);
    void batteryStatusChanged(QSystemDeviceInfo::BatteryStatus);
    void bluetoothStateChanged(bool);
    void currentProfileChanged(QSystemDeviceInfo::Profile);
    void powerStateChanged(QSystemDeviceInfo::PowerState);

protected:  //from MTelephonyInfoObserver
    void batteryLevelChanged();
    void powerStateChanged();
    
    void countryCodeChanged(){};
    void networkCodeChanged(){};
    void networkNameChanged(){};

    void cellNetworkSignalStrengthChanged(){};
    void cellNetworkStatusChanged(){};
};

//////// QSystemScreenSaver
class QSystemScreenSaverPrivate : public QObject
{
    Q_OBJECT

public:
    QSystemScreenSaverPrivate(QObject *parent = 0);

    bool screenSaverInhibited() {return false;}
    bool setScreenSaverInhibit() {return false;}
    static bool isScreenLockOn() {return false;}
};

//////// DeviceInfo (singleton)
class DeviceInfo
{

public:
    static DeviceInfo *instance()
    {
        if (!m_instance)
        {
            m_instance = new DeviceInfo;
        }
        return m_instance;
    }

    CPhoneInfo *phoneInfo()
    {
        if (!m_phoneInfo) {
            m_phoneInfo = new CPhoneInfo(*m_telephony);
        }
        return m_phoneInfo;
    }

    CSubscriberInfo *subscriberInfo()
    {
        if (!m_subscriberInfo) {
            m_subscriberInfo = new CSubscriberInfo(*m_telephony);
        }
        return m_subscriberInfo;
    }

    CBatteryInfo *batteryInfo()
    {
        if (!m_batteryInfo) {
            m_batteryInfo = new CBatteryInfo(*m_telephony);
        }
        return m_batteryInfo;
    }
    
    CCellNetworkInfo *cellNetworkInfo()
    {
        if (!m_cellNetworkInfo) {
            m_cellNetworkInfo = new CCellNetworkInfo(*m_telephony);
        }
        return m_cellNetworkInfo;
    }
    
    CCellNetworkRegistrationInfo *cellNetworkRegistrationInfo()
    {
        if (!m_cellNetworkRegistrationInfo) {
            m_cellNetworkRegistrationInfo = new CCellNetworkRegistrationInfo(*m_telephony);
        }
        return m_cellNetworkRegistrationInfo;
    }
    
    CCellSignalStrengthInfo *cellSignalStrenghtInfo()
    {
        if (!m_cellSignalStrengthInfo) {
            m_cellSignalStrengthInfo = new CCellSignalStrengthInfo(*m_telephony);
        }
        return m_cellSignalStrengthInfo;
    }

private:
    DeviceInfo() : m_phoneInfo(NULL), m_subscriberInfo(NULL), m_batteryInfo(NULL),
        m_cellNetworkInfo(NULL), m_cellNetworkRegistrationInfo(NULL),
        m_cellSignalStrengthInfo(NULL)
    {
        m_telephony = CTelephony::NewL();
    };

    ~DeviceInfo()
    {
        delete m_cellSignalStrengthInfo;
        delete m_cellNetworkRegistrationInfo;
        delete m_cellNetworkInfo;
        delete m_batteryInfo;
        delete m_subscriberInfo;
        delete m_phoneInfo;
        delete m_telephony;
    }

    static DeviceInfo *m_instance;

    CTelephony *m_telephony;
    CPhoneInfo *m_phoneInfo;
    CSubscriberInfo *m_subscriberInfo;
    CBatteryInfo *m_batteryInfo;
    CCellNetworkInfo *m_cellNetworkInfo;
    CCellNetworkRegistrationInfo *m_cellNetworkRegistrationInfo;
    CCellSignalStrengthInfo *m_cellSignalStrengthInfo;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif /*QSYSTEMSINFO_S60_H*/

// End of file
