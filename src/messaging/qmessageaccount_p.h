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
#include "qmessageaddress.h"

QTM_BEGIN_NAMESPACE

class QMessageAccountPrivate
{
    Q_DECLARE_PUBLIC(QMessageAccount)

public:
    QMessageAccountPrivate(QMessageAccount *account)
        :q_ptr(account)
    {
    }

    QMessageAccount *q_ptr;

    QMessageAccountId _id;
    QString _name;
#if defined(Q_OS_SYMBIAN)
    long int _service1EntryId; 
    long int _service2EntryId; 
#else
    QMessageAddress _address;
#endif    
    QMessage::TypeFlags _types;

#if defined(Q_OS_WIN)
    static QMessageAccount from(const QMessageAccountId &id, const QString &name, const QMessageAddress &address, const QMessage::TypeFlags &types);
#endif
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    static QMessageAccount from(const QMessageAccountId &id, const QString &name, const QMessageAddress &address, const QMessage::TypeFlags &types);
    static QMessageAccountPrivate* implementation(const QMessageAccount &account);
#endif
#if defined(Q_OS_SYMBIAN)
    static QMessageAccount from(const QMessageAccountId &id, const QString &name, long int service1EntryId, long int service2EntryId, const QMessage::TypeFlags &types);
#endif
};

QTM_END_NAMESPACE
