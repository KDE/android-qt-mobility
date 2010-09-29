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

#include "qorganizeritemmanager.h"

#include "qorganizeritemdetaildefinition.h"
#include "qorganizeritemmanagerengine.h"
#include "qorganizeritemabstractrequest.h"
#include "qorganizeritemrequests.h"
#include "qorganizeritemchangeset.h"
#include "qorganizeritemdetails.h"
#include "qorganizerevent.h"
#include "qorganizereventoccurrence.h"
#include "qorganizertodo.h"
#include "qorganizertodooccurrence.h"

#include "qorganizeritemmemorybackend_p.h"

#include <QTimer>
#include <QUuid>
#include <QSharedData>
#include <QDebug>

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerItemMemoryEngine
  \brief The QOrganizerItemMemoryEngine class provides an in-memory implementation
  of an organizer item backend.
  \inmodule QtOrganizer
 
  It may be used as a reference implementation, or when persistent storage is not required.
 
  During construction, it will load the in-memory data associated with the memory store
  identified by the "id" parameter from the given parameters if it exists, or a new,
  anonymous store if it does not.
 
  Data stored in this engine is only available in the current process.
 
  This engine supports sharing, so an internal reference count is increased
  whenever a manager uses this backend, and is decreased when the manager
  no longer requires this engine.
 */

/* static data for manager class */
QMap<QString, QOrganizerItemMemoryEngineData*> QOrganizerItemMemoryEngine::engineDatas;

/*!
  \class QOrganizerItemMemoryEngineLocalId
  \brief The QOrganizerItemMemoryEngineLocalId class provides an id which uniquely identifies
  a QOrganizerItem stored within a collection stored within a a QOrganizerItemMemoryEngine.

  It may be used as a reference implementation, although since different platforms
  have different semantics for ids (datastore-unique versus calendar-unique, etc),
  the precise implementation required may differ.
 */
QOrganizerItemMemoryEngineLocalId::QOrganizerItemMemoryEngineLocalId()
    : QOrganizerItemEngineLocalId(), m_localCollectionId(0), m_localItemId(0)
{
}

QOrganizerItemMemoryEngineLocalId::QOrganizerItemMemoryEngineLocalId(quint32 collectionId, quint32 itemId)
    : QOrganizerItemEngineLocalId(), m_localCollectionId(collectionId), m_localItemId(itemId)
{
}

QOrganizerItemMemoryEngineLocalId::~QOrganizerItemMemoryEngineLocalId()
{
}

QOrganizerItemMemoryEngineLocalId::QOrganizerItemMemoryEngineLocalId(const QOrganizerItemMemoryEngineLocalId& other)
    : QOrganizerItemEngineLocalId(), m_localCollectionId(other.m_localCollectionId), m_localItemId(other.m_localItemId)
{
}

bool QOrganizerItemMemoryEngineLocalId::isEqualTo(const QOrganizerItemEngineLocalId* other) const
{
    quint32 otherlocalCollectionId = static_cast<const QOrganizerItemMemoryEngineLocalId*>(other)->m_localCollectionId;
    quint32 otherlocalItemId = static_cast<const QOrganizerItemMemoryEngineLocalId*>(other)->m_localItemId;
    if (m_localCollectionId != otherlocalCollectionId)
        return false;
    if (m_localItemId != otherlocalItemId)
        return false;
    return true;
}

bool QOrganizerItemMemoryEngineLocalId::isLessThan(const QOrganizerItemEngineLocalId* other) const
{
    // order by collection, then by item in collection.
    quint32 otherlocalCollectionId = static_cast<const QOrganizerItemMemoryEngineLocalId*>(other)->m_localCollectionId;
    quint32 otherlocalItemId = static_cast<const QOrganizerItemMemoryEngineLocalId*>(other)->m_localItemId;
    if (m_localCollectionId < otherlocalCollectionId)
        return true;
    if (m_localCollectionId == otherlocalCollectionId)
        return (m_localItemId < otherlocalItemId);
    return false;
}

uint QOrganizerItemMemoryEngineLocalId::engineLocalIdType() const
{
    // engines should embed the result of this as const read-only data (uint),
    // instead of calculating it every time the function is called...
    return qHash(QString(QLatin1String("memory")));
}

