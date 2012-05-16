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

#include "qbluetoothlocaldevice.h"
#include "qbluetoothaddress.h"
#include "qbluetoothlocaldevice_p.h"
#include "android/jnithreadhelper.h"
#include <android/log.h>

QTM_BEGIN_NAMESPACE


jclass QBluetoothLocalDevicePrivate::btAdapterClass=NULL;
jobject QBluetoothLocalDevicePrivate::btAdapterObject=NULL;

void QBluetoothLocalDevicePrivate::initialize(JNIEnv *env){
    btAdapterClass = env->FindClass("android/bluetooth/BluetoothAdapter");
    if (btAdapterClass == NULL)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt", "Native registration unable to find class android/bluetooth/BluetoothAdapter");
    }

    jmethodID getDefaultAdapterID = env->GetStaticMethodID(btAdapterClass, "getDefaultAdapter", "()Landroid/bluetooth/BluetoothAdapter;");
    if(getDefaultAdapterID == NULL){
        __android_log_print(ANDROID_LOG_FATAL,"Qt", "Native registration unable to get method ID: getDefaultAdapter of android/bluetooth/BluetoothAdapter");
    }

    btAdapterObject = env->CallStaticObjectMethod(btAdapterClass, getDefaultAdapterID);
    if(btAdapterObject == NULL){
        __android_log_print(ANDROID_LOG_FATAL,"Qt", "Device does not support Bluetooth");
    }
    btAdapterObject = env->NewGlobalRef(btAdapterObject);
}

bool QBluetoothLocalDevicePrivate::startDiscovery(){
    JNIThreadHelper env;

    jmethodID startDiscoveryID = env->GetMethodID(btAdapterClass, "startDiscovery", "()Z");
    jboolean ret = env->CallBooleanMethod(btAdapterObject, startDiscoveryID);

    return ret;
}
bool QBluetoothLocalDevicePrivate::cancelDiscovery(){
    JNIThreadHelper env;

    jmethodID cancelDiscoveryID = env->GetMethodID(btAdapterClass, "cancelDiscovery", "()Z");
    jboolean ret = env->CallBooleanMethod(btAdapterObject, cancelDiscoveryID);

    return ret;
}
bool QBluetoothLocalDevicePrivate::isDiscovering(){
    JNIThreadHelper env;

    jmethodID isDiscoveringID = env->GetMethodID(btAdapterClass, "isDiscovering", "()Z");
    jboolean ret = env->CallBooleanMethod(btAdapterObject, isDiscoveringID);

    return ret;
}


QBluetoothLocalDevice::QBluetoothLocalDevice(QObject *parent)
:   QObject(parent)
{
}

QBluetoothLocalDevice::QBluetoothLocalDevice(const QBluetoothAddress &address, QObject *parent)
: QObject(parent)
{
}

QString QBluetoothLocalDevice::name() const
{
    if(d_ptr->btAdapterObject != 0){
        JNIThreadHelper env;

        static jmethodID getNameID = env->GetMethodID(d_ptr->btAdapterClass, "getName", "()Ljava/lang/String;");
        jstring peerName = (jstring) env->CallObjectMethod(d_ptr->btAdapterObject, getNameID);

        jboolean isCopy;
        const jchar* name = env->GetStringChars(peerName, &isCopy);
        QString qtName = QString::fromUtf16(name, env->GetStringLength(peerName));
        env->ReleaseStringChars(peerName, name);
        return qtName;
    }else{
        return QString();
    }
}

QBluetoothAddress QBluetoothLocalDevice::address() const
{
    if(d_ptr->btAdapterObject != 0){
        JNIThreadHelper env;

        static jmethodID getAddressID = env->GetMethodID(d_ptr->btAdapterClass, "getAddress", "()Ljava/lang/String;");
        jstring peerName = (jstring) env->CallObjectMethod(d_ptr->btAdapterObject, getAddressID);

        jboolean isCopy;
        const jchar* addressStr = env->GetStringChars(peerName, &isCopy);
        QString strAddress = QString::fromUtf16(addressStr, env->GetStringLength(peerName));
        QBluetoothAddress address(strAddress);
        env->ReleaseStringChars(peerName, addressStr);
        return address;
    }else{
        return QBluetoothAddress();
    }
}

void QBluetoothLocalDevice::powerOn()
{
    JNIThreadHelper env;

    jmethodID enableID = env->GetMethodID(d_ptr->btAdapterClass, "enable", "()Z");
    jboolean ret = env->CallBooleanMethod(d_ptr->btAdapterObject, enableID);
    if(!ret){
        emit error(QBluetoothLocalDevice::UnknownError);
    }
}

void QBluetoothLocalDevice::setHostMode(QBluetoothLocalDevice::HostMode mode)
{
    Q_UNUSED(mode);
    // TODO: That must be implemented over Actions framework
}

QBluetoothLocalDevice::HostMode QBluetoothLocalDevice::hostMode() const
{
    JNIThreadHelper env;

    jmethodID getScanModeID = env->GetMethodID(d_ptr->btAdapterClass, "getScanMode", "()I");
    jint scanMode = env->CallBooleanMethod(d_ptr->btAdapterObject, getScanModeID);
    switch(scanMode){
        case 20:
            return HostPoweredOff;
        case 21:
            return HostConnectable;
        case 23:
            return HostDiscoverable;
        default: // HostDiscoverableLimitedInquiry is not supported
            return HostPoweredOff;
    }
}

QList<QBluetoothHostInfo> QBluetoothLocalDevice::allDevices()
{
    QList<QBluetoothHostInfo> localDevices;
    return localDevices;
}

void QBluetoothLocalDevice::requestPairing(const QBluetoothAddress &address, Pairing pairing)
{
    // Not supported on Android. Android does it automatically while connecting.
    emit error(PairingError);

    Q_UNUSED(address);
    Q_UNUSED(pairing);
}

QBluetoothLocalDevice::Pairing QBluetoothLocalDevice::pairingStatus(const QBluetoothAddress &address) const
{

    Q_UNUSED(address);
    return Unpaired;
}

void QBluetoothLocalDevice::pairingConfirmation(bool confirmation)
{
    Q_UNUSED(confirmation);
}

QTM_END_NAMESPACE
