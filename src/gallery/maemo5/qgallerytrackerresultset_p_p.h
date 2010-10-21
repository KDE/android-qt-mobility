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

#ifndef QGALLERYTRACKERITEMLIST_P_P_H
#define QGALLERYTRACKERITEMLIST_P_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//


#include "qgallerytrackerresultset_p.h"
#include "qgalleryresultset_p.h"

#include "qgallerytrackerlistcolumn_p.h"
#include "qgallerytrackermetadataedit_p.h"
#include "qgallerytrackerschema_p.h"

#include <QtCore/qcoreapplication.h>
#include <QtCore/qbasictimer.h>
#include <QtCore/qcoreevent.h>
#include <QtCore/qfuturewatcher.h>
#include <QtCore/qqueue.h>
#include <QtCore/qwaitcondition.h>

QTM_BEGIN_NAMESPACE

class QGalleryTrackerResultSetPrivate : public QGalleryResultSetPrivate
{
    Q_DECLARE_PUBLIC(QGalleryTrackerResultSet)
public:
    struct SyncEvent
    {
        enum Type
        {
            Update,
            Replace,
            Finish
        };

        const Type type;
        const int rIndex;
        const int rCount;
        const int iIndex;
        const int iCount;

        static SyncEvent *updateEvent(int aIndex, int iIndex, int count) {
            return new SyncEvent(Update, aIndex, count, iIndex, count); }

        static SyncEvent *replaceEvent(int aIndex, int aCount, int iIndex, int iCount) {
            return new SyncEvent(Replace, aIndex, aCount, iIndex, iCount); }

        static SyncEvent *finishEvent(int aIndex, int iIndex) {
            return new SyncEvent(Finish, aIndex, 0, iIndex, 0); }

    private:
        SyncEvent(Type type, int rIndex, int rCount, int iIndex, int iCount)
            : type(type), rIndex(rIndex), rCount(rCount), iIndex(iIndex), iCount(iCount) {}

    };

    class SyncEventQueue
    {
    public:
        SyncEventQueue() {}
        ~SyncEventQueue() { qDeleteAll(m_queue); }

        bool enqueue(SyncEvent *event)
        {
            QMutexLocker locker(&m_mutex);

            m_queue.enqueue(event);
            m_wait.wakeOne();

            return m_queue.count() == 1;
        }

        SyncEvent *dequeue()
        {
            QMutexLocker locker(&m_mutex);

            return !m_queue.isEmpty() ? m_queue.dequeue() : 0;
        }

        bool waitForEvent(int msecs)
        {
            QMutexLocker locker(&m_mutex);

            if (!m_queue.isEmpty())
                return true;

            return m_wait.wait(&m_mutex, msecs);
        }

    private:
        QQueue<SyncEvent *> m_queue;
        QMutex m_mutex;
        QWaitCondition m_wait;
    };

    struct Row
    {
        Row() {}
        Row(QVector<QVariant>::iterator begin, QVector<QVariant>::iterator end)
            : begin(begin), end(end) {}

        QVector<QVariant>::iterator begin;
        QVector<QVariant>::iterator end;
    };

    struct row_iterator
    {
        row_iterator() {}
        row_iterator(QVector<QVariant>::iterator begin, int width) : begin(begin), width(width) {}

        bool operator != (const row_iterator &other) const { return begin != other.begin; }
        bool operator <(const row_iterator &other) const { return begin < other.begin; }

        row_iterator &operator ++() { begin += width; return *this; }
        row_iterator &operator --() { begin -= width; return *this; }

        row_iterator operator ++(int) { row_iterator n(*this); begin += width; return n; }
        row_iterator operator --(int) { row_iterator n(*this); begin -= width; return n; }

        int operator -(const row_iterator &other) const { return (begin - other.begin) / width; }
        int operator -(const QVector<QVariant>::const_iterator &iterator) const {
            return (begin - iterator) / width; }

        row_iterator operator +(int span) const {
            return row_iterator(begin + (span * width), width); }

        row_iterator &operator +=(int span) { begin += span * width; return *this; }

