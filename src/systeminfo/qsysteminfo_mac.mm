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

#include "qsysteminfocommon_p.h"
#include "qsysteminfo_mac_p.h"

#include <QStringList>
#include <QSize>
#include <QFile>
#include <QTextStream>
#include <QLocale>
#include <QLibraryInfo>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QSettings>
#include <QSysInfo>
#include <QNetworkInterface>
#include <QList>
#include <QSettings>
#include <QDir>
#include <QNetworkInterface>
#include <QString>
#include <QHostInfo>
#include <QCryptographicHash>

#include <locale.h>

#include <IOBluetooth/IOBluetooth.h>

#include <SystemConfiguration/SystemConfiguration.h>
#include <CoreFoundation/CFArray.h>
#include <CoreFoundation/CFArray.h>
#include <CoreFoundation/CFNumber.h>
#include <CoreFoundation/CFNotificationCenter.h>
#include <IOKit/graphics/IOGraphicsLib.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFLocale.h>
#include <ScreenSaver/ScreenSaverDefaults.h>
#include <DiskArbitration/DiskArbitration.h>
#include <ApplicationServices/ApplicationServices.h>
#include <DiskArbitration/DASession.h>

#include <mach/mach.h>

#include <dns_sd.h>

#include <QTKit/QTKit.h>

#include <IOKit/usb/IOUSBLib.h>
#include <IOKit/pwr_mgt/IOPM.h>
#include <IOKit/ps/IOPSKeys.h>
#include <IOKit/ps/IOPowerSources.h>
#include <IOKit/IOTypes.h>

#include <IOKit/storage/IOMedia.h>
#include <IOKit/storage/IOCDMedia.h>
#include <IOKit/storage/IODVDMedia.h>
#include <IOKit/storage/IOBlockStorageDevice.h>

#include <IOKit/IOKitLib.h>
#include <IOKit/hid/IOHIDLib.h>

#include <CoreServices/CoreServices.h>

#include <qabstracteventdispatcher.h>

#include <QtCore/qthread.h>
#include <QtCore/qmutex.h>
#include <QEventLoop>

#ifdef MAC_SDK_10_6

#include <CoreLocation/CLLocation.h>
#include <CoreLocation/CLLocationManager.h>
#include <CoreWLAN/CWInterface.h>
#include <CoreWLAN/CWGlobals.h>
#else
// 10.5 sdk is broken for this:
#include </Developer/SDKs/MacOSX10.5.sdk/System/Library/Frameworks/CoreServices.framework/Frameworks/OSServices.framework/Headers/Power.h>
#endif

#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/cdefs.h>
#include <sys/param.h>
#include <sys/ucred.h>
#include <sys/mount.h>
#include <math.h>
#include <net/if.h>

#include <net/if_types.h>
#include <net/if_media.h>
#include <sys/ioctl.h>
#include <sys/socket.h>


static QString stringFromCFString(CFStringRef value) {
    QString retVal;
    if(CFStringGetLength(value) > 1) {
        CFIndex maxLength = 2 * CFStringGetLength(value) + 1/*zero term*/; // max UTF8
        char *cstring = new char[maxLength];
        if (CFStringGetCString(CFStringRef(value), cstring, maxLength, kCFStringEncodingUTF8)) {
            retVal = QString::fromUtf8(cstring);
        }
        delete cstring;
    }
    return retVal;
}

inline CFStringRef qstringToCFStringRef(const QString &string)
{
    return CFStringCreateWithCharacters(0, reinterpret_cast<const UniChar *>(string.unicode()),
                                        string.length());
}

inline QString nsstringToQString(const NSString *nsstr)
{ return QString([nsstr UTF8String]);}

inline NSString *qstringToNSString(const QString &qstr)
{ return [reinterpret_cast<const NSString *>(qstringToCFStringRef(qstr)) autorelease]; }

inline QStringList nsarrayToQStringList(void *nsarray)
{
    NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];
    QStringList result;
    NSArray *array = static_cast<NSArray *>(nsarray);
    for (NSUInteger i=0; i<[array count]; ++i)
        result << nsstringToQString([array objectAtIndex:i]);
    [autoreleasepool release];
    return result;
}

bool hasIOServiceMatching(const QString &classstr)
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    io_iterator_t ioIterator = NULL;
    IOServiceGetMatchingServices(kIOMasterPortDefault, IOServiceNameMatching(classstr.toAscii()), &ioIterator);
    if(ioIterator) {
        [pool drain];
        return true;
    }
    [pool drain];
    return false;
}



#ifdef MAC_SDK_10_6

@interface QtMNSListener : NSObject
{
    NSNotificationCenter *center;
    CWInterface * currentInterface;
}
- (void)notificationHandler:(NSNotification *)notification;
- (void)remove;
@end

@implementation QtMNSListener
- (id) init
{
   [super init];
    center = [NSNotificationCenter defaultCenter];
    currentInterface = [CWInterface interfaceWithName:nil];

    [center addObserver:self selector:@selector(notificationHandler:) name:kCWModeDidChangeNotification object:nil];
    [center addObserver:self selector:@selector(notificationHandler:) name:kCWSSIDDidChangeNotification object:nil];
    [center addObserver:self selector:@selector(notificationHandler:) name:kCWBSSIDDidChangeNotification object:nil];
    [center addObserver:self selector:@selector(notificationHandler:) name:kCWCountryCodeDidChangeNotification object:nil];
    [center addObserver:self selector:@selector(notificationHandler:) name:kCWLinkDidChangeNotification object:nil];
    [center addObserver:self selector:@selector(notificationHandler:) name:kCWPowerDidChangeNotification object:nil];


    return self;
}

-(void)dealloc
{
   [center release];
   [super dealloc];
}

-(void)remove
{
    [center removeObserver:self];
}

- (void)notificationHandler:(NSNotification *)notification
{
    QTM_NAMESPACE::QSystemNetworkInfoPrivate::instance()->wifiNetworkChanged( nsstringToQString([notification name]), nsstringToQString([[notification object]name]));
}
@end


@interface QtMLangListener : NSObject
{
    NSNotificationCenter *center;
    QString currentLanguage;
}
- (void)languageHandler;//:(NSNotification *)notification;
- (void)remove;
- (void)getCurrentLanguage;
@end



@implementation QtMLangListener
- (id) init
{
    [super init];
    center = [NSNotificationCenter defaultCenter];
    [center addObserver:self selector:@selector(languageHandler:) name:NSCurrentLocaleDidChangeNotification object:nil];
    [self getCurrentLanguage];
    return self;
}

-(void)dealloc
{
//    [center release];
    [super dealloc];
}

-(void)remove
{
    [center removeObserver:self];
}

- (void)getCurrentLanguage
{
    NSUserDefaults* defs = [NSUserDefaults standardUserDefaults];
    NSArray* languages = [defs objectForKey:@"AppleLanguages"];
    NSString* language = [languages objectAtIndex:0];

    QString langString = nsstringToQString(language);
    if(langString != currentLanguage) {
        if(!currentLanguage.isEmpty())
            QTM_NAMESPACE::QSystemInfoPrivate::instance()->languageChanged(currentLanguage);
        currentLanguage = langString;
    }
}

- (void)languageHandler;//:(NSNotification *)notification
{
    [self getCurrentLanguage];
}
@end


@interface RemoteDeviceRSSIHostControllerDelegate : NSObject
{
}
// See IOBluetoothHostControllerDelegate
- (void)readRSSIForDeviceComplete:(id)controller device:(IOBluetoothDevice*)device info:(BluetoothHCIRSSIInfo*)info error:(IOReturn)error;
@end

@implementation RemoteDeviceRSSIHostControllerDelegate
- (id) init
{
   [super init];
   return self;
}

- (void)readRSSIForDeviceComplete:(id)controller device:(IOBluetoothDevice*)device info:(BluetoothHCIRSSIInfo*)info error:(IOReturn)error
{
    Q_UNUSED(controller);
    Q_UNUSED(device);

    if ((error != kIOReturnSuccess) || (info == NULL)) {
        qDebug() << "ERROR: readRSSIForDeviceComplete return error";

    } else if (info->handle == kBluetoothConnectionHandleNone) {
        qDebug() << "ERROR: readRSSIForDeviceComplete no connection";
    } else {
        NSLog(@"Rssi value: %@", info->RSSIValue);
    }
}
@end
#endif
NSObject* delegate;

@interface QtBtConnectListener : NSObject
{
}
@end

@implementation QtBtConnectListener
- (id) init
{
    [super init];
    [IOBluetoothDevice registerForConnectNotifications:self selector:@selector(bluetoothDeviceConnected:device:)];
    return self;
}

- (void)bluetoothDeviceConnected:(IOBluetoothUserNotification *)notification device:(IOBluetoothDevice *)device
{
    Q_UNUSED(notification);

    if([ device getClassOfDevice ] == 9536) {
        QTM_NAMESPACE::QSystemDeviceInfoPrivate::instance()->keyboardConnected(true);
        [device registerForDisconnectNotification:self selector:@selector(bluetoothDeviceDisconnected:device:)];
    }
}

- (void)bluetoothDeviceDisconnected:(IOBluetoothUserNotification *)notification device:(IOBluetoothDevice *)device
{
    Q_UNUSED(notification);
    if([ device getClassOfDevice ] == 9536) {
        QTM_NAMESPACE::QSystemDeviceInfoPrivate::instance()->keyboardConnected(false);
    }
}
@end

QTM_BEGIN_NAMESPACE
QtBtConnectListener *btConnListener;

Q_GLOBAL_STATIC(QSystemDeviceInfoPrivate, qsystemDeviceInfoPrivate)

QSystemInfoPrivate *QSystemInfoPrivate::self = 0;

QSystemInfoPrivate::QSystemInfoPrivate(QObject *parent)
 : QObject(parent),langloopThread(0),langThreadOk(0)
{
    if(!self)
        self = this;
}

QSystemInfoPrivate::~QSystemInfoPrivate()
{
    if(langThreadOk && langloopThread->keepRunning) {
        langloopThread->stop();
        delete langloopThread;
    }
}

QString QSystemInfoPrivate::currentLanguage() const
{
 QString lang = QLocale::system().name().left(2);
    if(lang.isEmpty() || lang == QLatin1String("C")) {
        lang = QLatin1String("en");
    }
    return lang;
}

QStringList QSystemInfoPrivate::availableLanguages() const
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    NSUserDefaults* defs = [NSUserDefaults standardUserDefaults];
    NSArray* languages = [defs objectForKey:@"AppleLanguages"];
    QStringList langList = nsarrayToQStringList(languages);

    QStringList returnList;
    for(int i = 0; i < langList.count(); i++) {
     QString language = langList.at(i).left(2).toLower();
     if(!returnList.contains(language))
         returnList << language;
    }
    [pool drain];
    return returnList;
}

