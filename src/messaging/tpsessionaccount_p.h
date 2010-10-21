/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef TPSESSIONACCOUNT_H
#define TPSESSIONACCOUNT_H

#include <TelepathyQt4/Types>
#include <TelepathyQt4/Account>
#include <TelepathyQt4/Types>
#include <TelepathyQt4/PendingChannelRequest>
#include <TelepathyQt4/ChannelRequest>
#include <TelepathyQt4/PendingChannel>
#include <TelepathyQt4/PendingContacts>
#include <TelepathyQt4/Channel>
#include <TelepathyQt4/TextChannel>
#include <TelepathyQt4/AccountManager>
#include <TelepathyQt4/PendingReady>
#include <TelepathyQt4/ContactManager>

#include <QString>
#include <QVector>

#include "tpsessionchannel_p.h"

class SendRequest;

class TpSessionAccount: public QObject
{
    Q_OBJECT

public:
    TpSessionAccount(Tp::AccountManagerPtr amv, const QString &objectPath);
    bool sendMessage(SendRequest *sendRequest);

signals:
    void accountReady(TpSessionAccount *);
    void channelReady(TpSessionAccount *);
    void messageReceived(const Tp::ReceivedMessage &, TpSessionAccount *);
    void messageSent(const Tp::Message &, TpSessionAccount *);
    void newChannel(TpSessionAccount *, QString, QString, const Tp::ChannelDetails &);

private slots:
    void onReady(Tp::PendingOperation *op);
    void onOutgoingChannelReady(TpSessionChannel *ch);
    void onContactsConnectionReady(Tp::PendingOperation *op);
    void onNewContactRetrieved(Tp::PendingOperation *op);
    void onMessageReceived(const Tp::ReceivedMessage &, TpSessionChannel *);
    void onMessageSent(const Tp::Message &, Tp::MessageSendingFlags, const QString &);
    void onNewChannels(const Tp::ChannelDetailsList&);

public:
    Tp::Account *acc;

private:
    
    struct SendJob
    {
	SendJob(const QString &addr, SendRequest *sendReq)
	    : address(addr)
	    , sendRequest(sendReq)
	    , contactReady(false)
	{}

	QString address;
	SendRequest *sendRequest;
	bool contactReady;
    };

    typedef QList<SendJob> SendJobList;

    void makeContactFromAddress(const QString &address);
    Tp::ContactPtr getContactFromAddress(const QString &address) const;
    void addOutgoingChannel(const Tp::ContactPtr &contact);
    void addOutgoingChannel(const QString &address);
    TpSessionChannel *getChannelFromPeerAddress(const QString &id) const;

    bool ready;
    Tp::AccountPtr mAcc;
    Tp::ConnectionPtr contactsConn;
    QSet<Tp::ContactPtr> myContacts;
    QSet<TpSessionChannel *> myChannels;
    SendJobList jobs;
};

#endif // TPSESSIONACCOUNT_H
