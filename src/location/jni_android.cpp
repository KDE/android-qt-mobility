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

#include<jni_android.h>
#include<QDebug>
#include <android/log.h>
#include <QMutex>

#ifndef NELEM
# define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
#endif

static JNIEnv *m_env = NULL;
static JavaVM *m_javaVM = NULL;

template <class T>
        class LocationGlobalObject
{
public:
    LocationGlobalObject(const T & value= NULL)
    {
        if (m_env)
            m_value=m_env->NewGlobalRef(value);
        else
            m_value=value;
    };
    virtual ~LocationGlobalObject()
    {
        if (m_value && m_env)
            m_env->DeleteGlobalRef(m_value);
    };
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

namespace QtLocationJni
{
    typedef union {
        JNIEnv* nativeEnvironment;
        void* venv;
    } UnionJNIEnvToVoid;

    static const char *qtLocationClassPathName = "eu/licentia/necessitas/mobile/QtLocation";
    static const char *qtSatInfoClassPathName="eu/licentia/necessitas/mobile/QtSatInfo";
    static const char *GPS_PROVIDER="gps";
    static const char *NETWORK_PROVIDER="network";

    class LocationAutolock
    {
    private:
        static QMutex mutex;
    public:
        LocationAutolock()
        {
            mutex.lock();
        }
        ~LocationAutolock()
        {
            mutex.unlock();
        }

    };

    QMutex LocationAutolock::mutex;

    //get method ID's
    struct QtLocationJniStruct
    {
        //for location
        jmethodID m_getConstructorID;
        jmethodID m_getsupportedPositioningMethodsID;
        jmethodID m_getLastKnownPositionID;
        jmethodID m_getRequestUpdatesID;
        jmethodID m_getDisableUpdatesID;
        LocationGlobalObject<jobject> m_locationClass;
        LocationGlobalObject<jobject> m_locationObject;
        QList<LocationAndroidListner *> m_listnerList;
        qint64 m_supportedPositioningMethods;
        //for satellites
        jmethodID m_getSatelliteRequestUpdatesID;
        jmethodID m_getSatelliteDisableUpdatesID;
        jfieldID m_prnNumberField;
        jfieldID m_signalStrengthField;
        jfieldID m_elevationField;
        jfieldID m_azimuthField;
        LocationGlobalObject<jobject> m_satInfoClass;
        QList<SatelliteInfoAndroidListner *> m_satelliteListnerList;
    };

    static QtLocationJniStruct qtLocationJni;

    static void locationDataUpdated(JNIEnv * env,_jobject,jdoubleArray dataArray,jstring provider)
    {
        double * data= env->GetDoubleArrayElements(dataArray,NULL);
        LocationAndroidListner * callListner;
        const char * providerName=env->GetStringUTFChars(provider,NULL);

        LocationAutolock _l;
        foreach(callListner,qtLocationJni.m_listnerList)
        {
            callListner->callListner(providerName,data);
        }

        env->ReleaseDoubleArrayElements(dataArray,data,0);
        env->ReleaseStringUTFChars(provider,providerName);

    }


    static void gpsSatelliteDataUpdated (JNIEnv * env,_jobject,jobjectArray satInfo)
    {
        int arrayLength=env->GetArrayLength(satInfo);
        QList<SatelliteInfo> satInfoList;
        for(int index=0;index<arrayLength;index++)
        {
            jobject getElement= env->GetObjectArrayElement(satInfo,index);
            SatelliteInfo satInfo;
            satInfo.m_prnNumber=env->GetIntField(getElement,qtLocationJni.m_prnNumberField);
            satInfo.m_azimuth=env->GetFloatField(getElement,qtLocationJni.m_azimuthField);
            satInfo.m_elevation=env->GetFloatField(getElement,qtLocationJni.m_elevationField);
            satInfo.m_signalStrength=env->GetFloatField(getElement
                                                       ,qtLocationJni.m_signalStrengthField);
            satInfoList.append(satInfo);
        }

        SatelliteInfoAndroidListner *callSatelliteInfoAvailable;
        LocationAutolock _l;
        foreach(callSatelliteInfoAvailable,qtLocationJni.m_satelliteListnerList)
        {
            callSatelliteInfoAvailable->onSatelliteInfoAvailable(satInfoList);
        }
    }


