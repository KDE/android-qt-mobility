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

#include <QtTest/QtTest>

#include <QCoreApplication>
#include "qtcontacts.h"
#include "qcontactmanagerdataholder.h" //QContactManagerDataHolder

QTM_USE_NAMESPACE

class tst_QContactAsync : public QObject
{
    Q_OBJECT

public:
    tst_QContactAsync();
    virtual ~tst_QContactAsync();

public slots:
    void init();
    void cleanup();

private:
    void addManagers(); // add standard managers to the data

private slots:
    void testDestructor();
    void testDestructor_data() { addManagers(); }

    void contactFetch();
    void contactFetch_data() { addManagers(); }
    void contactIdFetch();
    void contactIdFetch_data() { addManagers(); }
    void contactRemove();
    void contactRemove_data() { addManagers(); }
    void contactSave();
    void contactSave_data() { addManagers(); }

    void definitionFetch();
    void definitionFetch_data() { addManagers(); }
    void definitionRemove();
    void definitionRemove_data() { addManagers(); }
    void definitionSave();
    void definitionSave_data() { addManagers(); }

    void relationshipFetch();
    void relationshipFetch_data() { addManagers(); }
    void relationshipRemove();
    void relationshipRemove_data() { addManagers(); }
    void relationshipSave();
    void relationshipSave_data() { addManagers(); }

    void maliciousManager(); // uses it's own custom data (manager)

    void threadDelivery();
    void progressReceived(QContactFetchRequest* request, bool appendOnly);
    void threadDelivery_data() { addManagers(); }

private:
    bool containsIgnoringTimestamps(const QList<QContact>& list, const QContact& c);
    bool compareIgnoringTimestamps(const QContact& ca, const QContact& cb);
    bool prepareModel(const QString &uri, QContactManager *&cm);
    bool prepareModel(const QString &uri, QContactManager *&cm, QList<QContact> &contacts, QList<QContactRelationship> &relationships);

    Qt::HANDLE m_mainThreadId;
    Qt::HANDLE m_progressSlotThreadId;
    QContactManagerDataHolder managerDataHolder;
};

typedef QList<QContactLocalId> QContactLocalIdList;
Q_DECLARE_METATYPE(QContactLocalIdList);

tst_QContactAsync::tst_QContactAsync()
{
    // ensure we can load all of the plugins we need to.
    QString path = QApplication::applicationDirPath() + "/dummyplugin/plugins";
    QApplication::addLibraryPath(path);

    qRegisterMetaType<QContactLocalIdList>("QList<QContactLocalId>");
}

tst_QContactAsync::~tst_QContactAsync()
{
    QString path = QApplication::applicationDirPath() + "/dummyplugin/plugins";
    QApplication::removeLibraryPath(path);
}

void tst_QContactAsync::init()
{
}

void tst_QContactAsync::cleanup()
{
}

bool tst_QContactAsync::containsIgnoringTimestamps(const QList<QContact>& list, const QContact& c)
{
    QList<QContact> cl = list;
    QContact a(c);
    for (int i = 0; i < cl.size(); i++) {
        QContact b(cl.at(i));
        if (compareIgnoringTimestamps(a, b))
            return true;
    }

    return false;
}

bool tst_QContactAsync::compareIgnoringTimestamps(const QContact& ca, const QContact& cb)
{
    // Compares two contacts, ignoring any timestamp details
    QContact a(ca);
    QContact b(cb);
    QList<QContactDetail> aDetails = a.details();
    QList<QContactDetail> bDetails = b.details();

    // They can be in any order, so loop
    // First remove any matches, and any timestamps
    foreach (QContactDetail d, aDetails) {
        foreach (QContactDetail d2, bDetails) {
            if (d == d2) {
                a.removeDetail(&d);
                b.removeDetail(&d2);
                break;
            }

            if (d.definitionName() == QContactTimestamp::DefinitionName) {
                a.removeDetail(&d);
            }

            if (d2.definitionName() == QContactTimestamp::DefinitionName) {
                b.removeDetail(&d2);
            }
        }
    }

    if (a == b)
        return true;
    return false;
}

void tst_QContactAsync::testDestructor()
{
    QFETCH(QString, uri);
    QContactManager* cm(0);
    QVERIFY(prepareModel(uri, cm));
    QContactFetchRequest* req = new QContactFetchRequest;
    req->setManager(cm);

    QContactManager* cm2(0);
    QVERIFY(prepareModel(uri, cm2));
    QContactFetchRequest* req2 = new QContactFetchRequest;
    req2->setManager(cm2);

    // first, delete manager then request
    delete cm;
    delete req;

    // second, delete request then manager
    delete req2;
    delete cm2;
}

