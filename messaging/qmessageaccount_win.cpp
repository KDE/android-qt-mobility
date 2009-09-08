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
#include "qmessageaccount.h"
#include "qmessageaccount_p.h"
#include "qmessagestore.h"
#include <QDebug>

QMessageAccount QMessageAccountPrivate::from(const QMessageAccountId &id, const QString &name, const QMessageAddress &address, const QMessage::TypeFlags &types)
{
    QMessageAccount result;
    result.d_ptr->_id = id;
    result.d_ptr->_name = name;
    result.d_ptr->_address = address;
    result.d_ptr->_types = types;
    return result;
}

QMessageAccount::QMessageAccount()
    :d_ptr(new QMessageAccountPrivate(this))
{
}

QMessageAccount::QMessageAccount(const QMessageAccountId &id)
    :d_ptr(new QMessageAccountPrivate(this))
{
    *this = QMessageStore::instance()->account(id);
}

QMessageAccount::QMessageAccount(const QMessageAccount &other)
    :d_ptr(new QMessageAccountPrivate(this))
{
    this->operator=(other);
}

QMessageAccount& QMessageAccount::operator=(const QMessageAccount& other)
{
    if (&other != this)
        *d_ptr = *other.d_ptr;

    return *this;
}

QMessageAccount::~QMessageAccount()
{
    delete d_ptr;
    d_ptr = 0;
}

QMessageAccountId QMessageAccount::id() const
{
    return d_ptr->_id;
}

QString QMessageAccount::name() const
{
    return d_ptr->_name;
}

QMessage::TypeFlags QMessageAccount::messageTypes() const
{
    return d_ptr->_types;
}

QMessageAccountId QMessageAccount::defaultAccount(QMessage::Type type)
{
    //TODO Implement this correctly. For MAPI desktop there is only one account so just return it
    Q_UNUSED(type)
    QMessageAccountIdList accounts(QMessageStore::instance()->queryAccounts());
    if (!accounts.isEmpty())
        return accounts.first();

    return QMessageAccountId(); // stub
}
