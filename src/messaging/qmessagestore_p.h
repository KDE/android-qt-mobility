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
#ifndef QMESSAGESTOREPRIVATE_H
#define QMESSAGESTOREPRIVATE_H
#include "qmessagestore.h"
#ifdef Q_OS_WIN
QT_BEGIN_NAMESPACE
class QMutex;
QT_END_NAMESPACE
#endif

QTM_BEGIN_NAMESPACE

class QMessageStorePrivatePlatform;

class QMessageStorePrivate
{
    Q_DECLARE_PUBLIC(QMessageStore)

public:
    QMessageStorePrivate();
    ~QMessageStorePrivate();

    void initialize(QMessageStore *store);

    QMessageStore *q_ptr;
    QMessageStorePrivatePlatform *p_ptr;

#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    enum NotificationType
    {
        Added,
        Updated,
        Removed
    };
    void messageNotification(QMessageStorePrivate::NotificationType type, const QMessageId& id,
                             const QMessageManager::NotificationFilterIdSet &matchingFilters);
#endif
#ifdef Q_WS_MAEMO_6
    mutable QMessageManager::Error error;
#endif
#ifdef Q_OS_WIN
    static QMutex* mutex(QMessageStore*);
    static QMutex* mutex(QMessageManager&);
#endif

};

QTM_END_NAMESPACE
#endif
