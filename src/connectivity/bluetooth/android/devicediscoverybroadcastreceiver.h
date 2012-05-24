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

#ifndef DEVICEDISCOVERYBROADCASTRECEIVER_H
#define DEVICEDISCOVERYBROADCASTRECEIVER_H

#include "android/androidbroadcastreceiver.h"
#include "qmobilityglobal.h"
#include "qbluetoothdevicediscoveryagent.h"

QT_BEGIN_HEADER
QTM_BEGIN_NAMESPACE

class QBluetoothDeviceInfo;

class DeviceDiscoveryBroadcastReceiver : public AndroidBroadcastReceiver
{
    Q_OBJECT
public:
    DeviceDiscoveryBroadcastReceiver(QObject* parent = 0);
    virtual void onReceive(JNIEnv *env, jobject context, jobject intent);

signals:
    void deviceDiscovered(const QBluetoothDeviceInfo &info);
    void finished();
    void error(QBluetoothDeviceDiscoveryAgent::Error error);
    void canceled();
};

QTM_END_NAMESPACE
QT_END_HEADER
#endif // DEVICEDISCOVERYBROADCASTRECEIVER_H
