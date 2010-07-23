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
#include "qorganizermaemo5_p.h"
#include "qtorganizer.h"

#include <QDir>
#include <QFileSystemWatcher>

#include <CalendarErrors.h>
#include <CEvent.h>
#include <CJournal.h>
#include <CTodo.h>
#include <CRecurrence.h>

/**
 * entryType enum
 * This enum is from the Maemo5 calendar backend file Common.h
 * The enumeration is pasted here to avoid including Common.h file,
 * which contains dependencies to D-Bus handling and other components
 * that are not actually needed here.
*/
enum entryType {
    E_CALENDAR = 0,
    E_EVENT,
    E_TODO,
    E_JOURNAL,
    E_BDAY,
    E_SPARE,
    E_VTIMEZONE
};

QTM_USE_NAMESPACE

static const char* CALENDAR =  "/.calendar";
static const char* CALENDARDB = "/calendardb";

QOrganizerItemManagerEngine* QOrganizerItemMaemo5Factory::engine(const QMap<QString, QString> &parameters, QOrganizerItemManager::Error *error)
{
    Q_UNUSED(parameters);
    Q_UNUSED(error);

    QOrganizerItemMaemo5Engine* retn = new QOrganizerItemMaemo5Engine(); // manager takes ownership and will clean up.
    return retn;
}

QString QOrganizerItemMaemo5Factory::managerName() const
{
    return QString("maemo5");
}
Q_EXPORT_PLUGIN2(qtorganizer_maemo5, QOrganizerItemMaemo5Factory);

QOrganizerItemMaemo5Engine::QOrganizerItemMaemo5Engine()
    : d(new QOrganizerItemMaemo5EngineData)
{
    QString dbPath = QDir::homePath().append(CALENDAR).append(CALENDARDB);
    QFileSystemWatcher *databaseMonitor = new QFileSystemWatcher(this);
    databaseMonitor->addPath(dbPath);
    connect(databaseMonitor, SIGNAL(fileChanged(const QString &)), this, SLOT(dataChanged()));

    d->m_itemTransformer.setManagerUri(managerUri());
    d->m_asynchProcess = new OrganizerAsynchProcess(this);
}

void QOrganizerItemMaemo5Engine::dataChanged()
{
    // Timer prevents from sending multiple signals
    // when database is changed during short period of time
    if (!m_waitTimer.isActive()) {
        m_waitTimer.setSingleShot(true);
        m_waitTimer.setInterval(50);
        m_waitTimer.start();
        emit dataChanged();
    }
}

QOrganizerItemMaemo5Engine::~QOrganizerItemMaemo5Engine()
{
    delete d->m_asynchProcess;
}

QString QOrganizerItemMaemo5Engine::managerName() const
{
    return QLatin1String("maemo5");
}

QMap<QString, QString> QOrganizerItemMaemo5Engine::managerParameters() const
{
    return QMap<QString, QString>();
}

int QOrganizerItemMaemo5Engine::managerVersion() const
{
    return 1;
}

QList<QOrganizerItem> QOrganizerItemMaemo5Engine::itemInstances(const QOrganizerItem &generator, const QDateTime &periodStart, const QDateTime &periodEnd, int maxCount, QOrganizerItemManager::Error *error) const
{
    //qDebug() << "itemInstances(" << generator.localId() << ", " << periodStart << ", " << periodEnd << ", " << maxCount << ")";

    *error = QOrganizerItemManager::NoError;
    int calError = CALENDAR_OPERATION_SUCCESSFUL;
    QList<QOrganizerItem> retn;

    if (periodStart > periodEnd)
    {
        *error = QOrganizerItemManager::BadArgumentError;
        return retn;
    }

    CCalendar *cal = d->m_mcInstance->getDefaultCalendar();

    std::string nativeId = QString::number(generator.localId()).toStdString();

    if (generator.type() == QOrganizerItemType::TypeEvent)
    {
        CEvent *cevent = cal->getEvent(nativeId, calError);
        *error = d->m_itemTransformer.calErrorToManagerError(calError);
        if (cevent && *error == QOrganizerItemManager::NoError)
        {
            // Get event instance times
            std::vector< std::time_t > eventInstanceDates;
            cevent->generateInstanceTimes(periodStart.toTime_t(), periodEnd.toTime_t(), eventInstanceDates);

            // Calculate the generator event duration (the occurrences will have the same duration)
            time_t generatorDuration = cevent->getDateEnd() - cevent->getDateStart();

            // Generate the event occurrences
            std::vector< std::time_t >::const_iterator i;
            for (i = eventInstanceDates.begin(); i != eventInstanceDates.end(); ++i)
            {
                QDateTime instanceStartDate = QDateTime::fromTime_t(*i);
                QDateTime instanceEndDate = QDateTime::fromTime_t(*i + generatorDuration);

                // Ensure that the instance is within the period.
                // CEvent::generateInstanceTimes seems to sometime return erroneous dates.

                // NOTE: If this implementation have to be changed so that only items
                // which completely fit inside the period will be returned, it affects to
                // other parts of code as well. Especially event occurrence saving and
                // parent resolving must be verified to work.
                if (instanceStartDate >= periodStart && instanceStartDate <= periodEnd) {
                    QOrganizerEventOccurrence eventOcc =
                            d->m_itemTransformer.convertCEventToQEventOccurrence(cevent, instanceStartDate, instanceEndDate);
                    d->m_itemTransformer.fillInCommonCComponentDetails(&eventOcc, cevent, false); // false = do not set local ids
                    retn << eventOcc;
                }
            }

            // Now we have got the simple occurrences that are generated with the recurrence rules.
            // Events can have also occurrence that are not generated with rules, but saved as
            // events (because they have become modified). Those occurrences are saved with GUID set
            // equal to the generator event's GUID.
            QString eventType = QOrganizerItemType::TypeEvent;
            std::vector<CEvent*> occurrenceCandidates = cal->getEvents(cevent->getGUid(), calError);
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            if (*error == QOrganizerItemManager::NoError) {
                std::vector<CEvent*>::iterator occCand;
                for (occCand = occurrenceCandidates.begin(); occCand != occurrenceCandidates.end(); ++occCand) {
                    CEvent* coccurrenceCandidate = *occCand;
                    if (coccurrenceCandidate && coccurrenceCandidate->getId() != cevent->getId()) {
                        // for all other events than the generator itself
                        QDateTime instanceStartDate = QDateTime::fromTime_t(coccurrenceCandidate->getDateStart());
                        QDateTime instanceEndDate = QDateTime::fromTime_t(coccurrenceCandidate->getDateEnd());
                        QString idString = QString::fromStdString(cevent->getId());
                        QOrganizerItemLocalId parentLocalId = idString.toUInt();

                        // instance must be within the period
                        // NOTE: If this implementation have to be changed so that only items
                        // which completely fit inside the period will be returned, it affects to
                        // other parts of code as well. Especially event occurrence saving and
                        // parent resolving must be verified to work.
                        if (instanceStartDate >= periodStart && instanceStartDate <= periodEnd) {
                            if (isOccurrence(cal, coccurrenceCandidate, eventType, error)) {
                                if (*error == QOrganizerItemManager::NoError) {
                                    QOrganizerEventOccurrence eventOcc =
                                            d->m_itemTransformer.convertCEventToQEventOccurrence(coccurrenceCandidate, instanceStartDate, instanceEndDate, parentLocalId);
                                    d->m_itemTransformer.fillInCommonCComponentDetails(&eventOcc, coccurrenceCandidate);

                                    // insert occurrence to the result list in right position
                                    insertOccurenceSortedByStartDate(&eventOcc, retn);
                                }
                            }
                        }
                    }
                    delete coccurrenceCandidate;
                }
            }

            // finally the result list might need to be resized
            if (maxCount > 0) {
                while (retn.size() > maxCount)
                    retn.removeLast();
            }
        }
    }
    else if (generator.type() == QOrganizerItemType::TypeTodo)
    {
        CTodo* ctodo = cal->getTodo(nativeId, calError);
        *error = d->m_itemTransformer.calErrorToManagerError(calError);
        if (ctodo && *error == QOrganizerItemManager::NoError)
        {
            // In Maemo5, only one occurrence per todo. It has the same id as the parent
            QDateTime instanceStartDate = QDateTime::fromTime_t(ctodo->getDateStart());
            QDateTime instanceDueDate = QDateTime::fromTime_t(ctodo->getDue());
            if ((instanceStartDate >= periodStart && instanceStartDate <= periodEnd)
                || (instanceDueDate >= periodStart && instanceDueDate <= periodEnd)) {
                // Instance start time or instance due time is in period
                QOrganizerTodoOccurrence todoOcc = d->m_itemTransformer.convertCTodoToQTodoOccurrence(ctodo);
                d->m_itemTransformer.fillInCommonCComponentDetails(&todoOcc, ctodo);
                retn << todoOcc;
            }
        }
        delete ctodo;
    }
    else
    {
        // Other item types can't have occurrences
        *error = QOrganizerItemManager::BadArgumentError;
    }

    cleanupCal( cal );


    return retn;
}

