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

#include "qllcpserver_maemo6_p.h"

#include "manager_interface.h"
#include "maemo6/adapter_interface_p.h"
#include "qllcpsocket_maemo6_p.h"
#include "maemo6/socketrequestor_p.h"

using namespace com::nokia::nfc;

QTM_BEGIN_NAMESPACE

static QAtomicInt requestorId = 0;
static const char * const requestorBasePath = "/com/nokia/nfc/llcpserver/";

QLlcpServerPrivate::QLlcpServerPrivate(QLlcpServer *q)
:   q_ptr(q),
    m_connection(QDBusConnection::connectToBus(QDBusConnection::SystemBus, QUuid::createUuid())),
    m_socketRequestor(0)
{
}

bool QLlcpServerPrivate::listen(const QString &serviceUri)
{
    if (m_requestorPath.isEmpty()) {
        m_requestorPath = QLatin1String(requestorBasePath) +
                          QString::number(requestorId.fetchAndAddOrdered(1));
    }

    Manager manager(QLatin1String("com.nokia.nfc"), QLatin1String("/"), m_connection);
    QDBusObjectPath defaultAdapterPath = manager.DefaultAdapter();

    if (!m_socketRequestor) {
        m_socketRequestor = new SocketRequestor(defaultAdapterPath.path(), this);

        connect(m_socketRequestor, SIGNAL(accessFailed(QDBusObjectPath,QString)),
                this, SLOT(AccessFailed(QDBusObjectPath,QString)));
        connect(m_socketRequestor, SIGNAL(accessGranted(QDBusObjectPath,QString)),
                this, SLOT(AccessGranted(QDBusObjectPath,QString)));
        connect(m_socketRequestor, SIGNAL(accept(QDBusVariant,QDBusVariant,int,QVariantMap)),
                this, SLOT(Accept(QDBusVariant,QDBusVariant,int,QVariantMap)));
        connect(m_socketRequestor, SIGNAL(connect(QDBusVariant,QDBusVariant,int,QVariantMap)),
                this, SLOT(Connect(QDBusVariant,QDBusVariant,int,QVariantMap)));
        connect(m_socketRequestor, SIGNAL(socket(QDBusVariant,QDBusVariant,int,QVariantMap)),
                this, SLOT(Socket(QDBusVariant,QDBusVariant,int,QVariantMap)));
    }

    if (m_socketRequestor) {
        QString accessKind(QLatin1String("device.llcp.co.server:") + serviceUri);
        m_socketRequestor->requestAccess(m_requestorPath, accessKind);

        m_serviceUri = serviceUri;
    } else {
        m_error = QLlcpSocket::SocketResourceError;

        m_serviceUri.clear();
    }

    return !m_serviceUri.isEmpty();
}

bool QLlcpServerPrivate::isListening() const
{
    return !m_serviceUri.isEmpty();
}

void QLlcpServerPrivate::close()
{
    QString accessKind(QLatin1String("device.llcp.co.server:") + m_serviceUri);

    m_socketRequestor->cancelAccessRequest(m_requestorPath, accessKind);

    m_serviceUri.clear();
}

QString QLlcpServerPrivate::serviceUri() const
{
    return m_serviceUri;
}

quint8 QLlcpServerPrivate::serverPort() const
{
    return 0;
}

bool QLlcpServerPrivate::hasPendingConnections() const
{
    return !m_pendingSockets.isEmpty();
}

QLlcpSocket *QLlcpServerPrivate::nextPendingConnection()
{
    if (m_pendingSockets.isEmpty())
        return 0;

    int fd = m_pendingSockets.takeFirst();

    QLlcpSocketPrivate *socketPrivate =
        new QLlcpSocketPrivate(m_connection, fd);

    QLlcpSocket *socket = new QLlcpSocket(socketPrivate, 0);

    return socket;
}

QLlcpSocket::SocketError QLlcpServerPrivate::serverError() const
{
    return QLlcpSocket::UnknownSocketError;
}

void QLlcpServerPrivate::AccessFailed(const QDBusObjectPath &targetPath, const QString &error)
{
    Q_UNUSED(targetPath);
    Q_UNUSED(error);

    m_serviceUri.clear();

    m_error = QLlcpSocket::SocketAccessError;
}

void QLlcpServerPrivate::AccessGranted(const QDBusObjectPath &targetPath,
                                       const QString &accessKind)
{
    Q_UNUSED(targetPath);
    Q_UNUSED(accessKind);
}

void QLlcpServerPrivate::Accept(const QDBusVariant &lsap, const QDBusVariant &rsap,
                                int readFd, const QVariantMap &properties)
{
    Q_UNUSED(lsap);
    Q_UNUSED(rsap);
    Q_UNUSED(properties);

    Q_Q(QLlcpServer);

    m_pendingSockets.append(readFd);

    emit q->newConnection();
}

void QLlcpServerPrivate::Connect(const QDBusVariant &lsap, const QDBusVariant &rsap,
                                 int readFd, const QVariantMap &properties)
{
    Q_UNUSED(lsap);
    Q_UNUSED(rsap);
    Q_UNUSED(readFd);
    Q_UNUSED(properties);
}

void QLlcpServerPrivate::Socket(const QDBusVariant &lsap, const QDBusVariant &rsap,
                                int readFd, const QVariantMap &properties)
{
    Q_UNUSED(lsap);
    Q_UNUSED(rsap);
    Q_UNUSED(readFd);
    Q_UNUSED(properties);
}

#include "moc_qllcpserver_maemo6_p.cpp"

QTM_END_NAMESPACE