void QSystemInfoPrivate::languageChanged(const QString &lang)
{
    Q_EMIT currentLanguageChanged(lang);
}

void QSystemInfoPrivate::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(currentLanguageChanged(QString))) {
        langloopThread = new QLangLoopThread();
        langThreadOk = true;
    }
}

void QSystemInfoPrivate::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(currentLanguageChanged(QString))) {
        if(langloopThread->keepRunning) {
            langloopThread->stop();
        }
    }
}

QString QSystemInfoPrivate::version(QSystemInfo::Version type,  const QString &parameter)
{
    Q_UNUSED(parameter);
    QString errorStr = "Not Available";
    bool useDate = false;
    if(parameter == QLatin1String("versionDate")) {
        useDate = true;
    }
    switch(type) {
    case QSystemInfo::Os:
    {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        QString ver = nsstringToQString([[NSDictionary dictionaryWithContentsOfFile:@"/System/Library/CoreServices/SystemVersion.plist"] objectForKey:@"ProductVersion"]);
        [pool drain];
        return ver;
    }
        break;
    case QSystemInfo::QtCore:
       return  qVersion();
       break;
   case QSystemInfo::Firmware:
       {
           return QSystemDeviceInfoPrivate::model();
       }
       break;
    default:
        break;
    };
  return errorStr;
}


QString QSystemInfoPrivate::currentCountryCode() const
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    NSUserDefaults* defs = [NSUserDefaults standardUserDefaults];
    NSString* nslocale = [defs objectForKey:@"AppleLocale"];
    QString qtlocale = nsstringToQString(nslocale);

    [pool drain];
    return qtlocale.mid(3,2);
}


bool QSystemInfoPrivate::hasFeatureSupported(QSystemInfo::Feature feature)
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    bool featureSupported = false;
    switch (feature) {
    case QSystemInfo::BluetoothFeature:
        {
#ifdef  MAC_SDK_10_6
            IOBluetoothHostController* controller = [IOBluetoothHostController defaultController];
            if (controller != NULL) {
                featureSupported = true;
            }
#endif
        }
        break;
    case QSystemInfo::CameraFeature:
        {

           NSArray * videoDevices;
           videoDevices = [[QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeVideo] arrayByAddingObjectsFromArray:[QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeMuxed]];
           if([videoDevices count] > 0) {
               featureSupported = true;
           }
        }
        break;
    case QSystemInfo::FmradioFeature:
        break;
    case QSystemInfo::IrFeature:
        {
            if(hasIOServiceMatching("AppleIRController")) {
                featureSupported = true;
            }
        }
        break;
    case QSystemInfo::LedFeature:
        {
//kHIDPage_LEDs
        }
        break;
    case QSystemInfo::MemcardFeature:
        {
// IOSCSIPeripheralDeviceType0E
            if(hasIOServiceMatching("IOUSBMassStorageClass")) {
                featureSupported = true;
            }
        }
        break;
    case QSystemInfo::UsbFeature:
        {
            if(hasIOServiceMatching("AppleUSBOHCI")) {
                featureSupported = true;
            }
            if(hasIOServiceMatching("AppleUSBEHCI")) {
                featureSupported = true;
            }
        }
        break;
    case QSystemInfo::VibFeature:
        {
        }
        break;
    case QSystemInfo::WlanFeature:
        {
            if(!QSystemNetworkInfoPrivate::instance()->interfaceForMode(QSystemNetworkInfo::WlanMode).name().isEmpty()) {
                featureSupported = true;
            }
        }
        break;
    case QSystemInfo::SimFeature:
        {
        }
        break;
    case QSystemInfo::LocationFeature:
        {
#ifdef MAC_SDK_10_6
            CLLocationManager *locationManager = [[CLLocationManager alloc] init];
            if ([locationManager locationServicesEnabled]) {
                featureSupported = true;
            }
            [locationManager release];
#endif
        }
        break;
    case QSystemInfo::VideoOutFeature:
        {
            ComponentDescription description = {'vout', 0, 0, 0L, 1L << 0};
            if( ::CountComponents(&description) > 0) {
                featureSupported = true;
            }
        }
        break;
    case QSystemInfo::HapticsFeature:
        break;
    default:
        featureSupported = false;
        break;
    };
    [pool drain];
    return featureSupported;
}

QSystemNetworkInfoPrivate *QSystemNetworkInfoPrivate::self = 0;


void networkChangeCallback(SCDynamicStoreRef /*dynamicStore*/, CFArrayRef changedKeys, void */*networkConfigurationManagerPrivate*/)
{
// NSLog(@"changed keys %@", changedKeys);
    QStringList keyList = nsarrayToQStringList((void*)changedKeys);
    if(keyList.contains("State:/Network/Global/DNS")) {
    }
    if(keyList.contains("State:/Network/Global/IPv4")) {
        QTM_NAMESPACE::QSystemNetworkInfoPrivate::instance()->ethernetChanged();
        QTM_NAMESPACE::QSystemNetworkInfoPrivate::instance()->getDefaultInterface();
    }

    return;
}

#ifdef MAC_SDK_10_6
QtMLangListener *langListener;
#endif


QLangLoopThread::QLangLoopThread(QObject *parent)
    :QObject(parent)
{
    moveToThread(&t);
    t.start();
}

QLangLoopThread::~QLangLoopThread()
{
}

void QLangLoopThread::stop()
{
    keepRunning = false;
#ifdef MAC_SDK_10_6
    CFRunLoopStop(CFRunLoopGetCurrent());
    [langListener release];
#endif
}

void QLangLoopThread::doWork()
{
#ifdef MAC_SDK_10_6
    if(QThread::currentThread() != &t) {
        QMetaObject::invokeMethod(this, "doWork",
                                  Qt::QueuedConnection);
    } else {

        QMutexLocker locker(&mutex);
        locker.unlock();
        keepRunning = true;
        locker.relock();
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

        langListener = [[QtMLangListener alloc] init];
        SInt32 result;
        while (keepRunning &&
               (result = CFRunLoopRunInMode(kCFRunLoopDefaultMode ,2, YES))) {
        }
        [pool release];
    }
#endif
}

#ifdef MAC_SDK_10_6
QtMNSListener *listener;
#endif

QRunLoopThread::QRunLoopThread(QObject *parent)
    :QObject(parent)
{
    moveToThread(&t);
    t.start();
}

QRunLoopThread::~QRunLoopThread()
{
}

void QRunLoopThread::stop()
{
    QMutexLocker locker(&mutex);
    CFRunLoopStop(CFRunLoopGetCurrent());
    keepRunning = false;
//#ifdef MAC_SDK_10_6
//    [listener release];
//    [delegate release];
//#endif
    t.quit();
    t.wait();
}


void QRunLoopThread::doWork()
{
#ifdef MAC_SDK_10_6
    if(QThread::currentThread() != &t) {
        QMetaObject::invokeMethod(this, "doWork",
                                  Qt::QueuedConnection);
    } else {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        startNetworkChangeLoop();
        delegate = [[[RemoteDeviceRSSIHostControllerDelegate alloc] init] autorelease];

        mutex.lock();
        keepRunning = true;
        mutex.unlock();

        listener = [[[QtMNSListener alloc] init] autorelease];

        SInt32 result;
        while (keepRunning &&
               (result = CFRunLoopRunInMode(kCFRunLoopDefaultMode ,1, YES))) {
        }
        [pool release];
    }
#endif
}

void QRunLoopThread::startNetworkChangeLoop()
{
    storeSession = NULL;

    SCDynamicStoreContext dynStoreContext = { 0, this /*(void *)storeSession*/, NULL, NULL, NULL };
    storeSession = SCDynamicStoreCreate(NULL,
                                 CFSTR("networkChangeCallback"),
                                 networkChangeCallback,
                                 &dynStoreContext);
    if (!storeSession ) {
        qDebug()<< "could not open dynamic store: error:" << SCErrorString(SCError());
        return;
    }

    CFMutableArrayRef notificationKeys;
    notificationKeys = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
    CFMutableArrayRef patternsArray;
    patternsArray = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);

    CFStringRef storeKey;

    storeKey = SCDynamicStoreKeyCreateNetworkGlobalEntity(NULL,
                                                     kSCDynamicStoreDomainState,
                                                     kSCEntNetLink);
    CFArrayAppendValue(notificationKeys, storeKey);
    CFRelease(storeKey);

    storeKey = SCDynamicStoreKeyCreateNetworkServiceEntity(NULL,
                                                      kSCDynamicStoreDomainState,
                                                      kSCCompAnyRegex,
                                                      kSCEntNetLink);
    CFArrayAppendValue(patternsArray, storeKey);
    CFRelease(storeKey);

    storeKey = SCDynamicStoreKeyCreateNetworkGlobalEntity(NULL,
                                                     kSCDynamicStoreDomainState,
                                                     kSCEntNetDNS);
    CFArrayAppendValue(notificationKeys, storeKey);
    CFRelease(storeKey);

    storeKey = SCDynamicStoreKeyCreateNetworkServiceEntity(NULL,
                                                      kSCDynamicStoreDomainState,
                                                      kSCCompAnyRegex,
                                                      kSCEntNetDNS);
    CFArrayAppendValue(patternsArray, storeKey);
    CFRelease(storeKey);

    storeKey = SCDynamicStoreKeyCreateNetworkGlobalEntity(NULL,
                                                     kSCDynamicStoreDomainState,
                                                     kSCEntNetIPv4);
    CFArrayAppendValue(notificationKeys, storeKey);
    CFRelease(storeKey);

    storeKey = SCDynamicStoreKeyCreateNetworkServiceEntity(NULL,
                                                      kSCDynamicStoreDomainState,
                                                      kSCCompAnyRegex,
                                                      kSCEntNetIPv4);
    CFArrayAppendValue(patternsArray, storeKey);
    CFRelease(storeKey);

    if (!SCDynamicStoreSetNotificationKeys(storeSession , notificationKeys, patternsArray)) {
        qDebug()<< "register notification error:"<< SCErrorString(SCError());
        CFRelease(storeSession );
        CFRelease(notificationKeys);
        CFRelease(patternsArray);
        return;
    }
    CFRelease(notificationKeys);
    CFRelease(patternsArray);

    runloopSource = SCDynamicStoreCreateRunLoopSource(NULL, storeSession , 0);
    if (!runloopSource) {
        qDebug()<< "runloop source error:"<< SCErrorString(SCError());
        CFRelease(storeSession);
        return;
    }

    CFRunLoopAddSource(CFRunLoopGetCurrent(), runloopSource, kCFRunLoopDefaultMode);
    CFRelease(runloopSource);
    CFRelease(storeSession);
    return;
}


QDASessionThread::QDASessionThread(QObject *parent)
    :QObject(parent), session(NULL)
{
    moveToThread(&t);
    t.start();
    if(session == NULL)
        session = DASessionCreate(kCFAllocatorDefault);
}

QDASessionThread::~QDASessionThread()
{
}

