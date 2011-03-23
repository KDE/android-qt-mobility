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
#ifndef TELEPATHYENGINE_MAEMO6_H
#define TELEPATHYENGINE_MAEMO6_H

#include "qmessageglobal.h"
#include "qmessagemanager.h"
#include "qmessageaccount.h"
#include "qmessageaccountid.h"
#include "qmessage.h"
#include "qmessageservice.h"

#include <TelepathyQt4/Types>
#include <TelepathyQt4/PendingOperation>
#include <TelepathyQt4/AccountManager>
#include <TelepathyQt4/AccountSet>
#include <TelepathyQt4/ContactMessenger>

QTM_USE_NAMESPACE

class  TelepathyEngine : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(TelepathyEngine)
public:
    static TelepathyEngine *instance();

    TelepathyEngine();
    ~TelepathyEngine();

    QMessageAccountIdList queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountSortOrder &sortOrder,
                                        uint limit, uint offset, bool &isFiltered, bool &isSorted);
    int countAccounts(const QMessageAccountFilter &filter);
    QMessageAccount account(const QMessageAccountId &id);
    QMessageAccountId defaultAccount(QMessage::Type type);

    bool sendMessage(QMessage &message, QMessageService *service);
    QMessageManager::Error error() const;

private Q_SLOTS:
    void onAMReady(Tp::PendingOperation *);
    void onAccountReady(Tp::PendingOperation *);
    void onAccountAdded(const Tp::AccountPtr &account);
    void onAccountRemoved(const Tp::AccountPtr &account);

private:
    void syncDone();
    void addAccount(const Tp::AccountPtr &acc);
    QMessageManager::Error convertError(const Tp::PendingOperation *op);

    typedef QPair<Tp::AccountPtr, QMessageAccount> AccountPair;
    QMap<QString, AccountPair> m_accounts;

    Tp::AccountManagerPtr m_AM;
    Tp::AccountSetPtr m_accountSet;

    // Synchronous initialization
    bool m_sync;
    QEventLoop m_loop;
    QStringList m_initList;

    QMessageManager::Error m_error;
};

class SendRequest : public QObject
{
    Q_OBJECT
public:
    SendRequest(const QMessage &message, QMessageService *service);
    ~SendRequest();

    QStringList to() const;
    QString text() const;	  

    void setFinished(const QString &address, bool success);
    void addMessenger(const Tp::ContactMessengerPtr &);
    int requestCount() const;

public slots:
    void finished(Tp::PendingOperation *operation, bool processLater = false);
    void onServiceDestroyed(QObject*);

private slots:
    friend class QTimer;
    void down();

private:
    QMessageService *_service;
    QMessage _message;
    int _pendingRequestCount;
    int _failCount;
    QList<Tp::ContactMessengerPtr> _messengerList;
};

#endif // TELEPATHYENGINE_MAEMO_P_H
