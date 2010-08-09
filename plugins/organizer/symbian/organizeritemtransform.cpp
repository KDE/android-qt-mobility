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

#include "organizeritemtransform.h"

#include <QDebug>
#include <calentry.h>
#include <calinstance.h>

#include "qorganizeritem.h"
#include "organizereventtimerangetransform.h"
#include "organizeritemdescriptiontransform.h"
#include "organizeritemdetailtransform.h"
#include "organizeritemdisplaylabeltransform.h"
#include "organizeritemguidtransform.h"
#include "organizeritemlocationtransform.h"
#include "organizeritemprioritytransform.h"
#include "organizeritemrecurrencetransform.h"
#include "organizeritemtimestamptransform.h"
#include "organizeritemtypetransform.h"
#include "organizerjournaltimerangetransform.h"
#include "organizertodoprogresstransform.h"
#include "organizertodotimerangetransform.h"
#include "qorganizereventoccurrence.h"

QTM_USE_NAMESPACE

OrganizerItemTransform::OrganizerItemTransform()
{
    m_detailTransforms.append(new OrganizerItemTypeTransform()); // this must be first always
    m_detailTransforms.append(new OrganizerEventTimeRangeTransform());
    m_detailTransforms.append(new OrganizerItemDescriptionTransform());
    m_detailTransforms.append(new OrganizerItemDisplayLabelTransform());
    m_detailTransforms.append(new OrganizerItemGuidTransform());
    m_detailTransforms.append(new OrganizerItemLocationTransform());
    m_detailTransforms.append(new OrganizerItemPriorityTransform());
    m_detailTransforms.append(new OrganizerItemRecurrenceTransform());
    m_detailTransforms.append(new OrganizerItemTimeStampTransform());
    m_detailTransforms.append(new OrganizerJournalTimeRangeTransform());
    m_detailTransforms.append(new OrganizerTodoProgressTransform());
    m_detailTransforms.append(new OrganizerTodoTimeRangeTransform());    
}

OrganizerItemTransform::~OrganizerItemTransform()
{
    // Delete transform objects
    foreach (OrganizerItemDetailTransform *i, m_detailTransforms)
        delete i;
}

void OrganizerItemTransform::toEntryL(const QOrganizerItem &item, CCalEntry *entry)
{
    // Loop through transform objects
    foreach (OrganizerItemDetailTransform *i, m_detailTransforms) {
        // TODO: This is just for debugging. Remove before release.
        TRAPD(err, i->transformToEntryL(item, entry));
        if (err) {
            qDebug() << "transformToEntryL failed! detail:" << i->detailDefinitionName() << "err:" << err;
            User::Leave(err);
        }
    }
}

void OrganizerItemTransform::toItemL(const CCalEntry &entry, QOrganizerItem *item) const
{
    // Loop through transform objects
    foreach (OrganizerItemDetailTransform *i, m_detailTransforms) {
        // TODO: This is just for debugging. Remove before release.
        TRAPD(err, i->transformToDetailL(entry, item));
        if (err) {
            qDebug() << "transformToDetailL failed! detail:" << i->detailDefinitionName() << "err:" << err;
            User::Leave(err);
        }
    }
}

void OrganizerItemTransform::toItemPostSaveL(const CCalEntry &entry, QOrganizerItem *item) const
{
    // Loop through transform objects
    foreach (OrganizerItemDetailTransform *i, m_detailTransforms) {
        // TODO: This is just for debugging. Remove before release.
        TRAPD(err, i->transformToDetailPostSaveL(entry, item));
        if (err) {
            qDebug() << "transformToDetailPostSaveL failed! detail:" << i->detailDefinitionName() << "err:" << err;
            User::Leave(err);
        }
    }
}

void OrganizerItemTransform::toItemL(const CCalInstance &instance, QOrganizerItem *item) const
{
    
    // TODO: strip recurrence stuff
    // TODO: set item instance origin
    // TODO: transform  CCalInstance::Time()
    // TODO: transform  CCalInstance::StartTimeL()
    // TODO: transform  CCalInstance::EndTimeL()
    QOrganizerItem parentItem;
    toItemL(instance.Entry(), &parentItem);
    
    // Create an object of QOrganizerEventOccurrence when entry is of type event
    if (QOrganizerItemType::TypeEvent == parentItem.type()) {
    
        QOrganizerEventOccurrence* occurrence = (QOrganizerEventOccurrence*)item; 
        occurrence->setGuid(parentItem.guid());
        occurrence->setParentLocalId(instance.Entry().LocalUidL());
        
        //set start date time of the insatance.
        QDateTime startDateTime(OrganizerItemDetailTransform::toQDateTimeL(instance.StartTimeL()));
        occurrence->setStartDateTime(startDateTime);
    
        //set orginal date as its easy for client to modify and resave the occurrence
        occurrence->setOriginalDate(startDateTime.date());
        //set end date time of the insatance
        occurrence->setEndDateTime(OrganizerItemDetailTransform::toQDateTimeL(instance.EndTimeL()));
        occurrence->setDescription(parentItem.description());
        QOrganizerItemPriority priority(parentItem.detail(QOrganizerItemPriority::DefinitionName));
        if (!priority.isEmpty()) {
           occurrence->setPriority(priority.priority());
        }
        QOrganizerItemLocation location(parentItem.detail(QOrganizerItemLocation::DefinitionName));
        if (!location.isEmpty()) {
            occurrence->setLocationName(location.locationName());
            occurrence->setLocationGeoCoordinates(location.geoLocation());
        }       
    }
}       

void OrganizerItemTransform::debugEntryL(const CCalEntry &entry) const
{
    qDebug() << QString("CCalEntry uid-%1 localUid-%2")
        .arg(OrganizerItemDetailTransform::toQString(entry.UidL()))
        .arg(entry.LocalUidL());
    qDebug() << "Type        :" << entry.EntryTypeL();
    qDebug() << "Summary     :" << OrganizerItemDetailTransform::toQString(entry.SummaryL());
    qDebug() << "Desription  :" << OrganizerItemDetailTransform::toQString(entry.DescriptionL());
}
