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


#ifndef QCONTACTRELATIONSHIP_H
#define QCONTACTRELATIONSHIP_H

#include <QString>
#include <QList>
#include <QPair>
#include <QSharedDataPointer>

#include "qtcontactsglobal.h"
#include "qcontactid.h"

QTM_BEGIN_NAMESPACE

class QContactRelationshipPrivate;

class Q_CONTACTS_EXPORT QContactRelationship
{
public:
#ifdef Q_QDOC
    const char* HasMember;
    const char* Aggregates;
    const char* Is;
    const char* HasAssistant;
    const char* HasManager;
    const char* HasSpouse;
#else
    Q_DECLARE_LATIN1_LITERAL(HasMember, "HasMember");
    Q_DECLARE_LATIN1_LITERAL(Aggregates, "Aggregates");
    Q_DECLARE_LATIN1_LITERAL(Is, "Is");
    Q_DECLARE_LATIN1_LITERAL(HasAssistant, "HasAssistant");
    Q_DECLARE_LATIN1_LITERAL(HasManager, "HasManager");
    Q_DECLARE_LATIN1_LITERAL(HasSpouse, "HasSpouse");
#endif

    QContactRelationship();
    ~QContactRelationship();

    QContactRelationship(const QContactRelationship& other);
    QContactRelationship& operator=(const QContactRelationship& other);
    bool operator==(const QContactRelationship &other) const;
    bool operator!=(const QContactRelationship &other) const { return !(*this==other); }

    QContactId first() const;
    QContactId second() const;
    QString relationshipType() const;

    void setFirst(const QContactId& firstId);
    void setSecond(const QContactId& secondId);
    void setRelationshipType(const QString& relationshipType);

private:
    QSharedDataPointer<QContactRelationshipPrivate> d;
};

QTM_END_NAMESPACE

#endif

