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

#include"jni_android_p.h"
#include<QDebug>
#include <android/log.h>
#include <QMutex>

#ifndef NELEM
# define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
#endif

static JNIEnv *m_env = NULL;
static JavaVM *m_javaVM = NULL;

template <class T>
        class SystemInfoGlobalObject
{
public:
    SystemInfoGlobalObject(const T & value= NULL)
    {
        if (m_env)
            m_value=m_env->NewGlobalRef(value);
        else
            m_value=value;
    }
    virtual ~SystemInfoGlobalObject()
    {
        if (m_value && m_env)
            m_env->DeleteGlobalRef(m_value);
    }
    T& operator =(const T& value)
                 {
        if (m_value && m_env)
            m_env->DeleteGlobalRef(m_value);
        if (m_env)
            m_value=m_env->NewGlobalRef(value);
        else
            m_value=value;
        return m_value;
    }
    T& operator()()
    {
        return m_value;
    }

private:
    T m_value;
};

namespace QtSystemInfoJni
{
    typedef union {
        JNIEnv* nativeEnvironment;
        void* venv;
    } UnionJNIEnvToVoid;

    static const char *qtSystemInfoClassPathName = "eu/licentia/necessitas/mobile/QtSystemInfo";
    static const char *qtBatteryInfoClassPathName= "eu/licentia/necessitas/mobile/BatteryInfo";

    struct QtSystemInfoJnistruct
    {
        SystemInfoGlobalObject<jobject> m_systemInfoClass;
        SystemInfoGlobalObject<jobject> m_systemObject;
        jmethodID m_getConstructorID;
        jmethodID m_getSetScreenSaverInhibitID;
        jmethodID m_getInitScreensaverID;
        jmethodID m_getDisableScreenSaverInhibitID;
        jmethodID m_getInitBatteryID;
        jmethodID m_getExitBatteryID;
        BatteryInfo batInfo;
        BatteryInfoUpdateListner *batteryInfoUpdateListner;
        struct BatteryInfoFields
        {
            jfieldID m_batteryStatusField;
            jfieldID m_chargerTypeField;
            jfieldID m_chargingStateField;
            jfieldID m_maxBarsField;
            jfieldID m_remainingCapacityBarsField;
            jfieldID m_remainingCapacityPercentField;
            jfieldID m_voltageField;
        };
        BatteryInfoFields m_batteryInfoFields;

        //device
        jmethodID m_getInitDeviceID;
        jmethodID m_getExitDeviceID;
        jmethodID m_getBluetoothPowerStateID;
        jmethodID m_getImeiID;
        jmethodID m_getImsiID;
        jmethodID m_getIsDeviceLockedID;
        jmethodID m_getIsKeyboardFlipOpenedID;
        jmethodID m_getInputMethodTypeID;
        jmethodID m_getLockStatusID;
        jmethodID m_getManufacturerID;
        jmethodID m_getModelID;
        jmethodID m_getProductNameID;
        jmethodID m_getSimStatusID;
        jmethodID m_getKeyboardTypeID;
        DeviceUpdatelistener *m_deviceUpdateListner;
        ProfileMode m_profileMode;
        //system
        jmethodID m_getAvailableLanguagesID;
        jmethodID m_getCurrentCountryCodeID;
        jmethodID m_getCurrentLanguageID;
        jmethodID m_getInitSystemGeneralInfoID;
        jmethodID m_getExitSystemGeneralInfoID;
        jmethodID m_getFeaturesAvailableID;
        GeneralInfoUpdateListner *m_generalInfoUpdateListner;
        //display
        jmethodID m_getInitDisplayID;
        jmethodID m_getExitDisplayID;
        jmethodID m_getDPIHeightID;
        jmethodID m_getDPIWidthID;
        jmethodID m_getPhysicalHeightID;
        jmethodID m_getPhysicalWidthID;
        jmethodID m_getOrientationID;
        jmethodID m_getBacklightStatusID;
        jmethodID m_getDisplayBrightnessID;
        jmethodID m_getColorDepthID;
        //storage
        jmethodID m_getInitStorageID;
        jmethodID m_getExitStorageID;
        StorageInfoUpdateListner *m_storageInfoUpdateListner;
        //network
        jmethodID m_getInitNetworkID;
        jmethodID m_getExitNetworkID;
        jmethodID m_getNetworkNameID;
        NetworkInfoUpdateListener *m_networkInfoUpdateListener;
        jmethodID m_getMacAddressID;
        //jmethodID m_getNetworkStatusID;
        jmethodID m_getWifiStatusID;
        jmethodID m_getBluetoothStatusID;
        jmethodID m_getWifiStrengthID;
        jmethodID m_getCellID;
        jmethodID m_getCurrentMobileCountryCodeID;
        jmethodID m_getCurrentMobileNetworkCodeID;
        jmethodID m_getCurrentModeID;
        jmethodID m_getLocationAreaCodeID;
        int m_serviceStrength;
        int m_wifiStrength;
        int m_serviceStatus;
        int m_serviceMode;
        int m_networkStatus;
        int m_networkMode;
    };

