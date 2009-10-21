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
#include "qmessageserviceaction.h"
#include "qmfhelpers_p.h"

#include <qmailserviceaction.h>

#include <QTimer>
#include <qmessage_p.h>

namespace {

struct TextPartSearcher
{
    QString _search;

    TextPartSearcher(const QString &searchText) : _search(searchText) {}

    bool operator()(const QMailMessagePart &part)
    {
        if (part.contentType().type().toLower() == "text") {
            if (part.body().data().contains(_search, Qt::CaseInsensitive)) {
                // We have located some matching text - stop the traversal
                return false;
            }
        }

        return true;
    }
};

}

using namespace QmfHelpers;

class QMessageServiceActionPrivate : public QObject
{
    Q_OBJECT

public:
    QMessageServiceActionPrivate();

    QMailTransmitAction _transmit;
    QMailRetrievalAction _retrieval;
    QMailServiceAction *_active;
    QMessageStore::ErrorCode _error;

    QList<QMessageId> _matchingIds;
    QList<QMailMessageId> _candidateIds;
    QMessageFilter _lastFilter;
    QMessageOrdering _lastOrdering;
    QString _match;
    int _limit;
    int _offset;

    QMailMessageIdList _transmitIds;
    
signals:
    void stateChanged(QMessageServiceAction::State);
    void progressChanged(uint, uint);
    void messagesFound(const QMessageIdList&);

protected slots:
    void transmitActivityChanged(QMailServiceAction::Activity a);
    void retrievalActivityChanged(QMailServiceAction::Activity a);
    void statusChanged(const QMailServiceAction::Status &s);
    void completed();
    void reportMatchingIds();
    void findMatchingIds();
    void testNextMessage();

private:
    bool messageMatch(const QMailMessageId &messageid);
};

QMessageServiceActionPrivate::QMessageServiceActionPrivate()
    : QObject(),
      _active(0),
      _error(QMessageStore::NoError),
      _limit(0),
      _offset(0)
{
    connect(&_transmit, SIGNAL(activityChanged(QMailServiceAction::Activity)), this, SLOT(transmitActivityChanged(QMailServiceAction::Activity)));
    connect(&_transmit, SIGNAL(statusChanged(QMailServiceAction::Status)), this, SLOT(statusChanged(QMailServiceAction::Status)));

    connect(&_retrieval, SIGNAL(activityChanged(QMailServiceAction::Activity)), this, SLOT(retrievalActivityChanged(QMailServiceAction::Activity)));
    connect(&_retrieval, SIGNAL(statusChanged(QMailServiceAction::Status)), this, SLOT(statusChanged(QMailServiceAction::Status)));
}

void QMessageServiceActionPrivate::transmitActivityChanged(QMailServiceAction::Activity a)
{
    if (a == QMailServiceAction::Successful) {
        if (!_transmitIds.isEmpty()) {
            // If these messages were transmitted, we need to move them to Sent folder
            QMailMessageKey filter(QMailMessageKey::id(_transmitIds));
            foreach (const QMailMessageId &id, QMailStore::instance()->queryMessages(filter & QMailMessageKey::status(QMailMessage::Sent))) {
                QMessage message(convert(id));

                QMessagePrivate::setStandardFolder(message,QMessage::SentFolder);
                if (!QMessageStore::instance()->updateMessage(&message)) {
                    qWarning() << "Unable to mark message as sent!";
                }
            }

            _transmitIds.clear();
        }
    } else if (a == QMailServiceAction::Failed) {
        _transmitIds.clear();
    }

    emit stateChanged(convert(a));
}

void QMessageServiceActionPrivate::statusChanged(const QMailServiceAction::Status &s)
{
    if (s.errorCode != QMailServiceAction::Status::ErrNoError) {
        qWarning() << QString("Service error %1: \"%2\"").arg(s.errorCode).arg(s.text);

        if (s.errorCode == QMailServiceAction::Status::ErrNotImplemented) {
            _error = QMessageStore::NotYetImplemented;
        } else if ((s.errorCode == QMailServiceAction::Status::ErrNonexistentMessage) ||
                   (s.errorCode == QMailServiceAction::Status::ErrEnqueueFailed) ||
                   (s.errorCode == QMailServiceAction::Status::ErrInvalidAddress) ||
                   (s.errorCode == QMailServiceAction::Status::ErrInvalidData)) {
            _error = QMessageStore::ConstraintFailure;
        } else {
            _error = QMessageStore::FrameworkFault;
        }
    }
}