        Row &operator *() {  return row = Row(begin, begin + width); }
        const Row &operator *() const {  return row = Row(begin, begin + width); }

        bool isEqual(const row_iterator &other, int count) const {
            return qEqual(begin, begin + count, other.begin); }
        bool isEqual(const row_iterator &other, int index, int count) {
            return qEqual(begin + index, begin + count, other.begin + index); }

        const QVariant &operator[] (int column) const { return *(begin + column); }

        QVector<QVariant>::iterator begin;
        int width;
        mutable Row row;
    };

    struct const_row_iterator
    {
        const_row_iterator() {}
        const_row_iterator(QVector<QVariant>::const_iterator begin, int width)
            : begin(begin), width(width) {}

        bool operator != (const const_row_iterator &other) const { return begin != other.begin; }
        bool operator <(const const_row_iterator &other) const { return begin < other.begin; }

        const_row_iterator &operator ++() { begin += width; return *this; }
        const_row_iterator operator --(int) {
            const_row_iterator n(*this); begin -= width; return n; }

        int operator -(const const_row_iterator &other) const {
            return (begin - other.begin) / width; }
        int operator -(const QVector<QVariant>::const_iterator &iterator) const {
            return (begin - iterator) / width; }

        const_row_iterator operator +(int span) const {
            return const_row_iterator(begin + (span * width), width); }

        const_row_iterator &operator +=(int span) { begin += span * width; return *this; }

        bool isEqual(const const_row_iterator &other, int count) const {
            return qEqual(begin, begin + count, other.begin); }
        bool isEqual(const const_row_iterator &other, int index, int count) {
            return qEqual(begin + index, begin + count, other.begin + index); }

        QVector<QVariant>::const_iterator begin;
        int width;
    };

    struct Cache
    {
        Cache() : count(0), cutoff(0) {}

        int count;
        union
        {
            int offset;
            int cutoff;
        };
        QVector<QVariant> values;
    };

    typedef QVector<QGalleryTrackerSortCriteria>::const_iterator sort_iterator;

    enum Flag
    {
        Canceled        = 0x01,
        Live            = 0x02,
        Refresh         = 0x04,
        Reset           = 0x08,
        UpdateRequested = 0x10,
        Active          = 0x20,
        SyncFinished    = 0x40
    };

    Q_DECLARE_FLAGS(Flags, Flag)

    QGalleryTrackerResultSetPrivate(
            QGalleryTrackerResultSetArguments *arguments,
            bool autoUpdate,
            int offset,
            int limit)
        : idColumn(arguments->idColumn.take())
        , urlColumn(arguments->urlColumn.take())
        , typeColumn(arguments->typeColumn.take())
        , updateMask(arguments->updateMask)
        , identityWidth(arguments->identityWidth)
        , tableWidth(arguments->tableWidth)
        , valueOffset(arguments->valueOffset)
        , compositeOffset(arguments->compositeOffset)
        , aliasOffset(compositeOffset + arguments->compositeColumns.count())
        , columnCount(aliasOffset + arguments->aliasColumns.count())
        , queryOffset(offset)
        , queryLimit(limit)
        , currentRow(0)
        , currentIndex(-1)
        , rowCount(0)
        , progressMaximum(0)
        , queryInterface(arguments->queryInterface)
        , queryMethod(arguments->queryMethod)
        , queryArguments(arguments->queryArguments)
        , propertyNames(arguments->propertyNames)
        , propertyAttributes(arguments->propertyAttributes)
        , propertyTypes(arguments->propertyTypes)
        , valueColumns(arguments->valueColumns)
        , compositeColumns(arguments->compositeColumns)
        , aliasColumns(arguments->aliasColumns)
        , sortCriteria(arguments->sortCriteria)
        , resourceKeys(arguments->resourceKeys)
    {
        arguments->clear();

        if (autoUpdate)
            flags |= Live;
    }

    ~QGalleryTrackerResultSetPrivate()
    {
        qDeleteAll(valueColumns);
        qDeleteAll(compositeColumns);
    }

