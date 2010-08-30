/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qsystemnetworkinfo.h"
#include "qsysteminfocommon_p.h"
#include <QMetaType>

QTM_BEGIN_NAMESPACE
        Q_GLOBAL_STATIC(QSystemNetworkInfoPrivate, netInfoPrivate)

/*!
  \fn QSystemNetworkInfo::QSystemNetworkInfo(QObject *parent)
  Constructs a QSystemNetworkInfo object with the given \a parent.
*/

QSystemNetworkInfo::QSystemNetworkInfo(QObject *parent)
   : QObject(parent), d(netInfoPrivate())
{
    qRegisterMetaType<QSystemNetworkInfo::NetworkMode>("QSystemNetworkInfo::NetworkMode");
    qRegisterMetaType<QSystemNetworkInfo::NetworkStatus>("QSystemNetworkInfo::NetworkStatus");
}

/*!
 Destroys the QSystemNetworkInfo object.
*/
QSystemNetworkInfo::~QSystemNetworkInfo()
{
}

/*!
   Returns the status of the network \a mode.
*/
QSystemNetworkInfo::NetworkStatus QSystemNetworkInfo::networkStatus(QSystemNetworkInfo::NetworkMode mode)
{
   return netInfoPrivate()->networkStatus(mode);
}

/*!
   Returns the strength of the network signal, per network \a mode , 0 - 100 linear scaling,
   or -1 in the case of unknown network mode or error.

   In the case of QSystemNetworkInfo::EthMode, it will either be 100 for carrier active, or 0 for when
   there is no carrier or cable connected.
*/
int QSystemNetworkInfo::networkSignalStrength(QSystemNetworkInfo::NetworkMode mode)
{
   return netInfoPrivate()->networkSignalStrength(mode);
}

/*!
 \property QSystemNetworkInfo::cellId
 \brief The devices Cell ID
   Returns the Cell ID of the connected tower or based station.
*/
int QSystemNetworkInfo::cellId()
{
   return netInfoPrivate()->cellId();
}

/*!
 \property QSystemNetworkInfo::locationAreaCode
 \brief The LAC.

   Returns the Location Area Code. In the case of none such as a Desktop, an empty string.
*/
int QSystemNetworkInfo::locationAreaCode()
{
   return netInfoPrivate()->locationAreaCode();
}

/*!
  \property QSystemNetworkInfo::currentMobileCountryCode
  \brief The current MCC.

   Returns the current Mobile Country Code. In the case of none such as a Desktop, an empty string.
/*/
QString QSystemNetworkInfo::currentMobileCountryCode()
{
   return netInfoPrivate()->currentMobileCountryCode();
}

/*!
 \property QSystemNetworkInfo::currentMobileNetworkCode
 \brief The current MNC.

   Returns the current Mobile Network Code. In the case of none such as a Desktop, an empty string.
*/
QString QSystemNetworkInfo::currentMobileNetworkCode()
{
   return netInfoPrivate()->currentMobileNetworkCode();
}

/*!
 \property QSystemNetworkInfo::homeMobileCountryCode
 \brief The home MNC.

   Returns the home Mobile Country Code. In the case of none such as a Desktop, an empty string.
*/
QString QSystemNetworkInfo::homeMobileCountryCode()
{
   return netInfoPrivate()->homeMobileCountryCode();
}

/*!
 \property QSystemNetworkInfo::homeMobileNetworkCode
 \brief The home MCC.

   Returns the home Mobile Network Code. In the case of none such as a Desktop, an empty string.
   Note: Some platforms don't support retrieving this info. In this case the Network Code is
   returned only when the device is registered on home network.
*/
QString QSystemNetworkInfo::homeMobileNetworkCode()
{
   return netInfoPrivate()->homeMobileNetworkCode();
}

/*!
 Returns the name of the operator for the network \a mode.  For wlan this returns the network's current SSID.
In the case of no network such as a desktop, an empty string.
*/
QString QSystemNetworkInfo::networkName(QSystemNetworkInfo::NetworkMode mode)
{
   return netInfoPrivate()->networkName(mode);
}

/*!
 Returns the MAC address for the interface servicing the network \a mode.
 */