    JNINativeMethod methods[] = {
        {"locationDataUpdated", "([DLjava/lang/String;)V", (void *)locationDataUpdated},
        {"gpsSatelliteDataUpdated","([Ljava/lang/Object;)V",(void*)gpsSatelliteDataUpdated}
    };


    qint64 getSupportedPositioningMethods (JNIEnv* env)
    {

        qint64 retPositionMethod= env->CallIntMethod(qtLocationJni.m_locationObject (),
                                                     qtLocationJni.m_getsupportedPositioningMethodsID);
        return retPositionMethod;
    }

    qint64 supportedPositioningMethods()
    {
        return qtLocationJni.m_supportedPositioningMethods;
    }

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

        qtLocationJni.m_locationClass=clazz;


        //location api's
        qtLocationJni.m_getConstructorID=env->GetMethodID((jclass)qtLocationJni.
                                                          m_locationClass (),"<init>","()V");

        qtLocationJni.m_getLastKnownPositionID=env->GetMethodID((jclass)qtLocationJni.
                                                                m_locationClass (),"lastKnownPosition","(I)[D");

        qtLocationJni.m_getRequestUpdatesID=env->GetMethodID((jclass)qtLocationJni.m_locationClass (),
                                                             "requestUpdates","(J)V");

        qtLocationJni.m_getDisableUpdatesID=env->GetMethodID((jclass)qtLocationJni.m_locationClass (),
                                                             "disableUpdates","()V");

        qtLocationJni.m_getsupportedPositioningMethodsID=env->GetMethodID((jclass)qtLocationJni.m_locationClass (),
                                                                          "supportedPositiongMethods","()J");

        //satellite api's
        qtLocationJni.m_getSatelliteRequestUpdatesID=env->GetMethodID((jclass)qtLocationJni.m_locationClass (),
                                                                      "requestSatelliteUpdates","()V");

        qtLocationJni.m_getSatelliteDisableUpdatesID=env->GetMethodID((jclass)qtLocationJni.m_locationClass (),
                                                                      "disableSatelliteUpdates","()V");

        //create object
        qtLocationJni.m_locationObject=env->NewObject((jclass)qtLocationJni.m_locationClass (),
                                                      qtLocationJni.m_getConstructorID);

        //get the positioning methods supported since it will be the same through out the lifetime
        //of the system
        qtLocationJni.m_supportedPositioningMethods=getSupportedPositioningMethods(env);
        return JNI_TRUE;
    }

    static int registerSatInfoFields(JNIEnv* env, const char* className)
    {
        jclass clazz;

        clazz = env->FindClass(className);
        if (clazz == NULL)
        {
            __android_log_print(ANDROID_LOG_FATAL,"Qt", "Native registration unable to find class '%s'", className);
            return JNI_FALSE;
        }

        qtLocationJni.m_satInfoClass=clazz;
        qtLocationJni.m_prnNumberField=env->GetFieldID(clazz,"m_prnNumber","I");
        qtLocationJni.m_signalStrengthField=env->GetFieldID(clazz,"m_signalStrength","F");
        qtLocationJni.m_elevationField=env->GetFieldID(clazz,"m_elevation","F");
        qtLocationJni.m_azimuthField=env->GetFieldID(clazz,"m_azimuth","F");
        return JNI_TRUE;
    }


    void requestUpdates (uint64_t minTime,LocationAndroidListner * locationListner)
    {
        JNIEnv *env;
        LocationAutolock _l;
        static uint64_t requestTime=minTime;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return;
        }