QList<QOrganizerItemLocalId> QOrganizerItemMaemo5Engine::itemIds(const QOrganizerItemFilter &filter, const QList<QOrganizerItemSortOrder> &sortOrders, QOrganizerItemManager::Error *error) const
{
    // TODO: Make this method to use filters and sortorders

    *error = QOrganizerItemManager::NoError;
    int calError = CALENDAR_OPERATION_SUCCESSFUL;
    QList<QOrganizerItemLocalId> retn;
    CCalendar* cal = d->m_mcInstance->getDefaultCalendar();

    std::vector<std::string>::const_iterator id;

    // Append event ids
    std::vector<std::string> eventIds = cal->getIdList(E_EVENT, calError);
    *error = d->m_itemTransformer.calErrorToManagerError(calError);
    if (*error != QOrganizerItemManager::NoError)
        return retn;
    for (id = eventIds.begin(); id != eventIds.end(); ++id)
        retn << QString::fromStdString(*id).toUInt();

    // Append todo ids
    std::vector<std::string> todoIds = cal->getIdList(E_TODO, calError);
    *error = d->m_itemTransformer.calErrorToManagerError(calError);
    if (*error != QOrganizerItemManager::NoError)
        return retn;
    for (id = todoIds.begin(); id != todoIds.end(); ++id)
        retn << QString::fromStdString(*id).toUInt();

    // Append journal ids
    std::vector<std::string> journalIds = cal->getIdList(E_JOURNAL, calError);
    *error = d->m_itemTransformer.calErrorToManagerError(calError);
    if (*error != QOrganizerItemManager::NoError)
        return retn;
    for (id = journalIds.begin(); id != journalIds.end(); ++id)
        retn << QString::fromStdString(*id).toUInt();

    return retn;


    /*
        TODO

        Given the supplied filter and sort order, fetch the list of items [not instances] that correspond, and return their ids.

        If you don't support the filter or sort orders, you can fetch a partially (or un-) filtered list and ask the helper
        functions to filter and sort it for you.

        If you do have to fetch, consider setting a fetch hint that restricts the information to that needed for filtering/sorting.
    */
/*
    *error = QOrganizerItemManager::NotSupportedError; // TODO <- remove this

    QList<QOrganizerItem> partiallyFilteredItems; // = ..., your code here.. [TODO]
    QList<QOrganizerItem> retn;

    foreach(const QOrganizerItem& item, partiallyFilteredItems) {
        if (QOrganizerItemManagerEngine::testFilter(filter, item)) {
            retn.append(item);
        }
    }

    return QOrganizerItemManagerEngine::sortItems(retn, sortOrders);
    */
}

QList<QOrganizerItem> QOrganizerItemMaemo5Engine::items(const QOrganizerItemFilter &filter, const QList<QOrganizerItemSortOrder> &sortOrders, const QOrganizerItemFetchHint &fetchHint, QOrganizerItemManager::Error *error) const
{
    *error = QOrganizerItemManager::NoError;
    QList<QOrganizerItem> retn;
    CCalendar* cal = d->m_mcInstance->getDefaultCalendar();

    QList<QOrganizerItemLocalId> ids = itemIds(filter, sortOrders, error);
    if (*error != QOrganizerItemManager::NoError)
        return QList<QOrganizerItem>();

    QList<QOrganizerItemLocalId>::const_iterator id;
    for (id = ids.constBegin(); id != ids.constEnd(); ++id)
        retn << internalFetchItem(*id, fetchHint, error, true);

    return retn;

    /* // Previous implementation without itemIds():
    std::vector<CEvent*> events = cal->getEvents(calError);
    *error = d->m_itemTransformer.calErrorToManagerError(calError);
    if (calError == CALENDAR_OPERATION_SUCCESSFUL) {
        std::vector<CEvent*>::const_iterator event;
        for (event = events.begin(); event != events.end(); ++event)
        {
            CEvent* cevent = *event;
            QOrganizerItemLocalId eventId = QString::fromStdString(cevent->getId()).toUInt();
            QOrganizerItem item = internalFetchItem(eventId, fetchMinimalData(), error, true);
            retn << item;
            delete cevent;
        }
    }

    std::vector<CTodo*> todos = cal->getTodos(calError);
    *error = d->m_itemTransformer.calErrorToManagerError(calError);
    if (calError == CALENDAR_OPERATION_SUCCESSFUL) {
        std::vector<CTodo*>::const_iterator todo;
        for (todo = todos.begin(); todo != todos.end(); ++todo)
        {
            CTodo* ctodo = *todo;
            QOrganizerItemLocalId todoId = QString::fromStdString(ctodo->getId()).toUInt();
            QOrganizerItem item = internalFetchItem(todoId, fetchMinimalData(), error, true);
            retn << item;
            delete ctodo;
        }
    }

    std::vector<CJournal*> journals = cal->getJournals(calError);
    *error = d->m_itemTransformer.calErrorToManagerError(calError);
    if (calError == CALENDAR_OPERATION_SUCCESSFUL) {
        std::vector<CJournal*>::const_iterator journal;
        for (journal = journals.begin(); journal != journals.end(); ++journal)
        {
            CJournal* cjournal = *journal;
            QOrganizerItemLocalId journalId = QString::fromStdString(cjournal->getId()).toUInt();
            QOrganizerItem item = internalFetchItem(journalId, fetchMinimalData(), error, true);
            retn << item;
            delete cjournal;
        }
    }

    cleanupCal(cal);
    return retn;
    */
}

QOrganizerItem QOrganizerItemMaemo5Engine::item(const QOrganizerItemLocalId &itemId, const QOrganizerItemFetchHint &fetchHint, QOrganizerItemManager::Error *error) const
{
    return internalFetchItem(itemId, fetchHint, error, true);
}

bool QOrganizerItemMaemo5Engine::saveItems(QList<QOrganizerItem> *items, QMap<int, QOrganizerItemManager::Error> *errorMap, QOrganizerItemManager::Error* error)
{
    // TODO: Add changeset manipulation and signal emissions

    if (!items) {
        *error = QOrganizerItemManager::BadArgumentError;
        return false;
    }

    *error = QOrganizerItemManager::NoError;
    CCalendar *cal = d->m_mcInstance->getDefaultCalendar();
    bool success = true;
    QOrganizerItemChangeSet cs;

    for (int i = 0; i < items->size(); ++i) {
        QOrganizerItem curr = items->at(i);
        int calError = doSaveItem(cal, &curr, cs, error);

        if (calError != CALENDAR_OPERATION_SUCCESSFUL || *error != QOrganizerItemManager::NoError) {
            qDebug() << "SAVE FAILED BECAUSE:";
            qDebug() << calError;
            qDebug() << *error;
            success = false;
            curr.setId(QOrganizerItemId()); // clear ID
            if (errorMap) {
                if (*error != QOrganizerItemManager::NoError)
                    errorMap->insert(i, *error);
                else
                    errorMap->insert(i, QOrganizerItemManager::UnspecifiedError);
            }
        }

        items->replace(i, curr);
    }

    d->m_mcInstance->commitAllChanges(); // ensure that changes are committed before emitting signals
    cs.emitSignals(this);
    cleanupCal(cal);
    return success;
}

