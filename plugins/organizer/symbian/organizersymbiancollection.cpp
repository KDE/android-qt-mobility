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
#include "organizersymbiancollection.h"

#include <QDebug>
#include <calsession.h>
#include <calentryview.h>
#include <calinstanceview.h>
#include "organizersymbianutils.h"
#include "qorganizeritemchangeset.h"
#include "qorganizeritemmanagerengine.h"

using namespace OrganizerSymbianUtils;

OrganizerSymbianCollectionPrivate::OrganizerSymbianCollectionPrivate()
    :QSharedData(), 
    m_engine(0),
    m_calSession(0),
    m_calEntryView(0),
    m_calInstanceView(0),
    m_calCollectionId(0),
    m_error(0)
{
    m_activeSchedulerWait = new CActiveSchedulerWait();
}

OrganizerSymbianCollectionPrivate::~OrganizerSymbianCollectionPrivate()
{
    if (m_calSession)
        m_calSession->StopChangeNotification();
    delete m_calEntryView;
    delete m_calInstanceView;
    delete m_calSession;
    delete m_activeSchedulerWait;
}    

/*!
 * From MCalProgressCallBack
 */
void OrganizerSymbianCollectionPrivate::Progress(TInt /*aPercentageCompleted*/)
{
}

/*!
 * From MCalProgressCallBack
 */
void OrganizerSymbianCollectionPrivate::Completed(TInt aError)
{
    m_error = aError;
    // Let's continue the operation that started the calendar operation
    m_activeSchedulerWait->AsyncStop();
}

/*!
 * From MCalProgressCallBack
 */
TBool OrganizerSymbianCollectionPrivate::NotifyProgress()
{
    // No 
    return EFalse;
}

/*!
 * From MCalChangeCallBack2
 */
void OrganizerSymbianCollectionPrivate::CalChangeNotification(RArray<TCalChangeEntry>& aChangeItems)
{
    // NOTE: We will not be notified of a change if we are the source. So these events are
    // caused by something else than our manager instance.

    QOrganizerItemChangeSet changeSet;
    int count = aChangeItems.Count();
    for (int i=0; i<count; i++) 
    {
        QOrganizerItemLocalId id = toItemLocalId(m_calCollectionId, aChangeItems[i].iEntryId);
        switch(aChangeItems[i].iChangeType)
        {
        case MCalChangeCallBack2::EChangeAdd:       
            changeSet.insertAddedItem(id);
            break;

        case MCalChangeCallBack2::EChangeDelete:
            changeSet.insertRemovedItem(id);
            break;

        case MCalChangeCallBack2::EChangeModify:
            changeSet.insertChangedItem(id);
            break;

        case MCalChangeCallBack2::EChangeUndefined:
            // fallthrough
        default: 
            changeSet.setDataChanged(true);
            break;
        }
    }

    changeSet.emitSignals(m_engine);
}

OrganizerSymbianCollection::OrganizerSymbianCollection()
    :d(0)
{
    d = new OrganizerSymbianCollectionPrivate();
}

OrganizerSymbianCollection::OrganizerSymbianCollection(QOrganizerItemManagerEngine *engine)
    :d(0)
{
    d = new OrganizerSymbianCollectionPrivate();
    d->m_engine = engine;
    d->m_id.setManagerUri(engine->managerUri());
}

OrganizerSymbianCollection::OrganizerSymbianCollection(const OrganizerSymbianCollection &other)
    :d(other.d)
{

}

