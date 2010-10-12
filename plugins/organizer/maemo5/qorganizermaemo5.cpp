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
#include "qorganizermaemo5ids_p.h"
#include "qtorganizer.h"
#include "qorganizerdbcache.h"

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

QOrganizerItemMaemo5EngineLocalId::QOrganizerItemMaemo5EngineLocalId()
    : QOrganizerItemEngineLocalId(), m_localItemId(0)
{
}

QOrganizerItemMaemo5EngineLocalId::QOrganizerItemMaemo5EngineLocalId(quint32 itemId)
    : QOrganizerItemEngineLocalId(), m_localItemId(itemId)
{
}

QOrganizerItemMaemo5EngineLocalId::~QOrganizerItemMaemo5EngineLocalId()
{
}

QOrganizerItemMaemo5EngineLocalId::QOrganizerItemMaemo5EngineLocalId(const QOrganizerItemMaemo5EngineLocalId& other)
    : QOrganizerItemEngineLocalId(), m_localItemId(other.m_localItemId)
{
}

bool QOrganizerItemMaemo5EngineLocalId::isEqualTo(const QOrganizerItemEngineLocalId* other) const
{
    quint32 otherlocalItemId = static_cast<const QOrganizerItemMaemo5EngineLocalId*>(other)->m_localItemId;
    if (m_localItemId != otherlocalItemId)
        return false;
    return true;
}

bool QOrganizerItemMaemo5EngineLocalId::isLessThan(const QOrganizerItemEngineLocalId* other) const
{
    quint32 otherlocalItemId = static_cast<const QOrganizerItemMaemo5EngineLocalId*>(other)->m_localItemId;
    return (m_localItemId < otherlocalItemId);
}

uint QOrganizerItemMaemo5EngineLocalId::engineLocalIdType() const
{
    // engines should embed the result of this as const read-only data (uint),
    // instead of calculating it every time the function is called...
    return qHash(QString(QLatin1String("maemo5")));
}

