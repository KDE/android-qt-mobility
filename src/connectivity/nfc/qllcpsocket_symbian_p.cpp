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
#include "qllcpsocket_symbian_p.h"
#include "qllcpstate_symbian_p.h"
#include "symbian/llcpsockettype1_symbian.h"
#include "symbian/llcpsockettype2_symbian.h"
#include "symbian/nearfieldutility_symbian.h"
#include "symbian/debug.h"
QTM_BEGIN_NAMESPACE

#define Q_CHECK_LLCPTYPE(function, type)  do { \
if ((type) != m_socketType) \
    { \
    qWarning(#function" was called by a" \
             " socket other than "#type""); \
    return; \
    } } while (0)

#define Q_CHECK_LLCPTYPE_PARA_3(function, type, returnValue)  do { \
if ((type) != m_socketType) \
    { \
    qWarning(#function" was called by a" \
             " socket other than "#type""); \
    return returnValue; \
    } } while (0)

#define Q_CHECK_SOCKET_HANDLER1(function)  do { \
if (m_symbianSocketType1 == NULL) \
    { \
    qWarning("Socket Type1 Handler is NULL in "#function); \
    return; \
    } } while (0)

#define Q_CHECK_SOCKET_HANDLER2(function)  do { \
if (m_symbianSocketType2 == NULL) \
    { \
    qWarning("Socket Type2 Handler is NULL in "#function); \
    return; \
    } } while (0)



QLlcpSocketPrivate::QLlcpSocketPrivate(QLlcpSocket *q)
       : m_error(QLlcpSocket::UnknownSocketError),
         m_symbianSocketType1(NULL),
         m_symbianSocketType2(NULL),
         m_socketType(connectionUnknown),
         q_ptr(q),
         m_emittedReadyRead(false),
         m_emittedBytesWritten(false)
{
    BEGIN
    m_unconnectedState = new QLLCPUnconnected(this);
    m_connectingState = new QLLCPConnecting(this);
    m_connectedState = new QLLCPConnected(this);
    m_closingState = new QLLCPClosing(this);
    m_bindState = new QLLCPBind(this);
    m_state = m_unconnectedState;
    END
}


QLlcpSocketPrivate::~QLlcpSocketPrivate()
{
    BEGIN
    Q_Q(QLlcpSocket);
    if (q->isOpen()) {
        q->close();
    }

    delete m_symbianSocketType1;
    delete m_symbianSocketType2;
    delete m_state;
    delete m_unconnectedState;
    delete m_connectingState;
    delete m_connectedState;
    delete m_closingState;
    delete m_bindState;
    END
}

/*!
    Construct the socket and set it as connected state from llcp server side
*/
QLlcpSocketPrivate::QLlcpSocketPrivate(CLlcpSocketType2* socketType2_symbian)
    : m_error(QLlcpSocket::UnknownSocketError),
      m_symbianSocketType1(NULL),
      m_symbianSocketType2(socketType2_symbian),
      m_socketType(connectionType2),
      m_emittedReadyRead(false),
      m_emittedBytesWritten(false)
{
    BEGIN
    Q_CHECK_PTR(m_symbianSocketType2);
    m_state = new QLLCPConnected(this);
    END
}

void QLlcpSocketPrivate::connectToService(QNearFieldTarget *target, const QString &serviceUri)
{
    BEGIN
    if( connectionType1 == m_socketType){
       invokeError();
       END
       return;
    }
    else if (connectionUnknown == m_socketType){
       m_socketType = connectionType2;
    }
    Q_Q(QLlcpSocket);
    if (!q->isOpen())
        q->open(QIODevice::ReadWrite | QIODevice::Unbuffered);
    m_state->ConnectToService(target,serviceUri);
    END
}

void QLlcpSocketPrivate::disconnectFromService()
{
    BEGIN
    if( connectionType1 == m_socketType){
       invokeError();
       END
       return;
    }

    Q_CHECK_SOCKET_HANDLER2(QLlcpSocketPrivate::disconnectFromService());

    m_state->DisconnectFromService();

    Q_Q(QLlcpSocket);
    if (q->isOpen()) {
        q->close();
    }
    END
}

