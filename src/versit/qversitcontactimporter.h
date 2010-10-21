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

#ifndef QVERSITCONTACTIMPORTER_H
#define QVERSITCONTACTIMPORTER_H

#include "qmobilityglobal.h"
#include "qversitresourcehandler.h"

#include <qcontact.h>

#include <QList>

QTM_BEGIN_NAMESPACE

class QVersitDocument;
class QVersitContactImporterPrivate;
class QVersitProperty;

class Q_VERSIT_EXPORT QVersitContactImporterPropertyHandler
{
public:
    virtual ~QVersitContactImporterPropertyHandler() {}
    virtual bool preProcessProperty(const QVersitDocument& document,
                                    const QVersitProperty& property,
                                    int contactIndex,
                                    QContact* contact) = 0;
    virtual bool postProcessProperty(const QVersitDocument& document,
                                     const QVersitProperty& property,
                                     bool alreadyProcessed,
                                     int contactIndex,
                                     QContact* contact) = 0;
};

class Q_VERSIT_EXPORT QVersitContactImporterPropertyHandlerV2
{
public:
    virtual ~QVersitContactImporterPropertyHandlerV2() {}
    virtual void propertyProcessed(const QVersitDocument& document,
                                   const QVersitProperty& property,
                                   const QContact& contact,
                                   bool *alreadyProcessed,
                                   QList<QContactDetail>* updatedDetails) = 0;
    virtual void documentProcessed(const QVersitDocument& document,
                                   QContact* contact) = 0;
};

class Q_VERSIT_EXPORT QVersitContactImporter
{
public:
    enum Error {
        NoError = 0,
        InvalidDocumentError,
        EmptyDocumentError
    };

    QVersitContactImporter();
    explicit QVersitContactImporter(const QString& profile);
    explicit QVersitContactImporter(const QStringList& profiles);
    ~QVersitContactImporter();

    bool importDocuments(const QList<QVersitDocument>& documents);
    QList<QContact> contacts() const;
    QMap<int, Error> errorMap() const;

    void setPropertyHandler(QVersitContactImporterPropertyHandlerV2* handler);

    void setResourceHandler(QVersitResourceHandler* handler);
    QVersitResourceHandler* resourceHandler() const;

    /* deprecated */
    QMap<int, Error> errors() const;
    void setPropertyHandler(QVersitContactImporterPropertyHandler* handler);
    QVersitContactImporterPropertyHandler* propertyHandler() const;

private:
    QVersitContactImporterPrivate* d;
};

QTM_END_NAMESPACE

#endif // QVERSITCONTACTIMPORTER_H

