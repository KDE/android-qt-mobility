/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef DBERROR_H
#define DBERROR_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qmobilityglobal.h"
#include <QString>

QTM_BEGIN_NAMESPACE

class QM_AUTOTEST_EXPORT DBError
{
    public:
        enum ErrorCode {
            NoError,
            DatabaseNotOpen = -2000,    //A connection with the database has not been opened
                                        //  database needs to be opened before any operations take place
            InvalidDatabaseConnection,  //The database connection does not have a valid driver
            LocationAlreadyRegistered,  //A service location has already been registered.
            IfaceImplAlreadyRegistered, //An interface implementation by a given service is already registered to that service
            NotFound,       
            SqlError,               //An Sql error occurred.
            IfaceIDNotExternal,     //InterfaceID does not refer to an external interface implementation
            CannotCreateDbDir,      //Directory to contain database could not be created(usu a permissions issue)
            CannotOpenServiceDb,     //service database cannot be opened(usually a permissions issue)
            ExternalIfaceIDFound,   //Notification for interfaceDefault() on a user scope database
                                    //  to indicate that a default refers to an interface implementation in the
                                    //  system scope database
            InvalidDescriptorScope, //Notification for setInterfaceDefault() on a system scope database
                                    //  to indicate that a user scope descriptor cannot be used
                                    //  with a system scope database.
            InvalidDatabaseFile,    //database file is corrupted or not a valid database
            NoWritePermissions,     //trying to perform a write operation without sufficient permissions
            UnknownError
        };
        DBError();
        void setError(ErrorCode error, const QString &errorText = QString());
        void setSQLError(const QString &errorText) {
            m_error = SqlError;
            m_text = errorText;
        }
        void setNotFoundError(const QString &errorText) {
            m_error = NotFound;
            m_text = errorText;
        }
        QString text() const { return m_text; }
        ErrorCode code() const { return m_error; }
    private:
        QString m_text;
        ErrorCode m_error;
};
QTM_END_NAMESPACE

#endif  //DBERROR_H
