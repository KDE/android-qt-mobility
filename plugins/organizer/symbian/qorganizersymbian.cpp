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
//system includes
#include <calcommon.h>
#include <calsession.h>
#include <calchangecallback.h>
#include <calentryview.h>
#include <calinstanceview.h>
#include <calrrule.h>
#ifdef SYMBIAN_CALENDAR_V2
#include <QColor>
#include <calcalendariterator.h>
#include <calcalendarinfo.h>
// This file (calenmulticaluids.hrh) no longer exists in S^4, so use a local 
// copy for now
#include "local_calenmulticaluids.hrh"
#endif

// user includes
#include "qorganizersymbian_p.h"
#include "qtorganizer.h"
#include "organizeritemtypetransform.h"
#include "organizeritemguidtransform.h"
#include "qorganizeritemrequestqueue.h"
#include "organizersymbianutils.h"
#include "resetanddestroy.h"

using namespace OrganizerSymbianUtils;

QOrganizerItemSymbianEngineId::QOrganizerItemSymbianEngineId()
    : QOrganizerItemEngineId(), m_localCollectionId(0), m_localItemId(0)
{
}

QOrganizerItemSymbianEngineId::QOrganizerItemSymbianEngineId(quint64 collectionId, quint32 itemId)
    : QOrganizerItemEngineId(), m_localCollectionId(collectionId), m_localItemId(itemId)
{
}

QOrganizerItemSymbianEngineId::~QOrganizerItemSymbianEngineId()
{
}

QOrganizerItemSymbianEngineId::QOrganizerItemSymbianEngineId(const QOrganizerItemSymbianEngineId& other)
    : QOrganizerItemEngineId(), m_localCollectionId(other.m_localCollectionId), m_localItemId(other.m_localItemId)
{
}

bool QOrganizerItemSymbianEngineId::isEqualTo(const QOrganizerItemEngineId* other) const
{
    quint64 otherlocalCollectionId = static_cast<const QOrganizerItemSymbianEngineId*>(other)->m_localCollectionId;
    quint32 otherlocalItemId = static_cast<const QOrganizerItemSymbianEngineId*>(other)->m_localItemId;
    if (m_localCollectionId != otherlocalCollectionId)
        return false;
    if (m_localItemId != otherlocalItemId)
        return false;
    return true;
}

bool QOrganizerItemSymbianEngineId::isLessThan(const QOrganizerItemEngineId* other) const
{
    // order by collection, then by item in collection.
    quint64 otherlocalCollectionId = static_cast<const QOrganizerItemSymbianEngineId*>(other)->m_localCollectionId;
    quint32 otherlocalItemId = static_cast<const QOrganizerItemSymbianEngineId*>(other)->m_localItemId;
    if (m_localCollectionId < otherlocalCollectionId)
        return true;
    if (m_localCollectionId == otherlocalCollectionId)
        return (m_localItemId < otherlocalItemId);
    return false;
}

QString QOrganizerItemSymbianEngineId::managerUri() const
{
    // TODO: make this return the actual managerUri (including params) of the
    // engine it is associated with
    static const QString managerUri(QLatin1String("qtorganizer:symbian:"));
    return managerUri;
}

QOrganizerItemEngineId* QOrganizerItemSymbianEngineId::clone() const
{
    QOrganizerItemSymbianEngineId *myClone = new QOrganizerItemSymbianEngineId;
    myClone->m_localCollectionId = m_localCollectionId;
    myClone->m_localItemId = m_localItemId;
    return myClone;
}

QString QOrganizerItemSymbianEngineId::toString() const
{
    return QString::fromAscii("%1:%2").arg(m_localCollectionId).arg(m_localItemId);
}

#ifndef QT_NO_DEBUG_STREAM
QDebug& QOrganizerItemSymbianEngineId::debugStreamOut(QDebug& dbg) const
{
    dbg.nospace() << "QOrganizerItemSymbianEngineId(" << m_localCollectionId << ", " << m_localItemId << ")";
    return dbg.maybeSpace();
}
#endif

uint QOrganizerItemSymbianEngineId::hash() const
{
    // Note: doesn't need to be unique, since == ensures difference.
    // hash function merely determines distribution in a hash table.
    // TODO: collection id is 64 bit
    quint32 combinedLocalId = m_localItemId;
    combinedLocalId <<= 32;
    combinedLocalId += m_localCollectionId;
    return uint(((combinedLocalId >> (8 * sizeof(uint) - 1)) ^ combinedLocalId) & (~0U));
}

QOrganizerCollectionSymbianEngineId::QOrganizerCollectionSymbianEngineId()
    : QOrganizerCollectionEngineId(), m_localCollectionId(0)
{
}

QOrganizerCollectionSymbianEngineId::QOrganizerCollectionSymbianEngineId(quint64 collectionId)
    : QOrganizerCollectionEngineId(), m_localCollectionId(collectionId)
{
}

QOrganizerCollectionSymbianEngineId::QOrganizerCollectionSymbianEngineId(const QOrganizerCollectionSymbianEngineId& other)
    : QOrganizerCollectionEngineId(), m_localCollectionId(other.m_localCollectionId)
{
}

QOrganizerCollectionSymbianEngineId::~QOrganizerCollectionSymbianEngineId()
{
}

bool QOrganizerCollectionSymbianEngineId::isEqualTo(const QOrganizerCollectionEngineId* other) const
{
    quint64 otherlocalCollectionId = static_cast<const QOrganizerCollectionSymbianEngineId*>(other)->m_localCollectionId;
    if (m_localCollectionId != otherlocalCollectionId)
        return false;
    return true;
}

bool QOrganizerCollectionSymbianEngineId::isLessThan(const QOrganizerCollectionEngineId* other) const
{
    // order by collection, then by item in collection.
    quint64 otherlocalCollectionId = static_cast<const QOrganizerCollectionSymbianEngineId*>(other)->m_localCollectionId;
    if (m_localCollectionId < otherlocalCollectionId)
        return true;
    return false;
}

QString QOrganizerCollectionSymbianEngineId::managerUri() const
{
    // TODO: make this return the actual managerUri (including params) of the
    // engine it is associated with
    static const QString managerUri(QLatin1String("qtorganizer:symbian:"));
    return managerUri;
}

QOrganizerCollectionEngineId* QOrganizerCollectionSymbianEngineId::clone() const
{
    QOrganizerCollectionSymbianEngineId *myClone = new QOrganizerCollectionSymbianEngineId;
    myClone->m_localCollectionId = m_localCollectionId;
    return myClone;
}

QString QOrganizerCollectionSymbianEngineId::toString() const
{
    return QString::number(m_localCollectionId);
}

#ifndef QT_NO_DEBUG_STREAM
QDebug& QOrganizerCollectionSymbianEngineId::debugStreamOut(QDebug& dbg) const
{
    dbg.nospace() << "QOrganizerCollectionSymbianEngineId(" << m_localCollectionId << ")";
    return dbg.maybeSpace();
}
#endif

uint QOrganizerCollectionSymbianEngineId::hash() const
{
    return QT_PREPEND_NAMESPACE(qHash)(m_localCollectionId);
}


// Special (internal) error code to be used when an item occurrence is not
// valid. The error code is not expected to clash with any symbian calendar
// API errors.
const TInt KErrInvalidOccurrence(-32768);
const TInt KErrInvalidItemType(-32769);

QOrganizerManagerEngine* QOrganizerItemSymbianFactory::engine(
    const QMap<QString, QString>& parameters, 
    QOrganizerManager::Error* error)
{
    Q_UNUSED(parameters);

    // manager takes ownership and will clean up.
    QOrganizerItemSymbianEngine* ret = new QOrganizerItemSymbianEngine();
    TRAPD(err, ret->initializeL());
    QOrganizerItemSymbianEngine::transformError(err, error);
    if (*error != QOrganizerManager::NoError) {
        // Something went wrong. Return null so that 
        // QOrganizerManagerData::createEngine() will return 
        // QOrganizerItemInvalidEngine to the client. This will avoid null 
        // pointer exceptions if the client still tries to access the manager.
        delete ret;
        ret = 0;
    }
    
    return ret;
}

