/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef DIALOG_H
#define DIALOG_H

#include <QWidget>
#include <qsysteminfo.h>
#include <qsystemdeviceinfo.h>

#include "ui_dialog_small_landscape.h"
QTM_USE_NAMESPACE

class Dialog : public QWidget, public Ui_Dialog
{
    Q_OBJECT
public:
    Dialog();
    ~Dialog();
    void parseArguments();

protected:
    void changeEvent(QEvent *e);

private:

    void setupGeneral();
    void setupDevice();
    void setupDisplay();
    void setupStorage();
    void setupNetwork();
    void setupSaver();
    void setupBattery();
    void setupAlignedTimer();

    QSystemScreenSaver *saver;
    QSystemInfo *systemInfo;
    QSystemDeviceInfo *di;
    QSystemNetworkInfo *ni;
    QSystemStorageInfo *sti;
    QSystemBatteryInfo *bi;
    QSystemDisplayInfo *dis;

    QSystemAlignedTimer*alt;

    void updateStorage();

    QSystemBatteryInfo::BatteryStatus currentBatStat;
    QSystemDeviceInfo::PowerState currentPowerState;

    QSystemBatteryInfo::ChargerType currentChargerType;
    QSystemBatteryInfo::ChargingState currentChargingState;

    void updateKeyboard(QSystemDeviceInfo::KeyboardTypeFlags type);

    QString storageStateToString(QSystemStorageInfo::StorageState state);

    QString sizeToString(qlonglong size);
    QBrush brushForStorageState(QSystemStorageInfo::StorageState state);

    QString lockStateToString(QSystemDeviceInfo::LockTypeFlags);
    QSystemDeviceInfo::LockTypeFlags oldLockStatus;

    QSystemAlignedTimer*alt1;
    QSystemAlignedTimer*alt2;
    QSystemAlignedTimer*alt3;

private slots:
    void tabChanged(int index);
    void getVersion(int index);
    void getFeature(int index);
    void setSaverEnabled(bool b);
    void updateDeviceLockedState();

    void netStatusComboActivated(int);
    void updateBatteryStatus(int);
    void updatePowerState(QSystemDeviceInfo::PowerState);
    void displayBatteryStatus(QSystemBatteryInfo::BatteryStatus);
    void updateProfile(QSystemDeviceInfo::Profile profile);
    void updateSimStatus();
    void updateThermalState();
    void updateProfile();

     void displayNetworkStatus(QSystemNetworkInfo::NetworkStatus);
     void networkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus);
     void networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode, int);
     void networkNameChanged(QSystemNetworkInfo::NetworkMode,const QString &);
     void networkModeChanged(QSystemNetworkInfo::NetworkMode);

    void storageChanged(bool added,const QString &vol);
    void bluetoothChanged(bool);

    void chargingStateChanged(QSystemBatteryInfo::ChargingState chargingState);
    void chargerTypeChanged(QSystemBatteryInfo::ChargerType chargerType);

    void orientationChanged(QSystemDisplayInfo::DisplayOrientation);
    void keyboardFlipped(bool);

    void storageStateChanged(const QString &vol, QSystemStorageInfo::StorageState state);

    void backlightTotext(QSystemDisplayInfo::BacklightState);
    void dataTechnologyChanged(QSystemNetworkInfo::CellDataTechnology tech);

    void lockStatusChanged(QSystemDeviceInfo::LockTypeFlags);

    void cellIdChanged(int);

    void startAlignedTimers();
    void stopAlignedTimers();

    void setupAlignedTimers();

    void timeout1();
    void timeout2();
    void timeout3();

    void timerError(QSystemAlignedTimer::AlignedTimerError error);
};

#endif // DIALOG_H