QString QSystemNetworkInfo::macAddress(QSystemNetworkInfo::NetworkMode mode)
{
   return netInfoPrivate()->macAddress(mode);
}

/*!
 Returns the first found QNetworkInterface for type \a mode.

 */
QNetworkInterface QSystemNetworkInfo::interfaceForMode(QSystemNetworkInfo::NetworkMode mode)
{
   return netInfoPrivate()->interfaceForMode(mode);
}

/*!
    Returns the current active mode. If more than one mode is active, returns the
     default or preferred mode. If no modes are active, returns UnknownMode.

*/
QSystemNetworkInfo::NetworkMode QSystemNetworkInfo::currentMode()
{
   return netInfoPrivate()->currentMode();
}

/*!
   \internal

   This function is called when the client connects to the networkSignalStrengthChanged()
   signal.
*/
void QSystemNetworkInfo::connectNotify(const char *signal)
{
   //check for networkSignalStrengthChanged() signal connect notification
   //This is not required on all platforms
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
                                 networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode, int))))) {
        netInfoPrivate()->setWlanSignalStrengthCheckEnabled(true);
    }
#endif
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            currentMobileCountryCodeChanged(QString))))) {
        connect(d,SIGNAL(currentMobileCountryCodeChanged(QString)),
                this,SIGNAL(currentMobileCountryCodeChanged(QString)));
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            currentMobileNetworkCodeChanged(QString))))) {
        connect(d,SIGNAL(currentMobileNetworkCodeChanged(QString)),
                this,SIGNAL(currentMobileNetworkCodeChanged(QString)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            networkModeChanged(QSystemNetworkInfo::NetworkMode))))) {
        connect(d,SIGNAL(networkModeChanged(QSystemNetworkInfo::NetworkMode)),
                this,SIGNAL(networkModeChanged(QSystemNetworkInfo::NetworkMode)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            networkNameChanged(QSystemNetworkInfo::NetworkMode,QString))))) {
        connect(d,SIGNAL(networkNameChanged(QSystemNetworkInfo::NetworkMode,QString)),
                this,SIGNAL(networkNameChanged(QSystemNetworkInfo::NetworkMode,QString)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int))))) {
        connect(d,SIGNAL(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int)),
                this,SIGNAL(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus))))) {
        connect(d,SIGNAL(networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus)),
                this,SIGNAL(networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus)));
    }
}

/*!
   \internal

   This function is called when the client disconnects from the networkSignalStrengthChanged()
   signal.

   \sa connectNotify()
*/
void QSystemNetworkInfo::disconnectNotify(const char *signal)
{
   //check for networkSignalStrengthChanged() signal disconnect notification
   //This is not required on all platforms
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
                                 networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode, int))))) {
        netInfoPrivate()->setWlanSignalStrengthCheckEnabled(false);
    }
#endif
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            currentMobileCountryCodeChanged(QString))))) {
        disconnect(d,SIGNAL(currentMobileCountryCodeChanged(QString)),
                this,SIGNAL(currentMobileCountryCodeChanged(QString)));
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            currentMobileNetworkCodeChanged(QString))))) {
        disconnect(d,SIGNAL(currentMobileNetworkCodeChanged(QString)),
                this,SIGNAL(currentMobileNetworkCodeChanged(QString)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            networkModeChanged(QSystemNetworkInfo::NetworkMode))))) {
        disconnect(d,SIGNAL(networkModeChanged(QSystemNetworkInfo::NetworkMode)),
                this,SIGNAL(networkModeChanged(QSystemNetworkInfo::NetworkMode)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            networkNameChanged(QSystemNetworkInfo::NetworkMode,QString))))) {
        disconnect(d,SIGNAL(networkNameChanged(QSystemNetworkInfo::NetworkMode,QString)),
                this,SIGNAL(networkNameChanged(QSystemNetworkInfo::NetworkMode,QString)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int))))) {
        disconnect(d,SIGNAL(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int)),
                this,SIGNAL(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus))))) {
        disconnect(d,SIGNAL(networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus)),
                this,SIGNAL(networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus)));
    }

}


#include "moc_qsystemnetworkinfo.cpp"

QTM_END_NAMESPACE
