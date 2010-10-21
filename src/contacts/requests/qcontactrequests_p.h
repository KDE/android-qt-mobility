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

#ifndef QCONTACTREQUESTS_P_H
#define QCONTACTREQUESTS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qcontactabstractrequest_p.h"
#include "qcontactfilter.h"
#include "qcontactrelationshipfilter.h"
#include "qcontactsortorder.h"
#include "qcontact.h"
#include "qtcontactsglobal.h"

#include <QStringList>
#include <QList>

QTM_BEGIN_NAMESPACE

class QContactSaveRequestPrivate : public QContactAbstractRequestPrivate
{
public:
    QContactSaveRequestPrivate()
        : QContactAbstractRequestPrivate()
    {
    }

    ~QContactSaveRequestPrivate()
    {
    }

    QContactAbstractRequest::RequestType type() const
    {
        return QContactAbstractRequest::ContactSaveRequest;
    }

    QList<QContact> m_contacts;
    QMap<int, QContactManager::Error> m_errors;
    QStringList m_definitionMask;
};

class QContactFetchRequestPrivate : public QContactAbstractRequestPrivate
{
public:
    QContactFetchRequestPrivate()
        : QContactAbstractRequestPrivate()
    {
    }

    ~QContactFetchRequestPrivate()
    {
    }

    QContactAbstractRequest::RequestType type() const
    {
        return QContactAbstractRequest::ContactFetchRequest;
    }

    QContactFilter m_filter;
    QList<QContactSortOrder> m_sorting;
    QContactFetchHint m_fetchHint;

    QList<QContact> m_contacts;
};

class QContactFetchByIdRequestPrivate : public QContactAbstractRequestPrivate
{
public:
    QContactFetchByIdRequestPrivate()
        : QContactAbstractRequestPrivate()
    {
    }

    ~QContactFetchByIdRequestPrivate()
    {
    }

    QContactAbstractRequest::RequestType type() const
    {
        return QContactAbstractRequest::ContactFetchByIdRequest;
    }

    QList<QContactLocalId> m_localIds;
    QContactFetchHint m_fetchHint;

    QList<QContact> m_contacts;
    QMap<int, QContactManager::Error> m_errors;
};

class QContactRemoveRequestPrivate : public QContactAbstractRequestPrivate
{
public:
    QContactRemoveRequestPrivate()
        : QContactAbstractRequestPrivate()
    {
    }

    ~QContactRemoveRequestPrivate()
    {
    }

    QContactAbstractRequest::RequestType type() const
    {
        return QContactAbstractRequest::ContactRemoveRequest;
    }

    QList<QContactLocalId> m_contactIds;
    QMap<int, QContactManager::Error> m_errors;
};

class QContactLocalIdFetchRequestPrivate : public QContactAbstractRequestPrivate
{
public:
    QContactLocalIdFetchRequestPrivate()
        : QContactAbstractRequestPrivate()
    {
    }

    ~QContactLocalIdFetchRequestPrivate()
    {
    }

    QContactAbstractRequest::RequestType type() const
    {
        return QContactAbstractRequest::ContactLocalIdFetchRequest;
    }

    QContactFilter m_filter;
    QList<QContactSortOrder> m_sorting;

    QList<QContactLocalId> m_ids;
};

class QContactDetailDefinitionFetchRequestPrivate : public QContactAbstractRequestPrivate
{
public:
    QContactDetailDefinitionFetchRequestPrivate()
        : QContactAbstractRequestPrivate(),
        m_contactType(QString(QLatin1String(QContactType::TypeContact)))
    {
    }

    ~QContactDetailDefinitionFetchRequestPrivate()
    {
    }

    QContactAbstractRequest::RequestType type() const
    {
        return QContactAbstractRequest::DetailDefinitionFetchRequest;
    }

    QString m_contactType;
    QStringList m_names;
    QMap<QString, QContactDetailDefinition> m_definitions;
    QMap<int, QContactManager::Error> m_errors;
};

class QContactDetailDefinitionSaveRequestPrivate : public QContactAbstractRequestPrivate
{
public:
    QContactDetailDefinitionSaveRequestPrivate()
        : QContactAbstractRequestPrivate(),
        m_contactType(QString(QLatin1String(QContactType::TypeContact)))
    {
    }

    ~QContactDetailDefinitionSaveRequestPrivate()
    {
    }

    QContactAbstractRequest::RequestType type() const
    {
        return QContactAbstractRequest::DetailDefinitionSaveRequest;
    }

    QString m_contactType;
    QList<QContactDetailDefinition> m_definitions;
    QMap<int, QContactManager::Error> m_errors;
};

class QContactDetailDefinitionRemoveRequestPrivate : public QContactAbstractRequestPrivate
{
public:
    QContactDetailDefinitionRemoveRequestPrivate()
        : QContactAbstractRequestPrivate(),
        m_contactType(QString(QLatin1String(QContactType::TypeContact)))
    {
    }

    ~QContactDetailDefinitionRemoveRequestPrivate()
    {
    }

    QContactAbstractRequest::RequestType type() const
    {
        return QContactAbstractRequest::DetailDefinitionRemoveRequest;
    }

    QString m_contactType;
    QStringList m_names;
    QMap<int, QContactManager::Error> m_errors;
};

class QContactRelationshipFetchRequestPrivate : public QContactAbstractRequestPrivate
{
public:
    QContactRelationshipFetchRequestPrivate()
        : QContactAbstractRequestPrivate()
    {
    }

    ~QContactRelationshipFetchRequestPrivate()
    {
    }

    QContactAbstractRequest::RequestType type() const
    {
        return QContactAbstractRequest::RelationshipFetchRequest;
    }

    // selection criteria
    QContactId m_first;
    QContactId m_second;
    QString m_relationshipType;

    // results
    QList<QContactRelationship> m_relationships;
};

class QContactRelationshipSaveRequestPrivate : public QContactAbstractRequestPrivate
{
public:
    QContactRelationshipSaveRequestPrivate()
        : QContactAbstractRequestPrivate()
    {
    }

    ~QContactRelationshipSaveRequestPrivate()
    {
    }

    QContactAbstractRequest::RequestType type() const
    {
        return QContactAbstractRequest::RelationshipSaveRequest;
    }

    QList<QContactRelationship> m_relationships;
    QMap<int, QContactManager::Error> m_errors;
};

class QContactRelationshipRemoveRequestPrivate : public QContactAbstractRequestPrivate
{
public:
    QContactRelationshipRemoveRequestPrivate()
        : QContactAbstractRequestPrivate()
    {
    }

    ~QContactRelationshipRemoveRequestPrivate()
    {
    }

    QContactAbstractRequest::RequestType type() const
    {
        return QContactAbstractRequest::RelationshipRemoveRequest;
    }

    QList<QContactRelationship> m_relationships;
    QMap<int, QContactManager::Error> m_errors;
};

QTM_END_NAMESPACE

#endif
