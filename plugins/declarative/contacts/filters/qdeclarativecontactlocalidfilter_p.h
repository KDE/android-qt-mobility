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

#ifndef QDECLARATIVECONTACTLOCALIDFILTER_H
#define QDECLARATIVECONTACTLOCALIDFILTER_H

#include "qdeclarativecontactfilter_p.h"
#include "qcontactlocalidfilter.h"

#include <QStringList>
#include <QSet>
class QDeclarativeContactLocalIdFilter : public QDeclarativeContactFilter
{
    Q_OBJECT
    Q_PROPERTY(QStringList ids READ ids WRITE setIds NOTIFY valueChanged())
    Q_CLASSINFO("DefaultProperty", "ids")
public:
    QDeclarativeContactLocalIdFilter(QObject* parent = 0)
        :QDeclarativeContactFilter(parent)
    {
        connect(this, SIGNAL(valueChanged()), SIGNAL(filterChanged()));
    }

    QStringList ids() const
    {
        QStringList ids;
        foreach (const QContactLocalId& id, d.ids()) {
            ids << QString::number(id);
        }
        return ids;
    }

    void setIds(const QStringList& ids)
    {
        QList<QContactLocalId> contactIds;
        bool ok;

        foreach (const QString& id, ids) {
            QContactLocalId localId = id.toInt(&ok);
            if (ok) {
                contactIds << localId;
            }
        }
        if (contactIds.toSet() != d.ids().toSet()) {
            d.setIds(contactIds);
            emit valueChanged();
        }
    }

    QContactFilter filter() const
    {
        return d;
    }

signals:
    void valueChanged();

private:
    QContactLocalIdFilter d;
};

QML_DECLARE_TYPE(QDeclarativeContactLocalIdFilter)

#endif
