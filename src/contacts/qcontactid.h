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

#ifndef QCONTACTID_H
#define QCONTACTID_H

#include <QString>
#include <QSharedDataPointer>

#include "qtcontactsglobal.h"

class QDataStream;
QTM_BEGIN_NAMESPACE

typedef quint32 QContactLocalId;

class QContactIdPrivate;
class Q_CONTACTS_EXPORT QContactId
{
public:
    QContactId();
    ~QContactId();

    QContactId(const QContactId& other);
    QContactId& operator=(const QContactId& other);

    bool operator==(const QContactId& other) const;
    bool operator!=(const QContactId& other) const;
    bool operator<(const QContactId& other) const;

    QString managerUri() const;
    QContactLocalId localId() const;

    void setManagerUri(const QString& uri);
    void setLocalId(const QContactLocalId& id);

private:
    QSharedDataPointer<QContactIdPrivate> d;
};

Q_CONTACTS_EXPORT uint qHash(const QContactId& key);
#ifndef QT_NO_DEBUG_STREAM
Q_CONTACTS_EXPORT QDebug operator<<(QDebug dbg, const QContactId& id);
#endif
#ifndef QT_NO_DATASTREAM
Q_CONTACTS_EXPORT QDataStream& operator<<(QDataStream& out, const QContactId& id);
Q_CONTACTS_EXPORT QDataStream& operator>>(QDataStream& in, QContactId& id);
#endif

QTM_END_NAMESPACE

Q_DECLARE_TYPEINFO(QTM_PREPEND_NAMESPACE(QContactId), Q_MOVABLE_TYPE);


#endif

