/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qmessageservice.h"
#include "qmfservice_maemo6_p.h"

QTM_BEGIN_NAMESPACE

class QMessageServicePrivate
{
public:
    enum EnginesToCall
    {
        EnginesToCallQMF    = 0x1,
        EnginesToCallTelepathy = 0x2,
        // Extensible
        EnginesToCallAll = 0xFFFFFFFF,
    };

    QMessageServicePrivate(QMessageService* parent);
    ~QMessageServicePrivate();

    static QMessageServicePrivate* implementation(const QMessageService &service);

    bool queryMessages(QMessageService &messageService, const QMessageFilter &filter,
                       const QMessageSortOrder &sortOrder, uint limit, uint offset,
                       EnginesToCall enginesToCall = EnginesToCallAll);
    bool queryMessages(QMessageService &messageService, const QMessageFilter &filter,
                       const QString &body, QMessageDataComparator::MatchFlags matchFlags,
                       const QMessageSortOrder &sortOrder, uint limit, uint offset,
                       EnginesToCall enginesToCall = EnginesToCallAll);
    bool countMessages(QMessageService &messageService, const QMessageFilter &filter,
                       EnginesToCall enginesToCall = EnginesToCallAll);

    void setFinished(bool successful);
    void stateChanged(QMessageService::State state);
    void messagesFound(const QMessageIdList &ids, bool isFiltered, bool isSorted);
    void messagesCounted(int count);
    void progressChanged(uint value, uint total);

    bool compose(const QMessage &message);
    bool show(const QMessageId &id);

    void cancel();

public:
    QMessageService *q_ptr;
    QMFService *_qmfService;
    QMessageService::State _state;
    QMessageManager::Error _error;
    bool _active;
    int _actionId;
    int _pendingRequestCount;

    QMessageIdList _ids;
    int _count;
    bool _sorted;
    bool _filtered;

    QMessageFilter _filter;
    QMessageSortOrder _sortOrder;
    int _limit;
    int _offset;
};

QTM_END_NAMESPACE
