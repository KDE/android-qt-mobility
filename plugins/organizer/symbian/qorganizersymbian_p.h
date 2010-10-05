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

#ifndef QORGANIZERSYMBIAN_P_H
#define QORGANIZERSYMBIAN_P_H


//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QSharedData>
#include <QMap>
#include <QMultiMap>
#include <QList>
#include <QQueue>
#include <QPair>
#include <QSet>
#include <QDateTime>
#include <QString>
#include <QObject>

#include "qorganizeritem.h"
#include "qorganizeritemenginelocalid.h"
#include "qorganizercollectionenginelocalid.h"
#include "qorganizeritemmanager.h"
#include "qorganizeritemmanagerengine.h"
#include "qorganizeritemmanagerenginefactory.h"
#include "qorganizeritemchangeset.h"

#include "organizeritemtransform.h"
#include "organizersymbiancollection.h"

#include <calprogresscallback.h> // MCalProgressCallBack
#include <calchangecallback.h>
#ifdef SYMBIAN_CALENDAR_V2
#include <calfilechangenotification.h>
#endif


QTM_USE_NAMESPACE

// This is the result of qHash(QString(QLatin1String("symbian")))
// Precalculated value is used for performance reasons.
const uint KSymbianEngineLocalIdType = 0xb038f9e;

class OrganizerItemTransform; // forward declare transform class.
class QOrganizerItemSymbianEngine; // forward declare symbian engine.
class QOrganizerCollectionSymbianEngineLocalId : public QOrganizerCollectionEngineLocalId
{
public:
    QOrganizerCollectionSymbianEngineLocalId();
    QOrganizerCollectionSymbianEngineLocalId(quint64 collectionId);
    ~QOrganizerCollectionSymbianEngineLocalId();
    QOrganizerCollectionSymbianEngineLocalId(const QOrganizerCollectionSymbianEngineLocalId& other);

    bool isEqualTo(const QOrganizerCollectionEngineLocalId* other) const;
    bool isLessThan(const QOrganizerCollectionEngineLocalId* other) const;

    uint engineLocalIdType() const;
    QOrganizerCollectionEngineLocalId* clone() const;

#ifndef QT_NO_DEBUG_STREAM
    QDebug debugStreamOut(QDebug dbg);
#endif
#ifndef QT_NO_DATASTREAM
    QDataStream& dataStreamOut(QDataStream& out);
    QDataStream& dataStreamIn(QDataStream& in);
#endif
    uint hash() const;

private:
    quint64 m_localCollectionId;
    friend class QOrganizerItemSymbianEngine;
};

class QOrganizerItemSymbianEngineLocalId : public QOrganizerItemEngineLocalId
{
public:
    QOrganizerItemSymbianEngineLocalId();
    QOrganizerItemSymbianEngineLocalId(quint64 collectionId, quint32 itemId);
    ~QOrganizerItemSymbianEngineLocalId();
    QOrganizerItemSymbianEngineLocalId(const QOrganizerItemSymbianEngineLocalId& other);

    bool isEqualTo(const QOrganizerItemEngineLocalId* other) const;
    bool isLessThan(const QOrganizerItemEngineLocalId* other) const;

    uint engineLocalIdType() const;
    QOrganizerItemEngineLocalId* clone() const;

#ifndef QT_NO_DEBUG_STREAM
    QDebug debugStreamOut(QDebug dbg);
#endif
#ifndef QT_NO_DATASTREAM
    QDataStream& dataStreamOut(QDataStream& out);
    QDataStream& dataStreamIn(QDataStream& in);
#endif
    uint hash() const;
    
public:
    quint32 calLocalUid() { return m_localItemId; }
    quint64 calCollectionId() { return m_localCollectionId; }
    
private:
    quint64 m_localCollectionId;
    quint32 m_localItemId; // the symbian backend requires quint32 for itemId + quint64 for collectionId
    friend class QOrganizerItemSymbianEngine;
    friend class OrganizerItemTransform;
};




class QOrganizerItemSymbianFactory : public QObject, 
public QOrganizerItemManagerEngineFactory
{
  Q_OBJECT
  Q_INTERFACES(QtMobility::QOrganizerItemManagerEngineFactory)
  public:
    QOrganizerItemManagerEngine* engine(
        const QMap<QString, QString>& parameters, 
        QOrganizerItemManager::Error*);
    QString managerName() const;
    QOrganizerItemEngineLocalId* createItemEngineLocalId() const;
    QOrganizerCollectionEngineLocalId* createCollectionEngineLocalId() const;
};

