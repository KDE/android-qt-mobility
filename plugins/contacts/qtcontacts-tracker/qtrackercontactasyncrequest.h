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

#ifndef QTRACKERCONTACTASYNCREQUEST_H_
#define QTRACKERCONTACTASYNCREQUEST_H_

#include <QObject>

#include <QtTracker/Tracker>
#include <QtTracker/QLive>

#include <qmobilityglobal.h>
#include <qcontact.h>
#include <qcontactonlineaccount.h>
#include <qcontactmanager.h>

QTM_BEGIN_NAMESPACE
class QContactAbstractRequest;
class QContactManagerEngine;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

/*!
 * Current implementation only server get all contacts requests.
 * This class is a link between QtMobility async request and tracker signals
 */
class QTrackerContactAsyncRequest
{
public:
    QTrackerContactAsyncRequest(QContactAbstractRequest* req);
    virtual ~QTrackerContactAsyncRequest();

protected:
    QContactAbstractRequest* req;
};

namespace IMAccount {
    enum IMResultColumn {
        URI,
        ContactIMId,
        ContactPresence,
        ContactMessage,
        ContactNickname,
        ContactDisplayname
    };
};

namespace IMContact {
    enum IMResultColumn {
        URI,
        ContactId,
        ContactIMId,
        ContactPresence,
        ContactMessage,
        ContactNickname,
        AccountType,
        Capabilities,
        MetaContact,
        ServiceProvider
    };
};

/*!
 * Running QContactFetchRequest. Doing the async tracker query and when data is ready setting the
 * finished status of request. \sa QTrackerContactFetchRequest
 */
class QTrackerContactFetchRequest : public QObject, public QTrackerContactAsyncRequest
{
    Q_OBJECT
//    Q_ENUMS(IMResultColumn)
public:
    QTrackerContactFetchRequest(QContactAbstractRequest* req, QContactManagerEngine* parent);
public slots:
    void contactsReady();
    void phoneNumbersReady();
    void emailAddressesReady();
    void iMAcountsReady();

protected slots:
    virtual void run();
    virtual void emitFinished(QContactManager::Error error = QContactManager::NoError);

protected:
    QContactManager::Error applyFilterToContact(SopranoLive::RDFVariable &variable, const QContactFilter &filter);
    QContactManager::Error applyDetailRangeFilterToContact(SopranoLive::RDFVariable &variable, const QContactFilter &filter);

    // contacts query
    SopranoLive::LiveNodes query;

    QList<SopranoLive::LiveNodes> queryPhoneNumbersNodes; // 2 - one for affiliations and another one for PersonContact
    int queryPhoneNumbersNodesPending;
    QList<SopranoLive::LiveNodes> queryEmailAddressNodes; // 2 - one for affiliations and another one for PersonContact
    int queryEmailAddressNodesPending;
    SopranoLive::LiveNodes queryIMAccountNodes;
    int queryIMAccountNodesPending;

    // result of the request - multiple queries updating it
    QList<QContact> result;

private:
    bool isMeContact(const QContactFilter &filter);
    // fills received phone number from tracker to list of contacts to QContactPhoneMumber details
    // all the following methods update \sa result
    void processQueryPhoneNumbers(SopranoLive::LiveNodes queryPhoneNumbers, bool affiliationNumbers);
    void processQueryEmailAddresses(SopranoLive::LiveNodes queryEmailAddresses, bool affiliationEmails);
    void processQueryIMContacts(SopranoLive::LiveNodes queryIMContacts);
    void validateRequest();
    void readFromQueryRowToContact(QContact &contact, int queryRow);
    QContact &linkContactsWithSameMetaContact(QContact &first, QContact &second);
    void addContactToResultSet(QContact &contact, const QString &metacontact);
    QContactOnlineAccount getOnlineAccountFromIMQuery(SopranoLive::LiveNodes imAccountQuery, int queryRow);
    QContactOnlineAccount getIMAccountFromIMQuery(SopranoLive::LiveNodes imAccountQuery, int queryRow) ;
    QContactOnlineAccount getIMContactFromIMQuery(SopranoLive::LiveNodes imAccountQuery, int queryRow);

    // access existing contacts in result list, contactid to index in \sa result lookup
    QHash<quint32, int> id2ContactLookup;
    // metacontact to index in \sa result lookup - index of metacontact contact: only 1 contact returned when multiple have the same metacontact
    QHash<QString, int> metacontactLookup;
};

#endif /* QTRACKERCONTACTASYNCREQUEST_H_ */