void QLlcpSocketPrivate::invokeConnected()
{
    BEGIN
    Q_Q(QLlcpSocket);
    m_state->ConnectToServiceComplete();
    emit q->connected();
    END
}

void QLlcpSocketPrivate::invokeReadyRead()
{
    BEGIN
    Q_Q(QLlcpSocket);
    //If called from within a slot connected to the readyRead() signal, readyRead() will not be reemitted.
    if (!m_emittedReadyRead){
        m_emittedReadyRead = true;
        emit q->readyRead();
        m_emittedReadyRead = false;
    }
    END
}
void QLlcpSocketPrivate::attachCallbackHandler(QLlcpSocket *q)
{
    BEGIN
    q_ptr = q;
    if (!q->isOpen())
        q->open(QIODevice::ReadWrite | QIODevice::Unbuffered);
    END
}
qint64 QLlcpSocketPrivate::bytesAvailable() const
{
    BEGIN
    if( connectionType1 == m_socketType){
        return 0;
        }

    if (m_symbianSocketType2 == NULL)
        {
        return 0;
    }
    END
    return m_symbianSocketType2->BytesAvailable();
}
void QLlcpSocketPrivate::invokeBytesWritten(qint64 bytes)
{
    BEGIN
    Q_Q(QLlcpSocket);
    if (!m_emittedBytesWritten){
        m_emittedBytesWritten = true;
        emit q->bytesWritten(bytes);
        m_emittedBytesWritten = false;
    }
    END
}

void QLlcpSocketPrivate::invokeStateChanged(QLlcpSocket::State socketState)
{
    BEGIN
    Q_Q(QLlcpSocket);
    emit q->stateChanged(socketState);
    END
}

void QLlcpSocketPrivate::invokeError()
{
    BEGIN
    Q_Q(QLlcpSocket);
    emit q->error(QLlcpSocket::UnknownSocketError);
    END
}

void QLlcpSocketPrivate::invokeDisconnected()
{
    BEGIN
    Q_Q(QLlcpSocket);
    emit q->disconnected();
    END
}

/*!
    Only used at connectless mode, create type1 socket if necessary.
*/
bool QLlcpSocketPrivate::bind(quint8 port)
{
    BEGIN
    if( connectionType2 == m_socketType) {
        END
        return false;
    }
    else if (connectionUnknown == m_socketType) {
        m_socketType = connectionType1;
    }
    END
    return m_state->Bind(port);
}

/*!
    Returns true if at least one datagram is waiting to be read;
    otherwise returns false.
*/
bool QLlcpSocketPrivate::hasPendingDatagrams() const
{
    BEGIN
    bool val = false;
    Q_CHECK_LLCPTYPE_PARA_3(QLlcpSocketPrivate::hasPendingDatagrams(),connectionType1,val);
    if (m_symbianSocketType1) {
        val = m_symbianSocketType1->HasPendingDatagrams();
    }

    END
    return val;
}

/*!
    Returns the size of the first pending connectionless datagram. If there is
    no datagram available, this function returns -1.
*/
qint64 QLlcpSocketPrivate::pendingDatagramSize() const
{
    BEGIN
    int val = -1;
    Q_CHECK_LLCPTYPE_PARA_3(QLlcpSocketPrivate::pendingDatagramSize(), connectionType1, val);
    if (m_symbianSocketType1)
    {
        val = m_symbianSocketType1->PendingDatagramSize();
    }

    END
    return val;
}

CLlcpSocketType1* QLlcpSocketPrivate::socketType1Instance()
{
    if (NULL == m_symbianSocketType1)
    {
       QT_TRAP_THROWING(m_symbianSocketType1 = CLlcpSocketType1::NewL(*this));
    }
    return m_symbianSocketType1;
}

CLlcpSocketType2* QLlcpSocketPrivate::socketType2Instance()
{
    if (NULL == m_symbianSocketType2)
    {
       QT_TRAP_THROWING(m_symbianSocketType2 = CLlcpSocketType2::NewL(this));
    }
    return m_symbianSocketType2;
}