class QOrganizerItemSymbianEngineData : public QSharedData
{
public:
    QOrganizerItemSymbianEngineData()
        : QSharedData()
    {
    }

    QOrganizerItemSymbianEngineData(
        const QOrganizerItemSymbianEngineData& other) : QSharedData(other)
    {
    }

    ~QOrganizerItemSymbianEngineData()
    {
    }
};

class CCalSession;
class CCalEntryView;
class CCalInstanceView;
class CActiveSchedulerWait;
class QOrganizerItemRequestQueue;
#ifdef SYMBIAN_CALENDAR_V2
class CCalCalendarInfo;
#endif

class QOrganizerItemSymbianEngine : public QOrganizerItemManagerEngine

#ifdef SYMBIAN_CALENDAR_V2
                                    ,public MCalFileChangeObserver
#endif
{
    Q_OBJECT

public:
    static QOrganizerItemSymbianEngine *createSkeletonEngine(
        const QMap<QString, QString>& parameters);

    QOrganizerItemSymbianEngine();
    ~QOrganizerItemSymbianEngine();

    /* URI reporting */
    QString managerName() const;
    QMap<QString, QString> managerParameters() const;
    int managerVersion() const;

    QList<QOrganizerItem> itemInstances(const QOrganizerItem& generator, 
        const QDateTime& periodStart, const QDateTime& periodEnd, 
        int maxCount, QOrganizerItemManager::Error* error) const;
    QList<QOrganizerItem> itemInstances(const QOrganizerItemFilter& filter, 
        const QList<QOrganizerItemSortOrder>& sortOrders, 
        const QOrganizerItemFetchHint& fetchHint, 
        QOrganizerItemManager::Error* error) const;

    QList<QOrganizerItemLocalId> itemIds(const QOrganizerItemFilter& filter, 
        const QList<QOrganizerItemSortOrder>& sortOrders, 
        QOrganizerItemManager::Error* error) const;
    QList<QOrganizerItemLocalId> getIdsModifiedSinceDateL(
        const QOrganizerItemFilter& filter) const;
    void itemIdsL(
        QList<QOrganizerItemLocalId>& ids, 
        const QOrganizerItemFilter& filter, 
        const QList<QOrganizerItemSortOrder>& sortOrders) const;
    QList<QOrganizerItem> items(const QOrganizerItemFilter& filter, 
        const QList<QOrganizerItemSortOrder>& sortOrders, 
        const QOrganizerItemFetchHint& fetchHint, 
        QOrganizerItemManager::Error* error) const;
    void itemsL(QList<QOrganizerItem>& itemsList, 
        const QOrganizerItemFilter& filter, 
        const QList<QOrganizerItemSortOrder>& sortOrders, 
        const QOrganizerItemFetchHint& fetchHint) const;
    QOrganizerItem item(const QOrganizerItemLocalId& itemId, 
        const QOrganizerItemFetchHint& fetchHint, 
        QOrganizerItemManager::Error* error) const;

    bool saveItem(QOrganizerItem* item, 
        const QOrganizerCollectionLocalId& collectionId, 
        QOrganizerItemManager::Error* error);
    bool saveItems(QList<QOrganizerItem> *items, 
        const QOrganizerCollectionLocalId& collectionId, 
        QMap<int, QOrganizerItemManager::Error> *errorMap, 
        QOrganizerItemManager::Error *error);

    bool removeItem(const QOrganizerItemLocalId& organizeritemId, 
        QOrganizerItemManager::Error* error);
    bool removeItems(const QList<QOrganizerItemLocalId> &itemIds, 
        QMap<int, QOrganizerItemManager::Error> *errorMap, 
        QOrganizerItemManager::Error *error);
    
    /* Collections - every item belongs to exactly one collection */
    QOrganizerCollectionLocalId defaultCollectionId(
        QOrganizerItemManager::Error* error) const;
    QList<QOrganizerCollectionLocalId> collectionIds(
        QOrganizerItemManager::Error* error) const;
    QList<QOrganizerCollection> collections(
        const QList<QOrganizerCollectionLocalId>& collectionIds, 
        QMap<int, QOrganizerItemManager::Error>* errorMap,
        QOrganizerItemManager::Error* error) const;
    bool saveCollection(QOrganizerCollection* collection, 
        QOrganizerItemManager::Error* error);
    bool removeCollection(const QOrganizerCollectionLocalId& collectionId, 
        QOrganizerItemManager::Error* error);

    /* Definitions - Accessors and Mutators */
    QMap<QString, QOrganizerItemDetailDefinition> detailDefinitions(
        const QString& itemType, QOrganizerItemManager::Error* error) const;

    /* Capabilities reporting */
    bool hasFeature(QOrganizerItemManager::ManagerFeature feature, 
        const QString& itemType) const;
    bool isFilterSupported(const QOrganizerItemFilter& filter) const;
    QList<int> supportedDataTypes() const;
    QStringList supportedItemTypes() const;

    /* Asynchronous Request Support */
    void requestDestroyed(QOrganizerItemAbstractRequest* req);
    bool startRequest(QOrganizerItemAbstractRequest* req);
    bool cancelRequest(QOrganizerItemAbstractRequest* req);
    bool waitForRequestFinished(QOrganizerItemAbstractRequest* req, int msecs);

public: // MCalProgressCallBack
    void Progress(TInt aPercentageCompleted);
    void Completed(TInt aError);
    TBool NotifyProgress();

#ifdef SYMBIAN_CALENDAR_V2
public: // MCalFileChangeObserver
    void CalendarInfoChangeNotificationL(
        RPointerArray<CCalFileChangeInfo>& aCalendarInfoChangeEntries);
#endif
    
public:
    void initializeL();
    /* Util functions */
    static bool transformError(TInt symbianError, 
        QOrganizerItemManager::Error* qtError);
    void itemInstancesL(
        QList<QOrganizerItem> &itemInstances,
        const QOrganizerItem &generator,
        const QDateTime &periodStart,
        const QDateTime &periodEnd,
        int maxCount) const;
    QList<QOrganizerItem> itemInstancesL(
        QList<QOrganizerItem> &itemInstances,
        const QOrganizerItemFilter &filter,
        const QList<QOrganizerItemSortOrder> &sortOrders,
        const QOrganizerItemFetchHint &fetchHint) const;
    void toItemInstancesL(
        const RPointerArray<CCalInstance> &calInstanceList,
        QOrganizerItem generator,
        const int maxCount,
        QOrganizerCollectionLocalId collectionLocalId,
        QList<QOrganizerItem> &itemInstances) const;
    QOrganizerItem itemL(const QOrganizerItemLocalId& itemId,
            const QOrganizerItemFetchHint& fetchHint) const;
    void saveItemL(QOrganizerItem *item,
        const QOrganizerCollectionLocalId& collectionId,
        QOrganizerItemChangeSet *changeSet);
    void removeItemL(const QOrganizerItemLocalId& organizeritemId);
    QList<QOrganizerItem> slowFilter(const QList<QOrganizerItem> &items, 
        const QOrganizerItemFilter& filter, 
        const QList<QOrganizerItemSortOrder>& sortOrders) const;

private:
    QList<QOrganizerCollection> collectionsL(
        const QList<QOrganizerCollectionLocalId> &collectionIds) const;
    void saveCollectionL(QOrganizerCollection* collection);
    void removeCollectionL(const QOrganizerCollectionLocalId& collectionId);
    CCalEntryView* entryViewL(const QOrganizerCollectionLocalId& collectionId) const;
    CCalInstanceView* instanceViewL(const QOrganizerCollectionLocalId& collectionId) const;
    QOrganizerCollectionLocalId collectionLocalIdL(const QOrganizerItem &item, 
        const QOrganizerCollectionLocalId &collectionId = QOrganizerCollectionLocalId()) const;
    CCalEntry* entryForItemOccurrenceL(
        const QOrganizerCollectionLocalId &collectionId, const QOrganizerItem &item, 
        bool &isNewEntry) const;
    CCalEntry* entryForItemL(const QOrganizerCollectionLocalId &collectionId, 
        const QOrganizerItem &item, bool &isNewEntry) const;
    CCalEntry* findEntryL(const QOrganizerCollectionLocalId &collectionId, 
        const QOrganizerItemLocalId &localId, QString manageruri) const;
    CCalEntry* findEntryL(const QOrganizerCollectionLocalId &collectionId, 
        const TDesC8& globalUid) const;
    CCalEntry* findParentEntryLC(const QOrganizerCollectionLocalId &collectionId,
        const QOrganizerItem &item, const TDesC8& globalUid) const;
	
private:
    QOrganizerItemSymbianEngineData *d;
    
    OrganizerSymbianCollection m_defaultCollection;
    QMap<QOrganizerCollectionLocalId, OrganizerSymbianCollection> m_collections;
    QOrganizerItemRequestQueue* m_requestServiceProviderQueue;
    OrganizerItemTransform m_itemTransform;
    mutable QMap<QString, QMap<QString, QOrganizerItemDetailDefinition> > m_definition;

    friend class QOrganizerItemSymbianFactory;
};


#endif

