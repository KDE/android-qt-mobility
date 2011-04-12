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

#include "qbluetoothsocket_p.h"
#include "qbluetoothsocket.h"
#include "qbluetoothlocaldevice_p.h"
#include "symbian/utils_symbian_p.h"

#include <QCoreApplication>

#include <QDebug>

#include <limits.h>
#include <bt_sock.h>
#include <es_sock.h>

QTM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QSocketServerPrivate, getSocketServer)

QBluetoothSocketPrivate::QBluetoothSocketPrivate()
    : socket(0)
    , socketType(QBluetoothSocket::UnknownSocketType)
    , state(QBluetoothSocket::UnconnectedState)
    , readNotifier(0)
    , connectWriteNotifier(0)
    , discoveryAgent(0)
    , iSocket(0)
    , iBlankSocket(0)
    , rxDescriptor(0, 0)
    , txDescriptor(0, 0)
    , receiving(false)

{
}

QBluetoothSocketPrivate::~QBluetoothSocketPrivate()
{
    delete iBlankSocket;
    delete iSocket;
}

void QBluetoothSocketPrivate::connectToService(const QBluetoothAddress &address, quint16 port, QIODevice::OpenMode openMode)
{
    Q_Q(QBluetoothSocket);
    TBTSockAddr a;

    a.SetBTAddr(TBTDevAddr(address.toUInt64()));
    a.SetPort(port);

    if (iSocket->Connect(a) == KErrNone) {
        q->setSocketState(QBluetoothSocket::ConnectingState);
    } else {
        socketError = QBluetoothSocket::UnknownSocketError;
        emit q->error(socketError);
    }
}

bool QBluetoothSocketPrivate::ensureNativeSocket(QBluetoothSocket::SocketType type)
{
    if (iSocket) {
        if (socketType == type)
            return true;
        else
            delete iSocket;
    }

    socketType = type;

    switch (type) {
    case QBluetoothSocket::L2capSocket: {
        TRAPD(err, iSocket = CBluetoothSocket::NewL(*this, getSocketServer()->socketServer, _L("L2CAP")));
        Q_UNUSED(err);
        break;
    }
    case QBluetoothSocket::RfcommSocket: {
        TRAPD(err, iSocket = CBluetoothSocket::NewL(*this, getSocketServer()->socketServer, _L("RFCOMM")));
        Q_UNUSED(err);
        break;
    }
    default:
        iSocket = 0;
        return false;
    }

    if (iSocket)
        return true;

    return false;
}

void QBluetoothSocketPrivate::ensureBlankNativeSocket()
{
    if (iBlankSocket) {
        delete iBlankSocket;
        iBlankSocket = NULL;
    }

    TRAPD(err, iBlankSocket = CBluetoothSocket::NewL(*this, getSocketServer()->socketServer));
    Q_UNUSED(err);
}

void QBluetoothSocketPrivate::startReceive()
{
    if (receiving)
        return;

    Q_Q(QBluetoothSocket);
    if (!iSocket) {
        emit q->error(QBluetoothSocket::UnknownSocketError);
        return;
    }
    receiving = true;
    TInt err;
    recvMTU = 0;
    err = iSocket->GetOpt(KL2CAPInboundMTU, KSolBtL2CAP, recvMTU);
    if(err != KErrNone)
        return;
    receive();
}
void QBluetoothSocketPrivate::startServerSideReceive()
{
    Q_Q(QBluetoothSocket);
    if (!iBlankSocket) {
        emit q->error(QBluetoothSocket::UnknownSocketError);
        return;
    }
    TInt err;
    recvMTU = 0;
    err = iBlankSocket->GetOpt(KL2CAPInboundMTU, KSolBtL2CAP, recvMTU);
    if(err != KErrNone)
        return;   
    receive();
}

void QBluetoothSocketPrivate::receive()
{
    Q_Q(QBluetoothSocket);
    TInt err = KErrNone;
    if (!iSocket && !iBlankSocket)
    {
        emit q->error(QBluetoothSocket::UnknownSocketError);
        return;
    }
    char *bufPtr = buffer.reserve(recvMTU);
    rxDescriptor.Set(reinterpret_cast<unsigned char *>(bufPtr), 0,recvMTU);
    if (socketType == QBluetoothSocket::RfcommSocket) {
        if(iBlankSocket)
            err = iBlankSocket->RecvOneOrMore(rxDescriptor, 0, rxLength);
        else
            err = iSocket->RecvOneOrMore(rxDescriptor, 0, rxLength);
        if (err != KErrNone) {
            socketError = QBluetoothSocket::NetworkError;
            emit q->error(socketError);
        }
    } else if (socketType == QBluetoothSocket::L2capSocket) {
        if(iBlankSocket)
            err = iBlankSocket->Recv(rxDescriptor, 0);
        else
            err = iSocket->Recv(rxDescriptor, 0);
        if (err != KErrNone) {
            socketError = QBluetoothSocket::NetworkError;
            emit q->error(socketError);
        }
    }
}

