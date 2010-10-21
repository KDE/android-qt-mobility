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
#ifndef QMESSAGE_P_H
#define QMESSAGE_P_H

#include "qmessage.h"
#include "qmessageaddress.h"
#include <QMap>

#ifdef USE_QMF_IMPLEMENTATION
#include "qmfhelpers_p.h"
#endif



QTM_BEGIN_NAMESPACE

class QMessagePrivate
{
#ifdef USE_QMF_IMPLEMENTATION
public:
    QMessagePrivate() 
    {
        _message.setStatus(QMailMessage::LocalOnly);
    }

    QMailMessage _message;

    typedef QMap<QMessage::StandardFolder, QMessageFolderId> StandardFolderMap;
    Q_SCOPED_STATIC_DECLARE(StandardFolderMap,standardFolderMap);

    static QMessageFolderId standardFolderId(QMessage::StandardFolder folder);
    static QMessage::StandardFolder standardFolder(QMessageFolderId folderId);

    static QMessage convert(const QMailMessage &message);
    static QMailMessage convert(const QMessage &message);
    static QMailMessage *convert(QMessage *message);

    //static const QMailMessage *convert(const QMessage *message);
#else
    Q_DECLARE_PUBLIC(QMessage)

public:

    QMessagePrivate(QMessage *message)
        :q_ptr(message),
#if defined(Q_OS_WIN)
         _hasAttachments(false),
         _rtfInSync(false),
         _contentFormat(0),
#endif
         _size(0),
         _standardFolder(QMessage::DraftsFolder),
         _type(QMessage::NoType),
         _status(0),
         _priority(QMessage::NormalPriority),
         _modified(false)
    {
#if defined(Q_OS_WIN)
        _elementsPresent.properties = 1;
        _elementsPresent.recipients = 1;
        _elementsPresent.body = 1;
        _elementsPresent.attachments = 1;
#endif
    }

    QMessage *q_ptr;
#if defined(Q_OS_WIN)
    struct {
        unsigned properties : 1;
        unsigned recipients : 1;
        unsigned body : 1;
        unsigned attachments : 1;
    } _elementsPresent;
    bool _hasAttachments;
    bool _rtfInSync;
    LONG _contentFormat;
#endif

    QMessageId _id;
    int _size;
    QMessageAccountId _parentAccountId;
    QMessageFolderId _parentFolderId;
    QMessage::StandardFolder _standardFolder;

    QMessage::Type _type;
    QMessageAddress _from;
    QString _senderName;
    QMessageAddressList _to;
    QMessageAddressList _cc;
    QMessageAddressList _bcc;
    QMessage::StatusFlags _status;
    QMessage::Priority _priority;
    QString _subject;
    QDateTime _date;
    QDateTime _receivedDate;
    bool _modified;
    QMessageContentContainerId _bodyId;

#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    QString _url;
    bool _mimeInformationRetrieved;

    static QMessagePrivate* implementation(const QMessage &message);
    static QMessageContentContainerPrivate* containerImplementation(const QMessage &message);
#endif

#if defined(Q_WS_MAEMO_6)
    QString _contentScheme;
    QString _contentIdentifier;

    typedef QMap<QMessage::StandardFolder, QMessageFolderId> StandardFolderMap;
    Q_SCOPED_STATIC_DECLARE(StandardFolderMap,standardFolderMap);

    static QMessageFolderId standardFolderId(QMessage::StandardFolder folder);
    static QMessage::StandardFolder standardFolder(QMessageFolderId folderId);
#endif

    static QMessage from(const QMessageId &id);
    static QString senderName(const QMessage &message);
    static void setSenderName(const QMessage &message, const QString &senderName);
    static void setSize(const QMessage &message, int size);
    static void setParentFolderId(QMessage& message, const QMessageFolderId& id);

#if defined(Q_OS_WIN)
    void ensurePropertiesPresent(QMessage *msg) const;
    void ensureRecipientsPresent(QMessage *msg) const;
    void ensureBodyPresent(QMessage *msg) const;
    void ensureAttachmentsPresent(QMessage *msg) const;
#endif
#endif

public:
    static void setStandardFolder(QMessage& message, QMessage::StandardFolder sf);
};


QTM_END_NAMESPACE
#endif