/*!
    Connection-Oriented Style
*/
qint64 QLlcpSocketPrivate::writeDatagram(const char *data, qint64 size)
{
    BEGIN
    qint64 val = -1;
    Q_CHECK_LLCPTYPE_PARA_3(QLlcpSocketPrivate::waitForDisconnected(),connectionType2, val);
    val = m_state->WriteDatagram(data,size);
    END
    return val;
}

/*!
    Connection-Oriented Style
*/
qint64 QLlcpSocketPrivate::writeDatagram(const QByteArray &datagram)
{
    BEGIN
    qint64 val = -1;
    Q_CHECK_LLCPTYPE_PARA_3(QLlcpSocketPrivate::waitForDisconnected(),connectionType2, val);
    END
    return writeDatagram(datagram.constData(),datagram.size());
}

/*!
    Used for Both Connection-Less & Connection-Oriented Style
    As for Connection-Oriented mode, can not pass in the port & target parameters.
*/
qint64 QLlcpSocketPrivate::readDatagram(char *data, qint64 maxSize,
                                        QNearFieldTarget **target, quint8 *port)
{
    BEGIN
    qint64 val = -1;
    if ( (target != NULL && *target != NULL) || port != NULL){
       Q_CHECK_LLCPTYPE_PARA_3(QLlcpSocketPrivate::readDatagram(),connectionType1, val);
    }

    val = m_state->ReadDatagram(data,maxSize,target,port);
    END
    return val;
}

qint64 QLlcpSocketPrivate::writeDatagram(const char *data, qint64 size,
                                         QNearFieldTarget *target, quint8 port)
{
    BEGIN
    if( connectionType2 == m_socketType){
        END
        return -1;
    }
    else if (connectionUnknown == m_socketType){
        m_socketType = connectionType1;
    }

    qint64 val = m_state->WriteDatagram(data,size,target,port);
    END

    return val;
}

qint64 QLlcpSocketPrivate::writeDatagram(const QByteArray &datagram,
                                         QNearFieldTarget *target, quint8 port)
{
    BEGIN
    if( connectionType2 == m_socketType){
        END
        return -1;
    }
    else if (connectionUnknown == m_socketType){
        m_socketType = connectionType1;
    }
    END
    return writeDatagram(datagram.constData(),datagram.size(),target,port);
}

QLlcpSocket::Error QLlcpSocketPrivate::error() const
{
    BEGIN
    END
    return m_error;
}

QLlcpSocket::State QLlcpSocketPrivate::state() const
{
    BEGIN
    END
    return m_state->state();
}

qint64 QLlcpSocketPrivate::readData(char *data, qint64 maxlen)
{
    BEGIN
    END
    return readDatagram(data,maxlen);
}

qint64 QLlcpSocketPrivate::writeData(const char *data, qint64 len)
{
    BEGIN
    END
    return writeDatagram(data,len);
}

bool QLlcpSocketPrivate::waitForReadyRead(int msecs)
{
    BEGIN
    END
    return m_state->WaitForReadyRead(msecs);
}

bool QLlcpSocketPrivate::waitForBytesWritten(int msecs)
{
    BEGIN
    END
    return m_state->WaitForBytesWritten(msecs);
}

bool QLlcpSocketPrivate::waitForConnected(int msecs)
{
    BEGIN
    bool val = false;
    Q_CHECK_LLCPTYPE_PARA_3(QLlcpSocketPrivate::waitForConnected(),connectionType2, val);
    END
    return m_state->WaitForConnected(msecs);
}

bool QLlcpSocketPrivate::waitForDisconnected(int msecs)
{
    BEGIN
    bool val = false;
    Q_CHECK_LLCPTYPE_PARA_3(QLlcpSocketPrivate::waitForDisconnected(),connectionType2, val);
    END
    return m_state->WaitForDisconnected(msecs);
}

void QLlcpSocketPrivate::changeState(QLLCPSocketState* state)
{
    BEGIN
    m_state = state;
    END
}

QTM_END_NAMESPACE
