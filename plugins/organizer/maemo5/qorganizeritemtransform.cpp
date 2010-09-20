/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qorganizermaemo5ids_p.h"
#include "qorganizeritemtransform.h"
#include "qtorganizer.h"

#include <QtCore/quuid.h>
#include <CalendarErrors.h>
#include <CEvent.h>
#include <CTodo.h>
#include <CJournal.h>
#include <CRecurrence.h>
#include <CComponentDetails.h>
#include <CAlarm.h>

QTM_USE_NAMESPACE

OrganizerItemTransform::OrganizerItemTransform()
{
}

OrganizerItemTransform::~OrganizerItemTransform()
{
}

void OrganizerItemTransform::setManagerUri(QString managerUri)
{
    m_managerUri = managerUri;
}

QString OrganizerItemTransform::managerUri() const
{
    return m_managerUri;
}

OrganizerRecurrenceTransform* OrganizerItemTransform::recurrenceTransformer()
{
    return &m_recTransformer;
}

QOrganizerEvent OrganizerItemTransform::convertCEventToQEvent(CEvent *cevent)
{
    QOrganizerEvent retn;

    // Priority
    int tempint = cevent->getPriority();
    if (tempint != -1)
        retn.setPriority(static_cast<QOrganizerItemPriority::Priority>(tempint)); // assume that the saved priority is vCal compliant.

    // Location geo coordinates
    QString tempstr = QString::fromStdString(cevent->getGeo());
    if (!tempstr.isNull())
        retn.setLocationGeoCoordinates(tempstr);

    // Start time
    QDateTime tempdt = QDateTime::fromTime_t(cevent->getDateStart());
    if (!tempdt.isNull())
        retn.setStartDateTime(tempdt);

    // End time
    tempdt = QDateTime::fromTime_t(cevent->getDateEnd());
    if (!tempdt.isNull())
        retn.setEndDateTime(tempdt);

    // Recurrence information
    m_recTransformer.transformToQrecurrence(cevent->getRecurrence());

    retn.setRecurrenceRules(m_recTransformer.recurrenceRules());
    retn.setExceptionRules(m_recTransformer.exceptionRules());
    retn.setRecurrenceDates(m_recTransformer.recurrenceDates());
    retn.setExceptionDates(m_recTransformer.exceptionDates());

    return retn;
}

QOrganizerEventOccurrence OrganizerItemTransform::convertCEventToQEventOccurrence(CEvent *cevent)
{
    QDateTime instanceStartDate = QDateTime::fromTime_t(cevent->getDateStart());
    QDateTime instanceEndDate = QDateTime::fromTime_t(cevent->getDateEnd());
    return convertCEventToQEventOccurrence(cevent, instanceStartDate, instanceEndDate, QOrganizerItemLocalId(0));
}

QOrganizerEventOccurrence OrganizerItemTransform::convertCEventToQEventOccurrence(CEvent* cevent, const QDateTime& instanceStartDate, const QDateTime &instanceEndDate)
{
    QOrganizerEventOccurrence retn;

    // Priority
    int tempint = cevent->getPriority();
    if (tempint != -1)
        retn.setPriority(static_cast<QOrganizerItemPriority::Priority>(tempint)); // assume that the saved priority is vCal compliant.

    // Location geo coordinates
    QString tempstr = QString::fromStdString(cevent->getGeo());
    if (!tempstr.isNull())
        retn.setLocationGeoCoordinates(tempstr);

    // Start time
    if (!instanceStartDate.isNull())
        retn.setStartDateTime(instanceStartDate);

    // End time
    if (!instanceEndDate.isNull())
        retn.setEndDateTime(instanceEndDate);

    // Set parent local id
    QString idString = QString::fromStdString(cevent->getId());
    QOrganizerItemLocalId localId(new QOrganizerItemMaemo5EngineLocalId(idString.toUInt()));
    retn.setParentLocalId(localId);

    // Set original event date
    retn.setOriginalDate(instanceStartDate.date());

    return retn;
}

