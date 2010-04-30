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


#ifndef QCONTACTPRESENCE_H
#define QCONTACTPRESENCE_H

#include <QString>
#include <QUrl>

#include "qtcontactsglobal.h"
#include "qcontactdetail.h"
#include "qcontact.h"

QTM_BEGIN_NAMESPACE

/* Leaf class */
class Q_CONTACTS_EXPORT QContactPresence : public QContactDetail
{
public:
#ifdef Q_QDOC
    static const QLatin1Constant DefinitionName;
    static const QLatin1Constant FieldTimestamp;
    static const QLatin1Constant FieldNickname;
    static const QLatin1Constant FieldPresenceState;
    static const QLatin1Constant FieldPresenceStateText;
    static const QLatin1Constant FieldPresenceStateImageUrl;
    static const QLatin1Constant FieldCustomMessage;
#else
    Q_DECLARE_CUSTOM_CONTACT_DETAIL(QContactPresence, "Presence")
    Q_DECLARE_LATIN1_CONSTANT(FieldTimestamp, "Timestamp");
    Q_DECLARE_LATIN1_CONSTANT(FieldNickname, "Nickname");
    Q_DECLARE_LATIN1_CONSTANT(FieldPresenceState, "PresenceState");
    Q_DECLARE_LATIN1_CONSTANT(FieldPresenceStateText, "PresenceStateText");
    Q_DECLARE_LATIN1_CONSTANT(FieldPresenceStateImageUrl, "PresenceStateImageUrl");
    Q_DECLARE_LATIN1_CONSTANT(FieldCustomMessage, "CustomMessage");
#endif

    enum PresenceState {
        PresenceUnknown = 0,
        PresenceAvailable,
        PresenceHidden,
        PresenceBusy,
        PresenceAway,
        PresenceExtendedAway,
        PresenceOffline,
    };

    void setTimestamp(const QDateTime& timestamp) {setValue(FieldTimestamp, timestamp);}
    QDateTime timestamp() const {return value<QDateTime>(FieldTimestamp);}
    void setNickname(const QString& nickname) {setValue(FieldNickname, nickname);}
    QString nickname() const {return value(FieldNickname);}
    void setPresenceState(PresenceState presence) {setValue(FieldPresenceState, static_cast<int>(presence));}
    PresenceState presenceState() const {return static_cast<PresenceState>(value<int>(FieldPresenceState));}
    void setPresenceStateText(const QString& presenceStateText) {setValue(FieldPresenceStateText, presenceStateText);}
    QString presenceStateText() const {return value(FieldPresenceStateText);}
    void setPresenceStateImageUrl(const QUrl& presenceStateImageUrl) {setValue(FieldPresenceStateImageUrl, presenceStateImageUrl);}
    QUrl presenceStateImageUrl() const {return value(FieldPresenceStateImageUrl);}
    void setCustomMessage(const QString& customMessage) {setValue(FieldCustomMessage, customMessage);}
    QString customMessage() const {return value(FieldCustomMessage);}
};

QTM_END_NAMESPACE

#endif

