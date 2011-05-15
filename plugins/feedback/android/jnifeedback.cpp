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

#include<jnifeedback.h>


static JNIEnv *m_env = NULL;
static JavaVM *m_javaVM = NULL;

template <class T>
        class FeedbackGlobalObject
{
public:
    FeedbackGlobalObject(const T & value= NULL)
    {
        if (m_env)
            m_value=m_env->NewGlobalRef(value);
        else
            m_value=value;
    };
    virtual ~FeedbackGlobalObject()
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

namespace QtFeedbackJNI

{
    typedef union {
        JNIEnv* nativeEnvironment;
        void* venv;
    } UnionJNIEnvToVoid;

    struct JniMethods
    {
    jmethodID m_playThemeEffectID;
    jmethodID m_getFeedbackConstructorID;
    jmethodID m_loadMusicFileID;
    jmethodID m_setFileStateID;
    jmethodID m_setVibrateStateID;
    jmethodID m_setVibraDurPerID;
    jmethodID m_setHapticFeedbackID;
    jmethodID m_performHapticFeedbackID;
    jmethodID m_setHapticIntensityID;
    jfieldID m_getFileDurID;


    FeedbackGlobalObject<jobject> m_feedbackObject;
    FeedbackGlobalObject<jobject> m_feedbackInstance;

    const char *QtFeedbackClassPathName;

     };
    static struct JniMethods jniMethods;
    void playThemeEffect(QtFeedbackJNI::AndroidThemeEffect& themeEffect)
    {
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }


        env->CallVoidMethod((jobject)jniMethods.m_feedbackInstance (),jniMethods.m_playThemeEffectID,themeEffect);
        m_javaVM->DetachCurrentThread();

    }

    void loadMusicFile(QString & filepath)
    {
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }

        jstring jid = env->NewString((const jchar *) filepath.unicode(), (long) filepath.length());
        env->CallVoidMethod((jobject)jniMethods.m_feedbackInstance (),jniMethods.m_loadMusicFileID,jid);
        env->DeleteLocalRef(jid);
        m_javaVM->DetachCurrentThread();

    }

    void setFileState(QtFeedbackJNI::State & state)
    {
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }

        env->CallVoidMethod((jobject)jniMethods.m_feedbackInstance (),jniMethods.m_setFileStateID,state);
        m_javaVM->DetachCurrentThread();

    }

    int getFileDur()
    {
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return 0;
        }

        int fileDur = (int) env->GetIntField((jobject)jniMethods.m_feedbackInstance (),jniMethods.m_getFileDurID);
        m_javaVM->DetachCurrentThread();
        return fileDur;

    }

    void setVibraDurPer(int duration,int period,int resumeTime)
    {
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }

        env->CallVoidMethod((jobject)jniMethods.m_feedbackInstance (),jniMethods.m_setVibraDurPerID,duration,period,resumeTime);
        m_javaVM->DetachCurrentThread();

    }

    void setVibraState(QtFeedbackJNI::State & state)
    {
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }

        env->CallVoidMethod((jobject)jniMethods.m_feedbackInstance (),jniMethods.m_setVibrateStateID,state);
        m_javaVM->DetachCurrentThread();

    }

    void setHapticFeedback(bool enabled)
    {

        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }
        env->CallVoidMethod((jobject)jniMethods.m_feedbackInstance (),jniMethods.m_setHapticFeedbackID,enabled);
        m_javaVM->DetachCurrentThread();

    }

    void performHapticFeedback()
    {
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }

        env->CallVoidMethod((jobject)jniMethods.m_feedbackInstance (),jniMethods.m_performHapticFeedbackID);
        m_javaVM->DetachCurrentThread();

    }

    void setHapticIntensity(QtFeedbackJNI::FeedbackIntensity & feedbackIntensity)
    {
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }

        env->CallVoidMethod((jobject)jniMethods.m_feedbackInstance (),jniMethods.m_setHapticIntensityID,feedbackIntensity);
        m_javaVM->DetachCurrentThread();



    }





    static int getMethodIDs(JNIEnv* env, const char* className)
    {
        jclass clazz;

        clazz = env->FindClass(className);
        if (clazz == NULL)
        {
            return JNI_FALSE;
        }

        jniMethods.m_feedbackObject = clazz;
        jniMethods.m_playThemeEffectID = env->GetMethodID((jclass)jniMethods.m_feedbackObject (), "playThemeEffect", "(I)V");
        jniMethods.m_loadMusicFileID = env->GetMethodID((jclass)jniMethods.m_feedbackObject (), "loadMusicFile", "(Ljava/lang/String;)V");
        jniMethods.m_setFileStateID = env->GetMethodID((jclass)jniMethods.m_feedbackObject (), "setFileState", "(I)V");
        jniMethods.m_getFileDurID = env->GetFieldID((jclass)jniMethods.m_feedbackObject (), "m_getFileDur", "I");
        jniMethods.m_setVibrateStateID = env->GetMethodID((jclass)jniMethods.m_feedbackObject (), "setVibrateState", "(I)V");
        jniMethods.m_setVibraDurPerID = env->GetMethodID((jclass)jniMethods.m_feedbackObject (), "setVibraDuration", "(III)V");
        jniMethods.m_setHapticFeedbackID = env->GetMethodID((jclass)jniMethods.m_feedbackObject (), "setHapticFeedback", "(Z)V");
        jniMethods.m_performHapticFeedbackID = env->GetMethodID((jclass)jniMethods.m_feedbackObject (), "performHapticFeedback", "()V");
        jniMethods.m_setHapticIntensityID = env->GetMethodID((jclass)jniMethods.m_feedbackObject (), "setHapticIntensity", "(I)V");
        jniMethods.m_getFeedbackConstructorID=env->GetMethodID((jclass)jniMethods.m_feedbackObject (),"<init>","()V");
        jniMethods.m_feedbackInstance=env->NewObject((jclass)jniMethods.m_feedbackObject (), jniMethods.m_getFeedbackConstructorID);

        return 0;
    }




}

Q_DECL_EXPORT JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
    QtFeedbackJNI::UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;


    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK)
    {
        return -1;
    }

    m_env= uenv.nativeEnvironment;
    m_javaVM = vm;
    QtFeedbackJNI::jniMethods.QtFeedbackClassPathName = "eu/licentia/necessitas/mobile/QtFeedback";
    QtFeedbackJNI::getMethodIDs(m_env,QtFeedbackJNI::jniMethods.QtFeedbackClassPathName);
    return JNI_VERSION_1_4;
}

