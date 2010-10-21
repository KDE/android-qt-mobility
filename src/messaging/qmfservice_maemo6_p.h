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
#ifndef QMFSERVICE_MAEMO6_H
#define QMFSERVICE_MAEMO6_H
#include <QObject>
#include <QProcess>
#include <qmessageglobal.h>
#include <qmessagemanager.h>
#include <qmessageservice.h>
#include <qmessage.h>
#include <qmailserviceaction.h>

class QTimer;

QTM_BEGIN_NAMESPACE

class QMFService : public QObject
{
    Q_OBJECT

    friend class QMFServicePrivate;

public:

    QMFService(QMessageService *service, QObject *parent = 0);
    ~QMFService();

    bool queryMessages(const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset);
    bool queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset);

    bool countMessages(const QMessageFilter &filter);

    bool send(QMessage &message);
    bool compose(const QMessage &message);
    bool retrieveHeader(const QMessageId &id);
    bool retrieveBody(const QMessageId &id);
    bool retrieve(const QMessageId &messageId, const QMessageContentContainerId &id);
    bool show(const QMessageId &id);
    bool exportUpdates(const QMessageAccountId &id);
    QMessageService::State state() const;

public slots:
    void cancel();

protected slots:
    void transmitActivityChanged(QMailServiceAction::Activity a);
    void retrievalActivityChanged(QMailServiceAction::Activity a);
    void statusChanged(const QMailServiceAction::Status &s);
    void completed();
    void reportMatchingIds();
    void reportMatchingCount();
    void findMatchingIds();
    void testNextMessage();

private:

    QTimer *timer();
    QMessageManager::Error error() const;
    void setError(QMessageManager::Error error) const;
    void stateChanged(QMessageService::State state) const;

    void matchQueryCompleted();
    bool messageMatch(const QMailMessageId &messageid);
    bool isBusy() const;

    QMessageService *m_service;
    QTimer *m_timer;
    QMailTransmitAction m_transmit;
    QMailRetrievalAction m_retrieval;
    QMailServiceAction *m_active;
    bool m_activeStoreAction;

    QList<QMessageId> m_matchingIds;
    QList<QMessageId> m_resultIds;
    QList<QMailMessageId> m_candidateIds;
    QMessageFilter m_lastFilter;
    QMessageSortOrder m_lastOrdering;
    QString m_match;
    int m_limit;
    int m_offset;

    QMessageDataComparator::MatchFlags m_matchFlags;

    QMailMessageIdList m_transmitIds;
};

QTM_END_NAMESPACE
#endif