QOrganizerItemEngineLocalId* QOrganizerItemMaemo5EngineLocalId::clone() const
{
    QOrganizerItemMaemo5EngineLocalId *myClone = new QOrganizerItemMaemo5EngineLocalId;
    myClone->m_localItemId = m_localItemId;
    return myClone;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug QOrganizerItemMaemo5EngineLocalId::debugStreamOut(QDebug dbg)
{
    dbg.nospace() << "QOrganizerItemMaemo5EngineLocalId(" << m_localItemId << ")";
    return dbg.maybeSpace();
}
#endif

#ifndef QT_NO_DATASTREAM
QDataStream& QOrganizerItemMaemo5EngineLocalId::dataStreamOut(QDataStream& out)
{
    out << m_localItemId;
    return out;
}

QDataStream& QOrganizerItemMaemo5EngineLocalId::dataStreamIn(QDataStream& in)
{
    in >> m_localItemId;
    return in;
}
#endif

uint QOrganizerItemMaemo5EngineLocalId::hash() const
{
    // Note: doesn't need to be unique, since == ensures difference.
    // hash function merely determines distribution in a hash table.
    return m_localItemId;
}


QOrganizerCollectionMaemo5EngineLocalId::QOrganizerCollectionMaemo5EngineLocalId()
    : QOrganizerCollectionEngineLocalId(), m_localCollectionId(0)
{
}

QOrganizerCollectionMaemo5EngineLocalId::QOrganizerCollectionMaemo5EngineLocalId(quint32 collectionId)
    : QOrganizerCollectionEngineLocalId(), m_localCollectionId(collectionId)
{
}

QOrganizerCollectionMaemo5EngineLocalId::QOrganizerCollectionMaemo5EngineLocalId(const QOrganizerCollectionMaemo5EngineLocalId& other)
    : QOrganizerCollectionEngineLocalId(), m_localCollectionId(other.m_localCollectionId)
{
}

QOrganizerCollectionMaemo5EngineLocalId::~QOrganizerCollectionMaemo5EngineLocalId()
{
}

bool QOrganizerCollectionMaemo5EngineLocalId::isEqualTo(const QOrganizerCollectionEngineLocalId* other) const
{
    quint32 otherlocalCollectionId = static_cast<const QOrganizerCollectionMaemo5EngineLocalId*>(other)->m_localCollectionId;
    if (m_localCollectionId != otherlocalCollectionId)
        return false;
    return true;
}

bool QOrganizerCollectionMaemo5EngineLocalId::isLessThan(const QOrganizerCollectionEngineLocalId* other) const
{
    // order by collection, then by item in collection.
    quint32 otherlocalCollectionId = static_cast<const QOrganizerCollectionMaemo5EngineLocalId*>(other)->m_localCollectionId;
    if (m_localCollectionId < otherlocalCollectionId)
        return true;
    return false;
}

uint QOrganizerCollectionMaemo5EngineLocalId::engineLocalIdType() const
{
    // engines should embed the result of this as const read-only data (uint),
    // instead of calculating it every time the function is called...
    return qHash(QString(QLatin1String("maemo5")));
}

QOrganizerCollectionEngineLocalId* QOrganizerCollectionMaemo5EngineLocalId::clone() const
{
    QOrganizerCollectionMaemo5EngineLocalId *myClone = new QOrganizerCollectionMaemo5EngineLocalId;
    myClone->m_localCollectionId = m_localCollectionId;
    return myClone;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug QOrganizerCollectionMaemo5EngineLocalId::debugStreamOut(QDebug dbg)
{
    dbg.nospace() << "QOrganizerCollectionMaemo5EngineLocalId(" << m_localCollectionId << ")";
    return dbg.maybeSpace();
}
#endif

#ifndef QT_NO_DATASTREAM
QDataStream& QOrganizerCollectionMaemo5EngineLocalId::dataStreamOut(QDataStream& out)
{
    return (out << m_localCollectionId);
}

QDataStream& QOrganizerCollectionMaemo5EngineLocalId::dataStreamIn(QDataStream& in)
{
    return (in >> m_localCollectionId);
}
#endif

uint QOrganizerCollectionMaemo5EngineLocalId::hash() const
{
    return QT_PREPEND_NAMESPACE(qHash)(m_localCollectionId);
}

QOrganizerItemManagerEngine* QOrganizerItemMaemo5Factory::engine(const QMap<QString, QString> &parameters, QOrganizerItemManager::Error *error)
{
    Q_UNUSED(parameters);
    Q_UNUSED(error);

    QOrganizerItemMaemo5Engine* retn = new QOrganizerItemMaemo5Engine(); // manager takes ownership and will clean up.
    return retn;
}

QOrganizerItemEngineLocalId* QOrganizerItemMaemo5Factory::createItemEngineLocalId() const
{
    return new QOrganizerItemMaemo5EngineLocalId;
}

QOrganizerCollectionEngineLocalId* QOrganizerItemMaemo5Factory::createCollectionEngineLocalId() const
{
    return new QOrganizerCollectionMaemo5EngineLocalId;
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

    d->m_dbCache = new OrganizerDbCache();
    connect(databaseMonitor, SIGNAL(fileChanged(const QString &)), d->m_dbCache, SLOT(invalidate()));
    d->m_dbAccess = new OrganizerCalendarDatabaseAccess(d->m_dbCache);

    bool dbOk = d->m_dbAccess->open(QDir::homePath().append(CALENDAR).append(CALENDARDB));
    if (!dbOk) {
        qDebug() << "Database didn't open!";
        // TODO: Then what? Constructor has no error status. Throw an exception?
    }
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

QList<QOrganizerItem> QOrganizerItemMaemo5Engine::itemOccurrences(const QOrganizerItem& parentItem, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const
{
    Q_UNUSED(fetchHint);
    QMutexLocker locker(&m_operationMutex);
    return internalItemOccurrences(parentItem, periodStart, periodEnd, maxCount, error);
}

QList<QOrganizerItemLocalId> QOrganizerItemMaemo5Engine::itemIds(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter &filter, const QList<QOrganizerItemSortOrder> &sortOrders, QOrganizerItemManager::Error *error) const
{
    QMutexLocker locker(&m_operationMutex);
    return internalItemIds(startDate, endDate, filter, sortOrders, error);
}

QList<QOrganizerItem> QOrganizerItemMaemo5Engine::items(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter &filter, const QList<QOrganizerItemSortOrder> &sortOrders, const QOrganizerItemFetchHint &fetchHint, QOrganizerItemManager::Error *error) const
{
    QMutexLocker locker(&m_operationMutex);
    return internalItems(startDate, endDate, filter, sortOrders, fetchHint, false, error);
}

QList<QOrganizerItem> QOrganizerItemMaemo5Engine::itemsForExport(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter &filter, const QList<QOrganizerItemSortOrder> &sortOrders, const QOrganizerItemFetchHint &fetchHint, QOrganizerItemManager::Error *error) const
{
    QMutexLocker locker(&m_operationMutex);
    return internalItems(startDate, endDate, filter, sortOrders, fetchHint, true, error);
}

QOrganizerItem QOrganizerItemMaemo5Engine::item(const QOrganizerItemLocalId &itemId, const QOrganizerItemFetchHint &fetchHint, QOrganizerItemManager::Error *error) const
{
    QMutexLocker locker(&m_operationMutex);
    return internalItem(itemId, fetchHint, error);
}

bool QOrganizerItemMaemo5Engine::saveItems(QList<QOrganizerItem>* items, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error)
{
    QMutexLocker locker(&m_operationMutex);
    return internalSaveItems(items, errorMap, error);
}

bool QOrganizerItemMaemo5Engine::removeItems(const QList<QOrganizerItemLocalId> &itemIds, QMap<int, QOrganizerItemManager::Error> *errorMap, QOrganizerItemManager::Error *error)
{
    QMutexLocker locker(&m_operationMutex);
    return internalRemoveItems(itemIds, errorMap, error);
}

QOrganizerCollection QOrganizerItemMaemo5Engine::defaultCollection(QOrganizerItemManager::Error* error) const
{
    QMutexLocker locker(&m_operationMutex);
    return internalDefaultCollection(error);
}

QOrganizerCollection QOrganizerItemMaemo5Engine::collection(const QOrganizerCollectionLocalId& collectionId, QOrganizerItemManager::Error* error) const
{
    QMutexLocker locker(&m_operationMutex);
    return internalCollection(collectionId, error);
}

QList<QOrganizerCollection> QOrganizerItemMaemo5Engine::collections(QOrganizerItemManager::Error* error) const
{
    QMutexLocker locker(&m_operationMutex);
    return internalCollections(error);
}

bool QOrganizerItemMaemo5Engine::saveCollection(QOrganizerCollection* collection, QOrganizerItemManager::Error* error)
{
    QMutexLocker locker(&m_operationMutex);
    return internalSaveCollection(collection, error);
}

bool QOrganizerItemMaemo5Engine::removeCollection(const QOrganizerCollectionLocalId& collectionId, QOrganizerItemManager::Error* error)
{
    QMutexLocker locker(&m_operationMutex);
    return internalRemoveCollection(collectionId, error);
}

QList<QOrganizerItem> QOrganizerItemMaemo5Engine::internalItemOccurrences(const QOrganizerItem &parentItem, const QDateTime &periodStart, const QDateTime &periodEnd, int maxCount, QOrganizerItemManager::Error *error) const
{
    *error = QOrganizerItemManager::NoError;
    int calError = CALENDAR_OPERATION_SUCCESSFUL;
    QList<QOrganizerItem> retn;

    if (periodStart > periodEnd)
    {
        *error = QOrganizerItemManager::BadArgumentError;
        return retn;
    }

    // get the parent item's calendar (or the default calendar, if the parent item's collection id is not set)
    QOrganizerCollectionLocalId collectionLocalId = parentItem.collectionId().localId();
    CCalendar *cal = getCalendar(collectionLocalId, error);
    if (*error != QOrganizerItemManager::NoError)
        return retn;
    if (!cal) {
        *error = QOrganizerItemManager::UnspecifiedError;
        return retn;
    }

    std::string nativeId = QString::number(readItemLocalId(parentItem.localId())).toStdString();

    if (parentItem.type() == QOrganizerItemType::TypeEvent)
    {
        CEvent *cevent = d->m_dbAccess->getEvent(cal, nativeId, calError);
        *error = d->m_itemTransformer.calErrorToManagerError(calError);
        if (cevent && *error == QOrganizerItemManager::NoError)
        {
            // Get event instance times
            std::vector< std::time_t > eventInstanceDates;
            cevent->generateInstanceTimes(periodStart.toTime_t(), periodEnd.toTime_t(), eventInstanceDates);

            // Calculate the parent item's event duration (the occurrences will have the same duration)
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

                    // Set the collection id
                    eventOcc.setCollectionId(parentItem.collectionId());

                    retn << eventOcc;
                }
            }

            // Now we have got the simple occurrences that are generated with the recurrence rules.
            // Events can have also occurrence that are not generated with rules, but saved as
            // events (because they have become modified). Those occurrences are saved with GUID set
            // equal to the parent event's GUID.
            QString eventType = QOrganizerItemType::TypeEvent;
            std::vector<CEvent*> occurrenceCandidates = d->m_dbAccess->getEvents(cal->getCalendarId(), cevent->getGUid(), calError);
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            if (*error == QOrganizerItemManager::NoError) {
                std::vector<CEvent*>::iterator occCand;
                for (occCand = occurrenceCandidates.begin(); occCand != occurrenceCandidates.end(); ++occCand) {
                    CEvent* coccurrenceCandidate = *occCand;
                    if (coccurrenceCandidate && coccurrenceCandidate->getId() != cevent->getId()) {
                        // for all other events than the parent item itself
                        QDateTime instanceStartDate = QDateTime::fromTime_t(coccurrenceCandidate->getDateStart());
                        QDateTime instanceEndDate = QDateTime::fromTime_t(coccurrenceCandidate->getDateEnd());
                        QString idString = QString::fromStdString(cevent->getId());
                        QOrganizerItemLocalId parentLocalId(new QOrganizerItemMaemo5EngineLocalId(idString.toUInt())); // ctor takes ownership of the ptr.

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

                                    // Set the collection id
                                    eventOcc.setCollectionId(parentItem.collectionId());

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
    else if (parentItem.type() == QOrganizerItemType::TypeTodo)
    {
        CTodo* ctodo = d->m_dbAccess->getTodo(cal, nativeId, calError);
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

                // Set the collection id
                todoOcc.setCollectionId(parentItem.collectionId());

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

    cleanupCal(cal);

    return retn;
}

QList<QOrganizerItemLocalId> QOrganizerItemMaemo5Engine::internalItemIds(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter &filter, const QList<QOrganizerItemSortOrder> &sortOrders, QOrganizerItemManager::Error *error) const
{
    QList<QOrganizerItem> clist = internalItems(startDate, endDate, filter, sortOrders, fetchMinimalData(), true, error);

    /* Extract the ids */
    QList<QOrganizerItemLocalId> ids;
    foreach(const QOrganizerItem& c, clist)
        ids.append(c.localId());

    return ids;
}

QList<QOrganizerItem> QOrganizerItemMaemo5Engine::internalItems(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter &filter, const QList<QOrganizerItemSortOrder> &sortOrders, const QOrganizerItemFetchHint &fetchHint, bool forExport, QOrganizerItemManager::Error *error) const
{
    *error = QOrganizerItemManager::NoError;
    int calError = CALENDAR_OPERATION_SUCCESSFUL;
    QList<QOrganizerItemLocalId> localIds;

    // Resolve from which collections (calendars) the items must be fetch (pre-filtering)
    QSet<QOrganizerCollectionId> collectionIds = extractCollectionIds(filter);

    if (collectionIds.empty()) {
        // use all calendars if no collection filter is specified
        QSet<QOrganizerCollectionLocalId> localIds = internalCollectionIds(error).toSet();
        if (*error != QOrganizerItemManager::NoError)
            return QList<QOrganizerItem>();

        foreach(QOrganizerCollectionLocalId localId, localIds) {
            QOrganizerCollectionId newCollectionId;
            newCollectionId.setManagerUri(managerUri());
            newCollectionId.setLocalId(localId);
            collectionIds << newCollectionId;
        }
    }

    foreach (QOrganizerCollectionId collectionId, collectionIds) {
        CCalendar *cal = d->m_mcInstance->getCalendarById(static_cast<int>(static_cast<QOrganizerCollectionMaemo5EngineLocalId *>(QOrganizerItemManagerEngine::engineLocalCollectionId(collectionId.localId()))->m_localCollectionId), calError);

        *error = d->m_itemTransformer.calErrorToManagerError(calError);
        if (*error != QOrganizerItemManager::NoError)
            return QList<QOrganizerItem>();

        std::vector<std::string>::const_iterator id;

        // Append event ids
        std::vector<std::string> eventIds;
        d->m_dbAccess->getIdList(cal, E_EVENT, calError, eventIds);
        //std::vector<std::string> eventIds = cal->getIdList(E_EVENT, calError);
        *error = d->m_itemTransformer.calErrorToManagerError(calError);
        if (*error != QOrganizerItemManager::NoError)
            return QList<QOrganizerItem>();
        for (id = eventIds.begin(); id != eventIds.end(); ++id)
            localIds << QOrganizerItemLocalId(new QOrganizerItemMaemo5EngineLocalId(QString::fromStdString(*id).toUInt()));

        // Append todo ids
        std::vector<std::string> todoIds;
        d->m_dbAccess->getIdList(cal, E_TODO, calError, todoIds);
        //std::vector<std::string> todoIds = cal->getIdList(E_TODO, calError);
        *error = d->m_itemTransformer.calErrorToManagerError(calError);
        if (*error != QOrganizerItemManager::NoError)
            return QList<QOrganizerItem>();
        for (id = todoIds.begin(); id != todoIds.end(); ++id)
            localIds << QOrganizerItemLocalId(new QOrganizerItemMaemo5EngineLocalId(QString::fromStdString(*id).toUInt()));

        // Append journal ids
        std::vector<std::string> journalIds;
        d->m_dbAccess->getIdList(cal, E_JOURNAL, calError, journalIds);
        //std::vector<std::string> journalIds = cal->getIdList(E_JOURNAL, calError);
        *error = d->m_itemTransformer.calErrorToManagerError(calError);
        if (*error != QOrganizerItemManager::NoError)
            return QList<QOrganizerItem>();
        for (id = journalIds.begin(); id != journalIds.end(); ++id)
            localIds << QOrganizerItemLocalId(new QOrganizerItemMaemo5EngineLocalId(QString::fromStdString(*id).toUInt()));

        // Cleanup calendar
        cleanupCal(cal);
    }

    // Use the general implementation to filter and sort items
    QList<QOrganizerItem> filteredAndSorted;
    foreach(const QOrganizerItemLocalId& id, localIds) {
        QOrganizerItem item = internalFetchItem(id, fetchHint, error, true);
        if (*error == QOrganizerItemManager::NoError) {
            if (item.type() == QOrganizerItemType::TypeEvent) {
                internalAddOccurances(&filteredAndSorted, item, startDate, endDate, filter, sortOrders, forExport, error);
            } else
                if (QOrganizerItemManagerEngine::testFilter(filter, item) && QOrganizerItemManagerEngine::isItemBetweenDates(item, startDate, endDate)) {
                    QOrganizerItemManagerEngine::addSorted(&filteredAndSorted, item, sortOrders);
                }
        } else
            return QList<QOrganizerItem>();
    }

    return filteredAndSorted;
}

void QOrganizerItemMaemo5Engine::internalAddOccurances(QList<QOrganizerItem>* sorted, QOrganizerItem& item, const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter &filter, const QList<QOrganizerItemSortOrder> &sortOrders, bool forExport, QOrganizerItemManager::Error *error) const
{
    CCalendar *cal = d->m_mcInstance->getDefaultCalendar();
    std::string nativeId = QString::number(readItemLocalId(item.localId())).toStdString();
    int calError = CALENDAR_OPERATION_SUCCESSFUL;
    CEvent *cevent = d->m_dbAccess->getEvent(cal, nativeId, calError);
    *error = d->m_itemTransformer.calErrorToManagerError(calError);
    if (cevent && *error == QOrganizerItemManager::NoError)
    {
        if (containsRecurrenceInformation(cevent)) {
            // Get event instance times
            std::vector< std::time_t > eventInstanceDates;
            bool dontGenerateOccurrences = startDate.isNull() && endDate.isNull() && forExport;
            if (!dontGenerateOccurrences) {
                time_t realStartDate = startDate.isNull() ? cevent->getDateStart() : startDate.toTime_t();
                time_t realEndDate = endDate.isNull() ? realStartDate + 157680000 : endDate.toTime_t();
                cevent->generateInstanceTimes(realStartDate, realEndDate, eventInstanceDates);
            }

            if (dontGenerateOccurrences || eventInstanceDates.size() > 0) {
                if (forExport) {
                    if (QOrganizerItemManagerEngine::testFilter(filter, item)) {
                        QOrganizerItemManagerEngine::addSorted(sorted, item, sortOrders);
                    }
                } else {
                    time_t generatorDuration = cevent->getDateEnd() - cevent->getDateStart();

                    // Generate the event occurrences
                    std::vector< std::time_t >::const_iterator i;
                    for (i = eventInstanceDates.begin(); i != eventInstanceDates.end(); ++i)
                    {
                        QDateTime instanceStartDate = QDateTime::fromTime_t(*i);
                        QDateTime instanceEndDate = QDateTime::fromTime_t(*i + generatorDuration);
                        QOrganizerEventOccurrence eventOcc =
                                d->m_itemTransformer.convertCEventToQEventOccurrence(cevent, instanceStartDate, instanceEndDate);
                        d->m_itemTransformer.fillInCommonCComponentDetails(&eventOcc, cevent, false); // false = do not set local ids

                        // Set the collection id
                        eventOcc.setCollectionId(item.collectionId());

                        if (QOrganizerItemManagerEngine::testFilter(filter, eventOcc) && QOrganizerItemManagerEngine::isItemBetweenDates(eventOcc, startDate, endDate))
                            QOrganizerItemManagerEngine::addSorted(sorted, eventOcc, sortOrders);
                    }
                }
            }
        } else {
            if (QOrganizerItemManagerEngine::testFilter(filter, item) && QOrganizerItemManagerEngine::isItemBetweenDates(item, startDate, endDate))
                QOrganizerItemManagerEngine::addSorted(sorted, item, sortOrders);
        }
    }
}

QOrganizerItem QOrganizerItemMaemo5Engine::internalItem(const QOrganizerItemLocalId &itemId, const QOrganizerItemFetchHint &fetchHint, QOrganizerItemManager::Error *error) const
{
    return internalFetchItem(itemId, fetchHint, error, true);
}

bool QOrganizerItemMaemo5Engine::internalSaveItems(QList<QOrganizerItem>* items, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error)
{
    if (!items || items->isEmpty()) {
        *error = QOrganizerItemManager::BadArgumentError;
        return false;
    }

    *error = QOrganizerItemManager::NoError;
    QOrganizerCollectionId completeCollectionId;
    completeCollectionId.setManagerUri(managerUri());
    // we set the local id of the collection separately for each item.

    bool success = true;
    QOrganizerItemChangeSet cs;

    QOrganizerItemManager::Error tempError = QOrganizerItemManager::NoError;
    for (int i = 0; i < items->size(); ++i) {
        QOrganizerItem curr = items->at(i);
        QOrganizerCollectionLocalId collectionId = curr.collectionLocalId();
        CCalendar *cal = getCalendar(collectionId, &tempError);
        if (tempError != QOrganizerItemManager::NoError) {
            // unable to get the calendar to save this item in.
            if (errorMap) {
                errorMap->insert(i, QOrganizerItemManager::InvalidCollectionError);
            }
            *error = tempError;
	}
        completeCollectionId.setLocalId(collectionId);

        tempError = QOrganizerItemManager::NoError;
        int calError = doSaveItem(cal, &curr, cs, &tempError);

        if (calError != CALENDAR_OPERATION_SUCCESSFUL || tempError != QOrganizerItemManager::NoError) {
            success = false;
            curr.setId(QOrganizerItemId()); // clear IDs
            curr.setCollectionId(QOrganizerCollectionId());
            if (errorMap) {
                if (tempError == QOrganizerItemManager::NoError)
                    tempError = QOrganizerItemManager::UnspecifiedError;
                errorMap->insert(i, tempError);
                *error = tempError;
            }
        }
        else {
            // Set item collection id
	    curr.setCollectionId(completeCollectionId);
        }

        items->replace(i, curr);
	cleanupCal(cal); // TODO we may want to coalesce cleanup/instantiation into blocks (save items with the same collectionId at the same time)
    }

    d->m_mcInstance->commitAllChanges(); // ensure that changes are committed before emitting signals
    cs.emitSignals(this);
    return success;
}

bool QOrganizerItemMaemo5Engine::internalRemoveItems(const QList<QOrganizerItemLocalId> &itemIds, QMap<int, QOrganizerItemManager::Error> *errorMap, QOrganizerItemManager::Error *error)
{
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
            QString itemId = QString::number(readItemLocalId(currItem.localId()));
            int calError = CALENDAR_OPERATION_SUCCESSFUL;
            if (currItem.type() == QOrganizerItemType::TypeEvent) {
                // Delete also child events if this event is a parent
                CEvent* cevent = d->m_dbAccess->getEvent(cal, itemId.toStdString(), calError);
                if (calError == CALENDAR_OPERATION_SUCCESSFUL) {
                    bool parentItem = isParent(cal, cevent, QOrganizerItemType::TypeEvent, error);
                    if (parentItem && *error == QOrganizerItemManager::NoError) {
                        std::vector<CEvent*> eventsWithGuid = d->m_dbAccess->getEvents(cal->getCalendarId(), cevent->getGUid(), calError);
                        *error = d->m_itemTransformer.calErrorToManagerError(calError);
                        if (calError == CALENDAR_OPERATION_SUCCESSFUL) {
                            std::vector<CEvent*>::const_iterator childEvent;
                            for (childEvent = eventsWithGuid.begin(); childEvent != eventsWithGuid.end(); ++childEvent) {
                                QOrganizerItemLocalId childId = makeItemLocalId(QString::fromStdString((*childEvent)->getId()).toUInt());
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
                d->m_dbCache->invalidate();
                *error = d->m_itemTransformer.calErrorToManagerError(calError);

                if (calError == CALENDAR_OPERATION_SUCCESSFUL) {
                    // Success, update the changeset
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

QOrganizerCollection QOrganizerItemMaemo5Engine::internalDefaultCollection(QOrganizerItemManager::Error* error) const
{
    *error = QOrganizerItemManager::NoError;
    CCalendar *cal = d->m_mcInstance->getDefaultCalendar(); // TODO do we need to release this cal ptr?
    QOrganizerCollectionLocalId defaultCollectionId = makeCollectionLocalId(cal->getCalendarId());
    return internalCollection(defaultCollectionId, error);
}

QOrganizerCollection QOrganizerItemMaemo5Engine::internalCollection(const QOrganizerCollectionLocalId& collectionId, QOrganizerItemManager::Error* error) const
{
    *error = QOrganizerItemManager::NoError;
    QList<QOrganizerCollection> allCollections = internalCollections(error);
    foreach (const QOrganizerCollection& collection, allCollections) {
        if (collection.localId() == collectionId) {
            return collection;
        }
    }

    *error = QOrganizerItemManager::DoesNotExistError;
    return QOrganizerCollection();
}

QList<QOrganizerCollectionLocalId> QOrganizerItemMaemo5Engine::internalCollectionIds(QOrganizerItemManager::Error* error) const
{
    *error = QOrganizerItemManager::NoError;
    QList<QOrganizerCollectionLocalId> retn;

    // Append the default calendar id
    *error = QOrganizerItemManager::NoError;
    CCalendar *cal = d->m_mcInstance->getDefaultCalendar(); // TODO do we need to release this cal ptr?
    QOrganizerCollectionLocalId defaultCollectionId = makeCollectionLocalId(cal->getCalendarId());
    retn << defaultCollectionId;

    // Append other calendar ids
    std::vector<CCalendar*> calendars = d->m_mcInstance->getListCalFromMc();
    std::vector<CCalendar*>::iterator calendar;
    for (calendar = calendars.begin(); calendar != calendars.end(); ++calendar) {
        QOrganizerCollectionLocalId current(new QOrganizerCollectionMaemo5EngineLocalId((*calendar)->getCalendarId()));
        if (current != retn[0])
            retn << current;
    }

    // Cleanup
    d->m_mcInstance->releaseListCalendars(calendars);

    return retn;
}

QList<QOrganizerCollection> QOrganizerItemMaemo5Engine::internalCollections(QOrganizerItemManager::Error* error) const
{
    *error = QOrganizerItemManager::NoError;
    QList<QOrganizerCollection> retn;

    // Fetch all the available collection ids
    QList<QOrganizerCollectionLocalId> fetchCollIds = internalCollectionIds(error);

    int collectionIdsCount = fetchCollIds.count();
    for (int i = 0; i < collectionIdsCount; ++i) {
        QOrganizerCollectionLocalId collectionId = fetchCollIds.at(i);

        // Fetch calendar
        int calendarId = static_cast<int>(readCollectionLocalId(collectionId));
        int calError = CALENDAR_OPERATION_SUCCESSFUL;
        CCalendar *cal = d->m_mcInstance->getCalendarById(calendarId, calError);
        if (calError != CALENDAR_OPERATION_SUCCESSFUL) {
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
        }

        // Build collection id
        QOrganizerCollectionId currCollectionId;
        currCollectionId.setManagerUri(managerUri());
        currCollectionId.setLocalId(collectionId);

        // Set collection id
        QOrganizerCollection currCollection;
        currCollection.setId(currCollectionId);

        // Set collection metadata:
        // Available calendar colors
        QList<QString> availableColors = d->m_itemTransformer.calendarColourMap().values();
        currCollection.setMetaData("Available colors", QVariant(availableColors));

        // Calendar color
        currCollection.setMetaData(QOrganizerCollection::KeyColor, QVariant(
                d->m_itemTransformer.fromCalendarColour(cal->getCalendarColor())));

        // Calendar name
        currCollection.setMetaData(QOrganizerCollection::KeyName, QVariant(QString::fromStdString(cal->getCalendarName())));

        // Calendar version
        currCollection.setMetaData("Version", QVariant(QString::fromStdString(cal->getCalendarVersion())));

        // Available calendar types
        QList<QString> availableTypes = d->m_itemTransformer.calendarTypeMap().values();
        currCollection.setMetaData("Available types", QVariant(availableTypes));

        // Calendar type
        currCollection.setMetaData("Type", QVariant(
                d->m_itemTransformer.fromCalendarType(cal->getCalendarType())));

        // Calendar tune
        currCollection.setMetaData("Tune", QVariant(QString::fromStdString(cal->getCalendarTune())));

        // Readonly
        currCollection.setMetaData("Readonly", QVariant(cal->IsReadOnly()));

        // Visible
        currCollection.setMetaData("Visible", QVariant(cal->IsShown()));

        // Free calendar
        cleanupCal(cal);
        cal = 0;

        // Append to result
        retn << currCollection;
    }

    return retn;
}

bool QOrganizerItemMaemo5Engine::internalSaveCollection(QOrganizerCollection* collection, QOrganizerItemManager::Error* error)
{
    if (!collection) {
        *error = QOrganizerItemManager::BadArgumentError;
        return false;
    }

    if (collection->id().managerUri() != managerUri())
    {
        if (!collection->id().localId().isNull()) {
            // Do not allow to save other manager's collection with local id set
            *error = QOrganizerItemManager::BadArgumentError;
            return false;
        }
    }

    QList<QOrganizerCollectionLocalId> collIds = internalCollectionIds(error);
    if (*error != QOrganizerItemManager::NoError)
        return false;

    QString calName = ""; // default value
    QVariant tmpData = collection->metaData(QOrganizerCollection::KeyName);
    if (tmpData.isValid())
        calName = tmpData.toString();

    CalendarColour calColor = COLOUR_NEXT_FREE; // default value
    tmpData = collection->metaData(QOrganizerCollection::KeyColor);
    if (tmpData.isValid())
        calColor = d->m_itemTransformer.toCalendarColour(tmpData.toString());

    bool calReadOnly = false; // default value
    tmpData = collection->metaData("Readonly");
    if (tmpData.isValid())
        calReadOnly = tmpData.toBool();

    bool calVisible = true; // default value
    tmpData = collection->metaData("Visible");
    if (tmpData.isValid())
        calVisible = tmpData.toBool();

    CalendarType calType = LOCAL_CALENDAR; // default value
    tmpData = collection->metaData("Type");
    if (tmpData.isValid())
        calType = d->m_itemTransformer.toCalendarType(tmpData.toString());

    QString calTune = ""; // default value
    tmpData = collection->metaData("Tune");
    if (tmpData.isValid())
        calTune = tmpData.toString();

    QString calVersion = "1.0"; // default value
    tmpData = collection->metaData("Version");
    if (tmpData.isValid())
        calVersion = tmpData.toString();

    int calError = CALENDAR_OPERATION_SUCCESSFUL;

    if (!collIds.contains(collection->id().localId())) {
        // New calendar

        // Can't save two calendars with same name
        int calExistStatus = CALENDAR_OPERATION_SUCCESSFUL;
        CCalendar *oldCalendar = d->m_mcInstance->getCalendarByName(
                calName.toStdString(), calExistStatus);
        if (oldCalendar) {
            delete oldCalendar;
            oldCalendar = 0;
        }
        if (calExistStatus != CALENDAR_DOESNOT_EXISTS) {
            *error = QOrganizerItemManager::AlreadyExistsError;
            return false;
        }

        // Ok, create a new calendar
        CCalendar *newCalendar = d->m_mcInstance->addCalendar(
                calName.toStdString(),
                calColor,
                calReadOnly ? 1:0,
                calVisible ? 1:0,
                calType,
                calTune.toStdString(),
                calVersion.toStdString(),
                calError);
        d->m_dbCache->invalidate();

        if (!newCalendar) {
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            if (*error == QOrganizerItemManager::NoError)
                *error = QOrganizerItemManager::UnspecifiedError;
            return false;
        }
        if (calError != CALENDAR_OPERATION_SUCCESSFUL) {
            delete newCalendar;
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            return false;
        }

        // Build collection id
        QOrganizerCollectionId collId;
        collId.setManagerUri(managerUri());
        collId.setLocalId(makeCollectionLocalId(newCalendar->getCalendarId()));

        // Set collection id
        collection->setId(collId);

        // Free calendar
        delete newCalendar;
    }
    else {
        // Modify calendar

        bool ok = d->m_mcInstance->modifyCalendar(
                static_cast<int>(readCollectionLocalId(collection->id().localId())),
                calName.toStdString(),
                calColor,
                calReadOnly ? 1 : 0,
                calVisible ? 1 : 0,
                calType,
                calTune.toStdString(),
                calVersion.toStdString(),
                calError);
        d->m_dbCache->invalidate();

        if (calError != CALENDAR_OPERATION_SUCCESSFUL) {
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            return false;
        }
        else if (!ok) {
            *error = QOrganizerItemManager::UnspecifiedError;
            return false;
        }
    }

    return true;
}

bool QOrganizerItemMaemo5Engine::internalRemoveCollection(const QOrganizerCollectionLocalId& collectionId, QOrganizerItemManager::Error* error)
{
    int calError = CALENDAR_OPERATION_SUCCESSFUL;
    bool ok = d->m_mcInstance->deleteCalendar(static_cast<int>(readCollectionLocalId(collectionId)), calError);
    d->m_dbCache->invalidate();
    *error = d->m_itemTransformer.calErrorToManagerError(calError);
    return ok;
}

bool QOrganizerItemMaemo5Engine::startRequest(QOrganizerItemAbstractRequest* req)
{
    d->m_asynchProcess->addRequest(req);
    return true;
}

bool QOrganizerItemMaemo5Engine::cancelRequest(QOrganizerItemAbstractRequest* req)
{
    return d->m_asynchProcess->cancelRequest(req);
}

bool QOrganizerItemMaemo5Engine::waitForRequestFinished(QOrganizerItemAbstractRequest* req, int msecs)
{
    return d->m_asynchProcess->waitForRequestFinished(req, msecs);
}

void QOrganizerItemMaemo5Engine::requestDestroyed(QOrganizerItemAbstractRequest* req)
{
    return d->m_asynchProcess->requestDestroyed(req);
}

bool QOrganizerItemMaemo5Engine::hasFeature(QOrganizerItemManager::ManagerFeature feature, const QString &itemType) const
{
    Q_UNUSED(itemType);
    switch(feature) {
        case QOrganizerItemManager::MutableDefinitions:
            return false;
        case QOrganizerItemManager::Anonymous:
            return false;
        case QOrganizerItemManager::ChangeLogs:
            return false;
    }
    return false;
}

bool QOrganizerItemMaemo5Engine::isFilterSupported(const QOrganizerItemFilter &filter) const
{
    Q_UNUSED(filter);
    return false;
}

QList<int> QOrganizerItemMaemo5Engine::supportedDataTypes() const
{
    QList<int> retn;
    retn << QVariant::String;
    retn << QVariant::Date;
    retn << QVariant::DateTime;
    retn << QVariant::Time;
    return retn;
}

QStringList QOrganizerItemMaemo5Engine::supportedItemTypes() const
{
    QStringList retn;
    retn << QOrganizerItemType::TypeEvent;
    retn << QOrganizerItemType::TypeEventOccurrence;
    retn << QOrganizerItemType::TypeJournal;
    retn << QOrganizerItemType::TypeTodo;
    retn << QOrganizerItemType::TypeTodoOccurrence;
    return retn;
}

void QOrganizerItemMaemo5Engine::checkItemIdValidity(CCalendar *cal, QOrganizerItem *checkItem, QOrganizerItemManager::Error *error)
{
    *error = QOrganizerItemManager::NoError;

    // Check local id
    if (!checkItem->localId().isNull()) {
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

    // TODO: Maybe here should be added a GUID check?:
    // if item is not an occurrence, setting duplicate GUIDs should be disallowed

    // If the item is not an occurrence, no more checks are needed
    if (!eventOccurrence && !todoOccurrence)
        return;

    // Event occurrence validity checks:
    if (eventOccurrence) {
        // Original date must be set
        if (eventOccurrence->originalDate() == QDate()) {
            *error = QOrganizerItemManager::InvalidOccurrenceError;
            return;
        }

        // Either parent id or GUID (or both) must be set
        if (eventOccurrence->parentLocalId().isNull() && eventOccurrence->guid().isEmpty()) {
            *error = QOrganizerItemManager::InvalidOccurrenceError;
            return;
        }

        // If the parent ID is set, it must point to an event
        QOrganizerItem parent;
        if (!eventOccurrence->parentLocalId().isNull()) {
            parent = internalItem(eventOccurrence->parentLocalId(), fetchMinimalData(), error);
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
            std::vector<CEvent*> eventsWithGuid = d->m_dbAccess->getEvents(cal->getCalendarId(), eventOccurrence->guid().toStdString(), calError);
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
        if (!eventOccurrence->parentLocalId().isNull() && !eventOccurrence->guid().isEmpty()) {
            if (eventOccurrence->guid() != parent.guid()) {
                *error = QOrganizerItemManager::InvalidOccurrenceError;
                return;
            }
        }
    }

    // Todo occurrence validity checks:
    if (todoOccurrence) {
        // Original date must be set
        if (todoOccurrence->originalDate() == QDate()) {
            *error = QOrganizerItemManager::InvalidOccurrenceError;
            return;
        }

        // Either parent id or GUID (or both) must be set
        if (todoOccurrence->parentLocalId().isNull() && todoOccurrence->guid().isEmpty()) {
            *error = QOrganizerItemManager::InvalidOccurrenceError;
            return;
        }

        if (!todoOccurrence->parentLocalId().isNull()) {
            // Parent ID must point to this occurrence due to restrictions in Maemo5 todos
            if (todoOccurrence->parentLocalId() != todoOccurrence->localId()) {
                *error = QOrganizerItemManager::InvalidOccurrenceError;
                return;
            }
        }
        else {
            // Must find at least one todo with the GUID from the DB
            int calError = CALENDAR_OPERATION_SUCCESSFUL;
            std::vector<CTodo*> todosWithGuid = d->m_dbAccess->getTodos(cal->getCalendarId(), todoOccurrence->guid().toStdString(), calError);
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            int numberOfTodos = todosWithGuid.size();
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
    int calError = CALENDAR_OPERATION_SUCCESSFUL;
    *error = QOrganizerItemManager::NoError;

    // Check item validity
    checkItemIdValidity(cal, item, error);
    if (*error != QOrganizerItemManager::NoError)
        return calError;

    // Returns InvalidItemTypeError if the type won't be recognized later
    *error = QOrganizerItemManager::InvalidItemTypeError;

    CComponent *component = d->m_itemTransformer.createCComponent(cal, item, error);
    if (!component || *error != QOrganizerItemManager::NoError) {
        delete component;
        return calError;
    }

    // Read the given GUID
    item->setGuid(QString::fromStdString(component->getGUid()));

    if (item->type() == QOrganizerItemType::TypeEvent) {
        CEvent* cevent = static_cast<CEvent *>(component);
        QString ceventId = QString::fromStdString(cevent->getId());
        if (!ceventId.isEmpty()) {
            // CEvent ID is not empty, the event already exists in calendar

            // increase the sequence number
            cevent->setSequence(cevent->getSequence() + 1);

            cal->modifyEvent(cevent, calError);
            d->m_dbCache->invalidate();
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            if (*error == QOrganizerItemManager::NoError) {
                // Modify alarm to contain the reminder information
                // (must always be done only after the component is saved)
                QPair<qint32, qint32> cookieChange = d->m_itemTransformer.modifyAlarmEvent(cal, item, component);
                d->m_dbAccess->fixAlarmCookie(cookieChange);

                cs.insertChangedItem(item->localId());
            }
        }
        else {
            // CEvent ID is empty, the event is new

            // set the sequence number zero
            cevent->setSequence(0);

            cal->addEvent(cevent, calError);
            d->m_dbCache->invalidate();
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
                QOrganizerItemLocalId newId(new QOrganizerItemMaemo5EngineLocalId(newIdString.toUInt()));
                QOrganizerItemId id;
                id.setLocalId(newId);
                id.setManagerUri(managerUri());
                item->setId(id);

                // Modify alarm to contain the reminder information
                // (must always be done only after the component is saved)
                QPair<qint32, qint32> cookieChange = d->m_itemTransformer.modifyAlarmEvent(cal, item, component);
                d->m_dbAccess->fixAlarmCookie(cookieChange);

                // Update changeset
                if (calError == CALENDAR_ENTRY_DUPLICATED)
                    cs.insertChangedItem(item->localId());
                else
                    cs.insertAddedItem(item->localId());

                calError = CALENDAR_OPERATION_SUCCESSFUL; // reset the error
                *error = QOrganizerItemManager::NoError; // reset the error
            }
        }

        delete cevent;
        return calError;
    }
    else if (item->type() == QOrganizerItemType::TypeEventOccurrence)
    {
        QOrganizerEventOccurrence *eventOccurrence = static_cast<QOrganizerEventOccurrence *>(item);

        QOrganizerItem parentItem = parentOf(cal, eventOccurrence, error);
        if (*error == QOrganizerItemManager::NoError) {
            if (!parentItem.localId().isNull()) {
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

        return calError;
    }
    else if (item->type() == QOrganizerItemType::TypeTodo || item->type() == QOrganizerItemType::TypeTodoOccurrence) {
        CTodo* ctodo = static_cast<CTodo *>(component);
        QString ctodoId = QString::fromStdString(ctodo->getId());

        // A todo occurrence can never be a new item. If local id is empty, set it according
        // to the GUID.
        if (item->type() == QOrganizerItemType::TypeTodoOccurrence && ctodoId.isEmpty()) {
            calError = CALENDAR_OPERATION_SUCCESSFUL;
            std::vector<CTodo*> todosWithGuid = d->m_dbAccess->getTodos(cal->getCalendarId(), item->guid().toStdString(), calError);
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

            // increase the sequence number
            ctodo->setSequence(ctodo->getSequence() + 1);

            cal->modifyTodo(ctodo, calError);
            d->m_dbCache->invalidate();
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            if (*error == QOrganizerItemManager::NoError) {
                // Modify alarm to contain the reminder information
                // (must always be done only after the component is saved)
                QPair<qint32, qint32> cookieChange = d->m_itemTransformer.modifyAlarmEvent(cal, item, component);
                d->m_dbAccess->fixAlarmCookie(cookieChange);

                cs.insertChangedItem(item->localId());
            }
        }
        else {
            // CTodo ID is empty, the todo is new

            // set the sequence number zero
            ctodo->setSequence(0);

            cal->addTodo(ctodo, calError);
            d->m_dbCache->invalidate();
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
                QOrganizerItemLocalId newId(new QOrganizerItemMaemo5EngineLocalId(newIdString.toUInt()));
                QOrganizerItemId id;
                id.setLocalId(newId);
                id.setManagerUri(managerUri());
                item->setId(id);

                // Modify alarm to contain the reminder information
                // (must always be done only after the component is saved)
                QPair<qint32, qint32> cookieChange = d->m_itemTransformer.modifyAlarmEvent(cal, item, component);
                d->m_dbAccess->fixAlarmCookie(cookieChange);

                // Update changeset
                if (calError == CALENDAR_ENTRY_DUPLICATED)
                    cs.insertChangedItem(item->localId());
                else
                    cs.insertAddedItem(item->localId());

                calError = CALENDAR_OPERATION_SUCCESSFUL; // reset the error
                *error = QOrganizerItemManager::NoError; // reset the error
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

            // increase the sequence number
            cjournal->setSequence(cjournal->getSequence() + 1);

            cal->modifyJournal(cjournal, calError);
            d->m_dbCache->invalidate();
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            if (*error == QOrganizerItemManager::NoError) {
                // Modify alarm to contain the reminder information
                // (must always be done only after the component is saved)
                QPair<qint32, qint32> cookieChange = d->m_itemTransformer.modifyAlarmEvent(cal, item, component);
                d->m_dbAccess->fixAlarmCookie(cookieChange);

                cs.insertChangedItem(item->localId());
            }
        }
        else {
            // CJournal ID is empty, the journal is new

            // set the sequence number zero
            cjournal->setSequence(0);

            cal->addJournal(cjournal, calError);
            d->m_dbCache->invalidate();
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
                QOrganizerItemLocalId newId(new QOrganizerItemMaemo5EngineLocalId(newIdString.toUInt()));
                QOrganizerItemId id;
                id.setLocalId(newId);
                id.setManagerUri(managerUri());
                item->setId(id);

                // Modify alarm to contain the reminder information
                // (must always be done only after the component is saved)
                QPair<qint32, qint32> cookieChange = d->m_itemTransformer.modifyAlarmEvent(cal, item, component);
                d->m_dbAccess->fixAlarmCookie(cookieChange);

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

    return calError;
}

int QOrganizerItemMaemo5Engine::saveEventOccurrence(CCalendar *cal, QOrganizerEventOccurrence *occurrence, QOrganizerEvent *parent, QOrganizerItemChangeSet &cs, QOrganizerItemManager::Error *error)
{
    int calError = CALENDAR_OPERATION_SUCCESSFUL;

    // set occurrence GUID equal to the parent GUID
    occurrence->setGuid(parent->guid());

    // set occurrence's parent id
    occurrence->setParentLocalId(parent->localId());

    // backup the parent item
    QOrganizerEvent parentBackup(*parent);

    // did parent become modified
    bool parentModified = false;

    if (occurrence->localId().isNull()) {
        // Does not contain a local id => this is a generated occurrence or a new exception occurrence
        // After modifications this won't be saved anymore as a generated occurrence
        // but as an event to the calendar DB. Add an exception rule to the parent item
        // to leave out this one when generating occurrences.

        if (occurrence->originalDate().isValid() && occurrence->originalDate() != parent->startDateTime().date()) {
            // The original date is valid => this is a generated occurrence
            // First solve the original occurrence, using this occurrence's original date.
            // We need that to know the original length of the generated occurrence
            // (as it may have changed now).
            QDateTime originalPeriodStart = QDateTime(occurrence->originalDate(), QTime(0,0,0));
            QDateTime originalPeriodEnd = QDateTime(occurrence->originalDate(), QTime(23,59,59,999));
            QList<QOrganizerItem> parentsOccurrences = internalItemOccurrences(*parent, originalPeriodStart, originalPeriodEnd, 0, error);

            if (!parentsOccurrences.isEmpty() && occurrence->originalDate() != parent->startDateTime().date()) {
                QOrganizerEventOccurrence originalOccurrence = static_cast<QOrganizerEventOccurrence>(parentsOccurrences[0]);

                // Add exception dates to the parent
                QDate periodStart = originalOccurrence.startDateTime().date();
                QDate periodEnd = originalOccurrence.endDateTime().date();

                QSet<QDate> newExceptionDates = parent->exceptionDates();
                QDate exceptionDate = periodStart;
                while (exceptionDate <= periodEnd) {
                    if (!newExceptionDates.contains(exceptionDate))
                        newExceptionDates << exceptionDate;
                    exceptionDate = exceptionDate.addDays(1);
                }
                parent->setExceptionDates(newExceptionDates);
                parentModified = true;
            }
        }
        else {
            // The original date is not valid => this is a new exception occurrence, not generated
            // Add exception days covering the occurrence's period
            QDate periodStart = occurrence->startDateTime().date();
            QDate periodEnd = occurrence->endDateTime().date();

            QSet<QDate> newExceptionDates = parent->exceptionDates();
            QDate exceptionDate = periodStart;
            while (exceptionDate <= periodEnd) {
                if (!newExceptionDates.contains(exceptionDate))
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
    if (!component) {
        *error = QOrganizerItemManager::UnspecifiedError;
        return calError;
    }
    if (*error != QOrganizerItemManager::NoError) {
        delete component;
        return calError;
    }

    // save the parent back to the DB (if needed)
    if (parentModified) {
        calError = doSaveItem(cal, parent, cs, error);

        if (calError != CALENDAR_OPERATION_SUCCESSFUL) {
            // saving the parent modifications failed
            // we must not save the occurrence either
            delete component;
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            return calError;
        }
        if (*error != QOrganizerItemManager::NoError) {
            // saving the parent modifications failed
            // we must not save the occurrence either
            delete component;
            return calError;
        }
    }

    // Save occurrence
    CEvent* cevent = static_cast<CEvent *>(component);
    QString ceventId = QString::fromStdString(cevent->getId());
    if (!ceventId.isEmpty()) {
        // CEvent ID is not empty, the event already exists in calendar

        // increase the sequence number
        cevent->setSequence(cevent->getSequence() + 1);

        cal->modifyEvent(cevent, calError);
        d->m_dbCache->invalidate();
        *error = d->m_itemTransformer.calErrorToManagerError(calError);
        if (calError != CALENDAR_OPERATION_SUCCESSFUL && parentModified) {
            // occurrence saving failed, we must undo the parent changes too
            QOrganizerItemManager::Error ignoreError;
            doSaveItem(cal, &parentBackup, cs, &ignoreError);
        }
    }
    else {
        // CEvent ID is empty, the event is new

        // set the sequence number zero
        cevent->setSequence(0);

        cal->addEvent(cevent, calError);
        d->m_dbCache->invalidate();
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
            QOrganizerItemLocalId newId(new QOrganizerItemMaemo5EngineLocalId(newIdString.toUInt()));
            QOrganizerItemId id;
            id.setLocalId(newId);
            id.setManagerUri(managerUri());
            occurrence->setId(id);

            // Update changeset
            if (calError == CALENDAR_ENTRY_DUPLICATED)
                cs.insertChangedItem(occurrence->localId());
            else
                cs.insertAddedItem(occurrence->localId());

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
}

QOrganizerItem QOrganizerItemMaemo5Engine::parentOf(CCalendar *cal, QOrganizerItem *occurrence, QOrganizerItemManager::Error *error) const
{
    // the occurrence is supposed be valid when this method becomes called
    QOrganizerItemLocalId parentLocalId;
    if (occurrence->type() == QOrganizerItemType::TypeEventOccurrence) {
        QOrganizerEventOccurrence *eventOccurrence = static_cast<QOrganizerEventOccurrence *>(occurrence);
        if (!eventOccurrence->parentLocalId().isNull()) {
            // the simple case: parent local id was set
            parentLocalId = eventOccurrence->parentLocalId();
        }
        else {
            // parent local id was not set, fetch with [GUID,originalDate]
            int calError = CALENDAR_OPERATION_SUCCESSFUL;
            std::vector<CEvent*> parentCands = d->m_dbAccess->getEvents(cal->getCalendarId(), eventOccurrence->guid().toStdString(), calError);
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            std::vector<CEvent*>::iterator pCand;

            // Check if some event contains recurrence information and then that the given
            // occurrence is one of the occurrences obtained from the recurrence information.
            for (pCand = parentCands.begin(); pCand != parentCands.end(); ++pCand) {
                QOrganizerItemLocalId candidateLocalId(new QOrganizerItemMaemo5EngineLocalId(QString::fromStdString((*pCand)->getId()).toUInt()));

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
                                    internalItemOccurrences(*parentCandidateEvent, periodStart, periodEnd, 0, error);
                            if (*error != QOrganizerItemManager::NoError)
                                return QOrganizerItem(); // error occured

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

            if (!parentLocalId.isNull())
            {
                // Parent local id got, just free the resources
                for (pCand = parentCands.begin(); pCand != parentCands.end(); ++pCand)
                    delete *pCand;
            }
            else {
                // None of the parent candidates contained recurrence information
                // => return the earliest created event as parent

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
		    QOrganizerItemLocalId resolvedParentId(new QOrganizerItemMaemo5EngineLocalId(QString::fromStdString(firstCreated->getId()).toUInt()));
                    parentLocalId = resolvedParentId; // assignment operator will automatically delete the d if it exists, therefore avoids leaking memory.
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
    Q_UNUSED(fetchHint); // there are never large binary blobs etc. attached to maemo5 calendar items

    // no matter which calendar is used, all the items are available in all the calendars
    CCalendar *cal = d->m_mcInstance->getDefaultCalendar();

    const int EVENT_TYPE = 1;
    const int TODO_TYPE = 2;
    const int JOURNAL_TYPE = 3;

    // Get calendar id (or -1) of the target item
    int calId = d->m_dbAccess->calIdOf(itemId);
    QOrganizerCollectionLocalId collectionLocalId(new QOrganizerCollectionMaemo5EngineLocalId(calId));

    // Build collection id
    QOrganizerCollectionId collectionId;
    collectionId.setManagerUri(managerUri());
    collectionId.setLocalId(collectionLocalId);

    std::string nativeId = QString::number(readItemLocalId(itemId)).toStdString();
    int calError = CALENDAR_OPERATION_SUCCESSFUL;

    CEvent *cevent = 0;
    if (d->m_dbAccess->typeOf(itemId) == EVENT_TYPE)
        cevent = d->m_dbAccess->getEvent(cal, nativeId, calError);
    else
        calError = CALENDAR_NONE_INDB;

    *error = d->m_itemTransformer.calErrorToManagerError(calError);
    if (cevent) {
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
            QOrganizerEvent retn = d->m_itemTransformer.convertCEventToQEvent(cevent);
            d->m_itemTransformer.fillInCommonCComponentDetails(&retn, cevent);
            delete cevent;
            cleanupCal(cal);

            // Set calendar (collection) id
	    retn.setCollectionId(collectionId);

            return retn;
        }
        else {
            QOrganizerEventOccurrence retn = d->m_itemTransformer.convertCEventToQEventOccurrence(cevent);
            d->m_itemTransformer.fillInCommonCComponentDetails(&retn, cevent);

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

            // Set calendar (collection) id
	    retn.setCollectionId(collectionId);

            return retn;
        }
    }

    calError = CALENDAR_OPERATION_SUCCESSFUL; // reset error
    CTodo *todo = 0;
    if (d->m_dbAccess->typeOf(itemId) == TODO_TYPE)
        todo = d->m_dbAccess->getTodo(cal, nativeId, calError);
    else
        calError = CALENDAR_NONE_INDB;


    *error = d->m_itemTransformer.calErrorToManagerError(calError);
    if (todo) {
        QOrganizerTodo retn = d->m_itemTransformer.convertCTodoToQTodo(todo);
        d->m_itemTransformer.fillInCommonCComponentDetails(&retn, todo);
        delete todo;
        cleanupCal(cal);

        // Set calendar (collection) id
	retn.setCollectionId(collectionId);

        return retn;
    }

    calError = CALENDAR_OPERATION_SUCCESSFUL; // reset error
    CJournal *journal = 0;
    if (d->m_dbAccess->typeOf(itemId) == JOURNAL_TYPE)
        journal = d->m_dbAccess->getJournal(cal, nativeId, calError);
    else
        calError = CALENDAR_NONE_INDB;


    *error = d->m_itemTransformer.calErrorToManagerError(calError);
    if (journal) {
        QOrganizerJournal retn = d->m_itemTransformer.convertCJournalToQJournal(journal);
        d->m_itemTransformer.fillInCommonCComponentDetails(&retn, journal);
        delete journal;
        cleanupCal(cal);

        // Set calendar (collection) id
	retn.setCollectionId(collectionId);

        return retn;
    }

    // In an error situation return an invalid item
    cleanupCal(cal);
    *error = QOrganizerItemManager::DoesNotExistError;
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
        // if item contains recurrence information, it can't be an occurrence
        if (containsRecurrenceInformation(ccomponent))
            return false;

        // if no duplicate GUIDs are found, event can't be an occurrence
        int calError = CALENDAR_OPERATION_SUCCESSFUL;
        std::vector<CEvent*> eventsWithGuid = d->m_dbAccess->getEvents(cal->getCalendarId(), guid.toStdString(), calError);
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
        std::vector<CEvent*> eventsWithGuid = d->m_dbAccess->getEvents(cal->getCalendarId(), guid.toStdString(), calError);
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

CCalendar* QOrganizerItemMaemo5Engine::getCalendar(QOrganizerCollectionLocalId collectionId, QOrganizerItemManager::Error *error) const
{
    CCalendar *cal = 0;
    int calError = CALENDAR_OPERATION_SUCCESSFUL;
    ////
    if (collectionId.isNull())
        cal = d->m_mcInstance->getDefaultCalendar();
    else
        cal = d->m_mcInstance->getCalendarById(readCollectionLocalId(collectionId), calError);

    if (!cal || calError != CALENDAR_OPERATION_SUCCESSFUL)
        *error = QOrganizerItemManager::InvalidCollectionError;

    return cal;
}

QSet<QOrganizerCollectionId> QOrganizerItemMaemo5Engine::extractCollectionIds(const QOrganizerItemFilter& filter) const
{
    QSet<QOrganizerCollectionId> retn;

    QOrganizerItemFilter::FilterType type = filter.type();
    switch(type) {
    case QOrganizerItemFilter::CollectionFilter: {
        QOrganizerItemCollectionFilter collectionFilter = static_cast<QOrganizerItemCollectionFilter>(filter);
        QSet<QOrganizerCollectionLocalId> tmp = collectionFilter.collectionIds();
        foreach(QOrganizerCollectionLocalId curr, tmp) {
            QOrganizerCollectionId newCollectionId;
            newCollectionId.setManagerUri(managerUri());
            newCollectionId.setLocalId(curr);
            retn << newCollectionId;
        }

        break;
    }
    case QOrganizerItemFilter::IntersectionFilter: {
        QOrganizerItemIntersectionFilter intersectionFilter = static_cast<QOrganizerItemIntersectionFilter>(filter);
        QList<QOrganizerItemFilter> filters = intersectionFilter.filters();
        if (!filters.isEmpty()) {
            retn = extractCollectionIds(filters.at(0));
            for (int i = 1; i < filters.count(); ++i) {
                QSet<QOrganizerCollectionId> subset = extractCollectionIds(filters.at(i));
                // do not intersect empty subsets, because there can be other than collection filters that produce them
                if (!subset.empty())
                    retn.intersect(subset);
            }
        }
        break;
    }
    case QOrganizerItemFilter::UnionFilter: {
        QOrganizerItemUnionFilter unionFilter = static_cast<QOrganizerItemUnionFilter>(filter);
        QList<QOrganizerItemFilter> filters = unionFilter.filters();
        if (!filters.isEmpty()) {
            retn = extractCollectionIds(filters.at(0));
            for (int i = 1; i < filters.count(); ++i)
                retn.unite(extractCollectionIds(filters.at(i)));
        }
        break;
    }
    default:
        break;
    }
    return retn;
}

QSet<QOrganizerCollectionLocalId> QOrganizerItemMaemo5Engine::extractCollectionLocalIds(const QOrganizerItemFilter& filter) const
{
    QSet<QOrganizerCollectionLocalId> retn;

    QOrganizerItemFilter::FilterType type = filter.type();
    switch(type) {
    case QOrganizerItemFilter::CollectionFilter: {
        QOrganizerItemCollectionFilter collectionFilter = static_cast<QOrganizerItemCollectionFilter>(filter);
        retn = collectionFilter.collectionIds();
        break;
    }
    case QOrganizerItemFilter::IntersectionFilter: {
        QOrganizerItemIntersectionFilter intersectionFilter = static_cast<QOrganizerItemIntersectionFilter>(filter);
        QList<QOrganizerItemFilter> filters = intersectionFilter.filters();
        if (!filters.isEmpty()) {
            retn = extractCollectionLocalIds(filters.at(0));
            for (int i = 1; i < filters.count(); ++i) {
                QSet<QOrganizerCollectionLocalId> subset = extractCollectionLocalIds(filters.at(i));
                // do not intersect empty subsets, because there can be other than collection filters that produce them
                if (!subset.empty())
                    retn.intersect(subset);
            }
        }
        break;
    }
    case QOrganizerItemFilter::UnionFilter: {
        QOrganizerItemUnionFilter unionFilter = static_cast<QOrganizerItemUnionFilter>(filter);
        QList<QOrganizerItemFilter> filters = unionFilter.filters();
        if (!filters.isEmpty()) {
            retn = extractCollectionLocalIds(filters.at(0));
            for (int i = 1; i < filters.count(); ++i)
                retn.unite(extractCollectionLocalIds(filters.at(i)));
        }
        break;
    }
    default:
        break;
    }
    return retn;
}
