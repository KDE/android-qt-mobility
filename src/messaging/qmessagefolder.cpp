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

/*!
    \class QMessageFolder

    \brief The QMessageFolder class provides an interface for folders containing messages in the 
    messaging store.
    
    \ingroup messaging
 
    QMessageFolder represents a folder of messages, either fully or partially stored in the 
    messaging store. A QMessageFolder object has an optional parent of the same type, allowing 
    folders to be arranged in tree structures. Messages may be associated with folders, allowing 
    for simple classification and access by their parentFolderId property.

    \sa QMessage, QMessageStore::folder()
*/

/*!
    \fn QMessageFolder::QMessageFolder()
    
    Constructor that creates an empty and invalid QMessageFolder. An empty folder is one which 
    has no path, no parent folder and no parent account. An invalid folder does not exist in 
    the database and has an invalid id.
*/

/*!
    \fn QMessageFolder::QMessageFolder(const QMessageFolderId &id)
    
    Constructor that creates a QMessageFolder by loading the data from the messaging store as 
    specified by the QMessageFolderId \a id. If the folder does not exist in the messaging  
    store, then this constructor will create an empty and invalid QMessageFolder.
*/

/*!
    \fn QMessageFolder::QMessageFolder(const QMessageFolder &other)

    Constructs a copy of \a other.
*/

/*!
    \internal
    \fn QMessageFolder::operator=(const QMessageFolder& other)
*/

/*!
    \fn QMessageFolder::~QMessageFolder()
    
    Destroys the QMessageFolder object.
*/

/*!
    \fn QMessageFolder::id() const
    
    Returns the identifier of the QMessageFolder object. A QMessageFolder with an invalid 
    identifier does not yet exist in the messaging store.
*/

/*!
    \fn QMessageFolder::parentAccountId() const
    
    Returns the identifier of the parent account of the folder. If the folder is not linked to 
    an account an invalid identifier is returned.
*/

/*!
    \fn QMessageFolder::parentFolderId() const
    
    Returns the identifier of the parent folder. This folder is a root folder if the parent 
    identifier is invalid.
*/

/*!
    \fn QMessageFolder::name() const
    
    Returns the display name of the folder suitable for viewing in a user interface.  This is
    typically the same as, or a simplification of the path() for the folder.
*/

/*!
    \fn QMessageFolder::path() const
    
    Returns the path of the folder.
*/
