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
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessageid_p.h"
#include <QByteArray>
#include <QDataStream>
#include <MAPIUtil.h>

QMessageId QMessageIdPrivate::from(const MapiRecordKey &messageKey, const MapiRecordKey &folderKey, const MapiRecordKey &storeKey, const MapiEntryId &entryId)
{
    QMessageId result;
    if (!result.d_ptr)
        result.d_ptr = new QMessageIdPrivate(&result);
    result.d_ptr->_messageRecordKey = messageKey;
    result.d_ptr->_folderRecordKey = folderKey;
    result.d_ptr->_storeRecordKey = storeKey;
    result.d_ptr->_entryId = entryId;
    return result;
}

MapiEntryId QMessageIdPrivate::entryId(const QMessageId &id)
{
    if (id.d_ptr)
        return id.d_ptr->_entryId;
    return MapiEntryId();
}

MapiRecordKey QMessageIdPrivate::messageRecordKey(const QMessageId &id)
{
    if (id.d_ptr)
        return id.d_ptr->_messageRecordKey;
    return MapiRecordKey();
}

MapiRecordKey QMessageIdPrivate::folderRecordKey(const QMessageId &id)
{
    if (id.d_ptr)
        return id.d_ptr->_folderRecordKey;
    return MapiRecordKey();
}

MapiRecordKey QMessageIdPrivate::storeRecordKey(const QMessageId &id)
{
    if (id.d_ptr)
        return id.d_ptr->_storeRecordKey;
    return MapiRecordKey();
}

QMessageId::QMessageId()
    : d_ptr(0)
{
}

QMessageId::QMessageId(const QMessageId& other)
    : d_ptr(0)
{
    this->operator=(other);
}

QMessageId::QMessageId(const QString& id)
    : d_ptr(new QMessageIdPrivate(this))
{
    QDataStream idStream(QByteArray::fromBase64(id.toLatin1()));
    idStream >> d_ptr->_messageRecordKey;
    idStream >> d_ptr->_folderRecordKey;
    idStream >> d_ptr->_storeRecordKey;
    if (!idStream.atEnd())
        idStream >> d_ptr->_entryId;
}

QMessageId::~QMessageId()
{
    delete d_ptr;
}

bool QMessageId::operator==(const QMessageId& other) const
{
    if (isValid()) {
        if (other.isValid()) {
            bool result(true);
            result &= (d_ptr->_messageRecordKey == other.d_ptr->_messageRecordKey);
            result &= (d_ptr->_folderRecordKey == other.d_ptr->_folderRecordKey);
            result &= (d_ptr->_storeRecordKey == other.d_ptr->_storeRecordKey);
            return result;
        }
        return false;
    } else {
        return !other.isValid();
    }
}

QMessageId& QMessageId::operator=(const QMessageId& other)
{
    if (&other != this) {
        if (other.isValid()) {
            if (!d_ptr) {
                d_ptr = new QMessageIdPrivate(this);
            }
            d_ptr->_messageRecordKey = other.d_ptr->_messageRecordKey;
            d_ptr->_folderRecordKey = other.d_ptr->_folderRecordKey;
            d_ptr->_storeRecordKey = other.d_ptr->_storeRecordKey;
            d_ptr->_entryId = other.d_ptr->_entryId;
        } else {
            delete d_ptr;
            d_ptr = 0;
        }
    }

    return *this;
}

QString QMessageId::toString() const
{
    if (!isValid())
        return QString();
    QByteArray encodedId;
    QDataStream encodedIdStream(&encodedId, QIODevice::WriteOnly);
    encodedIdStream << d_ptr->_messageRecordKey;
    encodedIdStream << d_ptr->_folderRecordKey;
    encodedIdStream << d_ptr->_storeRecordKey;
    if (d_ptr->_entryId.count())
        encodedIdStream << d_ptr->_entryId;
    return encodedId.toBase64();
}

bool QMessageId::isValid() const
{
    return (d_ptr && !d_ptr->_messageRecordKey.isEmpty() && !d_ptr->_folderRecordKey.isEmpty() && !d_ptr->_storeRecordKey.isEmpty());
}

uint qHash(const QMessageId &id)
{
    Q_UNUSED(id)
    return 0; // stub
}

