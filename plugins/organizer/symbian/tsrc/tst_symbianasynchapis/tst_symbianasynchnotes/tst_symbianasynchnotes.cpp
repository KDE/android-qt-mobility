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

#include <QObject>
#include <qmobilityglobal.h>
#include <qtorganizer.h>
#include <QtTest/QtTest>
#include <QDebug>

QTM_USE_NAMESPACE

// Constants
const QString managerNameSymbian("symbian");
const int KNumberOfItems = 2;
const int KTimeToWait = KTimeToWait;

// We need to be able to pass QOrganizerItem as parameter from
// test data functions
Q_DECLARE_METATYPE(QOrganizerItem)

class TestNoteItems : public QObject
{
   Q_OBJECT
   
private slots:
   void initTestCase();
   void cleanupTestCase();
   
private slots:
   void saveItem();
   void fetchItem();
   void removeItem();
   void saveDetails();
   void removeDetails();
   void fetchDetails();
   
public slots:
   void requestStateChanged(QOrganizerItemAbstractRequest::State currentState);
   void requestResultsAvailable();
   
private:
   QList<QOrganizerItem> createItems(int noOfItems);
   //void removeItem();
private:
    QOrganizerItemManager*              m_om;
    QOrganizerItemAbstractRequest*      m_itemRequest;
    QList<QOrganizerItemLocalId>        m_itemIds;
    QStringList                         m_definitionNames;
};

void TestNoteItems::initTestCase()
{
    // Create a new item manager instance
    m_om = new QOrganizerItemManager(managerNameSymbian);
        
    // Cleanup by deleting all items
    m_om->removeItems(m_om->itemIds(), 0);
    
    // initialize the m_itemRequest to zero
    m_itemRequest = 0;
}

void TestNoteItems::cleanupTestCase()
{

    if (m_om) {
        delete m_om;
        m_om = 0;
    }
}
   
void TestNoteItems::saveItem()
{
    // Make sure to delete the old request, if any
    delete m_itemRequest;
    // Create new request
    m_itemRequest = new QOrganizerItemSaveRequest(this);
    // Set manager
    m_itemRequest->setManager(m_om);
    
    // Connect for the state change signal 
    connect(m_itemRequest, 
            SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)), 
            this, 
            SLOT(requestStateChanged(QOrganizerItemAbstractRequest::State)));
    connect(m_itemRequest, SIGNAL(resultsAvailable()), 
            this, SLOT(requestResultsAvailable()));

    // Type cast the new request to appropriate type
    QOrganizerItemSaveRequest * itemSaveRequest(
        (QOrganizerItemSaveRequest*)m_itemRequest);
    // Set Items
    itemSaveRequest->setItems(createItems(KNumberOfItems));
    // Start the request
    itemSaveRequest->start();
    // Wait for KTimeToWait millisecs or until request is finished
    itemSaveRequest->waitForFinished(KTimeToWait);
}

QList<QOrganizerItem> TestNoteItems::createItems(int noOfItems)
{
    QList<QOrganizerItem> itemsList;

    for (int index(0); index < noOfItems; index++) {
        // Create a new organizer item
        QOrganizerItem organizerItem;
        // Set the organizer item type
        organizerItem.setType(QOrganizerItemType::TypeNote);
        // Create description string 
        QString description("myDescription");
        // Set organizer item description
        organizerItem.setDescription(description);
        // Create desplay label
        QString desplaylabel("myDescription");
        // Set display label
        organizerItem.setDisplayLabel(desplaylabel);

        // Set current time
        QOrganizerEventTimeRange timeRange;
        QDateTime startTime;
        startTime.currentDateTime();
        timeRange.setStartDateTime(startTime.currentDateTime());

        QVERIFY(organizerItem.saveDetail(&timeRange));

        itemsList.append(organizerItem);
    }
    return itemsList;
}

