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

#include "messagingutil_p.h"
#include "qmessageid.h"
#include "qmessageaccountid.h"
#include "qmessagefolderid.h"
#include <qmessageglobal.h>

#include <QDebug>



QTM_BEGIN_NAMESPACE

namespace SymbianHelpers {

    EngineType idType(const QString& id)
    {
        if (id.startsWith(mtmPrefix)) {
            return EngineTypeMTM;
        } else if (id.startsWith(freestylePrefix)) {
            return EngineTypeFreestyle;
        } else {
            return EngineTypeMTM;
        }
    }

    EngineType idType(const QMessageId& id)
    {
        if (id.toString().startsWith(mtmPrefix)) {
            return EngineTypeMTM;
        } else if (id.toString().startsWith(freestylePrefix)) {
            return EngineTypeFreestyle;
        } else {
            return EngineTypeMTM;
        }
    }
    
    EngineType idType(const QMessageAccountId& id)
    {
        if (id.toString().startsWith(mtmPrefix)) {
            return EngineTypeMTM;
        } else if (id.toString().startsWith(freestylePrefix)) {
            return EngineTypeFreestyle;
        } else {
            return EngineTypeMTM;
        }
    }
    
    EngineType idType(const QMessageFolderId& id)
    {
        if (id.toString().startsWith(mtmPrefix)) {
            return EngineTypeMTM;
        } else if (id.toString().startsWith(freestylePrefix)) {
            return EngineTypeFreestyle;
        } else {
            return EngineTypeMTM;
        }
    }

    
    QString addIdPrefix(const QString& id, const EngineType& type)
    {
        switch (type) {
        case EngineTypeFreestyle:
            Q_ASSERT(!id.startsWith(freestylePrefix));
            return QString(freestylePrefix) + id;
        case EngineTypeMTM:
            Q_ASSERT(!id.startsWith(mtmPrefix));
            return QString(mtmPrefix) + id;
        default:
            return QString(id);
        }
    }

    QMessageAccountId addIdPrefix(const QMessageAccountId& id, const EngineType& type)
    {
        switch (type) {
        case EngineTypeFreestyle:
            Q_ASSERT(!id.toString().startsWith(freestylePrefix));
            return QMessageAccountId(QString(freestylePrefix) + id.toString());
        case EngineTypeMTM:
            Q_ASSERT(!id.toString().startsWith(mtmPrefix));
            return QMessageAccountId(QString(mtmPrefix) + id.toString());
        default:
            return QMessageAccountId(id);
        }
    }

    QMessageFolderId addIdPrefix(const QMessageFolderId& id, const EngineType& type)
    {
        switch (type) {
        case EngineTypeFreestyle:
            Q_ASSERT(!id.toString().startsWith(freestylePrefix));
            return QMessageFolderId(QString(freestylePrefix) + id.toString());
        case EngineTypeMTM:
            Q_ASSERT(!id.toString().startsWith(mtmPrefix));
            return QMessageFolderId(QString(mtmPrefix) + id.toString());
        default:
            return QMessageFolderId(id);
        }
    }

    QMessageId addIdPrefix(const QMessageId& id, const EngineType& type)
    {
        switch (type) {
        case EngineTypeFreestyle:
            Q_ASSERT(!id.toString().startsWith(freestylePrefix));
            return QMessageId(freestylePrefix + id.toString());
        case EngineTypeMTM:
            Q_ASSERT(!id.toString().startsWith(mtmPrefix));
            return QMessageId(mtmPrefix + id.toString());
        default:
            return QMessageId(id);
        }
    }

    QString stripIdPrefix(const QString& id)
    {
        if (id.startsWith(freestylePrefix))
            return id.right(id.length() - QString(freestylePrefix).length());
        else if (id.startsWith(mtmPrefix))
            return id.right(id.length() - QString(mtmPrefix).length());
        else
            return QString(id);
    }

    QMessageId stripIdPrefix(const QMessageId& id)
    {
        if (id.toString().startsWith(freestylePrefix))
            return QMessageId(id.toString().right(id.toString().length() - QString(freestylePrefix).length()));
        else if (id.toString().startsWith(mtmPrefix))
            return QMessageId(id.toString().right(id.toString().length() - QString(mtmPrefix).length()));
        else
            return QMessageId(id);
    }

    QMessageAccountId stripIdPrefix(const QMessageAccountId& id)
    {
        if (id.toString().startsWith(freestylePrefix))
            return QMessageAccountId(id.toString().right(id.toString().length() - QString(freestylePrefix).length()));
        else if (id.toString().startsWith(mtmPrefix))
            return QMessageAccountId(id.toString().right(id.toString().length() - QString(mtmPrefix).length()));
        else
            return QMessageAccountId(id);
    }

    QMessageFolderId stripIdPrefix(const QMessageFolderId& id)
    {
        if (id.toString().startsWith(freestylePrefix))
            return QMessageFolderId(id.toString().right(id.toString().length() - QString(freestylePrefix).length()));
        else if (id.toString().startsWith(mtmPrefix))
            return QMessageFolderId(id.toString().right(id.toString().length() - QString(mtmPrefix).length()));
        else
            return QMessageFolderId(id);
    }

}

namespace MessagingUtil {

    QString addIdPrefix(const QString& id)
    {
        if(id.startsWith(idPrefix()))
			qWarning() << "ID already prefixed";

        return idPrefix() + id;
    }

    QString stripIdPrefix(const QString& id)
    {
		if(!id.startsWith(idPrefix()))
			qWarning() << "ID not prefixed";

		return id.right(id.length() - idPrefix().length());
    }

    QString idPrefix()
    {
#ifdef Q_OS_SYMBIAN
        static QString prefix(SymbianHelpers::mtmPrefix);
#elif defined(Q_OS_WIN)
        static QString prefix("WIN_");
#else
        static QString prefix("QMF_");
#endif
        return prefix;
    }

}

QTM_END_NAMESPACE
