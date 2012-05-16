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

#include "devicediscoverybroadcastreceiver.h"
#include "qbluetoothdeviceinfo.h"
#include "qbluetoothaddress.h"

QTM_BEGIN_NAMESPACE

DeviceDiscoveryBroadcastReceiver::DeviceDiscoveryBroadcastReceiver(QObject* parent): AndroidBroadcastReceiver(parent)
{
   addAction("android.bluetooth.device.action.FOUND");
   addAction("android.bluetooth.adapter.action.DISCOVERY_STARTED");
   addAction("android.bluetooth.adapter.action.DISCOVERY_FINISHED");
}

void DeviceDiscoveryBroadcastReceiver::onReceive(JNIEnv *env, jobject context, jobject intent){
    static jclass intentClass = env->FindClass("android/content/Intent");

    jmethodID getParcelableExtraID = env->GetMethodID(intentClass, "getParcelableExtra", "(Ljava/lang/String;)Landroid/os/Parcelable;");

    jmethodID getActionID = env->GetMethodID(intentClass, "getAction", "()Ljava/lang/String;");
    jstring actionJString = (jstring)env->CallObjectMethod(intent, getActionID);
    jboolean isCopy;
    const jchar* actionChar = env->GetStringChars(actionJString, &isCopy);
    QString actionString = QString::fromUtf16(actionChar, env->GetStringLength(actionJString));
    env->ReleaseStringChars(actionJString, actionChar);

    if(actionString == "android.bluetooth.adapter.action.DISCOVERY_FINISHED" ){
        emit finished();
    }else if(actionString == "android.bluetooth.adapter.action.DISCOVERY_STARTED" ){

    }else if(actionString == "android.bluetooth.device.action.FOUND"){
        jstring DEVICE = env->NewStringUTF("android.bluetooth.device.extra.DEVICE");
        jobject bluetoothDeviceObject = env->CallObjectMethod(intent, getParcelableExtraID, DEVICE);

        static jclass bluetoothDeviceClass = env->FindClass("android/bluetooth/BluetoothDevice");
        static jclass bluetoothClassClass = env->FindClass("android/bluetooth/BluetoothClass");

        static jmethodID getNameID = env->GetMethodID(bluetoothDeviceClass, "getName", "()Ljava/lang/String;");
        static jmethodID getAddressID = env->GetMethodID(bluetoothDeviceClass, "getAddress", "()Ljava/lang/String;");
        static jmethodID getBluetoothClassID = env->GetMethodID(bluetoothDeviceClass, "getBluetoothClass", "()Landroid/bluetooth/BluetoothClass;");
        static jmethodID getDeviceClassID = env->GetMethodID(bluetoothClassClass, "getDeviceClass", "()I");

        if(bluetoothDeviceObject != NULL){
            // Name
            jstring deviceName = (jstring) env->CallObjectMethod(bluetoothDeviceObject, getNameID);
            const jchar* name = env->GetStringChars(deviceName, &isCopy);
            QString qtName = QString::fromUtf16(name, env->GetStringLength(deviceName));
            env->ReleaseStringChars(deviceName, name);

            // Address
            jstring deviceAddress = (jstring) env->CallObjectMethod(bluetoothDeviceObject, getAddressID);
            const jchar* address = env->GetStringChars(deviceAddress, &isCopy);
            QString qtAddress = QString::fromUtf16(address, env->GetStringLength(deviceAddress));
            env->ReleaseStringChars(deviceAddress, address);

            // Device Classes
            jobject bluetoothClassObject = env->CallObjectMethod(bluetoothDeviceObject, getBluetoothClassID);
            jint classesInt = env->CallIntMethod(bluetoothClassObject, getDeviceClassID);

            QBluetoothDeviceInfo info(QBluetoothAddress(qtAddress), qtName, classesInt);
            emit deviceDiscovered(info);
        }
    }
}

#include "moc_devicediscoverybroadcastreceiver.cpp"

QTM_END_NAMESPACE
