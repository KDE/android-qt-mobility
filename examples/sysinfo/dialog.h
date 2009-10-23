/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
class QSystemScreenSaver;
class QSystemInfo;
#include <qsysteminfo.h>

namespace Ui {
    class Dialog;
}

class Dialog : public QDialog {
    Q_OBJECT
public:
    Dialog(QWidget *parent = 0);
    ~Dialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::Dialog *ui;
    void setupGeneral();
    void setupDevice();
    void setupDisplay();
    void setupStorage();
    void setupNetwork();
    void setupSaver();

    QSystemScreenSaver *saver;
    QSystemInfo *systemInfo;
    QSystemDeviceInfo *di;
    QSystemNetworkInfo *ni;

private slots:
    void tabChanged(int index);
    void getVersion(int index);
    void getFeature(int index);
    void setSaverEnabled(bool b);

    void netStatusComboActivated(int);
    void updateBatteryStatus(int);
    void updatePowerState(QSystemDeviceInfo::PowerState);
    void displayBatteryStatus(QSystemDeviceInfo::BatteryStatus);

     void displayNetworkStatus(QSystemNetworkInfo::NetworkStatus);
     void networkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus);
     void networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode, int);
     void networkNameChanged(QSystemNetworkInfo::NetworkMode,const QString &);

};

#endif // DIALOG_H
