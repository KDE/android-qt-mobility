/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QCONTACTRELATIONSHIPFILTER_H
#define QCONTACTRELATIONSHIPFILTER_H

#include "qtcontactsglobal.h"
#include "qcontactfilter.h"

#include <QSharedDataPointer>
#include <QStringList>
#include <QList>
#include <QString>

class QContactRelationshipFilterPrivate;
class QTCONTACTS_EXPORT QContactRelationshipFilter : public QContactFilter
{
public:
    QContactRelationshipFilter();
    QContactRelationshipFilter(const QContactFilter& other);

//QCRF { fixedUri; fixedId; relType; contactRelPosition [left, right, either] } <-- possible API?
//QCRF { 0 ,0, 0, either }      -> any local contact that is a participant in any relationship
//QCRF { 0, 0, 0, left}         -> only local contacts that are on the left hand side of a relationship
//QCRF { mgr, id, 0, left }     -> only local contacts that are on the left hand side of a relationship, with mgr:id being on the right

    enum Side {
        Left = 0,
        Right,
        Either
    };

    void takeSide(QContactRelationshipFilter::Side side);
    void setFixedUri(const QString& fixedUri);
    void setFixedId(const QUniqueId& fixedId);
    void setRelationshipType(const QString& relationshipType);

    Side side() const;
    QString fixedUri() const;
    QUniqueId fixedId() const;
    QString relationshipType() const;

private:
    Q_DECLARE_CONTACTFILTER_PRIVATE(QContactRelationshipFilter)
};

#endif