        if(!qtLocationJni.m_listnerList.contains(locationListner))
        {
            requestTime=(minTime<requestTime)?minTime:requestTime;
            env->CallVoidMethod(qtLocationJni.m_locationObject ()
                                ,qtLocationJni.m_getRequestUpdatesID,requestTime);

            qtLocationJni.m_listnerList.append(locationListner);
        }
        m_javaVM->DetachCurrentThread();
    }

    void disableUpdates (LocationAndroidListner * locationListner)
    {
        JNIEnv *env;
        LocationAutolock _l;
        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return;
        }

        if(qtLocationJni.m_listnerList.contains(locationListner))
        {
            qtLocationJni.m_listnerList.removeOne(locationListner);
            if (qtLocationJni.m_listnerList.isEmpty())
            {
                env->CallVoidMethod(qtLocationJni.m_locationObject ()
                                    ,qtLocationJni.m_getDisableUpdatesID);
            }
        }

        m_javaVM->DetachCurrentThread();
    }

    void lastKnownPosition(double *lastKnownPositionData,int dataSize,bool fromSatellitePositioningMethodsOnly)
    {
        JNIEnv *env;
        jdoubleArray dataArray;
        double *data;
        int arrayLength;

        if(m_javaVM->AttachCurrentThread(&env,NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }

        dataArray=(jdoubleArray)env->CallObjectMethod(qtLocationJni.m_locationObject (),
                                                      qtLocationJni.m_getLastKnownPositionID
                                                      ,fromSatellitePositioningMethodsOnly);

        data =env->GetDoubleArrayElements(dataArray,NULL);
        arrayLength=env->GetArrayLength(dataArray);
        arrayLength=(dataSize>arrayLength)?arrayLength:dataSize;
        qMemCopy(lastKnownPositionData,data,arrayLength * sizeof(double));

        env -> ReleaseDoubleArrayElements(dataArray, data, 0);
        m_javaVM->DetachCurrentThread();
    }

    void satelliteRequestupdates (SatelliteInfoAndroidListner * listner)
    {
        JNIEnv * env;
        LocationAutolock _l;
        if(m_javaVM->AttachCurrentThread(&env,NULL))
        {
            qCritical()<<"AttachCurrentThread failed";
            return;
        }

        if(!qtLocationJni.m_satelliteListnerList.contains(listner))
        {
            env->CallVoidMethod(qtLocationJni.m_locationObject ()
                                ,qtLocationJni.m_getSatelliteRequestUpdatesID);

            qtLocationJni.m_satelliteListnerList.append(listner);
        }
        m_javaVM->DetachCurrentThread();
    }

    void satelliteDisableupdates(SatelliteInfoAndroidListner *listner)
    {
        JNIEnv * env;
        LocationAutolock _l;
        if(m_javaVM->AttachCurrentThread(&env,NULL))
        {
            qCritical()<<"AttachCurrentThread failed";
            return;
        }

        if(qtLocationJni.m_satelliteListnerList.contains(listner))
        {
            qtLocationJni.m_satelliteListnerList.removeOne(listner);
            if (qtLocationJni.m_satelliteListnerList.isEmpty())
            {
                env->CallVoidMethod(qtLocationJni.m_locationObject (),
                                    qtLocationJni.m_getSatelliteDisableUpdatesID);
            }
        }
        m_javaVM->DetachCurrentThread();

    }

    void LocationAndroidListner::setProvider(LocationProviders provider)
    {
        switch(provider)
        {
        case GPS:
            m_provider = GPS_PROVIDER;
            break;
        case OTHERS:
            m_provider = NETWORK_PROVIDER;
            break;
        case ALL:
            m_provider = GPS_PROVIDER;
            m_provider += NETWORK_PROVIDER;
            break;
        }
    }

    void LocationAndroidListner::callListner(const char * provider,double * data)
    {
        if(m_provider.contains(QObject::tr(provider)))
            this->onLocationChanged(data);
    }

    LocationAndroidListner::LocationAndroidListner ()
    {
        m_provider = GPS_PROVIDER;
        m_provider += NETWORK_PROVIDER;
    }

}

Q_DECL_EXPORT JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
    QtLocationJni::UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;

    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt","GetEnv failed");
        return -1;
    }

    m_env= uenv.nativeEnvironment;
    m_javaVM = vm;

    QtLocationJni::registerNativeMethods(m_env,QtLocationJni::qtLocationClassPathName
                                         ,QtLocationJni::methods,NELEM(QtLocationJni::methods));

    QtLocationJni::registerSatInfoFields(m_env,QtLocationJni::qtSatInfoClassPathName);
    return JNI_VERSION_1_4;
}
