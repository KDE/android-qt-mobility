/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QBLUETOOTHTRANSFERMANAGER_H
#define QBLUETOOTHTRANSFERMANAGER_H

#include <qmobilityglobal.h>
#include <qbluetoothaddress.h>

#include <QtCore/QObject>

QT_FORWARD_DECLARE_CLASS(QIODevice)

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QBluetoothTransferReply;
class QBluetoothTransferRequest;
class QBluetoothTranferManagerPrivate;

class Q_CONNECTIVITY_EXPORT QBluetoothTransferManager : public QObject
{
    Q_OBJECT

public:
    enum Operation {
        GetOperation,
        PutOperation
    };

    explicit QBluetoothTransferManager(QObject *parent = 0);
    ~QBluetoothTransferManager();

    QBluetoothTransferReply *put(const QBluetoothTransferRequest &request, QIODevice *data);    

signals:
    void finished(QBluetoothTransferReply *reply);

};

QTM_END_NAMESPACE

QT_END_HEADER

#endif // QBLUETOOTHTRANSFERMANAGER_H
