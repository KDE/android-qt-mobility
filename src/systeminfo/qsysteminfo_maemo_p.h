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
#ifndef QSYSTEMINFO_MAEMO_P_H
#define QSYSTEMINFO_MAEMO_P_H


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

#include "qsysteminfo_linux_common_p.h"
#include "qsysteminfo.h"
#include <qmobilityglobal.h>
#if !defined(QT_NO_DBUS)
#include <qhalservice_linux_p.h>

struct ProfileDataValue {
    QString key;
    QString val;
    QString type;
    };

Q_DECLARE_METATYPE(ProfileDataValue)
Q_DECLARE_METATYPE(QList<ProfileDataValue>)

#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE
class QStringList;
class QTimer;
QT_END_NAMESPACE

QTM_BEGIN_NAMESPACE

class QSystemNetworkInfo;
class QSystemInfoPrivate : public QSystemInfoLinuxCommonPrivate
{
    Q_OBJECT

public:

    QSystemInfoPrivate(QSystemInfoLinuxCommonPrivate *parent = 0);
    virtual ~QSystemInfoPrivate();
    QStringList availableLanguages() const;
    QString version(QSystemInfo::Version,  const QString &parameter = QString());

    bool hasFeatureSupported(QSystemInfo::Feature feature);
};

class QNetworkManagerInterface;
class QNetworkManagerInterfaceDeviceWired;
class QNetworkManagerInterfaceDeviceWireless;
class QNetworkManagerInterfaceAccessPoint;

class QSystemNetworkInfoPrivate : public QSystemNetworkInfoLinuxCommonPrivate
{
    Q_OBJECT

public:

    QSystemNetworkInfoPrivate(QSystemNetworkInfoLinuxCommonPrivate *parent = 0);
    virtual ~QSystemNetworkInfoPrivate();

    QSystemNetworkInfo::NetworkStatus networkStatus(QSystemNetworkInfo::NetworkMode mode);
    qint32 networkSignalStrength(QSystemNetworkInfo::NetworkMode mode);
    int cellId();
    int locationAreaCode();

    QString currentMobileCountryCode();
    QString currentMobileNetworkCode();
    QString homeMobileCountryCode();
    QString homeMobileNetworkCode();

    QString networkName(QSystemNetworkInfo::NetworkMode mode);
    QString macAddress(QSystemNetworkInfo::NetworkMode mode);

    QNetworkInterface interfaceForMode(QSystemNetworkInfo::NetworkMode mode);

};

class QSystemDisplayInfoPrivate : public QSystemDisplayInfoLinuxCommonPrivate
{
    Q_OBJECT

public:

    QSystemDisplayInfoPrivate(QSystemDisplayInfoLinuxCommonPrivate *parent = 0);
    virtual ~QSystemDisplayInfoPrivate();

    int displayBrightness(int screen);
};

class QSystemStorageInfoPrivate : public QSystemStorageInfoLinuxCommonPrivate
{
    Q_OBJECT

public:

    QSystemStorageInfoPrivate(QSystemStorageInfoLinuxCommonPrivate *parent = 0);
    virtual ~QSystemStorageInfoPrivate();

};

class QSystemDeviceInfoPrivate : public QSystemDeviceInfoLinuxCommonPrivate
{
    Q_OBJECT

public:

    QSystemDeviceInfoPrivate(QSystemDeviceInfoLinuxCommonPrivate *parent = 0);
    ~QSystemDeviceInfoPrivate();

    QString imei();
    QString imsi();
    QSystemDeviceInfo::SimStatus simStatus();
    bool isDeviceLocked();
    QSystemDeviceInfo::Profile currentProfile();
    QSystemDeviceInfo::PowerState currentPowerState();

protected:
#if !defined(QT_NO_DBUS)
    QHalInterface *halIface;
    QHalDeviceInterface *halIfaceDevice;
    void setupBluetooth();
    void setupProfile();

private Q_SLOTS:
    void halChanged(int,QVariantList);
    void bluezPropertyChanged(const QString&, QDBusVariant);
    void deviceModeChanged(QString newMode);
    void profileChanged(bool changed, bool active, QString profile, QList<ProfileDataValue> values);

private:
    bool flightMode;
    QString profileName;
    bool silentProfile;
    bool vibratingAlertEnabled;
    int ringingAlertVolume;

    QSystemDeviceInfo::PowerState previousPowerState;
#endif
};


class QSystemScreenSaverPrivate : public QSystemScreenSaverLinuxCommonPrivate
{
    Q_OBJECT

public:
    QSystemScreenSaverPrivate(QObject *parent = 0);
    ~QSystemScreenSaverPrivate();

    bool screenSaverInhibited();
    bool setScreenSaverInhibit();
    bool isScreenLockEnabled();
    bool isScreenSaverActive();

private Q_SLOTS:
    void display_blanking_pause();

private:    //data
    bool m_screenSaverInhibited;
    QTimer *ssTimer;

protected:
    QString screenPath;
    QString settingsPath;
    bool screenSaverSecure;
    uint currentPid;

};

QTM_END_NAMESPACE

QT_END_HEADER

#endif /*QSYSTEMINFO_MAEMO_P_H*/

// End of file

