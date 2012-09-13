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

#include <sensorjni.h>
#include <android/log.h>
#include <QDebug>
#include <QMutex>

static JNIEnv *m_env = NULL;
static JavaVM *m_javaVM = NULL;

template <class T>
        class SensorGlobalObject
{
public:
    SensorGlobalObject(const T & value= NULL)
    {
        if (m_env)
            m_value=m_env->NewGlobalRef(value);
        else
            m_value=value;
    };
    virtual ~SensorGlobalObject()
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

namespace QtSensorJNI
{

    typedef union {
        JNIEnv* nativeEnvironment;
        void* venv;
    } UnionJNIEnvToVoid;

    class SensorAutoLock
    {
        private:
            static QMutex mutex;
        public:
            SensorAutoLock ()
            {
                mutex.lock();
            }

            ~SensorAutoLock ()
            {
                mutex.unlock();
            }
    };

    QMutex SensorAutoLock::mutex;

    //get method ID's
    struct QtSensorJniStruct
    {
        jmethodID m_getSensorConstructorID;
        jmethodID m_getSensorStopID;
        jmethodID m_getSensorRegisterSensorsID;
        QMap<int,QPair< jobject, SensorAndroidListener *> > m_sensorBackendMapper;
        SensorGlobalObject<jobject> m_sensorsClass;
    };

    //path to the java plugin qtsensors
    static const char qtSensorsClassPathName[] = "org/kde/necessitas/mobile/QtSensors";
    static QtSensorJniStruct qtSensorJni;

    static void slotDataAvailable (JNIEnv* env,_jobject /*object*/,jfloatArray data,jlong timeEvent,jint accuracy,jint uniqueId)
    {

        jfloat* getData =(jfloat*)env->GetFloatArrayElements(data,0);

        SensorAutoLock lock;
        if(qtSensorJni.m_sensorBackendMapper.contains(uniqueId))
        {
            qtSensorJni.m_sensorBackendMapper[uniqueId].second->
                    dataAvailable(getData,timeEvent,accuracy);
        }

        env->ReleaseFloatArrayElements(data,getData,0);
    }


    JNINativeMethod methods[] = {
        {"slotDataAvailable", "([FJII)V", (void *)slotDataAvailable}
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

        qtSensorJni.m_sensorsClass = clazz;
        qtSensorJni.m_getSensorConstructorID=env->GetMethodID((jclass)qtSensorJni.m_sensorsClass(),"<init>","(III)V");
        qtSensorJni.m_getSensorStopID = env->GetMethodID((jclass)qtSensorJni.m_sensorsClass(), "stop", "()V");
        qtSensorJni.m_getSensorRegisterSensorsID=env->GetStaticMethodID((jclass)qtSensorJni.m_sensorsClass(), "registerSensors", "()V");
        return JNI_TRUE;
    }


    void start (int datarate,int uniqueID,int sensorType
                ,SensorAndroidListener * sensorAndroidListner)
    {

        SensorAutoLock lock;
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }

        //calling the constructor and then creating an instance of QtSensor from Java
        //we dont delete it because the grabage collecter will take care of it
        if(!qtSensorJni.m_sensorBackendMapper.contains(uniqueID))
        {
            jobject sensorsInstance=
                    env->NewObject((jclass)qtSensorJni.m_sensorsClass ()
                                   , qtSensorJni.m_getSensorConstructorID
                                   ,uniqueID,datarate,sensorType);

            qtSensorJni.m_sensorBackendMapper[uniqueID].first=env->NewGlobalRef(sensorsInstance);
            qtSensorJni.m_sensorBackendMapper[uniqueID].second=sensorAndroidListner;
        }

        m_javaVM->DetachCurrentThread();

    }

    void  stop(int uniqueID)
    {

        SensorAutoLock lock;
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }

        if(qtSensorJni.m_sensorBackendMapper.contains(uniqueID))
        {
            env->CallVoidMethod(qtSensorJni.m_sensorBackendMapper[uniqueID].first
                                ,qtSensorJni.m_getSensorStopID);
            env->DeleteGlobalRef(qtSensorJni.m_sensorBackendMapper[uniqueID].first);
            qtSensorJni.m_sensorBackendMapper.remove(uniqueID);

        }

        m_javaVM->DetachCurrentThread();

    }

    void  registerSensor()
    {
        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }
        env->CallStaticVoidMethod((jclass)qtSensorJni.m_sensorsClass()
                            ,qtSensorJni.m_getSensorRegisterSensorsID);
        m_javaVM->DetachCurrentThread();
    }
}

Q_DECL_EXPORT JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
    QtSensorJNI::UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;

    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt","GetEnv failed");
        return -1;
    }

    m_env= uenv.nativeEnvironment;
    m_javaVM = vm;

    QtSensorJNI::registerNativeMethods(m_env,QtSensorJNI::qtSensorsClassPathName
                                       ,QtSensorJNI::methods,1);

    return JNI_VERSION_1_4;
}
