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

#include <android/log.h>
#include "androidbroadcastreceiver.h"
#include "jnithreadhelper.h"
#include <QtGui/QPlatformNativeInterface>
#include <QtGui/QApplication>


static JNINativeMethod methods[] = {
    {"jniOnReceive",    "(ILandroid/content/Context;Landroid/content/Intent;)V",                    (void *)&Java_eu_licentia_necessitas_industrius_QtBroadcastReceiver_jniOnReceive},
};

jobject AndroidBroadcastReceiver::jActivityObject = NULL;
jclass AndroidBroadcastReceiver::jQtBroadcastReceiverClass = NULL;
jclass AndroidBroadcastReceiver::jIntentFilterClass = NULL;

void AndroidBroadcastReceiver::initialize(JNIThreadHelper& env, jclass appClass, jobject mainActivity){
    jActivityObject = mainActivity;

    jIntentFilterClass = env->FindClass("android/content/IntentFilter");
    if(jIntentFilterClass == NULL){
        __android_log_print(ANDROID_LOG_FATAL,"Qt","Cannot find android.content.IntentFilter");
    }
    env->RegisterNatives(jQtBroadcastReceiverClass, methods, 1);
}

void AndroidBroadcastReceiver::loadJavaClass(JNIEnv *env)
{
    jQtBroadcastReceiverClass = env->FindClass("org/kde/necessitas/industrius/QtBroadcastReceiver");
    if(jQtBroadcastReceiverClass == NULL){
        __android_log_print(ANDROID_LOG_FATAL,"Qt","Cannot find org/kde/necessitas/industrius/QtBroadcastReceiver");
        env->ExceptionClear();
    }
}

AndroidBroadcastReceiver::AndroidBroadcastReceiver(QObject* parent): QObject(parent)
{
    if(jQtBroadcastReceiverClass != NULL){
        JNIThreadHelper env;

        if(jActivityObject==NULL){
            QPlatformNativeInterface* nativeInterface = QApplication::platformNativeInterface();
            if(nativeInterface == NULL){
                __android_log_print(ANDROID_LOG_FATAL,"Qt", "QApplication::platformNativeInterface(); returned NULL. Unable to initialize connectivity module.");
                goto error;
            }
            void* pointer = QApplication::platformNativeInterface()->nativeResourceForWidget("ApplicationContext", 0);
            if(pointer == NULL){
                qDebug() << "nativeResourceForWidget(\"ApplicationContext\", 0)" << " - failed.";
                __android_log_print(ANDROID_LOG_FATAL,"Qt", "nativeResourceForWidget(\"ApplicationContext\", 0) returned NULL\nProbably QtGui lib is too old.");
                goto error;
            }else{
                jActivityObject = reinterpret_cast<jobject>(pointer);
                initialize(env, JNIThreadHelper::appContextClass(), jActivityObject);
            }
        }

        jmethodID jQtBroadcastReceiverConstructorID = env->GetMethodID(jQtBroadcastReceiverClass, "<init>", "()V"); // no parameters
        jQtBroadcastReceiverObject = env->NewObject(jQtBroadcastReceiverClass, jQtBroadcastReceiverConstructorID);
        jQtBroadcastReceiverObject = env->NewGlobalRef(jQtBroadcastReceiverObject);

        jfieldID jQtObjectFieldID = env->GetFieldID(jQtBroadcastReceiverClass, "qtObject", "I");
        env->SetIntField(jQtBroadcastReceiverObject, jQtObjectFieldID, reinterpret_cast<int>(this));

        jmethodID intentFilterConstructorID = env->GetMethodID(jIntentFilterClass, "<init>", "()V"); // no parameters
        jIntentFilterObject = env->NewObject(jIntentFilterClass, intentFilterConstructorID);
        jIntentFilterObject = env->NewGlobalRef(jIntentFilterObject);
    }
    error:;
}

AndroidBroadcastReceiver::~AndroidBroadcastReceiver(){
    unregisterReceiver();

    JNIThreadHelper env;
    env->DeleteGlobalRef(jQtBroadcastReceiverObject);
    env->DeleteGlobalRef(jIntentFilterObject);
}

void AndroidBroadcastReceiver::unregisterReceiver(){
    if(jActivityObject==NULL) return;

    JNIThreadHelper env;
    jclass jContextClass = env->FindClass("android/app/Application");
    jmethodID unregisterReceiverMethodID = env->GetMethodID(jContextClass, "unregisterReceiver", "(Landroid/content/BroadcastReceiver;)V");
    env->CallObjectMethod(jActivityObject, unregisterReceiverMethodID, jQtBroadcastReceiverObject);
}

void AndroidBroadcastReceiver::addAction(QString action){
    if(jActivityObject==NULL) return;

    JNIThreadHelper env;
    static jmethodID jAddActionID = env->GetMethodID(jIntentFilterClass, "addAction", "(Ljava/lang/String;)V");
    jstring filterStringUTF = env->NewStringUTF(action.toAscii());
    env->CallVoidMethod(jIntentFilterObject, jAddActionID, filterStringUTF);
    env->DeleteLocalRef(filterStringUTF);

    jclass jContextClass = env->FindClass("android/app/Application");
    jmethodID registerReceiverMethodID = env->GetMethodID(jContextClass, "registerReceiver", "(Landroid/content/BroadcastReceiver;Landroid/content/IntentFilter;)Landroid/content/Intent;");
    env->CallObjectMethod(jActivityObject, registerReceiverMethodID, jQtBroadcastReceiverObject, jIntentFilterObject);
}

#ifdef __cplusplus
extern "C" {
#endif
JNIEXPORT void JNICALL Java_eu_licentia_necessitas_industrius_QtBroadcastReceiver_jniOnReceive
  (JNIEnv *env, jobject javaObject, jint qtObject, jobject context, jobject intent){
    Q_UNUSED(javaObject);
    reinterpret_cast<AndroidBroadcastReceiver*>(qtObject)->onReceive(env, context, intent);
}
#ifdef __cplusplus
}

#endif

//#include "moc_androidbroadcastreceiver.cpp"