void TestNoteItems::fetchItem()
{
    // Make sure to delete the old request, if any
    delete m_itemRequest;
    // Create new request
    m_itemRequest = new QOrganizerItemFetchRequest(this);
    // Set manager
    m_itemRequest->setManager(m_om);

    // Connect for the state change signal 
    connect(m_itemRequest, 
        SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)), 
        this, 
        SLOT(requestStateChanged(QOrganizerItemAbstractRequest::State)));
    connect(m_itemRequest, SIGNAL(resultsAvailable()), 
        this, SLOT(requestResultsAvailable()));

    // Type cast the new request to appropriate type
    QOrganizerItemFetchRequest * itemFetchRequest(
        (QOrganizerItemFetchRequest*)m_itemRequest);

    QOrganizerItemLocalIdFilter localIdFilter;
    localIdFilter.setIds(m_itemIds);
    //itemFetchRequest->setFilter(localIdFilter);
        
    // Set ItemDetailsFilter
    QOrganizerItemDetailFilter detailsFilter;
    detailsFilter.setDetailDefinitionName(
            QOrganizerItemType::DefinitionName, 
            QOrganizerItemType::FieldType); 
    detailsFilter.setValue("Note");
    detailsFilter.setMatchFlags(QOrganizerItemFilter::MatchContains);
    itemFetchRequest->setFilter(detailsFilter);

    // Set sorting order
    QList<QOrganizerItemSortOrder> sortOrderlist;
    QOrganizerItemSortOrder sorting;
    sorting.setDetailDefinitionName(
        QOrganizerItemType::DefinitionName, 
        QOrganizerItemType::FieldType);
    sorting.setBlankPolicy(QOrganizerItemSortOrder::BlanksLast);
    sorting.setDirection(Qt::AscendingOrder);
    sorting.setCaseSensitivity(Qt::CaseInsensitive);
    sortOrderlist.append(sorting);
    itemFetchRequest->setSorting(sortOrderlist);

    // Start the request
    itemFetchRequest->start();
    // Wait for KTimeToWait millisecs or until request is finished
    itemFetchRequest->waitForFinished(KTimeToWait);
}

void TestNoteItems::removeItem()
{
    // Make sure to delete the old request, if any
    delete m_itemRequest;
    // Create new request
    m_itemRequest = new QOrganizerItemRemoveRequest(this);
    // Set manager
    m_itemRequest->setManager(m_om);

    // Connect for the state change signal 
    connect(m_itemRequest, 
            SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)), 
            this, 
            SLOT(requestStateChanged(QOrganizerItemAbstractRequest::State)));
    
    connect(m_itemRequest, SIGNAL(resultsAvailable()), 
            this, SLOT(requestResultsAvailable()));

    // Type cast the new request to appropriate type
    QOrganizerItemRemoveRequest * itemRemoveRequest(
            (QOrganizerItemRemoveRequest*)m_itemRequest);
    // Set items Ids to be deleted
    itemRemoveRequest->setItemIds(m_itemIds);
    // Start the request
    itemRemoveRequest->start();
    // Wait for KTimeToWait millisecs or until request is finished
    itemRemoveRequest->waitForFinished(KTimeToWait);
}

void TestNoteItems::saveDetails()
{
    // Make sure to delete the old request, if any
    delete m_itemRequest;
    // Create new request
    m_itemRequest = new QOrganizerItemDetailDefinitionSaveRequest(this);
    // Set manager
    m_itemRequest->setManager(m_om);

    // Connect for the state change signal 
    connect(m_itemRequest, 
        SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)), 
        this, 
        SLOT(requestStateChanged(QOrganizerItemAbstractRequest::State)));
    connect(m_itemRequest, SIGNAL(resultsAvailable()), 
        this, SLOT(requestResultsAvailable()));

    // Type cast the new request to appropriate type
    QOrganizerItemDetailDefinitionSaveRequest * itemDetailDefSaveRequest(
        (QOrganizerItemDetailDefinitionSaveRequest*)m_itemRequest);

    QList<QOrganizerItemDetailDefinition> detailDefinitions;
    // Create a dummy detail definition to append to the list for
    // populating test data, as symbian doesn't support 
    QOrganizerItemDetailDefinition definition;
    for (int index(0); index < KNumberOfItems; index++) {
        detailDefinitions.append(definition);
    }
    itemDetailDefSaveRequest->setDefinitions(detailDefinitions);
    itemDetailDefSaveRequest->setItemType(QOrganizerItemType::TypeNote);
    // Start save details request
    QVERIFY(itemDetailDefSaveRequest->start());
    // Wait for KTimeToWait millisecs or until request is finished
    itemDetailDefSaveRequest->waitForFinished(KTimeToWait);
}