void QMessageServiceActionPrivate::retrievalActivityChanged(QMailServiceAction::Activity a)
{
    emit stateChanged(convert(a));
}

void QMessageServiceActionPrivate::completed()
{
    emit stateChanged(convert(QMailServiceAction::Successful));
}

bool QMessageServiceActionPrivate::messageMatch(const QMailMessageId &messageId)
{
    if (_match.isEmpty()) {
        return true;
    }

    const QMailMessage candidate(messageId);
    if (candidate.id().isValid()) {
        // Search only messages or message parts that are of type 'text/*'
        if (candidate.hasBody()) {
            if (candidate.contentType().type().toLower() == "text") {
                if (candidate.body().data().contains(_match, Qt::CaseInsensitive))
                    return true;
            }
        } else if (candidate.multipartType() != QMailMessage::MultipartNone) {
            // Search all 'text/*' parts within this message
            TextPartSearcher searcher(_match);
            if (candidate.foreachPart<TextPartSearcher&>(searcher) == false) {
                // We found a matching part in the message
                return true;
            }
        }
    }

    return false;
}

void QMessageServiceActionPrivate::reportMatchingIds()
{
    emit messagesFound(convert(_candidateIds));
    completed();
}

void QMessageServiceActionPrivate::findMatchingIds()
{
    int required = ((_offset + _limit) - _matchingIds.count());

    // Are any of the existing IDs part of the result set?
    if (required < _limit) {
        emit messagesFound(_matchingIds.mid(_offset, _limit));
    }

    if (required > 0) {
        QTimer::singleShot(0, this, SLOT(testNextMessage()));
    } else {
        completed();
    }
}

void QMessageServiceActionPrivate::testNextMessage()
{
    int required = ((_offset + _limit) - _matchingIds.count());
    if (required > 0) {
        QMailMessageId messageId(_candidateIds.takeFirst());
        if (messageMatch(messageId)) {
            _matchingIds.append(convert(messageId));
            --required;

            if (required < _limit) {
                // This is within the result set
                emit messagesFound(QMessageIdList() << _matchingIds.last());
            }
        }

        if ((required > 0) && !_candidateIds.isEmpty()) {
            QTimer::singleShot(0, this, SLOT(testNextMessage()));
            return;
        }
    }

    completed();
}

QMessageServiceAction::QMessageServiceAction(QObject *parent)
    : QObject(parent),
      d_ptr(new QMessageServiceActionPrivate)
{
    connect(d_ptr, SIGNAL(stateChanged(QMessageServiceAction::State)), 
            this, SIGNAL(stateChanged(QMessageServiceAction::State)));
    connect(d_ptr, SIGNAL(messagesFound(QMessageIdList)), 
            this, SIGNAL(messagesFound(QMessageIdList)));
    connect(d_ptr, SIGNAL(progressChanged(uint, uint)), 
            this, SIGNAL(progressChanged(uint, uint)));
}

QMessageServiceAction::~QMessageServiceAction()
{
    delete d_ptr;
}

bool QMessageServiceAction::queryMessages(const QMessageFilter &filter, const QMessageOrdering &ordering, uint limit, uint offset) const
{
    if (d_ptr->_active && ((d_ptr->_active->activity() == QMailServiceAction::Pending) || (d_ptr->_active->activity() == QMailServiceAction::Pending))) {
        qWarning() << "Action is currently busy";
        return false;
    }
    d_ptr->_active = 0;
    
    d_ptr->_candidateIds = QMailStore::instance()->queryMessages(convert(filter), convert(ordering), limit, offset);
    d_ptr->_error = convert(QMailStore::instance()->lastError());

    if (d_ptr->_error == QMessageStore::NoError) {
        d_ptr->_lastFilter = QMessageFilter();
        d_ptr->_lastOrdering = QMessageOrdering();
        d_ptr->_match = QString();
        d_ptr->_limit = static_cast<int>(limit);
        d_ptr->_offset = 0;
        d_ptr->_matchingIds.clear();
        QTimer::singleShot(0, d_ptr, SLOT(reportMatchingIds()));
        return true;
    }

    return false;
}