QOrganizerEventOccurrence OrganizerItemTransform::convertCEventToQEventOccurrence(CEvent *cevent, const QDateTime &instanceStartDate, const QDateTime &instanceEndDate, QOrganizerItemLocalId parentLocalId)
{
    QOrganizerEventOccurrence retn = convertCEventToQEventOccurrence(cevent, instanceStartDate, instanceEndDate);
    retn.setParentLocalId(parentLocalId);
    return retn;
}

QOrganizerTodo OrganizerItemTransform::convertCTodoToQTodo(CTodo *ctodo)
{
    QOrganizerTodo retn;

    // Priority
    int tempint = ctodo->getPriority();
    if (tempint != -1)
        retn.setPriority(static_cast<QOrganizerItemPriority::Priority>(tempint));

    // Date start
    QDateTime tempdt = QDateTime::fromTime_t(ctodo->getDateStart());
    if (!tempdt.isNull())
        retn.setStartDateTime(tempdt);

    // Due
    tempdt = QDateTime::fromTime_t(ctodo->getDue());
    if (!tempdt.isNull())
        retn.setDueDateTime(tempdt);

    // Completed time
    tempdt = QDateTime::fromTime_t(ctodo->getCompleted());
    if (!tempdt.isNull())
        retn.setFinishedDateTime(tempdt);

    // Percent complete
    tempint = ctodo->getPercentComplete();
    if (tempint != -1)
        retn.setProgressPercentage(tempint);

    // Status
    retn.setStatus(static_cast<QOrganizerTodoProgress::Status>(ctodo->getStatus()));

    // Location geo coordinates
    QString tempstr = QString::fromStdString(ctodo->getGeo());
    if (!tempstr.isNull()) {
        QOrganizerItemLocation il = retn.detail<QOrganizerItemLocation>();
        il.setGeoLocation(tempstr);
        retn.saveDetail(&il);
    }

    return retn;
}

QOrganizerTodoOccurrence OrganizerItemTransform::convertCTodoToQTodoOccurrence(CTodo *ctodo)
{
    QOrganizerTodoOccurrence retn;

    // Keep the following details same as in convertCTodoToQTodo:

    // Priority
    int tempint = ctodo->getPriority();
    if (tempint != -1)
        retn.setPriority(static_cast<QOrganizerItemPriority::Priority>(tempint));

    // Date start
    QDateTime tempdt = QDateTime::fromTime_t(ctodo->getDateStart());
    if (!tempdt.isNull())
        retn.setStartDateTime(tempdt);

    // Due
    tempdt = QDateTime::fromTime_t(ctodo->getDue());
    if (!tempdt.isNull())
        retn.setDueDateTime(tempdt);

    // Completed time
    tempdt = QDateTime::fromTime_t(ctodo->getCompleted());
    if (!tempdt.isNull())
        retn.setFinishedDateTime(tempdt);

    // Percent complete
    tempint = ctodo->getPercentComplete();
    if (tempint != -1)
        retn.setProgressPercentage(tempint);

    // Status
    retn.setStatus(static_cast<QOrganizerTodoProgress::Status>(ctodo->getStatus()));

    // Location geo coordinates
    QString tempstr = QString::fromStdString(ctodo->getGeo());
    if (!tempstr.isNull()) {
        QOrganizerItemLocation il = retn.detail<QOrganizerItemLocation>();
        il.setGeoLocation(tempstr);
        retn.saveDetail(&il);
    }

    // Only the following are occurrence specific details:

    // In maemo, the parent id is the same as this id (todo's only have one occurrence)
    retn.setParentLocalId(QOrganizerItemLocalId(new QOrganizerItemMaemo5EngineLocalId(QString::fromStdString(ctodo->getId()).toUInt())));

    // Original date
    retn.setOriginalDate(retn.startDateTime().date());

    return retn;
}