void TestNoteItems::removeDetails()
{
    // Make sure to delete the old request, if any
    delete m_itemRequest;
    // Create new request
    m_itemRequest = new QOrganizerItemDetailDefinitionRemoveRequest(this);
    // Set manager
    m_itemRequest->setManager(m_om);

    // Connect for the state change signal 
    connect(m_itemRequest, 
        SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)), 
        this, 
        SLOT(requestStateChanged(QOrganizerItemAbstractRequest::State)));
    connect(m_itemRequest, SIGNAL(resultsAvailable()), 
        this, SLOT(requestResultsAvailable()));

    // Type cast the new request to appropriate type
    QOrganizerItemDetailDefinitionRemoveRequest * itemDetailDefRemoveRequest(
        (QOrganizerItemDetailDefinitionRemoveRequest*)m_itemRequest);

    QStringList stringList;
    QString string((QLatin1String)QOrganizerItemTimestamp::DefinitionName);
    for (int index(0); index < KNumberOfItems; index++) {
        stringList.append(string);
    }
    itemDetailDefRemoveRequest->setDefinitionNames(stringList);
    itemDetailDefRemoveRequest->setItemType(QOrganizerItemType::TypeNote);
    itemDetailDefRemoveRequest->start();
    itemDetailDefRemoveRequest->waitForFinished(KTimeToWait);
}

void TestNoteItems::fetchDetails()
{
    // Make sure to delete the old request, if any
    delete m_itemRequest;
    // Create new request
    m_itemRequest = new QOrganizerItemDetailDefinitionFetchRequest(this);
    // Set manager
    m_itemRequest->setManager(m_om);

    // Connect for the state change signal 
    connect(m_itemRequest, 
        SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)), 
        this, 
        SLOT(requestStateChanged(QOrganizerItemAbstractRequest::State)));
    connect(m_itemRequest, SIGNAL(resultsAvailable()), 
        this, SLOT(requestResultsAvailable()));

    // Type cast the new request to appropriate type
    QOrganizerItemDetailDefinitionFetchRequest * itemDetailDefFetchRequest(
        (QOrganizerItemDetailDefinitionFetchRequest*)m_itemRequest);

    m_definitionNames.append(QOrganizerItemType::DefinitionName);
    m_definitionNames.append(QOrganizerItemGuid::DefinitionName);
    m_definitionNames.append(QOrganizerItemTimestamp::DefinitionName);
    m_definitionNames.append(QOrganizerItemDisplayLabel::DefinitionName);
    m_definitionNames.append(QOrganizerItemDescription::DefinitionName);
    // Comment - Not supported
    m_definitionNames.append(QOrganizerItemComment::DefinitionName);
    m_definitionNames.append(QOrganizerItemRecurrence::DefinitionName);
    m_definitionNames.append(QOrganizerEventTimeRange::DefinitionName);
    m_definitionNames.append(QOrganizerItemPriority::DefinitionName);
    m_definitionNames.append(QOrganizerItemLocation::DefinitionName);

    // Reminders
    m_definitionNames.append(QOrganizerItemVisualReminder::DefinitionName);
    m_definitionNames.append(QOrganizerItemReminder::DefinitionName);
    m_definitionNames.append(QOrganizerItemEmailReminder::DefinitionName);
    m_definitionNames.append(QOrganizerItemAudibleReminder::DefinitionName);

    // Instance origin - Not supported
    m_definitionNames.append(QOrganizerItemInstanceOrigin::DefinitionName);
    // To-do progress - Not supported
    m_definitionNames.append(QOrganizerTodoProgress::DefinitionName);
    // To-do time range - Not supported
    m_definitionNames.append(QOrganizerTodoTimeRange::DefinitionName);
    // Journal time range - Not supported
    m_definitionNames.append(QOrganizerJournalTimeRange::DefinitionName);
 
    itemDetailDefFetchRequest->setDefinitionNames(m_definitionNames);
    itemDetailDefFetchRequest->setItemType(QOrganizerItemType::TypeNote);
    itemDetailDefFetchRequest->start();
    // Wait for the request to be completed
    itemDetailDefFetchRequest->waitForFinished(KTimeToWait);
}

