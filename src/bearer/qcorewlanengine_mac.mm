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

#include "qcorewlanengine_mac_p.h"
#include "qnetworkconfiguration_p.h"

#include <QtCore/qthread.h>
#include <QtCore/qmutex.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qstringlist.h>

#include <QtCore/qdebug.h>

#if defined(MAC_SDK_10_6) //not much functionality without this
#include <CoreWLAN/CoreWLAN.h>
#include <CoreWLAN/CWInterface.h>
#include <CoreWLAN/CWNetwork.h>
#include <CoreWLAN/CWNetwork.h>
#endif

#include <Foundation/NSEnumerator.h>
#include <Foundation/NSKeyValueObserving.h>
#include <Foundation/NSAutoreleasePool.h>

#include <SystemConfiguration/SCNetworkConfiguration.h>
QMap <QString, QString> networkInterfaces;

QT_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QCoreWlanEngine, coreWlanEngine)

inline QString cfstringRefToQstring(CFStringRef cfStringRef) {
//    return QString([cfStringRef UTF8String]);
    QString retVal;
    CFIndex maxLength = 2 * CFStringGetLength(cfStringRef) + 1/*zero term*/; // max UTF8
    char *cstring = new char[maxLength];
    if (CFStringGetCString(CFStringRef(cfStringRef), cstring, maxLength, kCFStringEncodingUTF8)) {
        retVal = QString::fromUtf8(cstring);
    }
    delete cstring;
    return retVal;
}

inline CFStringRef qstringToCFStringRef(const QString &string)
{
    return CFStringCreateWithCharacters(0, reinterpret_cast<const UniChar *>(string.unicode()),
                                        string.length());
}

inline NSString *qstringToNSString(const QString &qstr)
{ return [reinterpret_cast<const NSString *>(qstringToCFStringRef(qstr)) autorelease]; }

inline QString nsstringToQString(const NSString *nsstr)
{ return cfstringRefToQstring(reinterpret_cast<const CFStringRef>(nsstr)); }

inline QStringList nsarrayToQStringList(void *nsarray)
{
    QStringList result;
    NSArray *array = static_cast<NSArray *>(nsarray);
    for (NSUInteger i=0; i<[array count]; ++i)
        result << nsstringToQString([array objectAtIndex:i]);
    return result;
}

static QString qGetInterfaceType(const QString &interfaceString)
{
    return networkInterfaces.value(interfaceString);
}

QCoreWlanEngine::QCoreWlanEngine(QObject *parent)
:   QNetworkSessionEngine(parent)
{
    getAllScInterfaces();
    connect(&pollTimer, SIGNAL(timeout()), this, SIGNAL(configurationsChanged()));
    pollTimer.setInterval(10000);
}

QCoreWlanEngine::~QCoreWlanEngine()
{
}

QList<QNetworkConfigurationPrivate *> QCoreWlanEngine::getWifiConfigurations()
{
    QMapIterator<QString, QString> i(networkInterfaces);
    while (i.hasNext()) {
        i.next();
        QString interfaceName = i.key();
        if (i.value() == "WLAN") {
            if(!isWifiReady(interfaceName)) {
                qWarning() << "wifi not powered on";
                return QList<QNetworkConfigurationPrivate *>();
            } else {
           //     QList<QNetworkConfigurationPrivate *> profs = getWlanProfiles(interfaceName);
                scanForSsids(interfaceName);
            }
        } else {

        }
    }
    return QList<QNetworkConfigurationPrivate *> ();
}