QOrganizerItemEngineId* QOrganizerItemSymbianFactory::createItemEngineId(const QMap<QString, QString>& parameters, const QString& engineIdString) const
{
    Q_UNUSED(parameters);
    QStringList parts(engineIdString.split(QLatin1String(":")));
    if (parts.size() != 2)
        return NULL;

    bool ok = true;
    quint64 collectionid = parts[0].toULongLong(&ok);
    if (!ok)
        return NULL;
    quint32 itemId = parts[1].toUInt(&ok);
    if (!ok)
        return NULL;
    return new QOrganizerItemSymbianEngineId(collectionid, itemId);
}

QOrganizerCollectionEngineId* QOrganizerItemSymbianFactory::createCollectionEngineId(const QMap<QString, QString>& parameters, const QString& engineIdString) const
{
    Q_UNUSED(parameters);
    bool ok = true;
    quint64 collectionid = engineIdString.toULongLong(&ok);
    if (!ok)
        return NULL;
    else
        return new QOrganizerCollectionSymbianEngineId(collectionid);
}

QString QOrganizerItemSymbianFactory::managerName() const
{
    return QLatin1String("symbian");
}
Q_EXPORT_PLUGIN2(qtorganizer_symbian, QOrganizerItemSymbianFactory);

QOrganizerItemSymbianEngine::QOrganizerItemSymbianEngine() :
    QOrganizerManagerEngine(),
    m_defaultCollection(this),
    m_requestServiceProviderQueue(0)
{

}

void QOrganizerItemSymbianEngine::initializeL()
{
    // Open the default collection
    m_defaultCollection.openL(KNullDesC);
    m_collections.insert(m_defaultCollection.id(), m_defaultCollection);
    m_defaultCollection.createViewsL();

#ifdef SYMBIAN_CALENDAR_V2
    // Start listening to calendar file changes
    m_defaultCollection.calSession()->StartFileChangeNotificationL(*this);
    
    // Load available calendars
    CCalCalendarIterator *iterator = CCalCalendarIterator::NewL(*m_defaultCollection.calSession());
    CleanupStack::PushL(iterator);
    for (CCalCalendarInfo *calInfo=iterator->FirstL(); calInfo != 0; calInfo=iterator->NextL()) {
        
        CleanupStack::PushL(calInfo);

        // Skip calendars which are marked for deletion
        TBool markAsDelete = EFalse;
        TRAP_IGNORE(markAsDelete = getCalInfoPropertyL<TBool>(*calInfo, EMarkAsDelete));
        if (markAsDelete) {
            CleanupStack::PopAndDestroy(calInfo);
            continue;
        }

        // Skip default calendar (already loaded)
        QString fileName = toQString(calInfo->FileNameL());
        if (fileName.compare(m_defaultCollection.fileName(), Qt::CaseInsensitive) == 0) {
            CleanupStack::PopAndDestroy(calInfo);
            continue;
        }
        
        // Open a new session to the calendar
        OrganizerSymbianCollection collection(this);
        collection.openL(calInfo->FileNameL());
        m_collections.insert(collection.id(), collection);
        collection.createViewsL();

        CleanupStack::PopAndDestroy(calInfo);
    }
    CleanupStack::PopAndDestroy(iterator);
#endif
    // Create request queue for asynch requests
    m_requestServiceProviderQueue = QOrganizerItemRequestQueue::instance(*this);
}

QOrganizerItemSymbianEngine::~QOrganizerItemSymbianEngine()
{
	delete m_requestServiceProviderQueue;
}

QString QOrganizerItemSymbianEngine::managerName() const
{
    return QLatin1String("symbian");
}

QMap<QString, QString> QOrganizerItemSymbianEngine::managerParameters() const
{
    /* TODO - in case you have any actual parameters that are relevant that you saved in the factory method, return them here */
    return QMap<QString, QString>();
}

int QOrganizerItemSymbianEngine::managerVersion() const
{
    // This is strictly defined by the engine, so we can return whatever we like
    return 1;
}

QList<QOrganizerItem> QOrganizerItemSymbianEngine::itemOccurrences(
    const QOrganizerItem& parentItem,
    const QDateTime& periodStart,
    const QDateTime& periodEnd,
    int maxCount,
    const QOrganizerItemFetchHint& fetchHint,
    QOrganizerManager::Error* error) const
{
    QList<QOrganizerItem> itemOccurrences;
    TRAPD(err, itemOccurrences = itemOccurrencesL(parentItem.id(), periodStart, periodEnd, maxCount, fetchHint));
    transformError(err, error);
    return itemOccurrences;
}

QList<QOrganizerItem> QOrganizerItemSymbianEngine::itemOccurrencesL(
    const QOrganizerItemId &parentItemId,
    const QDateTime &periodStart,
    const QDateTime &periodEnd,
    int maxCount,
    const QOrganizerItemFetchHint& fetchHint) const
{
    Q_UNUSED(fetchHint)

    //Verify maximum number of items requested
    if (0 == maxCount)
    {
        //Return an empty occurrence list if maxCount is 0
        return QList<QOrganizerItem>();
    }

    // Verify that parent item exists
    QOrganizerManager::Error error;
    QOrganizerItem parentItem = this->item(parentItemId, QOrganizerItemFetchHint(), &error);
    if (error != QOrganizerManager::NoError)
        return QList<QOrganizerItem>(); // return an empty occurrence list if parent item is not found

    // Verify time range
    if (periodStart.isValid() && periodEnd.isValid() && periodEnd < periodStart)
        User::Leave(KErrArgument);

    // Set cal view filter based on the item type
    CalCommon::TCalViewFilter filter(0);
    if (parentItem.type() == QOrganizerItemType::TypeEvent) {
        filter = CalCommon::EIncludeAppts | CalCommon::EIncludeEvents;
    } else if (parentItem.type() == QOrganizerItemType::TypeTodo) {
        filter = (CalCommon::EIncludeCompletedTodos | CalCommon::EIncludeIncompletedTodos);
    } else {
        User::Leave(KErrInvalidItemType);
    }

    // If start time is not defined, use minimum start date
    TCalTime startTime;
    startTime.SetTimeUtcL(TCalTime::MinTime());
    if (periodStart.isValid())
        startTime.SetTimeLocalL(toTTime(periodStart, Qt::LocalTime));

    // If end date is not defined, use maximum end date
    TCalTime endTime;
    endTime.SetTimeUtcL(TCalTime::MaxTime());
    if (periodEnd.isValid())
        endTime.SetTimeLocalL(toTTime(periodEnd, Qt::LocalTime));

    // Find instances
    RPointerArray<CCalInstance> instanceList;
    CleanupResetAndDestroyPushL(instanceList);
    m_collections[parentItem.collectionId()].calInstanceView()->FindInstanceL(
        instanceList, filter, CalCommon::TCalTimeRange(startTime, endTime));
    
    // Transform CCalInstances to QOrganizerItems
    QList<QOrganizerItem> itemOccurrences;
    toItemOccurrencesL(instanceList, parentItem, maxCount, parentItem.collectionId(), itemOccurrences);
    
    CleanupStack::PopAndDestroy(&instanceList);
    
    return itemOccurrences;
}

QList<QOrganizerItem> QOrganizerItemSymbianEngine::items(
    const QDateTime& periodStart,
    const QDateTime& periodEnd,
    const QOrganizerItemFilter& filter,
    const QList<QOrganizerItemSortOrder>& sortOrders,
    const QOrganizerItemFetchHint& fetchHint,
    QOrganizerManager::Error* error) const
{
    QList<QOrganizerItem> items;
    TRAPD(err, items = itemOccurrencesL(periodStart, periodEnd, filter, sortOrders, fetchHint));
    transformError(err, error);
    return items;
}