void QDASessionThread::stop()
{
    QMutexLocker locker(&mutex);
    CFRunLoopStop(CFRunLoopGetCurrent());
    keepRunning = false;
    t.quit();
    t.wait();
}

void QDASessionThread::doWork()
{
#ifdef MAC_SDK_10_6
    if(QThread::currentThread() != &t) {
        QMetaObject::invokeMethod(this, "doWork",
                                  Qt::QueuedConnection);
    } else {

        mutex.lock();
        keepRunning = true;
        mutex.unlock();
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

        DASessionScheduleWithRunLoop(session,CFRunLoopGetCurrent(),kCFRunLoopDefaultMode);

        SInt32 result;
        while (keepRunning &&
               (result = CFRunLoopRunInMode(kCFRunLoopDefaultMode ,1, YES))) {
        }

        DASessionUnscheduleFromRunLoop(session, CFRunLoopGetCurrent(),kCFRunLoopDefaultMode);
        [pool release];
    }
#endif
}

static bool isBtPowerOn()
{
    CFMutableDictionaryRef matching = NULL;
    CFMutableDictionaryRef btDictionary = NULL;
    io_registry_entry_t entry = 0;
    matching = IOServiceMatching("IOBluetoothHCIController");
    entry = IOServiceGetMatchingService(kIOMasterPortDefault,matching);
    IORegistryEntryCreateCFProperties(entry, &btDictionary,NULL,0);
    bool powerOn = false;

    if ([[(NSDictionary*)btDictionary objectForKey:@"HCIControllerPowerIsOn"] boolValue]) {
        powerOn = true;
    }
    CFRelease(btDictionary);
    return powerOn;
}

void btPowerStateChange(void *ref, io_service_t /*service*/, natural_t messageType, void */*info*/)
{
    QBluetoothListenerThread * thread = reinterpret_cast< QBluetoothListenerThread *>(ref);
    switch (messageType) {
    case kIOMessageDeviceWillPowerOff:
        {
            if(!isBtPowerOn())
                thread->emitBtPower(false);
        }
        break;
    case kIOMessageDeviceHasPoweredOn:
        {
            if(isBtPowerOn())
                thread->emitBtPower(true);
        }
        break;
    }
}



QBluetoothListenerThread::QBluetoothListenerThread(QObject *parent)
    :QObject(parent)
{
    moveToThread(&t);
    t.start();
}

QBluetoothListenerThread::~QBluetoothListenerThread()
{
}

void QBluetoothListenerThread::stop()
{
    mutex.lock();
    keepRunning = false;
    mutex.unlock();
    [btConnListener release];
    t.quit();
    t.wait();
}

void QBluetoothListenerThread::doWork()
{
#ifdef MAC_SDK_10_6
    if(QThread::currentThread() != &t) {
        QMetaObject::invokeMethod(this, "doWork",
                                  Qt::QueuedConnection);
    } else {
        mutex.lock();
        keepRunning = true;
        mutex.unlock();

        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

        io_object_t notifyObject;
        io_service_t bluetoothservice;

        io_iterator_t ioIterator;
        mach_port_t masterPort;
        CFMutableDictionaryRef serviceMatchDict;

        if (0 != IOMasterPort(MACH_PORT_NULL, &masterPort)) {
            qDebug() << "IOMasterPort failed";
        }

        serviceMatchDict = IOServiceMatching("IOBluetoothHCIController");
        if (NULL == serviceMatchDict) {
            qDebug() << "IOServiceMatching failed";
        }

        if (0 != IOServiceGetMatchingServices(masterPort, serviceMatchDict, &ioIterator)) {
            qDebug() << "IOServiceGetMatchingServices failed";
        }

        IOReturn ret;

        bluetoothservice = IOIteratorNext(ioIterator);
        if (0 == bluetoothservice) {
            IOObjectRelease(ioIterator);
            qDebug() << "IOIteratorNext failed";
        }
        IOObjectRelease(ioIterator);

        port = IONotificationPortCreate(masterPort);
        if (0 == port) {
            qDebug() << "IONotificationPortCreate failed";
        }

        ret = IOServiceAddInterestNotification(port, bluetoothservice,
                                               kIOGeneralInterest, btPowerStateChange,
                                               this, &notifyObject);
        if(ret != kIOReturnSuccess) {
            qDebug() << "IOServiceAddInterestNotification failed";
            return;
        }

        rl = CFRunLoopGetCurrent();
        rls = IONotificationPortGetRunLoopSource(port);

        CFRunLoopAddSource(rl,
                           rls,
                           kCFRunLoopDefaultMode);
        SInt32 result;
        while (keepRunning &&
               (result = CFRunLoopRunInMode(kCFRunLoopDefaultMode ,1, NO))) {
        }

        CFRunLoopStop(rl);

        IOObjectRelease(bluetoothservice);
        CFRunLoopRemoveSource(rl,
                              rls,
                              kCFRunLoopDefaultMode);
        IONotificationPortDestroy(port);
        [pool release];
    }
#endif
}

void QBluetoothListenerThread::setupConnectNotify()
{
    btConnListener = [[QtBtConnectListener alloc] init];
}

void QBluetoothListenerThread::emitBtPower(bool b)
{
    Q_EMIT bluetoothPower(b);
}

QSystemNetworkInfoPrivate::QSystemNetworkInfoPrivate(QObject *parent)
        : QObject(parent), signalStrengthCache(0),networkThreadOk(0)
{
    defaultInterface = "";
    qRegisterMetaType<QSystemNetworkInfo::NetworkMode>("QSystemNetworkInfo::NetworkMode");
    qRegisterMetaType<QSystemNetworkInfo::NetworkStatus>("QSystemNetworkInfo::NetworkStatus");

#ifdef MAC_SDK_10_6
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
if([[CWInterface supportedInterfaces] count] > 0 ) {
        hasWifi = true;
    } else {
        hasWifi = false;
    }
 [pool release];
#endif
    rssiTimer = new QTimer(this);

    if(!self)
        self = this;
    QTimer::singleShot(200, this, SLOT(primaryInterface()));
}

QSystemNetworkInfoPrivate::~QSystemNetworkInfoPrivate()
{
#ifdef MAC_SDK_10_6
    if(hasWifi && networkThreadOk && runloopThread->keepRunning) {
        runloopThread->stop();
        delete runloopThread;
   }
#endif
}

void QSystemNetworkInfoPrivate::primaryInterface()
{
    defaultInterface = getDefaultInterface();
}

void QSystemNetworkInfoPrivate::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int))) {
        connect(rssiTimer, SIGNAL(timeout()), this, SLOT(rssiTimeout()));
        rssiTimer->start(5000);
    }
    if (QLatin1String(signal) == SIGNAL(networkNameChanged(QSystemNetworkInfo::NetworkMode,QString))
        || QLatin1String(signal) == SIGNAL(networkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus))) {
#ifdef MAC_SDK_10_6
        if(hasWifi) {
            runloopThread = new QRunLoopThread();
            runloopThread->doWork();
            networkThreadOk = true;
        }
#endif
    }
}

void QSystemNetworkInfoPrivate::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int))) {
        rssiTimer->stop();
        disconnect(rssiTimer, SIGNAL(timeout()), this, SLOT(rssiTimeout()));
    }
    if (QLatin1String(signal) == SIGNAL(networkNameChanged(QSystemNetworkInfo::NetworkMode,QString))
        || QLatin1String(signal) == SIGNAL(networkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus))) {
#ifdef MAC_SDK_10_6
        if(hasWifi && networkThreadOk) {
            runloopThread->stop();
            [delegate release];
        }
#endif
    }
}

QSystemNetworkInfo::NetworkMode QSystemNetworkInfoPrivate::modeForInterface(QString interfaceName)
{
    QSystemNetworkInfo::NetworkMode mode = QSystemNetworkInfo::UnknownMode;
    CFArrayRef interfaceArray = SCNetworkInterfaceCopyAll(); //10.4
    CFStringRef iName;
    CFStringRef type;

    for ( long i = 0; i < CFArrayGetCount(interfaceArray); i++) {
        SCNetworkInterfaceRef thisInterface =  (SCNetworkInterfaceRef ) CFArrayGetValueAtIndex(interfaceArray, i);
        type = SCNetworkInterfaceGetInterfaceType(thisInterface);
        iName = SCNetworkInterfaceGetBSDName(thisInterface);
        if( interfaceName == stringFromCFString(iName)) {
            if (type != NULL) {
                if (CFEqual(type, kSCNetworkInterfaceTypeBluetooth)) {
                    mode = QSystemNetworkInfo::BluetoothMode;
                    break;
                } else if (CFEqual(type, kSCNetworkInterfaceTypeEthernet)) {
                    mode = QSystemNetworkInfo::EthernetMode;
                    break;
                } else if (CFEqual(type, kSCNetworkInterfaceTypeIEEE80211)) {
                    mode = QSystemNetworkInfo::WlanMode;
                    break;
                }
            }
        }
    }
    CFRelease(interfaceArray);
    return mode;
}

QString QSystemNetworkInfoPrivate::getDefaultInterface()
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    SCDynamicStoreRef storeSession2;
    CFStringRef key;
    CFDictionaryRef	globalDict;
    CFStringRef primaryInterface = NULL;
    QString interfaceName;

    storeSession2 = SCDynamicStoreCreate(NULL, CFSTR("getPrimary"), NULL, NULL);
//    if (!storeSession2) {
//    }
    key = SCDynamicStoreKeyCreateNetworkGlobalEntity(NULL, kSCDynamicStoreDomainState,kSCEntNetIPv4);
    globalDict = (const __CFDictionary*)SCDynamicStoreCopyValue(storeSession2, key);
    CFRelease(key);
    if (globalDict) {

        primaryInterface = (CFStringRef )CFDictionaryGetValue(globalDict,
                                                                   kSCDynamicStorePropNetPrimaryInterface);
        if (primaryInterface) {
            CFRetain(primaryInterface);
        }
        CFRelease(globalDict);
    }

    CFRelease(storeSession2);
    if (primaryInterface) {
        interfaceName = stringFromCFString(primaryInterface);
        if(interfaceName != defaultInterface) {
            Q_EMIT networkModeChanged(modeForInterface(interfaceName));
             defaultInterface = interfaceName;
        }
    }
    [pool release];
    return interfaceName;
}

void QSystemNetworkInfoPrivate::rssiTimeout()
{
    networkSignalStrength(QSystemNetworkInfo::WlanMode);
}

bool QSystemNetworkInfoPrivate::isInterfaceActive(const char* netInterface)
{
    struct ifmediareq ifm;

    memset(&ifm, 0, sizeof(struct ifmediareq));
    strncpy(ifm.ifm_name, netInterface, IFNAMSIZ);
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    ioctl(s, SIOCGIFMEDIA, &ifm);
    if (ifm.ifm_status & IFM_ACTIVE) {
        return true;
    }
    return false;
}