void QBluetoothSocketPrivate::HandleAcceptCompleteL(TInt aErr)
{
    qDebug() << __PRETTY_FUNCTION__ << ">> aErr=" << aErr;
    Q_Q(QBluetoothSocket);
    emit q->connected();
}

void QBluetoothSocketPrivate::HandleActivateBasebandEventNotifierCompleteL(TInt aErr, TBTBasebandEventNotification &aEventNotification)
{
    qDebug() << __PRETTY_FUNCTION__;
}

void QBluetoothSocketPrivate::HandleConnectCompleteL(TInt aErr)
{
    qDebug() << __PRETTY_FUNCTION__ << ">> aErr=" << aErr;
    Q_Q(QBluetoothSocket);
    if (aErr == KErrNone) {
        q->setSocketState(QBluetoothSocket::ConnectedState);

        emit q->connected();

        startReceive();
    } else {
        q->setSocketState(QBluetoothSocket::UnconnectedState);

        switch (aErr) {
        case KErrCouldNotConnect:
            socketError = QBluetoothSocket::ConnectionRefusedError;
            break;
        default:
            qDebug() << __PRETTY_FUNCTION__ << aErr;
            socketError = QBluetoothSocket::UnknownSocketError;
        }
        emit q->error(socketError);
    }
}

void QBluetoothSocketPrivate::HandleIoctlCompleteL(TInt aErr)
{
    qDebug() << __PRETTY_FUNCTION__;
}

void QBluetoothSocketPrivate::HandleReceiveCompleteL(TInt aErr)
{
    receiving = false;
    Q_Q(QBluetoothSocket);
    if (aErr == KErrNone) {
        buffer.chop(recvMTU-rxDescriptor.Length());
        emit q->readyRead();
    } else {
        socketError = QBluetoothSocket::UnknownSocketError;
        emit q->error(socketError);
    }
}

void QBluetoothSocketPrivate::HandleSendCompleteL(TInt aErr)
{
    Q_Q(QBluetoothSocket);
    if (aErr == KErrNone) {
//        qint64 writeSize = txBuffer.size();
        qint64 writeSize = txDescriptor.Length();        
        txBuffer.skip(writeSize);
        txTempBuffer.clear();
        emit q->bytesWritten(writeSize);
              
        if (state == QBluetoothSocket::ClosingState && txBuffer.size() == 0) {
            q->close();        
        }
        else if (txBuffer.size()){
            txTempBuffer = txBuffer.readAll();

            txDescriptor.Set(reinterpret_cast<const unsigned char *>(txTempBuffer.constData()), txTempBuffer.length());
            TInt err = KErrNone;
            if(iBlankSocket)
              err = iBlankSocket->Send(txDescriptor, 0);
            else
              err = iSocket->Send(txDescriptor, 0);
        
            if (err != KErrNone) {
                socketError = QBluetoothSocket::UnknownSocketError;
                Q_Q(QBluetoothSocket);
                emit q->error(socketError);
                txTempBuffer.clear();
                return;
            }
        }
    } else {
        socketError = QBluetoothSocket::UnknownSocketError;
        emit q->error(socketError);
    }
}

void QBluetoothSocketPrivate::HandleShutdownCompleteL(TInt aErr)
{
    qDebug() << __PRETTY_FUNCTION__ << ">> aErr=" << aErr;
    Q_Q(QBluetoothSocket);
    if (aErr == KErrNone) {
        q->setSocketState(QBluetoothSocket::UnconnectedState);
        emit q->disconnected();
    } else {
        socketError = QBluetoothSocket::UnknownSocketError;
        emit q->error(socketError);
    }
}

QSocketServerPrivate::QSocketServerPrivate()
{
    /* connect to socket server */
    TInt result = socketServer.Connect();
    if (result != KErrNone) {
        qWarning("%s: RSocketServ.Connect() failed with error %d", __PRETTY_FUNCTION__, result);
        return;
    }
}

