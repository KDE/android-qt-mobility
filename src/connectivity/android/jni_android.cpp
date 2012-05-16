/*
    Copyright (c) 2011-2012, Lauri Laanmets <lauri.laanmets@proekspert.ee>
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
        * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
        * Neither the name of Lauri Laanmets (AS Proekspert) nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY Lauri Laanmets (AS Proekspert) ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL Lauri Laanmets (AS Proekspert) BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "jnithreadhelper.h"
#include <qbluetoothlocaldevice_p.h>
#include <android/log.h>
#include "androidbroadcastreceiver.h"


Q_DECL_EXPORT JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
    //QtConnectivityJni::UnionJNIEnvToVoid uenv;
    void* venv = NULL;
    JNIEnv* nativeEnvironment = NULL;

    if (vm->GetEnv(&venv, JNI_VERSION_1_4) != JNI_OK)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt","GetEnv failed");
        return -1;
    }
    nativeEnvironment = reinterpret_cast<JNIEnv*>(venv);

    JNIThreadHelper::setJvm(vm);
    jclass qtApplicationC;

    qtApplicationC = nativeEnvironment->FindClass("android/app/Application");
    if (qtApplicationC == NULL)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt", "Native registration unable to find class android.app.Application");
        return JNI_FALSE;
    }
    JNIThreadHelper::setAppContextClass(qtApplicationC);

    /*
    jclass qtNativeClass = nativeEnvironment->FindClass("org/kde/necessitas/industrius/QtNative");
    jmethodID activityID = nativeEnvironment->GetStaticMethodID(qtNativeClass, "activity", "()Landroid/app/Activity;");
    jobject mainContext = nativeEnvironment->CallStaticObjectMethod(qtNativeClass, activityID);
    */

    QTM_NAMESPACE::QBluetoothLocalDevicePrivate::initialize(nativeEnvironment);
    AndroidBroadcastReceiver::loadJavaClass(nativeEnvironment);

    return JNI_VERSION_1_4;
}

/*
JNIEXPORT void JNICALL Java_JNIRunnable_jniRun(JNIEnv *env, jobject javaObject, jint qtObject){
    env = env;
    reinterpret_cast<QBluetoothSocketPrivate*>(qtObject)->jniCallback();
}
static JNINativeMethod methods[] = {
    {"jniRun",    "(I)V", (void *)&Java_JNIRunnable_jniRun},
};*/
