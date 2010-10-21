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
#ifndef QMESSAGEMANAGER_H
#define QMESSAGEMANAGER_H
#include <QObject>
#include <QSet>
#include <QMetaType>

#include <qmessageglobal.h>
#include <qmessagefilter.h>
#include <qmessagesortorder.h>
#include <qmessagefoldersortorder.h>
#include <qmessageaccountsortorder.h>
#include <qmessage.h>
#include <qmessagefolder.h>
#include <qmessageaccount.h>


QTM_BEGIN_NAMESPACE

class QMessageStore;

class Q_MESSAGING_EXPORT QMessageManager : public QObject
{
    Q_OBJECT

    friend class QMessageStorePrivate;

public:
    enum RemovalOption
    {
        RemoveLocalCopyOnly = 1,
        RemoveOnOriginatingServer
    };

    enum Error
    {
        NoError = 0,
        InvalidId,
        ConstraintFailure,
        ContentInaccessible,
        NotYetImplemented,
        FrameworkFault,
        WorkingMemoryOverflow,
        Busy,
        RequestIncomplete
    };

    typedef int NotificationFilterId;
    typedef QSet<QMessageManager::NotificationFilterId> NotificationFilterIdSet;

    QMessageManager(QObject *parent = 0);
    ~QMessageManager();

    QMessageManager::Error error() const;

    QMessageIdList queryMessages(const QMessageFilter &filter = QMessageFilter(), const QMessageSortOrder &sortOrder = QMessageSortOrder(), uint limit = 0, uint offset = 0) const;
    QMessageIdList queryMessages(const QMessageFilter &filter, const QList<QMessageSortOrder> &sortOrders, uint limit = 0, uint offset = 0) const;

    QMessageIdList queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags = 0, const QMessageSortOrder &sortOrder = QMessageSortOrder(), uint limit = 0, uint offset = 0) const;
    QMessageIdList queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QList<QMessageSortOrder> &sortOrders, uint limit = 0, uint offset = 0) const;

    QMessageFolderIdList queryFolders(const QMessageFolderFilter &filter = QMessageFolderFilter(), const QMessageFolderSortOrder &sortOrder = QMessageFolderSortOrder(), uint limit = 0, uint offset = 0) const;
    QMessageFolderIdList queryFolders(const QMessageFolderFilter &filter, const QList<QMessageFolderSortOrder> &sortOrders, uint limit = 0, uint offset = 0) const;

    QMessageAccountIdList queryAccounts(const QMessageAccountFilter &filter = QMessageAccountFilter(), const QMessageAccountSortOrder &sortOrder = QMessageAccountSortOrder(), uint limit = 0, uint offset = 0) const;
    QMessageAccountIdList queryAccounts(const QMessageAccountFilter &filter, const QList<QMessageAccountSortOrder> &sortOrders, uint limit = 0, uint offset = 0) const;

    int countMessages(const QMessageFilter &filter = QMessageFilter()) const;
    int countFolders(const QMessageFolderFilter &filter = QMessageFolderFilter()) const;
    int countAccounts(const QMessageAccountFilter &filter = QMessageAccountFilter()) const;

    bool addMessage(QMessage *m);
    bool updateMessage(QMessage *m);
    bool removeMessage(const QMessageId &id, RemovalOption option = RemoveOnOriginatingServer);
    bool removeMessages(const QMessageFilter &filter, RemovalOption option = RemoveOnOriginatingServer);

    QMessage message(const QMessageId &id) const;
    QMessageFolder folder(const QMessageFolderId &id) const;
    QMessageAccount account(const QMessageAccountId &id) const;

    NotificationFilterId registerNotificationFilter(const QMessageFilter &filter);
    void unregisterNotificationFilter(NotificationFilterId filterId);

Q_SIGNALS:
    void messageAdded(const QMessageId &id, const QMessageManager::NotificationFilterIdSet &matchingFilterIds);
    void messageRemoved(const QMessageId &id, const QMessageManager::NotificationFilterIdSet &matchingFilterIds);
    void messageUpdated(const QMessageId &id, const QMessageManager::NotificationFilterIdSet &matchingFilterIds);

private:
    QMessageStore *store;
};
QTM_END_NAMESPACE

Q_DECLARE_METATYPE(QTM_PREPEND_NAMESPACE(QMessageManager::NotificationFilterId))
Q_DECLARE_METATYPE(QTM_PREPEND_NAMESPACE(QMessageManager::NotificationFilterIdSet))


#endif
