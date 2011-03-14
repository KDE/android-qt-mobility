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

#ifndef QVERSITCONTACTHANDLER_H
#define QVERSITCONTACTHANDLER_H

#include "qversitcontactimporter.h"
#include "qversitcontactexporter.h"

QTM_BEGIN_NAMESPACE

// qdoc seems to not find QVersitContactHandler if it is declared first.. ugh
class QVersitContactHandler;

class Q_VERSIT_EXPORT QVersitContactHandlerFactory
{
public:
    virtual ~QVersitContactHandlerFactory() {}
    virtual QSet<QString> profiles() const { return QSet<QString>(); }
    virtual QString name() const = 0;
    virtual int index() const { return 0; }
    virtual QVersitContactHandler* createHandler() const = 0;

#ifdef Q_QDOC
    static const QLatin1Constant ProfileSync;
    static const QLatin1Constant ProfileBackup;
#else
    Q_DECLARE_LATIN1_CONSTANT(ProfileSync, "Sync");
    Q_DECLARE_LATIN1_CONSTANT(ProfileBackup, "Backup");
    // This is not enabled for 1.1.x because it's binary incompatible with 1.1.0
    // TODO: enable this for 1.2
    //Q_DECLARE_LATIN1_CONSTANT(ProfilePreserve, "Preserve");
#endif
};

class Q_VERSIT_EXPORT QVersitContactHandler : public QVersitContactImporterPropertyHandlerV2,
                                              public QVersitContactExporterDetailHandlerV2
{
public:
    virtual ~QVersitContactHandler() {}
};

QTM_END_NAMESPACE

#define QT_VERSIT_CONTACT_HANDLER_INTERFACE "com.nokia.qt.mobility.versit.contacthandlerfactory/1.0"
Q_DECLARE_INTERFACE(QtMobility::QVersitContactHandlerFactory, QT_VERSIT_CONTACT_HANDLER_INTERFACE);

#endif