void TestNoteItems::requestStateChanged(QOrganizerItemAbstractRequest::State currentState)
{
    switch(currentState) {
    case QOrganizerItemAbstractRequest::InactiveState: {
        // Verify if the request is in inactive state
        QVERIFY(m_itemRequest->isInactive());
        // Compare the request state is set rightly
        QCOMPARE(m_itemRequest->state(), 
            QOrganizerItemAbstractRequest::InactiveState);
        // Operation not yet started start the operation
        m_itemRequest->start();
    }
    break;
    case QOrganizerItemAbstractRequest::ActiveState: {
        // Verify if the request is in active state
        QVERIFY(m_itemRequest->isActive());
        // Operation started, not yet finished operation already started
        // Compare the request state is set rightly
        QCOMPARE(m_itemRequest->state(), 
            QOrganizerItemAbstractRequest::ActiveState);
    }
    break;
    case QOrganizerItemAbstractRequest::CanceledState: {
        // Verify if the request is in canceled state
        QVERIFY(m_itemRequest->isCanceled());
        // Operation is finished due to cancellation test not completed, 
        // failed Compare the request state is set rightly
        QCOMPARE(m_itemRequest->state(), 
            QOrganizerItemAbstractRequest::CanceledState);
    }
    break;
    case QOrganizerItemAbstractRequest::FinishedState: {
        // Verify if the request is in finished state
        QVERIFY(m_itemRequest->isFinished());
        // Operation either completed successfully or failed.  
        // No further results will be available.
        // test completed, compare the results
        // Compare the request state is set rightly
        QCOMPARE(m_itemRequest->state(), 
            QOrganizerItemAbstractRequest::FinishedState);
    }
    break;
    default: {
        // Not handled
    }
    break;
    }
}