QList<QOrganizerItem> QOrganizerItemSymbianEngine::itemOccurrencesL(
    const QDateTime& periodStart,
    const QDateTime& periodEnd,
    const QOrganizerItemFilter &filter,
    const QList<QOrganizerItemSortOrder> &sortOrders,
    const QOrganizerItemFetchHint &fetchHint) const
{
    // TODO: It might be possible to optimize by using fetch hint
    Q_UNUSED(fetchHint);
    
    // Verify time range
    if (periodStart.isValid() && periodEnd.isValid() && periodEnd < periodStart)
        User::Leave(KErrArgument);  

    // If start time is not defined, use minimum start date
    TCalTime startTime;
    startTime.SetTimeUtcL(TCalTime::MinTime());
    if (periodStart.isValid())
        startTime.SetTimeLocalL(toTTime(periodStart, Qt::LocalTime));

    // If end date is not defined, use maximum end date
    TCalTime endTime;
    endTime.SetTimeUtcL(TCalTime::MaxTime());
    if (periodEnd.isValid())
        endTime.SetTimeLocalL(toTTime(periodEnd, Qt::LocalTime));

    // Loop through all the instance views and fetch the item instances
    QList<QOrganizerItem> items;
    QList<OrganizerSymbianCollection> collections = filteredSymbianCollectionsL(filter);
    foreach (OrganizerSymbianCollection collection, collections) {
        
        // Get instances
        RPointerArray<CCalInstance> instanceList;
        CleanupResetAndDestroyPushL(instanceList);
        collection.calInstanceView()->FindInstanceL(
            instanceList, CalCommon::EIncludeAll, CalCommon::TCalTimeRange(startTime, endTime));
        
        // Transform CCalInstances to QOrganizerItems
        toItemOccurrencesL(instanceList, QOrganizerItem(), -1, collection.id(), items);
        
        CleanupStack::PopAndDestroy(&instanceList);
    }

    // Use the general implementation to filter and sort items
    return slowFilterItems(items, filter, sortOrders);
}

void QOrganizerItemSymbianEngine::toItemOccurrencesL(
    const RPointerArray<CCalInstance> &calInstanceList,
    QOrganizerItem parentItem,
    const int maxCount,
    QOrganizerCollectionId collectionId,
    QList<QOrganizerItem> &itemOccurrences) const
{
    quint64 localCollectionIdValue = m_collections[collectionId].calCollectionId();

    // Counter for the found match items
    int count = 0;
    // Transform all the instances to QOrganizerItems
    for(int i(0); i < calInstanceList.Count(); i++) {
        QOrganizerItem itemOccurrence;
        CCalInstance* calInstance = calInstanceList[i];
        m_itemTransform.toItemOccurrenceL(*calInstance, &itemOccurrence);

        // Optimization: if a parent item instance is defined, skip the instances that do not match
        if (!parentItem.isEmpty() && parentItem.guid() != itemOccurrence.guid())
            continue;

        // Found one match item
        count++;
        // Check if maxCount limit is reached
        if (maxCount > 0 && count > maxCount)
            break;

        // Set local id if this is either an exceptional item or a non-recurring item
        CCalEntry &entry = calInstance->Entry();
        bool isException = entry.RecurrenceIdL().TimeUtcL() != Time::NullTTime();
        TCalRRule rrule;
        bool isRecurring = entry.GetRRuleL(rrule);
        if (isException || !isRecurring) {
            QOrganizerItemId itemId = QOrganizerItemId(new QOrganizerItemSymbianEngineId(
                localCollectionIdValue, calInstance->Entry().LocalUidL()));
            itemOccurrence.setId(itemId);
        }

        // Set instance origin, the detail is set here because transform classes are not aware of
        // the required APIs
        if (isException || isRecurring) {
            TCalLocalUid parentLocalUid(0);
            if (isException) {
                HBufC8* globalUid = OrganizerItemGuidTransform::guidLC(itemOccurrence);
                CCalEntry *parentEntry = findParentEntryLC(collectionId, itemOccurrence, *globalUid);
                parentLocalUid = parentEntry->LocalUidL();
                CleanupStack::PopAndDestroy(parentEntry);
                CleanupStack::PopAndDestroy(globalUid);
            } else {
                parentLocalUid = calInstance->Entry().LocalUidL();
            }
            QOrganizerItemParent origin(itemOccurrence.detail<QOrganizerItemParent>());
            origin.setParentId(toItemId(localCollectionIdValue, parentLocalUid));
            origin.setOriginalDate(toQDateTimeL(calInstance->StartTimeL()).date());
            itemOccurrence.saveDetail(&origin);
        }

        // Set collection id
        itemOccurrence.setCollectionId(collectionId);

        itemOccurrences.append(itemOccurrence);
    }
}

QList<QOrganizerItemId> QOrganizerItemSymbianEngine::itemIds(
        const QDateTime& periodStart,
        const QDateTime& periodEnd,
        const QOrganizerItemFilter& filter,
        const QList<QOrganizerItemSortOrder>& sortOrders,
        QOrganizerManager::Error* error) const
{
    QList<QOrganizerItemId> ids;
    TRAPD(err, ids = itemIdsL(periodStart, periodEnd, filter, sortOrders))
    transformError(err, error);
    return ids;
}

QList<QOrganizerItemId> QOrganizerItemSymbianEngine::itemIdsL(
    const QDateTime& periodStart,
    const QDateTime& periodEnd,
    const QOrganizerItemFilter& filter, 
    const QList<QOrganizerItemSortOrder>& sortOrders) const
{
    QList<OrganizerSymbianCollection> collections = filteredSymbianCollectionsL(filter);   
    QList<QOrganizerItemId> itemIds = itemIdsL(collections, periodStart, periodEnd);
    return slowFilterIdsL(itemIds, filter, sortOrders);
}

QList<QOrganizerItemId> QOrganizerItemSymbianEngine::itemIdsL(
    const QList<OrganizerSymbianCollection> &collections,
    const QDateTime& periodStart,
    const QDateTime& periodEnd) const
{
    // Verify time range
    if (periodStart.isValid() && periodEnd.isValid() && periodEnd < periodStart)
        User::Leave(KErrArgument);
    
    QSet<QOrganizerItemId> itemIds;
    foreach (OrganizerSymbianCollection collection, collections) {

        if (!periodStart.isValid() && !periodEnd.isValid()) {

            // Time range is not defined. So we can just return all entries. 
            // NOTE: this a lot faster than using instance view

            // Get all entries
            TCalTime minTime;
            minTime.SetTimeUtcL(TCalTime::MinTime());
            RArray<TCalLocalUid> ids;
            CleanupClosePushL(ids);
            collection.calEntryView()->GetIdsModifiedSinceDateL(minTime, ids);

            // Convert to item id's
            int count = ids.Count();
            for (int i=0; i<count; i++)
                itemIds << toItemId(collection.calCollectionId(), ids[i]);

            CleanupStack::PopAndDestroy(&ids);

        } else {

            // Time range is defined so we need to use instance view to find
            // all entries in the range. For example an item which starts before
            // periodStart might have an occurrence in the range. We cannot catch 
            // that through CCalEntryView.
            // NOTE: If the client does not define periodEnd we might get
            // a huge amount of instances!

            // If start time is not defined, use minimum start date
            TCalTime startTime;
            startTime.SetTimeUtcL(TCalTime::MinTime());
            if (periodStart.isValid())
                startTime.SetTimeLocalL(toTTime(periodStart, Qt::LocalTime));

            // If end date is not defined, use maximum end date
            TCalTime endTime;
            endTime.SetTimeUtcL(TCalTime::MaxTime());
            if (periodEnd.isValid())
                endTime.SetTimeLocalL(toTTime(periodEnd, Qt::LocalTime));

            // Find instances
            RPointerArray<CCalInstance> instances;
            CleanupResetAndDestroyPushL(instances);
            collection.calInstanceView()->FindInstanceL(
                instances, CalCommon::EIncludeAll, CalCommon::TCalTimeRange(startTime, endTime));

            // Get parent entry ids from instances
            for (int i=0; i<instances.Count(); i++) {
                QOrganizerItemId id = toItemId(collection.calCollectionId(), instances[i]->Entry().LocalUidL());
                if (!itemIds.contains(id))
                    itemIds << id;
            }

            CleanupStack::PopAndDestroy(&instances);
        }
    }
    return itemIds.toList();
}
QList<QOrganizerItem> QOrganizerItemSymbianEngine::itemsForExport(
    const QDateTime& periodStart,
    const QDateTime& periodEnd,
    const QOrganizerItemFilter& filter, 
    const QList<QOrganizerItemSortOrder>& sortOrders, 
    const QOrganizerItemFetchHint& fetchHint, 
    QOrganizerManager::Error* error) const
{
    Q_UNUSED(fetchHint);
    QList<QOrganizerItem> itemsList;
    TRAPD(err, itemsList = itemsForExportL(periodStart, periodEnd, filter, sortOrders));
    transformError(err, error);
    return itemsList;
}

