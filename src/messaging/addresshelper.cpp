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

#include "addresshelper_p.h"
#include <qpair.h>

QTM_BEGIN_NAMESPACE
static QPair<int, int> findDelimiters(const QString& text)
{
    int first = -1;
    int second = -1;

    bool quoted = false;
    bool escaped = false;

    const QChar* const begin = text.constData();
    const QChar* const end = begin + text.length();
    for (const QChar* it = begin; it != end; ++it ) {
        if ( !escaped && ( *it == '\\' ) ) {
            escaped = true;
            continue;
        }

        if ( !quoted && *it == '"' && !escaped ) {
            quoted = true;
        }
        else if ( quoted && *it == '"' && !escaped ) {
            quoted = false;
        }

        if ( !quoted ) {
            if ( first == -1 && *it == '<' ) {
                first = (it - begin);
            }
            else if ( second == -1 && *it == '>' ) {
                second = (it - begin);
                break;
            }
        }

        escaped = false;
    }

    return qMakePair(first, second);
}

void qParseMailbox(QString& input, QString& name, QString& address, QString& suffix, bool &startDelimeterFound, bool &endDelimeterFound)
{
    // See if there is a trailing suffix
    int pos = input.indexOf("/TYPE=");
    if (pos != -1)
    {
        suffix = input.mid(pos + 6);
        input = input.left(pos);
    }

    // Separate the email address from the name
    QPair<int, int> delimiters = findDelimiters(input);

    if (delimiters.first == -1 && delimiters.second == -1)
    {
        name = address = input.trimmed();
    }
    else
    {
        if (delimiters.first == -1)
        {
            // Unmatched '>'
            address = input.left( delimiters.second );
            if ( name.isEmpty() )
                name = address;
        }
        else
        {
            name = input.left( delimiters.first );

            if (delimiters.second == -1)
                address = input.right(input.length() - delimiters.first - 1);
            else
                address = input.mid(delimiters.first + 1, (delimiters.second - delimiters.first - 1)).trimmed();
        }
        name = name.trimmed();
    }

    if (delimiters.first == -1)
        startDelimeterFound = false;
    else
        startDelimeterFound = true;

    if (delimiters.second == -1)
        endDelimeterFound = false;
    else
        endDelimeterFound = true;
}

QTM_END_NAMESPACE