#ifdef SYMBIAN_CALENDAR_V2
void OrganizerSymbianCollection::openL(const TDesC &fileName, const CCalCalendarInfo *calInfo)
#else
void OrganizerSymbianCollection::openL(const TDesC &fileName)
#endif
{
    // Free the old
    if (d->m_calSession)
        d->m_calSession->StopChangeNotification();
    delete d->m_calEntryView;
    d->m_calEntryView = 0;
    delete d->m_calInstanceView;
    d->m_calInstanceView = 0;
    delete d->m_calSession;
    d->m_calSession = 0;
    
    // Create a new session
    d->m_calSession = CCalSession::NewL();
    
    // Try opening the calendar file
    TRAPD(err, d->m_calSession->OpenL(fileName));
    if (err == KErrNotFound) {
        // File not found
        // Create a new calendar file
#ifdef SYMBIAN_CALENDAR_V2
        if (calInfo)
            d->m_calSession->CreateCalFileL(fileName, *calInfo);
        else
            d->m_calSession->CreateCalFileL(fileName);
#else
        d->m_calSession->CreateCalFileL(fileName);
#endif
        // And open it
        d->m_calSession->OpenL(fileName);
    } else {
        User::LeaveIfError(err);
#ifdef SYMBIAN_CALENDAR_V2
        if (calInfo)
            d->m_calSession->SetCalendarInfoL(*calInfo);
#endif
    }

#ifdef SYMBIAN_CALENDAR_V2
    // Get the actual filename from the session (it may differ from the given filename)
    CCalCalendarInfo *info = d->m_calSession->CalendarInfoL();
    CleanupStack::PushL(info);
    d->m_fileName = toQString(info->FileNameL());
    CleanupStack::PopAndDestroy(info);
#else
    d->m_fileName = toQString(fileName);
#endif
    
    // Get collection id
#ifdef SYMBIAN_CALENDAR_V2
    // TODO: Use filename hash instead of CollectionIdL
    d->m_calCollectionId = d->m_calSession->CollectionIdL();
#else
    // TODO: If multiple calendars are to be supported without native support
    // we need to generate a real id here. Currently there will only be one
    // collection so it does not matter if its just a magic number.
    d->m_calCollectionId = 1;
#endif
    d->m_id.setLocalId(toCollectionLocalId(d->m_calCollectionId));
    
    // Start listening to calendar events
    TCalTime minTime;
    minTime.SetTimeUtcL(TCalTime::MinTime());
    TCalTime maxTime;
    maxTime.SetTimeUtcL(TCalTime::MaxTime());
    CalCommon::TCalTimeRange calTimeRange(minTime, maxTime);
    CCalChangeNotificationFilter *filter = CCalChangeNotificationFilter::NewL(MCalChangeCallBack2::EChangeEntryAll, true, calTimeRange);
    d->m_calSession->StartChangeNotification(*d, *filter);
    delete filter;
}

void OrganizerSymbianCollection::createViewsL()
{
    // Create an entry view
    d->m_calEntryView = CCalEntryView::NewL(*d->m_calSession, *d);
    d->m_activeSchedulerWait->Start(); // stopped at Completed()
    User::LeaveIfError(d->m_error);

    // Create an instance view
    d->m_calInstanceView = CCalInstanceView::NewL(*d->m_calSession, *d);
    d->m_activeSchedulerWait->Start();
    User::LeaveIfError(d->m_error);
}

QOrganizerCollectionId OrganizerSymbianCollection::id() const
{
    return d->m_id;
}

QOrganizerCollectionLocalId OrganizerSymbianCollection::localId() const 
{ 
    return d->m_id.localId(); 
}

quint32 OrganizerSymbianCollection::calCollectionId() const
{
    return d->m_calCollectionId;
}

CCalSession *OrganizerSymbianCollection::calSession() const
{
    return d->m_calSession; 
}

CCalEntryView *OrganizerSymbianCollection::calEntryView() const
{ 
    return d->m_calEntryView; 
}

CCalInstanceView *OrganizerSymbianCollection::calInstanceView() const
{ 
    return d->m_calInstanceView; 
}

QString OrganizerSymbianCollection::fileName() const
{
    return d->m_fileName;
}

bool OrganizerSymbianCollection::isValid() const
{
    return (d->m_calCollectionId > 0);
}

bool OrganizerSymbianCollection::isMarkedForDeletionL() const
{
#ifdef SYMBIAN_CALENDAR_V2
    // Get EMarkAsDelete property
    CCalCalendarInfo *calInfo = d->m_calSession->CalendarInfoL();
    TBool markAsDelete = EFalse;
    // NOTE: trap ignore because property might not exist
    TRAP_IGNORE(markAsDelete = getCalInfoPropertyL<TBool>(*calInfo, EMarkAsDelete));
    delete calInfo;
    return markAsDelete;
#else
    return false;
#endif
}

QOrganizerCollection OrganizerSymbianCollection::toQOrganizerCollectionL() const
{
    // Nothing to convert?
    if (!d->m_engine && !d->m_calSession)
        User::Leave(KErrNotReady);

    QOrganizerCollection collection;

#ifdef SYMBIAN_CALENDAR_V2
    // Read metadata
    CCalCalendarInfo* calInfo = d->m_calSession->CalendarInfoL();
    CleanupStack::PushL(calInfo);
    collection.setMetaData(toMetaDataL(*calInfo));       
    CleanupStack::PopAndDestroy(calInfo);
#else
    collection.setMetaData("FileName", d->m_fileName);
#endif

    // Set id
    collection.setId(this->id());
    
    return collection;
}