void tst_QContactAsync::contactFetch()
{
    QFETCH(QString, uri);
    QContactManager* cm(0);
    QVERIFY(prepareModel(uri, cm));
    QContactFetchRequest cfr;
    QVERIFY(cfr.type() == QContactAbstractRequest::ContactFetchRequest);

    // initial state - not started, no manager.
    QVERIFY(!cfr.isActive());
    QVERIFY(!cfr.isFinished());
    QVERIFY(!cfr.start());
    QVERIFY(!cfr.cancel());
    QVERIFY(!cfr.waitForFinished());
    QVERIFY(!cfr.waitForProgress());

    // "all contacts" retrieval
    QContactFilter fil;
    cfr.setManager(cm);
    QCOMPARE(cfr.manager(), cm);
    QVERIFY(!cfr.isActive());
    QVERIFY(!cfr.isFinished());
    QVERIFY(!cfr.cancel());
    QVERIFY(!cfr.waitForFinished());
    QVERIFY(!cfr.waitForProgress());
    qRegisterMetaType<QContactFetchRequest*>("QContactFetchRequest*");
    QSignalSpy spy(&cfr, SIGNAL(progress(QContactFetchRequest*, bool)));
    cfr.setFilter(fil);
    QCOMPARE(cfr.filter(), fil);
    QVERIFY(!cfr.cancel()); // not started
    QVERIFY(cfr.start());
    QVERIFY(cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!cfr.isFinished());
    QVERIFY(!cfr.start());  // already started.
    QVERIFY(cfr.waitForFinished());
    int expectedCount = 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(cfr.isFinished());
    QVERIFY(!cfr.isActive());

    QList<QContactLocalId> contactIds = cm->contacts();
    QList<QContact> contacts = cfr.contacts();
    QCOMPARE(contactIds.size(), contacts.size());
    for (int i = 0; i < contactIds.size(); i++) {
        QContact curr = cm->contact(contactIds.at(i));
        QVERIFY(contacts.at(i) == curr);
    }

    // asynchronous detail filtering
    QContactDetailFilter dfil;
    dfil.setDetailDefinitionName(QContactUrl::DefinitionName, QContactUrl::FieldUrl);
    cfr.setFilter(dfil);
    QVERIFY(cfr.filter() == dfil);
    QVERIFY(!cfr.cancel()); // not started
    QVERIFY(cfr.start());
    QVERIFY(cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!cfr.isFinished());
    QVERIFY(!cfr.start());  // already started.
    QVERIFY(cfr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(cfr.isFinished());
    QVERIFY(!cfr.isActive());

    contactIds = cm->contacts(dfil);
    contacts = cfr.contacts();
    QCOMPARE(contactIds.size(), contacts.size());
    for (int i = 0; i < contactIds.size(); i++) {
        QContact curr = cm->contact(contactIds.at(i));
        QVERIFY(contacts.at(i) == curr);
    }

    // sort order
    QContactSortOrder sortOrder;
    sortOrder.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
    QList<QContactSortOrder> sorting;
    sorting.append(sortOrder);
    cfr.setFilter(fil);
    cfr.setSorting(sorting);
    QCOMPARE(cfr.sorting(), sorting);
    QVERIFY(!cfr.cancel()); // not started
    QVERIFY(cfr.start());
    QVERIFY(cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!cfr.isFinished());
    QVERIFY(!cfr.start());  // already started.
    QVERIFY(cfr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(cfr.isFinished());
    QVERIFY(!cfr.isActive());

    contactIds = cm->contacts(sorting);
    contacts = cfr.contacts();
    QCOMPARE(contactIds.size(), contacts.size());
    for (int i = 0; i < contactIds.size(); i++) {
        QContact curr = cm->contact(contactIds.at(i));
        QVERIFY(contacts.at(i) == curr);
    }

    // restrictions
    sorting.clear();
    cfr.setFilter(fil);
    cfr.setSorting(sorting);
    cfr.setDefinitionRestrictions(QStringList(QContactName::DefinitionName));
    QCOMPARE(cfr.definitionRestrictions(), QStringList(QContactName::DefinitionName));
    QVERIFY(!cfr.cancel()); // not started
    QVERIFY(cfr.start());
    QVERIFY(cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!cfr.isFinished());
    QVERIFY(!cfr.start());  // already started.
    QVERIFY(cfr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(cfr.isFinished());
    QVERIFY(!cfr.isActive());

    contactIds = cm->contacts(sorting);
    contacts = cfr.contacts();
    QCOMPARE(contactIds.size(), contacts.size());
    for (int i = 0; i < contactIds.size(); i++) {
        // create a contact from the restricted data only (id + display label)
        QContact currFull = cm->contact(contactIds.at(i));
        QContact currRestricted;
        currRestricted.setId(currFull.id());
        QList<QContactName> names = currFull.details<QContactName>();
        foreach (const QContactName& name, names) {
            QContactName fullName = name;
            if (!fullName.isEmpty()) {
                currRestricted.saveDetail(&fullName);
            }
        }

        // now find the contact in the retrieved list which our restricted contact mimics
        QContact retrievedRestricted;
        bool found = false;
        foreach (const QContact& retrieved, contacts) {
            if (retrieved.id() == currRestricted.id()) {
                retrievedRestricted = retrieved;
                found = true;
            }
        }

        QVERIFY(found); // must exist or fail.

        // ensure that the contact is the same (except synth fields)
        QList<QContactDetail> fdets = retrievedRestricted.details();
        QList<QContactDetail> rdets = currRestricted.details();
        foreach (const QContactDetail& det, fdets) {
            // ignore backend synthesised details
            // again, this requires a "default contact details" function to work properly.
            if (det.definitionName() == QContactDisplayLabel::DefinitionName
                || det.definitionName() == QContactTimestamp::DefinitionName) {
                continue;
            }

            // everything else must exist in both.
            if(!rdets.contains(det)) {
                qWarning("A detail exists in retrieved contact which doesn't exist in restricted contact!  This could be due to backend synthesization, or represent a bug!  (Definition name: %s)", det.definitionName().toAscii().constData());
            }
        }
    }

    // cancelling
    sorting.clear();
    cfr.setFilter(fil);
    cfr.setSorting(sorting);
    cfr.setDefinitionRestrictions(QStringList());
    QVERIFY(!cfr.cancel()); // not started
    QVERIFY(cfr.start());
    QVERIFY(cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!cfr.isFinished());
    QVERIFY(cfr.cancel());
    QVERIFY(cfr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(cfr.isActive());    // still cancelling
    QVERIFY(!cfr.isFinished()); // not finished cancelling
    QVERIFY(!cfr.start());      // already started.
    QVERIFY(cfr.waitForFinished());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(cfr.isFinished());
    QVERIFY(!cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Cancelled);

    // restart, and wait for progress after cancel.
    QVERIFY(!cfr.cancel()); // not started
    QVERIFY(cfr.start());
    QVERIFY(cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!cfr.isFinished());
    QVERIFY(cfr.cancel());
    QVERIFY(cfr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(cfr.isActive());    // still cancelling
    QVERIFY(!cfr.isFinished()); // not finished cancelling
    QVERIFY(!cfr.start());      // already started.
    QVERIFY(cfr.waitForProgress());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(cfr.isFinished());
    QVERIFY(!cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Cancelled);

    delete cm;
}

void tst_QContactAsync::contactIdFetch()
{
    QFETCH(QString, uri);
    QContactManager* cm(0);
    QVERIFY(prepareModel(uri, cm));
    QContactLocalIdFetchRequest cfr;
    QVERIFY(cfr.type() == QContactAbstractRequest::ContactLocalIdFetchRequest);

    // initial state - not started, no manager.
    QVERIFY(!cfr.isActive());
    QVERIFY(!cfr.isFinished());
    QVERIFY(!cfr.start());
    QVERIFY(!cfr.cancel());
    QVERIFY(!cfr.waitForFinished());
    QVERIFY(!cfr.waitForProgress());

    // "all contacts" retrieval
    QContactFilter fil;
    cfr.setManager(cm);
    QCOMPARE(cfr.manager(), cm);
    QVERIFY(!cfr.isActive());
    QVERIFY(!cfr.isFinished());
    QVERIFY(!cfr.cancel());
    QVERIFY(!cfr.waitForFinished());
    QVERIFY(!cfr.waitForProgress());
    qRegisterMetaType<QContactLocalIdFetchRequest*>("QContactLocalIdFetchRequest*");
    QSignalSpy spy(&cfr, SIGNAL(progress(QContactLocalIdFetchRequest*, bool)));
    cfr.setFilter(fil);
    QCOMPARE(cfr.filter(), fil);
    QVERIFY(!cfr.cancel()); // not started
    QVERIFY(cfr.start());
    QVERIFY(cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!cfr.isFinished());
    QVERIFY(!cfr.start());  // already started.
    QVERIFY(cfr.waitForFinished());
    int expectedCount = 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(cfr.isFinished());
    QVERIFY(!cfr.isActive());

    QList<QContactLocalId> contactIds = cm->contacts();
    QList<QContactLocalId> result = cfr.ids();
    QCOMPARE(contactIds, result);

    // asynchronous detail filtering
    QContactDetailFilter dfil;
    dfil.setDetailDefinitionName(QContactUrl::DefinitionName, QContactUrl::FieldUrl);
    cfr.setFilter(dfil);
    QVERIFY(cfr.filter() == dfil);
    QVERIFY(!cfr.cancel()); // not started
    QVERIFY(cfr.start());
    QVERIFY(cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!cfr.isFinished());
    QVERIFY(!cfr.start());  // already started.
    QVERIFY(cfr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(cfr.isFinished());
    QVERIFY(!cfr.isActive());

    contactIds = cm->contacts(dfil);
    result = cfr.ids();
    QCOMPARE(contactIds, result);

    // sort order
    QContactSortOrder sortOrder;
    sortOrder.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
    QList<QContactSortOrder> sorting;
    sorting.append(sortOrder);
    cfr.setFilter(fil);
    cfr.setSorting(sorting);
    QCOMPARE(cfr.sorting(), sorting);
    QVERIFY(!cfr.cancel()); // not started
    QVERIFY(cfr.start());
    QVERIFY(cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!cfr.isFinished());
    QVERIFY(!cfr.start());  // already started.
    QVERIFY(cfr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(cfr.isFinished());
    QVERIFY(!cfr.isActive());

    contactIds = cm->contacts(sorting);
    result = cfr.ids();
    QCOMPARE(contactIds, result);

    // cancelling
    sorting.clear();
    cfr.setFilter(fil);
    cfr.setSorting(sorting);
    QVERIFY(!cfr.cancel()); // not started
    QVERIFY(cfr.start());
    QVERIFY(cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!cfr.isFinished());
    QVERIFY(cfr.cancel());
    QVERIFY(cfr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(cfr.isActive());    // still cancelling
    QVERIFY(!cfr.isFinished()); // not finished cancelling
    QVERIFY(!cfr.start());      // already started.
    QVERIFY(cfr.waitForFinished());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(cfr.isFinished());
    QVERIFY(!cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Cancelled);

    // restart, and wait for progress after cancel.
    QVERIFY(!cfr.cancel()); // not started
    QVERIFY(cfr.start());
    QVERIFY(cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!cfr.isFinished());
    QVERIFY(cfr.cancel());
    QVERIFY(cfr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(cfr.isActive());    // still cancelling
    QVERIFY(!cfr.isFinished()); // not finished cancelling
    QVERIFY(!cfr.start());      // already started.
    QVERIFY(cfr.waitForProgress());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(cfr.isFinished());
    QVERIFY(!cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Cancelled);

    delete cm;
}

void tst_QContactAsync::contactRemove()
{
    QFETCH(QString, uri);
    QContactManager* cm(0);
    QVERIFY(prepareModel(uri, cm));
    QContactRemoveRequest crr;
    QVERIFY(crr.type() == QContactAbstractRequest::ContactRemoveRequest);

    // initial state - not started, no manager.
    QVERIFY(!crr.isActive());
    QVERIFY(!crr.isFinished());
    QVERIFY(!crr.start());
    QVERIFY(!crr.cancel());
    QVERIFY(!crr.waitForFinished());
    QVERIFY(!crr.waitForProgress());

    // specific contact removal
    int originalCount = cm->contacts().size();
    QContactDetailFilter dfil;
    dfil.setDetailDefinitionName(QContactUrl::DefinitionName, QContactUrl::FieldUrl);
    crr.setFilter(dfil);
    crr.setManager(cm);
    QCOMPARE(crr.manager(), cm);
    QVERIFY(!crr.isActive());
    QVERIFY(!crr.isFinished());
    QVERIFY(!crr.cancel());
    QVERIFY(!crr.waitForFinished());
    QVERIFY(!crr.waitForProgress());
    qRegisterMetaType<QContactRemoveRequest*>("QContactRemoveRequest*");
    QSignalSpy spy(&crr, SIGNAL(progress(QContactRemoveRequest*)));
    QVERIFY(crr.filter() == dfil);
    QVERIFY(!crr.cancel()); // not started
    QVERIFY(crr.start());
    QVERIFY(crr.isActive());
    QVERIFY(crr.status() == QContactAbstractRequest::Active);
    QVERIFY(!crr.isFinished());
    QVERIFY(!crr.start());  // already started.
    QVERIFY(crr.waitForFinished());
    int expectedCount = 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(crr.isFinished());
    QVERIFY(!crr.isActive());

    QCOMPARE(cm->contacts().size(), originalCount - 1);
    QVERIFY(cm->contacts(dfil).isEmpty());

    // remove all contacts
    dfil.setDetailDefinitionName(QContactDisplayLabel::DefinitionName); // delete everything.
    crr.setFilter(dfil);
    QVERIFY(crr.filter() == dfil);
    QVERIFY(!crr.cancel()); // not started
    QVERIFY(crr.start());
    QVERIFY(crr.isActive());
    QVERIFY(crr.status() == QContactAbstractRequest::Active);
    QVERIFY(!crr.isFinished());
    QVERIFY(!crr.start());  // already started.
    QVERIFY(crr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(crr.isFinished());
    QVERIFY(!crr.isActive());

    QCOMPARE(cm->contacts().size(), 0); // no contacts should be left.

    // cancelling
    QContact temp;
    QContactName nameDetail;
    nameDetail.setFirst("Should not be removed");
    temp.saveDetail(&nameDetail);
    cm->saveContact(&temp);
    crr.setFilter(dfil);
    QVERIFY(!crr.cancel()); // not started
    QVERIFY(crr.start());
    QVERIFY(crr.isActive());
    QVERIFY(crr.status() == QContactAbstractRequest::Active);
    QVERIFY(!crr.isFinished());
    QVERIFY(crr.cancel());
    QVERIFY(crr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(crr.isActive());    // still cancelling
    QVERIFY(!crr.isFinished()); // not finished cancelling
    QVERIFY(!crr.start());      // already started.
    QVERIFY(crr.waitForFinished());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(crr.isFinished());
    QVERIFY(!crr.isActive());
    QVERIFY(crr.status() == QContactAbstractRequest::Cancelled);

    QCOMPARE(cm->contacts().size(), 1);
    QCOMPARE(cm->contact(cm->contacts().first()), temp);

    // restart, and wait for progress after cancel.
    QVERIFY(!crr.cancel()); // not started
    QVERIFY(crr.start());
    QVERIFY(crr.isActive());
    QVERIFY(crr.status() == QContactAbstractRequest::Active);
    QVERIFY(!crr.isFinished());
    QVERIFY(crr.cancel());
    QVERIFY(crr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(crr.isActive());    // still cancelling
    QVERIFY(!crr.isFinished()); // not finished cancelling
    QVERIFY(!crr.start());      // already started.
    QVERIFY(crr.waitForProgress());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(crr.isFinished());
    QVERIFY(!crr.isActive());
    QVERIFY(crr.status() == QContactAbstractRequest::Cancelled);

    QCOMPARE(cm->contacts().size(), 1);
    QCOMPARE(cm->contact(cm->contacts().first()), temp);
    cm->removeContact(temp.localId()); // clean up

    delete cm;
}

void tst_QContactAsync::contactSave()
{
    QFETCH(QString, uri);
    QContactManager* cm(0);
    QVERIFY(prepareModel(uri, cm));
    QContactSaveRequest csr;
    QVERIFY(csr.type() == QContactAbstractRequest::ContactSaveRequest);

    // initial state - not started, no manager.
    QVERIFY(!csr.isActive());
    QVERIFY(!csr.isFinished());
    QVERIFY(!csr.start());
    QVERIFY(!csr.cancel());
    QVERIFY(!csr.waitForFinished());
    QVERIFY(!csr.waitForProgress());

    // save a new contact
    int originalCount = cm->contacts().size();
    QContact testContact;
    QContactName nameDetail;
    nameDetail.setFirst("Test Contact");
    testContact.saveDetail(&nameDetail);
    QList<QContact> saveList;
    saveList << testContact;
    csr.setManager(cm);
    QCOMPARE(csr.manager(), cm);
    QVERIFY(!csr.isActive());
    QVERIFY(!csr.isFinished());
    QVERIFY(!csr.cancel());
    QVERIFY(!csr.waitForFinished());
    QVERIFY(!csr.waitForProgress());
    qRegisterMetaType<QContactSaveRequest*>("QContactSaveRequest*");
    QSignalSpy spy(&csr, SIGNAL(progress(QContactSaveRequest*)));
    csr.setContacts(saveList);
    QCOMPARE(csr.contacts(), saveList);
    QVERIFY(!csr.cancel()); // not started
    QVERIFY(csr.start());
    QVERIFY(csr.isActive());
    QVERIFY(csr.status() == QContactAbstractRequest::Active);
    QVERIFY(!csr.isFinished());
    QVERIFY(!csr.start());  // already started.
    QVERIFY(csr.waitForFinished());
    int expectedCount = 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(csr.isFinished());
    QVERIFY(!csr.isActive());

    QList<QContact> expected;
    expected << cm->contact(cm->contacts().last());
    QList<QContact> result = csr.contacts();
    QCOMPARE(expected, result);
    QCOMPARE(cm->contacts().size(), originalCount + 1);

    // update a previously saved contact
    QContactPhoneNumber phn;
    phn.setNumber("12345678");
    testContact = expected.first();
    testContact.saveDetail(&phn);
    saveList.clear();
    saveList << testContact;
    csr.setContacts(saveList);
    QCOMPARE(csr.contacts(), saveList);
    QVERIFY(!csr.cancel()); // not started
    QVERIFY(csr.start());
    QVERIFY(csr.isActive());
    QVERIFY(csr.status() == QContactAbstractRequest::Active);
    QVERIFY(!csr.isFinished());
    QVERIFY(!csr.start());  // already started.
    QVERIFY(csr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(csr.isFinished());
    QVERIFY(!csr.isActive());

    expected.clear();
    expected << cm->contact(cm->contacts().last());
    result = csr.contacts();
    QCOMPARE(expected, result);

    //here we can't compare the whole contact details, testContact would be updated by async call because we just use QSignalSpy to receive signals.
    //QVERIFY(containsIgnoringTimestamps(expected, testContact));
    QVERIFY(expected.at(0).detail<QContactPhoneNumber>().number() == phn.number());
    
    QCOMPARE(cm->contacts().size(), originalCount + 1);

    // cancelling
    QContact temp = testContact;
    QContactUrl url;
    url.setUrl("should not get saved");
    temp.saveDetail(&url);
    saveList.clear();
    saveList << temp;
    csr.setContacts(saveList);
    QVERIFY(!csr.cancel()); // not started
    QVERIFY(csr.start());
    QVERIFY(csr.isActive());
    QVERIFY(csr.status() == QContactAbstractRequest::Active);
    QVERIFY(!csr.isFinished());
    QVERIFY(csr.cancel());
    QVERIFY(csr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(csr.isActive());    // still cancelling
    QVERIFY(!csr.isFinished()); // not finished cancelling
    QVERIFY(!csr.start());      // already started.
    QVERIFY(csr.waitForFinished());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(csr.isFinished());
    QVERIFY(!csr.isActive());
    QVERIFY(csr.status() == QContactAbstractRequest::Cancelled);

    // verify that the changes were not saved
    expected.clear();
    QList<QContactLocalId> allContacts = cm->contacts();
    for (int i = 0; i < allContacts.size(); i++) {
        expected.append(cm->contact(allContacts.at(i)));
    }
    QVERIFY(!expected.contains(temp));
    QCOMPARE(cm->contacts().size(), originalCount + 1);

    // restart, and wait for progress after cancel.
    QVERIFY(!csr.cancel()); // not started
    QVERIFY(csr.start());
    QVERIFY(csr.isActive());
    QVERIFY(csr.status() == QContactAbstractRequest::Active);
    QVERIFY(!csr.isFinished());
    QVERIFY(csr.cancel());
    QVERIFY(csr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(csr.isActive());    // still cancelling
    QVERIFY(!csr.isFinished()); // not finished cancelling
    QVERIFY(!csr.start());      // already started.
    QVERIFY(csr.waitForProgress());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(csr.isFinished());
    QVERIFY(!csr.isActive());
    QVERIFY(csr.status() == QContactAbstractRequest::Cancelled);

    // verify that the changes were not saved
    expected.clear();
    allContacts = cm->contacts();
    for (int i = 0; i < allContacts.size(); i++) {
        expected.append(cm->contact(allContacts.at(i)));
    }
    QVERIFY(!expected.contains(temp));
    QCOMPARE(cm->contacts().size(), originalCount + 1);

    delete cm;
}

void tst_QContactAsync::definitionFetch()
{
    QFETCH(QString, uri);
    QContactManager* cm(0);
    QVERIFY(prepareModel(uri, cm));
    QContactDetailDefinitionFetchRequest dfr;
    QVERIFY(dfr.type() == QContactAbstractRequest::DetailDefinitionFetchRequest);
    dfr.setContactType(QContactType::TypeContact);
    QVERIFY(dfr.contactType() == QString(QLatin1String(QContactType::TypeContact)));

    // initial state - not started, no manager.
    QVERIFY(!dfr.isActive());
    QVERIFY(!dfr.isFinished());
    QVERIFY(!dfr.start());
    QVERIFY(!dfr.cancel());
    QVERIFY(!dfr.waitForFinished());
    QVERIFY(!dfr.waitForProgress());

    // "all definitions" retrieval
    dfr.setManager(cm);
    QCOMPARE(dfr.manager(), cm);
    QVERIFY(!dfr.isActive());
    QVERIFY(!dfr.isFinished());
    QVERIFY(!dfr.cancel());
    QVERIFY(!dfr.waitForFinished());
    QVERIFY(!dfr.waitForProgress());
    qRegisterMetaType<QContactDetailDefinitionFetchRequest*>("QContactDetailDefinitionFetchRequest*");
    QSignalSpy spy(&dfr, SIGNAL(progress(QContactDetailDefinitionFetchRequest*, bool)));
    dfr.setNames(QStringList());
    QVERIFY(!dfr.cancel()); // not started
    QVERIFY(dfr.start());
    QVERIFY(dfr.isActive());
    QVERIFY(dfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!dfr.isFinished());
    QVERIFY(!dfr.start());  // already started.
    QVERIFY(dfr.waitForFinished());
    int expectedCount = 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(dfr.isFinished());
    QVERIFY(!dfr.isActive());

    QMap<QString, QContactDetailDefinition> defs = cm->detailDefinitions();
    QMap<QString, QContactDetailDefinition> result = dfr.definitions();
    QCOMPARE(defs, result);

    // specific definition retrieval
    QStringList specific;
    specific << QContactUrl::DefinitionName;
    dfr.setNames(specific);
    QVERIFY(!dfr.cancel()); // not started
    QVERIFY(dfr.start());
    QVERIFY(dfr.isActive());
    QVERIFY(dfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!dfr.isFinished());
    QVERIFY(!dfr.start());  // already started.
    QVERIFY(dfr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(dfr.isFinished());
    QVERIFY(!dfr.isActive());

    defs.clear();
    defs.insert(QContactUrl::DefinitionName, cm->detailDefinition(QContactUrl::DefinitionName));
    result = dfr.definitions();
    QCOMPARE(defs, result);

    // cancelling
    dfr.setNames(QStringList());
    QVERIFY(!dfr.cancel()); // not started
    QVERIFY(dfr.start());
    QVERIFY(dfr.isActive());
    QVERIFY(dfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!dfr.isFinished());
    QVERIFY(dfr.cancel());
    QVERIFY(dfr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(dfr.isActive());    // still cancelling
    QVERIFY(!dfr.isFinished()); // not finished cancelling
    QVERIFY(!dfr.start());      // already started.
    QVERIFY(dfr.waitForFinished());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(dfr.isFinished());
    QVERIFY(!dfr.isActive());
    QVERIFY(dfr.status() == QContactAbstractRequest::Cancelled);

    // restart, and wait for progress after cancel.
    QVERIFY(!dfr.cancel()); // not started
    QVERIFY(dfr.start());
    QVERIFY(dfr.isActive());
    QVERIFY(dfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!dfr.isFinished());
    QVERIFY(dfr.cancel());
    QVERIFY(dfr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(dfr.isActive());    // still cancelling
    QVERIFY(!dfr.isFinished()); // not finished cancelling
    QVERIFY(!dfr.start());      // already started.
    QVERIFY(dfr.waitForProgress());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(dfr.isFinished());
    QVERIFY(!dfr.isActive());
    QVERIFY(dfr.status() == QContactAbstractRequest::Cancelled);

    delete cm;
}

void tst_QContactAsync::definitionRemove()
{
    QFETCH(QString, uri);
    QContactManager* cm(0);
    QVERIFY(prepareModel(uri, cm));
    if (!cm->hasFeature(QContactManager::MutableDefinitions, QContactType::TypeContact)) {
       QSKIP("This contact manager doest not support mutable definitions, can't remove a definition!", SkipSingle);
    }
    QContactDetailDefinitionRemoveRequest drr;
    QVERIFY(drr.type() == QContactAbstractRequest::DetailDefinitionRemoveRequest);
    drr.setContactType(QContactType::TypeContact);
    QVERIFY(drr.contactType() == QString(QLatin1String(QContactType::TypeContact)));

    // initial state - not started, no manager.
    QVERIFY(!drr.isActive());
    QVERIFY(!drr.isFinished());
    QVERIFY(!drr.start());
    QVERIFY(!drr.cancel());
    QVERIFY(!drr.waitForFinished());
    QVERIFY(!drr.waitForProgress());

    // specific group removal
    int originalCount = cm->detailDefinitions().keys().size();
    QStringList removeIds;
    removeIds << cm->detailDefinitions().keys().first();
    drr.setNames(removeIds);
    drr.setManager(cm);
    QCOMPARE(drr.manager(), cm);
    QVERIFY(!drr.isActive());
    QVERIFY(!drr.isFinished());
    QVERIFY(!drr.cancel());
    QVERIFY(!drr.waitForFinished());
    QVERIFY(!drr.waitForProgress());
    qRegisterMetaType<QContactDetailDefinitionRemoveRequest*>("QContactDetailDefinitionRemoveRequest*");
    QSignalSpy spy(&drr, SIGNAL(progress(QContactDetailDefinitionRemoveRequest*)));
    QVERIFY(drr.names() == removeIds);
    QVERIFY(!drr.cancel()); // not started
    QVERIFY(drr.start());
    QVERIFY(drr.isActive());
    QVERIFY(drr.status() == QContactAbstractRequest::Active);
    QVERIFY(!drr.isFinished());
    QVERIFY(!drr.start());  // already started.
    QVERIFY(drr.waitForFinished());
    int expectedCount = 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(drr.isFinished());
    QVERIFY(!drr.isActive());

    QCOMPARE(cm->detailDefinitions().keys().size(), originalCount - 1);
    cm->detailDefinition(removeIds.first()); // check that it has already been removed.
    QCOMPARE(cm->error(), QContactManager::DoesNotExistError);

    // remove (asynchronously) a nonexistent group - should fail.
    drr.setNames(removeIds);
    QVERIFY(!drr.cancel()); // not started
    QVERIFY(drr.start());
    QVERIFY(drr.isActive());
    QVERIFY(drr.status() == QContactAbstractRequest::Active);
    QVERIFY(!drr.isFinished());
    QVERIFY(!drr.start());  // already started.
    QVERIFY(drr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(drr.isFinished());
    QVERIFY(!drr.isActive());

    QCOMPARE(cm->detailDefinitions().keys().size(), originalCount - 1); // hasn't changed
    QCOMPARE(drr.error(), QContactManager::DoesNotExistError);

    // remove with list containing one valid and one invalid id.
    removeIds << cm->detailDefinitions().keys().first();
    drr.setNames(removeIds);
    QVERIFY(!drr.cancel()); // not started
    QVERIFY(drr.start());
    QVERIFY(drr.isActive());
    QVERIFY(drr.status() == QContactAbstractRequest::Active);
    QVERIFY(!drr.isFinished());
    QVERIFY(!drr.start());  // already started.
    QVERIFY(drr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(drr.isFinished());
    QVERIFY(!drr.isActive());

    QCOMPARE(cm->detailDefinitions().keys().size(), originalCount - 2); // only one more has been removed
    QCOMPARE(drr.errors().first(), QContactManager::DoesNotExistError);
    QCOMPARE(drr.errors().at(1), QContactManager::NoError);

    // remove with empty list - nothing should happen.
    removeIds.clear();
    drr.setNames(removeIds);
    QVERIFY(!drr.cancel()); // not started
    QVERIFY(drr.start());
    QVERIFY(drr.isActive());
    QVERIFY(drr.status() == QContactAbstractRequest::Active);
    QVERIFY(!drr.isFinished());
    QVERIFY(!drr.start());  // already started.
    QVERIFY(drr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(drr.isFinished());
    QVERIFY(!drr.isActive());

    QCOMPARE(cm->detailDefinitions().keys().size(), originalCount - 2); // hasn't changed
    QCOMPARE(drr.error(), QContactManager::NoError);  // no error but no effect.

    // cancelling
    removeIds.clear();
    removeIds << cm->detailDefinitions().keys().first();
    drr.setNames(removeIds);
    QVERIFY(!drr.cancel()); // not started
    QVERIFY(drr.start());
    QVERIFY(drr.isActive());
    QVERIFY(drr.status() == QContactAbstractRequest::Active);
    QVERIFY(!drr.isFinished());
    QVERIFY(drr.cancel());
    QVERIFY(drr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(drr.isActive());    // still cancelling
    QVERIFY(!drr.isFinished()); // not finished cancelling
    QVERIFY(!drr.start());      // already started.
    QVERIFY(drr.waitForFinished());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(drr.isFinished());
    QVERIFY(!drr.isActive());
    QVERIFY(drr.status() == QContactAbstractRequest::Cancelled);

    QCOMPARE(cm->detailDefinitions().keys().size(), originalCount - 2); // hasn't changed

    // restart, and wait for progress after cancel.
    QVERIFY(!drr.cancel()); // not started
    QVERIFY(drr.start());
    QVERIFY(drr.isActive());
    QVERIFY(drr.status() == QContactAbstractRequest::Active);
    QVERIFY(!drr.isFinished());
    QVERIFY(drr.cancel());
    QVERIFY(drr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(drr.isActive());    // still cancelling
    QVERIFY(!drr.isFinished()); // not finished cancelling
    QVERIFY(!drr.start());      // already started.
    QVERIFY(drr.waitForProgress());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(drr.isFinished());
    QVERIFY(!drr.isActive());
    QVERIFY(drr.status() == QContactAbstractRequest::Cancelled);

    QCOMPARE(cm->detailDefinitions().keys().size(), originalCount - 2); // hasn't changed

    delete cm;
}

void tst_QContactAsync::definitionSave()
{
    QFETCH(QString, uri);
    QContactManager* cm(0);
    QVERIFY(prepareModel(uri, cm));
    
    if (!cm->hasFeature(QContactManager::MutableDefinitions, QContactType::TypeContact)) {
       QSKIP("This contact manager doest not support mutable definitions, can't save a definition!", SkipSingle);
    }
    
    QContactDetailDefinitionSaveRequest dsr;
    QVERIFY(dsr.type() == QContactAbstractRequest::DetailDefinitionSaveRequest);
    dsr.setContactType(QContactType::TypeContact);
    QVERIFY(dsr.contactType() == QString(QLatin1String(QContactType::TypeContact)));

    // initial state - not started, no manager.
    QVERIFY(!dsr.isActive());
    QVERIFY(!dsr.isFinished());
    QVERIFY(!dsr.start());
    QVERIFY(!dsr.cancel());
    QVERIFY(!dsr.waitForFinished());
    QVERIFY(!dsr.waitForProgress());

    // save a new detail definition
    int originalCount = cm->detailDefinitions().keys().size();
    QContactDetailDefinition testDef;
    testDef.setName("TestDefinitionId");
    QMap<QString, QContactDetailDefinitionField> fields;
    QContactDetailDefinitionField f;
    f.setDataType(QVariant::String);
    fields.insert("TestDefinitionField", f);
    testDef.setFields(fields);
    QList<QContactDetailDefinition> saveList;
    saveList << testDef;
    dsr.setManager(cm);
    QCOMPARE(dsr.manager(), cm);
    QVERIFY(!dsr.isActive());
    QVERIFY(!dsr.isFinished());
    QVERIFY(!dsr.cancel());
    QVERIFY(!dsr.waitForFinished());
    QVERIFY(!dsr.waitForProgress());
    qRegisterMetaType<QContactDetailDefinitionSaveRequest*>("QContactDetailDefinitionSaveRequest*");
    QSignalSpy spy(&dsr, SIGNAL(progress(QContactDetailDefinitionSaveRequest*)));
    dsr.setDefinitions(saveList);
    QCOMPARE(dsr.definitions(), saveList);
    QVERIFY(!dsr.cancel()); // not started
    QVERIFY(dsr.start());
    QVERIFY(dsr.isActive());
    QVERIFY(dsr.status() == QContactAbstractRequest::Active);
    QVERIFY(!dsr.isFinished());
    QVERIFY(!dsr.start());  // already started.
    QVERIFY(dsr.waitForFinished());
    int expectedCount = 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(dsr.isFinished());
    QVERIFY(!dsr.isActive());

    QList<QContactDetailDefinition> expected;
    expected << cm->detailDefinition("TestDefinitionId");
    QList<QContactDetailDefinition> result = dsr.definitions();
    QCOMPARE(expected, result);
    QVERIFY(expected.contains(testDef));
    QCOMPARE(cm->detailDefinitions().values().size(), originalCount + 1);

    // update a previously saved group
    fields.insert("TestDefinitionFieldTwo", f);
    testDef.setFields(fields);
    saveList.clear();
    saveList << testDef;
    dsr.setDefinitions(saveList);
    QCOMPARE(dsr.definitions(), saveList);
    QVERIFY(!dsr.cancel()); // not started
    QVERIFY(dsr.start());
    QVERIFY(dsr.isActive());
    QVERIFY(dsr.status() == QContactAbstractRequest::Active);
    QVERIFY(!dsr.isFinished());
    QVERIFY(!dsr.start());  // already started.
    QVERIFY(dsr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(dsr.isFinished());
    QVERIFY(!dsr.isActive());

    expected.clear();
    expected << cm->detailDefinition("TestDefinitionId");
    result = dsr.definitions();
    QCOMPARE(expected, result);
    QVERIFY(expected.contains(testDef));
    QCOMPARE(cm->detailDefinitions().values().size(), originalCount + 1);

    // cancelling
    fields.insert("TestDefinitionFieldThree - shouldn't get saved", f);
    testDef.setFields(fields);
    saveList.clear();
    saveList << testDef;
    dsr.setDefinitions(saveList);
    QCOMPARE(dsr.definitions(), saveList);
    QVERIFY(!dsr.cancel()); // not started
    QVERIFY(dsr.start());
    QVERIFY(dsr.isActive());
    QVERIFY(dsr.status() == QContactAbstractRequest::Active);
    QVERIFY(!dsr.isFinished());
    QVERIFY(dsr.cancel());
    QVERIFY(dsr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(dsr.isActive());    // still cancelling
    QVERIFY(!dsr.isFinished()); // not finished cancelling
    QVERIFY(!dsr.start());      // already started.
    QVERIFY(dsr.waitForFinished());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(dsr.isFinished());
    QVERIFY(!dsr.isActive());
    QVERIFY(dsr.status() == QContactAbstractRequest::Cancelled);

    // verify that the changes were not saved
    QList<QContactDetailDefinition> allDefs = cm->detailDefinitions().values();
    QVERIFY(!allDefs.contains(testDef));
    QCOMPARE(cm->detailDefinitions().values().size(), originalCount + 1);

    // restart, and wait for progress after cancel.
    QVERIFY(!dsr.cancel()); // not started
    QVERIFY(dsr.start());
    QVERIFY(dsr.isActive());
    QVERIFY(dsr.status() == QContactAbstractRequest::Active);
    QVERIFY(!dsr.isFinished());
    QVERIFY(dsr.cancel());
    QVERIFY(dsr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(dsr.isActive());    // still cancelling
    QVERIFY(!dsr.isFinished()); // not finished cancelling
    QVERIFY(!dsr.start());      // already started.
    QVERIFY(dsr.waitForProgress());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(dsr.isFinished());
    QVERIFY(!dsr.isActive());
    QVERIFY(dsr.status() == QContactAbstractRequest::Cancelled);

    // verify that the changes were not saved
    allDefs = cm->detailDefinitions().values();
    QVERIFY(!allDefs.contains(testDef));
    QCOMPARE(cm->detailDefinitions().values().size(), originalCount + 1);

    delete cm;
}

void tst_QContactAsync::relationshipFetch()
{
    QFETCH(QString, uri);
    QContactManager* cm(0);
    QList<QContact> contacts;
    QList<QContactRelationship> relationships;
    QVERIFY(prepareModel(uri, cm, contacts, relationships));
    QContactRelationshipFetchRequest rfr;
    QVERIFY(rfr.type() == QContactAbstractRequest::RelationshipFetchRequest);
    
    // initial state - not started, no manager.
    QVERIFY(!rfr.isActive());
    QVERIFY(!rfr.isFinished());
    QVERIFY(!rfr.start());
    QVERIFY(!rfr.cancel());
    QVERIFY(!rfr.waitForFinished());
    QVERIFY(!rfr.waitForProgress());    
    
    if (!cm->hasFeature(QContactManager::Relationships))
    {
        // ensure manager returs errors
        rfr.setManager(cm);
        QCOMPARE(rfr.manager(), cm);
        QVERIFY(!rfr.isActive());
        QVERIFY(!rfr.isFinished());
        QVERIFY(!rfr.cancel());
        QVERIFY(!rfr.waitForFinished());
        QVERIFY(!rfr.waitForProgress());
        QVERIFY(!rfr.cancel()); // not started
        QVERIFY(rfr.start());
        QVERIFY(rfr.isActive());
        QVERIFY(rfr.status() == QContactAbstractRequest::Active);
        QVERIFY(!rfr.isFinished());
        QVERIFY(!rfr.start());  // already started.
        QVERIFY(rfr.waitForFinished());
        QVERIFY(rfr.error() == QContactManager::NotSupportedError);
        QVERIFY(rfr.isFinished());
        QVERIFY(!rfr.isActive());
        return;
    }
    
    // use variables to make code more readable
    QContactId aId = contacts.at(0).id();
    QContactId bId = contacts.at(1).id();
    QContactId cId = contacts.at(2).id();
    QContactId dId = contacts.at(3).id();
    QContactId eId = contacts.at(4).id();
    QContactId fId = contacts.at(5).id();    
    QContactRelationship adRel = relationships.at(0);
    QContactRelationship aeRel = relationships.at(1);
    QContactRelationship beRel = relationships.at(2);
    QContactRelationship ceRel = relationships.at(3);
    QContactRelationship cfRel = relationships.at(4);
    QString relType = adRel.relationshipType();

    // "all relationships" retrieval
    rfr.setManager(cm);
    QCOMPARE(rfr.manager(), cm);
    QVERIFY(!rfr.isActive());
    QVERIFY(!rfr.isFinished());
    QVERIFY(!rfr.cancel());
    QVERIFY(!rfr.waitForFinished());
    QVERIFY(!rfr.waitForProgress());
    qRegisterMetaType<QContactRelationshipFetchRequest*>("QContactRelationshipFetchRequest*");
    QSignalSpy spy(&rfr, SIGNAL(progress(QContactRelationshipFetchRequest*, bool)));
    QVERIFY(!rfr.cancel()); // not started
    QVERIFY(rfr.start());
    QVERIFY(rfr.isActive());
    QVERIFY(rfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!rfr.isFinished());
    QVERIFY(!rfr.start());  // already started.
    QVERIFY(rfr.waitForFinished());
    QVERIFY(rfr.error() == QContactManager::NoError);
    int expectedCount = 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(rfr.isFinished());
    QVERIFY(!rfr.isActive());
    QList<QContactRelationship> rels = cm->relationships();
    QList<QContactRelationship> result = rfr.relationships();
    QCOMPARE(rels, result);
    
    // specific relationship type retrieval
    rfr.setRelationshipType(relType);
    QVERIFY(!rfr.cancel()); // not started
    QVERIFY(rfr.start());
    QVERIFY(rfr.isActive());
    QVERIFY(rfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!rfr.isFinished());
    QVERIFY(!rfr.start());  // already started.
    QVERIFY(rfr.waitForFinished());
    QVERIFY(rfr.error() == QContactManager::NoError);
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(rfr.isFinished());
    QVERIFY(!rfr.isActive());
    rels = cm->relationships(relType);
    result = rfr.relationships();
    QCOMPARE(rels, result);

    // specific source contact retrieval
    rfr.setFirst(aId);
    rfr.setRelationshipType(QString());
    QVERIFY(!rfr.cancel()); // not started
    QVERIFY(rfr.start());
    QVERIFY(rfr.isActive());
    QVERIFY(rfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!rfr.isFinished());
    QVERIFY(!rfr.start());  // already started.
    QVERIFY(rfr.waitForFinished());
    QVERIFY(rfr.error() == QContactManager::NoError);
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(rfr.isFinished());
    QVERIFY(!rfr.isActive());
    rels = cm->relationships(aId, QContactRelationshipFilter::First);
    result = rfr.relationships();
    QCOMPARE(rels, result);

    // specific participant retrieval #1 - destination participant
    rfr.setFirst(QContactId());
    rfr.setParticipant(eId, QContactRelationshipFilter::Second);
    QVERIFY(rfr.participantRole() == QContactRelationshipFilter::Second);
    QVERIFY(!rfr.cancel()); // not started
    QVERIFY(rfr.start());
    QVERIFY(rfr.isActive());
    QVERIFY(rfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!rfr.isFinished());
    QVERIFY(!rfr.start());  // already started.
    QVERIFY(rfr.waitForFinished());
    QVERIFY(rfr.error() == QContactManager::NoError);
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(rfr.isFinished());
    QVERIFY(!rfr.isActive());
    rels = cm->relationships(eId, QContactRelationshipFilter::Second);
    result = rfr.relationships();
    QCOMPARE(rels, result);

    // specific participant retrieval #2 - source participant
    rfr.setFirst(QContactId());
    rfr.setParticipant(cId, QContactRelationshipFilter::First);
    QVERIFY(rfr.participantRole() == QContactRelationshipFilter::First);
    QVERIFY(!rfr.cancel()); // not started
    QVERIFY(rfr.start());
    QVERIFY(rfr.isActive());
    QVERIFY(rfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!rfr.isFinished());
    QVERIFY(!rfr.start());  // already started.
    QVERIFY(rfr.waitForFinished());
    QVERIFY(rfr.error() == QContactManager::NoError);
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(rfr.isFinished());
    QVERIFY(!rfr.isActive());
    rels = cm->relationships(cId, QContactRelationshipFilter::First);
    result = rfr.relationships();
    QCOMPARE(rels, result);
    
    if (relationships.count() > 4)
    {
        // specific participant retrieval #3 - either participant
        rfr.setFirst(QContactId());
        rfr.setParticipant(aId, QContactRelationshipFilter::Either);
        QVERIFY(rfr.participantRole() == QContactRelationshipFilter::Either);
        QVERIFY(!rfr.cancel()); // not started
        QVERIFY(rfr.start());
        QVERIFY(rfr.isActive());
        QVERIFY(rfr.status() == QContactAbstractRequest::Active);
        QVERIFY(!rfr.isFinished());
        QVERIFY(!rfr.start());  // already started.
        QVERIFY(rfr.waitForFinished());
        QVERIFY(rfr.error() == QContactManager::NoError);
        expectedCount += 2;
        QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
        QVERIFY(rfr.isFinished());
        QVERIFY(!rfr.isActive());
        rels = cm->relationships(aId); // either role.
        result = rfr.relationships();
        QCOMPARE(rels, result);
    }    

    // cancelling
    rfr.setRelationshipType(QString());
    QVERIFY(!rfr.cancel()); // not started
    QVERIFY(rfr.start());
    QVERIFY(rfr.isActive());
    QVERIFY(rfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!rfr.isFinished());
    QVERIFY(rfr.cancel());
    QVERIFY(rfr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(rfr.isActive());    // still cancelling
    QVERIFY(!rfr.isFinished()); // not finished cancelling
    QVERIFY(!rfr.start());      // already started.
    QVERIFY(rfr.waitForFinished());
    QVERIFY(rfr.error() == QContactManager::NoError);
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(rfr.isFinished());
    QVERIFY(!rfr.isActive());
    QVERIFY(rfr.status() == QContactAbstractRequest::Cancelled);

    // restart, and wait for progress after cancel.
    QVERIFY(!rfr.cancel()); // not started
    QVERIFY(rfr.start());
    QVERIFY(rfr.isActive());
    QVERIFY(rfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!rfr.isFinished());
    QVERIFY(rfr.cancel());
    QVERIFY(rfr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(rfr.isActive());    // still cancelling
    QVERIFY(!rfr.isFinished()); // not finished cancelling
    QVERIFY(!rfr.start());      // already started.
    QVERIFY(rfr.waitForProgress());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(rfr.isFinished());
    QVERIFY(!rfr.isActive());
    QVERIFY(rfr.status() == QContactAbstractRequest::Cancelled);
    
    delete cm;
}

void tst_QContactAsync::relationshipRemove()
{
    QFETCH(QString, uri);
    QContactManager* cm(0);
    QList<QContact> contacts;
    QList<QContactRelationship> relationships;
    QVERIFY(prepareModel(uri, cm, contacts, relationships));
    QContactRelationshipRemoveRequest rrr;
    QVERIFY(rrr.type() == QContactAbstractRequest::RelationshipRemoveRequest);
        
    if (!cm->hasFeature(QContactManager::Relationships))
    {
        // ensure manager returns error
        rrr.setFirst(contacts.at(0).id());
        rrr.setManager(cm);
        QCOMPARE(rrr.manager(), cm);
        QVERIFY(!rrr.isActive());
        QVERIFY(!rrr.isFinished());
        QVERIFY(!rrr.cancel());
        QVERIFY(!rrr.waitForFinished());
        QVERIFY(!rrr.waitForProgress());
        QVERIFY(!rrr.cancel()); // not started
        QVERIFY(rrr.start());
        QVERIFY(rrr.isActive());
        QVERIFY(rrr.status() == QContactAbstractRequest::Active);
        QVERIFY(!rrr.isFinished());
        QVERIFY(!rrr.start());  // already started.
        QVERIFY(rrr.waitForFinished());
        QVERIFY(rrr.error() == QContactManager::NotSupportedError);
        QVERIFY(rrr.isFinished());
        QVERIFY(!rrr.isActive());
        return;
    }

    // use variables to make code more readable
    QContactId aId = contacts.at(0).id();
    QContactId bId = contacts.at(1).id();
    QContactId cId = contacts.at(2).id();
    QContactId dId = contacts.at(3).id();
    QContactId eId = contacts.at(4).id();
    QContactId fId = contacts.at(5).id();    
    QContactRelationship adRel = relationships.at(0);
    QContactRelationship aeRel = relationships.at(1);
    QContactRelationship beRel = relationships.at(2);
    QContactRelationship ceRel = relationships.at(3);
    QContactRelationship cfRel = relationships.at(4);
    QString relType = adRel.relationshipType();
    
    // initial state - not started, no manager.
    QVERIFY(!rrr.isActive());
    QVERIFY(!rrr.isFinished());
    QVERIFY(!rrr.start());
    QVERIFY(!rrr.cancel());
    QVERIFY(!rrr.waitForFinished());
    QVERIFY(!rrr.waitForProgress());

    // specific source, destination and type removal
    int relationshipCount = cm->relationships().count();
    rrr.setFirst(adRel.first());
    rrr.setSecond(adRel.second());
    rrr.setRelationshipType(adRel.relationshipType());
    rrr.setManager(cm);
    qRegisterMetaType<QContactRelationshipRemoveRequest*>("QContactRelationshipRemoveRequest*");
    QSignalSpy spy(&rrr, SIGNAL(progress(QContactRelationshipRemoveRequest*)));
    QCOMPARE(rrr.manager(), cm);
    QVERIFY(!rrr.isActive());
    QVERIFY(!rrr.isFinished());
    QVERIFY(!rrr.cancel());
    QVERIFY(!rrr.waitForFinished());
    QVERIFY(!rrr.waitForProgress());
    QVERIFY(rrr.relationshipType() == adRel.relationshipType());
    QVERIFY(!rrr.cancel()); // not started
    QVERIFY(rrr.start());
    QVERIFY(rrr.isActive());
    QVERIFY(rrr.status() == QContactAbstractRequest::Active);
    QVERIFY(!rrr.isFinished());
    QVERIFY(!rrr.start());  // already started.
    QVERIFY(rrr.waitForFinished());
    QVERIFY(rrr.error() == QContactManager::NoError);
    int expectedCount = 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(rrr.isFinished());
    QVERIFY(!rrr.isActive());
    QCOMPARE(cm->relationships().count(), relationshipCount-1);
    
    // remove (asynchronously) a nonexistent relationship - should fail.
    relationshipCount = cm->relationships().count();
    rrr.setFirst(bId);
    rrr.setSecond(aId);
    rrr.setRelationshipType(relType);
    rrr.setManager(cm);
    QVERIFY(!rrr.cancel()); // not started
    QVERIFY(rrr.start());
    QVERIFY(rrr.isActive());
    QVERIFY(rrr.status() == QContactAbstractRequest::Active);
    QVERIFY(!rrr.isFinished());
    QVERIFY(!rrr.start());  // already started.
    QVERIFY(rrr.waitForFinished());
    QCOMPARE(rrr.error(), QContactManager::DoesNotExistError);
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(rrr.isFinished());
    QVERIFY(!rrr.isActive());
    QCOMPARE(cm->relationships().count(), relationshipCount);
    
    // specific relationship type plus source removal
    rrr.setFirst(cId);
    rrr.setSecond(QContactId());
    rrr.setRelationshipType(relType);
    rrr.setManager(cm);
    QCOMPARE(rrr.manager(), cm);
    QVERIFY(!rrr.isActive());
    QVERIFY(!rrr.cancel());
    QVERIFY(!rrr.waitForFinished());
    QVERIFY(!rrr.waitForProgress());
    QVERIFY(rrr.relationshipType() == relType);
    QVERIFY(!rrr.cancel()); // not started
    QVERIFY(rrr.start());
    QVERIFY(rrr.isActive());
    QVERIFY(rrr.status() == QContactAbstractRequest::Active);
    QVERIFY(!rrr.isFinished());
    QVERIFY(!rrr.start());  // already started.
    QVERIFY(rrr.waitForFinished());
    QVERIFY(rrr.error() == QContactManager::NoError);
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(rrr.isFinished());
    QVERIFY(!rrr.isActive());
    QCOMPARE(cm->relationships(relType, cId, QContactRelationshipFilter::First).size(), 0);
    QCOMPARE(cm->error(), QContactManager::DoesNotExistError);
    
    // specific source removal
    rrr.setFirst(aId);
    rrr.setSecond(QContactId());
    rrr.setRelationshipType(QString());
    rrr.setManager(cm);
    QCOMPARE(rrr.manager(), cm);
    QVERIFY(!rrr.isActive());
    QVERIFY(!rrr.cancel());
    QVERIFY(!rrr.waitForFinished());
    QVERIFY(!rrr.waitForProgress());
    QVERIFY(rrr.relationshipType() == QString());
    QVERIFY(!rrr.cancel()); // not started
    QVERIFY(rrr.start());
    QVERIFY(rrr.isActive());
    QVERIFY(rrr.status() == QContactAbstractRequest::Active);
    QVERIFY(!rrr.isFinished());
    QVERIFY(!rrr.start());  // already started.
    QVERIFY(rrr.waitForFinished());
    QVERIFY(rrr.error() == QContactManager::NoError);
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(rrr.isFinished());
    QVERIFY(!rrr.isActive());
    QCOMPARE(cm->relationships(aId, QContactRelationshipFilter::First).size(), 0);
    QCOMPARE(cm->error(), QContactManager::DoesNotExistError);
    
    // cancelling
    rrr.setFirst(bId);
    rrr.setSecond(QContactId());
    rrr.setRelationshipType(QString());
    QVERIFY(!rrr.cancel()); // not started
    QVERIFY(rrr.start());
    QVERIFY(rrr.isActive());
    QVERIFY(rrr.status() == QContactAbstractRequest::Active);
    QVERIFY(!rrr.isFinished());
    QVERIFY(rrr.cancel());
    QVERIFY(rrr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(rrr.isActive());    // still cancelling
    QVERIFY(!rrr.isFinished()); // not finished cancelling
    QVERIFY(!rrr.start());      // already started.
    QVERIFY(rrr.waitForFinished());
    QVERIFY(rrr.error() == QContactManager::NoError);
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(rrr.isFinished());
    QVERIFY(!rrr.isActive());
    QVERIFY(rrr.status() == QContactAbstractRequest::Cancelled);
    QVERIFY(cm->relationships(bId).size() != 0); // didn't remove them.
    
    // restart, and wait for progress after cancel.
    QVERIFY(!rrr.cancel()); // not started
    QVERIFY(rrr.start());
    QVERIFY(rrr.isActive());
    QVERIFY(rrr.status() == QContactAbstractRequest::Active);
    QVERIFY(!rrr.isFinished());
    QVERIFY(rrr.cancel());
    QVERIFY(rrr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(rrr.isActive());    // still cancelling
    QVERIFY(!rrr.isFinished()); // not finished cancelling
    QVERIFY(!rrr.start());      // already started.
    QVERIFY(rrr.waitForProgress());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(rrr.isFinished());
    QVERIFY(!rrr.isActive());
    QVERIFY(rrr.status() == QContactAbstractRequest::Cancelled);
    QVERIFY(cm->relationships(bId).size() != 0); // didn't remove them.
    
    // specific relationship type removal
    rrr.setFirst(QContactId());
    rrr.setSecond(QContactId());
    rrr.setRelationshipType(relType);
    rrr.setManager(cm);
    QCOMPARE(rrr.manager(), cm);
    QVERIFY(!rrr.isActive());
    QVERIFY(!rrr.cancel());
    QVERIFY(!rrr.waitForFinished());
    QVERIFY(!rrr.waitForProgress());
    QVERIFY(rrr.relationshipType() == relType);
    QVERIFY(!rrr.cancel()); // not started
    QVERIFY(rrr.start());
    QVERIFY(rrr.isActive());
    QVERIFY(rrr.status() == QContactAbstractRequest::Active);
    QVERIFY(!rrr.isFinished());
    QVERIFY(!rrr.start());  // already started.
    QVERIFY(rrr.waitForFinished());
    QVERIFY(rrr.error() == QContactManager::NoError);
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(rrr.isFinished());
    QVERIFY(!rrr.isActive());
    
    QCOMPARE(cm->relationships(relType).size(), 0);
    cm->relationships(relType); // check that it has already been removed.
    QCOMPARE(cm->error(), QContactManager::DoesNotExistError);

    delete cm;
}

void tst_QContactAsync::relationshipSave()
{
    QFETCH(QString, uri);
    QContactManager* cm(0);
    QList<QContact> contacts;
    QList<QContactRelationship> relationships;
    QVERIFY(prepareModel(uri, cm, contacts, relationships));
    QContactRelationshipSaveRequest rsr;
    QVERIFY(rsr.type() == QContactAbstractRequest::RelationshipSaveRequest);
    
    if (!cm->hasFeature(QContactManager::Relationships))
    {
        // ensure saving returns errors
        QContactRelationship rel;
        rel.setFirst(contacts.at(0).id());
        rel.setSecond(contacts.at(1).id());
        rel.setRelationshipType(QContactRelationship::HasManager);
        QList<QContactRelationship> list;
        list << rel;
        rsr.setManager(cm);
        QCOMPARE(rsr.manager(), cm);
        QVERIFY(!rsr.isActive());
        QVERIFY(!rsr.isFinished());
        QVERIFY(!rsr.cancel());
        QVERIFY(!rsr.waitForFinished());
        QVERIFY(!rsr.waitForProgress());
        rsr.setRelationships(list);
        QCOMPARE(rsr.relationships(), list);
        QVERIFY(!rsr.cancel()); // not started
        QVERIFY(rsr.start());
        QVERIFY(rsr.isActive());
        QVERIFY(rsr.status() == QContactAbstractRequest::Active);
        QVERIFY(!rsr.isFinished());
        QVERIFY(!rsr.start());  // already started.
        QVERIFY(rsr.waitForFinished());
        QVERIFY(rsr.error() == QContactManager::NotSupportedError);
        QVERIFY(rsr.isFinished());
        QVERIFY(!rsr.isActive());
        return;
    }
    
    // use variables to make code more readable
    QContactId aId = contacts.at(0).id();
    QContactId bId = contacts.at(1).id();
    QContactId cId = contacts.at(2).id();
    QContactId dId = contacts.at(3).id();
    QContactId eId = contacts.at(4).id();
    QContactId fId = contacts.at(5).id();    
    QContactRelationship adRel = relationships.at(0);
    QContactRelationship aeRel = relationships.at(1);
    QContactRelationship beRel = relationships.at(2);
    QContactRelationship ceRel = relationships.at(3);
    QContactRelationship cfRel = relationships.at(4);
    QString relType = adRel.relationshipType();

    // initial state - not started, no manager.
    QVERIFY(!rsr.isActive());
    QVERIFY(!rsr.isFinished());
    QVERIFY(!rsr.start());
    QVERIFY(!rsr.cancel());
    QVERIFY(!rsr.waitForFinished());
    QVERIFY(!rsr.waitForProgress());

    // save a new relationship
    int originalCount = cm->relationships(bId).size();
    QContactRelationship testRel;
    testRel.setFirst(bId);
    testRel.setSecond(dId);
    testRel.setRelationshipType(relType);
    QList<QContactRelationship> saveList;
    saveList << testRel;
    rsr.setManager(cm);
    QCOMPARE(rsr.manager(), cm);
    QVERIFY(!rsr.isActive());
    QVERIFY(!rsr.isFinished());
    QVERIFY(!rsr.cancel());
    QVERIFY(!rsr.waitForFinished());
    QVERIFY(!rsr.waitForProgress());
    qRegisterMetaType<QContactRelationshipSaveRequest*>("QContactRelationshipSaveRequest*");
    QSignalSpy spy(&rsr, SIGNAL(progress(QContactRelationshipSaveRequest*)));
    rsr.setRelationships(saveList);
    QCOMPARE(rsr.relationships(), saveList);
    QVERIFY(!rsr.cancel()); // not started
    QVERIFY(rsr.start());
    QVERIFY(rsr.isActive());
    QVERIFY(rsr.status() == QContactAbstractRequest::Active);
    QVERIFY(!rsr.isFinished());
    QVERIFY(!rsr.start());  // already started.
    QVERIFY(rsr.waitForFinished());
    int expectedCount = 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(rsr.isFinished());
    QVERIFY(!rsr.isActive());
    QList<QContactRelationship> result = rsr.relationships();
    QVERIFY(result.contains(testRel));
    QList<QContactRelationship> bRelationships = cm->relationships(relType, bId, QContactRelationshipFilter::First);
    QVERIFY(bRelationships.contains(testRel));
    QCOMPARE(cm->relationships(bId).size(), originalCount + 1); // should be one extra

    // save a new relationship
    testRel.setSecond(fId);
    saveList.clear();
    saveList << testRel;
    rsr.setRelationships(saveList);
    QCOMPARE(rsr.relationships(), saveList);
    QVERIFY(!rsr.cancel()); // not started
    QVERIFY(rsr.start());
    QVERIFY(rsr.isActive());
    QVERIFY(rsr.status() == QContactAbstractRequest::Active);
    QVERIFY(!rsr.isFinished());
    QVERIFY(!rsr.start());  // already started.
    QVERIFY(rsr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(rsr.isFinished());
    QVERIFY(!rsr.isActive());
    bRelationships.clear();
    bRelationships = cm->relationships(relType, bId, QContactRelationshipFilter::First);
    result = rsr.relationships();
    QCOMPARE(result, QList<QContactRelationship>() << testRel);
    QVERIFY(bRelationships.contains(testRel));
    QCOMPARE(cm->relationships(bId).size(), originalCount + 2); // should now be two extra

    // cancelling
    testRel.setSecond(bId); // shouldn't get saved (circular anyway)
    saveList.clear();
    saveList << testRel;
    rsr.setRelationships(saveList);
    QCOMPARE(rsr.relationships(), saveList);
    QVERIFY(!rsr.cancel()); // not started
    QVERIFY(rsr.start());
    QVERIFY(rsr.isActive());
    QVERIFY(rsr.status() == QContactAbstractRequest::Active);
    QVERIFY(!rsr.isFinished());
    QVERIFY(rsr.cancel());
    QVERIFY(rsr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(rsr.isActive());    // still cancelling
    QVERIFY(!rsr.isFinished()); // not finished cancelling
    QVERIFY(!rsr.start());      // already started.
    QVERIFY(rsr.waitForFinished());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(rsr.isFinished());
    QVERIFY(!rsr.isActive());
    QVERIFY(rsr.status() == QContactAbstractRequest::Cancelled);

    // verify that the changes were not saved
    QList<QContactRelationship> aRels = cm->relationships(bId, QContactRelationshipFilter::First);
    QVERIFY(!aRels.contains(testRel));
    QCOMPARE(cm->relationships(bId).size(), originalCount + 2); // should still only be two extra

    // restart, and wait for progress after cancel.
    QVERIFY(!rsr.cancel()); // not started
    QVERIFY(rsr.start());
    QVERIFY(rsr.isActive());
    QVERIFY(rsr.status() == QContactAbstractRequest::Active);
    QVERIFY(!rsr.isFinished());
    QVERIFY(rsr.cancel());
    QVERIFY(rsr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(rsr.isActive());    // still cancelling
    QVERIFY(!rsr.isFinished()); // not finished cancelling
    QVERIFY(!rsr.start());      // already started.
    QVERIFY(rsr.waitForProgress());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(rsr.isFinished());
    QVERIFY(!rsr.isActive());
    QVERIFY(rsr.status() == QContactAbstractRequest::Cancelled);

    // verify that the changes were not saved
    aRels = cm->relationships(bId);
    QVERIFY(!aRels.contains(testRel));
    QCOMPARE(cm->relationships(bId).size(), originalCount + 2); // should still only be two extra

    delete cm;
}

void tst_QContactAsync::maliciousManager()
{
    // use the invalid manager: passes all requests through to base class
    QContactManager cm("invalid");
    QContactFilter fil; // matches everything
    QContactFetchRequest cfr;
    cfr.setFilter(fil);
    cfr.setManager(&cm);
    QVERIFY(!cfr.start());
    QVERIFY(!cfr.cancel());
    QVERIFY(!cfr.waitForFinished());
    QVERIFY(!cfr.start());
    QVERIFY(!cfr.waitForProgress());

    // ensure that the base class implementation of requestDestroyed() is called
    QContactFetchRequest *destroyedRequest = new QContactFetchRequest;
    destroyedRequest->setManager(&cm);
    destroyedRequest->setFilter(fil);
    QVERIFY(!destroyedRequest->start());
    delete destroyedRequest;

    // now use a malicious manager that deliberately calls
    // incorrect "updateRequest" functions in base class:
    QContactManager mcm("maliciousplugin");
    QCOMPARE(mcm.managerName(), QString("maliciousplugin"));
    QList<QContact> emptyCList;
    QList<QContactLocalId> emptyIList;
    QList<QContactDetailDefinition> emptyDList;
    QStringList emptyDNList;
    QMap<QString, QContactDetailDefinition> emptyDMap;
    cfr.setFilter(fil);
    cfr.setManager(&mcm);
    QVERIFY(cfr.start());
    QVERIFY(cfr.cancel());
    QVERIFY(!cfr.waitForProgress(100));
    QVERIFY(!cfr.waitForFinished(100));
    QVERIFY(cfr.start());
    QVERIFY(!cfr.waitForProgress(100));
    QVERIFY(!cfr.waitForFinished(100));
    QVERIFY(cfr.cancel());

    QContactLocalIdFetchRequest cifr;
    cifr.setFilter(fil);
    cifr.setManager(&mcm);
    QVERIFY(cifr.start());
    QVERIFY(cifr.cancel());
    QVERIFY(!cifr.waitForProgress(100));
    QVERIFY(!cifr.waitForFinished(100));
    QVERIFY(cifr.start());
    QVERIFY(!cifr.waitForProgress(100));
    QVERIFY(!cifr.waitForFinished(100));
    QVERIFY(cifr.cancel());

    QContactRemoveRequest crr;
    crr.setFilter(fil);
    crr.setManager(&mcm);
    QVERIFY(crr.start());
    QVERIFY(crr.cancel());
    QVERIFY(!crr.waitForProgress(100));
    QVERIFY(!crr.waitForFinished(100));
    QVERIFY(crr.start());
    QVERIFY(!crr.waitForProgress(100));
    QVERIFY(!crr.waitForFinished(100));
    QVERIFY(crr.cancel());

    QContactSaveRequest csr;
    csr.setContacts(emptyCList);
    csr.setManager(&mcm);
    QVERIFY(csr.start());
    QVERIFY(csr.cancel());
    QVERIFY(!csr.waitForProgress(100));
    QVERIFY(!csr.waitForFinished(100));
    QVERIFY(csr.start());
    QVERIFY(!csr.waitForProgress(100));
    QVERIFY(!csr.waitForFinished(100));
    QVERIFY(csr.cancel());

    QContactDetailDefinitionFetchRequest dfr;
    dfr.setNames(emptyDNList);
    dfr.setManager(&mcm);
    QVERIFY(dfr.start());
    QVERIFY(dfr.cancel());
    QVERIFY(!dfr.waitForProgress(100));
    QVERIFY(!dfr.waitForFinished(100));
    QVERIFY(dfr.start());
    QVERIFY(!dfr.waitForProgress(100));
    QVERIFY(!dfr.waitForFinished(100));
    QVERIFY(dfr.cancel());

    QContactDetailDefinitionSaveRequest dsr;
    dsr.setDefinitions(emptyDList);
    dsr.setManager(&mcm);
    QVERIFY(dsr.start());
    QVERIFY(dsr.cancel());
    QVERIFY(!dsr.waitForProgress(100));
    QVERIFY(!dsr.waitForFinished(100));
    QVERIFY(dsr.start());
    QVERIFY(!dsr.waitForProgress(100));
    QVERIFY(!dsr.waitForFinished(100));
    QVERIFY(dsr.cancel());

    QContactDetailDefinitionRemoveRequest drr;
    drr.setNames(emptyDNList);
    drr.setManager(&mcm);
    QVERIFY(drr.start());
    QVERIFY(drr.cancel());
    QVERIFY(!drr.waitForProgress(100));
    QVERIFY(!drr.waitForFinished(100));
    QVERIFY(drr.start());
    QVERIFY(!drr.waitForProgress(100));
    QVERIFY(!drr.waitForFinished(100));
    QVERIFY(drr.cancel());
}

void tst_QContactAsync::threadDelivery()
{
    QFETCH(QString, uri);
    QContactManager *cm(0);
    QVERIFY(prepareModel(uri, cm));
    m_mainThreadId = cm->thread()->currentThreadId();
    m_progressSlotThreadId = m_mainThreadId;

    // now perform a fetch request and check that the progress is delivered to the correct thread.
    QContactFetchRequest *req = new QContactFetchRequest;
    req->setManager(cm);
    connect(req, SIGNAL(progress(QContactFetchRequest*,bool)), this, SLOT(progressReceived(QContactFetchRequest*, bool)));
    req->start();

    int totalWaitTime = 0;
    while (req->status() != QContactAbstractRequest::Finished) {
        // ensure that the progress signal was delivered to the main thread.
        QCOMPARE(m_mainThreadId, m_progressSlotThreadId);

        QTest::qWait(5); // spin until done
        totalWaitTime += 5;

        // break after 30 seconds.
        if (totalWaitTime > 30000) {
            delete req;
            delete cm;
            QSKIP("Asynchronous request not complete after 30 seconds!", SkipSingle);
        }
    }

    // ensure that the progress signal was delivered to the main thread.
    QCOMPARE(m_mainThreadId, m_progressSlotThreadId);
    delete req;
    delete cm;
}

void tst_QContactAsync::progressReceived(QContactFetchRequest* request, bool appendOnly)
{
    Q_UNUSED(appendOnly);
    m_progressSlotThreadId = request->thread()->currentThreadId();
}

void tst_QContactAsync::addManagers()
{
    QTest::addColumn<QString>("uri");

    // retrieve the list of available managers
    QStringList managers = QContactManager::availableManagers();

    // remove ones that we know will not pass
    managers.removeAll("invalid");
    managers.removeAll("maliciousplugin");
    managers.removeAll("testdummy");

    foreach(QString mgr, managers) {
        QMap<QString, QString> params;
        QTest::newRow(QString("mgr='%1'").arg(mgr).toLatin1().constData()) << QContactManager::buildUri(mgr, params);
        if (mgr == "memory") {
            params.insert("id", "tst_QContactManager");
            QTest::newRow(QString("mgr='%1', params").arg(mgr).toLatin1().constData()) << QContactManager::buildUri(mgr, params);
        }
    }
}

bool tst_QContactAsync::prepareModel(const QString &managerUri, QContactManager *&cm)
{
    QList<QContact> contacts;
    QList<QContactRelationship> relationships;
    return prepareModel(managerUri, cm, contacts, relationships);
}

bool tst_QContactAsync::prepareModel(const QString &managerUri, QContactManager *&cm, QList<QContact> &contacts, QList<QContactRelationship> &relationships)
{
    cm = QContactManager::fromUri(managerUri);

    // XXX TODO: ensure that this is the case:
    // there should be no contacts in the database.
    QList<QContactLocalId> toRemove = cm->contacts();
    foreach (const QContactLocalId& removeId, toRemove) {
        if (!cm->removeContact(removeId))
            return false;
    }

    QContact a, b, c, d, e, f;
    QContactPhoneNumber n;
    n.setNumber("1");
    a.saveDetail(&n);
    n.setNumber("2");
    b.saveDetail(&n);
    n.setNumber("3");
    c.saveDetail(&n);
    n.setNumber("4");
    d.saveDetail(&n);
    n.setNumber("5");
    e.saveDetail(&n);
    n.setNumber("6");
    f.saveDetail(&n);    

    QContactUrl url;
    url.setUrl("http://test.nokia.com");
    a.saveDetail(&url);
    
    a.setType(QContactType::TypeGroup);
    b.setType(QContactType::TypeGroup);
    c.setType(QContactType::TypeGroup);

    if (!cm->saveContact(&a)) 
        return false;
    if (!cm->saveContact(&b)) 
        return false;
    if (!cm->saveContact(&c)) 
        return false;
    if (!cm->saveContact(&d)) 
        return false;
    if (!cm->saveContact(&e)) 
        return false;
    if (!cm->saveContact(&f)) 
        return false;
    
    contacts.append(a);
    contacts.append(b);
    contacts.append(c);
    contacts.append(d);
    contacts.append(e);
    contacts.append(f);
    
    if (cm->hasFeature(QContactManager::Relationships))
    {
        QStringList supportedRelationshipTypes = cm->supportedRelationshipTypes();

        if (cm->hasFeature(QContactManager::ArbitraryRelationshipTypes)) {
            supportedRelationshipTypes.insert(0, "some-arbitrary-relationship");
            if (!supportedRelationshipTypes.contains(QContactRelationship::HasManager))
                supportedRelationshipTypes.append(QContactRelationship::HasManager);
            if (!supportedRelationshipTypes.contains(QContactRelationship::HasAssistant))
                supportedRelationshipTypes.append(QContactRelationship::HasAssistant);
            if (!supportedRelationshipTypes.contains(QContactRelationship::HasSpouse))
                supportedRelationshipTypes.append(QContactRelationship::HasSpouse);
        }
                    
        if (supportedRelationshipTypes.count() == 0)
            return false; // should not happen
                
        QContactRelationship adRel;
        adRel.setFirst(a.id());
        adRel.setSecond(d.id());
        adRel.setRelationshipType(supportedRelationshipTypes.at(0));
        if (!cm->saveRelationship(&adRel))
            return false;
        relationships.append(adRel);
        
        QContactRelationship aeRel;
        aeRel.setFirst(a.id());
        aeRel.setSecond(e.id());
        aeRel.setRelationshipType(supportedRelationshipTypes.at(0));
        if (!cm->saveRelationship(&aeRel))
            return false;
        relationships.append(aeRel);
        
        QContactRelationship beRel;
        beRel.setFirst(b.id());
        beRel.setSecond(e.id());
        beRel.setRelationshipType(supportedRelationshipTypes.at(0));
        if (!cm->saveRelationship(&beRel))
            return false; 
        relationships.append(beRel);
        
        QContactRelationship ceRel;
        ceRel.setFirst(c.id());
        ceRel.setSecond(e.id());
        ceRel.setRelationshipType(supportedRelationshipTypes.at(0));
        if (!cm->saveRelationship(&ceRel))
            return false;
        relationships.append(ceRel);
       
        QContactRelationship cfRel;
        cfRel.setFirst(c.id());
        cfRel.setSecond(f.id());
        cfRel.setRelationshipType(supportedRelationshipTypes.at(0));
        if (!cm->saveRelationship(&cfRel))
            return false;
        relationships.append(cfRel);
        
        if (supportedRelationshipTypes.count() > 1)
        {
            QContactRelationship daRel;
            daRel.setFirst(d.id());
            daRel.setSecond(a.id());
            daRel.setRelationshipType(supportedRelationshipTypes.at(1));
            if (!cm->saveRelationship(&daRel)) 
                return false;
            relationships.append(daRel);
        }
        
        if (supportedRelationshipTypes.count() > 2)
        {
            QContactRelationship adRel2;
            adRel2.setFirst(a.id());
            adRel2.setSecond(d.id());
            adRel2.setRelationshipType(supportedRelationshipTypes.at(2));
            if (!cm->saveRelationship(&adRel2)) 
                return false;
            relationships.append(adRel2);
        }
    }

    return true;

    // TODO: cleanup once test is complete
}

QTEST_MAIN(tst_QContactAsync)
#include "tst_qcontactasync.moc"