QList<QOrganizerItem> QOrganizerItemSymbianEngine::itemsForExportL( 
    const QDateTime& periodStart,
    const QDateTime& periodEnd,
    const QOrganizerItemFilter& filter, 
    const QList<QOrganizerItemSortOrder>& sortOrders) const
{
    QList<OrganizerSymbianCollection> collections = filteredSymbianCollectionsL(filter);   
    QList<QOrganizerItemId> itemIds = itemIdsL(collections, periodStart, periodEnd);
    return slowFilterIdsToItemsL(itemIds, filter, sortOrders);
}

QOrganizerItem QOrganizerItemSymbianEngine::item(
    const QOrganizerItemId& itemId, 
    const QOrganizerItemFetchHint& fetchHint, 
    QOrganizerManager::Error* error) const
{
    QOrganizerItem item;
    TRAPD(err, item = itemL(itemId, fetchHint));
    transformError(err, error);
    return item;
}

QOrganizerItem QOrganizerItemSymbianEngine::itemL(const QOrganizerItemId& itemId, 
    const QOrganizerItemFetchHint& fetchHint) const
{
	Q_UNUSED(fetchHint)

    if (itemId.managerUri() != managerUri()) // XXX TODO: cache managerUri for fast lookup.
        User::Leave(KErrNotFound);

    // Check collection id
    QOrganizerCollectionId collectionLocalId = getCollectionId(itemId);
    if (!m_collections.contains(collectionLocalId))
        User::Leave(KErrNotFound);
    
    // Get entry from collection
    TCalLocalUid calLocalId = toTCalLocalUid(itemId);
    CCalEntry *calEntry = m_collections[collectionLocalId].calEntryView()->FetchL(calLocalId);
    if (!calEntry)
        User::Leave(KErrNotFound);
    CleanupStack::PushL(calEntry);

    // Transform CCalEntry -> QOrganizerItem
    QOrganizerItem item;
    m_itemTransform.toItemL(*calEntry, &item);
    
    // Set instance origin
    if (item.type() == QOrganizerItemType::TypeEventOccurrence
        || item.type() == QOrganizerItemType::TypeTodoOccurrence) {
        HBufC8* globalUid = OrganizerItemGuidTransform::guidLC(item);
        quint64 localCollectionIdValue = m_collections[collectionLocalId].calCollectionId();
        CCalEntry *parentEntry = findParentEntryLC(collectionLocalId, item, *globalUid);

        // Set instance origin, the detail is set here because the corresponding transform class
        // does not know the required values
        QOrganizerItemParent origin(item.detail<QOrganizerItemParent>());
        origin.setParentId(toItemId(localCollectionIdValue, parentEntry->LocalUidL()));
        origin.setOriginalDate(toQDateTimeL(calEntry->StartTimeL()).date());
        item.saveDetail(&origin);

        CleanupStack::PopAndDestroy(parentEntry);
        CleanupStack::PopAndDestroy(globalUid);
    }
    CleanupStack::PopAndDestroy(calEntry);
    
    // Set item id
    item.setId(itemId);

    // Set collection id
    item.setCollectionId(collectionLocalId);
    
    return item;
}

bool QOrganizerItemSymbianEngine::saveItems(QList<QOrganizerItem> *items, 
    QMap<int, QOrganizerManager::Error> *errorMap, 
    QOrganizerManager::Error* error)
{
    // TODO: the performance would be probably better, if we had a separate
    // implementation for the case with a list of items that would save all
    // the items
    
    QOrganizerItemChangeSet changeSet;
    
    for (int i(0); i < items->count(); i++) {
        QOrganizerItem item = items->at(i);
        
        // Validate & save
        QOrganizerManager::Error saveError;
        if (validateItem(item, &saveError)) {
            TRAPD(err, saveItemL(&item, &changeSet));
            transformError(err, &saveError);
        }
        
        // Check error
        if (saveError != QOrganizerManager::NoError) {
            *error = saveError;
            if (errorMap)
                errorMap->insert(i, *error);
        } else {
            // Update the item with the data that is available after save
            items->replace(i, item);
        }
    }
    
    // Emit changes
    changeSet.emitSignals(this);
    
    return *error == QOrganizerManager::NoError;
}

bool QOrganizerItemSymbianEngine::saveItem(QOrganizerItem* item, 
    QOrganizerManager::Error* error)
{
    // Validate & save
    if (validateItem(*item, error)) {
        QOrganizerItemChangeSet changeSet;
        TRAPD(err, saveItemL(item, &changeSet));
        transformError(err, error);
        changeSet.emitSignals(this);
    }
    return *error == QOrganizerManager::NoError;
}

void QOrganizerItemSymbianEngine::saveItemL(QOrganizerItem *item, 
    QOrganizerItemChangeSet *changeSet)
{
    QOrganizerCollectionId completeCollectionId;
    QOrganizerCollectionId collectionId;
    if (item) {
        completeCollectionId = item->collectionId();
        collectionId = item->collectionId();
    }
    QOrganizerCollectionId collectionLocalId = collectionIdL(*item,
        collectionId);

    // Find the entry corresponding to the item or to the item occurrence.
    // Creates a new one, if the corresponding entry does not exist yet.
    CCalEntry *entry(0);
    bool isNewEntry(false);
    if(item->type()== QOrganizerItemType::TypeEventOccurrence
        || item->type()== QOrganizerItemType::TypeTodoOccurrence) {
        entry = entryForItemOccurrenceL(collectionLocalId, *item, isNewEntry);
    } else {
        entry = entryForItemL(collectionLocalId, *item, isNewEntry);
    }
    CleanupStack::PushL(entry);

    // Transform QOrganizerItem -> CCalEntry    
    m_itemTransform.toEntryL(*item, entry);

    // Save entry to the database
    RPointerArray<CCalEntry> entries;
    CleanupClosePushL(entries);
    entries.AppendL(entry);
    TInt count(0);
    entryViewL(collectionLocalId)->StoreL(entries, count);
    if (count != entries.Count()) {
        // The documentation states about count "On return, this
        // contains the number of entries which were successfully stored".
        // So it is not clear which error caused storing the entry to fail
        // -> let's use the "one-error-fits-all" error code KErrGeneral.
        User::Leave(KErrGeneral);
    }

    // Transform details that are available/updated after saving    
    m_itemTransform.toItemPostSaveL(*entry, item, managerUri());
    
    // Update id
    item->setId(toItemId(m_collections[collectionLocalId].calCollectionId(), entry->LocalUidL()));

    // Set collection id
    item->setCollectionId(collectionLocalId);

    // Cleanup
    CleanupStack::PopAndDestroy(&entries);
    CleanupStack::PopAndDestroy(entry);

    // Update change set for signal emissions
    if (changeSet) {
        if (isNewEntry)
            changeSet->insertAddedItem(item->id());
        else
            changeSet->insertChangedItem(item->id());
    }
}

/*!
 * Retrieves the entry view for the collection. Leaves with KErrArgument if
 * not found.
 */
CCalEntryView* QOrganizerItemSymbianEngine::entryViewL(
    const QOrganizerCollectionId& collectionId) const
{
    QOrganizerCollectionId tempCollectionId = collectionId;

    // Null is interpreted as the default collection
    if (tempCollectionId.isNull())
        tempCollectionId = m_defaultCollection.id();

    if (!m_collections.contains(tempCollectionId))
        User::Leave(KErrArgument);

    return m_collections[tempCollectionId].calEntryView();
}

/*!
 * Retrieves the instance view for the collection. Leaves with KErrArgument if
 * not found.
 */
CCalInstanceView* QOrganizerItemSymbianEngine::instanceViewL(const QOrganizerCollectionId& collectionId) const
{
    QOrganizerCollectionId tempCollectionId = collectionId;

    // Null is interpreted as the default collection
    if (tempCollectionId.isNull())
        tempCollectionId = m_defaultCollection.id();

    if (!m_collections.contains(tempCollectionId))
        User::Leave(KErrArgument);

    return m_collections[tempCollectionId].calInstanceView();
}
    
/*!
 * Returns item's collection id if it is valid. If not returns collectionId
 * given as a parameter if it is valid. Fallback is to return the default
 * session's collection id.
 */
QOrganizerCollectionId QOrganizerItemSymbianEngine::collectionIdL(
    const QOrganizerItem &item, const QOrganizerCollectionId &collectionId) const
{
#ifdef SYMBIAN_CALENDAR_V2
    QOrganizerCollectionId itemCollectionId = item.collectionId();

    if (!itemCollectionId.isNull() && !collectionId.isNull()
        && collectionId != itemCollectionId)
            User::Leave(KErrArgument);
    else if (!collectionId.isNull())
        return collectionId;
    else if (!itemCollectionId.isNull())
        return itemCollectionId;
#else
    Q_UNUSED(item);
    Q_UNUSED(collectionId);
#endif
    
    // Default collection id is the default session's collection id
    return m_defaultCollection.id();
}

