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
#ifdef QMESSAGING_OPTIONAL_FOLDER
#include "qmessagefolderid.h"
#include "qmfhelpers_p.h"

#include <qmailid.h>

class QMessageFolderIdPrivate
{
public:
    QMailFolderId _id;
};

QMessageFolderId::QMessageFolderId()
    : d_ptr(0)
{
}

QMessageFolderId::QMessageFolderId(const QMessageFolderId& other)
    : d_ptr(0)
{
    this->operator=(other);
}

QMessageFolderId::QMessageFolderId(const QString& id)
    : d_ptr(0)
{
    QMailFolderId fid(id.toULongLong());
    if (fid.isValid()) {
        d_ptr = new QMessageFolderIdPrivate;
        d_ptr->_id = fid;
    }
}

QMessageFolderId::~QMessageFolderId()
{
    delete d_ptr;
}

bool QMessageFolderId::operator==(const QMessageFolderId& other) const
{
    if (isValid()) {
        return (other.isValid() ? (d_ptr->_id == other.d_ptr->_id) : false);
    } else {
        return !other.isValid();
    }
}

QMessageFolderId& QMessageFolderId::operator=(const QMessageFolderId& other)
{
    if (&other != this) {
        if (other.isValid()) {
            if (!d_ptr) {
                d_ptr = new QMessageFolderIdPrivate;
            }
            d_ptr->_id = other.d_ptr->_id;
        } else {
            delete d_ptr;
        }
    }

    return *this;
}

QString QMessageFolderId::toString() const
{
    return (isValid() ? QString::number(d_ptr->_id.toULongLong()) : QString());
}

bool QMessageFolderId::isValid() const
{
    return (d_ptr && d_ptr->_id.isValid());
}

uint qHash(const QMessageFolderId &id)
{
    return qHash(QmfHelpers::convert(id));
}

#endif