bool QOrganizerItemMaemo5Engine::removeItems(const QList<QOrganizerItemLocalId> &itemIds, QMap<int, QOrganizerItemManager::Error> *errorMap, QOrganizerItemManager::Error *error)
{
    // TODO: Remove the commented out test&debug codes when not needed anymore:

    /*
    QOrganizerItemFetchRequest *r = new QOrganizerItemFetchRequest(0);
    startRequest(r);

    while(r->state() != QOrganizerItemAbstractRequest::ActiveState) {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    }

    d->m_asynchProcess->requestDestroyed(r);

    QOrganizerItemFetchRequest *r2 = new QOrganizerItemFetchRequest(0);
    startRequest(r2);
    */

    /*
    CCalendar* cal = d->m_mcInstance->getDefaultCalendar();
    CTodo *ctodo = new CTodo();
    ctodo->setDue(QDateTime(QDate(2010,7,15),QTime(12,0,0)).toTime_t());
    ctodo->setSummary(QString("Todo summary").toStdString());

    CEvent *cevent = new CEvent();
    cevent->setDateStart(QDateTime(QDate(2010,7,17),QTime(12,0,0)).toTime_t());
    cevent->setDateEnd(QDateTime(QDate(2010,7,17),QTime(14,0,0)).toTime_t());
    cevent->setSummary(QString("Event summary").toStdString());

    CRecurrence* rec = new CRecurrence();
    rec->setRtype(1); // daily
    std::vector< CRecurrenceRule* > rules;
    CRecurrenceRule* rrule = new CRecurrenceRule();
    rrule->setRuleType(RECURRENCE_RULE);
    rrule->setRrule(QString("FREQ=DAILY;COUNT=5;INTERVAL=1").toStdString());
    rules.push_back(rrule);
    rec->setRecurrenceRule(rules);
    ctodo->setRecurrence(rec);

    int calError;
    cal->addTodo(ctodo,calError);
    cal->addEvent(cevent,calError);


    return true;
    */

    /*
    CCalendar* cal = d->m_mcInstance->getDefaultCalendar();

    int calError;
    std::vector<CTodo*> evs = cal->getTodos(QString("report").toStdString(), calError);
    for( int i = 0; i < evs.size(); ++i) {
        qDebug() << QString::fromStdString(evs[i]->getId()) << ", " << QString::fromStdString(evs[i]->getGUid()) << ", " << QString::fromStdString(evs[i]->getSummary());
        delete evs[i];
    }
    */

    /*
    CTodo *ctodo = new CTodo();
    ctodo->setSummary(QString("New todo summary").toStdString());
    ctodo->setGUid(QString("dfgdgfdsfsdfgdf").toStdString());
    ctodo->setDateStart(QDateTime(QDate(2010,7,14),QTime(12,0,0)).toTime_t());
    ctodo->setDateEnd(QDateTime(QDate(2010,7,14),QTime(13,0,0)).toTime_t());
    int calError;
    cal->addTodo(ctodo, calError);
    qDebug() << "Todo added, id = " << QString::fromStdString(ctodo->getId());
    qDebug() << "Cal error = " << calError;

    CEvent *fetch = cal->getEventOrNull(ctodo->getId(), calError);
    if (fetch)
        qDebug() << "Managed to fetch, summary = " << QString::fromStdString(fetch->getSummary());
    else
        qDebug() << "Returned null";

        */
    //return true;


    // TODO: How should the errormap updated for the child events?
    *error = QOrganizerItemManager::NoError;
    CCalendar* cal = d->m_mcInstance->getDefaultCalendar();
    bool success = true;
    QOrganizerItemChangeSet cs;

    QList<QOrganizerItemLocalId> idsToDelete = itemIds;
    int i = 0;
    while (i < idsToDelete.size()) {
        QOrganizerItemLocalId currId = idsToDelete.at(i);

        QOrganizerItem currItem = internalFetchItem(currId, fetchMinimalData(), error, false);
        if (*error == QOrganizerItemManager::NoError) {
            // Item exists
            QString itemId = QString::number(currItem.localId());
            int calError = CALENDAR_OPERATION_SUCCESSFUL;
            if (currItem.type() == QOrganizerItemType::TypeEvent) {
                // Delete also child events if this event is a parent
                CEvent* cevent = cal->getEvent(itemId.toStdString(), calError);
                if (calError == CALENDAR_OPERATION_SUCCESSFUL) {
                    bool parentItem = isParent(cal, cevent, QOrganizerItemType::TypeEvent, error);
                    if (parentItem && *error == QOrganizerItemManager::NoError) {
                        std::vector<CEvent*> eventsWithGuid = cal->getEvents(cevent->getGUid(), calError);
                        *error = d->m_itemTransformer.calErrorToManagerError(calError);
                        if (calError == CALENDAR_OPERATION_SUCCESSFUL) {
                            std::vector<CEvent*>::const_iterator childEvent;
                            for (childEvent = eventsWithGuid.begin(); childEvent != eventsWithGuid.end(); ++childEvent) {
                                QOrganizerItemLocalId childId = QString::fromStdString((*childEvent)->getId()).toUInt();
                                if (!idsToDelete.contains(childId))
                                    idsToDelete << childId;
                                delete *childEvent;
                            }
                        }
                    }
                    delete cevent;
                }
            }
            *error = d->m_itemTransformer.calErrorToManagerError(calError);

            if (calError == CALENDAR_OPERATION_SUCCESSFUL) {
                // Delete a component:
                cal->deleteComponent(itemId.toStdString(), calError);
                *error = d->m_itemTransformer.calErrorToManagerError(calError);

                if (calError == CALENDAR_OPERATION_SUCCESSFUL) {
                    // Success, update the changeset
                    qDebug() << "DELETED " << itemId;
                    cs.insertRemovedItem(currItem.localId());
                }
                else {
                    success = false;
                    if (errorMap)
                        errorMap->insert(i, *error);
                }
            }
            else {
                success = false;
                if (errorMap)
                    errorMap->insert(i, *error);
            }
        }
        else {
            success = false;
            if (errorMap)
                errorMap->insert(i, *error);
        }
        ++i;
    }

    d->m_mcInstance->commitAllChanges(); // ensure that changes are committed before emitting signals
    cs.emitSignals(this);
    cleanupCal(cal);
    return success;
}

bool QOrganizerItemMaemo5Engine::startRequest(QOrganizerItemAbstractRequest* req)
{
    qDebug() << "startRequest() run in thread " << (int) QThread::currentThreadId();
    d->m_asynchProcess->addRequest(req);
    QMetaObject::invokeMethod(d->m_asynchProcess, "processRequest", Qt::QueuedConnection);
    return true; // TODO: Is this ok?



    /*
        TODO

        This is the entry point to the async API.  The request object describes the
        type of request (switch on req->type()).  Req will not be null when called
        by the framework.

        Generally, you can queue the request and process them at some later time
        (probably in another thread).

        Once you start a request, call the updateRequestState and/or the
        specific updateXXXXXRequest functions to mark it in the active state.

        If your engine is particularly fast, or the operation involves only in
        memory data, you can process and complete the request here.  That is
        probably not the case, though.

        Note that when the client is threaded, and the request might live on a
        different thread, you might need to be careful with locking.  In particular,
        the request might be deleted while you are still working on it.  In this case,
        your requestDestroyed function will be called while the request is still valid,
        and you should block in that function until your worker thread (etc) has been
        notified not to touch that request any more.

        We plan to provide some boiler plate code that will allow you to:

        1) implement the sync functions, and have the async versions call the sync
           in another thread

        2) or implement the async versions of the function, and have the sync versions
           call the async versions.

        It's not ready yet, though.

        Return true if the request can be started, false otherwise.  You can set an error
        in the request if you like.

    return QOrganizerItemManagerEngine::startRequest(req);
    */
}