QList<QNetworkConfigurationPrivate *> QCoreWlanEngine::getConfigurations(bool *ok)
{
    if (ok)
        *ok = true;
    NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];

    QList<QNetworkConfigurationPrivate *> foundConfigurations;
    QList<QNetworkConfigurationPrivate *> wificonfigs = getWifiConfigurations();

    uint identifier;
    QMapIterator<QString, QString> i(networkInterfaces);
    while (i.hasNext()) {
        i.next();
        if (i.value() == "WLAN") {
            QList<QNetworkConfigurationPrivate *> fetchedConfigurations = scanForSsids(i.key());
            for (int i = 0; i < fetchedConfigurations.count(); ++i) {

                QNetworkConfigurationPrivate *config = new QNetworkConfigurationPrivate;
                config->name = fetchedConfigurations.at(i)->name;
                config->isValid = fetchedConfigurations.at(i)->isValid;
                config->id = fetchedConfigurations.at(i)->id;

                config->state = fetchedConfigurations.at(i)->state;
                config->type = fetchedConfigurations.at(i)->type;
                config->roamingSupported = fetchedConfigurations.at(i)->roamingSupported;
                config->purpose = fetchedConfigurations.at(i)->purpose;
                config->internet = fetchedConfigurations.at(i)->internet;
                config->serviceInterface = fetchedConfigurations.at(i)->serviceInterface;

                identifier = config->name.toUInt();
                configurationInterface[identifier] =  config->serviceInterface.name();
                foundConfigurations.append(config);
            }
        }

        QNetworkInterface interface = QNetworkInterface::interfaceFromName(i.key());
        QNetworkConfigurationPrivate *cpPriv = new QNetworkConfigurationPrivate;
        const QString humanReadableName = interface.humanReadableName();
        cpPriv->name = humanReadableName.isEmpty() ? interface.name() : humanReadableName;
        cpPriv->isValid = true;

        if (interface.index())
            identifier = interface.index();
        else
            identifier = qHash(interface.hardwareAddress());

        cpPriv->id = QString::number(identifier);
        cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
        cpPriv->state = QNetworkConfiguration::Undefined;

        if (interface.flags() & QNetworkInterface::IsRunning) {
            cpPriv->state = QNetworkConfiguration::Defined;
            cpPriv->internet = true;
        }
        if ( !interface.addressEntries().isEmpty())  {
            cpPriv->state |= QNetworkConfiguration::Active;
            cpPriv->internet = true;
        }
        configurationInterface[identifier] = interface.name();
        foundConfigurations.append(cpPriv);
    }
    [autoreleasepool release];
    pollTimer.start();
    return foundConfigurations;
}

QString QCoreWlanEngine::getInterfaceFromId(const QString &id)
{
    return configurationInterface.value(id.toUInt());
}

bool QCoreWlanEngine::hasIdentifier(const QString &id)
{
    return configurationInterface.contains(id.toUInt());
}

QString QCoreWlanEngine::bearerName(const QString &id)
{
    QString interface = getInterfaceFromId(id);

    if (interface.isEmpty())
        return QString();

    return qGetInterfaceType(interface);
}

void QCoreWlanEngine::connectToId(const QString &id)
{
    NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];
    QString interfaceString = getInterfaceFromId(id);

    if(networkInterfaces.value(interfaceString) == "WLAN") {
#if defined(MAC_SDK_10_6)
        CWInterface *wifiInterface = [CWInterface interfaceWithName: qstringToNSString(interfaceString)];
        CWConfiguration *userConfig = [ wifiInterface configuration];

        NSSet *remNets = [userConfig rememberedNetworks]; //CWWirelessProfile

        NSEnumerator *enumerator = [remNets objectEnumerator];
        CWWirelessProfile *wProfile;
        NSUInteger index=0;
        while ((wProfile = [enumerator nextObject])) { //CWWirelessProfile

            if(id == nsstringToQString([wProfile ssid])) {
                CW8021XProfile *user8021XProfile = nil;
                user8021XProfile = [ wProfile user8021XProfile];

                NSError *err = nil;
                NSMutableDictionary *params = [NSMutableDictionary dictionaryWithCapacity:0];

                if(user8021XProfile) {
                    [params setValue: user8021XProfile forKey:kCWAssocKey8021XProfile];
                } else {
                    [params setValue: [wProfile passphrase] forKey: kCWAssocKeyPassphrase];
                }

               NSDictionary *parametersDict = nil;
                NSArray* apArray = [NSMutableArray arrayWithArray:[wifiInterface scanForNetworksWithParameters:parametersDict error:&err]];
                if(!err) {
                    for(uint row=0; row < [apArray count]; row++ ) {
                        CWNetwork *apNetwork = [apArray objectAtIndex:row];
                        if([[apNetwork ssid] compare:[wProfile ssid]] == NSOrderedSame) {
                            bool result = [wifiInterface associateToNetwork: apNetwork parameters:[NSDictionary dictionaryWithDictionary:params] error:&err];
                            if(!result) {
                                qWarning() <<"ERROR"<< nsstringToQString([err localizedDescription ]);
                                emit connectionError(id, ConnectError);
                            } else {
                                [autoreleasepool release];
                                return;
                            }
                        }
                    }
                }
            }
            index++;
        }
        emit connectionError(id, InterfaceLookupError);
#endif
    } else {
        // not wifi
    }
    emit connectionError(id, OperationNotSupported);
        [autoreleasepool release];
}