QOrganizerJournal OrganizerItemTransform::convertCJournalToQJournal(CJournal *cjournal)
{
    QOrganizerJournal retn;

    // Start time
    QDateTime tempdt = QDateTime::fromTime_t(cjournal->getDateStart());
    if (!tempdt.isNull())
        retn.setDateTime( tempdt );

    return retn;
}

void OrganizerItemTransform::fillInCommonCComponentDetails(QOrganizerItem *item, CComponent *component, bool setId)
{
    if (item) {
        // Summary
        QString tempstr = QString::fromStdString(component->getSummary());
        if (!tempstr.isEmpty())
            item->setDisplayLabel(tempstr);

        // Description
        tempstr = QString::fromStdString(component->getDescription());
        if (!tempstr.isEmpty())
            item->setDescription(tempstr);

        // Location
        tempstr = QString::fromStdString(component->getLocation());
        if(!tempstr.isEmpty()) {
            int lastCr = tempstr.lastIndexOf("\n");
            if (lastCr == -1) {
                QOrganizerItemLocation il = item->detail<QOrganizerItemLocation>();
                il.setLocationName(tempstr);
                item->saveDetail(&il);
            }
            else {
                QString locationName = tempstr.left(lastCr);
                QString locationAddress = tempstr.mid(lastCr + 1);
                QOrganizerItemLocation il = item->detail<QOrganizerItemLocation>();
                il.setLocationName(locationName);
                il.setAddress(locationAddress);
                item->saveDetail(&il);
            }
        }

        // Timestamps
        time_t createdTime = component->getCreatedTime();
        time_t lastModifiedTime = component->getLastModified();

        if (createdTime || lastModifiedTime) {
            QOrganizerItemTimestamp timeStamps = item->detail<QOrganizerItemTimestamp>();
            timeStamps.setCreated(QDateTime::fromTime_t(createdTime));
            timeStamps.setLastModified(QDateTime::fromTime_t(lastModifiedTime));
            item->saveDetail(&timeStamps);
        }

        // GUid
        QOrganizerItemGuid ig = item->detail<QOrganizerItemGuid>();
        tempstr = QString::fromStdString(component->getGUid());
        if(!tempstr.isEmpty())
            ig.setGuid(tempstr);
        item->saveDetail(&ig);

        // Set component ID
        QOrganizerItemId id;
        id.setManagerUri(managerUri());
        if (setId) {
            QString idString = QString::fromStdString(component->getId());
            QOrganizerItemLocalId localId(new QOrganizerItemMaemo5EngineLocalId(idString.toUInt()));
            id.setLocalId(localId);
        }
        else {
            id.setLocalId(QOrganizerItemLocalId()); // no local id
        }
        item->setId(id);

        // Set comments
        CComponentDetails *componentDetails = dynamic_cast<CComponentDetails*>(component);
        if (componentDetails) {
            string comments = componentDetails->getComments();
            if (!comments.empty()) {
                QStringList commentList = QString::fromStdString(comments).split('\n', QString::SkipEmptyParts);
                foreach(const QString comment, commentList)
                    item->addComment(comment);
            }
        }

        // Reminder (alarm)
        CAlarm *alarm = component->getAlarm();
        if (alarm) {
            // TODO: Only visual remainders are supported
            QOrganizerItemVisualReminder reminder = item->detail<QOrganizerItemVisualReminder>();
            reminder.setRepetition(alarm->getRepeat(), reminder.repetitionDelay());
            reminder.setTimeDelta(alarm->getTimeBefore());

            // Alarm time and messages can't be read with CAlarm,
            // read them straight from the alarm framework:
            // TODO: This does not work, but it seems that it doesn't work in Maemo5 the calendar
            // backend either. Maybe it's not possible to implement alarm fetch?

            /*
            // Get the cookie
            std::vector<long> cookies = alarm->getCookie();
            if (cookies.size() > 0) {
                cookie_t cookie = static_cast<cookie_t>(cookies[0]); // only one alarm supported

                alarm_event_t *eve = 0;
                if ((eve = alarmd_event_get(cookie)) != 0) {
                    QString message = QString::fromStdString(alarm_event_get_message(eve));
                    reminder.setMessage(message);
                    time_t alarmTime = alarm_event_get_trigger(eve);
                    reminder.setDateTime(QDateTime::fromTime_t(alarmTime));
                    alarm_event_delete(eve);
                }
            }
            */

            item->saveDetail(&reminder);
        }
    }
}

