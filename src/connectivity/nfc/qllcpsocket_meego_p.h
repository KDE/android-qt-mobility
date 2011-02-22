/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QLLCPSOCKET_MEEGO_P_H
#define QLLCPSOCKET_MEEGO_P_H

#include <qmobilityglobal.h>

#include "qllcpsocket.h"

#include <QtDBus/QDBusConnection>

QT_FORWARD_DECLARE_CLASS(QDBusObjectPath)
QT_FORWARD_DECLARE_CLASS(QDBusVariant)

class AccessRequestorAdaptor;
class LLCPRequestorAdaptor;

QTM_BEGIN_NAMESPACE

class QLlcpSocketPrivate : public QObject
{
    Q_OBJECT

    Q_DECLARE_PUBLIC(QLlcpSocket)

public:
    QLlcpSocketPrivate(QLlcpSocket *q);

    void connectToService(QNearFieldTarget *target, const QString &serviceUri);
    void disconnectFromService();

    bool bind(quint8 port);

    bool hasPendingDatagrams() const;
    qint64 pendingDatagramSize() const;

    qint64 writeDatagram(const char *data, qint64 size);
    qint64 writeDatagram(const QByteArray &datagram);

    qint64 readDatagram(char *data, qint64 maxSize,
                        QNearFieldTarget **target = 0, quint8 *port = 0);
    qint64 writeDatagram(const char *data, qint64 size,
                         QNearFieldTarget *target, quint8 port);
    qint64 writeDatagram(const QByteArray &datagram, QNearFieldTarget *target, quint8 port);

    QLlcpSocket::SocketError error() const;
    QLlcpSocket::SocketState state() const;

    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);

    bool bytesAvailable() const;

    bool waitForReadyRead(int msecs);
    bool waitForBytesWritten(int msecs);
    bool waitForConnected(int msecs);
    bool waitForDisconnected(int msecs);

private slots:
    // com.nokia.nfc.AccessRequestor
    void AccessFailed(const QDBusObjectPath &targetPath, const QString &error);
    void AccessGranted(const QDBusObjectPath &targetPath, const QStringList &accessKind);

    // com.nokia.nfc.LLCPRequestor
    void Accept(const QDBusObjectPath &targetPath, const QDBusVariant &lsap,
                const QDBusVariant &rsap, int fd);
    void Connect(const QDBusObjectPath &targetPath, const QDBusVariant &lsap,
                 const QDBusVariant &rsap, int fd);
    void Socket(const QDBusObjectPath &targetPath, const QDBusVariant &lsap,
                const QDBusVariant &rsap, int fd);

private:
    QLlcpSocket *q_ptr;

    QDBusConnection m_connection;

    AccessRequestorAdaptor *m_accessRequestor;
    LLCPRequestorAdaptor *m_llcpRequestor;
};

QTM_END_NAMESPACE

#endif // QLLCPSOCKET_MEEGO_P_H
