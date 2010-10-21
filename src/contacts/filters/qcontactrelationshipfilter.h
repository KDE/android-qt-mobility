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

#ifndef QCONTACTRELATIONSHIPFILTER_H
#define QCONTACTRELATIONSHIPFILTER_H

#include "qtcontactsglobal.h"
#include "qcontactfilter.h"
#include "qcontactid.h"
#include "qcontactrelationship.h"

#include <QSharedDataPointer>
#include <QStringList>
#include <QList>
#include <QString>

QTM_BEGIN_NAMESPACE

class QContactRelationshipFilterPrivate;
class Q_CONTACTS_EXPORT QContactRelationshipFilter : public QContactFilter
{
public:
    QContactRelationshipFilter();
    QContactRelationshipFilter(const QContactFilter& other);

    void setRelationshipType(const QString& relationshipType);
    void setRelatedContactId(const QContactId& relatedContactId);
    void setRelatedContactRole(QContactRelationship::Role relatedContactRole);

    QString relationshipType() const;
    QContactId relatedContactId() const;
    QContactRelationship::Role relatedContactRole() const;

private:
    Q_DECLARE_CONTACTFILTER_PRIVATE(QContactRelationshipFilter)
};

QTM_END_NAMESPACE

#endif
