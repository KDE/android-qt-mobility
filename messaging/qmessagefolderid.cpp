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
#ifdef QMESSAGING_OPTIONAL_FOLDER
#include "qmessagefolderid_p.h"

/*!
    \class QMessageFolderId
    \ingroup messaging

    \preliminary
    \brief The QMessageFolderId class provides a unique identifier for a QMessageFolder message 
    folder, within the scope of the messaging store.

    A QMessageFolderId can be constructed from a string, or converted to a string with toString().

    A QMessageFolderId instance can be tested for validity with isValid(), and compared to other 
    instances for equality.
    
    If the message a QMessageFolderId identifies is removed from the messaging store then the 
    identifier will not be reused.
    
    \sa QMessageFolder
*/

/*!
    \fn QMessageFolderId::QMessageFolderId()

    Creates an invalid identifier, toString() will return a null string.
*/

/*!
    \fn QMessageFolderId::QMessageFolderId(const QMessageFolderId& other)

    Constructs a copy of \a other.
*/

/*!
    \fn QMessageFolderId::QMessageFolderId(const QString& id)

    Constructs an identifier from \a id.
    
    \sa toString()
*/

/*!
    \fn QMessageFolderId::~QMessageFolderId()
    
    Destroys the identifier.
*/

/*!
    \fn bool QMessageFolderId::operator==(const QMessageFolderId &other) const

    Returns true if this identifier and the \a other identifier are identical;
    otherwise returns false.
*/

/*! \internal */
bool QMessageFolderId::operator!=(const QMessageFolderId &other) const
{
    return !operator==(other);
}

/*!
    \fn QMessageFolderId& QMessageFolderId::operator=(const QMessageFolderId &other)

    Assigns \a other to this identifier and returns a reference to this identifier.
*/

/*!
    \fn bool QMessageFolderId::toString() const

    Returns the string representation of this identifier.
    
    A null string should be returned if and only if the identifier is invalid.

    String representations of identifiers should not be used to test for equality, instead 
    the equality operator should be used.
*/

/*!
    \fn bool QMessageFolderId::isValid() const

    Returns true if this identifier is valid; otherwise returns false.
*/

/*! \typedef QMessageFolderIdList

    Qt-style synonym for QList<QMessageFolderId>
*/

/*! 
    \fn uint qHash(const QMessageFolderId&);

    \internal
*/
#endif
