/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QORGANIZERITEMCHANGELOGFILTER_P_H
#define QORGANIZERITEMCHANGELOGFILTER_P_H

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

#include "qorganizeritemfilter_p.h"
#include "qorganizeritemfilter.h"

#include <QDateTime>

QTM_BEGIN_NAMESPACE

class QOrganizerItemChangeLogFilterPrivate : public QOrganizerItemFilterPrivate
{
public:
    QOrganizerItemChangeLogFilterPrivate(QOrganizerItemChangeLogFilter::EventType type = QOrganizerItemChangeLogFilter::EventAdded)
        : QOrganizerItemFilterPrivate()
        , m_eventType(type)
    {

    }

    QOrganizerItemChangeLogFilterPrivate(const QOrganizerItemChangeLogFilterPrivate& other)
        : QOrganizerItemFilterPrivate(other)
        , m_eventType(other.m_eventType)
        , m_since(other.m_since)
    {

    }

    virtual bool compare(const QOrganizerItemFilterPrivate* other) const
    {
        const QOrganizerItemChangeLogFilterPrivate *od = static_cast<const QOrganizerItemChangeLogFilterPrivate*>(other);
        if (m_eventType != od->m_eventType)
            return false;
        if (m_since != od->m_since)
            return false;
        return true;
    }

    QDataStream& outputToStream(QDataStream& stream, quint8 formatVersion) const
    {
        if (formatVersion == 1) {
            stream << static_cast<quint32>(m_eventType) << m_since;
        }
        return stream;
    }

    QDataStream& inputFromStream(QDataStream& stream, quint8 formatVersion)
    {
        if (formatVersion == 1) {
            quint32 eventType;
            stream >> eventType >> m_since;
            m_eventType = static_cast<QOrganizerItemChangeLogFilter::EventType>(eventType);
        }
        return stream;
    }

    Q_IMPLEMENT_ORGANIZERITEMFILTER_VIRTUALCTORS(QOrganizerItemChangeLogFilter, QOrganizerItemFilter::ChangeLogFilter)

    QOrganizerItemChangeLogFilter::EventType m_eventType;
    QDateTime m_since;
};

QTM_END_NAMESPACE

#endif