void QSystemNetworkInfoPrivate::ethernetChanged()
{
    QSystemNetworkInfo::NetworkStatus status = QSystemNetworkInfo::NoNetworkAvailable;
    int carrier = 0;

    if(isInterfaceActive(interfaceForMode(QSystemNetworkInfo::EthernetMode).name().toLocal8Bit())) {
        status = QSystemNetworkInfo::Connected;
        carrier = 100;
    }
    Q_EMIT networkStatusChanged(QSystemNetworkInfo::EthernetMode,status);
    Q_EMIT networkSignalStrengthChanged(QSystemNetworkInfo::EthernetMode,carrier);
    Q_EMIT networkNameChanged(QSystemNetworkInfo::EthernetMode, networkName(QSystemNetworkInfo::EthernetMode));
    Q_EMIT networkModeChanged(modeForInterface(getDefaultInterface()));
}

QSystemNetworkInfo::NetworkStatus QSystemNetworkInfoPrivate::networkStatus(QSystemNetworkInfo::NetworkMode mode)
{
    QSystemNetworkInfo::NetworkStatus status = QSystemNetworkInfo::NoNetworkAvailable;
    switch(mode) {
        case QSystemNetworkInfo::GsmMode:
        break;
        case QSystemNetworkInfo::CdmaMode:
        break;
        case QSystemNetworkInfo::WcdmaMode:
        break;
    case QSystemNetworkInfo::WlanMode:
        {
#ifdef MAC_SDK_10_6
            if(hasWifi) {
                NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];
                CWInterface *wifiInterface = [CWInterface interfaceWithName:  qstringToNSString(interfaceForMode(mode).name())];

                if([wifiInterface power]) {
                    if(!rssiTimer->isActive())
                        rssiTimer->start(5000);
                }  else {
                    if(rssiTimer->isActive())
                        rssiTimer->stop();
                }

                switch([[wifiInterface interfaceState]intValue]) {
                case  kCWInterfaceStateInactive:
                    status = QSystemNetworkInfo::NoNetworkAvailable;
                    break;
                case kCWInterfaceStateScanning:
                case kCWInterfaceStateAuthenticating:
                case kCWInterfaceStateAssociating:
                    status = QSystemNetworkInfo::Searching;
                    break;
                case kCWInterfaceStateRunning:
                    status = QSystemNetworkInfo::Connected;
                    break;
                };
                [autoreleasepool release];
            }

#else
            if(isInterfaceActive(interfaceForMode(mode).name().toLatin1())) {
                status = QSystemNetworkInfo::Connected;
            }
#endif
        }
        break;
    case QSystemNetworkInfo::EthernetMode:
        {
            if(isInterfaceActive(interfaceForMode(mode).name().toLatin1())) {
                return QSystemNetworkInfo::Connected;
            } else {
                return QSystemNetworkInfo::NoNetworkAvailable;
            }
        }
        break;
        case QSystemNetworkInfo::BluetoothMode:
        {

        }
        break;
    case QSystemNetworkInfo::WimaxMode:
        break;
        default:
        break;
    };
    return status;
}

int QSystemNetworkInfoPrivate::networkSignalStrength(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {
        case QSystemNetworkInfo::GsmMode:
        break;
        case QSystemNetworkInfo::CdmaMode:
        break;
        case QSystemNetworkInfo::WcdmaMode:
        break;
    case QSystemNetworkInfo::WlanMode:
        {
            int signalQuality = 0;
            if(hasWifi) {
#ifdef MAC_SDK_10_6
                NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];
                QString name = interfaceForMode(mode).name();
                CWInterface *wifiInterface = [CWInterface interfaceWithName:qstringToNSString(name)];

                if([wifiInterface power]) {
                    if(!rssiTimer->isActive())
                        rssiTimer->start(5000);
                }  else {
                    if(rssiTimer->isActive())
                        rssiTimer->stop();
                }

                int rssiSignal = [[wifiInterface rssi] intValue];

                if(rssiSignal !=0 ) {
                    int maxRssi = -40;
                    int minRssi = [[wifiInterface noise] intValue];
                    signalQuality = ( 100 * (maxRssi - minRssi) * (maxRssi - minRssi) - (maxRssi - rssiSignal) *
                                      (15 * (maxRssi - minRssi) + 62 * (maxRssi - rssiSignal)) ) /
                                    ((maxRssi - minRssi) * (maxRssi - minRssi));

                } else {
                    signalQuality = 0;
                }

                if(signalStrengthCache != signalQuality) {
                    if(signalStrengthCache == 0) {
                        networkStatus(QSystemNetworkInfo::WlanMode);
                    }
                    signalStrengthCache = signalQuality;
                    Q_EMIT networkSignalStrengthChanged(mode, signalQuality);
                }
                [autoreleasepool release];
#endif
            }
            return signalQuality;
        }
        break;
    case QSystemNetworkInfo::EthernetMode:
        {
            int percent = (isInterfaceActive(interfaceForMode(mode).name().toLatin1())) ? 100 : 0;
            return percent;
        }
        break;
    case QSystemNetworkInfo::BluetoothMode:
        {
#ifdef  MAC_SDK_10_6
            NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
            IOBluetoothHostController* controller = IOBluetoothHostController.defaultController;

            if (controller != NULL) {

                NSArray *devices = [IOBluetoothDevice recentDevices:0];
                for ( IOBluetoothDevice *btDevice in devices ) {
                    if([btDevice isConnected]) {
                        qDebug()<<"IOBluetoothDevice connected"<< nsstringToQString([btDevice getName]);
//                        delegate = [[RemoteDeviceRSSIHostControllerDelegate alloc] init];
                        [delegate retain];
                        [controller setDelegate:delegate];
                        IOReturn rc = [controller readRSSIForDevice:btDevice];
                        if (rc != noErr) {
                            qDebug()<< "ERROR: call readRSSIForDevice failed";
                        }
//[delegate release];
                    }
                }
//              [devices release];
  //  [controller release];
            }
            [pool release];
#endif
        }
    case QSystemNetworkInfo::WimaxMode:
        break;
    default:
        break;
    };
    return -1;
}

int QSystemNetworkInfoPrivate::cellId()
{
    return -1;
}

int QSystemNetworkInfoPrivate::locationAreaCode()
{
    return -1;
}

QString QSystemNetworkInfoPrivate::currentMobileCountryCode()
{
    return "";
}

QString QSystemNetworkInfoPrivate::currentMobileNetworkCode()
{
    return "";
}

QString QSystemNetworkInfoPrivate::homeMobileCountryCode()
{
    return "";
}

QString QSystemNetworkInfoPrivate::homeMobileNetworkCode()
{
    return "";
}

QString QSystemNetworkInfoPrivate::networkName(QSystemNetworkInfo::NetworkMode mode)
{
    if(networkStatus(mode) == QSystemNetworkInfo::NoNetworkAvailable) {
        return "";
    }
    switch(mode) {
        case QSystemNetworkInfo::GsmMode:
        break;
        case QSystemNetworkInfo::CdmaMode:
        break;
        case QSystemNetworkInfo::WcdmaMode:
        break;
    case QSystemNetworkInfo::WlanMode:
        {
            QString name = interfaceForMode(mode).name();
            NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
#ifdef MAC_SDK_10_6
            if(hasWifi) {
                CWInterface *wifiInterface = [CWInterface interfaceWithName:qstringToNSString(name)];
                QString netname = nsstringToQString([wifiInterface ssid]);
                [pool release];
                return netname;
            }
#else
            SCDynamicStoreRef theDynamicStore;
            theDynamicStore = SCDynamicStoreCreate(nil, CFSTR("FindCurrentInterfaceAndIP"), nil, nil);

            NSMutableString *interfaceName = [NSMutableString string];
            NSString *airportPath = [NSString stringWithFormat:@"State:/Network/Interface/%@/AirPort", qstringToNSString(name)];

            CFDictionaryRef airportPlist = (const __CFDictionary*)SCDynamicStoreCopyValue(theDynamicStore, (CFStringRef)airportPath);

            CFRelease(theDynamicStore);
            QString netname = nsstringToQString([(NSDictionary *)airportPlist valueForKey:@"SSID_STR"]);
            [pool release];
            return netname;
#endif
        }
        break;
    case QSystemNetworkInfo::EthernetMode:
        {
            if(isInterfaceActive(interfaceForMode(mode).name().toLocal8Bit())) {
                return QHostInfo::localDomainName();
            }
        }
        break;
    case QSystemNetworkInfo::BluetoothMode:
        break;
    case QSystemNetworkInfo::WimaxMode:
        break;
    default:
        break;
    };
    return "";
}

QString QSystemNetworkInfoPrivate::macAddress(QSystemNetworkInfo::NetworkMode mode)
{
    return interfaceForMode(mode).hardwareAddress();
}

QNetworkInterface QSystemNetworkInfoPrivate::interfaceForMode(QSystemNetworkInfo::NetworkMode mode)
{
    NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];
    QNetworkInterface netInterface;
    CFArrayRef interfaceArray = SCNetworkInterfaceCopyAll(); //10.4
    CFStringRef iName;
    CFStringRef type;
    for ( long i = 0; i < CFArrayGetCount(interfaceArray); i++) {
        SCNetworkInterfaceRef thisInterface =  (SCNetworkInterfaceRef ) CFArrayGetValueAtIndex(interfaceArray, i);
        type = SCNetworkInterfaceGetInterfaceType(thisInterface);
        iName = SCNetworkInterfaceGetBSDName(thisInterface);

        if (type != NULL) {
            if (CFEqual(type, kSCNetworkInterfaceTypeBluetooth) && mode == QSystemNetworkInfo::BluetoothMode) {
                netInterface = QNetworkInterface::interfaceFromName(stringFromCFString(iName));
                // workaround for null MAC from SCNetworkInterfaceGetHardwareAddressString and bogus BSD name here
#ifdef  MAC_SDK_10_6
                IOBluetoothHostController* controller = IOBluetoothHostController.defaultController;
                QString macbtMac = nsstringToQString([controller addressAsString]).replace("-",":").toUpper();
                if(!macbtMac.isEmpty()) {
                    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
                    Q_FOREACH(const QNetworkInterface thisNetInterface, interfaces) {
                        if( thisNetInterface.hardwareAddress() == macbtMac) {
                            netInterface = thisNetInterface;
                            break;
                        }
                    }
                }
#endif
            } else if (CFEqual(type, kSCNetworkInterfaceTypeEthernet) && mode == QSystemNetworkInfo::EthernetMode) {
                netInterface = QNetworkInterface::interfaceFromName(stringFromCFString(iName));
                break;
            } else if (CFEqual(type, kSCNetworkInterfaceTypeIEEE80211) && mode == QSystemNetworkInfo::WlanMode) {
                netInterface = QNetworkInterface::interfaceFromName(stringFromCFString(iName));
                break;
            }
        }
    }
    CFRelease(interfaceArray);
    [autoreleasepool release];
    return netInterface;
}