    static QtSystemInfoJnistruct qtSystemInfoJni;
    static void BatteryDataUpdated(JNIEnv * env,_jobject,jobject qbatteryInfo)
    {
        qtSystemInfoJni.batInfo.m_batteryStatus=env->GetCharField(qbatteryInfo,qtSystemInfoJni.m_batteryInfoFields.m_batteryStatusField);
        qtSystemInfoJni.batInfo.m_chargerType=env->GetIntField(qbatteryInfo,qtSystemInfoJni.m_batteryInfoFields.m_chargerTypeField);
        qtSystemInfoJni.batInfo.m_chargingState=env->GetIntField(qbatteryInfo,qtSystemInfoJni.m_batteryInfoFields.m_chargingStateField);
        qtSystemInfoJni.batInfo.m_maxBars=env->GetIntField(qbatteryInfo,qtSystemInfoJni.m_batteryInfoFields.m_maxBarsField);
        qtSystemInfoJni.batInfo.m_remainingCapacityBars=env->GetIntField(qbatteryInfo,qtSystemInfoJni.m_batteryInfoFields.m_remainingCapacityBarsField);
        qtSystemInfoJni.batInfo.m_remainingCapacityPercent=env->GetIntField(qbatteryInfo,qtSystemInfoJni.m_batteryInfoFields.m_remainingCapacityPercentField);
        qtSystemInfoJni.batInfo.m_voltage=env->GetIntField(qbatteryInfo,qtSystemInfoJni.m_batteryInfoFields.m_voltageField);

        if(qtSystemInfoJni.batteryInfoUpdateListner!=NULL)
            qtSystemInfoJni.batteryInfoUpdateListner->onBatteryInfoUpdate(qtSystemInfoJni.batInfo);

    }