CComponent* OrganizerItemTransform::createCComponent(CCalendar *cal, const QOrganizerItem *item, QOrganizerItemManager::Error *error)
{
    *error = QOrganizerItemManager::InvalidItemTypeError;

    QOrganizerItemLocalId itemId = item->localId();

    QString itemIdStr = "";
    if (!itemId.isNull())
        itemIdStr = QString::number((static_cast<QOrganizerItemMaemo5EngineLocalId*>(QOrganizerItemManagerEngine::engineLocalItemId(itemId)))->m_localItemId);

    int calId = cal->getCalendarId();
    int calError = CALENDAR_OPERATION_SUCCESSFUL;
    CComponent *retn = 0; // Return null on errors

    if (item->type() == QOrganizerItemType::TypeEvent
        || item->type() == QOrganizerItemType::TypeEventOccurrence) {

        *error = QOrganizerItemManager::NoError;

        CEvent *cevent = cal->getEvent(itemIdStr.toStdString(), calError);
        if (!cevent) {
            // Event did not existed in calendar, create a new CEvent with an empty ID
            cevent = new CEvent();
        }
        else {
            // Event existed in calendar, use the existing event's ID
            cevent->setId(itemIdStr.toStdString());
        }
        cevent->setCalendarId(calId);

        // Set the event specific details:
        const QOrganizerEvent *event = static_cast<const QOrganizerEvent *>(item);

        // Location geo coordinates
        if (!event->locationGeoCoordinates().isEmpty())
            cevent->setGeo(event->locationGeoCoordinates().toStdString());

        // Priority
        cevent->setPriority(static_cast<int>(event->priority()));

        // Start date
        if (!event->startDateTime().isNull())
            cevent->setDateStart(event->startDateTime().toTime_t());

        // End date
        if (!event->endDateTime().isNull())
            cevent->setDateEnd(event->endDateTime().toTime_t());

        if (item->type() == QOrganizerItemType::TypeEvent) {
            // Build and set the recurrence information for the event
            CRecurrence *recurrence = createCRecurrence(item, error);
            cevent->setRecurrence(recurrence);
            delete recurrence; // setting makes a copy
            recurrence = 0;
        }
        // (else nothing: QOrganizerItemEventOccurrence does not have recurrence information)

        retn = cevent;
    }
    else if (item->type() == QOrganizerItemType::TypeTodo
            || item->type() == QOrganizerItemType::TypeTodoOccurrence) {

        *error = QOrganizerItemManager::NoError;

        CTodo *ctodo = cal->getTodo(itemIdStr.toStdString(), calError);
        if (!ctodo) {
            // Event did not existed in calendar, create a new CEvent with an empty ID
            ctodo = new CTodo();
        }
        else {
            // Event existed in calendar, use the existing event's ID
            ctodo->setId(itemIdStr.toStdString());
        }
        ctodo->setCalendarId(calId);

        // Set the todo specific details:
        const QOrganizerTodo *todo = static_cast<const QOrganizerTodo *>(item);

        // Priority
        ctodo->setPriority(static_cast<int>(todo->priority()));

        // Date start
        if (!todo->startDateTime().isNull())
            ctodo->setDateStart(todo->startDateTime().toTime_t());

        // Due date
        if (!todo->dueDateTime().isNull())
            ctodo->setDue(todo->dueDateTime().toTime_t());

        // Completed time
        if (!todo->finishedDateTime().isNull())
            ctodo->setCompleted(todo->finishedDateTime().toTime_t());

        // Percent complete
        ctodo->setPercentComplete(todo->progressPercentage());

        // Status
        ctodo->setStatus(todo->status());

        // Location geo coordinates
        if (!todo->detail("QOrganizerItemLocation::DefinitionName").isEmpty())
            ctodo->setGeo(todo->detail<QOrganizerItemLocation>().geoLocation().toStdString());

        // Recurrence is not set as todos can't contain any recurrence information in Maemo5

        retn = ctodo;
    }
    else if (item->type() == QOrganizerItemType::TypeJournal) {
        *error = QOrganizerItemManager::NoError;
        CJournal *cjournal = cal->getJournal(itemIdStr.toStdString(), calError);
        if (!cjournal) {
            // Event did not existed in calendar, create a new CEvent with an empty ID
            cjournal = new CJournal();
        }
        else {
            // Event existed in calendar, use the existing event's ID
            cjournal->setId(itemIdStr.toStdString());
        }
        cjournal->setCalendarId(calId);

        // Set journal specific details
        const QOrganizerJournal *journal = static_cast<const QOrganizerJournal *>(item);
        if (!journal->dateTime().isNull())
            cjournal->setDateStart(journal->dateTime().toTime_t());

        retn = cjournal;
    }

    if (retn) {
        // Set the details common for all the CComponents:

        // Summary
        if (!item->displayLabel().isEmpty())
            retn->setSummary(item->displayLabel().toStdString());

        // Descriptiom
        if (!item->description().isEmpty())
            retn->setDescription(item->description().toStdString());

        // Location (Geo location is not set here as it's not a general CComponent detail)
        QOrganizerItemLocation location = item->detail(QOrganizerItemLocation::DefinitionName);
        if (!location.isEmpty()) {
            QString locationString = location.locationName() + "\n" + location.address();
            retn->setLocation(locationString.toStdString());
        }

        // dateStart and dateEnd are common fields for all CComponents, but those are set
        // separately for each item type here, because there are independent time ranges
        // defined for each type in the Qt Mobility API.

        // GUid
        if (!item->guid().isEmpty())
            retn->setGUid(item->guid().toStdString());
        else
            retn->setGUid(randomGuid().toStdString()); // no GUID was set, generate a random GUID

        // Comments
        QStringList commentList = item->comments();
        if (!commentList.isEmpty()) {
            string comments;
            foreach(QString comment, commentList)
                comments += comment.append('\n').toStdString();

            CComponentDetails *componentDetails = dynamic_cast<CComponentDetails*>(retn);
            if (componentDetails)
                componentDetails->setComments(comments);
        }

        // Visual reminder (alarm)
        QOrganizerItemVisualReminder reminder = item->detail<QOrganizerItemVisualReminder>();
        if (reminder.dateTime() != QDateTime()) {
            // Set alarm for the ccomponent
            CAlarm alarm;

            alarm.setTimeBefore(reminder.timeDelta());
            alarm.setRepeat(reminder.repetitionCount());
            retn->setAlarmBefore(reminder.timeDelta());

            if (calError == CALENDAR_OPERATION_SUCCESSFUL)
                retn->setAlarm(&alarm); // makes a copy
        }
    }

    return retn;
}