void QSystemNetworkInfoPrivate::wifiNetworkChanged(const QString &notification, const QString interfaceName)
{
    getDefaultInterface();

    if(notification == QLatin1String("SSID_CHANGED_NOTIFICATION")) {
        Q_EMIT networkNameChanged(QSystemNetworkInfo::WlanMode, networkName(QSystemNetworkInfo::WlanMode));
    }

    if(notification == QLatin1String("BSSID_CHANGED_NOTIFICATION")) {
        QSystemNetworkInfo::NetworkStatus status =  networkStatus(QSystemNetworkInfo::WlanMode);
        Q_EMIT networkStatusChanged( QSystemNetworkInfo::WlanMode, status);
    }
    if(notification == QLatin1String("POWER_CHANGED_NOTIFICATION")) {
#ifdef MAC_SDK_10_6
        CWInterface *wifiInterface = [CWInterface interfaceWithName:  qstringToNSString(interfaceName)];
        if([wifiInterface power]) {
            if(!rssiTimer->isActive()) {
                rssiTimer->start(5000);

            }
        }  else {
            if(rssiTimer->isActive()) {
                rssiTimer->stop();
            }
            Q_EMIT networkSignalStrengthChanged(QSystemNetworkInfo::WlanMode, 0);
        }
#endif
    }
}

QSystemNetworkInfo::NetworkMode QSystemNetworkInfoPrivate::currentMode()
{
    return modeForInterface(getDefaultInterface());
}

QSystemNetworkInfo::CellDataTechnology QSystemNetworkInfoPrivate::cellDataTechnology()
{
    return QSystemNetworkInfo::UnknownDataTechnology;
}


QSystemDisplayInfoPrivate::QSystemDisplayInfoPrivate(QObject *parent)
        : QObject(parent)
{
}

QSystemDisplayInfoPrivate::~QSystemDisplayInfoPrivate()
{
}

CGDirectDisplayID getCGId(int screen)
{
    CGDirectDisplayID displayId[16];
    CGDisplayCount count;
    CGDisplayErr error = CGGetOnlineDisplayList(16,displayId, & count);
    if(error == kCGErrorSuccess) {
        return displayId[screen];
    }
    return CGMainDisplayID();
}

int QSystemDisplayInfoPrivate::displayBrightness(int screen)
{
    CGDisplayErr dErr;
    io_service_t service;
    CFStringRef key = CFSTR(kIODisplayBrightnessKey);

    float brightness = 0.0;
    int displayBrightness = -1;
    service = CGDisplayIOServicePort(getCGId(screen));
    dErr = IODisplayGetFloatParameter(service, kNilOptions, key, &brightness);
    displayBrightness = (int)(brightness * 100);
    return displayBrightness;
}

int QSystemDisplayInfoPrivate::colorDepth(int screen)
{
    long bitsPerPixel = 0;
    bitsPerPixel = CGDisplayBitsPerPixel(getCGId(screen));
    return (int)bitsPerPixel;
}

QSystemDisplayInfo::DisplayOrientation QSystemDisplayInfoPrivate::orientation(int screen)
{
    QSystemDisplayInfo::DisplayOrientation orientation = QSystemDisplayInfo::Unknown;

    if(screen < 16 && screen > -1) {
        int rotation = (int)CGDisplayRotation(getCGId(screen));
        switch(rotation) {
        case 0:
        case 360:
            orientation = QSystemDisplayInfo::Landscape;
            break;
        case 90:
            orientation = QSystemDisplayInfo::Portrait;
            break;
        case 180:
            orientation = QSystemDisplayInfo::InvertedLandscape;
            break;
        case 270:
            orientation = QSystemDisplayInfo::InvertedPortrait;
            break;
        };
    }
    return orientation;
}


float QSystemDisplayInfoPrivate::contrast(int screen)
{
    Q_UNUSED(screen);
    QString accessplist = QDir::homePath() + "/Library/Preferences/com.apple.universalaccess.plist";
    QSettings accessSettings(accessplist, QSettings::NativeFormat);
    accessSettings.value("contrast").toFloat();
    return accessSettings.value("contrast").toFloat();
}

static int GetIntFromDictionaryForKey(CFDictionaryRef desc, CFStringRef key)
{
    CFNumberRef value;
    int resultNumber = 0;
    if ((value = (const __CFNumber*)CFDictionaryGetValue(desc,key)) == NULL
            || CFGetTypeID(value) != CFNumberGetTypeID())
        return 0;
    CFNumberGetValue(value, kCFNumberIntType, &resultNumber);
    return resultNumber;
}

CGDisplayErr GetDisplayDPI(CFDictionaryRef displayModeDict,CGDirectDisplayID displayID,
    double *horizontalDPI, double *verticalDPI)
{
    CGDisplayErr displayError = kCGErrorFailure;
    io_connect_t ioPort;
    CFDictionaryRef displayDict;

    ioPort = CGDisplayIOServicePort(displayID);
    if (ioPort != MACH_PORT_NULL) {
        displayDict = IOCreateDisplayInfoDictionary(ioPort, 0);
        if (displayDict != NULL) {
            const double mmPerInch = 25.4;
            double horizontalSizeInInches = (double)GetIntFromDictionaryForKey(displayDict, CFSTR(kDisplayHorizontalImageSize)) / mmPerInch;
            double verticalSizeInInches = (double)GetIntFromDictionaryForKey(displayDict, CFSTR(kDisplayVerticalImageSize)) / mmPerInch;

            CFRelease(displayDict);

            *horizontalDPI = (double)GetIntFromDictionaryForKey(displayModeDict, kCGDisplayWidth) / horizontalSizeInInches;
            *verticalDPI = (double)GetIntFromDictionaryForKey(displayModeDict, kCGDisplayHeight) / verticalSizeInInches;
            displayError = CGDisplayNoErr;
        }
    }
    return displayError;
}

int QSystemDisplayInfoPrivate::getDPIWidth(int screen)
{
    int dpi=0;
    if(screen < 16 && screen > -1) {
        double horizontalDPI, verticalDPI;

        CGDisplayErr displayError = GetDisplayDPI(CGDisplayCurrentMode(kCGDirectMainDisplay), kCGDirectMainDisplay, &horizontalDPI, &verticalDPI);
        if (displayError == CGDisplayNoErr) {
            dpi = horizontalDPI;
        }
    }
    return dpi;
}

int QSystemDisplayInfoPrivate::getDPIHeight(int screen)
{
    int dpi=0;
    if(screen < 16 && screen > -1) {
        double horizontalDPI, verticalDPI;

        CGDisplayErr displayError = GetDisplayDPI(CGDisplayCurrentMode(kCGDirectMainDisplay),  kCGDirectMainDisplay, &horizontalDPI, &verticalDPI);
        if (displayError == CGDisplayNoErr) {
            dpi = verticalDPI;
        }
    }
    return dpi;
}


int QSystemDisplayInfoPrivate::physicalHeight(int screen)
{
    int height=0;
    if(screen < 16 && screen > -1) {
        CGSize size = CGDisplayScreenSize(getCGId(screen));
        height = size.height;
    }
    return height;
}

int QSystemDisplayInfoPrivate::physicalWidth(int screen)
{
    int width=0;
    if(screen < 16 && screen > -1) {
        CGSize size = CGDisplayScreenSize(getCGId(screen));
        width = size.width;
    }
    return width;
}

QSystemDisplayInfo::BacklightState  QSystemDisplayInfoPrivate::backlightStatus(int screen)
{
    int bright = displayBrightness(screen);
    if(bright == 0) {
        return QSystemDisplayInfo::BacklightStateOff;
    } else if(bright > 1 && bright < 99) {
        return QSystemDisplayInfo::BacklightStateDimmed;
       } else {
        return QSystemDisplayInfo::BacklightStateOn;
    }
    return QSystemDisplayInfo::BacklightStateUnknown;
}


DAApprovalSessionRef session = NULL;

void mountCallback(DADiskRef disk, CFArrayRef /*keys*/, void *context)
{
    NSDictionary *batDoctionary;
    batDoctionary = (NSDictionary *)DADiskCopyDescription(disk);
    NSURL *volumePath = [[batDoctionary objectForKey:(NSString *)kDADiskDescriptionVolumePathKey] copy];

    QString name = nsstringToQString([volumePath path]);

    static_cast<QSystemStorageInfoPrivate*>(context)->storageChanged(true, name);
}

void mountCallback2(DADiskRef diskRef, void *context)
{
    DADiskRef wholeDisk;
    wholeDisk = DADiskCopyWholeDisk(diskRef);
// only deal with whole disks here.. i.e. cdroms
    if (wholeDisk) {
        io_service_t mediaService;

        mediaService = DADiskCopyIOMedia(wholeDisk);
        if (mediaService) {
            if (IOObjectConformsTo(mediaService, kIOCDMediaClass) || IOObjectConformsTo(mediaService, kIODVDMediaClass)) {

                NSDictionary *batDoctionary;
                batDoctionary = (NSDictionary *)DADiskCopyDescription(diskRef);
                NSURL *volumePath = [[batDoctionary objectForKey:(NSString *)kDADiskDescriptionVolumePathKey] copy];

                QString name = nsstringToQString([volumePath path]);

                static_cast<QSystemStorageInfoPrivate*>(context)->storageChanged(true, name);
                CFRelease(batDoctionary);
            }
        }
        IOObjectRelease(mediaService);
        CFRelease(wholeDisk);
    }
}

void unmountCallback(DADiskRef disk, void *context)
{
    NSDictionary *batDoctionary;
    batDoctionary = (NSDictionary *)DADiskCopyDescription(disk);
    NSURL *volumePath = [[batDoctionary objectForKey:(NSString *)kDADiskDescriptionVolumePathKey] copy];

    QString name = nsstringToQString([volumePath path]);

    static_cast<QSystemStorageInfoPrivate*>(context)->storageChanged(false,name);
}

QSystemStorageInfoPrivate::QSystemStorageInfoPrivate(QObject *parent)
        : QObject(parent), storageTimer(0), daSessionThread(0),sessionThreadStarted(0)
{
    updateVolumesMap();
    checkAvailableStorage();
}


QSystemStorageInfoPrivate::~QSystemStorageInfoPrivate()
{
    if(sessionThreadStarted) {
        daSessionThread->stop();
        delete daSessionThread;
    }
}