bool QMessageServiceAction::queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::Options options, const QMessageOrdering &ordering, uint limit, uint offset) const
{
    //TODO: Support options
    Q_UNUSED(options)

    if (body.isEmpty()) {
        return queryMessages(filter, ordering, limit, offset);
    }

    if (d_ptr->_active && ((d_ptr->_active->activity() == QMailServiceAction::Pending) || (d_ptr->_active->activity() == QMailServiceAction::Pending))) {
        qWarning() << "Action is currently busy";
        return false;
    }
    d_ptr->_active = 0;
    
    if ((filter == d_ptr->_lastFilter) && (ordering == d_ptr->_lastOrdering) && (body == d_ptr->_match)) {
        // This is a continuation of the last search
        d_ptr->_limit = static_cast<int>(limit);
        d_ptr->_offset = static_cast<int>(offset);
        QTimer::singleShot(0, d_ptr, SLOT(findMatchingIds()));
        return true;
    }

    // Find all messages to perform the body search on
    d_ptr->_candidateIds = QMailStore::instance()->queryMessages(convert(filter), convert(ordering));
    d_ptr->_error = convert(QMailStore::instance()->lastError());

    if (d_ptr->_error == QMessageStore::NoError) {
        d_ptr->_lastFilter = filter;
        d_ptr->_lastOrdering = ordering;
        d_ptr->_match = body;
        d_ptr->_limit = static_cast<int>(limit);
        d_ptr->_offset = static_cast<int>(offset);
        d_ptr->_matchingIds.clear();
        QTimer::singleShot(0, d_ptr, SLOT(findMatchingIds()));
        return true;
    }

    return false;
}

bool QMessageServiceAction::countMessages(const QMessageFilter &filter) const
{
    // TODO: Implement this
    Q_UNUSED(filter);
    return false;
}

bool QMessageServiceAction::send(QMessage &message)
{
    if (d_ptr->_active && ((d_ptr->_active->activity() == QMailServiceAction::Pending) || (d_ptr->_active->activity() == QMailServiceAction::Pending))) {
        qWarning() << "Action is currently busy";
        return false;
    }
    d_ptr->_active = 0;

    if (!message.parentAccountId().isValid()) {
        // Attach to the default account
        message.setParentAccountId(QMessageAccount::defaultAccount(message.type()));
        if (!message.parentAccountId().isValid()) {
            d_ptr->_error = QMessageStore::InvalidId;
            qWarning() << "Invalid message account ID";
            return false;
        }
    }

    // Ensure the message contains a timestamp
    if (!message.date().isValid()) {
        message.setDate(QDateTime::currentDateTime());
    }

    QMessageFolderId existingFolderId(message.parentFolderId());

    // Move the message to the Outbox folder
    QMessagePrivate::setStandardFolder(message,QMessage::OutboxFolder);

    QMailMessage *msg(convert(&message));

    // Ensure that the from address is added
    if (msg->from().isNull()) {
        QMailAccount account(msg->parentAccountId());
        msg->setFrom(account.fromAddress());
    }

    // Mark this message as outgoing
    msg->setStatus(QMailMessage::Outbox, true);

    if (msg->id().isValid()) {
        // Update the message
        if (!QMailStore::instance()->updateMessage(msg)) {
            d_ptr->_error = QMessageStore::FrameworkFault;
            qWarning() << "Unable to mark message as outgoing";
            return false;
        }
    } else {
        // Add this message to the store
        if (!QMailStore::instance()->addMessage(msg)) {
            d_ptr->_error = QMessageStore::FrameworkFault;
            qWarning() << "Unable to store message for transmission";
            return false;
        }
    }

    d_ptr->_transmitIds = QMailStore::instance()->queryMessages(QMailMessageKey::status(QMailMessage::Outgoing) & QMailMessageKey::parentAccountId(msg->parentAccountId()));

    d_ptr->_error = QMessageStore::NoError;
    d_ptr->_active = &d_ptr->_transmit;
    d_ptr->_transmit.transmitMessages(msg->parentAccountId());
    return true;
}

bool QMessageServiceAction::compose(const QMessage &message)
{
    if (d_ptr->_active && ((d_ptr->_active->activity() == QMailServiceAction::Pending) || (d_ptr->_active->activity() == QMailServiceAction::Pending))) {
        qWarning() << "Action is currently busy";
        return false;
    }
    d_ptr->_active = 0;

    // TODO: To be implemented by integrator
    d_ptr->_error = QMessageStore::NotYetImplemented;
    qWarning() << "QMessageServiceAction::compose not yet implemented";
    return false;

    Q_UNUSED(message)
}

