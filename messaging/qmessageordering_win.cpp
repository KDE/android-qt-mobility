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
#include "qmessageordering.h"
#include "qmessageordering_p.h"
#include "qmessage_p.h"

QMessageOrderingPrivate::QMessageOrderingPrivate(QMessageOrdering *ordering)
    :q_ptr(ordering),
     _valid(true)
{
}

#define COMPARE(x,y) \
if ((x) < (y)) { \
    return true; \
} else if ((y) < (x)) { \
    return false; \
} else { \
    continue; \
}

bool QMessageOrderingPrivate::lessThan(const QMessageOrdering &ordering, const QMessage &l, const QMessage &r)
{
    QMessageOrderingPrivate *d(ordering.d_ptr);

    QList<QPair<Field, Qt::SortOrder> >::iterator it(d->_fieldOrderList.begin());
    while (it != d->_fieldOrderList.end()) {
        Field field((*it).first);
        Qt::SortOrder order((*it).second);
        ++it;

        const QMessage *left;
        const QMessage *right;
        if (order == Qt::AscendingOrder) {
            left = &l;
            right = &r; 
        } else {
            left = &r;
            right = &l; 
        }

        //TODO: Type and Priority will require multiple filter passes in QMessageStore
        //TODO: Recipients won't be supported
        //TODO: Status may require multiple passes, or may not be implementable with MAPI

        switch (field)
        {
        case Type: COMPARE(left->type(), right->type())
        case Sender: COMPARE(QMessagePrivate::senderName(*left), QMessagePrivate::senderName(*right));
        case Recipients: {
            QString leftStr;
            QString rightStr;
            foreach (QMessageAddress a, left->to() + left->cc() + left->bcc()) {
                leftStr.append(a.recipient() + ";");
            }
            foreach (QMessageAddress a, right->to() + right->cc() + right->bcc()) {
                rightStr.append(a.recipient() + ";");
            }
            COMPARE(leftStr, rightStr)
        }
        case Subject: COMPARE(left->subject(), right->subject())
        case TimeStamp: COMPARE(left->date(), right->date())
        case ReceptionTimeStamp: COMPARE(left->receivedDate(), right->receivedDate())
        case Read: COMPARE(left->status() & QMessage::Read, right->status() & QMessage::Read)
        case HasAttachments: COMPARE(left->status() & QMessage::Read, right->status() & QMessage::Read)
        case Incoming: COMPARE(left->status() & QMessage::Read, right->status() & QMessage::Read)
        case Removed: COMPARE(left->status() & QMessage::Read, right->status() & QMessage::Read)
        case Priority: COMPARE(left->priority(), right->priority())
        case Size: COMPARE(left->size(), right->size())
        }
    }
    return false; // equality
}

const int maxSortOrders(16);  // 16 levels of sort ordering should be more than sufficient
struct MapiSortOrderSet
{
    ULONG cSorts;
    ULONG cCategories;
    ULONG cExpanded;
    SSortOrder aSort[maxSortOrders];
};

void QMessageOrderingPrivate::sortTable(QMessageStore::ErrorCode *lastError, const QMessageOrdering &ordering, LPMAPITABLE messagesTable)
{
    MapiSortOrderSet multiSort;
    QMessageOrderingPrivate *d(ordering.d_ptr);
    QList<QPair<Field, Qt::SortOrder> > fieldOrderList(d->_fieldOrderList);
    multiSort.cSorts = qMin<int>(maxSortOrders, fieldOrderList.count());
    multiSort.cCategories = 0;
    multiSort.cExpanded = 0;

    if (!d || fieldOrderList.isEmpty())
        return;
    
    for (int i = 0; i < qMin<int>(maxSortOrders, fieldOrderList.count()); ++i) {
        ULONG order(TABLE_SORT_ASCEND);
        if (fieldOrderList[i].second == Qt::DescendingOrder)
            order = TABLE_SORT_DESCEND;
        
        ULONG propTag(PR_SUBJECT);
        switch (fieldOrderList[i].first)
        {
        case Subject:
            propTag = PR_SUBJECT;
            break;
        case TimeStamp:
            propTag = PR_CLIENT_SUBMIT_TIME;
            break;
        case ReceptionTimeStamp:
            propTag = PR_MESSAGE_DELIVERY_TIME;
            break;
        case Sender:
            propTag = PR_SENDER_NAME; // MAPI is limited to sorting by sender name only, sender name + sender address does not appear to be supported
            break;
        case Size:
#ifdef _WIN32_WCE
            propTag = PR_CONTENT_LENGTH;
#else
            propTag = PR_MESSAGE_SIZE;
#endif
            break;
        default:
            qWarning("Unhandled sort criteria");
            propTag = PR_SUBJECT; // TODO handle all cases
        }
        multiSort.aSort[i].ulPropTag = propTag;
        multiSort.aSort[i].ulOrder = order;
    }

    //Note: WinCE does not support multiple sort levels and should return an error if more than 1 level is used
    //TODO: Update doc to reflect this
    if (messagesTable->SortTable(reinterpret_cast<SSortOrderSet*>(&multiSort), 0) != S_OK) {
        *lastError = QMessageStore::NotYetImplemented;
        return;
    }
    return;
}