void QCoreWlanEngine::disconnectFromId(const QString &id)
{
    QString interfaceString = getInterfaceFromId(id);
    if(networkInterfaces.value(getInterfaceFromId(id)) == "WLAN") { //wifi only for now
#if defined(MAC_SDK_10_6)
        NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];
        CWInterface *wifiInterface = [CWInterface interfaceWithName:  qstringToNSString(interfaceString)];
        [wifiInterface disassociate];
        if([[wifiInterface interfaceState]intValue] != kCWInterfaceStateInactive) {
            emit connectionError(id, DisconnectionError);
        }
       [autoreleasepool release];
       return;
#endif
    } else {

    }
    emit connectionError(id, OperationNotSupported);
}

void QCoreWlanEngine::requestUpdate()
{
    getAllScInterfaces();
    emit configurationsChanged();
}

QCoreWlanEngine *QCoreWlanEngine::instance()
{
    return coreWlanEngine();
}

QList<QNetworkConfigurationPrivate *> QCoreWlanEngine::scanForSsids(const QString &interfaceName)
{
    QList<QNetworkConfigurationPrivate *> foundConfigs;
#if defined(MAC_SDK_10_6)
    NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];

    CWInterface *currentInterface = [CWInterface interfaceWithName:qstringToNSString(interfaceName)];
    NSError *err = nil;
    NSDictionary *parametersDict = nil;
    NSArray* apArray = [NSMutableArray arrayWithArray:[currentInterface scanForNetworksWithParameters:parametersDict error:&err]];

    if(!err) {
        for(uint row=0; row < [apArray count]; row++ ) {
            CWNetwork *apNetwork = [apArray objectAtIndex:row];
            QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
            QString networkSsid = nsstringToQString([apNetwork ssid]);
            cpPriv->name = networkSsid;
            cpPriv->isValid = true;
            cpPriv->id = networkSsid;
            cpPriv->internet = true;
            cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
            cpPriv->serviceInterface = QNetworkInterface::interfaceFromName(nsstringToQString([[CWInterface interface]  name]));

            CWWirelessProfile *networkProfile = apNetwork.wirelessProfile;
            CW8021XProfile *userNetworkProfile = networkProfile.user8021XProfile;
            if(!userNetworkProfile) {
            } else {
                qWarning() <<"Has profile!" ;
            }

            if( [currentInterface.interfaceState intValue] == kCWInterfaceStateRunning) {
                QString interfaceSsidString = nsstringToQString( [currentInterface ssid]);
                if( cpPriv->name == interfaceSsidString) {
                    cpPriv->state |=  QNetworkConfiguration::Active;
                }
            } else {
                if(isKnownSsid(cpPriv->serviceInterface.name(), networkSsid)) {
                    cpPriv->state =  QNetworkConfiguration::Discovered;
                } else {
                    cpPriv->state =  QNetworkConfiguration::Defined;
                }
            }
            if(!cpPriv->state) {
                cpPriv->state = QNetworkConfiguration::Undefined;
            }
            if([[apNetwork securityMode ] intValue]== kCWSecurityModeOpen)
                cpPriv->purpose = QNetworkConfiguration::Public;
            else
                cpPriv->purpose = QNetworkConfiguration::Private;
            foundConfigs.append(cpPriv);
        }
    } else {
        qWarning() << "ERROR scanning for ssids" << nsstringToQString([err localizedDescription])
                <<nsstringToQString([err domain]);
    }
    [autoreleasepool release];
