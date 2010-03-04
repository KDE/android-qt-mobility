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

#include "qlandmarkid.h"
#include "qlandmarkid_p.h"
#include <QString>

QTM_USE_NAMESPACE

/*!
    \class QLandmarkId
    \ingroup location

    \brief The QLandmarkId class provides a unique identifier for
    a category.

    It consists of a manager URI which identifies the manager which contains
    the category, and the id for that category.
*/

/*!

    Creates an invalid identifier, id() and managerUri() will return null strings
*/
QLandmarkId::QLandmarkId()
{
}

/*!
    Constructs a copy of \a other.
*/
QLandmarkId::QLandmarkId(const QLandmarkId &other)
    :d(other.d)
{
}

/*!
    Destroys the identifier.
*/
QLandmarkId::~QLandmarkId()
{
}

/*!
    Returns true if the identifier is valid, meaning that both
    a manager URI and id have been set
*/
bool QLandmarkId::isValid() const
{
    return (!d->m_databaseName.isEmpty() && !d->m_id.isEmpty());
}

/*!
    Returns the id of the category identifier.
*/
QString QLandmarkId::id() const
{
    return d->m_id;
}

/*!
    Sets the \a id of the category identifier
*/
void QLandmarkId::setId(const QString &id)
{
    d->m_id = id;
}

/*!
    Returns the URI of the manager which this category identifer
    belongs to.
*/
QString QLandmarkId::managerUri() const
{
    return d->m_databaseName;
}

/*!
    Sets the manager \a uri which this category identifier
    belongs to.
*/
void QLandmarkId::setManagerUri(const QString &uri)
{
    d->m_databaseName = uri;
}

/*!
    Assigns \a other to this category identifier and returns a reference
    to this category identifier.
*/
QLandmarkId &QLandmarkId::operator=(const QLandmarkId &other)
{
    d = other.d;
    return *this;
}

/*!
    Returns true if this category identifer is equal to \a other, otherwise
    returns false.

    Two QLandmarkIds are considered equal if both have the same
    manager URI and id.
*/
bool QLandmarkId::operator==(const QLandmarkId &other) const
{
    return ((d->m_id == other.d->m_id)
            && (d->m_databaseName == other.d->m_databaseName));
}

/*!
    Returns true if this category identifier is not equal to \a other,
    otherwise returns false.

    If either the manager URIs or ids differ, then the
    QLandmarkIds are not considered equal.
*/
bool QLandmarkId::operator!=(const QLandmarkId &other) const
{
    return !this->operator ==(other);
}


