/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in Technology Preview License Agreement accompanying
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QObject>
#include <QTest>
#include <QDebug>

#include "qtmessaging.h"
#include "../support/support.h"

//TESTED_CLASS=
//TESTED_FILES=

/*
    Unit test for classes implementing query keys for the QMessageStore class.
*/
class tst_QMessageStoreKeys : public QObject
{
    Q_OBJECT

public:
    tst_QMessageStoreKeys();
    virtual ~tst_QMessageStoreKeys();

private slots:
    void initTestCase();
    void cleanup();
    void cleanupTestCase();

    void testAccountFilter_data();
    void testAccountFilter();

    void testAccountOrdering_data();
    void testAccountOrdering();

#ifdef QMESSAGING_OPTIONAL_FOLDER
    void testFolderFilter_data();
    void testFolderFilter();

    void testFolderOrdering_data();
    void testFolderOrdering();
#endif
    void testMessageFilter_data();
    void testMessageFilter();

    void testMessageOrdering_data();
    void testMessageOrdering();

private:
    QSet<QMessageAccountId> existingAccountIds;
    QMessageAccountIdList accountIds;

#ifdef QMESSAGING_OPTIONAL_FOLDER
    QSet<QMessageFolderId> existingFolderIds;
    QMessageFolderIdList folderIds;
#endif

    QSet<QMessageId> existingMessageIds;
    QMessageIdList messageIds;
};

Q_DECLARE_METATYPE(QMessageAccountIdList)
Q_DECLARE_METATYPE(QMessageAccountFilter)
Q_DECLARE_METATYPE(QMessageAccountOrdering)

#ifdef QMESSAGING_OPTIONAL_FOLDER
Q_DECLARE_METATYPE(QMessageFolderIdList)
Q_DECLARE_METATYPE(QMessageFolderFilter)
Q_DECLARE_METATYPE(QMessageFolderOrdering)
#endif

Q_DECLARE_METATYPE(QMessageIdList)
Q_DECLARE_METATYPE(QMessageFilter)
Q_DECLARE_METATYPE(QMessageOrdering)

typedef QList<QMessageIdList> MessageListList;
Q_DECLARE_METATYPE(MessageListList)