CRecurrence* OrganizerItemTransform::createCRecurrence(const QOrganizerItem* item, QOrganizerItemManager::Error *error)
{
    *error = QOrganizerItemManager::NoError;

    // Only the event and todo types contain recurrence information
    if (item->type() == QOrganizerItemType::TypeEvent) {
        m_recTransformer.beginTransformToCrecurrence();
        const QOrganizerEvent *event = static_cast<const QOrganizerEvent *>(item);

        // Add recurrence rules
        QList<QOrganizerItemRecurrenceRule> recurrenceRules = event->recurrenceRules();
        foreach (QOrganizerItemRecurrenceRule rule, recurrenceRules)
            m_recTransformer.addQOrganizerItemRecurrenceRule(rule);

        // Add exception rules
        QList<QOrganizerItemRecurrenceRule> exceptionRules = event->exceptionRules();
        foreach (QOrganizerItemRecurrenceRule rule, exceptionRules)
            m_recTransformer.addQOrganizerItemExceptionRule(rule);

        // Add recurrence dates
        QDate dateLimit = event->startDateTime().date().addYears(6);
        QList<QDate> recurrenceDates = event->recurrenceDates();
        foreach (QDate recDate, recurrenceDates) {
            // Because recurrence dates are simulated with setting an appropriate
            // recurrence rule, no date must be set 6 years or more after
            // the current event's date. Otherwise setting a correct rule would be impossible.
            if (recDate >= dateLimit)
                *error = QOrganizerItemManager::NotSupportedError;

            // Still set the date, let the caller decise what to do
            m_recTransformer.addQOrganizerItemRecurrenceDate(recDate);
        }

        // Add exception dates
        QList<QDate> exceptionDates = event->exceptionDates();
        foreach (QDate exceptionDate, exceptionDates) {
            // Because exception dates are simulated with setting an appropriate
            // exception rule, no date must be set 6 years or more after
            // the current event's date. Otherwise setting a correct rule would be impossible.
            if (exceptionDate >= dateLimit)
                *error = QOrganizerItemManager::NotSupportedError;

            // Still set the date, let the caller decise what to do
            m_recTransformer.addQOrganizerItemExceptionDate(exceptionDate);
        }

        return m_recTransformer.crecurrence();
    }

    return 0; // no recurrence information for this item type
}

