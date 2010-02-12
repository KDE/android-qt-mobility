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

#ifndef DBUSCOMM_MAEMO_H
#define DBUSCOMM_MAEMO_H

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtDBus/QtDBus>

#include "qgeopositioninfo.h"
#include "qgeopositioninfosource.h"
#include "dbusserver_maemo_p.h"

QTM_BEGIN_NAMESPACE

class DBusServer;
class DBusComm: public QObject
{
    Q_OBJECT

private:
    QDBusInterface *positioningdProxy;
    DBusServer* dbusServer;
    QObject serverObj;

    QString positioningdService;
    QString positioningdPath;
    QString positioningdInterface;
    QString myService;
    QString myPath;
    QString myInterface;
    int     clientId;

    void createUniqueName();

public:
    enum cmds {CmdStart = 1, CmdStop = 2, CmdOneShot = 3, CmdSetMethods = 4, CmdSetInterval = 8};

    DBusComm();
    int  init();
    bool sendDBusRegister();
    int  sessionConfigRequest(const int command, const int method,
                              const int interval) const;
    int receiveDBusMessage(const QByteArray &message); // called by D-Bus server
    int receivePositionUpdate(const QGeoPositionInfo &update); // called by D-Bus server
    int receiveSettings(const QGeoPositionInfoSource::PositioningMethod methods,
                        const int interval);
signals:
    void receivedMessage(const QByteArray &msg);
    void receivedPositionUpdate(const QGeoPositionInfo &update);
};

QTM_END_NAMESPACE

#endif