QOrganizerItemEngineLocalId* QOrganizerItemMemoryEngineLocalId::clone() const
{
    QOrganizerItemMemoryEngineLocalId *myClone = new QOrganizerItemMemoryEngineLocalId;
    myClone->m_localCollectionId = m_localCollectionId;
    myClone->m_localItemId = m_localItemId;
    return myClone;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug QOrganizerItemMemoryEngineLocalId::debugStreamOut(QDebug dbg)
{
    dbg.nospace() << "QOrganizerItemMemoryEngineLocalId(" << m_localCollectionId << ", " << m_localItemId << ")";
    return dbg.maybeSpace();
}
#endif

#ifndef QT_NO_DATASTREAM
QDataStream& QOrganizerItemMemoryEngineLocalId::dataStreamOut(QDataStream& out)
{
    return (out << m_localItemId << m_localCollectionId);
}

QDataStream& QOrganizerItemMemoryEngineLocalId::dataStreamIn(QDataStream& in)
{
    in >> m_localItemId >> m_localCollectionId;
    return in;
}
#endif

uint QOrganizerItemMemoryEngineLocalId::hash() const
{
    // Note: doesn't need to be unique, since == ensures difference.
    // hash function merely determines distribution in a hash table.
    quint64 combinedLocalId = m_localItemId;
    combinedLocalId <<= 32;
    combinedLocalId += m_localCollectionId;
    return uint(((combinedLocalId >> (8 * sizeof(uint) - 1)) ^ combinedLocalId) & (~0U));
}

/*!
  \class QOrganizerCollectionMemoryEngineLocalId
  \brief The QOrganizerCollectionMemoryEngineLocalId class provides an id which uniquely identifies
  a QOrganizerCollection stored within a collection stored within a a QOrganizerCollectionMemoryEngine.

  It may be used as a reference implementation, although since different platforms
  have different semantics for ids (datastore-unique versus calendar-unique, etc),
  the precise implementation required may differ.
 */
QOrganizerCollectionMemoryEngineLocalId::QOrganizerCollectionMemoryEngineLocalId()
    : QOrganizerCollectionEngineLocalId(), m_localCollectionId(0)
{
}

QOrganizerCollectionMemoryEngineLocalId::QOrganizerCollectionMemoryEngineLocalId(quint32 collectionId)
    : QOrganizerCollectionEngineLocalId(), m_localCollectionId(collectionId)
{
}

QOrganizerCollectionMemoryEngineLocalId::QOrganizerCollectionMemoryEngineLocalId(const QOrganizerCollectionMemoryEngineLocalId& other)
    : QOrganizerCollectionEngineLocalId(), m_localCollectionId(other.m_localCollectionId)
{
}

QOrganizerCollectionMemoryEngineLocalId::~QOrganizerCollectionMemoryEngineLocalId()
{
}

bool QOrganizerCollectionMemoryEngineLocalId::isEqualTo(const QOrganizerCollectionEngineLocalId* other) const
{
    quint32 otherlocalCollectionId = static_cast<const QOrganizerCollectionMemoryEngineLocalId*>(other)->m_localCollectionId;
    if (m_localCollectionId != otherlocalCollectionId)
        return false;
    return true;
}

bool QOrganizerCollectionMemoryEngineLocalId::isLessThan(const QOrganizerCollectionEngineLocalId* other) const
{
    // order by collection, then by item in collection.
    quint32 otherlocalCollectionId = static_cast<const QOrganizerCollectionMemoryEngineLocalId*>(other)->m_localCollectionId;
    if (m_localCollectionId < otherlocalCollectionId)
        return true;
    return false;
}

uint QOrganizerCollectionMemoryEngineLocalId::engineLocalIdType() const
{
    // engines should embed the result of this as const read-only data (uint),
    // instead of calculating it every time the function is called...
    return qHash(QString(QLatin1String("memory")));
}

QOrganizerCollectionEngineLocalId* QOrganizerCollectionMemoryEngineLocalId::clone() const
{
    QOrganizerCollectionMemoryEngineLocalId *myClone = new QOrganizerCollectionMemoryEngineLocalId;
    myClone->m_localCollectionId = m_localCollectionId;
    return myClone;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug QOrganizerCollectionMemoryEngineLocalId::debugStreamOut(QDebug dbg)
{
    dbg.nospace() << "QOrganizerCollectionMemoryEngineLocalId(" << m_localCollectionId << ")";
    return dbg.maybeSpace();
}
#endif

#ifndef QT_NO_DATASTREAM
QDataStream& QOrganizerCollectionMemoryEngineLocalId::dataStreamOut(QDataStream& out)
{
    return (out << m_localCollectionId);
}

QDataStream& QOrganizerCollectionMemoryEngineLocalId::dataStreamIn(QDataStream& in)
{
    return (in >> m_localCollectionId);
}
#endif

uint QOrganizerCollectionMemoryEngineLocalId::hash() const
{
    return QT_PREPEND_NAMESPACE(qHash)(m_localCollectionId);
}

QOrganizerItemMemoryEngineData::QOrganizerItemMemoryEngineData()
    : QSharedData(),
    m_refCount(QAtomicInt(1)),
    m_nextOrganizerItemId(1),
    m_nextOrganizerCollectionId(2),
    m_anonymous(false)
{

}

/*!
 * Factory function for creating a new in-memory backend, based
 * on the given \a parameters.
 *
 * The same engine will be returned for multiple calls with the
 * same value for the "id" parameter, while one of them is in scope.
 */
QOrganizerItemMemoryEngine* QOrganizerItemMemoryEngine::createMemoryEngine(const QMap<QString, QString>& parameters)
{
    bool anonymous = false;
    QString idValue = parameters.value(QLatin1String("id"));
    if (idValue.isNull() || idValue.isEmpty()) {
        // no store given?  new, anonymous store.
        idValue = QUuid::createUuid().toString();
        anonymous = true;
    }

    QOrganizerItemMemoryEngineData* data = engineDatas.value(idValue);
    if (data) {
        data->m_refCount.ref();
    } else {
        data = new QOrganizerItemMemoryEngineData();
        data->m_id = idValue;
        data->m_anonymous = anonymous;
        engineDatas.insert(idValue, data);
    }
    return new QOrganizerItemMemoryEngine(data);
}

/*!
 * Constructs a new in-memory backend which shares the given \a data with
 * other shared memory engines.
 */
QOrganizerItemMemoryEngine::QOrganizerItemMemoryEngine(QOrganizerItemMemoryEngineData* data)
    : d(data)
{
    d->m_sharedEngines.append(this);

    // the default collection always exists.
    if (d->m_organizerCollectionIds.isEmpty()) {
        QOrganizerCollectionLocalId defaultLocalId = QOrganizerCollectionLocalId(new QOrganizerCollectionMemoryEngineLocalId(1));
        QOrganizerCollectionId defaultId;
        defaultId.setLocalId(defaultLocalId);
        defaultId.setManagerUri(managerUri());
        QOrganizerCollection defaultCollection;
        defaultCollection.setId(defaultId);
        defaultCollection.setMetaData(QOrganizerCollection::KeyName, QString(QLatin1String("Default Collection")));
        d->m_organizerCollectionIds << defaultLocalId;
        d->m_organizerCollections << defaultCollection;
    }
}

/*! Frees any memory used by this engine */
QOrganizerItemMemoryEngine::~QOrganizerItemMemoryEngine()
{
    d->m_sharedEngines.removeAll(this);
    if (!d->m_refCount.deref()) {
        engineDatas.remove(d->m_id);
        delete d;
    }
}

/*! \reimp */
QString QOrganizerItemMemoryEngine::managerName() const
{
    return QLatin1String("memory");
}

/*! \reimp */
QMap<QString, QString> QOrganizerItemMemoryEngine::managerParameters() const
{
    QMap<QString, QString> params;
    params.insert(QLatin1String("id"), d->m_id);
    return params;
}

/*! \reimp */
QOrganizerItem QOrganizerItemMemoryEngine::item(const QOrganizerItemLocalId& organizeritemId, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const
{
    Q_UNUSED(fetchHint); // no optimisations are possible in the memory backend; ignore the fetch hint.
    int index = d->m_organizeritemIds.indexOf(organizeritemId);
    if (index != -1) {
        // found the organizer item successfully.
        *error = QOrganizerItemManager::NoError;
        return d->m_organizeritems.at(index);
    }

    *error = QOrganizerItemManager::DoesNotExistError;
    return QOrganizerItem();
}

/*! \reimp */
QList<QOrganizerItemLocalId> QOrganizerItemMemoryEngine::itemIds(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, QOrganizerItemManager::Error* error) const
{
    /* Special case the fast case */
    if (filter.type() == QOrganizerItemFilter::DefaultFilter && sortOrders.count() == 0) {
        return d->m_organizeritemIds;
    } else {
        QList<QOrganizerItem> clist = items(filter, sortOrders, QOrganizerItemFetchHint(), error);

        /* Extract the ids */
        QList<QOrganizerItemLocalId> ids;
        foreach(const QOrganizerItem& c, clist)
            ids.append(c.localId());

        return ids;
    }
}

/*! \reimp */
QList<QOrganizerItem> QOrganizerItemMemoryEngine::itemInstances(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const
{
    Q_UNUSED(filter);
    Q_UNUSED(sortOrders);
    Q_UNUSED(fetchHint);
    // not implemented. XXX TODO.
    *error = QOrganizerItemManager::NotSupportedError;
    return QList<QOrganizerItem>();
}

QList<QDateTime> QOrganizerItemMemoryEngine::generateDateTimes(const QDateTime& initialDateTime, QOrganizerItemRecurrenceRule rrule, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount) const
{
    QList<QDateTime> retn;

    if (periodEnd.isValid() || maxCount <= 0)
        maxCount = INT_MAX; // count of returned items is unlimited
    if (rrule.count() > 0)
        maxCount = qMin(maxCount, rrule.count());

    QDateTime realPeriodEnd(periodEnd);
    if (rrule.endDate().isValid() && rrule.endDate() < realPeriodEnd.date())
        realPeriodEnd.setDate(rrule.endDate());

    QDate nextDate = periodStart.date();

    inferMissingCriteria(&rrule, initialDateTime.date());

    while (nextDate < realPeriodEnd.date()) {
        // Skip nextDate if it is not the right multiple of intervals away from initialDateTime.
        if (inIntervaledPeriod(nextDate, initialDateTime.date(), rrule.frequency(), rrule.interval(), rrule.weekStart())) {
            // Calculate the inclusive start and exclusive end of nextDate's week/month/year
            QDate subPeriodStart = firstDateInPeriod(nextDate, rrule.frequency(), rrule.weekStart());
            QDate subPeriodEnd = firstDateInNextPeriod(nextDate, rrule.frequency(), rrule.weekStart());

            // Compute matchesInPeriod to be the set of dates in the current week/month/year that match the rrule
            QList<QDate> matchesInPeriod = filterByPosition(
                    matchingDates(subPeriodStart, subPeriodEnd, rrule),
                    rrule.positions());

            // A final filter over the dates list before adding it to the returned list
            foreach (const QDate& match, matchesInPeriod) {
                nextDate = match;
                if (match >= realPeriodEnd.date() || retn.size() >= maxCount)
                    break;

                QDateTime generatedDateTime;
                generatedDateTime.setDate(match);
                generatedDateTime.setTime(initialDateTime.time());
                if (generatedDateTime >= periodStart && generatedDateTime < realPeriodEnd) {
                    retn.append(generatedDateTime);
                } else {
                    // We've gone past the end of the period.  Ensure we break both the foreach and
                    // the while loop
                    nextDate = match.addDays(1);
                    break;
                }
            }
        }
        nextDate = firstDateInNextPeriod(nextDate, rrule.frequency(), rrule.weekStart());
    }

    return retn;
}

/*!
 * Determines if \a rrule is underspecified and if so, fills in missing information based on \a
 * initialDate.
 */
void QOrganizerItemMemoryEngine::inferMissingCriteria(QOrganizerItemRecurrenceRule* rrule, const QDate& initialDate) const
{
    switch (rrule->frequency()) {
        case QOrganizerItemRecurrenceRule::Weekly:
            if (rrule->daysOfWeek().isEmpty()) {
                // derive day of week
                QList<Qt::DayOfWeek> days;
                days.append(static_cast<Qt::DayOfWeek>(initialDate.dayOfWeek()));
                rrule->setDaysOfWeek(days);
            }
            break;
        case QOrganizerItemRecurrenceRule::Monthly:
            if (rrule->daysOfWeek().isEmpty() && rrule->daysOfMonth().isEmpty()) {
                // derive day of month
                QList<int> days;
                days.append(initialDate.day());
                rrule->setDaysOfMonth(days);
            }
            break;
        case QOrganizerItemRecurrenceRule::Yearly:
            if (rrule->months().isEmpty()
                    && rrule->weeksOfYear().isEmpty()
                    && rrule->daysOfYear().isEmpty()
                    && rrule->daysOfMonth().isEmpty()
                    && rrule->daysOfWeek().isEmpty()) {
                // derive day of month and month of year
                QList<int> daysOfMonth;
                daysOfMonth.append(initialDate.day());
                rrule->setDaysOfMonth(daysOfMonth);
                QList<QOrganizerItemRecurrenceRule::Month> months;
                months.append(static_cast<QOrganizerItemRecurrenceRule::Month>(initialDate.month()));
                rrule->setMonths(months);
            } else if (!rrule->months().isEmpty()
                    && rrule->weeksOfYear().isEmpty()
                    && rrule->daysOfYear().isEmpty()
                    && rrule->daysOfMonth().isEmpty()
                    && rrule->daysOfWeek().isEmpty()) {
                // derive day of month
                QList<int> daysOfMonth;
                daysOfMonth.append(initialDate.day());
                rrule->setDaysOfMonth(daysOfMonth);
            } else if (!rrule->weeksOfYear().isEmpty()
                    && rrule->daysOfYear().isEmpty()
                    && rrule->daysOfMonth().isEmpty()
                    && rrule->daysOfWeek().isEmpty()) {
                // derive day of week
                QList<Qt::DayOfWeek> days;
                days.append(static_cast<Qt::DayOfWeek>(initialDate.dayOfWeek()));
                rrule->setDaysOfWeek(days);
            }
            break;
        case QOrganizerItemRecurrenceRule::Daily:
            break;
        case QOrganizerItemRecurrenceRule::Invalid:
            Q_ASSERT(false);
    }
}

/*!
 * Returns true iff the calendar period (specified by \a frequency) of \a date is an \a
 * interval-multiple of periods ahead of the calendar period of \a initialDate.  For Weekly
 * frequencies, \a firstDayOfWeek is used to determine when the week boundary is.
 * eg. If \a frequency is Monthly and \a interval is 3, then true is returned iff \a date is in the
 * same month as \a initialDate, in a month 3 months ahead, 6 months ahead, etc.
 */
bool QOrganizerItemMemoryEngine::inIntervaledPeriod(const QDate& date, const QDate& initialDate, QOrganizerItemRecurrenceRule::Frequency frequency, int interval, Qt::DayOfWeek firstDayOfWeek) const
{
    if (interval <= 1)
        return true;
    switch (frequency) {
        case QOrganizerItemRecurrenceRule::Yearly: {
            int yearsDelta = date.year() - initialDate.year();
            return (yearsDelta % interval == 0);
        }
        case QOrganizerItemRecurrenceRule::Monthly: {
            int monthsDelta = date.month() - initialDate.month() + (12 * (date.year() - initialDate.year()));
            return (monthsDelta % interval == 0);
        }
        case QOrganizerItemRecurrenceRule::Weekly: {
            // we need to adjust for the week start specified by the client if the interval is greater than 1
            // ie, every time we hit the day specified, we increment the week count.
            int weekCount = 0;
            QDate tempDate = initialDate;
            while (tempDate < date) {
                tempDate = tempDate.addDays(1);
                if (static_cast<Qt::DayOfWeek>(tempDate.dayOfWeek()) == firstDayOfWeek) {
                    weekCount += 1;
                }
            }
            return (weekCount % interval == 0);
        }
        case QOrganizerItemRecurrenceRule::Daily: {
            int daysDelta = initialDate.daysTo(date);
            return (daysDelta % interval == 0);
        }
        case QOrganizerItemRecurrenceRule::Invalid:
            Q_ASSERT(false);
    }
    return true;
}

/*!
 * Returns the date which is the first date of the calendar period that \a date resides in.  eg. if
 * the \a frequency is Monthly, then this returns the first day of \a date's month.  If the \a
 * frequency is Weekly, then it returns the first day of \a date's week, considering the week to
 * start on \a firstDayOfWeek
 */
QDate QOrganizerItemMemoryEngine::firstDateInPeriod(const QDate& date, QOrganizerItemRecurrenceRule::Frequency frequency, Qt::DayOfWeek firstDayOfWeek) const
{
    QDate retn(date);
    switch (frequency) {
        case QOrganizerItemRecurrenceRule::Yearly:
            retn.setDate(date.year(), 1, 1);
            return retn;
        case QOrganizerItemRecurrenceRule::Monthly:
            retn.setDate(date.year(), date.month(), 1);
            return retn;
        case QOrganizerItemRecurrenceRule::Weekly:
            while (retn.dayOfWeek() != firstDayOfWeek) {
                retn = retn.addDays(-1);
            } 
            return retn;
        case QOrganizerItemRecurrenceRule::Daily:
            return retn;
        default:
            Q_ASSERT(false);
            return retn;
    }
}

/*!
 * Returns the date which is the first date of the next calendar period after \a date specified by
 * \a frequency.  eg. if \a frequency is Monthly, then this returns the first day of the next month.
 * If \a frequency is Weekly, then it returns the first \a firstDayOfWeek after \a date.
 */
QDate QOrganizerItemMemoryEngine::firstDateInNextPeriod(const QDate& date, QOrganizerItemRecurrenceRule::Frequency frequency, Qt::DayOfWeek firstDayOfWeek) const
{
    QDate retn(date);
    switch (frequency) {
        case QOrganizerItemRecurrenceRule::Yearly:
            retn.setDate(date.year()+1, 1, 1);
            return retn;
        case QOrganizerItemRecurrenceRule::Monthly:
            {
            int newMonth = date.month() + 1;
            int newYear = date.year() + (newMonth==13 ? 1 : 0);
            retn.setDate(newYear, newMonth==13 ? 1 : newMonth, 1);
            }
            return retn;
        case QOrganizerItemRecurrenceRule::Weekly:
            do {
                retn = retn.addDays(1);
            } while (retn.dayOfWeek() != firstDayOfWeek);
            return retn;
        case QOrganizerItemRecurrenceRule::Daily:
            retn = retn.addDays(1);
            return retn;
        case QOrganizerItemRecurrenceRule::Invalid:
            Q_ASSERT(false);
    }
    return retn;
}

/*!
 * Returns a list of dates between \a periodStart (inclusive) and \a periodEnd (exclusive) which
 * match the \a rrule.  Only daysOfWeek, daysOfMonth, daysOfYear, weeksOfYear and months from the \a
 * rrule are matched.
 */
QList<QDate> QOrganizerItemMemoryEngine::matchingDates(const QDate& periodStart, const QDate& periodEnd, const QOrganizerItemRecurrenceRule& rrule) const
{
    QList<QDate> retn;

    QList<Qt::DayOfWeek> daysOfWeek = rrule.daysOfWeek();
    QList<int> daysOfMonth = rrule.daysOfMonth();
    QList<int> daysOfYear = rrule.daysOfYear();
    QList<int> weeksOfYear = rrule.weeksOfYear();
    QList<QOrganizerItemRecurrenceRule::Month> monthsOfYear = rrule.months();

    QDate tempDate = periodStart;
    while (tempDate < periodEnd) {
        if ((monthsOfYear.isEmpty() || monthsOfYear.contains(static_cast<QOrganizerItemRecurrenceRule::Month>(tempDate.month())))
                && (weeksOfYear.isEmpty() || weeksOfYear.contains(tempDate.weekNumber()))
                && (daysOfYear.isEmpty() || daysOfYear.contains(tempDate.dayOfYear()))
                && (daysOfMonth.isEmpty() || daysOfMonth.contains(tempDate.day()))
                && (daysOfWeek.isEmpty() || daysOfWeek.contains(static_cast<Qt::DayOfWeek>(tempDate.dayOfWeek())))) {
            retn.append(tempDate);
        }
        tempDate = tempDate.addDays(1);
    }
    return retn;
}

/*!
 * Returns a list of dates from \a dates which are at the indices specified by \a positions.
 * For positive values in \a positions, the values represent a 1-based index into \a dates.
 * For negative values, they represent indices counting from the end of \a dates (eg. -1 means the
 * last value of \a dates).
 */
QList<QDate> QOrganizerItemMemoryEngine::filterByPosition(const QList<QDate>& dates, const QList<int> positions) const
{
    if (positions.isEmpty()) {
        return dates;
    } else {
        QList<QDate> retn;
        foreach (int i, positions) {
            if (i >= 1 && i <= dates.size()) {
                // positions is 1-indexed, but the QList is 0-indexed
                retn.append(dates[i-1]);
            } else if (i <= -1 && i >= -dates.size()) {
                // for negative values, count from the end of the list
                retn.append(dates[dates.size() + i]);
            }
        }
        return retn;
    }
}

/*! \reimp */
QList<QOrganizerItem> QOrganizerItemMemoryEngine::itemInstances(const QOrganizerItem& generator, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount, QOrganizerItemManager::Error* error) const
{
    // given the generating item, grab it's QOrganizerItemRecurrence detail (if it exists), and calculate all of the dates within the given period.
    // how would a real backend do this?
    // Also, should this also return the exception instances (ie, return any persistent instances with parent information == generator?)
    // XXX TODO: in detail validation, ensure that the referenced parent Id exists...

    QDateTime realPeriodStart(periodStart);
    QDateTime realPeriodEnd(periodEnd);
    QDateTime initialDateTime;
    if (generator.type() == QOrganizerItemType::TypeEvent) {
        QOrganizerEvent evt = generator;
        initialDateTime = evt.startDateTime();
    } else if (generator.type() == QOrganizerItemType::TypeTodo) {
        QOrganizerTodo todo = generator;
        initialDateTime = todo.startDateTime();
    } else {
        // erm... not a recurring item in our schema...
        return QList<QOrganizerItem>() << generator;
    }

    if (initialDateTime > realPeriodStart)
        realPeriodStart = initialDateTime;
    if (!periodEnd.isValid()) {
        // If no endDateTime is given, we'll only generate items that occur within the next 4 years of realPeriodStart.
        realPeriodEnd.setDate(realPeriodStart.date().addDays(1461));
        realPeriodEnd.setTime(realPeriodStart.time());
    }
    if (realPeriodStart > realPeriodEnd) {
        *error = QOrganizerItemManager::BadArgumentError;
        return QList<QOrganizerItem>();
    }

    QList<QOrganizerItem> retn;
    QOrganizerItemRecurrence recur = generator.detail(QOrganizerItemRecurrence::DefinitionName);

    // first, retrieve all persisted instances (exceptions) which occur between the specified datetimes.
    QOrganizerItemDetailFilter parentFilter;
    parentFilter.setDetailDefinitionName(QOrganizerItemInstanceOrigin::DefinitionName, QOrganizerItemInstanceOrigin::FieldParentLocalId);
    parentFilter.setValue(QVariant::fromValue(generator.localId()));
    QList<QOrganizerItem> persistedExceptions = items(parentFilter, QList<QOrganizerItemSortOrder>(), QOrganizerItemFetchHint(), error);
    foreach (const QOrganizerItem& currException, persistedExceptions) {
        QDateTime lowerBound;
        QDateTime upperBound;
        if (currException.type() == QOrganizerItemType::TypeEventOccurrence) {
            QOrganizerEventOccurrence instance = currException;
            lowerBound = instance.startDateTime();
            upperBound = instance.endDateTime();
        } else {
            QOrganizerTodoOccurrence instance = currException;
            lowerBound = instance.startDateTime();
            upperBound = instance.dueDateTime();
        }

        if ((lowerBound.isNull() || lowerBound > realPeriodStart) && (upperBound.isNull() || upperBound < realPeriodEnd)) {
            // this occurrence fulfils the criteria.
            retn.append(currException);
        }
    }

    // then, generate the required (unchanged) instances from the generator.
    // before doing that, we have to find out all of the exception dates.
    QList<QDate> xdates;
    foreach (const QDate& xdate, recur.exceptionDates()) {
        xdates += xdate;
    }
    QList<QOrganizerItemRecurrenceRule> xrules = recur.exceptionRules();
    foreach (const QOrganizerItemRecurrenceRule& xrule, xrules) {
        if (xrule.frequency() != QOrganizerItemRecurrenceRule::Invalid
                && ((xrule.endDate().isNull()) || (xrule.endDate() >= realPeriodStart.date()))) {
            // we cannot skip it, since it applies in the given time period.
            QList<QDateTime> xdatetimes = generateDateTimes(initialDateTime, xrule, realPeriodStart, realPeriodEnd, 50); // max count of 50 is arbitrary...
            foreach (const QDateTime& xdatetime, xdatetimes) {
                xdates += xdatetime.date();
            }
        }
    }

    // now generate a list of rdates (from the recurrenceDates and recurrenceRules)
    QList<QDateTime> rdates;
    foreach (const QDate& rdate, recur.recurrenceDates()) {
        rdates += QDateTime(rdate, initialDateTime.time());
    }
    QList<QOrganizerItemRecurrenceRule> rrules = recur.recurrenceRules();
    foreach (const QOrganizerItemRecurrenceRule& rrule, rrules) {
        if (rrule.frequency() != QOrganizerItemRecurrenceRule::Invalid
                && ((rrule.endDate().isNull()) || (rrule.endDate() >= realPeriodStart.date()))) {
            // we cannot skip it, since it applies in the given time period.
            rdates += generateDateTimes(initialDateTime, rrule, realPeriodStart, realPeriodEnd, 50); // max count of 50 is arbitrary...
        }
    }

    // now order the contents of retn by date
    qSort(rdates);
    if (qBinaryFind(rdates, initialDateTime) == rdates.end()) {
        rdates.prepend(initialDateTime);
    }

    // now for each rdate which isn't also an xdate
    foreach (const QDateTime& rdate, rdates) {
        if (!xdates.contains(rdate.date()) && rdate >= realPeriodStart && rdate < realPeriodEnd) {
            // generate the required instance and add it to the return list.
            retn.append(generateInstance(generator, rdate));
        }
    }

    // and return the first maxCount entries.
    return retn.mid(0, maxCount);
}

QOrganizerItem QOrganizerItemMemoryEngine::generateInstance(const QOrganizerItem& generator, const QDateTime& rdate)
{
    QOrganizerItem instanceItem;
    if (generator.type() == QOrganizerItemType::TypeEvent) {
        instanceItem = QOrganizerEventOccurrence();
    } else {
        instanceItem = QOrganizerTodoOccurrence();
    }

    // XXX TODO: something better than this linear search...
    // Grab all details from the generator except the recurrence information, and event/todo time range
    QList<QOrganizerItemDetail> allDets = generator.details();
    QList<QOrganizerItemDetail> occDets;
    foreach (const QOrganizerItemDetail& det, allDets) {
        if (det.definitionName() != QOrganizerItemRecurrence::DefinitionName
                && det.definitionName() != QOrganizerEventTimeRange::DefinitionName
                && det.definitionName() != QOrganizerTodoTimeRange::DefinitionName) {
            occDets.append(det);
        }
    }

    // add the detail which identifies exactly which instance this item is.
    QOrganizerItemInstanceOrigin currOrigin;
    currOrigin.setParentLocalId(generator.localId());
    currOrigin.setOriginalDate(rdate.date());
    occDets.append(currOrigin);

    // save those details in the instance.
    foreach (const QOrganizerItemDetail& det, occDets) {
        // copy every detail except the type
        if (det.definitionName() != QOrganizerItemType::DefinitionName) {
            QOrganizerItemDetail modifiable = det;
            instanceItem.saveDetail(&modifiable);
        }
    }

    // and update the time range in the instance based on the current instance date
    if (generator.type() == QOrganizerItemType::TypeEvent) {
        QOrganizerEventTimeRange etr = generator.detail<QOrganizerEventTimeRange>();
        QDateTime temp = etr.startDateTime();
        temp.setDate(rdate.date());
        etr.setStartDateTime(temp);
        temp = etr.endDateTime();
        temp.setDate(rdate.date());
        etr.setEndDateTime(temp);
        instanceItem.saveDetail(&etr);
    }

    // for todo's?
    if (generator.type() == QOrganizerItemType::TypeTodo) {
        QOrganizerTodoTimeRange ttr = generator.detail<QOrganizerTodoTimeRange>();
        QDateTime temp = ttr.dueDateTime();
        temp.setDate(rdate.date());
        ttr.setDueDateTime(temp);
        temp = ttr.startDateTime();
        temp.setDate(rdate.date());
        ttr.setStartDateTime(temp);
        instanceItem.saveDetail(&ttr);
    }

    return instanceItem;
}

/*! \reimp */
QList<QOrganizerItem> QOrganizerItemMemoryEngine::items(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const
{
    Q_UNUSED(fetchHint); // no optimisations are possible in the memory backend; ignore the fetch hint.
    Q_UNUSED(error);

    QList<QOrganizerItem> sorted;

    /* First filter out organizer items - check for default filter first */
    if (filter.type() == QOrganizerItemFilter::DefaultFilter) {
        foreach(const QOrganizerItem&c, d->m_organizeritems) {
            QOrganizerItemManagerEngine::addSorted(&sorted,c, sortOrders);
        }
    } else {
        foreach(const QOrganizerItem&c, d->m_organizeritems) {
            if (QOrganizerItemManagerEngine::testFilter(filter, c))
                QOrganizerItemManagerEngine::addSorted(&sorted,c, sortOrders);
        }
    }

    return sorted;
}

/*! Saves the given organizeritem \a theOrganizerItem, storing any error to \a error and
    filling the \a changeSet with ids of changed organizeritems as required */
bool QOrganizerItemMemoryEngine::saveItem(QOrganizerItem* theOrganizerItem, const QOrganizerCollectionLocalId& collectionId, QOrganizerItemChangeSet& changeSet, QOrganizerItemManager::Error* error)
{
    QOrganizerCollectionLocalId targetCollectionId = collectionId;

    // check that the collection exists (or is null :. default collection):
    if (!targetCollectionId.isNull() && !d->m_organizerCollectionIds.contains(targetCollectionId)) {
        *error = QOrganizerItemManager::InvalidCollectionError;
        return false;
    }

    // ensure that the organizer item's details conform to their definitions
    if (!validateItem(*theOrganizerItem, error)) {
        return false;
    }

    // check to see if this organizer item already exists
    int index = d->m_organizeritemIds.indexOf(theOrganizerItem->id().localId());
    if (index != -1) {
        /* We also need to check that there are no modified create only details */
        QOrganizerItem oldOrganizerItem = d->m_organizeritems.at(index);

        if (oldOrganizerItem.type() != theOrganizerItem->type()) {
            *error = QOrganizerItemManager::AlreadyExistsError;
            return false;
        }

        // check that the old and new collection is the same (ie, not attempting to save to a different collection)
        if (targetCollectionId.isNull()) {
            // it already exists, so save it where it already exists.
            targetCollectionId = d->m_itemsInCollections.key(theOrganizerItem->localId());
        } else if (!d->m_itemsInCollections.values(targetCollectionId).contains(theOrganizerItem->localId())) {
            // the given collection id was non-null but doesn't already contain this item.  error.
            *error = QOrganizerItemManager::InvalidCollectionError;
            return false;
        }

        QOrganizerItemTimestamp ts = theOrganizerItem->detail(QOrganizerItemTimestamp::DefinitionName);
        ts.setLastModified(QDateTime::currentDateTime());
        QOrganizerItemManagerEngine::setDetailAccessConstraints(&ts, QOrganizerItemDetail::ReadOnly | QOrganizerItemDetail::Irremovable);
        theOrganizerItem->saveDetail(&ts);

        if (!fixOccurrenceReferences(theOrganizerItem, error)) {
            return false;
        }

        // Looks ok, so continue
        d->m_organizeritems.replace(index, *theOrganizerItem);
        changeSet.insertChangedItem(theOrganizerItem->localId());
    } else {
        // id does not exist; if not zero, fail.
        QOrganizerItemId newId;
        newId.setManagerUri(managerUri());
        if (!theOrganizerItem->id().isNull()) {
            // the ID is not empty, and it doesn't identify an existing organizer item in our database either.
            *error = QOrganizerItemManager::DoesNotExistError;
            return false;
        }

        /* New organizer item */
        QOrganizerItemTimestamp ts = theOrganizerItem->detail(QOrganizerItemTimestamp::DefinitionName);
        ts.setLastModified(QDateTime::currentDateTime());
        ts.setCreated(ts.lastModified());
        setDetailAccessConstraints(&ts, QOrganizerItemDetail::ReadOnly | QOrganizerItemDetail::Irremovable);
        theOrganizerItem->saveDetail(&ts);

        // now modify the target collection id; if null, set to default id.  BUT record whether we did that.
        bool targetCollectionWasNull = false; // this determines for OCCURRENCES whether we ignore the default id.
        if (targetCollectionId.isNull()) {
            targetCollectionWasNull = true;
            targetCollectionId = QOrganizerCollectionLocalId(new QOrganizerCollectionMemoryEngineLocalId(1));
        }

        // update the organizer item - set its ID
        quint32 nextOrganizerItemId = d->m_nextOrganizerItemId; // don't increment it until we're successful.
        nextOrganizerItemId += 1;
        QOrganizerItemMemoryEngineLocalId* newMemoryEngineLocalId = new QOrganizerItemMemoryEngineLocalId;
        newMemoryEngineLocalId->m_localCollectionId = static_cast<QOrganizerCollectionMemoryEngineLocalId*>(QOrganizerItemManagerEngine::engineLocalCollectionId(targetCollectionId))->m_localCollectionId;
        newMemoryEngineLocalId->m_localItemId = nextOrganizerItemId;
        newId.setLocalId(QOrganizerItemLocalId(newMemoryEngineLocalId));
        theOrganizerItem->setId(newId);
        // note: do NOT delete the QOrganizerItemMemoryEngineLocalId -- the QOrganizerItemLocalId ctor takes ownership of it.

        if (!fixOccurrenceReferences(theOrganizerItem, error)) {
            return false;
        }
        // set the guid if not set, and ensure that it's the same as the parent's
        if (theOrganizerItem->guid().isEmpty())
            theOrganizerItem->setGuid(QUuid::createUuid().toString());

        // if we're saving an exception occurrence, we need to add it's original date as an exdate to the parent.
        if (theOrganizerItem->type() == QOrganizerItemType::TypeEventOccurrence) {
            // update the event by adding an EX-DATE which corresponds to the original date of the occurrence being saved.
            QOrganizerItemManager::Error tempError = QOrganizerItemManager::NoError;
            QOrganizerItemInstanceOrigin origin = theOrganizerItem->detail<QOrganizerItemInstanceOrigin>();
            QOrganizerItemLocalId parentId = origin.parentLocalId();

            // for occurrences, if given a null collection id, save it in the same collection as the parent.
            // otherwise, ensure that the parent is in the same collection.  You cannot save an exception to a different collection than the parent.
            if (targetCollectionWasNull) {
                targetCollectionId = d->m_itemsInCollections.key(parentId);
                if (targetCollectionId.isNull()) {
                    *error = QOrganizerItemManager::UnspecifiedError; // this should never occur; parent should _always_ be in a collection.
                    return false;
                }
                newId.setLocalId(QOrganizerItemLocalId(QOrganizerItemLocalId(new QOrganizerItemMemoryEngineLocalId(static_cast<QOrganizerCollectionMemoryEngineLocalId*>(QOrganizerItemManagerEngine::engineLocalCollectionId(targetCollectionId))->m_localCollectionId, nextOrganizerItemId))));
                theOrganizerItem->setId(newId);
            } else if (!d->m_itemsInCollections.values(targetCollectionId).contains(parentId)) {
                // nope, the specified collection doesn't contain the parent.  error.
                *error = QOrganizerItemManager::InvalidCollectionError;
                return false;
            }

            QOrganizerEvent parentEvent = item(parentId, QOrganizerItemFetchHint(), &tempError);
            QDate originalDate = origin.originalDate();
            QList<QDate> currentExceptionDates = parentEvent.exceptionDates();
            if (!currentExceptionDates.contains(originalDate)) {
                currentExceptionDates.append(originalDate);
                parentEvent.setExceptionDates(currentExceptionDates);
                int parentEventIndex = d->m_organizeritemIds.indexOf(parentEvent.localId());
                d->m_organizeritems.replace(parentEventIndex, parentEvent);
                changeSet.insertChangedItem(parentEvent.localId()); // is this correct?  it's an exception, so change parent?
            }
        } else if (theOrganizerItem->type() == QOrganizerItemType::TypeTodoOccurrence) {
            // update the todo by adding an EX-DATE which corresponds to the original date of the occurrence being saved.
            QOrganizerItemManager::Error tempError = QOrganizerItemManager::NoError;
            QOrganizerItemInstanceOrigin origin = theOrganizerItem->detail<QOrganizerItemInstanceOrigin>();
            QOrganizerItemLocalId parentId = origin.parentLocalId();

            // for occurrences, if given a null collection id, save it in the same collection as the parent.
            // otherwise, ensure that the parent is in the same collection.  You cannot save an exception to a different collection than the parent.
            if (targetCollectionWasNull) {
                targetCollectionId = d->m_itemsInCollections.key(parentId);
                if (targetCollectionId.isNull()) {
                    *error = QOrganizerItemManager::UnspecifiedError; // this should never occur; parent should _always_ be in a collection.
                    return false;
                }
                newId.setLocalId(QOrganizerItemLocalId(QOrganizerItemLocalId(new QOrganizerItemMemoryEngineLocalId(static_cast<QOrganizerCollectionMemoryEngineLocalId*>(QOrganizerItemManagerEngine::engineLocalCollectionId(targetCollectionId))->m_localCollectionId, nextOrganizerItemId))));
                theOrganizerItem->setId(newId);
            } else if (!d->m_itemsInCollections.values(targetCollectionId).contains(parentId)) {
                // nope, the specified collection doesn't contain the parent.  error.
                *error = QOrganizerItemManager::InvalidCollectionError;
                return false;
            }

            QOrganizerTodo parentTodo = item(parentId, QOrganizerItemFetchHint(), &tempError);
            QDate originalDate = origin.originalDate();
            QList<QDate> currentExceptionDates = parentTodo.exceptionDates();
            if (!currentExceptionDates.contains(originalDate)) {
                currentExceptionDates.append(originalDate);
                parentTodo.setExceptionDates(currentExceptionDates);
                int parentTodoIndex = d->m_organizeritemIds.indexOf(parentTodo.localId());
                d->m_organizeritems.replace(parentTodoIndex, parentTodo);
                changeSet.insertChangedItem(parentTodo.localId()); // is this correct?  it's an exception, so change parent?
            }
        }

        // given that we were successful, now increment the persistent version of the next item id.
        d->m_nextOrganizerItemId += 1;

        // finally, add the organizer item to our internal lists and return
        QOrganizerCollectionId targetCompleteCollectionId;
        targetCompleteCollectionId.setLocalId(targetCollectionId);
        targetCompleteCollectionId.setManagerUri(managerUri());
        QOrganizerItemManagerEngine::setItemCollectionId(theOrganizerItem, targetCompleteCollectionId);
        d->m_organizeritems.append(*theOrganizerItem);              // add organizer item to list
        d->m_organizeritemIds.append(theOrganizerItem->localId());  // track the organizer item id.
        d->m_itemsInCollections.insert(targetCollectionId, theOrganizerItem->localId());

        changeSet.insertAddedItem(theOrganizerItem->localId());

        // XXX TODO: prior to all of this, need to check:
        // 1) is it an Occurrence item?
        //      - if so, does it differ from the generated instance for that date?
        //          - if not different, return AlreadyExistsError
        //          - if different, save it, AND THEN UPDATE THE PARENT ITEM with EXDATE added!
        //      - if not, the current codepath is ok.
    }

    *error = QOrganizerItemManager::NoError;     // successful.
    return true;
}

/*!
 * For Occurrence type items, ensure the ParentLocalId and the Guid are set consistently.  Returns
 * false and sets \a error on error, returns true otherwise.
 */
bool QOrganizerItemMemoryEngine::fixOccurrenceReferences(QOrganizerItem* theItem, QOrganizerItemManager::Error* error)
{
    if (theItem->type() == QOrganizerItemType::TypeEventOccurrence
            || theItem->type() == QOrganizerItemType::TypeTodoOccurrence) {
        const QString guid = theItem->guid();
        QOrganizerItemInstanceOrigin instanceOrigin = theItem->detail<QOrganizerItemInstanceOrigin>();
        if (!instanceOrigin.originalDate().isValid()) {
            *error = QOrganizerItemManager::InvalidOccurrenceError;
            return false;
        }
        QOrganizerItemLocalId parentId = instanceOrigin.parentLocalId();
        if (!guid.isEmpty()) {
            if (!parentId.isNull()) {
                QOrganizerItemManager::Error tempError;
                QOrganizerItem parentItem = item(parentId, QOrganizerItemFetchHint(), &tempError);
                if (guid != parentItem.guid()
                        || !typesAreRelated(theItem->type(), parentItem.type())) {
                    // parentId and guid are both set and inconsistent, or the parent is the wrong
                    // type
                    *error = QOrganizerItemManager::InvalidOccurrenceError;
                    return false;
                }
            } else {
                // guid set but not parentId
                // find an item with the given guid
                foreach (const QOrganizerItem& item, d->m_organizeritems) {
                    if (item.guid() == guid) {
                        parentId = item.localId();
                        break;
                    }
                }
                if (parentId.isNull()) {
                    // couldn't find an item with the given guid
                    *error = QOrganizerItemManager::InvalidOccurrenceError;
                    return false;
                }
                QOrganizerItemManager::Error tempError;
                QOrganizerItem parentItem = item(parentId, QOrganizerItemFetchHint(), &tempError);
                if (!typesAreRelated(theItem->type(), parentItem.type())) {
                    // the parent is the wrong type
                    *error = QOrganizerItemManager::InvalidOccurrenceError;
                    return false;
                }
                // found a matching item - set the parentId of the occurrence
                QOrganizerItemInstanceOrigin origin = theItem->detail<QOrganizerItemInstanceOrigin>();
                origin.setParentLocalId(parentId);
                theItem->saveDetail(&origin);
            }
        } else if (!parentId.isNull()) {
            QOrganizerItemManager::Error tempError;
            QOrganizerItem parentItem = item(parentId, QOrganizerItemFetchHint(), &tempError);
            if (parentItem.guid().isEmpty()
                    || !typesAreRelated(theItem->type(), parentItem.type())) {
                // found the matching item but it has no guid, or it isn't the right type
                *error = QOrganizerItemManager::InvalidOccurrenceError;
                return false;
            }
            theItem->setGuid(parentItem.guid());
        } else {
            // neither parentId or guid is supplied
            *error = QOrganizerItemManager::InvalidOccurrenceError;
            return false;
        }
    }
    return true;
}

/*!
 * Returns true if and only if \a occurrenceType is the "Occurrence" version of \a parentType.
 */
bool QOrganizerItemMemoryEngine::typesAreRelated(const QString& occurrenceType, const QString& parentType)
{
    return ((parentType == QOrganizerItemType::TypeEvent
                && occurrenceType == QOrganizerItemType::TypeEventOccurrence)
            || (parentType == QOrganizerItemType::TypeTodo
                && occurrenceType == QOrganizerItemType::TypeTodoOccurrence));
}

/*! \reimp */
bool QOrganizerItemMemoryEngine::saveItems(QList<QOrganizerItem>* organizeritems, const QOrganizerCollectionLocalId& collectionId, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error)
{
    if(errorMap) {
        errorMap->clear();
    }

    if (!organizeritems) {
        *error = QOrganizerItemManager::BadArgumentError;
        return false;
    }

    QOrganizerItemChangeSet changeSet;
    QOrganizerItem current;
    QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
    for (int i = 0; i < organizeritems->count(); i++) {
        current = organizeritems->at(i);
        if (!saveItem(&current, collectionId, changeSet, error)) {
            operationError = *error;
            errorMap->insert(i, operationError);
        } else {
            (*organizeritems)[i] = current;
        }
    }

    *error = operationError;
    d->emitSharedSignals(&changeSet);
    // return false if some error occurred
    return (*error == QOrganizerItemManager::NoError);
}

/*! Removes the organizer item identified by the given \a organizeritemId, storing any error to \a error and
    filling the \a changeSet with ids of changed organizer items and relationships as required */
bool QOrganizerItemMemoryEngine::removeItem(const QOrganizerItemLocalId& organizeritemId, QOrganizerItemChangeSet& changeSet, QOrganizerItemManager::Error* error)
{
    int index = d->m_organizeritemIds.indexOf(organizeritemId);

    if (index == -1) {
        *error = QOrganizerItemManager::DoesNotExistError;
        return false;
    }

    // remove the organizer item from the lists.
    QOrganizerItemId thisOrganizerItem;
    thisOrganizerItem.setManagerUri(managerUri());
    thisOrganizerItem.setLocalId(organizeritemId);

    d->m_organizeritems.removeAt(index);
    d->m_organizeritemIds.removeAt(index);
    d->m_itemsInCollections.remove(d->m_itemsInCollections.key(organizeritemId), organizeritemId);
    *error = QOrganizerItemManager::NoError;

    changeSet.insertRemovedItem(organizeritemId);
    return true;
}

/*! \reimp */
bool QOrganizerItemMemoryEngine::removeItems(const QList<QOrganizerItemLocalId>& organizeritemIds, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error)
{
    if (organizeritemIds.count() == 0) {
        *error = QOrganizerItemManager::BadArgumentError;
        return false;
    }
    
    QOrganizerItemChangeSet changeSet;
    QOrganizerItemLocalId current;
    QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
    for (int i = 0; i < organizeritemIds.count(); i++) {
        current = organizeritemIds.at(i);
        if (!removeItem(current, changeSet, error)) {
            operationError = *error;
            errorMap->insert(i, operationError);
        }
    }

    *error = operationError;
    d->emitSharedSignals(&changeSet);
    // return false if some errors occurred
    return (*error == QOrganizerItemManager::NoError);
}

QOrganizerCollectionLocalId QOrganizerItemMemoryEngine::defaultCollectionId(QOrganizerItemManager::Error* error) const
{
    // default collection has id of 1.
    *error = QOrganizerItemManager::NoError;
    return QOrganizerCollectionLocalId(new QOrganizerCollectionMemoryEngineLocalId(1));
}

QList<QOrganizerCollectionLocalId> QOrganizerItemMemoryEngine::collectionIds(QOrganizerItemManager::Error* error) const
{
    *error = QOrganizerItemManager::NoError;
    return d->m_organizerCollectionIds;
}

QList<QOrganizerCollection> QOrganizerItemMemoryEngine::collections(const QList<QOrganizerCollectionLocalId>& collectionIds, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error) const
{
    *error = QOrganizerItemManager::NoError;
    QList<QOrganizerCollection> retn;
    for (int i = 0; i < collectionIds.size(); ++i) {
        QOrganizerCollectionLocalId currentInput = collectionIds.at(i);
        bool foundThisId = false;
        for (int j = 0; j < d->m_organizerCollectionIds.size(); ++j) {
            QOrganizerCollectionLocalId currentComparison = d->m_organizerCollectionIds.at(j);
            if (currentInput == currentComparison) {
                // they want this collection to be returned.
                retn.append(d->m_organizerCollections.at(j));
                foundThisId = true;
                break;
            }
        }

        // check to see if we found this particular id in the given list.
        if (!foundThisId) {
            if (errorMap) {
                errorMap->insert(i, QOrganizerItemManager::DoesNotExistError);
                *error = QOrganizerItemManager::DoesNotExistError;
            }
        }
    }

    // return the results.
    return retn;
}

QOrganizerCollection QOrganizerItemMemoryEngine::compatibleCollection(const QOrganizerCollection& original, QOrganizerItemManager::Error* error) const
{
    *error = QOrganizerItemManager::NoError;

    // we don't allow people to change the default collection.
    QOrganizerCollectionLocalId defaultCollectionLocalId = QOrganizerCollectionLocalId(new QOrganizerCollectionMemoryEngineLocalId(1));
    if (original.localId() == defaultCollectionLocalId) {
        for (int i = 0; i < d->m_organizerCollections.size(); ++i) {
            QOrganizerCollection current = d->m_organizerCollections.at(i);
            if (current.localId() == defaultCollectionLocalId) {
                return current;
            }
        }
    }

    // if it isn't the default id, it's fine, since anything can be saved in the memory engine.
    return original;
}

bool QOrganizerItemMemoryEngine::saveCollection(QOrganizerCollection* collection, QOrganizerItemManager::Error* error)
{
    QOrganizerCollectionChangeSet cs; // for signal emission.

    *error = QOrganizerItemManager::NoError;
    QOrganizerCollectionLocalId colId = collection->localId();
    if (colId == QOrganizerCollectionLocalId(new QOrganizerCollectionMemoryEngineLocalId(1))) {
        // attempting to update the default collection.  this is not allowed in the memory engine.
        *error = QOrganizerItemManager::PermissionsError;
        return false;
    }

    // if it's not the default collection, they can do whatever they like.  A collection does not need any metadata to be valid.
    for (int i = 0; i < d->m_organizerCollectionIds.size(); ++i) {
        if (d->m_organizerCollectionIds.at(i) == colId) {
            // this collection already exists.  update our internal list.
            d->m_organizerCollections.replace(i, *collection);
            cs.insertChangedCollection(colId);
            cs.emitSignals(this);
            return true;
        }
    }

    // this must be a new collection.  check that the id is null.
    if ((collection->id().managerUri() != managerUri() && !collection->id().managerUri().isEmpty()) || !colId.isNull()) {
        // nope, this collection belongs in another manager, or has been deleted.
        *error = QOrganizerItemManager::DoesNotExistError;
        return false;
    }

    // this is a new collection with a null id; create a new id, add it to our list.
    QOrganizerCollectionId newId;
    QOrganizerCollectionLocalId newLocalId = QOrganizerCollectionLocalId(new QOrganizerCollectionMemoryEngineLocalId(d->m_nextOrganizerCollectionId++));
    newId.setLocalId(newLocalId);
    newId.setManagerUri(managerUri());
    collection->setId(newId);
    d->m_organizerCollections.append(*collection);
    d->m_organizerCollectionIds.append(newLocalId);
    cs.insertAddedCollection(newLocalId);
    cs.emitSignals(this);
    return true;
}

bool QOrganizerItemMemoryEngine::removeCollection(const QOrganizerCollectionLocalId& collectionId, QOrganizerItemManager::Error* error)
{
    QOrganizerCollectionChangeSet cs; // for signal emission.
    *error = QOrganizerItemManager::NoError;
    if (collectionId == QOrganizerCollectionLocalId(new QOrganizerCollectionMemoryEngineLocalId(1))) {
        // attempting to remove the default collection.  this is not allowed in the memory engine.
        *error = QOrganizerItemManager::PermissionsError;
        return false;
    }

    // try to find the collection to remove it (and the items it contains)
    QList<QOrganizerItemLocalId> itemsToRemove = d->m_itemsInCollections.values(collectionId);
    for (int i = 0; i < d->m_organizerCollectionIds.size(); ++i) {
        if (d->m_organizerCollectionIds.at(i) == collectionId) {
            // found the collection to remove.  remove the items in the collection.
            if (!itemsToRemove.isEmpty()) {
                if (!removeItems(itemsToRemove, 0, error)) {
                    // without transaction support, we can't back out.  but the operation should fail.
                    return false;
                }
            }

            // now remove the collection from our lists.
            d->m_organizerCollectionIds.removeAt(i);
            d->m_organizerCollections.removeAt(i);
            d->m_itemsInCollections.remove(collectionId);
            cs.insertRemovedCollection(collectionId);
            cs.emitSignals(this);
            return true;
        }
    }

    // the collection doesn't exist...
    *error = QOrganizerItemManager::DoesNotExistError;
    return false;
}

/*! \reimp */
QMap<QString, QOrganizerItemDetailDefinition> QOrganizerItemMemoryEngine::detailDefinitions(const QString& organizeritemType, QOrganizerItemManager::Error* error) const
{
    // lazy initialisation of schema definitions.
    if (d->m_definitions.isEmpty()) {
        d->m_definitions = QOrganizerItemManagerEngine::schemaDefinitions();
    }

    *error = QOrganizerItemManager::NoError;
    return d->m_definitions.value(organizeritemType);
}

/*! Saves the given detail definition \a def, storing any error to \a error and
    filling the \a changeSet with ids of changed organizer items as required */
bool QOrganizerItemMemoryEngine::saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& organizeritemType, QOrganizerItemChangeSet& changeSet, QOrganizerItemManager::Error* error)
{
    // we should check for changes to the database in this function, and add ids of changed data to changeSet. // XXX TODO.
    Q_UNUSED(changeSet);

    if (!validateDefinition(def, error)) {
        return false;
    }

    detailDefinitions(organizeritemType, error); // just to populate the definitions if we haven't already.
    QMap<QString, QOrganizerItemDetailDefinition> defsForThisType = d->m_definitions.value(organizeritemType);
    defsForThisType.insert(def.name(), def);
    d->m_definitions.insert(organizeritemType, defsForThisType);

    *error = QOrganizerItemManager::NoError;
    return true;
}

/*! \reimp */
bool QOrganizerItemMemoryEngine::saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& organizeritemType, QOrganizerItemManager::Error* error)
{
    QOrganizerItemChangeSet changeSet;
    bool retn = saveDetailDefinition(def, organizeritemType, changeSet, error);
    d->emitSharedSignals(&changeSet);
    return retn;
}

/*! Removes the detail definition identified by \a definitionId, storing any error to \a error and
    filling the \a changeSet with ids of changed organizer items as required */
bool QOrganizerItemMemoryEngine::removeDetailDefinition(const QString& definitionId, const QString& organizeritemType, QOrganizerItemChangeSet& changeSet, QOrganizerItemManager::Error* error)
{
    // we should check for changes to the database in this function, and add ids of changed data to changeSet...
    // we should also check to see if the changes have invalidated any organizer item data, and add the ids of those organizer items
    // to the change set.  TODO!
    Q_UNUSED(changeSet);

    if (definitionId.isEmpty()) {
        *error = QOrganizerItemManager::BadArgumentError;
        return false;
    }

    detailDefinitions(organizeritemType, error); // just to populate the definitions if we haven't already.
    QMap<QString, QOrganizerItemDetailDefinition> defsForThisType = d->m_definitions.value(organizeritemType);
    bool success = defsForThisType.remove(definitionId);
    d->m_definitions.insert(organizeritemType, defsForThisType);
    if (success)
        *error = QOrganizerItemManager::NoError;
    else
        *error = QOrganizerItemManager::DoesNotExistError;
    return success;
}

/*! \reimp */
bool QOrganizerItemMemoryEngine::removeDetailDefinition(const QString& definitionId, const QString& organizeritemType, QOrganizerItemManager::Error* error)
{
    QOrganizerItemChangeSet changeSet;
    bool retn = removeDetailDefinition(definitionId, organizeritemType, changeSet, error);
    d->emitSharedSignals(&changeSet);
    return retn;
}

/*! \reimp */
void QOrganizerItemMemoryEngine::requestDestroyed(QOrganizerItemAbstractRequest* req)
{
    Q_UNUSED(req);
}

/*! \reimp */
bool QOrganizerItemMemoryEngine::startRequest(QOrganizerItemAbstractRequest* req)
{
    if (!req)
        return false;
    updateRequestState(req, QOrganizerItemAbstractRequest::ActiveState);
    performAsynchronousOperation(req);
    return true;
}

/*! \reimp */
bool QOrganizerItemMemoryEngine::cancelRequest(QOrganizerItemAbstractRequest* req)
{
    Q_UNUSED(req); // we can't cancel since we complete immediately
    return false;
}

/*! \reimp */
bool QOrganizerItemMemoryEngine::waitForRequestFinished(QOrganizerItemAbstractRequest* req, int msecs)
{
    // in our implementation, we always complete any operation we start.
    Q_UNUSED(msecs);
    Q_UNUSED(req);

    return true;
}

/*!
 * This slot is called some time after an asynchronous request is started.
 * It performs the required operation, sets the result and returns.
 */
void QOrganizerItemMemoryEngine::performAsynchronousOperation(QOrganizerItemAbstractRequest *currentRequest)
{
    // store up changes, and emit signals once at the end of the (possibly batch) operation.
    QOrganizerItemChangeSet changeSet;

    // Now perform the active request and emit required signals.
    Q_ASSERT(currentRequest->state() == QOrganizerItemAbstractRequest::ActiveState);
    switch (currentRequest->type()) {
        case QOrganizerItemAbstractRequest::ItemFetchRequest:
        {
            QOrganizerItemFetchRequest* r = static_cast<QOrganizerItemFetchRequest*>(currentRequest);
            QOrganizerItemFilter filter = r->filter();
            QList<QOrganizerItemSortOrder> sorting = r->sorting();
            QOrganizerItemFetchHint fetchHint = r->fetchHint();

            QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
            QList<QOrganizerItem> requestedOrganizerItems = items(filter, sorting, fetchHint, &operationError);

            // update the request with the results.
            if (!requestedOrganizerItems.isEmpty() || operationError != QOrganizerItemManager::NoError)
                updateItemFetchRequest(r, requestedOrganizerItems, operationError, QOrganizerItemAbstractRequest::FinishedState);
            else
                updateRequestState(currentRequest, QOrganizerItemAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerItemAbstractRequest::ItemLocalIdFetchRequest:
        {
            QOrganizerItemLocalIdFetchRequest* r = static_cast<QOrganizerItemLocalIdFetchRequest*>(currentRequest);
            QOrganizerItemFilter filter = r->filter();
            QList<QOrganizerItemSortOrder> sorting = r->sorting();

            QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
            QList<QOrganizerItemLocalId> requestedOrganizerItemIds = itemIds(filter, sorting, &operationError);

            if (!requestedOrganizerItemIds.isEmpty() || operationError != QOrganizerItemManager::NoError)
                updateItemLocalIdFetchRequest(r, requestedOrganizerItemIds, operationError, QOrganizerItemAbstractRequest::FinishedState);
            else
                updateRequestState(currentRequest, QOrganizerItemAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerItemAbstractRequest::ItemSaveRequest:
        {
            QOrganizerItemSaveRequest* r = static_cast<QOrganizerItemSaveRequest*>(currentRequest);
            QList<QOrganizerItem> organizeritems = r->items();

            QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
            QMap<int, QOrganizerItemManager::Error> errorMap;
            saveItems(&organizeritems, r->collectionId(), &errorMap, &operationError);

            updateItemSaveRequest(r, organizeritems, operationError, errorMap, QOrganizerItemAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerItemAbstractRequest::ItemRemoveRequest:
        {
            QOrganizerItemRemoveRequest* r = static_cast<QOrganizerItemRemoveRequest*>(currentRequest);
            QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
            QList<QOrganizerItemLocalId> organizeritemsToRemove = r->itemIds();
            QMap<int, QOrganizerItemManager::Error> errorMap;

            for (int i = 0; i < organizeritemsToRemove.size(); i++) {
                QOrganizerItemManager::Error tempError = QOrganizerItemManager::NoError;
                removeItem(organizeritemsToRemove.at(i), changeSet, &tempError);

                if (tempError != QOrganizerItemManager::NoError) {
                    errorMap.insert(i, tempError);
                    operationError = tempError;
                }
            }

            if (!errorMap.isEmpty() || operationError != QOrganizerItemManager::NoError)
                updateItemRemoveRequest(r, operationError, errorMap, QOrganizerItemAbstractRequest::FinishedState);
            else
                updateRequestState(currentRequest, QOrganizerItemAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerItemAbstractRequest::CollectionFetchRequest:
        {
            QOrganizerCollectionFetchRequest* r = static_cast<QOrganizerCollectionFetchRequest*>(currentRequest);
            QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
            QMap<int, QOrganizerItemManager::Error> errorMap;
            QList<QOrganizerCollection> requestedOrganizerCollections = collections(r->collectionIds(), &errorMap, &operationError);

            // update the request with the results.
            updateCollectionFetchRequest(r, requestedOrganizerCollections, operationError, errorMap, QOrganizerItemAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerItemAbstractRequest::CollectionLocalIdFetchRequest:
        {
            QOrganizerCollectionLocalIdFetchRequest* r = static_cast<QOrganizerCollectionLocalIdFetchRequest*>(currentRequest);
            QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
            QList<QOrganizerCollectionLocalId> requestedOrganizerCollectionIds = collectionIds(&operationError);

            if (!requestedOrganizerCollectionIds.isEmpty() || operationError != QOrganizerItemManager::NoError)
                updateCollectionLocalIdFetchRequest(r, requestedOrganizerCollectionIds, operationError, QOrganizerItemAbstractRequest::FinishedState);
            else
                updateRequestState(currentRequest, QOrganizerItemAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerItemAbstractRequest::CollectionSaveRequest:
        {
            QOrganizerCollectionSaveRequest* r = static_cast<QOrganizerCollectionSaveRequest*>(currentRequest);
            QList<QOrganizerCollection> collections = r->collections();
            QList<QOrganizerCollection> retn;

            QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
            QMap<int, QOrganizerItemManager::Error> errorMap;
            for (int i = 0; i < collections.size(); ++i) {
                QOrganizerItemManager::Error tempError = QOrganizerItemManager::NoError;
                QOrganizerCollection curr = collections.at(i);
                if (!saveCollection(&curr, &tempError)) {
                    errorMap.insert(i, tempError);
                    operationError = tempError;
                }
                retn.append(curr);
            }

            updateCollectionSaveRequest(r, retn, operationError, errorMap, QOrganizerItemAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerItemAbstractRequest::CollectionRemoveRequest:
        {
            // removes the collections identified in the list of ids.
            QOrganizerCollectionRemoveRequest* r = static_cast<QOrganizerCollectionRemoveRequest*>(currentRequest);
            QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
            QList<QOrganizerCollectionLocalId> collectionsToRemove = r->collectionIds();
            QMap<int, QOrganizerItemManager::Error> errorMap;

            for (int i = 0; i < collectionsToRemove.size(); i++) {
                QOrganizerItemManager::Error tempError = QOrganizerItemManager::NoError;
                removeCollection(collectionsToRemove.at(i), &tempError);

                if (tempError != QOrganizerItemManager::NoError) {
                    errorMap.insert(i, tempError);
                    operationError = tempError;
                }
            }

            if (!errorMap.isEmpty() || operationError != QOrganizerItemManager::NoError)
                updateCollectionRemoveRequest(r, operationError, errorMap, QOrganizerItemAbstractRequest::FinishedState);
            else
                updateRequestState(currentRequest, QOrganizerItemAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerItemAbstractRequest::DetailDefinitionFetchRequest:
        {
            QOrganizerItemDetailDefinitionFetchRequest* r = static_cast<QOrganizerItemDetailDefinitionFetchRequest*>(currentRequest);
            QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
            QMap<int, QOrganizerItemManager::Error> errorMap;
            QMap<QString, QOrganizerItemDetailDefinition> requestedDefinitions;
            QStringList names = r->definitionNames();
            if (names.isEmpty())
                names = detailDefinitions(r->itemType(), &operationError).keys(); // all definitions.

            QOrganizerItemManager::Error tempError = QOrganizerItemManager::NoError;
            for (int i = 0; i < names.size(); i++) {
                QOrganizerItemDetailDefinition current = detailDefinition(names.at(i), r->itemType(), &tempError);
                requestedDefinitions.insert(names.at(i), current);

                if (tempError != QOrganizerItemManager::NoError) {
                    errorMap.insert(i, tempError);
                    operationError = tempError;
                }
            }

            if (!errorMap.isEmpty() || !requestedDefinitions.isEmpty() || operationError != QOrganizerItemManager::NoError)
                updateDefinitionFetchRequest(r, requestedDefinitions, operationError, errorMap, QOrganizerItemAbstractRequest::FinishedState);
            else
                updateRequestState(currentRequest, QOrganizerItemAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerItemAbstractRequest::DetailDefinitionSaveRequest:
        {
            QOrganizerItemDetailDefinitionSaveRequest* r = static_cast<QOrganizerItemDetailDefinitionSaveRequest*>(currentRequest);
            QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
            QMap<int, QOrganizerItemManager::Error> errorMap;
            QList<QOrganizerItemDetailDefinition> definitions = r->definitions();
            QList<QOrganizerItemDetailDefinition> savedDefinitions;

            QOrganizerItemManager::Error tempError = QOrganizerItemManager::NoError;
            for (int i = 0; i < definitions.size(); i++) {
                QOrganizerItemDetailDefinition current = definitions.at(i);
                saveDetailDefinition(current, r->itemType(), changeSet, &tempError);
                savedDefinitions.append(current);

                if (tempError != QOrganizerItemManager::NoError) {
                    errorMap.insert(i, tempError);
                    operationError = tempError;
                }
            }

            // update the request with the results.
            updateDefinitionSaveRequest(r, savedDefinitions, operationError, errorMap, QOrganizerItemAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerItemAbstractRequest::DetailDefinitionRemoveRequest:
        {
            QOrganizerItemDetailDefinitionRemoveRequest* r = static_cast<QOrganizerItemDetailDefinitionRemoveRequest*>(currentRequest);
            QStringList names = r->definitionNames();

            QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
            QMap<int, QOrganizerItemManager::Error> errorMap;

            for (int i = 0; i < names.size(); i++) {
                QOrganizerItemManager::Error tempError = QOrganizerItemManager::NoError;
                removeDetailDefinition(names.at(i), r->itemType(), changeSet, &tempError);

                if (tempError != QOrganizerItemManager::NoError) {
                    errorMap.insert(i, tempError);
                    operationError = tempError;
                }
            }

            // there are no results, so just update the status with the error.
            updateDefinitionRemoveRequest(r, operationError, errorMap, QOrganizerItemAbstractRequest::FinishedState);
        }
        break;

        default: // unknown request type.
        break;
    }

    // now emit any signals we have to emit
    d->emitSharedSignals(&changeSet);
}

/*!
 * \reimp
 */
bool QOrganizerItemMemoryEngine::hasFeature(QOrganizerItemManager::ManagerFeature feature, const QString& organizeritemType) const
{
    if (!supportedItemTypes().contains(organizeritemType))
        return false;

    switch (feature) {
        case QOrganizerItemManager::MutableDefinitions:
            return true;
        case QOrganizerItemManager::Anonymous:
            return d->m_anonymous;

        default:
            return false;
    }
}

/*!
 * \reimp
 */
QList<int> QOrganizerItemMemoryEngine::supportedDataTypes() const
{
    QList<int> st;
    st.append(QVariant::String);
    st.append(QVariant::Date);
    st.append(QVariant::DateTime);
    st.append(QVariant::Time);
    st.append(QVariant::Bool);
    st.append(QVariant::Char);
    st.append(QVariant::Int);
    st.append(QVariant::UInt);
    st.append(QVariant::LongLong);
    st.append(QVariant::ULongLong);
    st.append(QVariant::Double);

    return st;
}

/*!
 * The function returns true if the backend natively supports the given filter \a filter, otherwise false.
 */
bool QOrganizerItemMemoryEngine::isFilterSupported(const QOrganizerItemFilter& filter) const
{
    Q_UNUSED(filter);

    // Until we add hashes for common stuff, fall back to slow code
    return false;
}

#include "moc_qorganizeritemmemorybackend_p.cpp"

QTM_END_NAMESPACE

