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

#ifndef QNEARFIELDTARGET_MAEMO6_P_H
#define QNEARFIELDTARGET_MAEMO6_P_H

#include <qmobilityglobal.h>

#include "qnearfieldmanager_maemo6_p.h"

#include <qnearfieldtarget.h>
#include <qnearfieldtarget_p.h>
#include <qnearfieldtagtype1.h>
#include <qnearfieldtagtype2.h>
#include <qnearfieldtagtype3.h>
#include <qnearfieldtagtype4.h>
#include <qndefmessage.h>

#include "maemo6/adapter_interface_p.h"
#include "maemo6/target_interface_p.h"
#include "maemo6/tag_interface_p.h"

#include <QtDBus/QDBusPendingReply>

using namespace com::nokia::nfc;

QTM_BEGIN_NAMESPACE

class PendingCallWatcher : public QObject
{
    Q_OBJECT

public:
    PendingCallWatcher(QObject *parent) : QObject(parent) { }

    void addSendCommand(const QDBusPendingReply<QByteArray> &reply,
                        const QNearFieldTarget::RequestId &id);
    void addReadNdefMessages(const QDBusPendingReply<QList<QByteArray> > &reply,
                             const QNearFieldTarget::RequestId &id);
    void addWriteNdefMessages(const QDBusPendingReply<> &reply,
                              const QNearFieldTarget::RequestId &id);

private slots:
    void sendCommandFinished(QDBusPendingCallWatcher *watcher);
    void readNdefMessagesFinished(QDBusPendingCallWatcher *watcher);
    void writeNdefMessages(QDBusPendingCallWatcher *watcher);

private:
    QMap<QDBusPendingCallWatcher *, QNearFieldTarget::RequestId> m_pendingCommands;
    QMap<QDBusPendingCallWatcher *, QNearFieldTarget::RequestId> m_pendingNdefReads;
    QMap<QDBusPendingCallWatcher *, QNearFieldTarget::RequestId> m_pendingNdefWrites;
};

template <typename T>
class NearFieldTarget : public T
{
public:
    NearFieldTarget(QNearFieldManagerPrivateImpl *manager, Target *target, Tag *tag)
    :   T(manager), m_manager(manager), m_target(target), m_tag(tag),
        m_callWatcher(new PendingCallWatcher(this))
    {
    }

    ~NearFieldTarget()
    {
        delete m_tag;
        delete m_target;
    }

    QByteArray uid() const
    {
        return m_tag->uID();
    }

    QNearFieldTarget::Type type() const
    {
        if (!m_tag)
            return QNearFieldTarget::NfcForumDevice;

        return T::type();
    }

    QNearFieldTarget::AccessMethods accessMethods() const
    {
        QNearFieldTarget::AccessMethods result = QNearFieldTarget::NdefAccess;
#ifdef MAEMO6_TAG_TYPE_SEPECIFIC_ACCESS_SUPPORTED
        if (m_tag)
            result |= QNearFieldTarget::TagTypeSpecificAccess;
#endif
        if (!m_tag)
            result |= QNearFieldTarget::LlcpAccess;

        return result;
    }

    bool hasNdefMessage()
    {
        return true;
    }

    QNearFieldTarget::RequestId readNdefMessages()
    {
        QNearFieldTarget::RequestId id(new QNearFieldTarget::RequestIdPrivate);

        QDBusPendingReply<QList<QByteArray> > reply = m_tag->ReadNDEFData();
        m_callWatcher->addReadNdefMessages(reply, id);

        return id;
    }

    QNearFieldTarget::RequestId writeNdefMessages(const QList<QNdefMessage> &messages)
    {
        QNearFieldTarget::RequestId id(new QNearFieldTarget::RequestIdPrivate);
        QList<QByteArray> rawMessages;

        foreach (const QNdefMessage &message, messages)
            rawMessages.append(message.toByteArray());

        QDBusPendingReply<> reply = m_tag->WriteNDEFData(rawMessages);
        m_callWatcher->addWriteNdefMessages(reply, id);

        return id;
    }

    QNearFieldTarget::RequestId sendCommand(const QByteArray &command)
    {
#if 0
        quint16 crc = qNfcChecksum(command.constData(), command.length());

        QNearFieldTarget::RequestId id(new QNearFieldTarget::RequestIdPrivate);

        QDBusPendingReply<QByteArray> reply =
            m_tag->RawRequest(command + char(crc & 0xff) + char(crc >> 8));

        m_callWatcher->addSendCommand(reply, id);

        return id;
#else
        Q_UNUSED(command);
        return QNearFieldTarget::RequestId();
#endif
    }

    QNearFieldTarget::RequestId sendCommands(const QList<QByteArray> &commands)
    {
#if 0
        for (int i = 0; i < commandCount; ++i) {
            reply[i] = m_tag->RawRequest(commands.at(i));
        }

        QList<QByteArray> results;
        for (int i = 0; i < commandCount; ++i) {
            reply[i].waitForFinished();
            results.append(reply[i].isError() ? QByteArray() : reply->value());
        }

        return results;
#else
        Q_UNUSED(commands);
        return QNearFieldTarget::RequestId();
#endif
    }

protected:
    QNearFieldManagerPrivateImpl *m_manager;
    Target *m_target;
    Tag *m_tag;
    PendingCallWatcher *m_callWatcher;
};

template <>
inline QNearFieldTarget::Type NearFieldTarget<QNearFieldTarget>::type() const
{
    const QString tagType = m_tag->technology();
    if (tagType == QLatin1String("jewel"))
        return QNearFieldTarget::NfcTagType1;
    else if (tagType == QLatin1String("mifare-ul"))
        return QNearFieldTarget::NfcTagType2;
    else if (tagType == QLatin1String("felica"))
        return QNearFieldTarget::NfcTagType3;
    else if (tagType == QLatin1String("iso-4a"))
        return QNearFieldTarget::NfcTagType4;
    else if (tagType == QLatin1String("mifare-1k"))
        return QNearFieldTarget::MifareTag;
    else
        return QNearFieldTarget::ProprietaryTag;
}

class TagType1 : public NearFieldTarget<QNearFieldTagType1>
{
public:
    TagType1(QNearFieldManagerPrivateImpl *manager, Target *target, Tag *tag)
    :   NearFieldTarget<QNearFieldTagType1>(manager, target, tag)
    {
    }

    ~TagType1()
    {
    }

    int memorySize() const;
};

class TagType2 : public NearFieldTarget<QNearFieldTagType2>
{
public:
    TagType2(QNearFieldManagerPrivateImpl *manager, Target *target, Tag *tag)
    :   NearFieldTarget<QNearFieldTagType2>(manager, target, tag)
    {
    }

    ~TagType2()
    {
    }

    int memorySize() const;
};

class TagType3 : public NearFieldTarget<QNearFieldTagType3>
{
public:
    TagType3(QNearFieldManagerPrivateImpl *manager, Target *target, Tag *tag)
    :   NearFieldTarget<QNearFieldTagType3>(manager, target, tag)
    {
    }

    ~TagType3()
    {
    }

    int memorySize() const;
};

class TagType4 : public NearFieldTarget<QNearFieldTagType4>
{
public:
    TagType4(QNearFieldManagerPrivateImpl *manager, Target *target, Tag *tag)
    :   NearFieldTarget<QNearFieldTagType4>(manager, target, tag)
    {
    }

    ~TagType4()
    {
    }

    int memorySize() const;
};

QTM_END_NAMESPACE

#endif // QNEARFIELDTARGET_MAEMO6_P_H