bool QOrganizerItemMaemo5Engine::cancelRequest(QOrganizerItemAbstractRequest* req)
{
    return d->m_asynchProcess->cancelRequest(req);

    /*
        TODO

        Cancel an in progress async request.  If not possible, return false from here.

    return QOrganizerItemManagerEngine::cancelRequest(req);*/
}

bool QOrganizerItemMaemo5Engine::waitForRequestFinished(QOrganizerItemAbstractRequest* req, int msecs)
{
    return d->m_asynchProcess->waitForRequestFinished(req, msecs);

    /*
        TODO

        Wait for a request to complete (up to a max of msecs milliseconds).

        Return true if the request is finished (including if it was already).  False otherwise.

        You should really implement this function, if nothing else than as a delay, since clients
        may call this in a loop.

        It's best to avoid processing events, if you can, or at least only process non-UI events.

    return QOrganizerItemManagerEngine::waitForRequestFinished(req, msecs);*/
}

void QOrganizerItemMaemo5Engine::requestDestroyed(QOrganizerItemAbstractRequest* req)
{
    return d->m_asynchProcess->requestDestroyed(req);

    /*
        TODO

        This is called when a request is being deleted.  It lets you know:

        1) the client doesn't care about the request any more.  You can still complete it if
           you feel like it.
        2) you can't reliably access any properties of the request pointer any more.  The pointer will
           be invalid once this function returns.

        This means that if you have a worker thread, you need to let that thread know that the
        request object is not valid and block until that thread acknowledges it.  One way to do this
        is to have a QSet<QOIAR*> (or QMap<QOIAR, MyCustomRequestState>) that tracks active requests, and
        insert into that set in startRequest, and remove in requestDestroyed (or when it finishes or is
        cancelled).  Protect that set/map with a mutex, and make sure you take the mutex in the worker
        thread before calling any of the QOIAR::updateXXXXXXRequest functions.  And be careful of lock
        ordering problems :D


    return QOrganizerItemManagerEngine::requestDestroyed(req);*/
}

bool QOrganizerItemMaemo5Engine::hasFeature(QOrganizerItemManager::ManagerFeature feature, const QString &itemType) const
{
    // TODO - the answer to the question may depend on the type
    Q_UNUSED(itemType);
    switch(feature) {
        case QOrganizerItemManager::MutableDefinitions:
            // TODO If you support save/remove detail definition, return true
            return false;

        case QOrganizerItemManager::Anonymous:
            // TODO if this engine is anonymous (e.g. no other engine can share the data) return true
            // (mostly for an in memory engine)
            return false;
        case QOrganizerItemManager::ChangeLogs:
            // TODO if this engine supports filtering by last modified/created/removed timestamps, return true
            return false;
    }
    return false;
}

bool QOrganizerItemMaemo5Engine::isFilterSupported(const QOrganizerItemFilter &filter) const
{
    // TODO if you engine can natively support the filter, return true.  Otherwise you should emulate support in the item{Ids} functions.
    Q_UNUSED(filter);
    return false;
}

QList<QVariant::Type> QOrganizerItemMaemo5Engine::supportedDataTypes() const
{
    QList<QVariant::Type> retn;
    // TODO - tweak which data types this engine understands
    retn << QVariant::String;
    retn << QVariant::Date;
    retn << QVariant::DateTime;
    retn << QVariant::Time;

    return retn;
}

QStringList QOrganizerItemMaemo5Engine::supportedItemTypes() const
{
    // TODO - return which [predefined] types this engine supports
    QStringList retn;

    retn << QOrganizerItemType::TypeEvent;
    retn << QOrganizerItemType::TypeEventOccurrence;
    retn << QOrganizerItemType::TypeJournal;
    retn << QOrganizerItemType::TypeNote;
    retn << QOrganizerItemType::TypeTodo;
    retn << QOrganizerItemType::TypeTodoOccurrence;

    return retn;
}

void QOrganizerItemMaemo5Engine::checkItemIdValidity(CCalendar *cal, QOrganizerItem *checkItem, QOrganizerItemManager::Error *error)
{
    *error = QOrganizerItemManager::NoError;

    // Check local id
    if (checkItem->localId()) {
        // Don't allow saving with local id defined unless the item is from this manager.
        if (checkItem->id().managerUri() != managerUri()) {
            *error = QOrganizerItemManager::BadArgumentError;
            return;
        }
    }

    QOrganizerEventOccurrence *eventOccurrence(0);
    if (checkItem->type() == QOrganizerItemType::TypeEventOccurrence)
        eventOccurrence = static_cast<QOrganizerEventOccurrence *>(checkItem);
    QOrganizerTodoOccurrence *todoOccurrence(0);
    if (checkItem->type() == QOrganizerItemType::TypeTodoOccurrence)
        todoOccurrence = static_cast<QOrganizerTodoOccurrence *>(checkItem);

    // TODO: Maybe here should be added a GUID check: if item is NOT an occurrence, setting duplicate GUIDs should be disallowed

    // If the item is not an occurrence, no more checks are needed
    if (!eventOccurrence && !todoOccurrence)
        return;

    // Event occurrence validity checks:
    if (eventOccurrence) {
        // Either parent id or GUID (or both) must be set
        if (!eventOccurrence->parentLocalId() && eventOccurrence->guid().isEmpty()) {
            *error = QOrganizerItemManager::InvalidOccurrenceError;
            return;
        }

        // If the parent ID is set, it must point to an event
        QOrganizerItem parent;
        if (eventOccurrence->parentLocalId()) {
            parent = item(eventOccurrence->parentLocalId(), fetchMinimalData(), error);
            if (*error != QOrganizerItemManager::NoError)
                return;

            if (parent.type() != QOrganizerItemType::TypeEvent) {
                *error = QOrganizerItemManager::InvalidOccurrenceError;
                return;
            }
        }
        else {
            // The parent ID was not set, so the GUID is always set here.
            // Must find at least one event with the given GUID.
            int calError = CALENDAR_OPERATION_SUCCESSFUL;
            std::vector<CEvent*> eventsWithGuid = cal->getEvents(eventOccurrence->guid().toStdString(), calError);
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            int numberOfEvents = eventsWithGuid.size();
            for (int i = 0; i < numberOfEvents; ++i)
                delete eventsWithGuid[i];

            if (*error != QOrganizerItemManager::NoError)
                return;

            if (!numberOfEvents) {
                *error = QOrganizerItemManager::InvalidOccurrenceError;
                return;
            }
        }

        // If both parent ID and GUID are set, they must be consistent
        if (eventOccurrence->parentLocalId() && !eventOccurrence->guid().isEmpty()) {
            if (eventOccurrence->guid() != parent.guid()) {
                *error = QOrganizerItemManager::InvalidOccurrenceError;
                return;
            }
        }
    }

    // Todo occurrence validity checks:
    if (todoOccurrence) {
        // Either parent id or GUID (or both) must be set
        if (!todoOccurrence->parentLocalId() && todoOccurrence->guid().isEmpty()) {
            *error = QOrganizerItemManager::InvalidOccurrenceError;
            return;
        }

        if (todoOccurrence->parentLocalId()) {
            // Parent ID must point to this occurrence due to restrictions in Maemo5 todos
            if (todoOccurrence->parentLocalId() != todoOccurrence->localId()) {
                *error = QOrganizerItemManager::InvalidOccurrenceError;
                return;
            }
        }
        else {
            qDebug() << "Guid check";
            // Must find at least one todo with the GUID from the DB
            int calError = CALENDAR_OPERATION_SUCCESSFUL;
            std::vector<CTodo*> todosWithGuid = cal->getTodos(todoOccurrence->guid().toStdString(), calError);
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            int numberOfTodos = todosWithGuid.size();
            qDebug() << "Number of todos = " << numberOfTodos;
            for (int i = 0; i < numberOfTodos; ++i)
                delete todosWithGuid[i];

            if (*error != QOrganizerItemManager::NoError)
                return;

            if (!numberOfTodos) {
                *error = QOrganizerItemManager::InvalidOccurrenceError;
                return;
            }
        }
    }
}

