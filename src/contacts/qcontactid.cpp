/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qcontactid.h"
#include "qcontactid_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QContactId
  \brief The QContactId class provides information that uniquely identifies
  a contact in a particular manager.
 
  It consists of a manager URI which identifies the manager which contains the contact,
  and the local id of the contact in that manager.
 */

/*!
 * Constructs a new contact id
 */
QContactId::QContactId()
        : d(new QContactIdPrivate)
{
}

/*!
 * Cleans up the memory in use by the contact id
 */
QContactId::~QContactId()
{
}

/*! Constructs a new contact id as a copy of \a other */
QContactId::QContactId(const QContactId& other)
        : d(other.d)
{
}

/*! Assigns the contact id to be equal to \a other */
QContactId& QContactId::operator=(const QContactId& other)
{
    d = other.d;
    return *this;
}

/*! Returns true if the contact id has the same manager URI and local id as \a other */
bool QContactId::operator==(const QContactId& other) const
{
    if (d->m_managerUri != other.d->m_managerUri)
        return false;
    if (d->m_localId != other.d->m_localId)
        return false;
    return true;
}

/*! Returns true if either the manager URI or local id of the contact id is different to that of \a other */
bool QContactId::operator!=(const QContactId& other) const
{
    return !(*this == other);
}

/*!
 * Returns the URI of the manager which contains the contact identified by this id
 */
QString QContactId::managerUri() const
{
    return d->m_managerUri;
}

/*!
 * Returns the manager-local id of the contact identified by this contact id
 */
QContactLocalId QContactId::localId() const
{
    return d->m_localId;
}

/*!
 * Sets the URI of the manager which contains the contact identified by this id to \a uri
 */
void QContactId::setManagerUri(const QString& uri)
{
    d->m_managerUri = uri;
}

/*!
 * Sets the manager-local id of the contact identified by this contact id to \a id
 */
void QContactId::setLocalId(const QContactLocalId& id)
{
    d->m_localId = id;
}

QTM_END_NAMESPACE
