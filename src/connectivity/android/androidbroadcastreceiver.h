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

#ifndef JNIBROADCASTRECEIVER_H
#define JNIBROADCASTRECEIVER_H
#include <jni.h>
#include <QtCore>
#include <android/log.h>
#include "jnithreadhelper.h"

/* Header for class eu_licentia_necessitas_industrius_QtBroadcastReceiver */
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     eu_licentia_necessitas_industrius_QtBroadcastReceiver
 * Method:    jniOnReceive
 * Signature: (ILandroid/content/Context;Landroid/content/Intent;)V
 */
JNIEXPORT void JNICALL Java_eu_licentia_necessitas_industrius_QtBroadcastReceiver_jniOnReceive
  (JNIEnv *, jobject, jint, jobject, jobject);

#ifdef __cplusplus
}
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class AndroidBroadcastReceiver: public QObject
{
    Q_OBJECT
public:
    AndroidBroadcastReceiver(QObject* parent = 0);
    virtual ~AndroidBroadcastReceiver();

    void addAction(QString filter);
    static void initialize(JNIThreadHelper& environment, jclass appClass, jobject mainActivity);
    static void loadJavaClass(JNIEnv*);

private:
    friend void Java_eu_licentia_necessitas_industrius_QtBroadcastReceiver_jniOnReceive(JNIEnv *, jobject, jint, jobject, jobject);
    virtual void onReceive(JNIEnv *env, jobject context, jobject intent)=0;

    void unregisterReceiver();
    static void defineJavaClass(JNIThreadHelper&, jclass);

    jobject jQtBroadcastReceiverObject;
    jobject jIntentFilterObject;

    static jclass jIntentFilterClass;
    static jclass jQtBroadcastReceiverClass;
    static jobject jActivityObject;
};

QT_END_NAMESPACE

QT_END_HEADER


#endif // JNIBROADCASTRECEIVER_H