int QOrganizerItemMaemo5Engine::doSaveItem(CCalendar *cal, QOrganizerItem *item, QOrganizerItemChangeSet &cs, QOrganizerItemManager::Error *error)
{
    qDebug() << "doSaveitem";
    qDebug() << "item type = " << item->type();
    qDebug() << "item label = " << item->displayLabel();
    int calError = CALENDAR_OPERATION_SUCCESSFUL;
    *error = QOrganizerItemManager::NoError;

    // Check item validity
    checkItemIdValidity(cal, item, error);
    if (*error != QOrganizerItemManager::NoError)
        return calError;

    qDebug() << "validity check passed";

    // Returns InvalidItemTypeError if the type won't be recognized later
    *error = QOrganizerItemManager::InvalidItemTypeError;

    CComponent *component = d->m_itemTransformer.createCComponent(cal, item, error);
    if (!component || *error != QOrganizerItemManager::NoError) {
        delete component;
        return calError;
    }

    // Read the given GUID
    item->setGuid(QString::fromStdString(component->getGUid()));

    // TODO: The custom detail fields should be iterated and the corresponding
    // fields should be set to CComponent for saving.

    if (item->type() == QOrganizerItemType::TypeEvent) {
        qDebug() << "Event type recognized";
        CEvent* cevent = static_cast<CEvent *>(component);
        QString ceventId = QString::fromStdString(cevent->getId());
        if (!ceventId.isEmpty()) {
            // CEvent ID is not empty, the event already exists in calendar
            cal->modifyEvent(cevent, calError);
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            if (*error == QOrganizerItemManager::NoError)
                cs.insertChangedItem(item->localId());
            qDebug() << "****** Modified event" << ceventId;
        }
        else {
            // CEvent ID is empty, the event is new
            cal->addEvent(cevent, calError);
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            if (calError == CALENDAR_OPERATION_SUCCESSFUL || calError == CALENDAR_ENTRY_DUPLICATED)
                // The Maemo5 calendar does not accept two items if all the
                // time details are equal. That's so even if the item IDs differ.
                // If the "new" item is actually a duplicate, then let
                // the calendar to modify the existing item.
                // TODO: Is that what we want?
            {
                // Set id for the event
                QString newIdString = QString::fromStdString(cevent->getId());
                QOrganizerItemLocalId newId = newIdString.toUInt();
                QOrganizerItemId id;
                id.setLocalId(newId);
                id.setManagerUri(managerUri());
                item->setId(id);

                // Update changeset
                if (calError == CALENDAR_ENTRY_DUPLICATED)
                    cs.insertChangedItem(item->localId());
                else
                    cs.insertAddedItem(item->localId());

                calError = CALENDAR_OPERATION_SUCCESSFUL; // reset the error
                *error = QOrganizerItemManager::NoError; // reset the error

                qDebug() << "****** Added event" << newIdString;
            }
        }

        delete cevent;
        qDebug() << "doSaveItem: event saving ended with return value " << calError;
        return calError;
    }
    else if (item->type() == QOrganizerItemType::TypeEventOccurrence)
    {
        qDebug() << "Event occurrence type recognized";
        QOrganizerEventOccurrence *eventOccurrence = static_cast<QOrganizerEventOccurrence *>(item);

        QOrganizerItem parentItem = parentOf(cal, eventOccurrence, error);
        if (*error == QOrganizerItemManager::NoError) {
            //qDebug() << "Parent fetch ok";

            if (parentItem.localId()) {
                //qDebug() << "Parent found";
                // parent item found, cast it to event
                QOrganizerEvent *parentEvent = static_cast<QOrganizerEvent *>(&parentItem);

                // save the event occurrence
                calError = saveEventOccurrence(cal, eventOccurrence, parentEvent, cs, error);
                *error = d->m_itemTransformer.calErrorToManagerError(calError);
            }
            else {
                // Event occurrence without a parent item fails,
                // but this should not happen after passing the validity check...
                *error = QOrganizerItemManager::InvalidOccurrenceError;
            }
        }

        qDebug() << "doSaveItem: event occurrence saving ended with return value " << calError;
        return calError;
    }
    else if (item->type() == QOrganizerItemType::TypeTodo || item->type() == QOrganizerItemType::TypeTodoOccurrence) {
        qDebug() << "Todo or todo occurrence type recognized";
        CTodo* ctodo = static_cast<CTodo *>(component);
        QString ctodoId = QString::fromStdString(ctodo->getId());

        // A todo occurrence can never be a new item. If local id is empty, set it according
        // to the GUID.
        if (item->type() == QOrganizerItemType::TypeTodoOccurrence && ctodoId.isEmpty()) {
            calError = CALENDAR_OPERATION_SUCCESSFUL;
            std::vector<CTodo*> todosWithGuid = cal->getTodos(item->guid().toStdString(), calError);
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            if (calError != CALENDAR_OPERATION_SUCCESSFUL) {
                delete ctodo;
                return calError;
            }
            ctodo->setId(todosWithGuid[0]->getId()); // this is safe as validity check has passed
            ctodoId = QString::fromStdString(ctodo->getId());
            int count = todosWithGuid.size();
            for (int i = 0; i < count; ++i)
                delete todosWithGuid[i];
        }

        if (!ctodoId.isEmpty()) {
            // CTodo ID is not empty, the todo already exists in calendar
            cal->modifyTodo(ctodo, calError);
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            if (*error == QOrganizerItemManager::NoError)
                cs.insertChangedItem(item->localId());
            qDebug() << "****** Modified todo" << ctodoId;
        }
        else {
            // CTodo ID is empty, the todo is new
            cal->addTodo(ctodo, calError);
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            if (calError == CALENDAR_OPERATION_SUCCESSFUL || calError == CALENDAR_ENTRY_DUPLICATED)
                // The Maemo5 calendar does not accept two items if all the
                // time details are equal. That's so even if the item IDs differ.
                // If the "new" item is actually a duplicate, then let
                // the calendar to modify the existing item.
                // TODO: Is that what we want?
            {
                // Set id for the todo
                QString newIdString = QString::fromStdString(ctodo->getId());
                QOrganizerItemLocalId newId = newIdString.toUInt();
                QOrganizerItemId id;
                id.setLocalId(newId);
                id.setManagerUri(managerUri());
                item->setId(id);

                // Update changeset
                if (calError == CALENDAR_ENTRY_DUPLICATED)
                    cs.insertChangedItem(item->localId());
                else
                    cs.insertAddedItem(item->localId());

                calError = CALENDAR_OPERATION_SUCCESSFUL; // reset the error
                *error = QOrganizerItemManager::NoError; // reset the error

                qDebug() << "****** Added todo " << newIdString;
            }
        }

        delete ctodo;
        return calError;
    }
    else if (item->type() == QOrganizerItemType::TypeJournal) {
        CJournal* cjournal = static_cast<CJournal *>(component);
        QString cjournalId = QString::fromStdString(cjournal->getId());
        if (!cjournalId.isEmpty()) {
            // CJournal ID is not empty, the journal already exists in calendar
            cal->modifyJournal(cjournal, calError);
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            if (*error == QOrganizerItemManager::NoError)
                cs.insertChangedItem(item->localId());
        }
        else {
            // CJournal ID is empty, the journal is new
            cal->addJournal(cjournal, calError);
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            if (calError == CALENDAR_OPERATION_SUCCESSFUL || calError == CALENDAR_ENTRY_DUPLICATED)
                // The Maemo5 calendar does not accept two items if all the
                // time details are equal. That's so even if the item IDs differ.
                // If the "new" item is actually a duplicate, then let
                // the calendar to modify the existing item.
                // TODO: Is that what we want?
            {
                // Set id for the journal
                QString newIdString = QString::fromStdString(cjournal->getId());
                QOrganizerItemLocalId newId = newIdString.toUInt();
                QOrganizerItemId id;
                id.setLocalId(newId);
                id.setManagerUri(managerUri());
                item->setId(id);

                // Update changeset
                if (calError == CALENDAR_ENTRY_DUPLICATED)
                    cs.insertChangedItem(item->localId());
                else
                    cs.insertAddedItem(item->localId());

                calError = CALENDAR_OPERATION_SUCCESSFUL; // reset the error
                *error = QOrganizerItemManager::NoError; // reset the error
            }
        }

        delete cjournal;
        return calError;
    }
    else if (item->type() == QOrganizerItemType::TypeNote) {
        // TODO
    }

    qDebug() << "doSaveItem: went to end, return value = " << calError;
    return calError;
}