void QSystemStorageInfoPrivate::storageChanged( bool added, const QString &vol)
{
    if(!vol.isEmpty()) {
        QMapIterator<QString, QString> it(mountEntriesMap);
        QString foundKey;
        bool seen = false;
        while (it.hasNext()) {
            it.next();
            if( vol == it.value()) {
                seen = true;
                foundKey = it.key();
            }
        }

        if(added && !seen) {
            Q_EMIT logicalDriveChanged(added,vol);
            updateVolumesMap();
        }
        if(!added && seen) {
            mountEntriesMap.remove(foundKey);
            Q_EMIT logicalDriveChanged(added,vol);
        }


    } else {
        if(added) {

        } else { //removed
            // cdroms unmounting seem to not have a volume name with the notification here, so
            // we need to manually deal with it
            QMap <QString,QString> oldDrives = mountEntriesMap;
            updateVolumesMap();
            QStringList newDrives = mountEntriesMap.keys();
            QString foundDrive;

            QMapIterator<QString, QString> it(oldDrives);
            while (it.hasNext()) {
                it.next();
                if(!newDrives.contains(it.key())) {
                    Q_EMIT logicalDriveChanged(added, it.value());
                }
            }
        }
    }
}

bool QSystemStorageInfoPrivate::updateVolumesMap()
{
    struct statfs *buf = NULL;
    unsigned i, count = 0;

    mountEntriesMap.clear();

    count = getmntinfo(&buf, 0);
    for (i=0; i<count; i++) {
        char *volName = buf[i].f_mntonname;

        if(buf[i].f_type != 19
           && buf[i].f_type != 20
           && !mountEntriesMap.contains(volName)) {
            mountEntriesMap.insert(buf[i].f_mntfromname,volName);
        }
    }
    return true;
}


qint64 QSystemStorageInfoPrivate::availableDiskSpace(const QString &driveVolume)
{
    qint64 totalFreeBytes=0;
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSDictionary *attr = [ [NSFileManager defaultManager] attributesOfFileSystemForPath:qstringToNSString(driveVolume) error:nil];
    totalFreeBytes = [[attr objectForKey:NSFileSystemFreeSize] doubleValue];
    [pool release];

    return  totalFreeBytes;
}

qint64 QSystemStorageInfoPrivate::totalDiskSpace(const QString &driveVolume)
{
    getStorageState(driveVolume);
    qint64 totalBytes=0;
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSString *vol = qstringToNSString(driveVolume);
    NSDictionary *attr = [ [NSFileManager defaultManager] attributesOfFileSystemForPath:vol error:nil];
    totalBytes = [[attr objectForKey:NSFileSystemSize] doubleValue];
    [pool release];

    return totalBytes;
}

QSystemStorageInfo::DriveType QSystemStorageInfoPrivate::typeForDrive(const QString &driveVolume)
{
    QSystemStorageInfo::DriveType drivetype =  QSystemStorageInfo::NoDrive;

    DADiskRef diskRef;
    DASessionRef sessionRef;
    CFBooleanRef boolRef;
    CFBooleanRef boolRef2;
    CFDictionaryRef descriptionDictionary;

    sessionRef = DASessionCreate(NULL);
    if (sessionRef == NULL) {
        return QSystemStorageInfo::NoDrive;
    }

    diskRef = DADiskCreateFromBSDName(NULL, sessionRef, mountEntriesMap.key(driveVolume).toLatin1());
    if (diskRef == NULL) {
        CFRelease(sessionRef);
        return QSystemStorageInfo::NoDrive;
    }

    descriptionDictionary = DADiskCopyDescription(diskRef);
    if (descriptionDictionary == NULL) {
        CFRelease(diskRef);
        CFRelease(sessionRef);
        return QSystemStorageInfo::RemoteDrive;
    }

    boolRef = (CFBooleanRef)
              CFDictionaryGetValue(descriptionDictionary, kDADiskDescriptionMediaRemovableKey);
    if (boolRef) {
        if(CFBooleanGetValue(boolRef)) {
            drivetype = QSystemStorageInfo::RemovableDrive;
        } else {
            drivetype = QSystemStorageInfo::InternalDrive;
        }
    }
    boolRef2 = (CFBooleanRef)
              CFDictionaryGetValue(descriptionDictionary, kDADiskDescriptionVolumeNetworkKey);
    if (boolRef2) {
        if(CFBooleanGetValue(boolRef2)) {
            drivetype = QSystemStorageInfo::RemoteDrive;
        }
    }

    DADiskRef wholeDisk;
    wholeDisk = DADiskCopyWholeDisk(diskRef);

    if (wholeDisk) {
        io_service_t mediaService;

        mediaService = DADiskCopyIOMedia(wholeDisk);
        if (mediaService) {
            if (IOObjectConformsTo(mediaService, kIOCDMediaClass)) {
                drivetype = QSystemStorageInfo::CdromDrive;
            }
            if (IOObjectConformsTo(mediaService, kIODVDMediaClass)) {
                drivetype = QSystemStorageInfo::CdromDrive;
            }
            IOObjectRelease(mediaService);
        }
        CFRelease(wholeDisk);
    }
    CFRelease(diskRef);
    CFRelease(descriptionDictionary);
    CFRelease(boolRef);
    CFRelease(boolRef2);
    CFRelease(sessionRef);

    return drivetype;
}

QStringList QSystemStorageInfoPrivate::logicalDrives()
{
    updateVolumesMap();
    QStringList drivesList;
    QMapIterator<QString, QString> it(mountEntriesMap);
     while (it.hasNext()) {
         it.next();
         drivesList << it.value();
     }
     drivesList.sort();
    return drivesList;
}

bool QSystemStorageInfoPrivate::sessionThread()
{
    if(!sessionThreadStarted) {
        daSessionThread = new QDASessionThread();
        sessionThreadStarted = true;
    }
    daSessionThread->doWork();
    return true;
}


void QSystemStorageInfoPrivate::connectNotify(const char *signal)
{
    if (QLatin1String(signal) ==
        QLatin1String(QMetaObject::normalizedSignature(SIGNAL(logicalDriveChanged(bool,const QString&))))) {
        sessionThread();

        DARegisterDiskDescriptionChangedCallback(daSessionThread->session,kDADiskDescriptionMatchVolumeMountable,
                                                 kDADiskDescriptionWatchVolumePath, mountCallback,this);
        DARegisterDiskAppearedCallback(daSessionThread->session,kDADiskDescriptionMatchVolumeMountable,mountCallback2,this);
        DARegisterDiskDisappearedCallback(daSessionThread->session,kDADiskDescriptionMatchVolumeMountable,unmountCallback,this);
        sessionThreadStarted = true;
    }

    if (QLatin1String(signal) ==
        QLatin1String(QMetaObject::normalizedSignature(SIGNAL(storageStateChanged(const QString &, QSystemStorageInfo::StorageState))))) {
        if(!storageTimer) {
            storageTimer = new QTimer(this);
        }
        connect(storageTimer,SIGNAL(timeout()),this,SLOT(checkAvailableStorage()));
        if(!storageTimer->isActive())
            storageTimer->start(60 * 1000);
    }
}


void QSystemStorageInfoPrivate::disconnectNotify(const char *signal)
{

    if (QLatin1String(signal) ==
        QLatin1String(QMetaObject::normalizedSignature(SIGNAL(logicalDriveChanged(bool,const QString &))))) {
#ifdef MAC_SDK_10_6
     //   DAUnregisterApprovalCallback(daSessionThread->session,(void*)mountCallback,NULL);
#else
       // DAUnregisterApprovalCallback((__DAApprovalSession *)daSessionThread->session,(void*)unmountCallback,NULL);
#endif
    }
    if (QLatin1String(signal) ==
        QLatin1String(QMetaObject::normalizedSignature(SIGNAL(storageStateChanged(const QString &, QSystemStorageInfo::StorageState))))) {
        disconnect(storageTimer,SIGNAL(timeout()),this,SLOT(checkAvailableStorage()));
        storageTimer = 0;
    }
}

QString QSystemStorageInfoPrivate::uriForDrive(const QString &driveVolume)
{
    //TODO
    Q_UNUSED(driveVolume);
    return QString();
}

QSystemStorageInfo::StorageState QSystemStorageInfoPrivate::getStorageState(const QString &driveVolume)
{
    QSystemStorageInfo::StorageState storState = QSystemStorageInfo::UnknownStorageState;

    struct statfs fs;
    if (statfs(driveVolume.toLocal8Bit(), &fs) == 0) {
        if( fs.f_bfree != 0) {
            long percent = 100 -(fs.f_blocks - fs.f_bfree) * 100 / fs.f_blocks;
            if(percent < 41 && percent > 10 ) {
                storState = QSystemStorageInfo::LowStorageState;
            } else if(percent < 11 && percent > 2 ) {
                storState =  QSystemStorageInfo::VeryLowStorageState;
            } else if(percent < 3  ) {
                storState =  QSystemStorageInfo::CriticalStorageState;
            } else {
                storState =  QSystemStorageInfo::NormalStorageState;
            }
        }
    }
    return storState;
}

void QSystemStorageInfoPrivate::checkAvailableStorage()
{
    QMap<QString, QString> oldDrives = mountEntriesMap;
    Q_FOREACH(const QString &vol, oldDrives.keys()) {
        QSystemStorageInfo::StorageState storState = getStorageState(vol);
        if(!stateMap.contains(vol)) {
            stateMap.insert(vol,storState);
        } else {
            if(stateMap.value(vol) != storState) {
                stateMap[vol] = storState;
                qDebug() << "storage state changed" << storState;
                Q_EMIT storageStateChanged(vol, storState);
            }
        }
    }
}

void powerInfoChanged(void* context)
{
    QSystemDeviceInfoPrivate *sys = reinterpret_cast<QSystemDeviceInfoPrivate *>(context);
    if(sys) {
        sys->batteryLevel();
        sys->currentPowerState();
    }
}

void batteryInfoChanged(void* context)
{
    QSystemBatteryInfoPrivate *bat = reinterpret_cast<QSystemBatteryInfoPrivate *>(context);
    if(bat) {
        bat->getBatteryInfo();
    }
}


QSystemDeviceInfoPrivate::QSystemDeviceInfoPrivate(QObject *parent)
        : QObject(parent), btThreadOk(0) ,btThread(0),hasWirelessKeyboardConnected(0)
{
    batteryLevelCache = 0;
    currentPowerStateCache = QSystemDeviceInfo::UnknownPower;
    batteryStatusCache = QSystemDeviceInfo::NoBatteryLevel;
    hasWirelessKeyboardConnected = isWirelessKeyboardConnected();
}

QSystemDeviceInfoPrivate::~QSystemDeviceInfoPrivate()
{
    if( btThreadOk && btThread->keepRunning) {
        btThread->stop();
        delete btThread;
    }
}

QSystemDeviceInfoPrivate *QSystemDeviceInfoPrivate::instance()
{
    return qsystemDeviceInfoPrivate();
}