CCalEntry* QOrganizerItemSymbianEngine::entryForItemOccurrenceL(
    const QOrganizerCollectionId &collectionId, const QOrganizerItem &item, 
    bool &isNewEntry) const
{
    CCalEntry * entry(NULL);

    // Check manager uri (if provided)
    if (!item.id().managerUri().isEmpty()) {
        if (item.id().managerUri() != managerUri())
            User::Leave(KErrInvalidOccurrence);
    }

    // Find the child entry corresponding to the item occurrence
    if (!item.id().isNull()) {
        // Fetch the item (will return NULL if the localid is not found)
        entry = entryViewL(collectionId)->FetchL(toTCalLocalUid(item.id()));
        if (!entry)
            User::Leave(KErrInvalidOccurrence);
        return entry;
    }

    // Entry not found, find the parent entry and create a new child for it
    HBufC8* parentGlobalUid(OrganizerItemGuidTransform::guidLC(item));
    CCalEntry *parentEntry(
        findParentEntryLC(collectionId, item, *parentGlobalUid));

    // Get the parameters for the new child entry
    QOrganizerItemParent origin(
        item.detail<QOrganizerItemParent>());
    if (!origin.originalDate().isValid()) {
        User::Leave(KErrInvalidOccurrence);
    }
    QDateTime parentStartTime = toQDateTimeL(parentEntry->StartTimeL());
    QDateTime recurrenceDateTime = QDateTime(origin.originalDate(), parentStartTime.time());
    TCalTime recurrenceId = toTCalTimeL(recurrenceDateTime);
    HBufC8* globalUid = HBufC8::NewLC(parentEntry->UidL().Length());
    globalUid->Des().Copy(parentEntry->UidL());

    // Create the new child entry
    entry = CCalEntry::NewL(parentEntry->EntryTypeL(),
                            globalUid,
                            parentEntry->MethodL(),
                            parentEntry->SequenceNumberL(),
                            recurrenceId,
                            CalCommon::EThisOnly);
    isNewEntry = true;
    CleanupStack::Pop(globalUid); // Ownership transferred
    CleanupStack::PopAndDestroy(parentEntry);
    CleanupStack::PopAndDestroy(parentGlobalUid);

    return entry; // Ownership transferred
}

CCalEntry* QOrganizerItemSymbianEngine::entryForItemL(
    const QOrganizerCollectionId &collectionId, 
    const QOrganizerItem &item, bool &isNewEntry) const
{
    // Try to find with local id
    CCalEntry *entry = findEntryL(collectionId, item.id(), item.id().managerUri());

    // Not found. Try to find with globalUid
    if (!entry) {
        HBufC8* globalUid = OrganizerItemGuidTransform::guidLC(item);

        entry = findEntryL(collectionId, *globalUid);
        // Not found? Create a new entry instance to be saved to the database
        if (!entry) {
            CCalEntry::TType type = OrganizerItemTypeTransform::entryTypeL(item);
            entry = CCalEntry::NewL(type, globalUid, CCalEntry::EMethodAdd, 0);
            isNewEntry = true;
            CleanupStack::Pop(globalUid); // Ownership transferred to the new entry
            return entry;
        }
        CleanupStack::PopAndDestroy(globalUid);
    }
    return entry;
}

CCalEntry * QOrganizerItemSymbianEngine::findEntryL(
    const QOrganizerCollectionId &collectionId, 
    const QOrganizerItemId &id, QString manageruri) const
{
    CCalEntry *entry(0);

    // Check that manager uri match to this manager (if provided)
    if (!manageruri.isEmpty()) {
        if (manageruri != managerUri())
            User::Leave(KErrArgument);
    }

    // There must be an existing entry if id is provided
    if (!id.isNull()) {
        // Fetch the item (will return NULL if the id is not found)
        entry = entryViewL(collectionId)->FetchL(toTCalLocalUid(id));
        if (!entry)
            User::Leave(KErrNotFound);
    }

    // ownership transferred
    return entry;
}

CCalEntry * QOrganizerItemSymbianEngine::findEntryL(
    const QOrganizerCollectionId &collectionId, 
    const TDesC8& globalUid) const
{
    CCalEntry *entry(0);

    if (globalUid.Length()) {
        // Search for an existing entry based on guid
        RPointerArray<CCalEntry> calEntryArray;
        CleanupResetAndDestroyPushL(calEntryArray);
        entryViewL(collectionId)->FetchL(globalUid, calEntryArray);
        if (calEntryArray.Count()) {
            // take the first item in the array
            entry = calEntryArray[0];
            calEntryArray.Remove(0);
        }
        CleanupStack::PopAndDestroy(&calEntryArray);
    }

    // ownership transferred
    return entry;
}

CCalEntry* QOrganizerItemSymbianEngine::findParentEntryLC(
    const QOrganizerCollectionId &collectionId, 
    const QOrganizerItem &item, const TDesC8& globalUid) const
{
    CCalEntry *parent(0);

    // Try to find with parent's id
    QOrganizerItemParent origin = item.detail<QOrganizerItemParent>();
    if (!origin.parentId().isNull()) {
        // Fetch the item (will return NULL if the id is not found)
        parent = entryViewL(collectionId)->FetchL(toTCalLocalUid(origin.parentId())); // ownership transferred
        if (!parent)
            User::Leave(KErrInvalidOccurrence);
        CleanupStack::PushL(parent);
    // Try to find with globalUid
    } else if (globalUid.Length()) {
        parent = findEntryL(collectionId, globalUid);
        if (!parent)
            User::Leave(KErrInvalidOccurrence);
        CleanupStack::PushL(parent);

    } else {
        User::Leave(KErrInvalidOccurrence);
    }

    // Verify the item against parent
    if(parent->EntryTypeL() != OrganizerItemTypeTransform::entryTypeL(item))
        User::Leave(KErrInvalidOccurrence);

    // Check for UID consistency for item with parentEntry
    if (!item.guid().isEmpty()
        && globalUid.Compare(parent->UidL())) {
        // Guid is not consistent with parentEntry UID
        User::Leave(KErrInvalidOccurrence);
    } else if (!origin.parentId().isNull()
        && (toTCalLocalUid(origin.parentId()) != parent->LocalUidL())) {
        // parentId is not consistent with parentEntry localUID
        User::Leave(KErrInvalidOccurrence);
    }

    return parent;
}

bool QOrganizerItemSymbianEngine::removeItems(
    const QList<QOrganizerItemId>& itemIds, 
    QMap<int, QOrganizerManager::Error>* errorMap, 
    QOrganizerManager::Error* error)
{
    // Note: the performance would be probably better, if we had a separate
    // implementation for the case with a list of item ids that would
    // remove all the items

    QOrganizerItemChangeSet changeSet;

    for (int i(0); i < itemIds.count(); i++) {
        // Remove
        QOrganizerManager::Error removeError(
            QOrganizerManager::NoError);
        TRAPD(err, removeItemL(itemIds.at(i)));
        if (err != KErrNone) {
            transformError(err, &removeError);
            *error = removeError;
            if (errorMap)
                errorMap->insert(i, *error);
        } else {
            // Signals
            changeSet.insertRemovedItem(itemIds.at(i));
        }
    }

    // Emit changes
    changeSet.emitSignals(this);

    return *error == QOrganizerManager::NoError;
}

bool QOrganizerItemSymbianEngine::removeItem(
    const QOrganizerItemId& organizeritemId, 
    QOrganizerManager::Error* error)
{
    TRAPD(err, removeItemL(organizeritemId));
    if (err != KErrNone) {
        transformError(err, error);
    } else {
        // Signals
        QOrganizerItemChangeSet changeSet;
        changeSet.insertRemovedItem(organizeritemId);
        changeSet.emitSignals(this);
    }
    return *error == QOrganizerManager::NoError;
}

