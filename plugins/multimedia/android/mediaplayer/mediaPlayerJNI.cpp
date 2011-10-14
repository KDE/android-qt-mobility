/*
Copyright (c) 2011 Elektrobit (EB), All rights reserved.
Contact: oss-devel@elektrobit.com

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
* Neither the name of the Elektrobit (EB) nor the names of its
contributors may be used to endorse or promote products derived from
 this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY Elektrobit (EB) ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Elektrobit
(EB) BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

//JNI file to Interact With Java
#include <QtCore>

#include <android/log.h>
#include <jni.h>

#include "qandroidplayersession.h"
#include "mediaPlayerJNI.h"
#include <QWaitCondition>

static JavaVM *m_javaVM = 0;
static JNIEnv *m_env = 0;
static QWaitCondition waitCondition;
template <class T>
        class JavaMediaPlayerGlobalObject
{
public:
    JavaMediaPlayerGlobalObject(const T & value = NULL)
    {
        if (m_env && value)
        {
            m_value=m_env->NewGlobalRef(value);
        }
        else
        {
            m_value=value;
        }
    };
    virtual ~JavaMediaPlayerGlobalObject()
    {
        if (m_value && m_env)
        {
            m_env->DeleteGlobalRef(m_value);
        }
    };
    T& operator =(const T& value)
                 {
        if (m_value && m_env)
        {
            m_env->DeleteGlobalRef(m_value);
        }
        if (m_env && value)
        {
            m_value=m_env->NewGlobalRef(value);
        }
        else
        {
            m_value=value;
        }
        return m_value;
    }
    T& operator()()
    {
        return m_value;
    }

private:
    T m_value;
};

class autolock
{
private :
        static QMutex mutex;
public :
        autolock()
{
    mutex.lock();
}

~autolock ()
{
    mutex.unlock();
}
};

static QMap<int,QtMediaPlayerJNI::QAndroidMediaListner*> mapOfListeners;
static QtMediaPlayerJNI::QAndroidMediaListner *mediaListener;
static const char *QtMediaPlayerClassPathName = "org/kde/necessitas/mobile/QtMediaPlayer";

struct fields_t {
    jmethodID m_setQtMediaPlayerID;
    jmethodID m_qtMediaPlayerConstructorID;
    JavaMediaPlayerGlobalObject<jobject> m_qtMediaPlayerObject;
    jmethodID m_getCurrentPositionID;
    jmethodID m_playID;
    jmethodID m_pauseID;
    jmethodID m_stopID;
    jmethodID m_seekToID;
    jmethodID m_setVolumeID;
    jmethodID m_resumeID;
    jmethodID m_releaseID;
};
static fields_t fields;
QMutex autolock::mutex;
static QMutex * mutexload=NULL;
void QtMediaPlayerJNI::wait (QMutex * mutex)
{
    mutexload=mutex;
    waitCondition.wait(mutex);
}

void QtMediaPlayerJNI::WakeUpThread()
{
    while(mutexload==NULL);
    mutexload->lock();
    waitCondition.wakeAll();
    mutexload->unlock();
    mutexload = NULL;
}

int QtMediaPlayerJNI::setQtMediaPlayer(QAndroidMediaListner *listnener,int uniqueID,QString path)
{
    JNIEnv* env;
    if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
    {
        qCritical()<<"AttachCurrentThread failed";
        return NULL;
    }
    mediaListener=listnener;
    mapOfListeners.insert(uniqueID,listnener);
    jobject jQtMediaPlayerObject = env->NewObject((jclass)fields.m_qtMediaPlayerObject(),fields.m_qtMediaPlayerConstructorID,(jint)uniqueID);
    jstring jpath = env->NewString((const jchar *)path.unicode(), (long)path.length());
    int ret = env->CallIntMethod(jQtMediaPlayerObject,fields.m_setQtMediaPlayerID,jpath);
    m_javaVM->DetachCurrentThread();
    return ret;
}

void QtMediaPlayerJNI::play()
{
    JNIEnv* env;

    if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
    {
        qCritical()<<"AttachCurrentThread failed";

    }
    WakeUpThread();
    env->CallVoidMethod(fields.m_qtMediaPlayerObject(),fields.m_playID);

}

void QtMediaPlayerJNI::pause()
{
    JNIEnv* env;

    if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
    {
        qCritical()<<"AttachCurrentThread failed";

    }
    env->CallVoidMethod(fields.m_qtMediaPlayerObject(),fields.m_pauseID);
    m_javaVM->DetachCurrentThread();

}

void QtMediaPlayerJNI::stop()
{
    JNIEnv* env;

    if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
    {
        qCritical()<<"AttachCurrentThread failed";

    }
    env->CallVoidMethod(fields.m_qtMediaPlayerObject(),fields.m_stopID);
    m_javaVM->DetachCurrentThread();

}

void QtMediaPlayerJNI::resume()
{
    JNIEnv* env;

    if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
    {
        qCritical()<<"AttachCurrentThread failed";

    }
    env->CallVoidMethod(fields.m_qtMediaPlayerObject(),fields.m_resumeID);
    m_javaVM->DetachCurrentThread();

}

void QtMediaPlayerJNI::release()
{
    JNIEnv* env;

    if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
    {
        qCritical()<<"AttachCurrentThread failed";

    }
    env->CallVoidMethod(fields.m_qtMediaPlayerObject(),fields.m_releaseID);
    m_javaVM->DetachCurrentThread();

}

void QtMediaPlayerJNI::seekTo(int mSec)
{
    JNIEnv* env;

    if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
    {
        qCritical()<<"AttachCurrentThread failed";

    }

    env->CallVoidMethod(fields.m_qtMediaPlayerObject(),fields.m_seekToID,mSec);
    m_javaVM->DetachCurrentThread();

}

void QtMediaPlayerJNI::setVolume(float lVolume, float rVolume)
{
    JNIEnv* env;

    if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
    {
        qCritical()<<"AttachCurrentThread failed";

    }
    env->CallVoidMethod(fields.m_qtMediaPlayerObject(),fields.m_setVolumeID,lVolume,rVolume);
    m_javaVM->DetachCurrentThread();

}

int QtMediaPlayerJNI::getCurrentPosition()
{
    JNIEnv* env;

    if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
    {
        qCritical()<<"AttachCurrentThread failed";

    }
    int position = env->CallIntMethod(fields.m_qtMediaPlayerObject(),fields.m_getCurrentPositionID);
    return position;

}

void QtMediaPlayerJNI::removeListener(int uniqueID)
{
    autolock l;
    mapOfListeners.remove(uniqueID);
}

static int setQtApplicationObject(JNIEnv* env)
{
    jclass clazz;
    clazz = env->FindClass(QtMediaPlayerClassPathName);
    if (clazz == NULL)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt", "Native registration unable to find class '%s'", QtMediaPlayerClassPathName);
        return JNI_FALSE;
    }
    fields.m_qtMediaPlayerObject = clazz;
    return true;
}

void playCompleted(JNIEnv* env,jobject thiz,int uniqueID)
{
    autolock l;
    if(mapOfListeners.contains(uniqueID))
    {
        QtMediaPlayerJNI::QAndroidMediaListner *currListener =mapOfListeners.value(uniqueID);
        currListener->playFinshed();
    }
}

void bufferProgress(JNIEnv* env,jobject thiz,int bufferedPercent,int uniqueID)
{
    autolock l;
    if(mapOfListeners.contains(uniqueID))
    {
        QtMediaPlayerJNI::QAndroidMediaListner *currListener =mapOfListeners.value(uniqueID);
        currListener->updateBufferedProgress(bufferedPercent);
    }
}

static JNINativeMethod methods[] = {
    {"playCompleted", "(I)V", (void *)playCompleted},
    {"bufferProgress", "(II)V", (void *)bufferProgress},
};

static int registerNativeMethods(JNIEnv* env, const char* className,
                                 JNINativeMethod* gMethods, int numMethods)
{
    if (!setQtApplicationObject(env))
    {
        return JNI_FALSE;
    }

    if (env->RegisterNatives((jclass)fields.m_qtMediaPlayerObject(), gMethods, numMethods) < 0)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt", "RegisterNatives failed for '%s'", className);
        return JNI_FALSE;
    }

    fields.m_setQtMediaPlayerID = env->GetMethodID((jclass)fields.m_qtMediaPlayerObject(),"setQtMediaPlayer","(Ljava/lang/String;)I");
    fields.m_qtMediaPlayerConstructorID = env->GetMethodID((jclass)fields.m_qtMediaPlayerObject(),"<init>","(I)V");
    fields.m_getCurrentPositionID = env->GetMethodID((jclass)fields.m_qtMediaPlayerObject(),"getCurrentPosition","()I");
    fields.m_playID = env->GetMethodID((jclass)fields.m_qtMediaPlayerObject(),"play","()V");
    fields.m_pauseID = env->GetMethodID((jclass)fields.m_qtMediaPlayerObject(),"pause","()V");
    fields.m_stopID = env->GetMethodID((jclass)fields.m_qtMediaPlayerObject(),"stop","()V");
    fields.m_seekToID =env->GetMethodID((jclass)fields.m_qtMediaPlayerObject(),"seekTo","(I)V");
    fields.m_setVolumeID = env->GetMethodID((jclass)fields.m_qtMediaPlayerObject(),"setVolume","(FF)V");
    fields.m_resumeID = env->GetMethodID((jclass)fields.m_qtMediaPlayerObject(),"resume","()V");
    fields.m_releaseID = env->GetMethodID((jclass)fields.m_qtMediaPlayerObject(),"release","()V");

    return JNI_TRUE;
}

static int registerNatives(JNIEnv* env)
{
    if (!registerNativeMethods(env, QtMediaPlayerClassPathName, methods, sizeof(methods) / sizeof(methods[0])))
        return JNI_FALSE;

    return JNI_TRUE;
}

typedef union {
    JNIEnv* nativeEnvironment;
    void* venv;
} UnionJNIEnvToVoid;

Q_DECL_EXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
    __android_log_print(ANDROID_LOG_INFO,"Qt", "qt media plugin start");
    UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;
    m_javaVM = 0;

    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt","GetEnv failed");
        return -1;
    }
    m_env = uenv.nativeEnvironment;
    if (!registerNatives(m_env))
    {
        __android_log_print(ANDROID_LOG_FATAL, "Qt", "registerNatives failed");
        return -1;
    }
    m_javaVM = vm;
    return JNI_VERSION_1_4;
}