int QOrganizerItemMaemo5Engine::saveEventOccurrence(CCalendar *cal, QOrganizerEventOccurrence *occurrence, QOrganizerEvent *parent, QOrganizerItemChangeSet &cs, QOrganizerItemManager::Error *error)
{
    qDebug() << "saveEventOccurrence()";
    int calError = CALENDAR_OPERATION_SUCCESSFUL;

    // set occurrence GUID equal to the parent GUID
    occurrence->setGuid(parent->guid());

    // set occurrence's parent id
    occurrence->setParentLocalId(parent->localId());

    // backup the parent item
    QOrganizerEvent parentBackup(*parent);

    // did parent become modified
    bool parentModified = false;

    if (!occurrence->localId()) {
        qDebug() << "No occurrence local id";
        // Does not contain a local id => this is a generated occurrence or a new exception occurrence
        // After modifications this won't be saved anymore as a generated occurrence
        // but as an event to the calendar DB. Add an exception rule to the parent item
        // to leave out this one when generating occurrences.

        if (occurrence->originalDate().isValid()) {
            qDebug() << "Original date exists = " << occurrence->originalDate();
            // The original date is valid => this is a generated occurrence
            // First solve the original occurrence, using this occurrence's original date.
            // We need that to know the original length of the generated occurrence
            // (as it may have changed now).
            QDateTime originalPeriodStart = QDateTime(occurrence->originalDate(), QTime(0,0,0));
            QDateTime originalPeriodEnd = QDateTime(occurrence->originalDate(), QTime(23,59,59,999));
            qDebug() << "Find parent's occurrences in period " << originalPeriodStart << " - " << originalPeriodEnd;
            QList<QOrganizerItem> parentsOccurrences = itemInstances( *parent, originalPeriodStart, originalPeriodEnd, 0, error);

            if (!parentsOccurrences.isEmpty()) {
                qDebug() << "Parent's occurrences found";
                QOrganizerEventOccurrence originalOccurrence = static_cast<QOrganizerEventOccurrence>(parentsOccurrences[0]);

                // Add exception dates to the parent
                QDate periodStart = originalOccurrence.startDateTime().date();
                QDate periodEnd = originalOccurrence.endDateTime().date();

                qDebug() << "To add = " << periodStart << " to " << periodEnd;

                QList<QDate> newExceptionDates = parent->exceptionDates();
                QDate exceptionDate = periodStart;
                while (exceptionDate <= periodEnd) {
                    qDebug() << "Added exception date: " << exceptionDate;
                    if (newExceptionDates.indexOf(exceptionDate) == -1)
                        newExceptionDates << exceptionDate;
                    exceptionDate = exceptionDate.addDays(1);
                }
                parent->setExceptionDates(newExceptionDates);
                parentModified = true;
            }
        }
        else {
            qDebug() << "No original date";
            // The original date is not valid => this is a new exception occurrence, not generated
            // Add exception days covering the occurrence's period
            QDate periodStart = occurrence->startDateTime().date();
            QDate periodEnd = occurrence->endDateTime().date();

            qDebug() << "To add = " << periodStart << " to " << periodEnd;

            QList<QDate> newExceptionDates = parent->exceptionDates();
            QDate exceptionDate = periodStart;
            while (exceptionDate <= periodEnd) {
                qDebug() << "Added exception date: " << exceptionDate;
                if (newExceptionDates.indexOf(exceptionDate) == -1)
                    newExceptionDates << exceptionDate;
                exceptionDate = exceptionDate.addDays(1);
            }
            parent->setExceptionDates(newExceptionDates);
            parentModified = true;
        }
    }

    // Create a CComponent from occurrence
    // This is done before saving the parent, so we don't have to rollback
    // the parent changes if the component creation fails.
    calError = CALENDAR_OPERATION_SUCCESSFUL;
    CComponent *component = d->m_itemTransformer.createCComponent(cal, occurrence, error);
    qDebug() << "main, 1053, error = " << *error;
    if (!component) {
        *error = QOrganizerItemManager::UnspecifiedError;
        return calError;
    }
    if (*error != QOrganizerItemManager::NoError) {
        delete component;
        return calError;
    }
    // TODO: The custom detail fields should be iterated and the corresponding
    // fields should be set to CComponent for saving.

    // save the parent back to the DB (if needed)
    if (parentModified) {
        calError = doSaveItem(cal, parent, cs, error);

        if (calError != CALENDAR_OPERATION_SUCCESSFUL) {
            qDebug() << "saving parent modification failed";
            // saving the parent modifications failed
            // we must not save the occurrence either
            delete component;
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            return calError;
        }
        if (*error != QOrganizerItemManager::NoError) {
            qDebug() << "saving parent modification failed";
            // saving the parent modifications failed
            // we must not save the occurrence either
            delete component;
            return calError;
        }
    }

    /*
    qDebug() << "Occurrence id: " << occurrence->id();
    qDebug() << "Occurrence guid: " << occurrence->guid();
    qDebug() << "Occurrence display label: " << occurrence->displayLabel();
    qDebug() << "Occurrence desc: " << occurrence->description();
    qDebug() << "Occurrence start date: " << occurrence->startDateTime();
    qDebug() << "Occurrence end date: " << occurrence->endDateTime();
    */

    // Save occurrence
    CEvent* cevent = static_cast<CEvent *>(component);
    QString ceventId = QString::fromStdString(cevent->getId());
    if (!ceventId.isEmpty()) {
        // CEvent ID is not empty, the event already exists in calendar

        qDebug() << "occurrence, modify:";

        cal->modifyEvent(cevent, calError);
        qDebug() << "cal error = " << calError;
        *error = d->m_itemTransformer.calErrorToManagerError(calError);
        if (calError != CALENDAR_OPERATION_SUCCESSFUL && parentModified) {
            // occurrence saving failed, we must undo the parent changes too
            QOrganizerItemManager::Error ignoreError;
            doSaveItem(cal, &parentBackup, cs, &ignoreError);
        }
    }
    else {
        // CEvent ID is empty, the event is new

        qDebug() << "occurrence, add new:";

        cal->addEvent(cevent, calError);
        qDebug() << "cal error = " << calError;
        *error = d->m_itemTransformer.calErrorToManagerError(calError);

        if (calError == CALENDAR_OPERATION_SUCCESSFUL || calError == CALENDAR_ENTRY_DUPLICATED)
        {
            // The Maemo5 calendar does not accept two items if all the
            // time details are equal. That's so even if the item IDs differ.
            // If the "new" item is actually a duplicate, then let
            // the calendar to modify the existing item.
            // TODO: Is that what we want?

            // Set id for the occurrence
            QString newIdString = QString::fromStdString(cevent->getId());
            QOrganizerItemLocalId newId = newIdString.toUInt();
            QOrganizerItemId id;
            id.setLocalId(newId);
            id.setManagerUri(managerUri());
            occurrence->setId(id);

            qDebug() << "new occurrence saved with id = " << newIdString;

            /*
            // Update changeset
            if (calError == CALENDAR_ENTRY_DUPLICATED)
                cs.insertChangedItem(item->localId());
            else
                cs.insertAddedItem(item->localId());
                */

            calError = CALENDAR_OPERATION_SUCCESSFUL; // reset the error
            *error = QOrganizerItemManager::NoError; // reset the error
        }
        else if (parentModified) {
            // occurrence saving failed, we must undo the parent changes too
            QOrganizerItemManager::Error ignoreError;
            doSaveItem(cal, &parentBackup, cs, &ignoreError);
        }
    }

    delete cevent;
    cevent = 0;

    return calError;
}