#else
    Q_UNUSED(interfaceName);
#endif
    return foundConfigs;
}

bool QCoreWlanEngine::isWifiReady(const QString &wifiDeviceName)
{
#if defined(MAC_SDK_10_6)
    CWInterface *defaultInterface = [CWInterface interfaceWithName: qstringToNSString(wifiDeviceName)];
    if([defaultInterface power])
        return true;
#else
    Q_UNUSED(wifiDeviceName);
#endif
    return false;
}

bool QCoreWlanEngine::isKnownSsid(const QString &interfaceName, const QString &ssid)
{
#if defined(MAC_SDK_10_6)
    CWInterface *wifiInterface = [CWInterface interfaceWithName: qstringToNSString(interfaceName)];
    CWConfiguration *userConfig = [wifiInterface configuration];
    NSSet *remNets = [userConfig rememberedNetworks];
    for (CWWirelessProfile *wProfile in remNets) {
        if(ssid == nsstringToQString([wProfile ssid]))
            return true;
    }
#else
    Q_UNUSED(interfaceName);
    Q_UNUSED(ssid);
#endif
    return false;
}

QList<QNetworkConfigurationPrivate *> QCoreWlanEngine::getWlanProfiles(const QString &interfaceName)
{
    Q_UNUSED(interfaceName)
#if defined(MAC_SDK_10_6)
//    for( CW8021XProfile *each8021XProfile in [CW8021XProfile allUser8021XProfiles] ) {
//        qWarning() << "Profile name" << nsstringToQString([each8021XProfile ssid]);
//    }

#endif
    return QList<QNetworkConfigurationPrivate *> ();
}

bool QCoreWlanEngine::getAllScInterfaces()
{
    networkInterfaces.clear();
    NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];

    CFArrayRef interfaces = SCNetworkInterfaceCopyAll();
    if (interfaces != NULL) {
        CFIndex interfaceCount;
        CFIndex interfaceIndex;
        interfaceCount = CFArrayGetCount(interfaces);
        for (interfaceIndex = 0; interfaceIndex < interfaceCount; interfaceIndex++) {

            CFStringRef bsdName;
            CFTypeRef thisInterface = CFArrayGetValueAtIndex(interfaces, interfaceIndex);
            bsdName = SCNetworkInterfaceGetBSDName((SCNetworkInterfaceRef)thisInterface);
            QString interfaceName = cfstringRefToQstring(bsdName);
            QString typeStr;
            CFStringRef type = SCNetworkInterfaceGetInterfaceType((SCNetworkInterfaceRef)thisInterface);
            if ( CFEqual(type, kSCNetworkInterfaceTypeIEEE80211)) {
                typeStr = "WLAN";
//            } else if (CFEqual(type, kSCNetworkInterfaceTypeBluetooth)) {
//                typeStr = "Bluetooth";
            } else if(CFEqual(type, kSCNetworkInterfaceTypeEthernet)) {
                typeStr = "Ethernet";
            } else if(CFEqual(type, kSCNetworkInterfaceTypeFireWire)) {
                typeStr = "Ethernet"; //ok a bit fudged
            }
            if(!networkInterfaces.contains(interfaceName) && !typeStr.isEmpty()) {
                networkInterfaces.insert(interfaceName,typeStr);
            }
        }
    }
    CFRelease(interfaces);

    [autoreleasepool release];
    return true;
}


QT_END_NAMESPACE
