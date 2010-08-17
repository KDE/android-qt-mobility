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

#ifndef QFSENGINE_SYMBIAN_H
#define QFSENGINE_SYMBIAN_H

#include <qmessageglobal.h>
#include <QObject>
#include <QMap>
#include <QSet>
#include <QList>
#include <QHash>

#include <e32base.h>
#include <nmapimailbox.h>
#include <nmapimessage.h>
#include <nmapimessageenvelope.h>
#include <nmapifolder.h>
#include <nmapimailboxlisting.h>
#include <nmapienvelopelisting.h>
#include <nmapimessagesearch.h>
#include <nmapicommon.h>

#include "qmessagemanager.h"
#include "qmessagestore_symbian_p.h"
#include "qmessagefilter.h"
#include "qmessagefilter_p.h"
#include "qmessagefolderfilter.h"
#include "qmessageservice.h"

using namespace EmailClientApi;

QTM_BEGIN_NAMESPACE

class CFSMessagesFindOperation;
class QMessageId;
class QMessageAccount;

struct FSMessageQueryInfo
{
    int operationId;
    bool isQuery;
    QString body;
    QMessageDataComparator::MatchFlags matchFlags;
    QMessageFilter filter;
    QMessageSortOrder sortOrder;
    int offset;
    int limit;
    QMessageServicePrivate* privateService;
    CFSMessagesFindOperation* findOperation;
    int currentFilterListIndex;
    QMessageIdList ids;
    int count;
};

struct FSSearchOperation
{
    enum FSSearchOperationType {
        SearchFolder,
        SearchAccount
    };

    enum FSSearchOperationStatus {
        SearchActive,
        SearchQueued
    };
    NmApiEnvelopeListing* m_FolderSearch;
    NmApiMessageSearch* m_AccountSearch;
    FSSearchOperationType m_Type;
    FSSearchOperationStatus m_Status;
};


class CFSEngine: public QObject
{
    Q_OBJECT;
    
public: 
    
    static CFSEngine* instance();

    CFSEngine();
    ~CFSEngine();
    
    QMessageAccountIdList queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountSortOrder &sortOrder, uint limit, uint offset) const;
    int countAccounts(const QMessageAccountFilter &filter) const;
    QMessageAccount account(const QMessageAccountId &id) const;
    QMessageAccountId defaultAccount(QMessage::Type type) const;
    
    QMessageFolderIdList queryFolders(const QMessageFolderFilter &filter, const QMessageFolderSortOrder &sortOrder, uint limit, uint offset) const;
    int countFolders(const QMessageFolderFilter &filter) const;
    QMessageFolder folder(const QMessageFolderId &id) const;
    
    bool sendEmail(QMessage &message);
    bool addMessage(QMessage *m);
    bool updateMessage(QMessage *m);
    bool removeMessage(const QMessageId &id, QMessageManager::RemovalOption option);
    bool removeMessages(const QMessageFilter &filter, QMessageManager::RemovalOption option);
    bool queryMessages(QMessageServicePrivate& privateService, const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset) const;
    bool queryMessages(QMessageServicePrivate& privateService, const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset) const;
    bool countMessages(QMessageServicePrivate& privateService, const QMessageFilter &filter);
    bool showMessage(const QMessageId &id);
    bool composeMessage(const QMessage &message);   
    QMessage message(const QMessageId& id) const;
   
    bool retrieve(QMessageServicePrivate& privateService, const QMessageId &messageId, const QMessageContentContainerId& id);
    bool retrieveBody(QMessageServicePrivate& privateService, const QMessageId& id);
    bool retrieveHeader(QMessageServicePrivate& privateService, const QMessageId& id);
    bool exportUpdates(const QMessageAccountId &id);
    
    QMessageManager::NotificationFilterId registerNotificationFilter(QMessageStorePrivate& aPrivateStore,
                                        const QMessageFilter& filter, QMessageManager::NotificationFilterId aId);
    void unregisterNotificationFilter(QMessageManager::NotificationFilterId notificationFilterId);

    void filterAndOrderMessagesReady(bool success, int operationId, QMessageIdList ids, int numberOfHandledFilters,
                                     bool resultSetOrdered);
    
    void setMtmAccountIdList(QMessageAccountIdList accountList);
      