void QOrganizerItemSymbianEngine::removeItemL(
    const QOrganizerItemId& organizeritemId)
{
    // TODO: How to remove item instances?

    if (organizeritemId.managerUri() != managerUri()) // XXX TODO: cache managerUri for fast lookup.
        User::Leave(KErrNotFound);

    QOrganizerCollectionId collectionId = getCollectionId(organizeritemId);
    if (!m_collections.contains(collectionId))
        User::Leave(KErrNotFound);

    // Find entry
    TCalLocalUid calLocalId = toTCalLocalUid(organizeritemId);
    CCalEntry *calEntry = entryViewL(collectionId)->FetchL(calLocalId);
    CleanupStack::PushL(calEntry);
    if (!calEntry)
        User::Leave(KErrNotFound);

    // Remove entry
    entryViewL(collectionId)->DeleteL(*calEntry);
    CleanupStack::PopAndDestroy(calEntry);
}

QList<QOrganizerItem> QOrganizerItemSymbianEngine::slowFilterIdsToItemsL(
    const QList<QOrganizerItemId> &itemIds,
    const QOrganizerItemFilter& filter, 
    const QList<QOrganizerItemSortOrder>& sortOrders) const
{
    // Nothing to filter?
    if (filter.type() == QOrganizerItemFilter::InvalidFilter)
        return QList<QOrganizerItem>();
    
    // Get item. Filter and sort it.
    QList<QOrganizerItem> filteredAndSorted;
    foreach (const QOrganizerItemId &id, itemIds)
        addFilteredAndSorted(&filteredAndSorted, itemL(id, QOrganizerItemFetchHint()), filter, sortOrders);

    return filteredAndSorted;
}

QList<QOrganizerItemId> QOrganizerItemSymbianEngine::slowFilterIdsL(
    const QList<QOrganizerItemId> &itemIds,
    const QOrganizerItemFilter& filter, 
    const QList<QOrganizerItemSortOrder>& sortOrders) const
{
    // Nothing to filter?
    if (filter.type() == QOrganizerItemFilter::InvalidFilter)
        return QList<QOrganizerItemId>();
    
    // No filtering and sorting needed?
    if (filter.type() == QOrganizerItemFilter::DefaultFilter && sortOrders.count() == 0)
        return itemIds;    
    
    // Get item. Filter and sort it.
    QList<QOrganizerItem> filteredAndSorted;
    foreach (const QOrganizerItemId &id, itemIds)
        addFilteredAndSorted(&filteredAndSorted, itemL(id, QOrganizerItemFetchHint()), filter, sortOrders);

    // Convert items to item id's
    QList<QOrganizerItemId> ids;
    foreach (const QOrganizerItem& item, filteredAndSorted)
        ids << item.id();
        
    return ids;
}

QList<QOrganizerItem> QOrganizerItemSymbianEngine::slowFilterItems(
    const QList<QOrganizerItem> &items,
    const QOrganizerItemFilter& filter, 
    const QList<QOrganizerItemSortOrder>& sortOrders) const
{
    // Nothing to filter?
    if (filter.type() == QOrganizerItemFilter::InvalidFilter)
        return QList<QOrganizerItem>();
    
    // No filtering and sorting needed?
    if (filter.type() == QOrganizerItemFilter::DefaultFilter && sortOrders.count() == 0)
        return items; 
    
    // Filter and sort
    QList<QOrganizerItem> filteredAndSorted;
    foreach(const QOrganizerItem& item, items)
        addFilteredAndSorted(&filteredAndSorted, item, filter, sortOrders);
    return filteredAndSorted;
}

void QOrganizerItemSymbianEngine::addFilteredAndSorted(
    QList<QOrganizerItem> *items,
    const QOrganizerItem &item,
    const QOrganizerItemFilter& filter,
    const QList<QOrganizerItemSortOrder>& sortOrders) const
{
    if (filter.type() == QOrganizerItemFilter::DefaultFilter) {
        QOrganizerManagerEngine::addSorted(items, item, sortOrders); // only sorting needed
    } else {
        if (QOrganizerManagerEngine::testFilter(filter, item))
            QOrganizerManagerEngine::addSorted(items, item, sortOrders);
    }
}

QList<OrganizerSymbianCollection> QOrganizerItemSymbianEngine::filteredSymbianCollectionsL(
    const QOrganizerItemFilter &filter) const
{
    // No filter for collections?
    if (filter.type() == QOrganizerItemFilter::DefaultFilter)
        return m_collections.values();
    
    // Nothing to filter?
    if (filter.type() == QOrganizerItemFilter::InvalidFilter)
        return QList<OrganizerSymbianCollection>();
    
    // Get filters
    QList<QOrganizerItemFilter> filters;
    if (filter.type() == QOrganizerItemFilter::UnionFilter) 
        filters = static_cast<QOrganizerItemUnionFilter>(filter).filters();
    if (filter.type() == QOrganizerItemFilter::CollectionFilter)
        filters.append(filter);
    
    // TODO: QOrganizerItemFilter::IntersectionFilter
    // Implementing this is not really worth the effort. It's an unlikely use case.
    // Simpler just to return all collections and let slowfilter handle this.
    
    // Get collection ids from collection filters
    QList<QOrganizerCollectionId> cIds;
    foreach (const QOrganizerItemFilter &f, filters) {
        if (f.type() == QOrganizerItemFilter::CollectionFilter)
            cIds << static_cast<QOrganizerItemCollectionFilter>(f).collectionIds().toList();
    }
    
    // No filter found?
    if (cIds.count() == 0)
        return m_collections.values();
    
    // Find matching collections
    QList<OrganizerSymbianCollection> collections;
    foreach (const QOrganizerCollectionId &id, cIds) {
        if (m_collections.contains(id))
            collections << m_collections[id];
        else
            User::Leave(KErrArgument); // TODO: Not really sure if we should leave in this case...
    }    
    return collections;
}

QOrganizerCollection QOrganizerItemSymbianEngine::defaultCollection(
    QOrganizerManager::Error* error) const
{
    *error = QOrganizerManager::NoError;
    return m_defaultCollection.toQOrganizerCollectionL();
}

QOrganizerCollection QOrganizerItemSymbianEngine::collection(
    const QOrganizerCollectionId& collectionId,
    QOrganizerManager::Error* error) const
{
    if (m_collections.contains(collectionId))
        return (m_collections[collectionId].toQOrganizerCollectionL());
    *error = QOrganizerManager::DoesNotExistError;
    return QOrganizerCollection();
}

QList<QOrganizerCollection> QOrganizerItemSymbianEngine::collections(
    QOrganizerManager::Error* error) const
{
    // Get collections
    QList<QOrganizerCollection> collections;
    TRAPD(err, collections = collectionsL());
    transformError(err, error);
    return collections;
}

QList<QOrganizerCollection> QOrganizerItemSymbianEngine::collectionsL() const
{
    QList<QOrganizerCollection> collections;
    QList<QOrganizerCollectionId> collectionIds = m_collections.keys();
    foreach (const QOrganizerCollectionId &id, collectionIds) {
        collections << m_collections[id].toQOrganizerCollectionL();
    }
    return collections;
}

bool QOrganizerItemSymbianEngine::saveCollection(
    QOrganizerCollection* collection, 
    QOrganizerManager::Error* error)
{
    bool isNewCollection = true;
    if (!collection->id().isNull())
        isNewCollection = false;
    
    TRAPD(err, saveCollectionL(collection));
    transformError(err, error);

    if (*error == QOrganizerManager::NoError) {
        if (isNewCollection) {
            // Emit changes
            QOrganizerCollectionChangeSet changeSet;
            changeSet.insertAddedCollection(collection->id());
            changeSet.emitSignals(this);
        }
        // NOTE: collectionsChanged signal will be emitted from 
        // CalendarInfoChangeNotificationL
    }

    return (*error == QOrganizerManager::NoError);   
}

