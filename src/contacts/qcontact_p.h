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


#ifndef QCONTACT_P_H
#define QCONTACT_P_H

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

#include <QSharedData>
#include <QList>
#include <QDateTime>
#include <QMultiHash>

#include "qtcontacts.h"

QTM_BEGIN_NAMESPACE

class QContactData : public QSharedData
{
public:
    QContactData()
        : QSharedData()
    {
    }

    QContactData(const QContactData& other)
        : QSharedData(other),
        m_id(other.m_id),
        m_details(other.m_details),
        m_relationshipsCache(other.m_relationshipsCache),
        m_preferences(other.m_preferences)
    {
    }

    ~QContactData() {}

    QContactId m_id;
    QList<QContactDetail> m_details;
    QList<QContactRelationship> m_relationshipsCache;
    QMap<QString, int> m_preferences;

    // Helper function
    void removeOnly(const QSet<QString>& detailMask);

    // Trampoline
    static QSharedDataPointer<QContactData>& contactData(QContact& contact) {return contact.d;}
};

QTM_END_NAMESPACE

#endif

