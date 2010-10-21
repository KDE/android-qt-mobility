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


#ifndef QCONTACTRELATIONSHIP_H
#define QCONTACTRELATIONSHIP_H

#include <QString>
#include <QList>
#include <QPair>
#include <QSharedDataPointer>

#include "qtcontactsglobal.h"
#include "qcontactid.h"

class QDataStream;

QTM_BEGIN_NAMESPACE

class QContactRelationshipPrivate;

class Q_CONTACTS_EXPORT QContactRelationship
{
public:
#ifdef Q_QDOC
    static const QLatin1Constant HasMember;
    static const QLatin1Constant Aggregates;
    static const QLatin1Constant IsSameAs;
    static const QLatin1Constant HasAssistant;
    static const QLatin1Constant HasManager;
    static const QLatin1Constant HasSpouse;
#else
    Q_DECLARE_LATIN1_CONSTANT(HasMember, "HasMember");
    Q_DECLARE_LATIN1_CONSTANT(Aggregates, "Aggregates");
    Q_DECLARE_LATIN1_CONSTANT(IsSameAs, "IsSameAs");
    Q_DECLARE_LATIN1_CONSTANT(HasAssistant, "HasAssistant");
    Q_DECLARE_LATIN1_CONSTANT(HasManager, "HasManager");
    Q_DECLARE_LATIN1_CONSTANT(HasSpouse, "HasSpouse");
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

    enum Role {
        First = 0,
        Second,
        Either
    };

private:
    QSharedDataPointer<QContactRelationshipPrivate> d;
};

Q_CONTACTS_EXPORT uint qHash(const QContactRelationship& key);
#ifndef QT_NO_DEBUG_STREAM
Q_CONTACTS_EXPORT QDebug operator<<(QDebug dbg, const QContactRelationship& rel);
#endif
#ifndef QT_NO_DATASTREAM
Q_CONTACTS_EXPORT QDataStream& operator<<(QDataStream& out, const QContactRelationship& rel);
Q_CONTACTS_EXPORT QDataStream& operator>>(QDataStream& in, QContactRelationship& rel);
#endif

QTM_END_NAMESPACE

#endif