void TestNoteItems::requestResultsAvailable()
{
    QOrganizerItemAbstractRequest::RequestType reqType(m_itemRequest->type());
    switch (reqType) {
    case QOrganizerItemAbstractRequest::ItemSaveRequest : {
        QList<QOrganizerItem> items = ((QOrganizerItemSaveRequest*)
            m_itemRequest)->items();
        int count(items.count());
        QCOMPARE(KNumberOfItems, count);

        // Clear m_itemIds to fetch all the item Ids
        m_itemIds.clear();
        for (int index(0); index < count; index++) {
            QVERIFY(items[index].type() == QOrganizerItemType::TypeNote);
            // Get the local Id
            QOrganizerItemLocalId itemId(items.at(index).localId());
            // Append local Id of item
            m_itemIds.append(itemId);
        }
    }
    break;
    case QOrganizerItemAbstractRequest::ItemFetchRequest : {
        QList<QOrganizerItem> items = ((QOrganizerItemFetchRequest*)
            (m_itemRequest))->items();
        int count(items.count());
        QCOMPARE(KNumberOfItems, items.count());

        // Clear m_itemIds to fetch all the item Ids
        m_itemIds.clear();
        for (int index(0); index < count; index++) {
            QVERIFY(items[index].type() == QOrganizerItemType::TypeNote);
            // Get the local Id
            QOrganizerItemLocalId itemId(items.at(index).localId());
            // Append local Id of item
            m_itemIds.append(itemId);
        }

    }
    break;
    case QOrganizerItemAbstractRequest::ItemRemoveRequest : {
        // Check error map
        QMap<int, QOrganizerItemManager::Error> erroMap;
        erroMap = ((QOrganizerItemRemoveRequest*)
            m_itemRequest)->errorMap();
        // Error map should contain zero errors to indicate successful deletion
        // of all the items
        QCOMPARE(0, erroMap.count());
    }
    break;
    case QOrganizerItemAbstractRequest::DetailDefinitionSaveRequest : {
        // No definitions should be saved, so definitions() should return an 
        // empty list
        QList<QOrganizerItemDetailDefinition> savedDefinitions(
            ((QOrganizerItemDetailDefinitionSaveRequest*)
            (m_itemRequest))->definitions());
        int count(savedDefinitions.count());
        QCOMPARE(0, count);
        // Check the error map, error map should contain the number of errors
        // equal to number of definitions to be saved. Also all the erros 
        // returned should be NotSupported
        QMap<int, QOrganizerItemManager::Error> errorMap(
            ((QOrganizerItemDetailDefinitionSaveRequest*)
            (m_itemRequest))->errorMap());
        count = errorMap.count();
        QCOMPARE(KNumberOfItems, count);
        // Compare the errors returned
        for (int index(0); index < count; index++){
            QCOMPARE(QOrganizerItemManager::NotSupportedError, errorMap[index]);
        }
    }
    break;
    case QOrganizerItemAbstractRequest::DetailDefinitionRemoveRequest : {
        // Check the error map, error map should contain the number of errors
        // equal to number of definitions to be saved. Also all the errors 
        // returned should be NotSupported
        QMap<int, QOrganizerItemManager::Error> errorMap(
            ((QOrganizerItemDetailDefinitionRemoveRequest*)
            (m_itemRequest))->errorMap());
        int count = errorMap.count();
        QCOMPARE(KNumberOfItems, count);
        // Compare the errors returned, as remove detail defs is not supported
        for (int index(0); index < count; index++){
            QCOMPARE(QOrganizerItemManager::NotSupportedError, errorMap[index]);
        }
    }
    break;
    case QOrganizerItemAbstractRequest::DetailDefinitionFetchRequest : {

        // Get the error map
        QMap<int, QOrganizerItemManager::Error> errorMap(
            ((QOrganizerItemDetailDefinitionFetchRequest*)
            (m_itemRequest))->errorMap());
        // Get the iterator to the error map
        QMapIterator<int, QOrganizerItemManager::Error> iter(errorMap);
        // Compare the errors returned
        while (iter.hasNext()){
            // Advance the iterator to point to next item in the error map
            iter.next();
            // Compare the error value
            QCOMPARE(QOrganizerItemManager::DoesNotExistError, iter.value());
            // Get the index for the error returned
            int keyIndex(iter.key());
            
            qWarning() << m_definitionNames[keyIndex] << "is not supported";
        }
        
        // Get the definitions map
        QMap<QString, QOrganizerItemDetailDefinition> definitionsmap( 
            ((QOrganizerItemDetailDefinitionFetchRequest*)
            (m_itemRequest))->definitions());
        // Get the number of definitions supported
        int count(definitionsmap.count());
        // Get the expected count as total definitions minus error count
        int expectedItems(m_definitionNames.count() - errorMap.count());
        // Compare the expected and actual definitions supported
        QCOMPARE(expectedItems, count);
    }
    break;
    }
}

QTEST_MAIN(TestNoteItems);

#include "tst_symbianasynchnotes.moc"
