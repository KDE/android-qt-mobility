/****************************************************************************
**
** Copyright 2010 Elektrobit(EB)(http://www.elektrobit.com)
**
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
**
****************************************************************************/

#include <sensorjni.h>
#include <android/log.h>
#include <QDebug>

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

    //get method ID's
    struct QtSensorJniStruct
    {
        jmethodID m_getSensorConstructorID;
        jmethodID m_getSensorStopID;
        jmethodID m_getSensorRegisterSensorsID;
        QMap<int,QPair<jobject,SensorAndroidListener *> > m_sensorBackendMapper;
        SensorGlobalObject<jobject> m_sensorsClass;
    };

    //path to the java plugin qtsensors
    static const char *qtSensorsClassPathName = "com/nokia/qt/android/QtSensors";
    static QtSensorJniStruct qtSensorJni;

    static void slotDataAvailable (JNIEnv* env,_jobject /*object*/,jfloatArray data,jlong timeEvent,jint accuracy,jint uniqueId)
    {

        jfloat* getData =(jfloat*)env->GetFloatArrayElements(data,0);

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
        qtSensorJni.m_getSensorConstructorID=env->GetMethodID((jclass)qtSensorJni.m_sensorsClass (),"<init>","(III)V");
        qtSensorJni.m_getSensorStopID = env->GetMethodID((jclass)qtSensorJni.m_sensorsClass(), "stop", "()V");
        qtSensorJni.m_getSensorRegisterSensorsID=env->GetStaticMethodID((jclass)qtSensorJni.m_sensorsClass(), "registerSensors", "()V");

        return JNI_TRUE;
    }


    void start (int datarate,int uniqueID,int sensorType
                ,SensorAndroidListener * sensorAndroidListner)
    {
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

            qtSensorJni.m_sensorBackendMapper[uniqueID].first=sensorsInstance;
            qtSensorJni.m_sensorBackendMapper[uniqueID].second=sensorAndroidListner;
        }

        m_javaVM->DetachCurrentThread();

    }

    void  stop(int uniqueID)
    {
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }

        if(qtSensorJni.m_sensorBackendMapper.contains(uniqueID))
        {
            env->CallVoidMethod((jobject)qtSensorJni.m_sensorBackendMapper[uniqueID].first
                                ,qtSensorJni.m_getSensorStopID);

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

        env->CallVoidMethod((jobject)qtSensorJni.m_sensorsClass ()
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