void OrganizerItemTransform::setAlarm(CCalendar *cal, QOrganizerItem *item, CComponent *component)
{
    CAlarm *alarm = component->getAlarm();
    if (alarm) {
        // Delete all the previous alarms
        std::vector<long> cookies = alarm->getCookie();
        std::vector<long>::iterator cookie;
        int ignoreErrors = 0;
        for (cookie = cookies.begin(); cookie != cookies.end(); ++cookie)
            alarm->deleteAlarmEvent(*cookie, ignoreErrors);

        // Set alarm
        QOrganizerItemVisualReminder reminder = item->detail<QOrganizerItemVisualReminder>();
        if (reminder.dateTime() != QDateTime()) {
            alarm->addAlarmEvent(reminder.dateTime().toTime_t(), reminder.message().toStdString(),
                                 component->getLocation(), component->getDateStart(), component->getDateEnd(),
                                 component->getId(), cal->getCalendarId(), component->getDescription(),
                                 component->getType(), component->getAllDay(), QString("").toStdString(),
                                 ignoreErrors);
        }
    }
}

QOrganizerItemManager::Error OrganizerItemTransform::calErrorToManagerError(int calError) const
{
    switch(calError) {
        case CALENDAR_OPERATION_SUCCESSFUL:
            return QOrganizerItemManager::NoError;

        case CALENDAR_SYSTEM_ERROR:
        case CALENDAR_DATABASE_ERROR:
        // case CALENDAR_DBUS_ERROR: // CALENDAR_DBUS_ERROR and CALENDAR_FILE_ERROR have the same value 3
        case CALENDAR_LTIME_ERROR:
        case CALENDAR_LIBALARM_ERROR:
        case CALENDAR_ALARM_ERROR:
        case CALENDAR_APP_ERROR:
        case CALENDAR_FUNC_ERROR:
        case CALENDAR_ICAL_PARSE_ERROR:
        case CALENDAR_INVALID_FILE:
        case CALENDAR_INVALID_ICSFILE:
        case CALENDAR_SCHEMA_CHANGED:
        case CALENDAR_IMPORT_INCOMPLETE:
            return QOrganizerItemManager::UnspecifiedError;

        case CALENDAR_MEMORY_ERROR:
            return QOrganizerItemManager::OutOfMemoryError;

        case CALENDAR_FILE_ERROR:
        case CALENDAR_DOESNOT_EXISTS:
        case CALENDAR_NONE_INDB:
        case NO_DUPLICATE_ITEM:
        case CALENDAR_FETCH_NOITEMS:
            return QOrganizerItemManager::DoesNotExistError;

        case CALENDAR_DISK_FULL:
        case CALENDAR_DB_FULL:
            return QOrganizerItemManager::LimitReachedError;

        case CALENDAR_INVALID_ARG_ERROR:
            return QOrganizerItemManager::BadArgumentError;

        case CALENDAR_ALREADY_EXISTS:
        case CALENDAR_ENTRY_DUPLICATED:
            return QOrganizerItemManager::AlreadyExistsError;

        case CALENDAR_CANNOT_BE_DELETED:
        case EXT_ITEM_RETAINED:
        case LOCAL_ITEM_RETAINED:
            return QOrganizerItemManager::PermissionsError;

        case CALENDAR_DB_LOCKED:
            return QOrganizerItemManager::LockedError;

        case CALENDAR_ICS_COMPONENT_INVALID:
        case CALENDAR_BATCH_ADD_INVALID:
            return QOrganizerItemManager::InvalidDetailError;

        default:
            return QOrganizerItemManager::UnspecifiedError;
    }
}