private:

    void queryMessagesL(QMessageServicePrivate& privateService, const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset) const;
    void queryMessagesL(QMessageServicePrivate& privateService, const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset) const;
    void countMessagesL(QMessageServicePrivate& privateService, const QMessageFilter &filter);
    
    QMessageAccountIdList accountsByType(QMessage::Type type) const;
    void updateEmailAccounts() const;
    void updateMessageL(QMessage* message);
    EmailClientApi::NmApiMessage* createFSMessage(QMessage& message);
    
    QMessageFolderIdList folderIdsByAccountId(const QMessageAccountId& accountId) const;
    QMessageFolderIdList filterMessageFoldersL(const QMessageFolderFilter& filter, bool& filterHandled) const;
    QMessageFolderIdList allFolders() const;
    bool fsFolderL(const QMessageFolderId& id, NmApiMailbox* mailbox, NmApiFolder* folder) const;
    QList<NmApiFolder> getFolderListByAccountId(const quint64 mailboxId) const;
    NmApiEmailFolderType standardFolderId(QMessage::StandardFolder standardFolder);
    
    QMessageFolderIdList filterMessageFolders(const QMessageFolderFilter& filter, bool& filterHandled) const;
    QMessage CreateQMessage(EmailClientApi::NmApiMessage* aMessage) const; 

    void addAttachmentToMessage(QMessage& message, QMessageContentContainer& attachment) const;
    QDateTime symbianTTimetoQDateTime(const TTime& time) const;
    TTime qDateTimeToSymbianTTime(const QDateTime& date) const;
    
    QMessage messageL(const QMessageId& id) const;
    static bool accountLessThan(const QMessageAccountId accountId1, const QMessageAccountId accountId2);
    void orderAccounts(QMessageAccountIdList& accountIds,  const QMessageAccountSortOrder &sortOrder) const;
    void applyOffsetAndLimitToAccountIds(QMessageAccountIdList& idList, int offset, int limit) const;
    
    static bool folderLessThan(const QMessageFolderId folderId1, const QMessageFolderId folderId2);
    void orderFolders(QMessageFolderIdList& folderIds,  const QMessageFolderSortOrder &sortOrder) const;
    void applyOffsetAndLimitToMsgFolderIds(QMessageFolderIdList& idList, int offset, int limit) const;
    void handleNestedFiltersFromFolderFilter(QMessageFolderFilter &filter) const;

    static bool messageLessThan(const QMessage& message1, const QMessage& message2);
    void orderMessages(QMessageIdList& messageIds,  const QMessageSortOrder &sortOrder) const;
    void applyOffsetAndLimitToMsgIds(QMessageIdList& idList, int offset, int limit) const;

    void handleNestedFiltersFromMessageFilter(QMessageFilter &filter) const;
    void exportUpdatesL(const QMessageAccountId &id);
       
    friend class QMessageService;
    friend class CMessagesFindOperation;
    
private:

    mutable QHash<QString, QMessageAccount> m_accounts;
    mutable int m_operationIds;
    mutable QList<FSMessageQueryInfo> m_messageQueries;
    QMessageAccountIdList m_mtmAccountList;

    QMessageStorePrivate* ipMessageStorePrivate;
    bool iListenForNotifications;
    QMessageManager::NotificationFilterId m_filterId;
    QMap<QMessageManager::NotificationFilterId, QMessageFilter> m_filters;
    QMessageAccount m_account;
    QMessageServicePrivate* m_privateService;
    bool m_createMessageError;
    friend class QMessageService;
    friend class CFSMessagesFindOperation;
    
    mutable QMessageAccountSortOrder m_currentAccountOrdering;
    mutable QMessageFolderSortOrder m_currentFolderOrdering;
    mutable QMessageSortOrder m_currentMessageOrdering;


};

class CFSMessagesFindOperation : public QObject
{
    Q_OBJECT
    
public:
    CFSMessagesFindOperation(CFSEngine& aOwner, int aOperationId); 
    ~CFSMessagesFindOperation();

    void filterAndOrderMessages(const QMessageFilter& filter,
                                const QMessageSortOrder& sortOrder,
                                const QString body = QString(),
                                QMessageDataComparator::MatchFlags matchFlags = 0);
    void filterAndOrderMessages(const QMessageFilterPrivate::SortedMessageFilterList& filters,
                                const QMessageSortOrder& sortOrder,
                                const QString body = QString(),
                                QMessageDataComparator::MatchFlags matchFlags = 0);

    enum Field {None = 0, Sender, Recipients, Subject, Body};
 
public slots:
    void searchOperationCompleted();
    void searchCompleted();

private:
    //void getAllMessagesL();
    void getAllMessages(NmApiMailSortCriteria& sortCriteria);
    void getFolderSpecificMessages(QMessageFolder& messageFolder);
    void getAccountSpecificMessages(QMessageAccount& messageAccount, NmApiMailSortCriteria& sortCriteria);
    
    bool fillsSearchKeyCriteria(QMessageId& messageId);

    
private: // Data
    CFSEngine& m_owner;
    
    int m_numberOfHandledFilters;
    int m_operationId;
    int m_activeSearchCount;
    //TMsvSelectionOrdering iOrdering;
    bool m_resultCorrectlyOrdered;
    QMessageIdList m_idList;
    QMessageFilterPrivate::SortedMessageFilterList m_filterList;
    QMessageIdList m_excludeIdList;

    bool m_receiveNewMessages;
    QList<FSSearchOperation> m_searchOperations;
    
    Field m_searchField;
    QMessageDataComparator::MatchFlags m_matchFlags;
    QString m_searchKey;
};


QTM_END_NAMESPACE

#endif // QFSENGINE_SYMBIAN_H
