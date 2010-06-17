/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "organizeritemtimestamptransform.h"
#include "qorganizeritemtimestamp.h"

void OrganizerItemTimeStampTransform::transformToDetailL(const CCalEntry& entry, QOrganizerItem *item)
{
    QOrganizerItemTimestamp timeStamp = item->detail<QOrganizerItemTimestamp>();
    timeStamp.setCreated(toQDateTimeL(entry.DTStampL()));
    timeStamp.setLastModified(toQDateTimeL(entry.LastModifiedDateL()));
    item->saveDetail(&timeStamp);
}

void OrganizerItemTimeStampTransform::transformToDetailPostSaveL(const CCalEntry& entry, QOrganizerItem *item)
{
    transformToDetailL(entry, item);
}

void OrganizerItemTimeStampTransform::transformToEntryL(const QOrganizerItem& item, CCalEntry* entry)
{
    QOrganizerItemTimestamp timeStamp = item.detail<QOrganizerItemTimestamp>();
    if (!timeStamp.created().isNull() && timeStamp.created().isValid()) {
        // This is an old entry.
        entry->SetDTStampL(toTCalTimeL(timeStamp.created()));    
    } else {
        // This is new entry. Generate creation time.
        TCalTime dtstamp;
        TTime currentUtcTime;
        currentUtcTime.UniversalTime();
        dtstamp.SetTimeUtcL(currentUtcTime);
        entry->SetDTStampL(dtstamp);
    }
    // NOTE: modified timestamp is automatically set by symbian calendar server
}

QString OrganizerItemTimeStampTransform::detailDefinitionName()
{
    return QOrganizerItemTimestamp::DefinitionName;    
}
