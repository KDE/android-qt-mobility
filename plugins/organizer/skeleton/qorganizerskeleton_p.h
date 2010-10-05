/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QORGANIZERSKELETON_P_H
#define QORGANIZERSKELETON_P_H

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
#include "qorganizeritemmanager.h"
#include "qorganizeritemmanagerengine.h"
#include "qorganizeritemmanagerenginefactory.h"
#include "qorganizeritemdetaildefinition.h"
#include "qorganizeritemabstractrequest.h"
#include "qorganizeritemchangeset.h"
#include "qorganizeritemenginelocalid.h"
#include "qorganizercollectionenginelocalid.h"

QTM_USE_NAMESPACE

class QOrganizerItemSkeletonFactory : public QObject, public QOrganizerItemManagerEngineFactory
{
  Q_OBJECT
  Q_INTERFACES(QtMobility::QOrganizerItemManagerEngineFactory)
  public:
    QOrganizerItemManagerEngine* engine(const QMap<QString, QString>& parameters, QOrganizerItemManager::Error*);
    QOrganizerItemEngineLocalId* createItemEngineLocalId() const;
    QOrganizerCollectionEngineLocalId* createCollectionEngineLocalId() const;
    QString managerName() const;
};

class QOrganizerCollectionSkeletonEngineLocalId : public QOrganizerCollectionEngineLocalId
{
public:
    QOrganizerCollectionSkeletonEngineLocalId();
    QOrganizerCollectionSkeletonEngineLocalId(quint32 collectionId);
    ~QOrganizerCollectionSkeletonEngineLocalId();
    QOrganizerCollectionSkeletonEngineLocalId(const QOrganizerCollectionSkeletonEngineLocalId& other);

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

    // data members:
    // Your backend can use whatever it likes as an id internally.
    // In this example, we use just a single quint32, but you can
    // use any datatype you need to (filename string, etc).
    quint32 m_localCollectionId;
};

class QOrganizerItemSkeletonEngineLocalId : public QOrganizerItemEngineLocalId
{
public:
    QOrganizerItemSkeletonEngineLocalId();
    QOrganizerItemSkeletonEngineLocalId(quint32 itemId);
    ~QOrganizerItemSkeletonEngineLocalId();
    QOrganizerItemSkeletonEngineLocalId(const QOrganizerItemSkeletonEngineLocalId& other);

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

    // data members:
    // Your backend can use whatever it likes as an id internally.
    // In this example, we use just a single quint32, but you can
    // use a pair of ints (one for collectionId, one for itemId)
    // or any other information (uuid string, etc).
    quint32 m_localItemId;
};

class QOrganizerItemSkeletonEngineData : public QSharedData
{
public:
    QOrganizerItemSkeletonEngineData()
        : QSharedData()
    {
    }

    QOrganizerItemSkeletonEngineData(const QOrganizerItemSkeletonEngineData& other)
        : QSharedData(other)
    {
    }

    ~QOrganizerItemSkeletonEngineData()
    {
    }
};

class QOrganizerItemSkeletonEngine : public QOrganizerItemManagerEngine
{
    Q_OBJECT

public:
    static QOrganizerItemSkeletonEngine *createSkeletonEngine(const QMap<QString, QString>& parameters);

    ~QOrganizerItemSkeletonEngine();

    /* URI reporting */
    QString managerName() const;
    QMap<QString, QString> managerParameters() const;
    int managerVersion() const;

    QList<QOrganizerItem> itemInstances(const QOrganizerItem& generator, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount, QOrganizerItemManager::Error* error) const;
    QList<QOrganizerItemLocalId> itemIds(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, QOrganizerItemManager::Error* error) const;
    QList<QOrganizerItem> items(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const;
    QOrganizerItem item(const QOrganizerItemLocalId& itemId, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const;

    bool saveItems(QList<QOrganizerItem>* items, const QOrganizerCollectionLocalId& collectionId, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error);
    bool removeItems(const QList<QOrganizerItemLocalId>& itemIds, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error);

    /* Definitions - Accessors and Mutators */
    QMap<QString, QOrganizerItemDetailDefinition> detailDefinitions(const QString& itemType, QOrganizerItemManager::Error* error) const;
    QOrganizerItemDetailDefinition detailDefinition(const QString& definitionId, const QString& itemType, QOrganizerItemManager::Error* error) const;
    bool saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& itemType, QOrganizerItemManager::Error* error);
    bool removeDetailDefinition(const QString& definitionId, const QString& itemType, QOrganizerItemManager::Error* error);

    /* Collections - every item belongs to exactly one collection */
    QOrganizerCollectionLocalId defaultCollectionId(QOrganizerItemManager::Error* error) const;
    QList<QOrganizerCollectionLocalId> collectionIds(QOrganizerItemManager::Error* error) const;
    QList<QOrganizerCollection> collections(const QList<QOrganizerCollectionLocalId>& collectionIds, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error) const;
    bool saveCollection(QOrganizerCollection* collection, QOrganizerItemManager::Error* error);
    bool removeCollection(const QOrganizerCollectionLocalId& collectionId, QOrganizerItemManager::Error* error);

    /* Capabilities reporting */
    bool hasFeature(QOrganizerItemManager::ManagerFeature feature, const QString& itemType) const;
    bool isFilterSupported(const QOrganizerItemFilter& filter) const;
    QList<int> supportedDataTypes() const;
    QStringList supportedItemTypes() const;

    /* Asynchronous Request Support */
    void requestDestroyed(QOrganizerItemAbstractRequest* req);
    bool startRequest(QOrganizerItemAbstractRequest* req);
    bool cancelRequest(QOrganizerItemAbstractRequest* req);
    bool waitForRequestFinished(QOrganizerItemAbstractRequest* req, int msecs);

private:
    QOrganizerItemSkeletonEngineData* d;

    friend class QOrganizerItemSkeletonFactory;
};

#endif