QSocketServerPrivate::~QSocketServerPrivate()
{
    if (socketServer.Handle() != 0)
        socketServer.Close();
}

QString QBluetoothSocketPrivate::localName() const
{
    return QBluetoothLocalDevicePrivate::name();
}

QBluetoothAddress QBluetoothSocketPrivate::localAddress() const
{
    TBTSockAddr address;

    iSocket->LocalName(address);

    return qTBTDevAddrToQBluetoothAddress(address.BTAddr());
}

quint16 QBluetoothSocketPrivate::localPort() const
{
    return iSocket->LocalPort();
}

QString QBluetoothSocketPrivate::peerName() const
{
    RHostResolver resolver;

    TInt err = resolver.Open(getSocketServer()->socketServer, KBTAddrFamily, KBTLinkManager);
    if (err==KErrNone) {
        TNameEntry nameEntry;
        TBTSockAddr sockAddr;
        if(iBlankSocket)
            iBlankSocket->RemoteName(sockAddr);
        else
            iSocket->RemoteName(sockAddr);
        
        TInquirySockAddr address(sockAddr);
        address.SetBTAddr(sockAddr.BTAddr());
        address.SetAction(KHostResName|KHostResIgnoreCache); // ignore name stored in cache
        err = resolver.GetByAddress(address, nameEntry);
        if(err == KErrNone) {
            TNameRecord name = nameEntry();            
            m_peerName = QString((QChar*)name.iName.Ptr(),name.iName.Length());
      	}
    }
    resolver.Close();

    if (err != KErrNone) {
        return QString();
    }
    return m_peerName;
}

QBluetoothAddress QBluetoothSocketPrivate::peerAddress() const
{
    TBTSockAddr address;

    iSocket->RemoteName(address);

    return qTBTDevAddrToQBluetoothAddress(address.BTAddr());
}

quint16 QBluetoothSocketPrivate::peerPort() const
{
    TBTSockAddr address;

    iSocket->RemoteName(address);

    return address.Port();
}

void QBluetoothSocketPrivate::close()
{
    if (state != QBluetoothSocket::ConnectedState && state != QBluetoothSocket::ListeningState)
        return;
    Q_Q(QBluetoothSocket);
    q->setSocketState(QBluetoothSocket::ClosingState);

    iSocket->Shutdown(RSocket::ENormal);
}

void QBluetoothSocketPrivate::abort()
{
    iSocket->CancelWrite();

    iSocket->Shutdown(RSocket::EImmediate);

    // force active object to run and shutdown socket.
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
}

qint64 QBluetoothSocketPrivate::readData(char *data, qint64 maxSize)
{
    qint64 size = buffer.read(data, maxSize);

    Q_Q(QBluetoothSocket);
    QMetaObject::invokeMethod(q, "_q_startReceive", Qt::QueuedConnection);

    return size;
}

qint64 QBluetoothSocketPrivate::writeData(const char *data, qint64 maxSize)
{    
    if(txBuffer.size() > 32768)
      return 0;
       
    if(txBuffer.size()){
        char *buf = txBuffer.reserve(maxSize);
        memcpy(buf, data, maxSize);
        return maxSize;
    }

    char *buf = txBuffer.reserve(maxSize);
    memcpy(buf, data, maxSize);

    txDescriptor.Set(reinterpret_cast<const unsigned char *>(buf), maxSize);
    TInt err = KErrNone;
    if(iBlankSocket)
      err = iBlankSocket->Send(txDescriptor, 0);
    else
      err = iSocket->Send(txDescriptor, 0);
    
    if (err != KErrNone) {
        socketError = QBluetoothSocket::NetworkError;
        Q_Q(QBluetoothSocket);
        emit q->error(socketError);
        return 0;
    }
    
    return maxSize;
}

void QBluetoothSocketPrivate::_q_readNotify()
{
}

void QBluetoothSocketPrivate::_q_writeNotify()
{

}

bool QBluetoothSocketPrivate::setSocketDescriptor(int socketDescriptor, QBluetoothSocket::SocketType socketType,
    QBluetoothSocket::SocketState socketState, QBluetoothSocket::OpenMode openMode)
{
    return false;
}

void QBluetoothSocketPrivate::_q_startReceive()
{
    receive();
}

qint64 QBluetoothSocketPrivate::bytesAvailable() const
{
    return buffer.size();
}


QTM_END_NAMESPACE

