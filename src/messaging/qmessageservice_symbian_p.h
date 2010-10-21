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


QTM_BEGIN_NAMESPACE

class QMessageServicePrivate : public QObject
{
	Q_OBJECT
    Q_DECLARE_PUBLIC(QMessageService)

public:
    QMessageServicePrivate(QMessageService* parent);
    ~QMessageServicePrivate();

	bool sendSMS(QMessage &message);
	bool sendMMS(QMessage &message);
	bool sendEmail(QMessage &message);
	bool show(const QMessageId& id);
	bool compose(const QMessage &message);
	bool retrieve(const QMessageId &messageId, const QMessageContentContainerId &id);
	bool retrieveBody(const QMessageId& id);
	bool retrieveHeader(const QMessageId& id);
	bool exportUpdates(const QMessageAccountId &id);
	
    bool queryMessages(const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset) const;
    bool queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset) const;
    
    bool countMessages(const QMessageFilter &filter);
	
    void setFinished(bool successful);
    
    void messagesFound(const QMessageIdList &ids, bool isFiltered, bool isSorted);
    void messagesCounted(int count);
    
    void cancel();

signals:
	void stateChanged(QMessageService::State);
	void messagesFound(const QMessageIdList&);
    //void messagesCounted(int count);
	void progressChanged(uint, uint);

private:	
    QMessageService *q_ptr;
    QMessageService::State _state;
    QMessageManager::Error _error;
    int _actionId;
    int _count;
    mutable bool _active;
    mutable int _pendingRequestCount;
    QMessageIdList _ids;
    mutable bool _sorted;
    mutable bool _filtered;

    mutable QMessageFilter _filter;
    mutable QMessageSortOrder _sortOrder;
    mutable int _limit;
    mutable int _offset;

    
    friend class CMTMEngine;
    friend class CFSEngine;
};



QTM_END_NAMESPACE