void QOrganizerItemSymbianEngine::saveCollectionL(
    QOrganizerCollection* collection)
{
#ifndef SYMBIAN_CALENDAR_V2
    Q_UNUSED(collection);
    User::Leave(KErrNotSupported);
#else
    // Check manager uri if defined
    if (!collection->id().managerUri().isEmpty()) {
        if (collection->id().managerUri() != this->managerUri())
            User::Leave(KErrArgument); // uri does not match this manager
    }
    
    // Find existing collection
    QOrganizerCollectionId id = collection->id();
    OrganizerSymbianCollection symbianCollection(this);
    if (!id.isNull()) {
        if (m_collections.contains(id))
            symbianCollection = m_collections[id];
        else
            User::Leave(KErrArgument); // collection id was defined but was not found 
    }

    // Convert into a compatible collection
    QOrganizerManager::Error error(QOrganizerManager::NoError);
    *collection = compatibleCollection(*collection, &error);
    if (error != QOrganizerManager::NoError)
        User::Leave(KErrArgument); // Could not convert -> collection not valid

    // Convert metadata to cal info
    CCalCalendarInfo *calInfo = toCalInfoLC(collection->metaData());
    
    // Update modification time
    TTime currentTime;
    currentTime.UniversalTime();
    setCalInfoPropertyL(calInfo, EModificationTime, currentTime);
    
    // Get filename from collection to be saved
    QString fileName = collection->metaData(OrganizerSymbianCollection::KeyFileName).toString();
    
    // Did we found an existing collection?
    if (!symbianCollection.isValid()) {

        // Set creation time
        setCalInfoPropertyL(calInfo, ECreationTime, currentTime);
        
        // If filename is not provided use collection name as a filename
        if (fileName.isEmpty())
            fileName = collection->metaData(QOrganizerCollection::KeyName).toString();
                
        // Create a new collection
        symbianCollection.openL(toPtrC16(fileName), calInfo);
        m_collections.insert(symbianCollection.id(), symbianCollection);
        symbianCollection.createViewsL();
    }
    else {
        // Cannot allow changing the filename for an existing collection
        if (symbianCollection.fileName() != fileName)
            User::Leave(KErrArgument);

        // Preserve creation time by copying it from the old cal info
        TTime creationTime = Time::NullTTime();
        CCalCalendarInfo *calInfoOld = symbianCollection.calSession()->CalendarInfoL();
        TRAP_IGNORE(creationTime = getCalInfoPropertyL<TTime>(*calInfoOld, ECreationTime));
        delete calInfoOld;
        setCalInfoPropertyL(calInfo, ECreationTime, creationTime);
        
        // Update the existing collection
        symbianCollection.calSession()->SetCalendarInfoL(*calInfo);
    }
    CleanupStack::PopAndDestroy(calInfo);

    // Update collection information for client
    *collection = symbianCollection.toQOrganizerCollectionL();
#endif //SYMBIAN_CALENDAR_V2
}

bool QOrganizerItemSymbianEngine::removeCollection(
    const QOrganizerCollectionId& collectionId, 
    QOrganizerManager::Error* error)
{
    TRAPD(err, removeCollectionL(collectionId));
    transformError(err, error);
    if (*error == QOrganizerManager::NoError) {
        QOrganizerCollectionChangeSet collectionChangeSet;
        collectionChangeSet.insertRemovedCollection(collectionId);
        collectionChangeSet.emitSignals(this);
    }
    return (*error == QOrganizerManager::NoError);
}

void QOrganizerItemSymbianEngine::removeCollectionL(
    const QOrganizerCollectionId& collectionId)
{
#ifndef SYMBIAN_CALENDAR_V2
    Q_UNUSED(collectionId);
    User::Leave(KErrNotSupported);
#else
    // Dont allow removing the default collection
    if (collectionId == m_defaultCollection.id())
        User::Leave(KErrAccessDenied);
    
    // Find collection
    foreach(const OrganizerSymbianCollection &collection, m_collections) {
        if (collection.id() == collectionId) {
            
            // Get cal info
            CCalCalendarInfo *calInfo = collection.calSession()->CalendarInfoL();
            CleanupStack::PushL(calInfo);
                        
            // Remove the calendar file itself
            TRAPD(err, 
                collection.calSession()->DeleteCalFileL(calInfo->FileNameL()));
            if( err == KErrInUse ) {
                
                // We cannot remove the calendar if we are not the only one
                // who has it open. So instead just disable it and mark it for
                // deletion. The native symbian calendar will remove it
                // during the next startup.
                // TODO: should we try to delete those during startup also?
                calInfo->SetEnabled( EFalse );
                setCalInfoPropertyL(calInfo, ESyncStatus, EFalse);
                setCalInfoPropertyL(calInfo, EMarkAsDelete, ETrue);
                
                // Update modification time
                TTime modificationTime;
                modificationTime.HomeTime();
                setCalInfoPropertyL(calInfo, EModificationTime, modificationTime);
                
                // TODO: Should we remove all entries also? 
                // Client might reopen the calendar before its really deleted.
                            
                // Update calendar info
                collection.calSession()->SetCalendarInfoL( *calInfo );
                }
            else {
                User::LeaveIfError(err);
            }
            CleanupStack::PopAndDestroy(calInfo);

            // Delete the collection
            m_collections.remove(collection.id());
            return;
        }
    }
    User::Leave(KErrNotFound);
#endif // SYMBIAN_CALENDAR_V2
}

QOrganizerItem QOrganizerItemSymbianEngine::compatibleItem(
    const QOrganizerItem& original,
    QOrganizerManager::Error* error) const
{
    *error = QOrganizerManager::NoError;

    if (original.type() == QOrganizerItemType::TypeEvent) {
        QOrganizerEvent event = original;
        if (!event.startDateTime().isValid()) {
            // Event type requires start time in symbian calendar API
            event.setStartDateTime(QDateTime::currentDateTime());
        }
        return event;
    }

    return original;
}

QOrganizerCollection QOrganizerItemSymbianEngine::compatibleCollection(
    const QOrganizerCollection& original, QOrganizerManager::Error* error) const
{
    *error = QOrganizerManager::NoError;

    QOrganizerCollection compatibleCollection = original;

    // Check that the collection has either name or file name. Note: If the
    // file name is missing but name is available, file name will be generated
    // from name.
    if (!original.metaData().contains(OrganizerSymbianCollection::KeyFileName)) {
        if (original.metaData().contains(QOrganizerCollection::KeyName)) {
            // Use collection name as file name
            compatibleCollection.setMetaData(
                OrganizerSymbianCollection::KeyFileName,
                original.metaData(QOrganizerCollection::KeyName));
        } else {
            // Neither file name nor name available -> synthesize a file name
            QString unnamed("Unnamed");
            const int KMaxSynthesizedCount(99);
            for (int i(0); i < KMaxSynthesizedCount; i++) {
                QString synthesizedName = unnamed + QString::number(i);
                if (isCollectionNameAvailable(synthesizedName)) {
                    compatibleCollection.setMetaData(OrganizerSymbianCollection::KeyFileName, synthesizedName);
                    break;
                }
            }
        }
    }

    return compatibleCollection;
}

bool QOrganizerItemSymbianEngine::isCollectionNameAvailable(QString name) const
{
    foreach (OrganizerSymbianCollection collection, m_collections.values()) {
        if (collection.fileName() == name) {
            return false;
        }
    }
    return true;
}

QMap<QString, QOrganizerItemDetailDefinition> 
QOrganizerItemSymbianEngine::detailDefinitions(
    const QString& itemType, QOrganizerManager::Error* error) const
{
    if (m_definition.isEmpty()) {
        // Get all the detail definitions from the base implementation
        m_definition = QOrganizerManagerEngine::schemaDefinitions();
        
        // Modify the base schema to match backend support
        m_itemTransform.modifyBaseSchemaDefinitions(m_definition);
    }
    
    // Check if we support the item type
    if (!m_definition.contains(itemType)) {
        *error = QOrganizerManager::NotSupportedError;
        return QMap<QString, QOrganizerItemDetailDefinition>();
    }
    
    *error = QOrganizerManager::NoError;
    return m_definition.value(itemType);
}

bool QOrganizerItemSymbianEngine::startRequest(
    QOrganizerAbstractRequest* req)
{
    /*
        This is the entry point to the async API.  The request object describes 
        the type of request (switch on req->type()).  Req will not be null when 
        called by the framework.

        Generally, you can queue the request and process them at some later time
        (probably in another thread).

        Once you start a request, call the updateRequestState and/or the
        specific updateXXXXXRequest functions to mark it in the active state.

        If your engine is particularly fast, or the operation involves only in
        memory data, you can process and complete the request here.  That is
        probably not the case, though.

        Note that when the client is threaded, and the request might live on a
        different thread, you might need to be careful with locking.  
        In particular, the request might be deleted while you are still working 
        on it.  In this case, your requestDestroyed function will be called 
        while the request is still valid, and you should block in that function 
        until your worker thread (etc) has been notified not to touch that 
        request any more. 
        
        We plan to provide some boiler plate code that will allow you to:

        1) implement the sync functions, and have the async versions call the 
        sync in another thread

        2) or implement the async versions of the function, and have the sync 
        versions call the async versions.

        It's not ready yet, though.

        Return true if the request can be started, false otherwise.  You can set
        an error in the request if you like.
     */
    return m_requestServiceProviderQueue->startRequest(req);
}