void QOrganizerItemMaemo5Engine::insertOccurenceSortedByStartDate(QOrganizerItem *occurrence, QList<QOrganizerItem> &target) const
{
    if (occurrence->type() == QOrganizerItemType::TypeEventOccurrence)
    {
        QOrganizerEventOccurrence *occ = static_cast<QOrganizerEventOccurrence *>(occurrence);
        int index = 0;
        while (index < target.count()) {
            QOrganizerEventOccurrence curr = static_cast<QOrganizerEventOccurrence>(target[index]);
            if (curr.startDateTime() > occ->startDateTime())
                break;
            ++index;
        }
        target.insert(index, *occ);
    }
    /*
    else if (occurrence->type() == QOrganizerItemType::TypeTodoOccurrence)
    {
        QOrganizerTodoOccurrence *occ = static_cast<QOrganizerTodoOccurrence *>(occurrence);
        int index = 0;
        while (index < target.count()) {
            QOrganizerTodoOccurrence curr = static_cast<QOrganizerTodoOccurrence>(target[index]);
            if (curr.startDateTime() > occ->startDateTime())
                break;
            ++index;
        }
        target.insert(index, *occ);
    }*/
}

QOrganizerItem QOrganizerItemMaemo5Engine::parentOf(CCalendar *cal, QOrganizerItem *occurrence, QOrganizerItemManager::Error *error) const
{
    // the occurrence is supposed be valid when this method becomes called
    QOrganizerItemLocalId parentLocalId = 0;
    if (occurrence->type() == QOrganizerItemType::TypeEventOccurrence) {
        QOrganizerEventOccurrence *eventOccurrence = static_cast<QOrganizerEventOccurrence *>(occurrence);
        if (eventOccurrence->parentLocalId()) {
            // the simple case: parent local id was set
            parentLocalId = eventOccurrence->parentLocalId();
        }
        else {
            qDebug() << "GUID case";

            // parent local id was not set, fetch with [GUID,originalDate]
            int calError = CALENDAR_OPERATION_SUCCESSFUL;
            std::vector<CEvent*> parentCands = cal->getEvents(eventOccurrence->guid().toStdString(), calError);
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            std::vector<CEvent*>::iterator pCand;

            qDebug() << "Candidate count = " << parentCands.size();

            // Check if some event contains recurrence information and then that the given
            // occurrence is one of the occurrences obtained from the recurrence information.
            for (pCand = parentCands.begin(); pCand != parentCands.end(); ++pCand) {
                QOrganizerItemLocalId candidateLocalId = QString::fromStdString((*pCand)->getId()).toUInt();

                QOrganizerItem parentCandidate = internalFetchItem(candidateLocalId, fetchMinimalData(), error, false);
                if (*error == QOrganizerItemManager::NoError) {
                    // parent candidate must be an event here as events were requested from the GUID mapper
                    QOrganizerEvent *parentCandidateEvent = static_cast<QOrganizerEvent *>(&parentCandidate);

                    if (!parentCandidateEvent->recurrenceDates().isEmpty()
                        || !parentCandidateEvent->recurrenceRules().isEmpty()) {

                        if (eventOccurrence->originalDate() == QDate()) {
                            // The given occurrence is a new one, not previously saved in the DB.
                            // If there is an event with recurrence information, it must be the parent.
                            parentLocalId = candidateLocalId;
                            break; // parent event found
                        }
                        else {
                            // The given occurrence has existed before.
                            // Get the occurrences of the parent candidate event. If there's a match
                            // to our occurrence's originalDate, then we have found the parent.
                            QDateTime periodStart = QDateTime(eventOccurrence->originalDate(), QTime(0,0,0));
                            QDateTime periodEnd = QDateTime(eventOccurrence->originalDate(), QTime(23,59,59,999));

                            QList<QOrganizerItem> parentCandidateOccurrences =
                                    itemInstances(*parentCandidateEvent, periodStart, periodEnd, 0, error);
                            if (*error != QOrganizerItemManager::NoError)
                                return QOrganizerItem(); // error occured

                            //qDebug() << "Occurrences got";
                            //qDebug() << "Occ count = " << parentCandidateOccurrences.count();

                            // Because the period was set, it's enough to just check if we got any
                            if (!parentCandidateOccurrences.isEmpty()) {
                                parentLocalId = candidateLocalId;
                                break; // parent event found
                            }
                        }
                    }
                }
                else if (*error == QOrganizerItemManager::DoesNotExistError) {
                    // Tried to get an item that does not exist.
                    // It is not a serious error here, so just reset the error.
                    *error = QOrganizerItemManager::NoError;
                }
            }

            qDebug() << "parentLocalId = " << parentLocalId;

            if (parentLocalId)
            {
                // Parent local id got, just free the resources
                for (pCand = parentCands.begin(); pCand != parentCands.end(); ++pCand)
                    delete *pCand;
            }
            else {
                // None of the parent candidates contained recurrence information
                // => return the earliest created event as parent

                qDebug() << "Trying the resolve the first one";

                CEvent *firstCreated = 0;
                if (parentCands.size() > 0) {
                    firstCreated = parentCands[0];
                    for (pCand = parentCands.begin(); pCand != parentCands.end(); ++pCand) {
                        if ((*pCand)->getCreatedTime() < firstCreated->getCreatedTime()) {
                            firstCreated = *pCand;
                        }
                        else if((*pCand)->getCreatedTime() == firstCreated->getCreatedTime()
                            && (*pCand)->getId() < firstCreated->getId()) {
                            firstCreated = *pCand; // if two entries are created exactly at the same time, consider one with the smaller id as earlier
                        }
                    }
                }

                if (firstCreated) {
                    // the firstly created event resolved, return it as parent
                    parentLocalId = QString::fromStdString(firstCreated->getId()).toUInt();
                }

                // Free the resources
                for (pCand = parentCands.begin(); pCand != parentCands.end(); ++pCand)
                    delete *pCand;
            }
        }
    }
    else if (occurrence->type() == QOrganizerItemType::TypeTodoOccurrence) {
        // In Maemo5 parent of a todo occurrence is the occurrence itself
        parentLocalId = occurrence->localId();
    }
    else {
        return QOrganizerItem(); // other types can't have a parent
    }

    // Parent local id got, now fetch & return the parent
    return internalFetchItem(parentLocalId, fetchMinimalData(), error, false);
}

