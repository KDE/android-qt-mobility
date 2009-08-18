/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmfhelpers_p.h"
#include "qmessagestore.h"

#include <QRegExp>

namespace {

quint64 messageStatusMask(const QString &field)
{
    return QmfHelpers::convert(QMessageStore::instance())->messageStatusMask(field);
}

}

namespace QmfHelpers {

QMessageId convert(const QMailMessageId &id)
{
    return QMessageId(QString::number(id.toULongLong()));
}

QMailMessageId convert(const QMessageId &id)
{
    return QMailMessageId(id.toString().toULongLong());
}

QMessageAccountId convert(const QMailAccountId &id)
{
    return QMessageAccountId(QString::number(id.toULongLong()));
}

QMailAccountId convert(const QMessageAccountId &id)
{
    return QMailAccountId(id.toString().toULongLong());
}

QMessageFolderId convert(const QMailFolderId &id)
{
    return QMessageFolderId(QString::number(id.toULongLong()));
}

QMailFolderId convert(const QMessageFolderId &id)
{
    return QMailFolderId(id.toString().toULongLong());
}

/* in qmessagecontentcontainerid_qmf.cpp
QMessageContentContainerId convert(const QMailMessagePart::Location &location);

QMailMessagePart::Location convert(const QMessageContentContainerId &id);

QMessageContentContainerId bodyId(const QMailMessageId &id);
*/

QMessageIdList convert(const QList<QMailMessageId> &ids)
{
    QMessageIdList result;

    foreach (const QMailMessageId &id, ids) {
        result.append(convert(id));
    }

    return result;
}

QList<QMailMessageId> convert(const QMessageIdList &ids)
{
    QList<QMailMessageId> result;

    foreach (const QMessageId &id, ids) {
        result.append(convert(id));
    }

    return result;
}

QMessageAccountIdList convert(const QList<QMailAccountId> &ids)
{
    QMessageAccountIdList result;

    foreach (const QMailAccountId &id, ids) {
        result.append(convert(id));
    }

    return result;
}

QList<QMailAccountId> convert(const QMessageAccountIdList &ids)
{
    QList<QMailAccountId> result;

    foreach (const QMessageAccountId &id, ids) {
        result.append(convert(id));
    }

    return result;
}

QMessageFolderIdList convert(const QList<QMailFolderId> &ids)
{
    QMessageFolderIdList result;

    foreach (const QMailFolderId &id, ids) {
        result.append(convert(id));
    }

    return result;
}

QList<QMailFolderId> convert(const QMessageFolderIdList &ids)
{
    QList<QMailFolderId> result;

    foreach (const QMessageFolderId &id, ids) {
        result.append(convert(id));
    }

    return result;
}

QMessageContentContainerIdList convert(const QList<QMailMessagePart::Location> &locations)
{
    QMessageContentContainerIdList result;

    foreach (const QMailMessagePart::Location &location, locations) {
        result.append(convert(location));
    }

    return result;
}

QList<QMailMessagePart::Location> convert(const QMessageContentContainerIdList &ids)
{
    QList<QMailMessagePart::Location> result;

    foreach (const QMessageContentContainerId &id, ids) {
        result.append(convert(id));
    }

    return result;
}

QMailMessage::MessageType convert(QMessage::Type t)
{
    QMailMessage::MessageType result(QMailMessage::None);

    // This could be a single value or a mask
    if (t & QMessage::Mms) {
        result = static_cast<QMailMessage::MessageType>(result | QMailMessage::Mms);
    }
    if (t & QMessage::Sms) {
        result = static_cast<QMailMessage::MessageType>(result | QMailMessage::Sms);
    }
    if (t & QMessage::Email) {
        result = static_cast<QMailMessage::MessageType>(result | QMailMessage::Email);
    }
    if (t & QMessage::Xmpp) {
        result = static_cast<QMailMessage::MessageType>(result | QMailMessage::Instant);
    }

    return result;
}

QMessage::Type convert(QMailMessage::MessageType t)
{
    QMessage::Type result(QMessage::None);

    // This could be a single value or a mask
    if (t & QMailMessage::Mms) {
        result = static_cast<QMessage::Type>(static_cast<uint>(result | QMessage::Mms));
    }
    if (t & QMailMessage::Sms) {
        result = static_cast<QMessage::Type>(static_cast<uint>(result | QMessage::Sms));
    }
    if (t & QMailMessage::Email) {
        result = static_cast<QMessage::Type>(static_cast<uint>(result | QMessage::Email));
    }
    if (t & QMailMessage::Instant) {
        result = static_cast<QMessage::Type>(static_cast<uint>(result | QMessage::Xmpp));
    }

    return result;
}

QMailMessage::MessageType convert(QMessage::TypeFlags v)
{
    return convert(static_cast<QMessage::Type>(static_cast<uint>(v)));
}

QMailStore::ErrorCode convert(QMessageStore::ErrorCode v)
{
    switch (v) {
    case QMessageStore::InvalidId: return QMailStore::InvalidId;
    case QMessageStore::ConstraintFailure: return QMailStore::ConstraintFailure;
    case QMessageStore::ContentInaccessible: return QMailStore::ContentInaccessible;
    case QMessageStore::NotYetImplemented: return QMailStore::NotYetImplemented;
    case QMessageStore::FrameworkFault: return QMailStore::FrameworkFault;
    default: break;
    }

    return QMailStore::NoError;
}

QMessageStore::ErrorCode convert(QMailStore::ErrorCode v)
{
    switch (v) {
    case QMailStore::InvalidId: return QMessageStore::InvalidId;
    case QMailStore::ConstraintFailure: return QMessageStore::ConstraintFailure;
    case QMailStore::ContentInaccessible: return QMessageStore::ContentInaccessible;
    case QMailStore::NotYetImplemented: return QMessageStore::NotYetImplemented;
    case QMailStore::FrameworkFault: return QMessageStore::FrameworkFault;
    default: break;
    }

    return QMessageStore::NoError;
}

QMailStore::MessageRemovalOption convert(QMessageStore::RemovalOption v)
{
    switch (v) {
    case QMessageStore::RemoveOnOriginatingServer: return QMailStore::CreateRemovalRecord;
    default: break;
    }

    return QMailStore::NoRemovalRecord;
}

QMessageStore::RemovalOption convert(QMailStore::MessageRemovalOption v)
{
    switch (v) {
    case QMailStore::CreateRemovalRecord: return QMessageStore::RemoveOnOriginatingServer;
    default: break;
    }

    return QMessageStore::RemoveLocalCopyOnly;
}

QMailServiceAction::Activity convert(QMessageServiceAction::Activity v)
{
    switch (v) {
    case QMessageServiceAction::Pending: return QMailServiceAction::Pending;
    case QMessageServiceAction::InProgress: return QMailServiceAction::InProgress;
    case QMessageServiceAction::Successful: return QMailServiceAction::Successful;
    case QMessageServiceAction::Failed: return QMailServiceAction::Failed;
    default: break;
    }

    return QMailServiceAction::Pending;
}

QMessageServiceAction::Activity convert(QMailServiceAction::Activity v)
{
    switch (v) {
    case QMailServiceAction::Pending: return QMessageServiceAction::Pending;
    case QMailServiceAction::InProgress: return QMessageServiceAction::InProgress;
    case QMailServiceAction::Successful: return QMessageServiceAction::Successful;
    case QMailServiceAction::Failed: return QMessageServiceAction::Failed;
    default: break;
    }

    return QMessageServiceAction::Pending;
}

QMessage::StatusFlags convert(quint64 v)
{
    QMessage::StatusFlags result(0);

    if (v & (QMailMessage::Read | QMailMessage::ReadElsewhere)) {
        result |= QMessage::Read;
    }
    if (v & QMailMessage::HasAttachments) {
        result |= QMessage::HasAttachments;
    }
    if (v & QMailMessage::Incoming) {
        result |= QMessage::Incoming;
    }
    if (v & QMailMessage::Removed) {
        result |= QMessage::Removed;
    }

    return result;
}

quint64 convert(QMessage::Status v)
{
    // We cannot rely on the QMailMessage status masks until the store has been initialized
    static QMailStore *store = QMailStore::instance();
    Q_UNUSED(store);

    quint64 result(0);

    if (v & QMessage::Read) {
        result |= QMailMessage::Read;
    }
    if (v & QMessage::HasAttachments) {
        result |= QMailMessage::HasAttachments;
    }
    if (v & QMessage::Incoming) {
        result |= QMailMessage::Incoming;
    }
    if (v & QMessage::Removed) {
        result |= QMailMessage::Removed;
    }

    return result;
}

quint64 convert(QMessage::StatusFlags v)
{
    return convert(static_cast<QMessage::Status>(static_cast<uint>(v)));
}

QMessageAddress convert(const QMailAddress &address)
{
    QString addr(address.toString());
    if (!addr.isEmpty()) {
        QMessageAddress::Type type(QMessageAddress::Email);

        QRegExp suffix("\\s+\\(TYPE=(\\w*)\\)$");
        int index = suffix.indexIn(addr);
        if (index != -1) {
            addr = addr.left(addr.length() - suffix.cap(0).length());

            QString spec(suffix.cap(1));
            if (spec == "System") {
                type = QMessageAddress::System;
            } else if (spec == "Phone") {
                type = QMessageAddress::Phone;
            } else if (spec == "XMPP") {
                type = QMessageAddress::Xmpp;
            }
        }

        return QMessageAddress(addr, type);
    }

    return QMessageAddress();
}

QMailAddress convert(const QMessageAddress &address)
{
    QString suffix;
    if (address.type() == QMessageAddress::System) {
        suffix = " (TYPE=System)";
    } else if (address.type() == QMessageAddress::Phone) {
        suffix = " (TYPE=Phone)";
    } else if (address.type() == QMessageAddress::Xmpp) {
        suffix = " (TYPE=XMPP)";
    }

    return QMailAddress(address.recipient() + suffix);
}

QMessageAddressList convert(const QList<QMailAddress> &list)
{
    QList<QMessageAddress> result;

    foreach (const QMailAddress &address, list) {
        result.append(convert(address));
    }

    return result;
}

QList<QMailAddress> convert(const QMessageAddressList& list)
{
    QList<QMailAddress> result;

    foreach (const QMessageAddress &address, list) {
        result.append(convert(address));
    }

    return result;
}

QMessageDataComparator::EqualityComparator convert(QMailDataComparator::EqualityComparator cmp)
{
    switch (cmp)
    {
    case QMailDataComparator::Equal: return QMessageDataComparator::Equal;
    case QMailDataComparator::NotEqual: return QMessageDataComparator::NotEqual;
    default: break;
    }

    return QMessageDataComparator::Equal;
}

QMailDataComparator::EqualityComparator convert(QMessageDataComparator::EqualityComparator cmp)
{
    switch (cmp)
    {
    case QMessageDataComparator::Equal: return QMailDataComparator::Equal;
    case QMessageDataComparator::NotEqual: return QMailDataComparator::NotEqual;
    default: break;
    }

    return QMailDataComparator::Equal;
}

QMessageDataComparator::InclusionComparator convert(QMailDataComparator::InclusionComparator cmp)
{
    switch (cmp)
    {
    case QMailDataComparator::Includes: return QMessageDataComparator::Includes;
    case QMailDataComparator::Excludes: return QMessageDataComparator::Excludes;
    default: break;
    }
    
    return QMessageDataComparator::Includes;
}

QMailDataComparator::InclusionComparator convert(QMessageDataComparator::InclusionComparator cmp)
{
    switch (cmp)
    {
    case QMessageDataComparator::Includes: return QMailDataComparator::Includes;
    case QMessageDataComparator::Excludes: return QMailDataComparator::Excludes;
    default: break;
    }

    return QMailDataComparator::Includes;
}

QMessageDataComparator::RelationComparator convert(QMailDataComparator::RelationComparator cmp)
{
    switch (cmp)
    {
    case QMailDataComparator::LessThan: return QMessageDataComparator::LessThan;
    case QMailDataComparator::LessThanEqual: return QMessageDataComparator::LessThanEqual;
    case QMailDataComparator::GreaterThan: return QMessageDataComparator::GreaterThan;
    case QMailDataComparator::GreaterThanEqual: return QMessageDataComparator::GreaterThanEqual;
    default: break;
    }
    
    return QMessageDataComparator::LessThan;
}

QMailDataComparator::RelationComparator convert(QMessageDataComparator::RelationComparator cmp)
{
    switch (cmp)
    {
    case QMessageDataComparator::LessThan: return QMailDataComparator::LessThan;
    case QMessageDataComparator::LessThanEqual: return QMailDataComparator::LessThanEqual;
    case QMessageDataComparator::GreaterThan: return QMailDataComparator::GreaterThan;
    case QMessageDataComparator::GreaterThanEqual: return QMailDataComparator::GreaterThanEqual;
    default: break;
    }

    return QMailDataComparator::LessThan;
}

/* in qmessageaccountfilterkey_qmf.cpp
QMessageAccountFilterKey convert(const QMailAccountKey &key);
QMailAccountKey convert(const QMessageAccountFilterKey &key);
*/

/* in qmessagefolderfilterkey_qmf.cpp
QMessageFolderFilterKey convert(const QMailFolderKey &key);
QMailFolderKey convert(const QMessageFolderFilterKey &key);
*/

/* in qmessagefilterkey_qmf.cpp
QMessageFilterKey convert(const QMailMessageKey &key);
QMailMessageKey convert(const QMessageFilterKey &key);
*/

/* in qmessageaccountsortkey_qmf.cpp
QMessageAccountSortKey convert(const QMailAccountSortKey &key);
QMailAccountSortKey convert(const QMessageAccountSortKey &key);
*/

/* in qmessagefoldersortkey_qmf.cpp
QMessageFolderSortKey convert(const QMailFolderSortKey &key);
QMailFolderSortKey convert(const QMessageFolderSortKey &key);
*/

/* in qmessagesortkey_qmf.cpp
QMessageSortKey convert(const QMailMessageSortKey &key);
QMailMessageSortKey convert(const QMessageSortKey &key);
*/

/* in qmessageaccount_qmf.cpp
QMessageAccount convert(const QMailAccount &account);
QMailAccount convert(const QMessageAccount &account);
*/

/* in qmessagefolder_qmf.cpp
QMessageFolder convert(const QMailFolder &folder);
QMailFolder convert(const QMessageFolder &folder);
*/

/* in qmessage_qmf.cpp
QMessage convert(const QMailMessage &message);
QMailMessage convert(const QMessage &message);
QMailMessage* convert(QMessage *message);
*/

/* in qmessagestore_qmf.cpp
QMailStore *convert(QMessageStore *store);
const QMailStore *convert(const QMessageStore *store);
*/

quint64 highPriorityMask()
{
    static quint64 mask(messageStatusMask("QMessage::HighPriority"));
    return mask;
}

quint64 lowPriorityMask()
{
    static quint64 mask(messageStatusMask("QMessage::LowPriority"));
    return mask;
}

}

bool operator==(const QMailMessagePart::Location &lhs, const QMailMessagePart::Location &rhs)
{
    return (lhs.toString(true) == rhs.toString(true));
}