    Flags flags;
    const QScopedPointer<QGalleryTrackerCompositeColumn> idColumn;
    const QScopedPointer<QGalleryTrackerCompositeColumn> urlColumn;
    const QScopedPointer<QGalleryTrackerCompositeColumn> typeColumn;

    const int updateMask;
    const int identityWidth;
    const int tableWidth;
    const int valueOffset;
    const int compositeOffset;
    const int aliasOffset;
    const int columnCount;
    const int queryOffset;
    const int queryLimit;
    QVector<QVariant>::const_iterator currentRow;
    int currentIndex;
    int rowCount;
    int progressMaximum;
    const QGalleryDBusInterfacePointer queryInterface;
    const QString queryMethod;
    const QVariantList queryArguments;
    const QStringList propertyNames;
    const QList<int> propertyKeys;
    const QVector<QGalleryProperty::Attributes> propertyAttributes;
    const QVector<QVariant::Type> propertyTypes;
    const QVector<QGalleryTrackerValueColumn *> valueColumns;
    const QVector<QGalleryTrackerCompositeColumn *> compositeColumns;
    const QVector<int> aliasColumns;
    const QVector<QGalleryTrackerSortCriteria> sortCriteria;
    const QVector<int> resourceKeys;
    Cache rCache;   // Remove cache.
    Cache iCache;   // Insert cache.

    QScopedPointer<QDBusPendingCallWatcher> queryWatcher;
    QFutureWatcher<bool> parseWatcher;
    QList<QGalleryTrackerMetaDataEdit *> edits;
    QBasicTimer updateTimer;
    SyncEventQueue syncEvents;

    inline int rCacheIndex(const const_row_iterator &iterator) const {
        return iterator - rCache.values.begin(); }
    inline int iCacheIndex(const const_row_iterator &iterator) const {
        return iterator - iCache.values.begin(); }
    
    void update();
    void requestUpdate()
    {
        if (!(flags & UpdateRequested)) {
            flags |= UpdateRequested;
            QCoreApplication::postEvent(q_func(), new QEvent(QEvent::UpdateRequest));
        }
    }

    void query();

    void queryFinished(const QDBusPendingCall &call);
    bool parseRows(const QDBusPendingCall &call, int limit, bool reset);
    void correctRows(
            row_iterator begin,
            row_iterator end,
            const sort_iterator sortBegin,
            const sort_iterator sortEnd,
            bool reversed = false) const;

    void synchronize();

    void postSyncEvent(SyncEvent *event)
    {
        if (syncEvents.enqueue(event))
            QCoreApplication::postEvent(q_func(), new QEvent(QEvent::UpdateLater));
    }

    void processSyncEvents();
    void removeItems(const int rIndex, const int iIndex, const int count);
    void insertItems(const int rIndex, const int iIndex, const int count);
    void syncUpdate(const int aIndex, const int aCount, const int iIndex, const int iCount);
    void syncReplace(const int aIndex, const int aCount, const int iIndex, const int iCount);
    void syncFinish(const int aIndex, const int iIndex);
    bool waitForSyncFinish(int msecs);

    void _q_queryFinished(QDBusPendingCallWatcher *watcher);
    void _q_parseFinished();
    void _q_editFinished(QGalleryTrackerMetaDataEdit *edit);
};

QTM_END_NAMESPACE

template <> inline void qSwap<QTM_PREPEND_NAMESPACE(QGalleryTrackerResultSetPrivate::Row)>(
        QTM_PREPEND_NAMESPACE(QGalleryTrackerResultSetPrivate::Row) &row1,
        QTM_PREPEND_NAMESPACE(QGalleryTrackerResultSetPrivate::Row) &row2)
{
    typedef QVector<QVariant>::iterator iterator;

    for (iterator it1 = row1.begin, it2 = row2.begin; it1 != row1.end; ++it1, ++it2)
        qSwap(*it1, *it2);
}

#endif

Q_DECLARE_OPERATORS_FOR_FLAGS(QTM_PREPEND_NAMESPACE(QGalleryTrackerResultSetPrivate::Flags))