void QSystemDeviceInfoPrivate::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(bluetoothStateChanged(bool))
        || QLatin1String(signal) == SIGNAL(wirelessKeyboardConnected(bool))) {

        if(!btThread) {
            btThread = new QBluetoothListenerThread();
            btThreadOk = true;
        }
        btThread->doWork();

        if (QLatin1String(signal) == SIGNAL(bluetoothStateChanged(bool))) {
            connect(btThread,SIGNAL(bluetoothPower(bool)), this, SIGNAL(bluetoothStateChanged(bool)),Qt::UniqueConnection);
        }
        if( QLatin1String(signal) == SIGNAL(wirelessKeyboardConnected(bool))) {
            btThread->setupConnectNotify();
        }
    }

    if (QLatin1String(signal) == SIGNAL(powerStateChanged(QSystemDeviceInfo::PowerState))
            || QLatin1String(signal) == SIGNAL(batteryLevelChanged(int))
            || QLatin1String(signal) == SIGNAL(batteryStatusChanged(QSystemDeviceInfo::BatteryStatus))
            ) {

        NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];

//        if(!runLoopSource)
        CFRunLoopSourceRef runLoopSource = (CFRunLoopSourceRef)IOPSNotificationCreateRunLoopSource(powerInfoChanged, this);
        if (runLoopSource) {
            CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopDefaultMode);
        }
        [autoreleasepool release];
        CFRelease(runLoopSource);
    }
}

void QSystemDeviceInfoPrivate::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(bluetoothStateChanged(bool))) {
        if(btThread->keepRunning) {
            btThread->stop();
        }
    }
}

QSystemDeviceInfo::Profile QSystemDeviceInfoPrivate::currentProfile()
{
    return QSystemDeviceInfo::UnknownProfile;
}

QSystemDeviceInfo::InputMethodFlags QSystemDeviceInfoPrivate::inputMethodType()
{
    QSystemDeviceInfo::InputMethodFlags methods = 0;

    if(hasIOServiceMatching("AppleUSBTCButtons")) {
        methods = (methods | QSystemDeviceInfo::Keys);
    }
    if(hasIOServiceMatching("AppleUSBTCKeyboard")) {
        methods = (methods | QSystemDeviceInfo::Keyboard);
    }
    if(hasIOServiceMatching("AppleUSBMultitouchDriver")) {
        methods = (methods | QSystemDeviceInfo::MultiTouch);
    }
    if(hasIOServiceMatching("IOHIDPointing")) {
        methods = (methods | QSystemDeviceInfo::Mouse);
    }
    return methods;
}

QSystemDeviceInfo::PowerState QSystemDeviceInfoPrivate::currentPowerState()
{
    NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];
    QSystemDeviceInfo::PowerState state = QSystemDeviceInfo::UnknownPower;

    CFDictionaryRef powerSourceDict = NULL;
    CFStringRef powerStateString;

    CFTypeRef powerSourcesInfoBlob = IOPSCopyPowerSourcesInfo();
    CFArrayRef powerSourcesList = IOPSCopyPowerSourcesList(powerSourcesInfoBlob);
    int i;
    for (i=0; i<CFArrayGetCount(powerSourcesList); i++) {
        powerSourceDict = IOPSGetPowerSourceDescription(powerSourcesInfoBlob, /* [powerSourcesList objectAtIndex:0]*/ CFArrayGetValueAtIndex(powerSourcesList, i));
        if(!powerSourceDict) {
            state = QSystemDeviceInfo::UnknownPower;
            break;
        }

        powerStateString = (CFStringRef)CFDictionaryGetValue(powerSourceDict, CFSTR(kIOPSPowerSourceStateKey));
        if(CFStringCompare(powerStateString,CFSTR(kIOPSBatteryPowerValue),0)==kCFCompareEqualTo) {
            //has battery
            state = QSystemDeviceInfo::BatteryPower;
        } else {

            NSDictionary *powerSourceInfo = nil;
            powerSourceInfo = [NSDictionary dictionaryWithDictionary:(NSDictionary*)powerSourceDict];
            bool charging = (bool)[[powerSourceInfo valueForKey:[NSString stringWithUTF8String:kIOPSIsChargingKey]] boolValue];
            if (charging ) {
                state = QSystemDeviceInfo::WallPowerChargingBattery;
            } else {
                state = QSystemDeviceInfo::WallPower;
            }
        }
    }

    CFRelease(powerSourcesInfoBlob);
    CFRelease(powerSourcesList);

    if( currentPowerStateCache != state) {
        currentPowerStateCache = state;
        Q_EMIT powerStateChanged(state);
    }
    [autoreleasepool release];
    return state;
}

QSystemDeviceInfo::ThermalState QSystemDeviceInfoPrivate::currentThermalState()
{
    return QSystemDeviceInfo::UnknownThermal;
}

QString QSystemDeviceInfoPrivate::imei()
{
    return "";
}

QString QSystemDeviceInfoPrivate::imsi()
{
    return "";
}

QString QSystemDeviceInfoPrivate::manufacturer()
{
    return QString("Apple"); //pretty sure we can hardcode this one
}

QString QSystemDeviceInfoPrivate::model()
{
    char modelBuffer[256];
    QString model;
      size_t sz = sizeof(modelBuffer);
      if (0 == sysctlbyname("hw.model", modelBuffer, &sz, NULL, 0)) {
          model = QLatin1String(modelBuffer);
      }
    return  model;
}

QString QSystemDeviceInfoPrivate::productName()
{
    NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];
    QString name = nsstringToQString([[[NSDictionary dictionaryWithContentsOfFile:@"/System/Library/CoreServices/SystemVersion.plist"] objectForKey:@"ProductName"]autorelease]);
    [autoreleasepool release];
    return name ;
}

int QSystemDeviceInfoPrivate::batteryLevel()
{
    float level = 0;
    NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];

    CFDictionaryRef powerSourceDict = NULL;
    CFTypeRef powerSourcesInfoBlob = IOPSCopyPowerSourcesInfo();
    CFArrayRef powerSourcesList = IOPSCopyPowerSourcesList(powerSourcesInfoBlob);
    int i;
    for (i=0; i<CFArrayGetCount(powerSourcesList); i++) {
        powerSourceDict = IOPSGetPowerSourceDescription(powerSourcesInfoBlob, /* [powerSourcesList objectAtIndex:0]*/ CFArrayGetValueAtIndex(powerSourcesList, i));
        if(!powerSourceDict) {
            break;
        }

        float currentLevel = 0;
        float maxLevel = 0;
        const void *powerStateValue;

        powerStateValue = CFDictionaryGetValue(powerSourceDict, CFSTR(kIOPSCurrentCapacityKey));
        CFNumberGetValue((CFNumberRef)powerStateValue, kCFNumberSInt32Type, &currentLevel);

        powerStateValue = CFDictionaryGetValue(powerSourceDict, CFSTR(kIOPSMaxCapacityKey));
        CFNumberGetValue((CFNumberRef)powerStateValue, kCFNumberSInt32Type, &maxLevel);
        level = (currentLevel/maxLevel) * 100;
    }

    CFRelease(powerSourcesInfoBlob);
    CFRelease(powerSourcesList);

    if(batteryLevelCache != level) {
        batteryLevelCache = level;
        Q_EMIT batteryLevelChanged(level);
    }

    if(batteryLevelCache < 4 && batteryStatusCache != QSystemDeviceInfo::BatteryCritical) {
        batteryStatusCache = QSystemDeviceInfo::BatteryCritical;
        Q_EMIT batteryStatusChanged(batteryStatusCache);
    } else if((batteryLevelCache > 3 && batteryLevelCache < 11) && batteryStatusCache != QSystemDeviceInfo::BatteryVeryLow) {
        batteryStatusCache = QSystemDeviceInfo::BatteryVeryLow;
        Q_EMIT batteryStatusChanged(batteryStatusCache);
    } else if((batteryLevelCache > 10 && batteryLevelCache < 41) && batteryStatusCache != QSystemDeviceInfo::BatteryLow) {
        batteryStatusCache = QSystemDeviceInfo::BatteryLow;
        Q_EMIT batteryStatusChanged(batteryStatusCache);
    } else if(batteryLevelCache > 40 && batteryStatusCache != QSystemDeviceInfo::BatteryNormal) {
        batteryStatusCache = QSystemDeviceInfo::BatteryNormal;
        Q_EMIT batteryStatusChanged(batteryStatusCache);
    }

    [autoreleasepool release];
    return (int)level;
}

QSystemDeviceInfo::SimStatus QSystemDeviceInfoPrivate::simStatus()
{
    return QSystemDeviceInfo::SimNotAvailable;
}

bool QSystemDeviceInfoPrivate::isDeviceLocked()
{
    // find out if auto login is being used.
    QSettings loginSettings("/Library/Preferences/com.apple.loginwindow.plist", QSettings::NativeFormat);
    QString autologinname = loginSettings.value("autoLoginUser").toString();

// find out if locked screensaver is used.
    int passWordProtected = 0;
#if defined(QT_ARCH_X86_64)  && !defined(MAC_SDK_10_6)
    ScreenSaverDefaults *ssDefaults;
    ssDefaults = [ScreenSaverDefaults defaultsForModuleWithName:@"com.apple.screensaver"];
    passWordProtected = [ssDefaults integerForKey:@"askForPassword"];
#endif

    if(autologinname.isEmpty() || passWordProtected == 1) {
        return true;
    } else {
        return false;
    }
}

bool QSystemDeviceInfoPrivate::currentBluetoothPowerState()
{
    if(isBtPowerOn())
        return true;
    return false;
}

QSystemDeviceInfo::KeyboardTypeFlags QSystemDeviceInfoPrivate::keyboardTypes()
{
    QSystemDeviceInfo::InputMethodFlags methods = inputMethodType();
    QSystemDeviceInfo::KeyboardTypeFlags keyboardFlags = QSystemDeviceInfo::UnknownKeyboard;

    if((methods & QSystemDeviceInfo::Keyboard)) {
        keyboardFlags = (keyboardFlags | QSystemDeviceInfo::FullQwertyKeyboard);
  }
    if(isWirelessKeyboardConnected()) {
        keyboardFlags = (keyboardFlags | QSystemDeviceInfo::WirelessKeyboard);
    }
// how to check softkeys on desktop?
    return keyboardFlags;
}

bool QSystemDeviceInfoPrivate::isWirelessKeyboardConnected()
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSArray *pairedDevices = [ IOBluetoothDevice pairedDevices];

    for (IOBluetoothDevice *currentBtDevice in pairedDevices) {
        if([ currentBtDevice getClassOfDevice ] == 9536
           && [currentBtDevice isConnected]) {
            hasWirelessKeyboardConnected = true;
        }
    }
    [pool drain];
    return hasWirelessKeyboardConnected;
}

bool QSystemDeviceInfoPrivate::isKeyboardFlippedOpen()
{
    return false;
}

void QSystemDeviceInfoPrivate::keyboardConnected(bool connect)
{
    if(connect != hasWirelessKeyboardConnected)
        hasWirelessKeyboardConnected = connect;
    Q_EMIT wirelessKeyboardConnected(connect);
}