bool QOrganizerItemSymbianEngine::cancelRequest(
    QOrganizerAbstractRequest* req)
{
    /*
        Cancel an in progress async request.  If not possible, return false 
        from here.
    */
    return m_requestServiceProviderQueue->cancelRequest(req);
}

bool QOrganizerItemSymbianEngine::waitForRequestFinished(
    QOrganizerAbstractRequest* req, int msecs)
{
    /*
        Wait for a request to complete (up to a max of msecs milliseconds).

        Return true if the request is finished (including if it was already).  
        False otherwise.

        You should really implement this function, if nothing else than as a 
        delay, since clients may call this in a loop.

        It's best to avoid processing events, if you can, or at least only 
        process non-UI events.
    */
    return m_requestServiceProviderQueue->waitForRequestFinished(req, msecs);
}

void QOrganizerItemSymbianEngine::requestDestroyed(
    QOrganizerAbstractRequest* req)
{
    /*
        This is called when a request is being deleted.  It lets you know:

        1) the client doesn't care about the request any more.  You can still 
        complete it if you feel like it.
        2) you can't reliably access any properties of the request pointer any 
        more. The pointer will be invalid once this function returns.

        This means that if you have a worker thread, you need to let that 
        thread know that the request object is not valid and block until that 
        thread acknowledges it.  One way to do this is to have a QSet<QOIAR*> 
        (or QMap<QOIAR, MyCustomRequestState>) that tracks active requests, and
        insert into that set in startRequest, and remove in requestDestroyed 
        (or when it finishes or is cancelled).  Protect that set/map with a 
        mutex, and make sure you take the mutex in the worker thread before 
        calling any of the QOIAR::updateXXXXXXRequest functions.  And be 
        careful of lock ordering problems :D

    */
        m_requestServiceProviderQueue->requestDestroyed(req);
}

bool QOrganizerItemSymbianEngine::hasFeature(
    QOrganizerManager::ManagerFeature feature, 
    const QString& itemType) const
{
    Q_UNUSED(itemType);
    switch(feature) {
        case QOrganizerManager::MutableDefinitions:
            // We don't support save/remove detail definition
            return false;
        case QOrganizerManager::Anonymous:
            // The engines share the same data
            return false;
        case QOrganizerManager::ChangeLogs:
            // Change logs not supported
            return false;
    }
    return false;
}

bool QOrganizerItemSymbianEngine::isFilterSupported(
    const QOrganizerItemFilter& filter) const
{
    Q_UNUSED(filter);
    // TODO: filtering based on timestamps could be an exception to the rule,
    // i.e. timestamp detail filters should then return true.
    return false;
}

QList<int> QOrganizerItemSymbianEngine::supportedDataTypes() const
{
    QList<int> ret;
    ret << QVariant::String;
    ret << QVariant::Date;
    ret << QVariant::DateTime;
    ret << QVariant::Time;

    return ret;
}

QStringList QOrganizerItemSymbianEngine::supportedItemTypes() const
{
    // Lazy initialization
    if (m_definition.isEmpty()) {
        m_definition = QOrganizerManagerEngine::schemaDefinitions();
        m_itemTransform.modifyBaseSchemaDefinitions(m_definition);
    }
    
    return m_definition.keys();
}

#ifdef SYMBIAN_CALENDAR_V2
void QOrganizerItemSymbianEngine::CalendarInfoChangeNotificationL(
    RPointerArray<CCalFileChangeInfo>& aCalendarInfoChangeEntries)
{
    // TODO: QOrganizerCollectionChangeSet?
    QSet<QOrganizerCollectionId> collectionsAdded;
    QSet<QOrganizerCollectionId> collectionsChanged;
    QSet<QOrganizerCollectionId> collectionsRemoved;
    
    // Loop through changes
    int changeCount = aCalendarInfoChangeEntries.Count();
    for (int i=0; i<changeCount; i++) {

        // Get changed calendar file name
        const TDesC& fileName = aCalendarInfoChangeEntries[i]->FileNameL();
        
        // Try to find matching collection
        OrganizerSymbianCollection collection(this);
        foreach (const OrganizerSymbianCollection &c, m_collections) {
            if (c.fileName() == toQString(fileName))
                collection = c; 
        }
        
        // Check change type
        switch (aCalendarInfoChangeEntries[i]->ChangeType())
        {
        case ECalendarFileCreated:
            if (!collection.isValid()) {
                // A calendar file has been created but not by this manager 
                // instance.
                collection.openL(fileName);
                m_collections.insert(collection.id(), collection);
                collection.createViewsL();
                collectionsAdded << collection.id();
            }
            break;
            
        case ECalendarFileDeleted:
            if (collection.isValid()) {
                // A calendar file has been removed but not by this manager 
                // instance.
                QOrganizerCollectionId id = collection.id();
                m_collections.remove(id);
                collectionsRemoved << id;
            }
            break;
            
        case ECalendarInfoCreated:
            break;
            
        case ECalendarInfoUpdated:
            if (collection.isValid()) {
                if (collection.isMarkedForDeletionL()) {
                    // A calendar file has been marked for deletion but not by 
                    // this manager instance
                    QOrganizerCollectionId id = collection.id();
                    m_collections.remove(id);
                    collectionsRemoved << id;
                    // TODO: Try removing the calendar file?                    
                } else {
                    collectionsChanged << collection.id();
                }
            } else {
                // Calendar file has been modified but we do not have a session 
                // to it.
                collection.openL(fileName);
                
                // Is it marked for deletion?
                if (collection.isMarkedForDeletionL()) {
                    // Something has modified a calendar which is marked for 
                    // deletion.
                } else {
                    // A calendar file which was marked for deletion has been 
                    // taken into use again.
                    m_collections.insert(collection.id(), collection);
                    collection.createViewsL();
                    collectionsAdded << collection.id();
                }
            }
            break;
            
        case ECalendarInfoDeleted:
            break;
            
        default:
            break;
        }
    }
    
    // Emit signals
    if (collectionsAdded.count())
        emit this->collectionsAdded(collectionsAdded.toList());
    if (collectionsChanged.count())
        emit this->collectionsChanged(collectionsChanged.toList());
    if (collectionsRemoved.count())
        emit this->collectionsRemoved(collectionsRemoved.toList());
}
#endif

/*! Transform a Symbian error id to QOrganizerManager::Error.
 *
 * \param symbianError Symbian error.
 * \param QtError Qt error.
 * \return true if there was no error
 *         false if there was an error
*/
bool QOrganizerItemSymbianEngine::transformError(TInt symbianError, QOrganizerManager::Error* qtError)
{
    switch(symbianError)
    {
        case KErrNone:
        {
            *qtError = QOrganizerManager::NoError;
            break;
        }
        case KErrNotFound:
        {
            *qtError = QOrganizerManager::DoesNotExistError;
            break;
        }
        case KErrAlreadyExists:
        {
            *qtError = QOrganizerManager::AlreadyExistsError;
            break;
        }
        case KErrLocked:
        {
            *qtError = QOrganizerManager::LockedError;
            break;
        }
        case KErrAccessDenied:
        case KErrPermissionDenied:
        {
            *qtError = QOrganizerManager::PermissionsError;
            break;
        }
        case KErrNoMemory:
        {
            *qtError = QOrganizerManager::OutOfMemoryError;
            break;
        }
        case KErrNotSupported:
        {
            *qtError = QOrganizerManager::NotSupportedError;
            break;
        }
        case KErrArgument:
        {
            *qtError = QOrganizerManager::BadArgumentError;
            break;
        }
        // KErrInvalidOccurrence is a special error code defined for Qt
        // Organizer API implementation purpose only
        case KErrInvalidOccurrence:
        {
            *qtError = QOrganizerManager::InvalidOccurrenceError;
            break;
        }
        case KErrInvalidItemType:
        {
            *qtError = QOrganizerManager::InvalidItemTypeError;
            break;
        }
        default:
        {
            *qtError = QOrganizerManager::UnspecifiedError;
            break;
        }
    }
    return *qtError == QOrganizerManager::NoError;
}

