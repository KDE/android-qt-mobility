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
#ifndef QMESSAGE_H
#define QMESSAGE_H
#include <qmessageglobal.h>
#include <qmessageaccountid.h>
#include <qmessagefolderid.h>
#include <qmessagecontentcontainer.h>
#include <qmessageaddress.h>

#include <QDateTime>
#include <QStringList>
#include <QTextStream>

class QMessagePrivate;

class Q_MESSAGING_EXPORT QMessage : public QMessageContentContainer {
    Q_DECLARE_PRIVATE(QMessage)

public:
    enum Type
    {
        NoType  = 0,
        Mms     = 0x1,
        Sms     = 0x2,
        Email   = 0x4,
        Xmpp    = 0x8,
        // Extensible
        AnyType = 0xFFFFFFFF
    };
    Q_DECLARE_FLAGS(TypeFlags, Type)

    enum Status
    {
        Read = 0x1,
        HasAttachments = 0x2,
        Incoming = 0x4,
        Removed = 0x8
    };
    Q_DECLARE_FLAGS(StatusFlags, Status)

    enum Priority
    {
        HighPriority = 1,
        NormalPriority,
        LowPriority
    };

    enum StandardFolder
    {
        InboxFolder = 1,
        OutboxFolder,
        DraftsFolder,
        SentFolder,
        TrashFolder
    };

    enum ResponseType
    {
        ReplyToSender = 1,
        ReplyToAll,
        Forward
    };

    QMessage();
    QMessage(const QMessageId &id);
    QMessage(const QMessage &other);
    virtual ~QMessage();

    const QMessage& operator=(const QMessage &other);

    static QMessage fromTransmissionFormat(Type t, const QByteArray &ba);
    static QMessage fromTransmissionFormatFile(Type t, const QString &fileName);

    QByteArray toTransmissionFormat() const;
    void toTransmissionFormat(QDataStream &out) const;

    QMessageId id() const;

    Type type() const;
    void setType(Type t);

    QMessageAccountId parentAccountId() const;
    void setParentAccountId(const QMessageAccountId &accountId);

#ifdef QMESSAGING_OPTIONAL_FOLDER
    QMessageFolderId parentFolderId() const;
#endif

    StandardFolder standardFolder() const;
    void setStandardFolder(StandardFolder sf);

    QMessageAddress from() const;
    void setFrom(const QMessageAddress &address);

    QString subject() const;
    void setSubject(const QString &s);

    QDateTime date() const;
    void setDate(const QDateTime &d);

    QDateTime receivedDate() const;
    void setReceivedDate(const QDateTime &d);

    QMessageAddressList to() const;
    void setTo(const QMessageAddressList &toList);
    void setTo(const QMessageAddress &address);

    QMessageAddressList cc() const;
    void setCc(const QMessageAddressList &ccList);

    QMessageAddressList bcc() const;
    void setBcc(const QMessageAddressList &bccList);

    StatusFlags status() const;
    void setStatus(StatusFlags newStatus);
    void setStatus(Status flag, bool set = true);

    Priority priority() const;
    void setPriority(Priority newPriority);

    uint size() const;

    QMessageContentContainerId body() const;

    void setBody(const QString &body, const QByteArray &mimeType = QByteArray());
    void setBody(QTextStream &in, const QByteArray &mimeType = QByteArray());

    QMessageContentContainerIdList attachments() const;

    void appendAttachments(const QStringList &fileNames);
    void clearAttachments();

#ifdef QMESSAGING_OPTIONAL
    void setOriginatorPort(uint port);
    uint originatorPort();
    void setDestinationPort(uint port);
    uint destinationPort();

    QString customField(const QString &name) const;
    void setCustomField(const QString &name, const QString &value);
    QStringList customFields() const;
#endif

    bool isModified() const;

    QMessage createResponseMessage(ResponseType type) const;

    static void setPreferredCharsets(const QList<QByteArray> &charsetNames);
    static QList<QByteArray> preferredCharsets();

    static QByteArray preferredCharsetFor(const QString &text);

// TODO: Why is there no setParentFolderId()?
//private:
    friend class QMessageStore;

    QMessagePrivate *d_ptr;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QMessage::TypeFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(QMessage::StatusFlags)
#endif