bool QSystemDeviceInfoPrivate::keypadLightOn(QSystemDeviceInfo::KeypadType type)
{
    if(type == QSystemDeviceInfo::PrimaryKeypad) {
         static io_connect_t dataPort = 0;

         kern_return_t kreturn;
         io_service_t ioService;

         ioService = IOServiceGetMatchingService(kIOMasterPortDefault, IOServiceMatching("AppleLMUController"));
         if (!ioService) {
             qDebug() << "AppleLMUController error";
             return false;
         }

         kreturn = IOServiceOpen(ioService, mach_task_self(), 0, &dataPort);
         IOObjectRelease(ioService);
         if (kreturn != KERN_SUCCESS) {
             qDebug() << "IOServiceOpen "<< kreturn;
             return false;
         }

         uint64_t inputValues[1] = {0};

         uint32_t outputCount = 1;
         uint64_t outputValues[1];

         kreturn = IOConnectCallScalarMethod(dataPort,1,inputValues,1,outputValues,&outputCount);
         if (kreturn != KERN_SUCCESS) {
             qDebug() << "keyboard error";
             return false;
         }

         if(outputValues[0] > 0) return true;
     }
    return false;
}

QByteArray QSystemDeviceInfoPrivate::uniqueDeviceID()
{
    CFStringRef uuidKey = CFSTR(kIOPlatformUUIDKey);
    io_service_t ioService = IOServiceGetMatchingService(kIOMasterPortDefault,
                                                         IOServiceMatching("IOPlatformExpertDevice"));
    QCryptographicHash hash(QCryptographicHash::Sha1);
    if (ioService) {
        CFTypeRef cfStringKey = IORegistryEntryCreateCFProperty(ioService, uuidKey, kCFAllocatorDefault, 0);

        hash.addData(stringFromCFString((const __CFString*)cfStringKey).toLocal8Bit());
        return hash.result().toHex();
    }
    hash.addData(QString::number(gethostid()).toLocal8Bit());
    return hash.result().toHex();
}

QSystemDeviceInfo::LockTypeFlags QSystemDeviceInfoPrivate::lockStatus()
{
    if(isDeviceLocked()) {
        return QSystemDeviceInfo::PinLocked;
    }
    return QSystemDeviceInfo::UnknownLock;
}

int QSystemDeviceInfoPrivate::messageRingtoneVolume()
{
    return 0;
}

int QSystemDeviceInfoPrivate::voiceRingtoneVolume()
{
    return 0;
}

bool QSystemDeviceInfoPrivate::vibrationActive()
{
    return false;
}

QSystemScreenSaverPrivate::QSystemScreenSaverPrivate(QObject *parent)
        : QObject(parent), isInhibited(0)
{
    ssTimer = new QTimer(this);
    connect(ssTimer, SIGNAL(timeout()), this, SLOT(activityTimeout()));
}

QSystemScreenSaverPrivate::~QSystemScreenSaverPrivate()
{
    setScreenSaverInhibited(false);
}

bool QSystemScreenSaverPrivate::setScreenSaverInhibit()
{
    activityTimeout();
    ssTimer->start(1000 * 60);
    if(ssTimer->isActive()) {
        isInhibited = true;
        return isInhibited;
    }

    return false;
}


bool QSystemScreenSaverPrivate::screenSaverInhibited()
{
    return isInhibited;
}

void QSystemScreenSaverPrivate::activityTimeout()
{
    UpdateSystemActivity(OverallAct);
}

void QSystemScreenSaverPrivate::setScreenSaverInhibited(bool on)
{
    if (on) {
        setScreenSaverInhibit();
    } else {
        if(ssTimer->isActive()) {
            ssTimer->stop();
            isInhibited = false;
        }
    }
}

QSystemBatteryInfoPrivate::QSystemBatteryInfoPrivate(QObject *parent)
: QObject(parent)
{
    getBatteryInfo();

    NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];

    CFRunLoopSourceRef runLoopSource = (CFRunLoopSourceRef)IOPSNotificationCreateRunLoopSource(batteryInfoChanged, this);
    if (runLoopSource) {
        CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopDefaultMode);
        CFRelease(runLoopSource);
    }
    [autoreleasepool release];
}

QSystemBatteryInfoPrivate::~QSystemBatteryInfoPrivate()
{
}

QSystemBatteryInfo::ChargerType QSystemBatteryInfoPrivate::chargerType() const
{
    return curChargeType;
}

QSystemBatteryInfo::ChargingState QSystemBatteryInfoPrivate::chargingState() const
{
    return curChargeState;
}

int QSystemBatteryInfoPrivate::nominalCapacity() const
{
    return capacity;
}

int QSystemBatteryInfoPrivate::remainingCapacityPercent() const
{
    return currentBatLevelPercent;
}

int QSystemBatteryInfoPrivate::remainingCapacity() const
{
    return remainingEnergy;
}

int QSystemBatteryInfoPrivate::voltage() const
{
    return currentVoltage;
}

int QSystemBatteryInfoPrivate::remainingChargingTime() const
{
    return timeToFull;
}

int QSystemBatteryInfoPrivate::currentFlow() const
{
    return dischargeRate;
}

int QSystemBatteryInfoPrivate::remainingCapacityBars() const
{
    return 0;
}

int QSystemBatteryInfoPrivate::maxBars() const
{
    return 0;
}

QSystemBatteryInfo::BatteryStatus QSystemBatteryInfoPrivate::batteryStatus() const
{
    return currentBatStatus;
}


QSystemBatteryInfo::EnergyUnit QSystemBatteryInfoPrivate::energyMeasurementUnit()
{
    return QSystemBatteryInfo::UnitmAh;
}

void QSystemBatteryInfoPrivate::getBatteryInfo()
{
    int cLevel = 0;
    int cEnergy = 0;
    int cVoltage = 0;
    int cTime = 0;
    int rEnergy = 0;
    QSystemBatteryInfo::ChargingState cState = QSystemBatteryInfo::ChargingError;
    QSystemBatteryInfo::ChargerType cType = QSystemBatteryInfo::UnknownCharger;
    bool isCharging;

    CFTypeRef info;
    CFArrayRef list;
    CFDictionaryRef battery;

    info = IOPSCopyPowerSourcesInfo();
    if(info == NULL) {
        qDebug() << "IOPSCopyPowerSourcesInfo error";
        return;
    }
    list = IOPSCopyPowerSourcesList(info);
    if(list == NULL) {
        CFRelease(info);
        qDebug() << "IOPSCopyPowerSourcesList error";
        return;
    }

    CFMutableDictionaryRef matching = NULL;
    CFMutableDictionaryRef batDoctionary = NULL;
    io_registry_entry_t entry = 0;
    matching = IOServiceMatching("IOPMPowerSource");
    entry = IOServiceGetMatchingService(kIOMasterPortDefault,matching);
    IORegistryEntryCreateCFProperties(entry, &batDoctionary,NULL,0);

    //    NSLog( @"%@" , batDoctionary );

    NSDictionary *legacyDict = [(NSDictionary*)batDoctionary objectForKey:@"LegacyBatteryInfo"];

    if(CFArrayGetCount(list) && (battery = IOPSGetPowerSourceDescription(info, CFArrayGetValueAtIndex(list,0)))) {

        cLevel = [[(NSDictionary*)battery objectForKey:@kIOPSCurrentCapacityKey] intValue];
        if(cLevel != currentBatLevelPercent) {
            currentBatLevelPercent = cLevel;
            Q_EMIT remainingCapacityPercentChanged(currentBatLevelPercent);
        }

        QSystemBatteryInfo::BatteryStatus stat = QSystemBatteryInfo::BatteryUnknown;

        if (currentBatLevelPercent < 4) {
            stat = QSystemBatteryInfo::BatteryCritical;
        } else if (currentBatLevelPercent < 11) {
             stat = QSystemBatteryInfo::BatteryVeryLow;
        } else if (currentBatLevelPercent < 41) {
             stat =  QSystemBatteryInfo::BatteryLow;
        } else if (currentBatLevelPercent > 40 && currentBatLevelPercent < 100) {
             stat = QSystemBatteryInfo::BatteryOk;
        } else if (currentBatLevelPercent == 100) {
             stat = QSystemBatteryInfo::BatteryFull;
        }
        if(currentBatStatus != stat) {
            currentBatStatus = stat;
            Q_EMIT batteryStatusChanged(stat);
        }

        isCharging = [[(NSDictionary*)batDoctionary objectForKey:@"IsCharging"] boolValue];
        if([(NSString*)[(NSDictionary*)battery objectForKey:@kIOPSPowerSourceStateKey] isEqualToString:@kIOPSACPowerValue]) {
            cType = QSystemBatteryInfo::WallCharger;
        } else {
            cType = QSystemBatteryInfo::NoCharger;
        }

        if(cType != curChargeType) {
            curChargeType = cType;
            Q_EMIT chargerTypeChanged(curChargeType);
        }

        if ([[(NSDictionary*)batDoctionary objectForKey:@"IsCharging"] boolValue]) {
            cState = QSystemBatteryInfo::Charging;
        } else {
            cState = QSystemBatteryInfo::NotCharging;
        }
        if (cState != curChargeState ) {
            curChargeState = cState;
            Q_EMIT chargingStateChanged(curChargeState);
        }
        cVoltage = [[(NSDictionary*)batDoctionary objectForKey:@"Voltage"] intValue];
        if (cVoltage != currentVoltage ) {
            currentVoltage = cVoltage;
        }

        int amp = /*[[legacyDict objectForKey:@"Current"] intValue];
        capacity =*/ [[(NSDictionary*)batDoctionary objectForKey:@"Current"] intValue];
        cEnergy = /*currentVoltage * */amp /*/ 1000*/;
        if (cEnergy != curChargeState ) {
            dischargeRate = cEnergy;
            Q_EMIT currentFlowChanged(dischargeRate);
        }

        cTime = [[(NSDictionary*)batDoctionary objectForKey:@"AvgTimeToFull"] intValue]; //FIXME thats not right
        if(!isCharging) {
            cTime = 0;
        }
        if (cTime != timeToFull) {
            timeToFull = cTime * 60;
            Q_EMIT remainingChargingTimeChanged(timeToFull);
        }
        capacity = [[(NSDictionary*)batDoctionary objectForKey:@"MaxCapacity"] intValue];
        if (cState != curChargeState ) {
            curChargeState = cState;
            Q_EMIT chargingStateChanged(curChargeState);
        }
        rEnergy = [[(NSDictionary*)batDoctionary objectForKey:@"CurrentCapacity"] intValue];
        if (rEnergy != remainingEnergy ) {
            remainingEnergy = rEnergy;
            Q_EMIT remainingCapacityChanged(remainingEnergy);
        }
    }
    CFRelease(list);
    CFRelease(info);
    CFRelease(batDoctionary);
}

#include "moc_qsysteminfo_mac_p.cpp"

QTM_END_NAMESPACE
