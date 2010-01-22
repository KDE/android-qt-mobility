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

#include "qservicecontrol_p.h"
#include "ipcendpoint_p.h"
#include "objectendpoint_p.h"

#ifdef Q_OS_SYMBIAN
#else
#include <QLocalServer>
#include <QLocalSocket>
#include <QDataStream>
#include <QTimer>
#endif

QTM_BEGIN_NAMESPACE

//IPC is based on QLocalSocket

#ifdef Q_OS_SYMBIAN
#else

class LocalSocketEndPoint : public QServiceIpcEndPoint
{
    Q_OBJECT
public:
    LocalSocketEndPoint(QLocalSocket* s, QObject* parent = 0)
        : QServiceIpcEndPoint(parent), socket(s)
    {
        Q_ASSERT(socket);
        socket->setParent(this);
        connect(s, SIGNAL(readyRead()), this, SLOT(readIncoming()));
        connect(s, SIGNAL(disconnected()), this, SIGNAL(disconnected()));

        if (socket->bytesAvailable())
            QTimer::singleShot(0, this, SLOT(readIncoming()));
    }

    ~LocalSocketEndPoint() 
    {
    }


protected:
    void flushPackage(const QServicePackage& package)
    {
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << package;
        socket->write(block);
    }

protected slots:
    void readIncoming()
    {
        QDataStream in(socket);
        in.setVersion(QDataStream::Qt_4_6);

        while(socket->bytesAvailable()) {
            QServicePackage package;
            in >> package;
            incoming.enqueue(package);
        }

        emit readyRead();
    }

private:
    QLocalSocket* socket;
};

#endif

QServiceControlPrivate::QServiceControlPrivate(QObject* parent)
    : QObject(parent)
{
}

void QServiceControlPrivate::publishServices( const QString& ident)
{
    //for now we just have LocalSocket
    createServiceEndPoint(ident) ;
}

void QServiceControlPrivate::processIncoming()
{
#ifdef Q_OS_SYMBIAN
#else
    //for now we have QLocalSocket only
    qDebug() << "Processing incoming connect";
    if (localServer->hasPendingConnections()) {
        QLocalSocket* s = localServer->nextPendingConnection();
        //LocalSocketEndPoint owns socket 
        LocalSocketEndPoint* ipcEndPoint = new LocalSocketEndPoint(s);
        ObjectEndPoint* endpoint = new ObjectEndPoint(ObjectEndPoint::Service, ipcEndPoint, this);
    }
#endif
}

/*!
    Creates endpoint on service side.
*/
bool QServiceControlPrivate::createServiceEndPoint(const QString& ident)
{
    //for now we have QLocalSocket only
    //other IPC mechanisms such as dbus may have to publish the
    //meta object definition for all registered service types
#ifdef Q_OS_SYMBIAN
#else
    QLocalServer::removeServer(ident);
    qDebug() << "Start listening for incoming connections";
    localServer = new QLocalServer(this);
    if ( !localServer->listen(ident) ) {
        qWarning() << "Cannot create local socket endpoint";
        return false;
    }
    connect(localServer, SIGNAL(newConnection()), this, SLOT(processIncoming()));
    if (localServer->hasPendingConnections())
        QTimer::singleShot(0, this, SLOT(processIncoming()));

    return true;
#endif
}

/*!
    Creates endpoint on client side.
*/
QObject* QServiceControlPrivate::proxyForService(const QServiceTypeIdent& typeIdent, const QString& location)
{
#ifdef Q_OS_SYMBIAN
#else
    QLocalSocket* socket = new QLocalSocket();
    //location format:  protocol:address
    socket->connectToServer(location.section(':', 1, 1));
    if (!socket->isValid()) {
        qWarning() << "Cannot connect to remote service" << location;
        return 0;
    }
    LocalSocketEndPoint* ipcEndPoint = new LocalSocketEndPoint(socket);
    ObjectEndPoint* endPoint = new ObjectEndPoint(ObjectEndPoint::Client, ipcEndPoint);

    QObject *proxy = endPoint->constructProxy(typeIdent);
    QObject::connect(proxy, SIGNAL(destroyed()), endPoint, SLOT(deleteLater()));
    return proxy;
#endif
    return 0;
}

#include "moc_qservicecontrol_p.cpp"
#include "qservicecontrol_p.moc"
QTM_END_NAMESPACE
