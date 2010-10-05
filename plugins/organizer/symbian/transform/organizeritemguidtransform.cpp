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
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "organizeritemguidtransform.h"
#include "qorganizeritemguid.h"

const TInt KGuidLength = 30;

OrganizerItemGuidTransform::OrganizerItemGuidTransform()
{
    // Set seed for qrand()
    TTime homeTime;
    homeTime.HomeTime();
    uint seed = (uint) homeTime.Int64();
    qsrand(seed);
}

void OrganizerItemGuidTransform::transformToDetailL(const CCalEntry& entry, QOrganizerItem *item)
{
    QString guid = toQString(entry.UidL());
    if (!guid.isEmpty())
        item->setGuid(guid);
}

void OrganizerItemGuidTransform::transformToDetailPostSaveL(const CCalEntry& entry, QOrganizerItem *item)
{
    transformToDetailL(entry, item);
}

void OrganizerItemGuidTransform::transformToEntryL(const QOrganizerItem& item, CCalEntry* entry)
{
    Q_UNUSED(item);
    Q_UNUSED(entry);
    // Not used. Guid is already set when CCalEntry was created.
}

QString OrganizerItemGuidTransform::detailDefinitionName()
{
    return QOrganizerItemGuid::DefinitionName;    
}

HBufC8 *OrganizerItemGuidTransform::guidLC(const QOrganizerItem &item)
{
    // Read guid from organizer item
    QString globalUidString = item.guid();

    // Create a buffer for guid
    HBufC8* globalUidBuf = HBufC8::NewLC(KGuidLength);
    TPtr8 ptr = globalUidBuf->Des();

    if (item.guid().isEmpty()) {
        // Generate a new quid
        // TODO: is this long enough for us? We could add imei or second random number to it...
        ptr.Num(qrand());
    } else {
        // Use the old guid
        QByteArray bytes = globalUidString.toUtf8();
        ptr = toPtrC8(bytes);
    }

    return globalUidBuf; // ownership passed
}