QString OrganizerItemTransform::fromCalendarColour(CalendarColour calendarColour)
{
    QMap<CalendarColour, QString> colourMap = calendarColourMap();
    if (colourMap.contains(calendarColour))
        return colourMap[calendarColour];
    else
        return QString();
}

CalendarColour OrganizerItemTransform::toCalendarColour(QString calendarColour)
{
    QMap<CalendarColour, QString> colourMap = calendarColourMap();
    if (colourMap.values().contains(calendarColour))
        return colourMap.key(calendarColour);
    else
        return COLOUR_DARKBLUE; // default color
}

QMap<CalendarColour, QString> OrganizerItemTransform::calendarColourMap() const
{
    QMap<CalendarColour, QString> retn;
    retn[COLOUR_DARKBLUE] = "Dark blue";
    retn[COLOUR_DARKGREEN] = "Dark green";
    retn[COLOUR_DARKRED] = "Dark red";
    retn[COLOUR_ORANGE] = "Orange";
    retn[COLOUR_VIOLET] = "Violet";
    retn[COLOUR_YELLOW] = "Yellow";
    retn[COLOUR_WHITE] = "White";
    retn[COLOUR_RED] = "Red";
    retn[COLOUR_GREEN] = "Green";
    retn[COLOUR_BLUE] = "Blue";
    retn[COLOUR_NEXT_FREE] = "Next free";
    return retn;
}

QString OrganizerItemTransform::fromCalendarType(CalendarType calendarType)
{
    QMap<CalendarType, QString> typeMap = calendarTypeMap();
    if (typeMap.contains(calendarType))
        return typeMap[calendarType];
    else
        return QString();
}

CalendarType OrganizerItemTransform::toCalendarType(QString calendarType)
{
    QMap<CalendarType, QString> typeMap = calendarTypeMap();
    if (typeMap.values().contains(calendarType))
        return typeMap.key(calendarType);
    else
        return LOCAL_CALENDAR; // default type
}

QMap<CalendarType, QString> OrganizerItemTransform::calendarTypeMap() const
{
    QMap<CalendarType, QString> retn;
    retn[LOCAL_CALENDAR] = "Local";
    retn[BIRTHDAY_CALENDAR] = "Birthday";
    retn[SYNC_CALENDAR] = "Sync";
    retn[DEFAULT_PRIVATE] = "Default private";
    retn[DEFAULT_SYNC] = "Default sync";
    return retn;
}

QString OrganizerItemTransform::randomGuid() const
{
    QUuid guid = QUuid::createUuid();
    return guid.toString();
}