QDebug &operator<<(QDebug &dbg, const QMessageAccountId &id)
{
    return dbg << id.toString();
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
QDebug &operator<<(QDebug &dbg, const QMessageFolderId &id)
{
    return dbg << id.toString();
}
#endif
QDebug &operator<<(QDebug &dbg, const QMessageId &id)
{
    return dbg << id.toString();
}

QTEST_MAIN(tst_QMessageStoreKeys)

#include "tst_qmessagestorekeys.moc"

tst_QMessageStoreKeys::tst_QMessageStoreKeys()
{
}

tst_QMessageStoreKeys::~tst_QMessageStoreKeys()
{
}

class Params
{
    QMap<const char*, const char*> _map;

public:
    Params() {}

    Params& operator()(const char* name, const char* value)
    {
        _map.insert(name, value); return *this; 
    }

    operator Support::Parameters() const
    {
        Support::Parameters result;
        QMap<const char*, const char*>::const_iterator it = _map.begin(), end = _map.end();
        for ( ; it != end; ++it) {
            result.insert(it.key(), it.value());
        }
        return result;
    }
};

void tst_QMessageStoreKeys::initTestCase()
{
    Support::clearMessageStore();

    existingAccountIds = QMessageStore::instance()->queryAccounts().toSet();

    QList<Support::Parameters> accountParams;

    accountParams << Params()("name", "Work")
                             ("fromAddress", "Important.Person@example.com")
                  << Params()("name", "Personal")
                             ("fromAddress", "superstar2000@example.net")
                  << Params()("name", "Alter Ego")
                             ("fromAddress", "UltraMegaLightningBabe@superhero.test");

    foreach (const Support::Parameters &params, accountParams) {
        accountIds.append(Support::addAccount(params));
        QVERIFY(accountIds.last().isValid());
    }

#ifdef QMESSAGING_OPTIONAL_FOLDER
    existingFolderIds = QMessageStore::instance()->queryFolders().toSet();

    QList<Support::Parameters> folderParams;
    folderParams << Params()("parentAccountName", "Alter Ego")
                            ("path", "My messages")
                            ("displayName", "My messages")
                            ("parentFolderPath", "")
                 << Params()("parentAccountName", "Work")
                            ("path", "Inbox")
                            ("displayName", "Inbox")
                            ("parentFolderPath", "")
                 << Params()("parentAccountName", "Work")
                            ("path", "Inbox/X-Announce")
                            ("displayName", "X-Announce")
                            ("parentFolderPath", "Inbox")
                 << Params()("parentAccountName", "Work")
                            ("path", "Inbox/X-Announce/X-Archived")
                            ("displayName", "X-Archived")
                            ("parentFolderPath", "Inbox/X-Announce");

    foreach (const Support::Parameters &params, folderParams) {
        folderIds.append(Support::addFolder(params));
        QVERIFY(folderIds.last().isValid());
    }
#endif

    existingMessageIds = QMessageStore::instance()->queryMessages().toSet();

    QList<Support::Parameters> messageParams;
    messageParams << Params()("parentAccountName", "Alter Ego")
                             ("parentFolderPath", "My messages")
                             ("type", "sms")
                             ("to", "SuperMegaLightningBabe")
                             ("from", "Frozone")
                             ("subject", "Ice to meet you")
                             ("date", "2000-01-01T12:00:00Z")
                             ("receivedDate", "2000-01-01T12:01:00Z")
                             ("priority", "")
                             ("size", "160")
                             ("status-read", "true")
                             ("custom-flagged", "true")
                  << Params()("parentAccountName", "Work")
                             ("parentFolderPath", "Inbox")
                             ("type", "email")
                             ("to", "Important.Person@example.com")
                             ("from", "Esteemed.Colleague@example.com")
                             ("subject", "Meeting agenda")
                             ("date", "2000-01-01T12:00:00Z")
                             ("receivedDate", "2000-01-01T20:00:00Z")
                             ("priority", "High")
                             ("size", "10240")
                             ("status-hasAttachments", "true")
                             ("custom-spam", "filter:no")
                             ("custom-flagged", "true")
                  << Params()("parentAccountName", "Work")
                             ("parentFolderPath", "Inbox")
                             ("type", "email")
                             ("to", "Important.Person@example.com,Minion@example.com")
                             ("from", "Big.Boss@example.com")
                             ("subject", "Motivational message")
                             ("date", "2000-01-01T13:00:00Z")
                             ("receivedDate", "2000-01-01T13:05:00Z")
                             ("priority", "High")
                             ("size", "20480")
                             ("status-hasAttachments", "true")
                             ("custom-spam", "filter:no")
                  << Params()("parentAccountName", "Work")
                             ("parentFolderPath", "Inbox/X-Announce")
                             ("type", "email")
                             ("to", "announce@example.com,maintenance-log@example.com")
                             ("from", "sysadmin@example.com")
                             ("subject", "Scheduled maintenance")
                             ("date", "2000-01-01T13:00:00Z")
                             ("receivedDate", "2000-01-01T13:00:01Z")
                             ("priority", "")
                             ("size", "1056")
                             ("status-read", "true")
                             ("custom-spam", "filter:maybe")
                  << Params()("parentAccountName", "Work")
                             ("parentFolderPath", "Inbox/X-Announce/X-Archived")
                             ("type", "email")
                             ("to", "announce@example.com")
                             ("from", "Big.Boss@example.com")
                             ("subject", "Free beer")
                             ("date", "1999-04-01T10:30:00Z")
                             ("receivedDate", "1999-04-01T10:31:00Z")
                             ("priority", "Low")
                             ("size", "4096")
                             ("status-read", "true")
                             ("status-hasAttachments", "true")
                             ("custom-spam", "filter:yes");

        foreach (const Support::Parameters &params, messageParams) {
        messageIds.append(Support::addMessage(params));
        QVERIFY(messageIds.last().isValid());
    }
}

void tst_QMessageStoreKeys::cleanup()
{
}

void tst_QMessageStoreKeys::cleanupTestCase()
{
}

void tst_QMessageStoreKeys::testAccountFilter_data()
{
    QTest::addColumn<QMessageAccountFilter>("filter");
    QTest::addColumn<QMessageAccountIdList>("ids");
    QTest::addColumn<QMessageAccountIdList>("negatedIds");

    QTest::newRow("empty filter")
        << QMessageAccountFilter() 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("id equality 1")
        << QMessageAccountFilter::byId(accountIds[0], QMessageDataComparator::Equal) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] );

    QTest::newRow("id equality 2")
        << QMessageAccountFilter::byId(accountIds[1], QMessageDataComparator::Equal) 
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] );

    QTest::newRow("id equality invalid")
        << QMessageAccountFilter::byId(QMessageAccountId(), QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("id inequality 1")
        << QMessageAccountFilter::byId(accountIds[0], QMessageDataComparator::NotEqual) 
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("id inequality 2")
        << QMessageAccountFilter::byId(accountIds[1], QMessageDataComparator::NotEqual) 
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[1] );

    QTest::newRow("id inequality invalid")
        << QMessageAccountFilter::byId(QMessageAccountId(), QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("id list inclusion 1")
        << QMessageAccountFilter::byId(QMessageAccountIdList() << accountIds[0], QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] );

    QTest::newRow("id list inclusion 2")
        << QMessageAccountFilter::byId(QMessageAccountIdList() << accountIds[1], QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] );

    QTest::newRow("id list inclusion 3")
        << QMessageAccountFilter::byId(QMessageAccountIdList() << accountIds[0] << accountIds[1], QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[2] );

    QTest::newRow("id list inclusion empty")
        << QMessageAccountFilter::byId(QMessageAccountIdList(), QMessageDataComparator::Includes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("id list exclusion 1")
        << QMessageAccountFilter::byId(QMessageAccountIdList() << accountIds[0], QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("id list exclusion 2")
        << QMessageAccountFilter::byId(QMessageAccountIdList() << accountIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[1] );

    QTest::newRow("id list exclusion 3")
        << QMessageAccountFilter::byId(QMessageAccountIdList() << accountIds[0] << accountIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[1] );

    QTest::newRow("id list exclusion empty")
        << QMessageAccountFilter::byId(QMessageAccountIdList(), QMessageDataComparator::Excludes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("id filter inclusion 1")
        << QMessageAccountFilter::byId(QMessageAccountFilter::byName("Work", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] );

    QTest::newRow("id filter inclusion 2")
        << QMessageAccountFilter::byId(QMessageAccountFilter::byName("Personal", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] );

    QTest::newRow("id filter inclusion 3")
        << QMessageAccountFilter::byId(QMessageAccountFilter::byName("o", QMessageDataComparator::Includes), QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("id filter inclusion empty")
        << QMessageAccountFilter::byId(QMessageAccountFilter(), QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("id filter inclusion non-matching")
        << QMessageAccountFilter::byId(QMessageAccountFilter::byName("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("id filter exclusion 1")
        << QMessageAccountFilter::byId(QMessageAccountFilter::byName("Work", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("id filter exclusion 2")
        << QMessageAccountFilter::byId(QMessageAccountFilter::byName("Personal", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[1] );

    QTest::newRow("id filter exclusion 3")
        << QMessageAccountFilter::byId(QMessageAccountFilter::byName("o", QMessageDataComparator::Includes), QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("id filter exclusion empty")
        << QMessageAccountFilter::byId(QMessageAccountFilter(), QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("id filter exclusion non-matching")
        << QMessageAccountFilter::byId(QMessageAccountFilter::byName("NoneSuch"), QMessageDataComparator::Excludes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name equality 1")
        << QMessageAccountFilter::byName("Work", QMessageDataComparator::Equal) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] );

    QTest::newRow("name equality 2")
        << QMessageAccountFilter::byName("Personal", QMessageDataComparator::Equal) 
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] );

    QTest::newRow("name equality non-matching")
        << QMessageAccountFilter::byName("Nonesuch", QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("name equality empty")
        << QMessageAccountFilter::byName(QString(), QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("name equality zero-length")
        << QMessageAccountFilter::byName("", QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("name inequality 1")
        << QMessageAccountFilter::byName("Work", QMessageDataComparator::NotEqual) 
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("name inequality 2")
        << QMessageAccountFilter::byName("Personal", QMessageDataComparator::NotEqual) 
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[1] );

    QTest::newRow("name inequality non-matching")
        << QMessageAccountFilter::byName("Nonesuch", QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name inequality empty")
        << QMessageAccountFilter::byName(QString(), QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name inequality zero-length")
        << QMessageAccountFilter::byName("", QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name inclusion 1")
        << QMessageAccountFilter::byName("ork", QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] );

    QTest::newRow("name inclusion 2")
        << QMessageAccountFilter::byName("o", QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name inclusion non-matching")
        << QMessageAccountFilter::byName("Nonesuch", QMessageDataComparator::Includes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("name inclusion empty")
        << QMessageAccountFilter::byName(QString(), QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name inclusion zero-length")
        << QMessageAccountFilter::byName("", QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name exclusion 1")
        << QMessageAccountFilter::byName("ork", QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("name exclusion 2")
        << QMessageAccountFilter::byName("o", QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("name exclusion non-matching")
        << QMessageAccountFilter::byName("Nonesuch", QMessageDataComparator::Excludes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name exclusion empty")
        << QMessageAccountFilter::byName(QString(), QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("name exclusion zero-length")
        << QMessageAccountFilter::byName("", QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("fromAddress equality 1")
        << QMessageAccountFilter::byFromAddress("Important.Person@example.com", QMessageDataComparator::Equal) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] );

    QTest::newRow("fromAddress equality 2")
        << QMessageAccountFilter::byFromAddress("superstar2000@example.net", QMessageDataComparator::Equal) 
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] );

    QTest::newRow("fromAddress equality non-matching")
        << QMessageAccountFilter::byFromAddress("Nonesuch", QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("fromAddress equality empty")
        << QMessageAccountFilter::byFromAddress(QString(), QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("fromAddress equality zero-length")
        << QMessageAccountFilter::byFromAddress("", QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("fromAddress inequality 1")
        << QMessageAccountFilter::byFromAddress("Important.Person@example.com", QMessageDataComparator::NotEqual) 
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("fromAddress inequality 2")
        << QMessageAccountFilter::byFromAddress("superstar2000@example.net", QMessageDataComparator::NotEqual) 
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[1] );

    QTest::newRow("fromAddress inequality non-matching")
        << QMessageAccountFilter::byFromAddress("Nonesuch", QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("fromAddress inequality empty")
        << QMessageAccountFilter::byFromAddress(QString(), QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("fromAddress inequality zero-length")
        << QMessageAccountFilter::byFromAddress("", QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("fromAddress inclusion 1")
        << QMessageAccountFilter::byFromAddress("Import", QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] );

    QTest::newRow("fromAddress inclusion 2")
        << QMessageAccountFilter::byFromAddress("@example.", QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[2] );

    QTest::newRow("fromAddress inclusion non-matching")
        << QMessageAccountFilter::byFromAddress("Nonesuch", QMessageDataComparator::Includes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("fromAddress inclusion empty")
        << QMessageAccountFilter::byFromAddress(QString(), QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("fromAddress inclusion zero-length")
        << QMessageAccountFilter::byFromAddress("", QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("fromAddress exclusion 1")
        << QMessageAccountFilter::byFromAddress("Import", QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("fromAddress exclusion 2")
        << QMessageAccountFilter::byFromAddress("@example.", QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[1] );

    QTest::newRow("fromAddress exclusion non-matching")
        << QMessageAccountFilter::byFromAddress("Nonesuch", QMessageDataComparator::Excludes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("fromAddress exclusion empty")
        << QMessageAccountFilter::byFromAddress(QString(), QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("fromAddress exclusion zero-length")
        << QMessageAccountFilter::byFromAddress("", QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;
}

void tst_QMessageStoreKeys::testAccountFilter()
{
    QFETCH(QMessageAccountFilter, filter);
    QFETCH(QMessageAccountIdList, ids);
    QFETCH(QMessageAccountIdList, negatedIds);

    if (filter.isSupported()) {
        // Order is irrelevant for filtering
        QCOMPARE(QMessageStore::instance()->queryAccounts(filter).toSet().subtract(existingAccountIds), ids.toSet());
        QCOMPARE(QMessageStore::instance()->queryAccounts(~filter).toSet().subtract(existingAccountIds), negatedIds.toSet());
    }
}

void tst_QMessageStoreKeys::testAccountOrdering_data()
{
    QTest::addColumn<QMessageAccountOrdering>("ordering");
    QTest::addColumn<QMessageAccountIdList>("ids");

    QTest::newRow("name ascending")
        << QMessageAccountOrdering::byName(Qt::AscendingOrder)
        << ( QMessageAccountIdList() << accountIds[2] << accountIds[1] << accountIds[0] );

    QTest::newRow("name descending")
        << QMessageAccountOrdering::byName(Qt::DescendingOrder)
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[1] << accountIds[2] );
}

void tst_QMessageStoreKeys::testAccountOrdering()
{
    QFETCH(QMessageAccountOrdering, ordering);
    QFETCH(QMessageAccountIdList, ids);

    // Filter out the existing accounts
    QMessageAccountIdList sortedIds(QMessageStore::instance()->queryAccounts(QMessageAccountFilter(), ordering));
    for (QMessageAccountIdList::iterator it = sortedIds.begin(); it != sortedIds.end(); ) {
        if (existingAccountIds.contains(*it)) {
            it = sortedIds.erase(it);
        } else {
            ++it;
        }
    }

    QCOMPARE(sortedIds, ids);
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
void tst_QMessageStoreKeys::testFolderFilter_data()
{
    QTest::addColumn<QMessageFolderFilter>("filter");
    QTest::addColumn<QMessageFolderIdList>("ids");
    QTest::addColumn<QMessageFolderIdList>("negatedIds");

    QTest::newRow("empty filter")
        << QMessageFolderFilter() 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("id equality 1")
        << QMessageFolderFilter::byId(folderIds[0], QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("id equality 2")
        << QMessageFolderFilter::byId(folderIds[1], QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[2] << folderIds[3] );

    QTest::newRow("id equality invalid")
        << QMessageFolderFilter::byId(QMessageFolderId(), QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("id inequality 1")
        << QMessageFolderFilter::byId(folderIds[0], QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("id inequality 2")
        << QMessageFolderFilter::byId(folderIds[1], QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[1] );

    QTest::newRow("id inequality invalid")
        << QMessageFolderFilter::byId(QMessageFolderId(), QMessageDataComparator::NotEqual) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("id list inclusion 1")
        << QMessageFolderFilter::byId(QMessageFolderIdList() << folderIds[0], QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("id list inclusion 2")
        << QMessageFolderFilter::byId(QMessageFolderIdList() << folderIds[1], QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[2] << folderIds[3] );

    QTest::newRow("id list inclusion 3")
        << QMessageFolderFilter::byId(QMessageFolderIdList() << folderIds[0] << folderIds[1], QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("id list inclusion empty")
        << QMessageFolderFilter::byId(QMessageFolderIdList(), QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("id list exclusion 1")
        << QMessageFolderFilter::byId(QMessageFolderIdList() << folderIds[0], QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("id list exclusion 2")
        << QMessageFolderFilter::byId(QMessageFolderIdList() << folderIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[1] );

    QTest::newRow("id list exclusion 3")
        << QMessageFolderFilter::byId(QMessageFolderIdList() << folderIds[0] << folderIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("id list exclusion empty")
        << QMessageFolderFilter::byId(QMessageFolderIdList(), QMessageDataComparator::Excludes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("id filter inclusion 1")
        << QMessageFolderFilter::byId(QMessageFolderFilter::byPath("My messages", QMessageDataComparator::Equal), QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("id filter inclusion 2")
        << QMessageFolderFilter::byId(QMessageFolderFilter::byPath("Inbox/X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] );

    QTest::newRow("id filter inclusion 3")
        << QMessageFolderFilter::byId(QMessageFolderFilter::byPath("box/X-Ann", QMessageDataComparator::Includes), QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("id filter inclusion empty")
        << QMessageFolderFilter::byId(QMessageFolderFilter(), QMessageDataComparator::Includes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("id filter inclusion non-matching")
        << QMessageFolderFilter::byId(QMessageFolderFilter::byPath("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("id filter exclusion 1")
        << QMessageFolderFilter::byId(QMessageFolderFilter::byPath("My messages", QMessageDataComparator::Equal), QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("id filter exclusion 2")
        << QMessageFolderFilter::byId(QMessageFolderFilter::byPath("Inbox/X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[2] );

    QTest::newRow("id filter exclusion 3")
        << QMessageFolderFilter::byId(QMessageFolderFilter::byPath("box/X-Ann", QMessageDataComparator::Includes), QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("id filter exclusion empty")
        << QMessageFolderFilter::byId(QMessageFolderFilter(), QMessageDataComparator::Excludes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("id filter exclusion non-matching")
        << QMessageFolderFilter::byId(QMessageFolderFilter::byPath("NoneSuch"), QMessageDataComparator::Excludes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("path equality 1")
        << QMessageFolderFilter::byPath("My messages", QMessageDataComparator::Equal)
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("path equality 2")
        << QMessageFolderFilter::byPath("Inbox/X-Announce", QMessageDataComparator::Equal)
        << ( QMessageFolderIdList() << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] );

    QTest::newRow("path equality non-matching")
        << QMessageFolderFilter::byPath("Nonesuch", QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("path equality empty")
        << QMessageFolderFilter::byPath(QString(), QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("path equality zero-length")
        << QMessageFolderFilter::byPath("", QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("path inequality 1")
        << QMessageFolderFilter::byPath("My messages", QMessageDataComparator::NotEqual)
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("path inequality 2")
        << QMessageFolderFilter::byPath("Inbox/X-Announce", QMessageDataComparator::NotEqual)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[2] );

    QTest::newRow("path inequality non-matching")
        << QMessageFolderFilter::byPath("Nonesuch", QMessageDataComparator::NotEqual) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("path inequality empty")
        << QMessageFolderFilter::byPath(QString(), QMessageDataComparator::NotEqual) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("path inequality zero-length")
        << QMessageFolderFilter::byPath("", QMessageDataComparator::NotEqual) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("path inclusion 1")
        << QMessageFolderFilter::byPath("ess", QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("path inclusion 2")
        << QMessageFolderFilter::byPath("box/X-Ann", QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("path inclusion non-matching")
        << QMessageFolderFilter::byPath("Nonesuch", QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("path inclusion empty")
        << QMessageFolderFilter::byPath(QString(), QMessageDataComparator::Includes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("path inclusion zero-length")
        << QMessageFolderFilter::byPath("", QMessageDataComparator::Includes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("path exclusion 1")
        << QMessageFolderFilter::byPath("ess", QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("path exclusion 2")
        << QMessageFolderFilter::byPath("box/X-Ann", QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("path exclusion non-matching")
        << QMessageFolderFilter::byPath("Nonesuch", QMessageDataComparator::Excludes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("path exclusion empty")
        << QMessageFolderFilter::byPath(QString(), QMessageDataComparator::Excludes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("path exclusion zero-length")
        << QMessageFolderFilter::byPath("", QMessageDataComparator::Excludes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("displayName equality 1")
        << QMessageFolderFilter::byDisplayName("My messages", QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("displayName equality 2")
        << QMessageFolderFilter::byDisplayName("X-Announce", QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] );

    QTest::newRow("displayName equality non-matching")
        << QMessageFolderFilter::byDisplayName("Nonesuch", QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("displayName equality empty")
        << QMessageFolderFilter::byDisplayName(QString(), QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("displayName equality zero-length")
        << QMessageFolderFilter::byDisplayName("", QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("displayName inequality 1")
        << QMessageFolderFilter::byDisplayName("My messages", QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("displayName inequality 2")
        << QMessageFolderFilter::byDisplayName("X-Announce", QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[2] );

    QTest::newRow("displayName inequality non-matching")
        << QMessageFolderFilter::byDisplayName("Nonesuch", QMessageDataComparator::NotEqual) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("displayName inequality empty")
        << QMessageFolderFilter::byDisplayName(QString(), QMessageDataComparator::NotEqual) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("displayName inequality zero-length")
        << QMessageFolderFilter::byDisplayName("", QMessageDataComparator::NotEqual) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("displayName inclusion 1")
        << QMessageFolderFilter::byDisplayName("message", QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("displayName inclusion 2")
        << QMessageFolderFilter::byDisplayName("X-A", QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("displayName inclusion non-matching")
        << QMessageFolderFilter::byDisplayName("Nonesuch", QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("displayName inclusion empty")
        << QMessageFolderFilter::byDisplayName(QString(), QMessageDataComparator::Includes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("displayName inclusion zero-length")
        << QMessageFolderFilter::byDisplayName("", QMessageDataComparator::Includes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("displayName exclusion 1")
        << QMessageFolderFilter::byDisplayName("messages", QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("displayName exclusion 2")
        << QMessageFolderFilter::byDisplayName("X-A", QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("displayName exclusion non-matching")
        << QMessageFolderFilter::byDisplayName("Nonesuch", QMessageDataComparator::Excludes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("displayName exclusion empty")
        << QMessageFolderFilter::byDisplayName(QString(), QMessageDataComparator::Excludes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("displayName exclusion zero-length")
        << QMessageFolderFilter::byDisplayName("", QMessageDataComparator::Excludes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("parentAccountId equality 1")
        << QMessageFolderFilter::byParentAccountId(accountIds[0], QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("parentAccountId equality 2")
        << QMessageFolderFilter::byParentAccountId(accountIds[2], QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("parentAccountId equality invalid")
        << QMessageFolderFilter::byParentAccountId(QMessageAccountId(), QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("parentAccountId inequality 1")
        << QMessageFolderFilter::byParentAccountId(accountIds[0], QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("parentAccountId inequality 2")
        << QMessageFolderFilter::byParentAccountId(accountIds[2], QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("parentAccountId inequality invalid")
        << QMessageFolderFilter::byParentAccountId(QMessageAccountId(), QMessageDataComparator::NotEqual) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("parentAccountId filter inclusion 1")
        << QMessageFolderFilter::byParentAccountId(QMessageAccountFilter::byName("Alter Ego", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("parentAccountId filter inclusion 2")
        << QMessageFolderFilter::byParentAccountId(QMessageAccountFilter::byName("Work", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("parentAccountId filter inclusion 3")
        << QMessageFolderFilter::byParentAccountId(QMessageAccountFilter::byName("o", QMessageDataComparator::Includes), QMessageDataComparator::Includes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("parentAccountId filter inclusion empty")
        << QMessageFolderFilter::byParentAccountId(QMessageAccountFilter(), QMessageDataComparator::Includes) 
        << folderIds 
        << QMessageFolderIdList();

    QTest::newRow("parentAccountId filter inclusion non-matching")
        << QMessageFolderFilter::byParentAccountId(QMessageAccountFilter::byName("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("parentAccountId filter exclusion 1")
        << QMessageFolderFilter::byParentAccountId(QMessageAccountFilter::byName("Alter Ego", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("parentAccountId filter exclusion 2")
        << QMessageFolderFilter::byParentAccountId(QMessageAccountFilter::byName("Work", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("parentAccountId filter exclusion 3")
        << QMessageFolderFilter::byParentAccountId(QMessageAccountFilter::byName("o", QMessageDataComparator::Includes), QMessageDataComparator::Excludes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("parentAccountId filter exclusion empty")
        << QMessageFolderFilter::byParentAccountId(QMessageAccountFilter(), QMessageDataComparator::Excludes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("parentAccountId filter exclusion non-matching")
        << QMessageFolderFilter::byParentAccountId(QMessageAccountFilter::byName("NoneSuch"), QMessageDataComparator::Excludes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("parentFolderId equality 1")
        << QMessageFolderFilter::byParentFolderId(folderIds[1], QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] );

    QTest::newRow("parentFolderId equality 2")
        << QMessageFolderFilter::byParentFolderId(folderIds[2], QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] );

    QTest::newRow("parentFolderId equality invalid")
        << QMessageFolderFilter::byParentFolderId(QMessageFolderId(), QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("parentFolderId inequality 1")
        << QMessageFolderFilter::byParentFolderId(folderIds[1], QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[2] );

    QTest::newRow("parentFolderId inequality 2")
        << QMessageFolderFilter::byParentFolderId(folderIds[2], QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[3] );

    QTest::newRow("parentFolderId inequality invalid")
        << QMessageFolderFilter::byParentFolderId(QMessageFolderId(), QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("parentFolderId filter inclusion 1")
        << QMessageFolderFilter::byParentFolderId(QMessageFolderFilter::byPath("Inbox/X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] );

    QTest::newRow("parentFolderId filter inclusion 2")
        << QMessageFolderFilter::byParentFolderId(QMessageFolderFilter::byPath("Inbox", QMessageDataComparator::Equal), QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] );

    QTest::newRow("parentFolderId filter inclusion 3")
        << QMessageFolderFilter::byParentFolderId(QMessageFolderFilter::byPath("Inbox", QMessageDataComparator::Includes), QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("parentFolderId filter inclusion empty")
        << QMessageFolderFilter::byParentFolderId(QMessageFolderFilter(), QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("parentFolderId filter inclusion non-matching")
        << QMessageFolderFilter::byParentFolderId(QMessageFolderFilter::byPath("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("parentFolderId filter exclusion 1")
        << QMessageFolderFilter::byParentFolderId(QMessageFolderFilter::byPath("Inbox/X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[3] );

    QTest::newRow("parentFolderId filter exclusion 2")
        << QMessageFolderFilter::byParentFolderId(QMessageFolderFilter::byPath("Inbox", QMessageDataComparator::Equal), QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[2] );

    QTest::newRow("parentFolderId filter exclusion 3")
        << QMessageFolderFilter::byParentFolderId(QMessageFolderFilter::byPath("Inbox", QMessageDataComparator::Includes), QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("parentFolderId filter exclusion empty")
        << QMessageFolderFilter::byParentFolderId(QMessageFolderFilter(), QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("parentFolderId filter exclusion non-matching")
        << QMessageFolderFilter::byParentFolderId(QMessageFolderFilter::byPath("NoneSuch"), QMessageDataComparator::Excludes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("ancestorFolderIds list inclusion 1")
        << QMessageFolderFilter::byAncestorFolderIds(folderIds[2], QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] );

    QTest::newRow("ancestorFolderIds list inclusion 2")
        << QMessageFolderFilter::byAncestorFolderIds(folderIds[1], QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("ancestorFolderIds list inclusion invalid")
        << QMessageFolderFilter::byAncestorFolderIds(QMessageFolderId(), QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("ancestorFolderIds list exclusion 1")
        << QMessageFolderFilter::byAncestorFolderIds(folderIds[2], QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[3] );

    QTest::newRow("ancestorFolderIds list exclusion 2")
        << QMessageFolderFilter::byAncestorFolderIds(folderIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("ancestorFolderIds list exclusion invalid")
        << QMessageFolderFilter::byAncestorFolderIds(QMessageFolderId(), QMessageDataComparator::Excludes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("ancestorFolderIds filter inclusion 1")
        << QMessageFolderFilter::byAncestorFolderIds(QMessageFolderFilter::byPath("Inbox/X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] );

    QTest::newRow("ancestorFolderIds filter inclusion 2")
        << QMessageFolderFilter::byAncestorFolderIds(QMessageFolderFilter::byPath("Inbox", QMessageDataComparator::Equal), QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("ancestorFolderIds filter inclusion 3")
        << QMessageFolderFilter::byAncestorFolderIds(QMessageFolderFilter::byPath("Inbox", QMessageDataComparator::Includes), QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("ancestorFolderIds filter inclusion empty")
        << QMessageFolderFilter::byAncestorFolderIds(QMessageFolderFilter(), QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("ancestorFolderIds filter inclusion non-matching")
        << QMessageFolderFilter::byAncestorFolderIds(QMessageFolderFilter::byPath("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("ancestorFolderIds filter exclusion 1")
        << QMessageFolderFilter::byAncestorFolderIds(QMessageFolderFilter::byPath("Inbox/X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[3] );

    QTest::newRow("ancestorFolderIds filter exclusion 2")
        << QMessageFolderFilter::byAncestorFolderIds(QMessageFolderFilter::byPath("Inbox", QMessageDataComparator::Equal), QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("ancestorFolderIds filter exclusion 3")
        << QMessageFolderFilter::byAncestorFolderIds(QMessageFolderFilter::byPath("Inbox", QMessageDataComparator::Includes), QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("ancestorFolderIds filter exclusion empty")
        << QMessageFolderFilter::byAncestorFolderIds(QMessageFolderFilter(), QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("ancestorFolderIds filter exclusion non-matching")
        << QMessageFolderFilter::byAncestorFolderIds(QMessageFolderFilter::byPath("NoneSuch"), QMessageDataComparator::Excludes) 
        << folderIds
        << QMessageFolderIdList();
}

void tst_QMessageStoreKeys::testFolderFilter()
{
    QFETCH(QMessageFolderFilter, filter);
    QFETCH(QMessageFolderIdList, ids);
    QFETCH(QMessageFolderIdList, negatedIds);

    if (filter.isSupported()) {
        // Order is irrelevant for filtering
        QCOMPARE(QMessageStore::instance()->queryFolders(filter).toSet().subtract(existingFolderIds), ids.toSet());
        QCOMPARE(QMessageStore::instance()->queryFolders(~filter).toSet().subtract(existingFolderIds), negatedIds.toSet());
    }
}

void tst_QMessageStoreKeys::testFolderOrdering_data()
{
    QTest::addColumn<QMessageFolderOrdering>("ordering");
    QTest::addColumn<QMessageFolderIdList>("ids");

    QTest::newRow("path ascending")
        << QMessageFolderOrdering::byPath(Qt::AscendingOrder)
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] << folderIds[0] );

    QTest::newRow("path descending")
        << QMessageFolderOrdering::byPath(Qt::DescendingOrder)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[3] << folderIds[2] << folderIds[1] );

    QTest::newRow("displayName ascending")
        << QMessageFolderOrdering::byDisplayName(Qt::AscendingOrder)
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[0] << folderIds[2] << folderIds[3] );

    QTest::newRow("displayName descending")
        << QMessageFolderOrdering::byDisplayName(Qt::DescendingOrder)
        << ( QMessageFolderIdList() << folderIds[3] << folderIds[2] << folderIds[0] << folderIds[1] );
}

void tst_QMessageStoreKeys::testFolderOrdering()
{
    QFETCH(QMessageFolderOrdering, ordering);
    QFETCH(QMessageFolderIdList, ids);

    // Filter out the existing folders
    QMessageFolderIdList sortedIds(QMessageStore::instance()->queryFolders(QMessageFolderFilter(), ordering));
    for (QMessageFolderIdList::iterator it = sortedIds.begin(); it != sortedIds.end(); ) {
        if (existingFolderIds.contains(*it)) {
            it = sortedIds.erase(it);
        } else {
            ++it;
        }
    }

    QCOMPARE(sortedIds, ids);
}

#endif //QMESSAGING_OPTIONAL_FOLDER

void tst_QMessageStoreKeys::testMessageFilter_data()
{
    QTest::addColumn<QMessageFilter>("filter");
    QTest::addColumn<QMessageIdList>("ids");
    QTest::addColumn<QMessageIdList>("negatedIds");

    QTest::newRow("empty filter")
        << QMessageFilter() 
        << messageIds
        << QMessageIdList();

    QTest::newRow("id equality 1")
        << QMessageFilter::byId(messageIds[0], QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("id equality 2")
        << QMessageFilter::byId(messageIds[1], QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[1] )
        << ( QMessageIdList() << messageIds[0] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("id equality invalid")
        << QMessageFilter::byId(QMessageId(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("id inequality 1")
        << QMessageFilter::byId(messageIds[0], QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("id inequality 2")
        << QMessageFilter::byId(messageIds[1], QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] );

    QTest::newRow("id inequality invalid")
        << QMessageFilter::byId(QMessageId(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("id list inclusion 1")
        << QMessageFilter::byId(QMessageIdList() << messageIds[0], QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("id list inclusion 2")
        << QMessageFilter::byId(QMessageIdList() << messageIds[1], QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[1] )
        << ( QMessageIdList() << messageIds[0] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("id list inclusion 3")
        << QMessageFilter::byId(QMessageIdList() << messageIds[0] << messageIds[1], QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] )
        << ( QMessageIdList() << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("id list inclusion empty")
        << QMessageFilter::byId(QMessageIdList(), QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("id list exclusion 1")
        << QMessageFilter::byId(QMessageIdList() << messageIds[0], QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("id list exclusion 2")
        << QMessageFilter::byId(QMessageIdList() << messageIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] );

    QTest::newRow("id list exclusion 3")
        << QMessageFilter::byId(QMessageIdList() << messageIds[0] << messageIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] );

    QTest::newRow("id list exclusion empty")
        << QMessageFilter::byId(QMessageIdList(), QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("type equality 1")
        << QMessageFilter::byType(QMessage::Sms, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("type equality 2")
        << QMessageFilter::byType(QMessage::Email, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("type equality invalid")
        << QMessageFilter::byType(QMessage::None, QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("type inequality 1")
        << QMessageFilter::byType(QMessage::Sms, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("type inequality 2")
        << QMessageFilter::byType(QMessage::Email, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("type inequality invalid")
        << QMessageFilter::byType(QMessage::None, QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("type mask inclusion 1")
        << QMessageFilter::byType(QMessage::Sms, QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("type mask inclusion 2")
        << QMessageFilter::byType(QMessage::Email, QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("type mask inclusion 3")
        << QMessageFilter::byType(QMessage::Sms | QMessage::Email, QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("type mask inclusion empty")
        << QMessageFilter::byType(QMessage::None, QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("type mask exclusion 1")
        << QMessageFilter::byType(QMessage::Sms, QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("type mask exclusion 2")
        << QMessageFilter::byType(QMessage::Email, QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("type mask exclusion 3")
        << QMessageFilter::byType(QMessage::Sms | QMessage::Email, QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("type mask exclusion empty")
        << QMessageFilter::byType(QMessage::None, QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("sender equality 1")
        << QMessageFilter::bySender("Esteemed.Colleague@example.com", QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[1] )
        << ( QMessageIdList() << messageIds[0] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("sender equality 2")
        << QMessageFilter::bySender("sysadmin@example.com", QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[3] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[4] );

    QTest::newRow("sender equality non-matching")
        << QMessageFilter::bySender("Nonesuch", QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("sender equality empty")
        << QMessageFilter::bySender(QString(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("sender equality zero-length")
        << QMessageFilter::bySender("", QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("sender inequality 1")
        << QMessageFilter::bySender("Esteemed.Colleague@example.com", QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] );

    QTest::newRow("sender inequality 2")
        << QMessageFilter::bySender("sysadmin@example.com", QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[4] )
        << ( QMessageIdList() << messageIds[3] );

    QTest::newRow("sender inequality non-matching")
        << QMessageFilter::bySender("Nonesuch", QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("sender inequality empty")
        << QMessageFilter::bySender(QString(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("sender inequality zero-length")
        << QMessageFilter::bySender("", QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("sender inclusion 1")
        << QMessageFilter::bySender("example", QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("sender inclusion 2")
        << QMessageFilter::bySender("ozone", QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("sender inclusion non-matching")
        << QMessageFilter::bySender("Nonesuch", QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("sender inclusion empty")
        << QMessageFilter::bySender(QString(), QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("sender inclusion zero-length")
        << QMessageFilter::bySender("", QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("sender exclusion 1")
        << QMessageFilter::bySender("example", QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("sender exclusion 2")
        << QMessageFilter::bySender("ozone", QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("sender exclusion non-matching")
        << QMessageFilter::bySender("Nonesuch", QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("sender exclusion empty")
        << QMessageFilter::bySender(QString(), QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("sender exclusion zero-length")
        << QMessageFilter::bySender("", QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("recipients equality 1")
        << QMessageFilter::byRecipients("Minion@example.com", QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] );

    QTest::newRow("recipients equality 2")
        << QMessageFilter::byRecipients("Important.Person@example.com", QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] );

    QTest::newRow("recipients equality non-matching")
        << QMessageFilter::byRecipients("Nonesuch", QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("recipients equality empty")
        << QMessageFilter::byRecipients(QString(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("recipients equality zero-length")
        << QMessageFilter::byRecipients("", QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("recipients inequality 1")
        << QMessageFilter::byRecipients("Minion@example.com", QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[2] );

    QTest::newRow("recipients inequality 2")
        << QMessageFilter::byRecipients("Important.Person@example.com", QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] );

    QTest::newRow("recipients inequality non-matching")
        << QMessageFilter::byRecipients("Nonesuch", QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("recipients inequality empty")
        << QMessageFilter::byRecipients(QString(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("recipients inequality zero-length")
        << QMessageFilter::byRecipients("", QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("recipients inclusion 1")
        << QMessageFilter::byRecipients("example", QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("recipients inclusion 2")
        << QMessageFilter::byRecipients("Mega", QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("recipients inclusion non-matching")
        << QMessageFilter::byRecipients("Nonesuch", QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("recipients inclusion empty")
        << QMessageFilter::byRecipients(QString(), QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("recipients inclusion zero-length")
        << QMessageFilter::byRecipients("", QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("recipients exclusion 1")
        << QMessageFilter::byRecipients("example", QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("recipients exclusion 2")
        << QMessageFilter::byRecipients("Mega", QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("recipients exclusion non-matching")
        << QMessageFilter::byRecipients("Nonesuch", QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("recipients exclusion empty")
        << QMessageFilter::byRecipients(QString(), QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("recipients exclusion zero-length")
        << QMessageFilter::byRecipients("", QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("subject equality 1")
        << QMessageFilter::bySubject("Free beer", QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("subject equality 2")
        << QMessageFilter::bySubject("Scheduled maintenance", QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[3] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[4] );

    QTest::newRow("subject equality non-matching")
        << QMessageFilter::bySubject("Nonesuch", QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("subject equality empty")
        << QMessageFilter::bySubject(QString(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("subject equality zero-length")
        << QMessageFilter::bySubject("", QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("subject inequality 1")
        << QMessageFilter::bySubject("Free beer", QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] );

    QTest::newRow("subject inequality 2")
        << QMessageFilter::bySubject("Scheduled maintenance", QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[4] )
        << ( QMessageIdList() << messageIds[3] );

    QTest::newRow("subject inequality non-matching")
        << QMessageFilter::bySubject("Nonesuch", QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("subject inequality empty")
        << QMessageFilter::bySubject(QString(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("subject inequality zero-length")
        << QMessageFilter::bySubject("", QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("subject inclusion 1")
        << QMessageFilter::bySubject("vation", QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] );

    QTest::newRow("subject inclusion 2")
        << QMessageFilter::bySubject(" ", QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("subject inclusion non-matching")
        << QMessageFilter::bySubject("Nonesuch", QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("subject inclusion empty")
        << QMessageFilter::bySubject(QString(), QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("subject inclusion zero-length")
        << QMessageFilter::bySubject("", QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("subject exclusion 1")
        << QMessageFilter::bySubject("vation", QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[2] );

    QTest::newRow("subject exclusion 2")
        << QMessageFilter::bySubject(" ", QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("subject exclusion non-matching")
        << QMessageFilter::bySubject("Nonesuch", QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("subject exclusion empty")
        << QMessageFilter::bySubject(QString(), QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("subject exclusion zero-length")
        << QMessageFilter::bySubject("", QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("timeStamp equality 1")
        << QMessageFilter::byTimeStamp(QDateTime::fromString("1999-04-01T10:30:00Z", Qt::ISODate), QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("timeStamp equality 2")
        << QMessageFilter::byTimeStamp(QDateTime::fromString("2000-01-01T13:00:00Z", Qt::ISODate), QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[3] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[4] );

    QTest::newRow("timeStamp equality non-matching")
        << QMessageFilter::byTimeStamp(QDateTime::fromString("1990-01-01T01:00:00Z", Qt::ISODate), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("timeStamp equality empty")
        << QMessageFilter::byTimeStamp(QDateTime(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("timeStamp inequality 1")
        << QMessageFilter::byTimeStamp(QDateTime::fromString("1999-04-01T10:30:00Z", Qt::ISODate), QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] );

    QTest::newRow("timeStamp inequality 2")
        << QMessageFilter::byTimeStamp(QDateTime::fromString("2000-01-01T13:00:00Z", Qt::ISODate), QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[4] )
        << ( QMessageIdList() << messageIds[3] << messageIds[2] );

    QTest::newRow("timeStamp inequality non-matching")
        << QMessageFilter::byTimeStamp(QDateTime::fromString("1990-01-01T01:00:00Z", Qt::ISODate), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("timeStamp inequality empty")
        << QMessageFilter::byTimeStamp(QDateTime(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QDateTime epoch(QDateTime::fromString("2000-01-01T12:00:00Z", Qt::ISODate));

    QTest::newRow("timeStamp less than")
        << QMessageFilter::byTimeStamp(epoch, QMessageDataComparator::LessThan) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("timeStamp less than equal")
        << QMessageFilter::byTimeStamp(epoch, QMessageDataComparator::LessThanEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[4] )
        << ( QMessageIdList() << messageIds[2] << messageIds[3] );

    QTest::newRow("timeStamp greater than")
        << QMessageFilter::byTimeStamp(epoch, QMessageDataComparator::GreaterThan) 
        << ( QMessageIdList() << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[4] );

    QTest::newRow("timeStamp greater than equal")
        << QMessageFilter::byTimeStamp(epoch, QMessageDataComparator::GreaterThanEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] );

    QTest::newRow("receptionTimeStamp equality 1")
        << QMessageFilter::byReceptionTimeStamp(QDateTime::fromString("1999-04-01T10:31:00Z", Qt::ISODate), QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("receptionTimeStamp equality 2")
        << QMessageFilter::byReceptionTimeStamp(QDateTime::fromString("2000-01-01T13:05:00Z", Qt::ISODate), QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] );

    QTest::newRow("receptionTimeStamp equality non-matching")
        << QMessageFilter::byReceptionTimeStamp(QDateTime::fromString("1990-01-01T01:00:00Z", Qt::ISODate), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("receptionTimeStamp equality empty")
        << QMessageFilter::byReceptionTimeStamp(QDateTime(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("receptionTimeStamp inequality 1")
        << QMessageFilter::byReceptionTimeStamp(QDateTime::fromString("1999-04-01T10:31:00Z", Qt::ISODate), QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] );

    QTest::newRow("receptionTimeStamp inequality 2")
        << QMessageFilter::byReceptionTimeStamp(QDateTime::fromString("2000-01-01T13:05:00Z", Qt::ISODate), QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[2] );

    QTest::newRow("receptionTimeStamp inequality non-matching")
        << QMessageFilter::byReceptionTimeStamp(QDateTime::fromString("1990-01-01T01:00:00Z", Qt::ISODate), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("receptionTimeStamp inequality empty")
        << QMessageFilter::byReceptionTimeStamp(QDateTime(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("receptionTimeStamp less than")
        << QMessageFilter::byReceptionTimeStamp(epoch, QMessageDataComparator::LessThan) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("receptionTimeStamp less than equal")
        << QMessageFilter::byReceptionTimeStamp(epoch, QMessageDataComparator::LessThanEqual) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("receptionTimeStamp greater than")
        << QMessageFilter::byReceptionTimeStamp(epoch, QMessageDataComparator::GreaterThan) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] );

    QTest::newRow("receptionTimeStamp greater than equal")
        << QMessageFilter::byReceptionTimeStamp(epoch, QMessageDataComparator::GreaterThanEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] );

    QTest::newRow("status equality 1")
        << QMessageFilter::byStatus(QMessage::Read, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] );

    QTest::newRow("status equality 2")
        << QMessageFilter::byStatus(QMessage::HasAttachments, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] );

    QTest::newRow("status equality 3")
        << QMessageFilter::byStatus(QMessage::Removed, QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("status inequality 1")
        << QMessageFilter::byStatus(QMessage::Read, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] );

    QTest::newRow("status inequality 2")
        << QMessageFilter::byStatus(QMessage::HasAttachments, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[3] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[4] );

    QTest::newRow("status inequality 3")
        << QMessageFilter::byStatus(QMessage::Removed, QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("status mask inclusion 1")
        << QMessageFilter::byStatus(QMessage::Read, QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] );

    QTest::newRow("status mask inclusion 2")
        << QMessageFilter::byStatus(QMessage::Read | QMessage::HasAttachments, QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("status mask inclusion 3")
        << QMessageFilter::byStatus(QMessage::Read | QMessage::Removed, QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("status mask inclusion empty")
        << QMessageFilter::byStatus(static_cast<QMessage::StatusFlags>(0), QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("status mask exclusion 1")
        << QMessageFilter::byStatus(QMessage::Read, QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] );

    QTest::newRow("status mask exclusion 2")
        << QMessageFilter::byStatus(QMessage::Read | QMessage::HasAttachments, QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("status mask exclusion 3")
        << QMessageFilter::byStatus(QMessage::Read | QMessage::Removed, QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] );

    QTest::newRow("status mask exclusion empty")
        << QMessageFilter::byStatus(static_cast<QMessage::StatusFlags>(0), QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("priority equality 1")
        << QMessageFilter::byPriority(QMessage::HighPriority, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] );

    QTest::newRow("priority equality 2")
        << QMessageFilter::byPriority(QMessage::LowPriority, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("priority equality 3")
        << QMessageFilter::byPriority(QMessage::NormalPriority, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[0] << messageIds[3] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[4] );

    QTest::newRow("priority inequality 1")
        << QMessageFilter::byPriority(QMessage::HighPriority, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] );

    QTest::newRow("priority inequality 2")
        << QMessageFilter::byPriority(QMessage::LowPriority, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] );

    QTest::newRow("priority inequality 3")
        << QMessageFilter::byPriority(QMessage::NormalPriority, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] );

    QTest::newRow("size equality 1")
        << QMessageFilter::bySize(1056, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[3] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[4] );

    QTest::newRow("size equality 2")
        << QMessageFilter::bySize(20480, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] );

    QTest::newRow("size equality non-matching")
        << QMessageFilter::bySize(13, QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("size equality zero")
        << QMessageFilter::bySize(0, QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("size inequality 1")
        << QMessageFilter::bySize(1056, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[4] )
        << ( QMessageIdList() << messageIds[3] );

    QTest::newRow("size inequality 2")
        << QMessageFilter::bySize(20480, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[2] );

    QTest::newRow("size inequality non-matching")
        << QMessageFilter::bySize(13, QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("size inequality empty")
        << QMessageFilter::bySize(0, QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    uint discriminator(10240);

    QTest::newRow("size less than")
        << QMessageFilter::bySize(discriminator, QMessageDataComparator::LessThan) 
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] );

    QTest::newRow("size less than equal")
        << QMessageFilter::bySize(discriminator, QMessageDataComparator::LessThanEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[2] );

    QTest::newRow("size greater than")
        << QMessageFilter::bySize(discriminator, QMessageDataComparator::GreaterThan) 
        << ( QMessageIdList() << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] );

    QTest::newRow("size greater than equal")
        << QMessageFilter::bySize(discriminator, QMessageDataComparator::GreaterThanEqual) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] );

    // Note: custom field negations do not work as expected!
    QTest::newRow("customField equality 1")
        << QMessageFilter::byCustomField("flagged", "true", QMessageDataComparator::Equal)
        << ( QMessageIdList() << messageIds[0] << messageIds[1] )
        //<< ( QMessageIdList() << messageIds[2] << messageIds[3] << messageIds[4] );
        << QMessageIdList();

    QTest::newRow("customField equality 2")
        << QMessageFilter::byCustomField("spam", "filter:yes", QMessageDataComparator::Equal)
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("customField equality empty")
        << QMessageFilter::byCustomField("spam", "", QMessageDataComparator::Equal)
        << QMessageIdList()
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("customField equality non-matching filter")
        << QMessageFilter::byCustomField("hello", "world", QMessageDataComparator::Equal)
        << QMessageIdList()
        << QMessageIdList();

    QTest::newRow("customField equality non-matching value")
        << QMessageFilter::byCustomField("spam", "spam spam spam", QMessageDataComparator::Equal)
        << QMessageIdList()
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("customField inequality 1")
        << QMessageFilter::byCustomField("flagged", "true", QMessageDataComparator::NotEqual)
        << QMessageIdList()
        << ( QMessageIdList() << messageIds[0] << messageIds[1] );

    QTest::newRow("customField inequality 2")
        << QMessageFilter::byCustomField("spam", "filter:yes", QMessageDataComparator::NotEqual)
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] );

    QTest::newRow("customField inequality empty")
        << QMessageFilter::byCustomField("spam", "", QMessageDataComparator::NotEqual)
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << QMessageIdList();

    QTest::newRow("customField inequality non-matching filter")
        << QMessageFilter::byCustomField("hello", "world", QMessageDataComparator::NotEqual)
        << QMessageIdList()
        << QMessageIdList();

    QTest::newRow("customField inequality non-matching value")
        << QMessageFilter::byCustomField("spam", "spam spam spam", QMessageDataComparator::NotEqual)
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << QMessageIdList();

    QTest::newRow("customField inclusion 1")
        << QMessageFilter::byCustomField("flagged", "ru", QMessageDataComparator::Includes)
        << ( QMessageIdList() << messageIds[0] << messageIds[1] )
        << QMessageIdList();

    QTest::newRow("customField inclusion 2")
        << QMessageFilter::byCustomField("spam", "filter", QMessageDataComparator::Includes)
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << QMessageIdList();

    QTest::newRow("customField inclusion empty")
        << QMessageFilter::byCustomField("spam", "", QMessageDataComparator::Includes)
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << QMessageIdList();

    QTest::newRow("customField inclusion non-matching filter")
        << QMessageFilter::byCustomField("hello", "world", QMessageDataComparator::Includes)
        << QMessageIdList()
        << QMessageIdList();

    QTest::newRow("customField inclusion non-matching value")
        << QMessageFilter::byCustomField("spam", "spam spam spam", QMessageDataComparator::Includes)
        << QMessageIdList()
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("customField exclusion 1")
        << QMessageFilter::byCustomField("flagged", "ru", QMessageDataComparator::Excludes)
        << QMessageIdList()
        << ( QMessageIdList() << messageIds[0] << messageIds[1] );

    QTest::newRow("customField exclusion 2")
        << QMessageFilter::byCustomField("spam", "filter", QMessageDataComparator::Excludes)
        << QMessageIdList()
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("customField exclusion empty")
        << QMessageFilter::byCustomField("spam", "", QMessageDataComparator::Excludes)
        << QMessageIdList()
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("customField exclusion non-matching filter")
        << QMessageFilter::byCustomField("hello", "world", QMessageDataComparator::Excludes)
        << QMessageIdList()
        << QMessageIdList();

    QTest::newRow("customField exclusion non-matching value")
        << QMessageFilter::byCustomField("spam", "spam spam spam", QMessageDataComparator::Excludes)
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << QMessageIdList();

    QTest::newRow("parentAccountId equality 1")
        << QMessageFilter::byParentAccountId(accountIds[0], QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("parentAccountId equality 2")
        << QMessageFilter::byParentAccountId(accountIds[2], QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("parentAccountId equality invalid")
        << QMessageFilter::byParentAccountId(QMessageAccountId(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("parentAccountId inequality 1")
        << QMessageFilter::byParentAccountId(accountIds[0], QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("parentAccountId inequality 2")
        << QMessageFilter::byParentAccountId(accountIds[2], QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("parentAccountId inequality invalid")
        << QMessageFilter::byParentAccountId(QMessageAccountId(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("parentAccountId filter inclusion 1")
        << QMessageFilter::byParentAccountId(QMessageAccountFilter::byName("Alter Ego", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("parentAccountId filter inclusion 2")
        << QMessageFilter::byParentAccountId(QMessageAccountFilter::byName("Work", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("parentAccountId filter inclusion 3")
        << QMessageFilter::byParentAccountId(QMessageAccountFilter::byName("o", QMessageDataComparator::Includes), QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("parentAccountId filter inclusion empty")
        << QMessageFilter::byParentAccountId(QMessageAccountFilter(), QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("parentAccountId filter inclusion non-matching")
        << QMessageFilter::byParentAccountId(QMessageAccountFilter::byName("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("parentAccountId filter exclusion 1")
        << QMessageFilter::byParentAccountId(QMessageAccountFilter::byName("Alter Ego", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("parentAccountId filter exclusion 2")
        << QMessageFilter::byParentAccountId(QMessageAccountFilter::byName("Work", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("parentAccountId filter exclusion 3")
        << QMessageFilter::byParentAccountId(QMessageAccountFilter::byName("o", QMessageDataComparator::Includes), QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("parentAccountId filter exclusion empty")
        << QMessageFilter::byParentAccountId(QMessageAccountFilter(), QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("parentAccountId filter exclusion non-matching")
        << QMessageFilter::byParentAccountId(QMessageAccountFilter::byName("NoneSuch"), QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList();


#ifdef QMESSAGING_OPTIONAL_FOLDER
    QTest::newRow("parentFolderId equality 1")
        << QMessageFilter::byParentFolderId(folderIds[0], QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("parentFolderId equality 2")
        << QMessageFilter::byParentFolderId(folderIds[1], QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] );

    QTest::newRow("parentFolderId equality invalid")
        << QMessageFilter::byParentFolderId(QMessageFolderId(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("parentFolderId inequality 1")
        << QMessageFilter::byParentFolderId(folderIds[0], QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("parentFolderId inequality 2")
        << QMessageFilter::byParentFolderId(folderIds[1], QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] );

    QTest::newRow("parentFolderId inequality invalid")
        << QMessageFilter::byParentFolderId(QMessageFolderId(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("parentFolderId filter inclusion 1")
        << QMessageFilter::byParentFolderId(QMessageFolderFilter::byPath("My messages", QMessageDataComparator::Equal), QMessageDataComparator::Includes)
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("parentFolderId filter inclusion 2")
        << QMessageFilter::byParentFolderId(QMessageFolderFilter::byPath("Inbox", QMessageDataComparator::Equal), QMessageDataComparator::Includes)
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] );

    QTest::newRow("parentFolderId filter inclusion 3")
        << QMessageFilter::byParentFolderId(QMessageFolderFilter::byPath("Inbox", QMessageDataComparator::Includes), QMessageDataComparator::Includes)
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("parentFolderId filter inclusion empty")
        << QMessageFilter::byParentFolderId(QMessageFolderFilter(), QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("parentFolderId filter inclusion non-matching")
        << QMessageFilter::byParentFolderId(QMessageFolderFilter::byPath("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("parentFolderId filter exclusion 1")
        << QMessageFilter::byParentFolderId(QMessageFolderFilter::byPath("My messages", QMessageDataComparator::Equal), QMessageDataComparator::Excludes)
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("parentFolderId filter exclusion 2")
        << QMessageFilter::byParentFolderId(QMessageFolderFilter::byPath("Inbox", QMessageDataComparator::Equal), QMessageDataComparator::Excludes)
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] );

    QTest::newRow("parentFolderId filter exclusion 3")
        << QMessageFilter::byParentFolderId(QMessageFolderFilter::byPath("Inbox", QMessageDataComparator::Includes), QMessageDataComparator::Excludes)
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("parentFolderId filter exclusion empty")
        << QMessageFilter::byParentFolderId(QMessageFolderFilter(), QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("parentFolderId filter exclusion non-matching")
        << QMessageFilter::byParentFolderId(QMessageFolderFilter::byPath("NoneSuch"), QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList();
    QTest::newRow("ancestorFolderIds inclusion 1")
        << QMessageFilter::byAncestorFolderIds(folderIds[1], QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] );

    QTest::newRow("ancestorFolderIds inclusion 2")
        << QMessageFilter::byAncestorFolderIds(folderIds[2], QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("ancestorFolderIds inclusion invalid")
        << QMessageFilter::byAncestorFolderIds(QMessageFolderId(), QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("ancestorFolderIds exclusion 1")
        << QMessageFilter::byAncestorFolderIds(folderIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[3] << messageIds[4] );

    QTest::newRow("ancestorFolderIds exclusion 2")
        << QMessageFilter::byAncestorFolderIds(folderIds[2], QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] );

    QTest::newRow("ancestorFolderIds exclusion invalid")
        << QMessageFilter::byAncestorFolderIds(QMessageFolderId(), QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("ancestorFolderIds filter inclusion 1")
        << QMessageFilter::byAncestorFolderIds(QMessageFolderFilter::byDisplayName("Inbox", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] );

    QTest::newRow("ancestorFolderIds filter inclusion 2")
        << QMessageFilter::byAncestorFolderIds(QMessageFolderFilter::byDisplayName("X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("ancestorFolderIds filter inclusion 3")
        << QMessageFilter::byAncestorFolderIds(QMessageFolderFilter::byPath("Inbox", QMessageDataComparator::Includes), QMessageDataComparator::Includes)
        << ( QMessageIdList() << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] );

    QTest::newRow("ancestorFolderIds filter inclusion empty")
        << QMessageFilter::byAncestorFolderIds(QMessageFolderFilter(), QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] );

    QTest::newRow("ancestorFolderIds filter inclusion non-matching")
        << QMessageFilter::byAncestorFolderIds(QMessageFolderFilter::byPath("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("ancestorFolderIds filter exclusion 1")
        << QMessageFilter::byAncestorFolderIds(QMessageFolderFilter::byDisplayName("Inbox", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[3] << messageIds[4] );

    QTest::newRow("ancestorFolderIds filter exclusion 2")
        << QMessageFilter::byAncestorFolderIds(QMessageFolderFilter::byDisplayName("X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] );

    QTest::newRow("ancestorFolderIds filter exclusion 3")
        << QMessageFilter::byAncestorFolderIds(QMessageFolderFilter::byPath("Inbox", QMessageDataComparator::Includes), QMessageDataComparator::Excludes)
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[3] << messageIds[4] );

    QTest::newRow("ancestorFolderIds filter exclusion empty")
        << QMessageFilter::byAncestorFolderIds(QMessageFolderFilter(), QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[3] << messageIds[4] );

    QTest::newRow("ancestorFolderIds filter exclusion non-matching")
        << QMessageFilter::byAncestorFolderIds(QMessageFolderFilter::byPath("NoneSuch"), QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList();
#endif
}

void tst_QMessageStoreKeys::testMessageFilter()
{
    QFETCH(QMessageFilter, filter);
    QFETCH(QMessageIdList, ids);
    QFETCH(QMessageIdList, negatedIds);

    if (filter.isSupported()) {
        // Order is irrelevant for filtering
        QCOMPARE(QMessageStore::instance()->queryMessages(filter).toSet().subtract(existingMessageIds), ids.toSet());
        QCOMPARE(QMessageStore::instance()->queryMessages(~filter).toSet().subtract(existingMessageIds), negatedIds.toSet());
    }
}

void tst_QMessageStoreKeys::testMessageOrdering_data()
{
    QTest::addColumn<QMessageOrdering>("ordering");
    QTest::addColumn<MessageListList>("ids");

    QTest::newRow("type ascending")
        << QMessageOrdering::byType(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] ) );

    QTest::newRow("type descending")
        << QMessageOrdering::byType(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] ) );

    QTest::newRow("sender ascending")
        << QMessageOrdering::bySender(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[2] << messageIds[4] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[3] ) );

    QTest::newRow("sender descending")
        << QMessageOrdering::bySender(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] << messageIds[4] ) );

    QTest::newRow("recipients ascending")
        << QMessageOrdering::byRecipients(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[1] << messageIds[2] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[3] << messageIds[4] ) );

    QTest::newRow("recipients descending")
        << QMessageOrdering::byRecipients(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[3] << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[1] << messageIds[2] ) );

    QTest::newRow("subject ascending")
        << QMessageOrdering::bySubject(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[3] ) );

    QTest::newRow("subject descending")
        << QMessageOrdering::bySubject(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[4] ) );

    QTest::newRow("timeStamp ascending")
        << QMessageOrdering::byTimeStamp(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] << messageIds[3] ) );

    QTest::newRow("timeStamp descending")
        << QMessageOrdering::byTimeStamp(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[2] << messageIds[3] )
                               << ( QMessageIdList() << messageIds[0] << messageIds[1] )
                               << ( QMessageIdList() << messageIds[4] ) );

    QTest::newRow("receptionTimeStamp ascending")
        << QMessageOrdering::byReceptionTimeStamp(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] ) );

    QTest::newRow("receptionTimeStamp descending")
        << QMessageOrdering::byReceptionTimeStamp(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[4] ) );

    QTest::newRow("priority ascending")
        << QMessageOrdering::byPriority(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] << messageIds[3] )
                               << ( QMessageIdList() << messageIds[1] << messageIds[2] ) );

    QTest::newRow("priority descending")
        << QMessageOrdering::byPriority(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[1] << messageIds[2] )
                               << ( QMessageIdList() << messageIds[0] << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] ) );

    QTest::newRow("size ascending")
        << QMessageOrdering::bySize(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] ) );

    QTest::newRow("size descending")
        << QMessageOrdering::bySize(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[0] ) );

    QTest::newRow("type ascending, priority ascending, size ascending")
        << QMessageOrdering::byType(Qt::AscendingOrder) + QMessageOrdering::byPriority(Qt::AscendingOrder) + QMessageOrdering::bySize(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] ) );

    QTest::newRow("type ascending, priority ascending, size descending")
        << QMessageOrdering::byType(Qt::AscendingOrder) + QMessageOrdering::byPriority(Qt::AscendingOrder) + QMessageOrdering::bySize(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] ) );

    QTest::newRow("type ascending, priority descending, size ascending")
        << QMessageOrdering::byType(Qt::AscendingOrder) + QMessageOrdering::byPriority(Qt::DescendingOrder) + QMessageOrdering::bySize(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] ) );

    QTest::newRow("type ascending, priority descending, size descending")
        << QMessageOrdering::byType(Qt::AscendingOrder) + QMessageOrdering::byPriority(Qt::DescendingOrder) + QMessageOrdering::bySize(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] ) );

    QTest::newRow("type descending, priority ascending, size ascending")
        << QMessageOrdering::byType(Qt::DescendingOrder) + QMessageOrdering::byPriority(Qt::AscendingOrder) + QMessageOrdering::bySize(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[0] ) );

    QTest::newRow("type descending, priority ascending, size descending")
        << QMessageOrdering::byType(Qt::DescendingOrder) + QMessageOrdering::byPriority(Qt::AscendingOrder) + QMessageOrdering::bySize(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[0] ) );

    QTest::newRow("type descending, priority descending, size ascending")
        << QMessageOrdering::byType(Qt::DescendingOrder) + QMessageOrdering::byPriority(Qt::DescendingOrder) + QMessageOrdering::bySize(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] ) );

    QTest::newRow("type descending, priority descending, size descending")
        << QMessageOrdering::byType(Qt::DescendingOrder) + QMessageOrdering::byPriority(Qt::DescendingOrder) + QMessageOrdering::bySize(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] ) );
}

void tst_QMessageStoreKeys::testMessageOrdering()
{
    QFETCH(QMessageOrdering, ordering);
    QFETCH(MessageListList, ids);

    // Filter out the existing messages
    QMessageIdList sortedIds(QMessageStore::instance()->queryMessages(QMessageFilter(), ordering));
    for (QMessageIdList::iterator it = sortedIds.begin(); it != sortedIds.end(); ) {
        if (existingMessageIds.contains(*it)) {
            it = sortedIds.erase(it);
        } else {
            ++it;
        }
    }

    QMessageIdList::const_iterator sit = sortedIds.begin();
    MessageListList::const_iterator iit = ids.begin();

    bool match(true);
    for (int groupCount = 0; sit != sortedIds.end(); ++sit) {
        if (iit == ids.end()) {
            match = false;
            break;
        } else {
            if ((*iit).contains(*sit)) {
                ++groupCount;
                if (groupCount == (*iit).count()) {
                    // We have matched all members of this group
                    ++iit;
                    groupCount = 0;
                }
            } else {
                match = false;
                break;
            }
        }
    }
    QVERIFY(match);
    QVERIFY(iit == ids.end());
}