    void initBattery()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return;
        }

        env->CallVoidMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getInitBatteryID);

        m_javaVM->DetachCurrentThread();
    }


    void registerBatteryInfoUpdates(BatteryInfoUpdateListner *listner)
    {
        qtSystemInfoJni.batteryInfoUpdateListner=listner;
        initBattery();
    }

    void unregisterBatteryInfoUpdates()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return;
        }

        env->CallVoidMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getExitBatteryID);

        //m_javaVM->DetachCurrentThread();
    }

    BatteryInfo getBatteryInfo ()
    {
        return qtSystemInfoJni.batInfo;
    }

    //device

    void initDevice()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return;
        }

        env->CallVoidMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getInitDeviceID);

        m_javaVM->DetachCurrentThread();
    }

    void exitDevice()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return;
        }

        env->CallVoidMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getExitDeviceID);

        //m_javaVM->DetachCurrentThread();
    }

    static void bluetoothStateChanged(JNIEnv *,_jobject,bool state)
    {
        if(qtSystemInfoJni.m_deviceUpdateListner!=NULL)
            qtSystemInfoJni.m_deviceUpdateListner->onBluetoothStateChanged(state);
    }

    static void deviceLocked(JNIEnv *,_jobject,bool isDeviceLocked)
    {
        if(qtSystemInfoJni.m_deviceUpdateListner!=NULL)
            qtSystemInfoJni.m_deviceUpdateListner->onDeviceLocked(isDeviceLocked);
    }

    static void profileChanged(JNIEnv *,_jobject,int mode)
    {
        switch(mode)
        {
        case 2:
            qtSystemInfoJni.m_profileMode=NormalProfile;
            break;
        case 0:
            qtSystemInfoJni.m_profileMode=SilentProfile;
            break;
        case 1:
            qtSystemInfoJni.m_profileMode=VibProfile;
            break;
        default:
            qtSystemInfoJni.m_profileMode=UnknownProfile;
        }

        if(qtSystemInfoJni.m_deviceUpdateListner!=NULL)
            qtSystemInfoJni.m_deviceUpdateListner->onProfileChanged(qtSystemInfoJni
                                                                    .m_profileMode);
    }

    bool getBluetoothState()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return false;
        }

        bool state=env->CallBooleanMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getBluetoothPowerStateID);

        m_javaVM->DetachCurrentThread();
        return state;
    }

    bool isDeviceLocked ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return false;
        }

        bool state=env->CallBooleanMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getIsDeviceLockedID);

        m_javaVM->DetachCurrentThread();
        return state;
    }

    QString imei ()
    {

        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return QString();
        }

        jobject imeiObject=env->CallObjectMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getImeiID);

        const char * imei=env->GetStringUTFChars((jstring)imeiObject,NULL);

        QString imeiId(imei);

        env->ReleaseStringUTFChars((jstring)imeiObject,imei);
        m_javaVM->DetachCurrentThread();
        return imeiId;
    }

    QString imsi ()
    {

        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return QString ();
        }

        jobject imsiObject=env->CallObjectMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getImsiID);

        const char * imsi=env->GetStringUTFChars((jstring)imsiObject,NULL);

        QString imsiId(imsi);

        env->ReleaseStringUTFChars((jstring)imsiObject,imsi);
        m_javaVM->DetachCurrentThread();
        return imsiId;
    }

    ProfileMode getProfileState ()
    {
        return qtSystemInfoJni.m_profileMode;
    }

    void registerDeviceInfoUpdate (DeviceUpdatelistener * listener)
    {
        qtSystemInfoJni.m_deviceUpdateListner=listener;
    }

    bool isKeyboardFlipOpened ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return -1;
        }

        bool state=env->CallBooleanMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getIsKeyboardFlipOpenedID);

        m_javaVM->DetachCurrentThread();
        return state;
    }

    int inputMethodType ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return -1;
        }

        int inputMethodType=env->CallIntMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getInputMethodTypeID);

        m_javaVM->DetachCurrentThread();
        return inputMethodType;
    }

    int lockStatus ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return -1;
        }

        int lockStatus=env->CallIntMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getLockStatusID);

        m_javaVM->DetachCurrentThread();
        return lockStatus;
    }

    QString manufacturer ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return QString();
        }

        jobject manufacturerObject=env->CallObjectMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getManufacturerID);

        const char * manufacturer=env->GetStringUTFChars((jstring)manufacturerObject,NULL);
        QString manufacturerName(manufacturer);
        env->ReleaseStringUTFChars((jstring)manufacturerObject,manufacturer);

        m_javaVM->DetachCurrentThread();
        return manufacturerName;
    }

    QString model ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return QString();
        }

        jobject modelObject=env->CallObjectMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getModelID);

        const char * model=env->GetStringUTFChars((jstring)modelObject,NULL);
        QString modelName(model);
        env->ReleaseStringUTFChars((jstring)modelObject,model);

        m_javaVM->DetachCurrentThread();
        return modelName;
    }

    QString productName ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return QString();
        }

        jobject productObject=env->CallObjectMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getProductNameID);

        const char * product=env->GetStringUTFChars((jstring)productObject,NULL);
        QString productName(product);
        env->ReleaseStringUTFChars((jstring)productObject,product);

        m_javaVM->DetachCurrentThread();
        return productName;
    }

    int simStatus ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return -1;
        }

        int simStatus=env->CallIntMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getSimStatusID);

        m_javaVM->DetachCurrentThread();
        return simStatus;
    }

    int keyboardType ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return -1;
        }

        int keyboardType=env->CallIntMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getKeyboardTypeID);

        m_javaVM->DetachCurrentThread();
        return keyboardType;
    }

    //systeminfo

    QList<int> featuresSupported ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return QList<int> ();
        }
        jintArray jFeatures=(jintArray)env->CallObjectMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getFeaturesAvailableID);

        int length= env->GetArrayLength(jFeatures);
        int *features= env->GetIntArrayElements(jFeatures,NULL);
        QList<int> featuresSupported;
        for(int index=0;index<length;index++)
        {
            featuresSupported.append(features[index]);
        }
        env->ReleaseIntArrayElements(jFeatures,features,0);
        m_javaVM->DetachCurrentThread();
        return featuresSupported;
    }

    QStringList availableLanguages ()
    {
        JNIEnv *env;
        QStringList availableLanguages;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return QStringList ();
        }

        jobjectArray languageArray=(jobjectArray)env->CallObjectMethod(qtSystemInfoJni.m_systemObject (),
                              qtSystemInfoJni.m_getAvailableLanguagesID);
        int arrayLength=env->GetArrayLength(languageArray);

        for(int index=0;index<arrayLength;index++)
        {
            jstring getElement= (jstring)env->GetObjectArrayElement(languageArray,index);
            const char * language=env->GetStringUTFChars((jstring)getElement,NULL);
            availableLanguages.append(QObject::tr(language));
        }
        env->DeleteLocalRef(languageArray);
        m_javaVM->DetachCurrentThread();
        return availableLanguages;
    }

    QString currentCountryCode ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return QString ();
        }

        jstring jCurrentCountryCode=(jstring)env->CallObjectMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getCurrentCountryCodeID);

        const char * cCurrentCountryCode=env->GetStringUTFChars(jCurrentCountryCode,NULL);
        QString currentCountryCode(cCurrentCountryCode);
        env->ReleaseStringUTFChars(jCurrentCountryCode,cCurrentCountryCode);
        m_javaVM->DetachCurrentThread();
        return currentCountryCode;
    }

    QString currentLanguage ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return QString ();
        }

        jstring jCurrentLanguage=(jstring)env->CallObjectMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getCurrentLanguageID);

        const char * cCurrentLanguage=env->GetStringUTFChars(jCurrentLanguage,NULL);
        QString currentLanguage(cCurrentLanguage);
        env->ReleaseStringUTFChars(jCurrentLanguage,cCurrentLanguage);
        m_javaVM->DetachCurrentThread();
        return currentLanguage;
    }


    void registerGeneralInfoupdates(GeneralInfoUpdateListner * listener)
    {
        qtSystemInfoJni.m_generalInfoUpdateListner=listener;

        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return;
        }

        env->CallVoidMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getInitSystemGeneralInfoID);

        m_javaVM->DetachCurrentThread();
    }

    void unregisterGeneralInfoupdates()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return;
        }

        env->CallVoidMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getExitSystemGeneralInfoID);


    }

    static void languageChanged(JNIEnv * env,_jobject,jstring jLanguage)
    {

        const char * language=env->GetStringUTFChars(jLanguage,NULL);
        QString qLanguage=QString(language);

        if(qtSystemInfoJni.m_generalInfoUpdateListner!=NULL)
            qtSystemInfoJni.m_generalInfoUpdateListner->onLanguageChanged
                                                                (qLanguage);
        env->ReleaseStringUTFChars(jLanguage,language);
    }
    QString os ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return QString ();
        }
        jclass clazz=env->FindClass("android/os/Build$VERSION");
        if(clazz==NULL)
        {
            return QString ();
        }
        jfieldID releaseID=env->GetStaticFieldID(clazz,"RELEASE","Ljava/lang/String;");
        jstring jrelease=(jstring)env->GetStaticObjectField(clazz,releaseID);
        const char * release=env->GetStringUTFChars(jrelease,NULL);
        QString os(release);
        env->ReleaseStringUTFChars(jrelease,release);
        m_javaVM->DetachCurrentThread();
        return os;
    }

    ////start of QSystemScreenSaver
    void initScreensaver ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return;
        }

        env->CallVoidMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getInitScreensaverID);

        m_javaVM->DetachCurrentThread();
    }

    void setScreenSaverInhibit ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return;
        }

        env->CallVoidMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getSetScreenSaverInhibitID);

        m_javaVM->DetachCurrentThread();
    }

    void disableScreenSaverInhbit ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return;
        }

        env->CallVoidMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getDisableScreenSaverInhibitID);

        m_javaVM->DetachCurrentThread();
    }


    ////end of QSystemScreenSaver//////


    //display start//

    void initDisplay()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return;
        }

        env->CallVoidMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getInitDisplayID);

        m_javaVM->DetachCurrentThread();
    }

    void exitDisplay()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return;
        }

        env->CallVoidMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getExitDisplayID);

        //m_javaVM->DetachCurrentThread();
    }

    int getDPIHeight ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return -1;
        }

        float DPIHeight=env->CallFloatMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getDPIHeightID);

        m_javaVM->DetachCurrentThread();
        return (int)DPIHeight;
    }

    int getDPIWidth ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return -1;
        }

        float DPIWidth=env->CallFloatMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getDPIWidthID);

        m_javaVM->DetachCurrentThread();
        return (int)DPIWidth;
    }

    int physicalHeight ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return -1;
        }

        float physicalHeight=env->CallFloatMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getPhysicalHeightID);

        m_javaVM->DetachCurrentThread();
        return (int)physicalHeight;
    }

    int physicalWidth ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return -1;
        }

        float physicalWidth=env->CallFloatMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getPhysicalWidthID);

        m_javaVM->DetachCurrentThread();
        return (int)physicalWidth;
    }

    int orientation ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return -1;
        }

        int orientation=env->CallIntMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getOrientationID);

        m_javaVM->DetachCurrentThread();
        return orientation;
    }

    int backLightStatus ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return -1;
        }

        int backLightStatus=env->CallIntMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getBacklightStatusID);

        m_javaVM->DetachCurrentThread();
        return backLightStatus;
    }

    int displayBrightness ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return -1;
        }

        int displayBrightness=env->CallIntMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getDisplayBrightnessID);

        m_javaVM->DetachCurrentThread();
        return displayBrightness;
    }

    float colorDepth ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return -1;
        }

        float colorDepth=env->CallFloatMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getColorDepthID);

        m_javaVM->DetachCurrentThread();
        return colorDepth;
    }

    //end display

    //storage
    void initStorage ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return;
        }

        env->CallVoidMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getInitStorageID);

        m_javaVM->DetachCurrentThread();
    }

    void registerStorageInfoUpdates (StorageInfoUpdateListner *listener)
    {
        qtSystemInfoJni.m_storageInfoUpdateListner=listener;
        initStorage();
    }

    void unregisterStorageInfoUpdates ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return;
        }

        env->CallVoidMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getExitStorageID);

        m_javaVM->DetachCurrentThread();
    }

    static void storageStateChanged(JNIEnv *,_jobject)
    {
        if(qtSystemInfoJni.m_storageInfoUpdateListner!=NULL)
            qtSystemInfoJni.m_storageInfoUpdateListner->onStorageStateChanged();
    }

    static void logicalDriveChanged(JNIEnv * env,_jobject,jstring mountPath,bool added)
    {
        const char * drive=env->GetStringUTFChars(mountPath,NULL);
        QString qDrive=QString(drive);

        if(qtSystemInfoJni.m_storageInfoUpdateListner!=NULL)
            qtSystemInfoJni.m_storageInfoUpdateListner->onLogicalDriveChanged(qDrive,added);

        env->ReleaseStringUTFChars(mountPath,drive);
    }

    //network
    void initNetwork ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return;
        }
        env->CallVoidMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getInitNetworkID);

        m_javaVM->DetachCurrentThread();
    }

    void registerNetworkInfoUpdates (NetworkInfoUpdateListener *listener)
    {
        qtSystemInfoJni.m_networkInfoUpdateListener=listener;
        initNetwork();
    }

    void unregisterNetworkInfoUpdates ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return;
        }
        env->CallVoidMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getExitNetworkID);

        //m_javaVM->DetachCurrentThread();
    }

    QString networkName (int mode)
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return QString ();
        }

        jstring jNetworkName=(jstring)env->CallObjectMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getNetworkNameID,mode);

        const char * cNetworkName=env->GetStringUTFChars(jNetworkName,NULL);
        QString networkName(cNetworkName);
        env->ReleaseStringUTFChars(jNetworkName,cNetworkName);
        m_javaVM->DetachCurrentThread();
        return networkName;
    }

    QString macAddress (int mode)
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return QString ();
        }

        jstring jMacAddress=(jstring)env->CallObjectMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getMacAddressID,mode);

        const char * cMacAddress=env->GetStringUTFChars(jMacAddress,NULL);
        QString macAddress(cMacAddress);
        env->ReleaseStringUTFChars(jMacAddress,cMacAddress);
        m_javaVM->DetachCurrentThread();
        return macAddress;
    }

    int phoneStrength ()
    {
        return qtSystemInfoJni.m_serviceStrength;
    }

    int wifiStrength ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return -1;
        }
        int strength=env->CallIntMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getWifiStrengthID);

        m_javaVM->DetachCurrentThread();
        return strength;
    }

    static void phoneSignalStrengthChanged(JNIEnv *,_jobject,int strength)
    {

        qtSystemInfoJni.m_serviceStrength=strength;
        if(qtSystemInfoJni.m_networkInfoUpdateListener!=NULL)
            qtSystemInfoJni.m_networkInfoUpdateListener->onPhoneStrengthChanged(strength);
    }

    static void wifiSignalStrengthChanged(JNIEnv *,_jobject,int strength)
    {

        qtSystemInfoJni.m_wifiStrength=strength;
        if(qtSystemInfoJni.m_networkInfoUpdateListener!=NULL)
            qtSystemInfoJni.m_networkInfoUpdateListener->onWifiStrengthChanged(strength);
    }

    static void serviceStatusChanged(JNIEnv *,_jobject,int state,int mode)
    {
        qtSystemInfoJni.m_serviceMode=mode;
        qtSystemInfoJni.m_serviceStatus=state;
        if(qtSystemInfoJni.m_networkInfoUpdateListener!=NULL)
            qtSystemInfoJni.m_networkInfoUpdateListener->onNetworkStatusChanged(state,mode);

    }

    static void networkStatusChanged(JNIEnv *,_jobject,int state,int mode)
    {
        qtSystemInfoJni.m_networkMode=mode;
        qtSystemInfoJni.m_networkStatus=state;
        if(qtSystemInfoJni.m_networkInfoUpdateListener!=NULL)
            qtSystemInfoJni.m_networkInfoUpdateListener->onNetworkStatusChanged(state,mode);

    }

    static void wifiStatusChanged(JNIEnv *,_jobject,int state)
    {
        if(qtSystemInfoJni.m_networkInfoUpdateListener!=NULL)
            qtSystemInfoJni.m_networkInfoUpdateListener->onNetworkStatusChanged(state,4);

    }

    int serviceStatus(int mode)
    {
        return (mode==qtSystemInfoJni.m_serviceMode)?qtSystemInfoJni.m_serviceStatus:0;
    }

    int networkStatus(int mode)
    {
        return (mode==qtSystemInfoJni.m_networkStatus)?qtSystemInfoJni.m_networkStatus:0;
    }

    int wifiStatus ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return -1;
        }
        qWarning()<<"the wiifi status is called **************";
        int wifiStatus=env->CallIntMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getWifiStatusID);

        m_javaVM->DetachCurrentThread();
        return wifiStatus;
    }

    int bluetoothStatus ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return -1;
        }

        int bluetoothStatus=env->CallIntMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getBluetoothStatusID);

        m_javaVM->DetachCurrentThread();
        return bluetoothStatus;
    }

    static void networkNameChanged(JNIEnv * env,_jobject,int mode,jstring jname)
    {
        const char * name=env->GetStringUTFChars(jname,NULL);
        QString qName=QString(name);
        if(qtSystemInfoJni.m_networkInfoUpdateListener!=NULL)
            qtSystemInfoJni.m_networkInfoUpdateListener->onNetworkNameChanged(mode,qName);
        env->ReleaseStringUTFChars(jname,name);
    }

    static void  cellIdChanged (JNIEnv *,_jobject,int cellId)
    {

        if(qtSystemInfoJni.m_networkInfoUpdateListener!=NULL)
            qtSystemInfoJni.m_networkInfoUpdateListener->onCellIdChanged(cellId);;
    }

    int cellId ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return -1;
        }
        int cellId=env->CallIntMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getCellID);

        m_javaVM->DetachCurrentThread();
        return cellId;
    }

    QString currentMobileCountryCode ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return QString ();
        }

        jstring jMcc=(jstring)env->CallObjectMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getCurrentCountryCodeID);

        const char * cMcc=env->GetStringUTFChars(jMcc,NULL);
        QString mcc(cMcc);
        env->ReleaseStringUTFChars(jMcc,cMcc);
        m_javaVM->DetachCurrentThread();
        return mcc;
    }

    QString currentMobileNetworkCode ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return QString ();
        }

        jstring jMnc=(jstring)env->CallObjectMethod(qtSystemInfoJni.m_systemObject (),
                            qtSystemInfoJni.m_getCurrentMobileNetworkCodeID);

        const char * cMnc=env->GetStringUTFChars(jMnc,NULL);
        QString mnc(cMnc);
        env->ReleaseStringUTFChars(jMnc,cMnc);
        m_javaVM->DetachCurrentThread();
        return mnc;
    }

    int locationAreaCode ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return -1;
        }
        int locationAreaCode=env->CallIntMethod(qtSystemInfoJni.m_systemObject (),
                        qtSystemInfoJni.m_getLocationAreaCodeID);

        m_javaVM->DetachCurrentThread();
        return locationAreaCode;
    }

    int currentMode ()
    {
        JNIEnv *env;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qWarning()<<"unable to attach to the thread "<<__func__;
            return -1;
        }
        int currentMode=env->CallIntMethod(qtSystemInfoJni.m_systemObject (),
                        qtSystemInfoJni.m_getCurrentModeID);

        m_javaVM->DetachCurrentThread();
        return currentMode;
    }

    JNINativeMethod methods[] = {
        {"BatteryDataUpdated","(Ljava/lang/Object;)V",(void *)BatteryDataUpdated},
        {"bluetoothStateChanged","(Z)V",(void *)bluetoothStateChanged},
        {"profileChanged","(I)V",(void *)profileChanged},
        {"deviceLocked","(Z)V",(void *)deviceLocked},
        {"languageChanged","(Ljava/lang/String;)V",(void *)languageChanged},
        {"storageStateChanged","()V",(void *)storageStateChanged},
        {"logicalDriveChanged","(Ljava/lang/String;Z)V",(void *)logicalDriveChanged},
        {"wifiSignalStrengthChanged","(I)V",(void *)wifiSignalStrengthChanged},
        {"phoneSignalStrengthChanged","(I)V",(void *)phoneSignalStrengthChanged},
        {"serviceStatusChanged","(II)V",(void *)serviceStatusChanged},
        {"networkStatusChanged","(II)V",(void *)networkStatusChanged},
        {"wifiStatusChanged","(I)V",(void *)wifiStatusChanged},
        {"networkNameChanged","(ILjava/lang/String;)V",(void *)networkNameChanged},
        {"cellIdChanged","(I)V",(void *)cellIdChanged},
    };

    static int registerNativeMethods(JNIEnv* env, const char* className,
                                     JNINativeMethod* gMethods, int numMethods)
    {
        jclass clazz;

        clazz = env->FindClass(className);
        if (clazz == NULL)
        {
            __android_log_print(ANDROID_LOG_FATAL,"Qt", "Native registration unable to find class '%s'", className);
            return JNI_FALSE;
        }

        if (env->RegisterNatives(clazz, gMethods, numMethods) < 0)
        {
            __android_log_print(ANDROID_LOG_FATAL,"Qt", "RegisterNatives failed for '%s'", className);
            return JNI_FALSE;
        }

        qtSystemInfoJni.m_systemInfoClass=clazz;

        qtSystemInfoJni.m_getConstructorID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                          m_systemInfoClass (),"<init>","()V");

        //battery
        qtSystemInfoJni.m_getInitBatteryID=env->GetMethodID((jclass)qtSystemInfoJni.
                                               m_systemInfoClass (),"initBattery","()V");

        qtSystemInfoJni.m_getExitBatteryID=env->GetMethodID((jclass)qtSystemInfoJni.
                                               m_systemInfoClass (),"exitBattery","()V");

        //screensaver
        qtSystemInfoJni.m_getSetScreenSaverInhibitID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                          m_systemInfoClass (),"setScreenSaverInhibit"
                                                                                               ,"()V");
        qtSystemInfoJni.m_getInitScreensaverID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"initScreensaver","()V");

        qtSystemInfoJni.m_getDisableScreenSaverInhibitID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                                          m_systemInfoClass (),"DisableScreenSaverInhibit","()V");

        //device
        qtSystemInfoJni.m_getInitDeviceID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"initDevice","()V");
        qtSystemInfoJni.m_getExitDeviceID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"exitDevice","()V");

        qtSystemInfoJni.m_getBluetoothPowerStateID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"bluetoothPowerState","()Z");

        qtSystemInfoJni.m_getImeiID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"imei","()Ljava/lang/String;");

        qtSystemInfoJni.m_getImsiID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"imsi","()Ljava/lang/String;");

        qtSystemInfoJni.m_getIsDeviceLockedID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"isDeviceLocked","()Z");

        qtSystemInfoJni.m_getIsKeyboardFlipOpenedID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"isKeyboardFlipOpened","()Z");

        qtSystemInfoJni.m_getInputMethodTypeID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"inputMethodType","()I");

        qtSystemInfoJni.m_getLockStatusID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"lockStatus","()I");

        qtSystemInfoJni.m_getManufacturerID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"manufacturer","()Ljava/lang/String;");

        qtSystemInfoJni.m_getModelID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"model","()Ljava/lang/String;");

        qtSystemInfoJni.m_getProductNameID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"productName","()Ljava/lang/String;");

        qtSystemInfoJni.m_getSimStatusID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"simStatus","()I");

        qtSystemInfoJni.m_getKeyboardTypeID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"keyboardType","()I");

        //systemInfo
        qtSystemInfoJni.m_getAvailableLanguagesID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"availableLanguages","()[Ljava/lang/String;");

        qtSystemInfoJni.m_getCurrentCountryCodeID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"currentCountryCode","()Ljava/lang/String;");

        qtSystemInfoJni.m_getCurrentLanguageID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"currentLanguage","()Ljava/lang/String;");

        qtSystemInfoJni.m_getInitSystemGeneralInfoID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"initSystemGeneralInfo","()V");

        qtSystemInfoJni.m_getExitSystemGeneralInfoID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"exitSystemGeneralInfo","()V");

        qtSystemInfoJni.m_getFeaturesAvailableID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"featuresAvailable","()[I");

        //display

        qtSystemInfoJni.m_getInitDisplayID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                            m_systemInfoClass (),"initDisplay","()V");

        qtSystemInfoJni.m_getExitDisplayID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                            m_systemInfoClass (),"exitDisplay","()V");

        qtSystemInfoJni.m_getDPIHeightID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                            m_systemInfoClass (),"getDPIHeight","()F");
        qtSystemInfoJni.m_getDPIWidthID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"getDPIWidth","()F");
        qtSystemInfoJni.m_getPhysicalHeightID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"physicalHeight","()F");
        qtSystemInfoJni.m_getPhysicalWidthID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"physicalWidth","()F");
        qtSystemInfoJni.m_getOrientationID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"orientation","()I");
        qtSystemInfoJni.m_getBacklightStatusID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"backLightStatus","()I");
        qtSystemInfoJni.m_getDisplayBrightnessID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"displayBrightness","()I");
        qtSystemInfoJni.m_getColorDepthID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                          m_systemInfoClass (),"colorDepth","()F");
        //storage
        qtSystemInfoJni.m_getInitStorageID =env->GetMethodID((jclass)qtSystemInfoJni.
                                                                    m_systemInfoClass (),"initStorage","()V");

        qtSystemInfoJni.m_getExitStorageID =env->GetMethodID((jclass)qtSystemInfoJni.
                                                                    m_systemInfoClass (),"exitStorage","()V");
        //network
        qtSystemInfoJni.m_getInitNetworkID =env->GetMethodID((jclass)qtSystemInfoJni.
                                                                    m_systemInfoClass (),"initNetwork","()V");

        qtSystemInfoJni.m_getExitNetworkID =env->GetMethodID((jclass)qtSystemInfoJni.
                                                                    m_systemInfoClass (),"exitNetwork","()V");

        qtSystemInfoJni.m_getNetworkNameID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"networkName","(I)Ljava/lang/String;");

        qtSystemInfoJni.m_getMacAddressID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"macAddress","(I)Ljava/lang/String;");

        qtSystemInfoJni.m_getBluetoothStatusID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"bluetoothStatus","()I");

        qtSystemInfoJni.m_getWifiStatusID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"wifiStatus","()I");

        qtSystemInfoJni.m_getWifiStrengthID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"wifiStrength","()I");

        qtSystemInfoJni.m_getCellID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"cellId","()I");


        qtSystemInfoJni.m_getCurrentMobileCountryCodeID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"currentMobileCountryCode","()Ljava/lang/String;");

        qtSystemInfoJni.m_getCurrentMobileNetworkCodeID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"currentMobileNetworkCode","()Ljava/lang/String;");

        qtSystemInfoJni.m_getCurrentModeID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"currentMode","()I");

        qtSystemInfoJni.m_getLocationAreaCodeID=env->GetMethodID((jclass)qtSystemInfoJni.
                                                           m_systemInfoClass (),"locationAreaCode","()I");
        //create object
        qtSystemInfoJni.m_systemObject=env->NewObject((jclass)qtSystemInfoJni.m_systemInfoClass (),
                                                      qtSystemInfoJni.m_getConstructorID);

        return JNI_TRUE;
    }

    static int registerBatteryInfoFields(JNIEnv* env, const char* className)
    {
        jclass clazz;

        clazz = env->FindClass(className);
        if (clazz == NULL)
        {
            __android_log_print(ANDROID_LOG_FATAL,"Qt", "Native registration unable to find class '%s'", className);
            return JNI_FALSE;
        }

        qtSystemInfoJni.m_batteryInfoFields.m_batteryStatusField=env->GetFieldID(clazz,"m_batteryStatus","C");
        qtSystemInfoJni.m_batteryInfoFields.m_chargerTypeField=env->GetFieldID(clazz,"m_chargerType","I");
        qtSystemInfoJni.m_batteryInfoFields.m_chargingStateField=env->GetFieldID(clazz,"m_chargingState","I");
        qtSystemInfoJni.m_batteryInfoFields.m_maxBarsField=env->GetFieldID(clazz,"m_maxBars","I");
        qtSystemInfoJni.m_batteryInfoFields.m_remainingCapacityBarsField=env->GetFieldID(clazz,"m_remainingCapacityBars","I");
        qtSystemInfoJni.m_batteryInfoFields.m_remainingCapacityPercentField=env->GetFieldID(clazz,"m_remainingCapacityPercent","I");
        qtSystemInfoJni.m_batteryInfoFields.m_voltageField=env->GetFieldID(clazz,"m_voltage","I");
        return JNI_TRUE;
    }



}

Q_DECL_EXPORT JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
    QtSystemInfoJni::UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;

    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt","GetEnv failed");
        return -1;
    }

    m_env= uenv.nativeEnvironment;
    m_javaVM = vm;

    QtSystemInfoJni::registerBatteryInfoFields(m_env,QtSystemInfoJni::qtBatteryInfoClassPathName);
    QtSystemInfoJni::registerNativeMethods(m_env,QtSystemInfoJni::qtSystemInfoClassPathName
                                         ,QtSystemInfoJni::methods,NELEM(QtSystemInfoJni::methods));

    return JNI_VERSION_1_4;
}

