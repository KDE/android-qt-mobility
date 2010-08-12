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
#include <qnamespace.h>
#include <qtorganizer.h>
#include <QtTest/QtTest>
#include <QDebug>

QTM_USE_NAMESPACE

// Constants
const QString managerNameSymbian("symbian");
const int KNumberOfEntries = 5; 

// We need to be able to pass QOrganizerItem as parameter from
// test data functions
Q_DECLARE_METATYPE(QOrganizerItem)

class TestFetchItems : public QObject
{
   Q_OBJECT
   
private slots:
   void init();
   void cleanup();
   
private slots:
   void saveItems(); // Start the test case by saving items into Agenda DB
   // fetchItems would be called as and when the results for saveItems are
   // available
   
public slots:
   void fetchRequestStateChanged(
           QOrganizerItemAbstractRequest::State currentState);
   void fetchRequestResultsAvailable();
   void saveRequestStateChanged(
           QOrganizerItemAbstractRequest::State currentState);
   void saveRequestResultsAvailable();

   
private:
   QList<QOrganizerItem> createItems(int noOfItems);
   void fetchItems();
   
private:
    QOrganizerItemManager*              m_om;
    QOrganizerItemFetchRequest*         m_fetchItemRequest;
    QOrganizerItemSaveRequest*          m_saveItemRequest;
};

void TestFetchItems::init()
{
    // Create a new item manager instance
    m_om = new QOrganizerItemManager(managerNameSymbian);
    // Cleanup by deleting all items
    m_om->removeItems(m_om->itemIds(), 0);
    
    // Create an item save request before fetching an item
    // Create asynchronous request to save an item
    m_saveItemRequest = new QOrganizerItemSaveRequest(this);
    // Connect for the state change signal 
    connect(m_saveItemRequest, 
            SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)), this, 
            SLOT(saveRequestStateChanged(QOrganizerItemAbstractRequest::State)));
    connect(m_saveItemRequest, SIGNAL(resultsAvailable()), 
            this, SLOT(saveRequestResultsAvailable()));

    // Create an item fetch request
    m_fetchItemRequest = new QOrganizerItemFetchRequest(this);
    // Connect for the state change signal 
    connect(m_fetchItemRequest, 
            SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)), 
            this, 
            SLOT(fetchRequestStateChanged(QOrganizerItemAbstractRequest::State)));
    connect(m_fetchItemRequest, SIGNAL(resultsAvailable()), 
            this, SLOT(fetchRequestResultsAvailable()));
}

void TestFetchItems::cleanup()
{
    if (m_om) {
        delete m_om;
        m_om = 0;
    }
}

void TestFetchItems::saveItems()
{
    // Set items to be saved
    m_saveItemRequest->setItems(createItems(KNumberOfEntries));
    // Set item manager
    m_saveItemRequest->setManager(m_om);
    // Start save request
    m_saveItemRequest->start();
    // Wait for the request to be completed
    m_saveItemRequest->waitForFinished(1000);
}
   
// Returns a list of noOfItems
QList<QOrganizerItem> TestFetchItems::createItems(int noOfItems)
{
    QList<QOrganizerItem> itemsList;
    
    for (int index(0); index < noOfItems; index++) {
        // Create a new organizer item
        QOrganizerItem organizerItem;
        // Set the organizer item type
        organizerItem.setType(QOrganizerItemType::TypeEvent);
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

// request status changed for save request
void TestFetchItems::saveRequestStateChanged(
        QOrganizerItemAbstractRequest::State currentState)
{
    switch(currentState) {
        case QOrganizerItemAbstractRequest::InactiveState: { 
            // Operation not yet started start the operation
            saveItems();
            break;
        }
        case QOrganizerItemAbstractRequest::ActiveState: { 
            // Operation started, not yet finished operation already started
        break;
        }
        case QOrganizerItemAbstractRequest::CanceledState: { 
            // Operation is finished due to cancellation test not completed, 
            // failed
        break;
        }
        case QOrganizerItemAbstractRequest::FinishedState: { 
            // Operation either completed successfully or failed.  
            // No further results will be available.
            // test completed, compare the results when results available 
            break;
        }
        default: {
            // Not handled
        }
    }
}

// Save request completed/results available
void TestFetchItems::saveRequestResultsAvailable()
{
    QList<QOrganizerItem> items = m_saveItemRequest->items();
    // Compate the number of items saved
    QCOMPARE(KNumberOfEntries, items.count());
    
    // Start fetchItems test now
    fetchItems();
}


void TestFetchItems::fetchItems()
{
    QOrganizerItemLocalIdFilter localIdFilter;
    QList<QOrganizerItemLocalId> lUids;
    lUids.append(3);
    lUids.append(4);
    localIdFilter.setIds(lUids);
    //m_fetchItemRequest->setFilter(localIdFilter);
    // Set ItemDetailsFilter
    QOrganizerItemDetailFilter detailsFilter;
    detailsFilter.setDetailDefinitionName(
            QOrganizerItemDisplayLabel::DefinitionName, 
            QOrganizerItemDisplayLabel::FieldLabel); 
    detailsFilter.setValue("myDescription");
    detailsFilter.setMatchFlags(QOrganizerItemFilter::MatchContains);
    m_fetchItemRequest->setFilter(detailsFilter);

    // Set sorting order
    QList<QOrganizerItemSortOrder> sortOrderlist;
    QOrganizerItemSortOrder sorting;
    sorting.setDetailDefinitionName(
            QOrganizerItemDisplayLabel::DefinitionName, 
            QOrganizerItemDisplayLabel::FieldLabel);
    sorting.setBlankPolicy(QOrganizerItemSortOrder::BlanksLast);
    //sorting.setDirection(SortOrder::AscendingOrder);
    //sorting.setCaseSensitivity(CaseSensitivity::CaseInsensitive);
    sortOrderlist.append(sorting);
    m_fetchItemRequest->setSorting(sortOrderlist);

    // Set fetch hint
    QOrganizerItemFetchHint fetchHint;
    //fetchHint.setDetailDefinitionsHint();
    fetchHint.setOptimizationHints(QOrganizerItemFetchHint::AllRequired);
    m_fetchItemRequest->setFetchHint(fetchHint);
    
    // Set manager
    m_fetchItemRequest->setManager(m_om);
    // Start the request
    m_fetchItemRequest->start();
}

// Fetch request state changed
void TestFetchItems::fetchRequestStateChanged(
        QOrganizerItemAbstractRequest::State currentState)
{
    switch(currentState) {
        case QOrganizerItemAbstractRequest::InactiveState: { 
            // Operation not yet started start the operation
            fetchItems();
            break;
        }
        case QOrganizerItemAbstractRequest::ActiveState: { 
            // Operation started, not yet finished operation already started
        break;
        }
        case QOrganizerItemAbstractRequest::CanceledState: { 
            // Operation is finished due to cancellation test not completed, 
            // failed
        break;
        }
        case QOrganizerItemAbstractRequest::FinishedState: { 
            // Operation either completed successfully or failed.  
            // No further results will be available.
            // test completed, compate the results while available
        }
        default: {
            // Not handled
        }
    }
}

// Fetch request available compare results
void TestFetchItems::fetchRequestResultsAvailable()
{
    QList<QOrganizerItem> items = m_fetchItemRequest->items();
    int count(items.count());
    QCOMPARE(KNumberOfEntries, items.count());
}

QTEST_MAIN(TestFetchItems);

#include "tst_symbianasynchfetchitems.moc"
