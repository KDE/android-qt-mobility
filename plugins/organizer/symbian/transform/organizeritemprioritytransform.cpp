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
#include "organizeritemprioritytransform.h"
#include "qorganizeritempriority.h"

#define MINIMUM_PRIORITY 0
#define MAXIMUM_PRIORITY 255
#define PRIORITY_RESOLUTION 25

void OrganizerItemPriorityTransform::transformToDetailL(const CCalEntry& entry, QOrganizerItem *item)
{
    QOrganizerItemPriority priority;
    // Item has range from 0-9 and entry has range from 0 - 255.
    // NOTE: Zero means unknown priority. So just leave the detail out.
	int entryPriority = entry.PriorityL();
	if (entryPriority > 0) { 
	    entryPriority = entryPriority / PRIORITY_RESOLUTION;
	    entryPriority = (entryPriority > QOrganizerItemPriority::LowestPriority) ?
	        QOrganizerItemPriority::LowestPriority : entryPriority;
	    priority.setPriority((QOrganizerItemPriority::Priority) entryPriority);
	    item->saveDetail(&priority);
	}
}

void OrganizerItemPriorityTransform::transformToEntryL(const QOrganizerItem& item, CCalEntry* entry)
{
    QOrganizerItemPriority itemPriority = item.detail<QOrganizerItemPriority>();
    if (!itemPriority.isEmpty()) {
        int priority = itemPriority.priority();
        priority *= PRIORITY_RESOLUTION;
        entry->SetPriorityL(priority);
    }
}

QString OrganizerItemPriorityTransform::detailDefinitionName()
{
    return QOrganizerItemPriority::DefinitionName;    
}