QMessageOrdering QMessageOrderingPrivate::from(QMessageOrderingPrivate::Field field, Qt::SortOrder order)
{
    QMessageOrdering result;
    QPair<QMessageOrderingPrivate::Field, Qt::SortOrder> fieldOrder(field, order);
    result.d_ptr->_fieldOrderList.append(fieldOrder);
    return result;
}


QMessageOrdering::QMessageOrdering()
    :d_ptr(new QMessageOrderingPrivate(this))
{
}

QMessageOrdering::~QMessageOrdering()
{
    delete d_ptr;
    d_ptr = 0;
}

QMessageOrdering::QMessageOrdering(const QMessageOrdering &other)
    :d_ptr(new QMessageOrderingPrivate(this))
{
    this->operator=(other);
}

QMessageOrdering& QMessageOrdering::operator=(const QMessageOrdering& other)
{
    if (&other != this) {
        d_ptr->_fieldOrderList = other.d_ptr->_fieldOrderList;
        d_ptr->_valid = other.d_ptr->_valid;
    }

    return *this;
}

bool QMessageOrdering::isEmpty() const
{
    return d_ptr->_fieldOrderList.isEmpty();
}

bool QMessageOrdering::isSupported() const
{
    return d_ptr->_valid;
}

QMessageOrdering QMessageOrdering::operator+(const QMessageOrdering& other) const
{
    QMessageOrdering sum;
    sum.d_ptr->_fieldOrderList = d_ptr->_fieldOrderList + other.d_ptr->_fieldOrderList;
#ifdef _WIN32_WCE
    // Multiple sort orders are not supported on WinCE
    sum.d_ptr->_valid = false;
#endif
    return sum;
}

QMessageOrdering& QMessageOrdering::operator+=(const QMessageOrdering& other)
{
    if (&other == this)
        return *this;
    d_ptr->_fieldOrderList += other.d_ptr->_fieldOrderList;
#ifdef _WIN32_WCE
    // Multiple sort orders are not supported on WinCE
    d__ptr->_valid = false;
#endif;
    return *this;
}

bool QMessageOrdering::operator==(const QMessageOrdering& other) const
{
    return (d_ptr->_fieldOrderList == other.d_ptr->_fieldOrderList);
}

QMessageOrdering QMessageOrdering::byType(Qt::SortOrder order)
{
    QMessageOrdering result(QMessageOrderingPrivate::from(QMessageOrderingPrivate::Type, order)); // stub
    result.d_ptr->_valid = false; // Not yet implemented
    return result;
}

QMessageOrdering QMessageOrdering::bySender(Qt::SortOrder order)
{
    // Partially implemented, can sort by sender name only not sender email address
    // TODO: Update doc to reflect this
    return QMessageOrderingPrivate::from(QMessageOrderingPrivate::Sender, order);
}

QMessageOrdering QMessageOrdering::byRecipients(Qt::SortOrder order)
{
    QMessageOrdering result(QMessageOrderingPrivate::from(QMessageOrderingPrivate::Recipients, order));
    result.d_ptr->_valid = false; // Not supported
    return result;
}

QMessageOrdering QMessageOrdering::bySubject(Qt::SortOrder order)
{
    QMessageOrdering result(QMessageOrderingPrivate::from(QMessageOrderingPrivate::Subject, order));
    return result;
}

QMessageOrdering QMessageOrdering::byTimeStamp(Qt::SortOrder order)
{
    QMessageOrdering result(QMessageOrderingPrivate::from(QMessageOrderingPrivate::TimeStamp, order));
#ifdef _WIN32_WCE
    result.d__ptr->_valid = false; // Not supported on WinCE
#endif
    return result;
}

QMessageOrdering QMessageOrdering::byReceptionTimeStamp(Qt::SortOrder order)
{
    return QMessageOrderingPrivate::from(QMessageOrderingPrivate::ReceptionTimeStamp, order);
}

QMessageOrdering QMessageOrdering::byStatus(QMessage::Status flag, Qt::SortOrder order)
{
    QMessageOrdering result;
    switch (flag) {
    case QMessage::Read:
        result = QMessageOrderingPrivate::from(QMessageOrderingPrivate::Read, order);
    case QMessage::HasAttachments:
        result = QMessageOrderingPrivate::from(QMessageOrderingPrivate::HasAttachments, order);
    case QMessage::Incoming:
        result = QMessageOrderingPrivate::from(QMessageOrderingPrivate::Incoming, order);
    case QMessage::Removed:
        result = QMessageOrderingPrivate::from(QMessageOrderingPrivate::Removed, order);
    }
    result.d_ptr->_valid = false; // Not yet implemented
    return result;
}

QMessageOrdering QMessageOrdering::byPriority(Qt::SortOrder order)
{
    QMessageOrdering result(QMessageOrderingPrivate::from(QMessageOrderingPrivate::Priority, order));
    result.d_ptr->_valid = false; // Not yet implemented
    return result;
}

QMessageOrdering QMessageOrdering::bySize(Qt::SortOrder order)
{
    return QMessageOrderingPrivate::from(QMessageOrderingPrivate::Size, order);
}