bool QMessageServiceAction::retrieveHeader(const QMessageId& id)
{
    if (d_ptr->_active && ((d_ptr->_active->activity() == QMailServiceAction::Pending) || (d_ptr->_active->activity() == QMailServiceAction::Pending))) {
        qWarning() << "Action is currently busy";
        return false;
    }
    d_ptr->_active = 0;

    QMailMessageId messageId(convert(id));
    if (!messageId.isValid()) {
        d_ptr->_error = QMessageStore::InvalidId;
        qWarning() << "Invalid message ID";
        return false;
    }

    // Operation is not relevant to QMF - meta data retrieval always includes header information
    d_ptr->_error = QMessageStore::NoError;
    d_ptr->_active = 0;
    QTimer::singleShot(0, d_ptr, SLOT(completed()));
    return true;
}

bool QMessageServiceAction::retrieveBody(const QMessageId& id)
{
    if (d_ptr->_active && ((d_ptr->_active->activity() == QMailServiceAction::Pending) || (d_ptr->_active->activity() == QMailServiceAction::Pending))) {
        qWarning() << "Action is currently busy";
        return false;
    }
    d_ptr->_active = 0;

    QMailMessageId messageId(convert(id));
    if (!messageId.isValid()) {
        d_ptr->_error = QMessageStore::InvalidId;
        qWarning() << "Invalid message ID";
        return false;
    }

    d_ptr->_error = QMessageStore::NoError;
    d_ptr->_active = &d_ptr->_retrieval;
    d_ptr->_retrieval.retrieveMessages(QMailMessageIdList() << messageId, QMailRetrievalAction::Content);
    return true;
}

bool QMessageServiceAction::retrieve(const QMessageContentContainerId& id)
{
    if (d_ptr->_active && ((d_ptr->_active->activity() == QMailServiceAction::Pending) || (d_ptr->_active->activity() == QMailServiceAction::Pending))) {
        qWarning() << "Action is currently busy";
        return false;
    }
    d_ptr->_active = 0;

    QMailMessagePart::Location location(convert(id));
    if (!location.isValid()) {
        d_ptr->_error = QMessageStore::InvalidId;
        qWarning() << "Invalid message part location";
        return false;
    }
    
    d_ptr->_error = QMessageStore::NoError;
    d_ptr->_active = &d_ptr->_retrieval;
    d_ptr->_retrieval.retrieveMessagePart(location);
    return true;
}

bool QMessageServiceAction::show(const QMessageId& id)
{
    if (d_ptr->_active && ((d_ptr->_active->activity() == QMailServiceAction::Pending) || (d_ptr->_active->activity() == QMailServiceAction::Pending))) {
        qWarning() << "Action is currently busy";
        return false;
    }
    d_ptr->_active = 0;

    // TODO: To be implemented by integrator
    d_ptr->_error = QMessageStore::NotYetImplemented;
    qWarning() << "QMessageServiceAction::show not yet implemented";
    return false;

    Q_UNUSED(id)
}

bool QMessageServiceAction::exportUpdates(const QMessageAccountId &id)
{
    if (d_ptr->_active && ((d_ptr->_active->activity() == QMailServiceAction::Pending) || (d_ptr->_active->activity() == QMailServiceAction::Pending))) {
        qWarning() << "Action is currently busy";
        return false;
    }
    d_ptr->_active = 0;

    QMailAccountId accountId(convert(id));
    if (!accountId.isValid()) {
        d_ptr->_error = QMessageStore::InvalidId;
        qWarning() << "Account ID is not valid";
        return false;
    }

    d_ptr->_error = QMessageStore::NoError;
    d_ptr->_active = &d_ptr->_retrieval;
    d_ptr->_retrieval.exportUpdates(accountId);
    return true;
}

QMessageServiceAction::State QMessageServiceAction::state() const
{
    if (d_ptr->_active) {
        return convert(d_ptr->_active->activity());
    }

    return Successful;
}

void QMessageServiceAction::cancelOperation()
{
    if (d_ptr->_active) {
        d_ptr->_active->cancelOperation();
    }
}

QMessageStore::ErrorCode QMessageServiceAction::lastError() const
{
    return d_ptr->_error;
}

#include "qmessageserviceaction_qmf.moc"