QOrganizerItem QOrganizerItemMaemo5Engine::internalFetchItem(const QOrganizerItemLocalId &itemId, const QOrganizerItemFetchHint &fetchHint, QOrganizerItemManager::Error *error, bool fetchOccurrences) const
{
    Q_UNUSED(fetchHint);
    // TODO: Make this method to use the fetch hint

    CCalendar *cal = d->m_mcInstance->getDefaultCalendar();
    std::string nativeId = QString::number(itemId).toStdString();
    int calError = CALENDAR_OPERATION_SUCCESSFUL;

    qDebug() << "item(): id to fetch = " << itemId;

    CEvent *cevent = cal->getEventOrNull(nativeId, calError);
    qDebug() << "cal error = " << calError;
    *error = d->m_itemTransformer.calErrorToManagerError(calError);
    if (cevent) {
        qDebug() << "display label = " << QString::fromStdString(cevent->getSummary());
        // first resolve is this event or an event occurrence (exception event)
        bool isOcc = false;
        if (fetchOccurrences) {
            QString type = QOrganizerItemType::TypeEvent;
            isOcc = isOccurrence(cal, cevent, type, error);
            if (*error != QOrganizerItemManager::NoError) {
                delete cevent;
                cleanupCal(cal);
                return QOrganizerItem();
            }
        }

        if (!isOcc) {
            qDebug() << "event";
            QOrganizerEvent retn = d->m_itemTransformer.convertCEventToQEvent(cevent);
            d->m_itemTransformer.fillInCommonCComponentDetails(&retn, cevent);
            delete cevent;
            cleanupCal(cal);
            return retn;
        }
        else {
            qDebug() << "exception event";

            QOrganizerEventOccurrence retn = d->m_itemTransformer.convertCEventToQEventOccurrence(cevent);
            d->m_itemTransformer.fillInCommonCComponentDetails(&retn, cevent);

            /*
            qDebug() << "Result event occurrence:";
            qDebug() << "Id: " << retn.localId();
            qDebug() << "Guid: " << retn.guid();
            qDebug() << "label: " << retn.displayLabel();
            qDebug() << "start: " << retn.startDateTime();
            qDebug() << "end: " << retn.endDateTime();
            qDebug() << "parent: " << retn.parentLocalId();
            */

            // resolve and set the parent event
            QOrganizerItem parent = parentOf(cal, &retn, error);
            if (*error != QOrganizerItemManager::NoError) {
                delete cevent;
                cleanupCal(cal);
                return QOrganizerItem();
            }
            retn.setParentLocalId(parent.localId());

            delete cevent;
            cleanupCal(cal);
            return retn;
        }
    }

    CTodo *todo = cal->getTodoOrNull(nativeId, calError);
    *error = d->m_itemTransformer.calErrorToManagerError(calError);
    if (todo) {
        qDebug() << "todo";
        QOrganizerTodo retn = d->m_itemTransformer.convertCTodoToQTodo(todo);
        d->m_itemTransformer.fillInCommonCComponentDetails(&retn, todo);
        delete todo;
        cleanupCal(cal);
        return retn;
    }

    CJournal *journal = cal->getJournalOrNull(nativeId, calError);
    *error = d->m_itemTransformer.calErrorToManagerError(calError);
    if (journal) {
        qDebug() << "journal";
        QOrganizerJournal retn = d->m_itemTransformer.convertCJournalToQJournal(journal);
        d->m_itemTransformer.fillInCommonCComponentDetails(&retn, journal);
        delete journal;
        cleanupCal(cal);
        return retn;
    }

    // In an error situation return an invalid item
    cleanupCal(cal);
    *error = QOrganizerItemManager::DoesNotExistError;
    qDebug() << "not exist";
    return QOrganizerItem();
}

bool QOrganizerItemMaemo5Engine::isOccurrence(CCalendar *cal, CComponent *ccomponent, QString typeStr, QOrganizerItemManager::Error *error) const
{
    // checks if a native event or todo is actually an occurrence

    *error = QOrganizerItemManager::NoError;
    QString guid = QString::fromStdString(ccomponent->getGUid());
    if (guid.isEmpty())
        return false; // possible items with no GUID are never considered as occurrences

    if (typeStr == QOrganizerItemType::TypeEvent) {

        qDebug() << "event";

        // if item contains recurrence information, it can't be an occurrence
        if (containsRecurrenceInformation(ccomponent))
            return false;

        // if no duplicate GUIDs are found, event can't be an occurrence
        int calError = CALENDAR_OPERATION_SUCCESSFUL;
        std::vector<CEvent*> eventsWithGuid = cal->getEvents(guid.toStdString(), calError);
        int eventsWithGuidSize = eventsWithGuid.size();
        *error = d->m_itemTransformer.calErrorToManagerError(calError);
        if (eventsWithGuidSize < 2) {
            for(int i = 0; i < eventsWithGuidSize; ++i)
                delete eventsWithGuid[i];
            return false;
        }

        // Now check if any of the events with the equal GUID contains recurrence
        // information. If there's at least one such, then this item is an occurrence.
        QString ccomponentId = QString::fromStdString(ccomponent->getId());
        for (int i = 0; i < eventsWithGuidSize; ++i) {
            CEvent *currEvent = eventsWithGuid[i];
            if (currEvent && QString::fromStdString(currEvent->getId()) != ccomponentId) { // exclude itself
                if (containsRecurrenceInformation(currEvent)) {
                    for(int j = 0; j < eventsWithGuidSize; ++j)
                        delete eventsWithGuid[j];
                    return true;
                }
            }
        }

        // There are multiple events with equal GUID and no event contains recurrence information.
        // If this event is created first, it is consider as a parent, otherwise an occurrence.
        CEvent* firstCreated = eventsWithGuid[0]; // always contains >= 2 events here
        for (int i = 1; i < eventsWithGuidSize; ++i) {
            CEvent *currEvent = eventsWithGuid[i];
            if (currEvent->getCreatedTime() < firstCreated->getCreatedTime()) {
                firstCreated = currEvent;
            }
            else if (currEvent->getCreatedTime() == firstCreated->getCreatedTime()
                && currEvent->getId() < firstCreated->getId()) {
                firstCreated = currEvent; // if two entries are created exactly at the same time, consider one with the smaller id as earlier
            }
        }

        bool returnValue = (ccomponent->getId() != firstCreated->getId());
        for(int i = 0; i < eventsWithGuidSize; ++i)
            delete eventsWithGuid[i];

        return returnValue;
    }
    else if (typeStr == QOrganizerItemType::TypeTodo || typeStr == QOrganizerItemType::TypeJournal
             || typeStr == QOrganizerItemType::TypeNote) {
        return false; // these types never have occurrences saved in the DB
    }
    else {
        return true; // an occurrence type was given
    }
}

bool QOrganizerItemMaemo5Engine::isParent(CCalendar *cal, CComponent *ccomponent, QString typeStr, QOrganizerItemManager::Error *error) const
{
    *error = QOrganizerItemManager::NoError;
    QString guid = QString::fromStdString(ccomponent->getGUid());
    if (guid.isEmpty())
        return false; // possible items with no GUID are never parents

    if (typeStr == QOrganizerItemType::TypeEvent) {
        bool retValue = false;
        int calError = CALENDAR_OPERATION_SUCCESSFUL;
        std::vector<CEvent*> eventsWithGuid = cal->getEvents(guid.toStdString(), calError);
        *error = d->m_itemTransformer.calErrorToManagerError(calError);
        int eventsWithGuidSize = eventsWithGuid.size();
        if (eventsWithGuidSize > 1) {
            // Multiple events with GUID found => if this one is not an occurrence
            // then it has to be a parent
            bool isOcc = isOccurrence(cal, ccomponent, QOrganizerItemType::TypeEvent, error);
            if (!isOcc && *error == QOrganizerItemManager::NoError)
                retValue = true;
        }

        // Free the resources
        for (int i = 0; i < eventsWithGuidSize; ++i)
            delete eventsWithGuid[i];

        return retValue;
    }
    else {
        return false;  // these types never have occurrences saved in the DB
    }
}

bool QOrganizerItemMaemo5Engine::containsRecurrenceInformation(CComponent *ccomponent) const
{
    CRecurrence *crecurrence = ccomponent->getRecurrence();
    if (crecurrence) {
        return (crecurrence->getRecurrenceRule().size() > 0
                || crecurrence->getRDays().size() > 0
                || crecurrence->getEDays().size() > 0);
    }
    else {
        return false;
    }
}

QOrganizerItemFetchHint QOrganizerItemMaemo5Engine::fetchMinimalData() const
{
    QOrganizerItemFetchHint::OptimizationHints optimizationHints(QOrganizerItemFetchHint::NoBinaryBlobs);
    optimizationHints |= QOrganizerItemFetchHint::NoActionPreferences;
    QOrganizerItemFetchHint fetchHints;
    fetchHints.setOptimizationHints(optimizationHints);
    return fetchHints;
}

void QOrganizerItemMaemo5Engine::cleanupCal(CCalendar *cal) const
{
    if (cal != d->m_mcInstance->getDefaultCalendar())
        delete cal; // The default calendar should not be deleted
}
