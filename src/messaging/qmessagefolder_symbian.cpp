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
#include "qmessagefolder.h"
#include "qmessagefolder_p.h"
#include "qmessagemanager.h"


QTM_BEGIN_NAMESPACE

QMessageFolder QMessageFolderPrivate::from(const QMessageFolderId &id, const QMessageAccountId &accountId, const QMessageFolderId &parentId, const QString &name, const QString &path)
{
    QMessageFolder result;
    result.d_ptr->_id = id;
    result.d_ptr->_parentAccountId = accountId;
    result.d_ptr->_parentFolderId = parentId;
    result.d_ptr->_name = name;
    result.d_ptr->_path = path;
    return result;
}

QMessageFolder::QMessageFolder()
    :d_ptr(new QMessageFolderPrivate(this))
{
}

QMessageFolder::QMessageFolder(const QMessageFolderId &id)
    :d_ptr(new QMessageFolderPrivate(this))
{
    *this = QMessageManager().folder(id);
}

QMessageFolder::QMessageFolder(const QMessageFolder &other)
    :d_ptr(new QMessageFolderPrivate(this))
{
    this->operator=(other);
}

QMessageFolder& QMessageFolder::operator=(const QMessageFolder& other)
{
    if (&other != this)
        *d_ptr = *other.d_ptr;

    return *this;
}

QMessageFolder::~QMessageFolder()
{
    delete d_ptr;
    d_ptr = 0;
}

QMessageFolderId QMessageFolder::id() const
{
    return d_ptr->_id;
}

QMessageAccountId QMessageFolder::parentAccountId() const
{
    return d_ptr->_parentAccountId;
}

QMessageFolderId QMessageFolder::parentFolderId() const
{
    return d_ptr->_parentFolderId;
}

QString QMessageFolder::name() const
{
    return d_ptr->_name;
}

QString QMessageFolder::path() const
{
    return d_ptr->_path;
}

QTM_END_NAMESPACE
