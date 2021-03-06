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

#include <QtTest/QtTest>

#include <QCoreApplication>
#include <QScopedPointer>

#include "qtcontacts.h"
#include "qcontactmanagerdataholder.h" //QContactManagerDataHolder

QTM_USE_NAMESPACE
/* Define an innocuous request (fetch ie doesn't mutate) to "fill up" any queues */
#define FILL_QUEUE_WITH_FETCH_REQUESTS() QContactFetchRequest fqcfr1, fqcfr2, fqcfr3; \
                                         QContactDetailDefinitionFetchRequest fqdfr1, fqdfr2, fqdfr3; \
                                         fqcfr1.start(); \
                                         fqcfr2.start(); \
                                         fqcfr3.start(); \
                                         fqdfr1.start(); \
                                         fqdfr2.start(); \
                                         fqdfr3.start();

//TESTED_COMPONENT=src/contacts
//TESTED_CLASS=
//TESTED_FILES=

// Unfortunately the plumbing isn't in place to allow cancelling requests at arbitrary points
// in their processing.  So we do multiple loops until things work out.. or not
#define MAX_OPTIMISTIC_SCHEDULING_LIMIT 100


// Thread capable QThreadSignalSpy (to avoid data races with count/appendArgS)
class QThreadSignalSpy: public QObject
{
public:
    QThreadSignalSpy(QObject *obj, const char *aSignal)
    {
        QMutexLocker m(&lock);
#ifdef Q_CC_BOR
        const int memberOffset = QObject::staticMetaObject.methodCount();
#else
        static const int memberOffset = QObject::staticMetaObject.methodCount();
#endif
        Q_ASSERT(obj);
        Q_ASSERT(aSignal);

        if (((aSignal[0] - '0') & 0x03) != QSIGNAL_CODE) {
            qWarning("QThreadSignalSpy: Not a valid signal, use the SIGNAL macro");
            return;
        }

        QByteArray ba = QMetaObject::normalizedSignature(aSignal + 1);
        const QMetaObject *mo = obj->metaObject();
        int sigIndex = mo->indexOfMethod(ba.constData());
        if (sigIndex < 0) {
            qWarning("QThreadSignalSpy: No such signal: '%s'", ba.constData());
            return;
        }

        if (!QMetaObject::connect(obj, sigIndex, this, memberOffset,
                    Qt::DirectConnection, 0)) {
            qWarning("QThreadSignalSpy: QMetaObject::connect returned false. Unable to connect.");
            return;
        }
        sig = ba;
        initArgs(mo->method(sigIndex));
    }

    inline bool isValid() const { return !sig.isEmpty(); }
    inline QByteArray signal() const { return sig; }

    int qt_metacall(QMetaObject::Call call, int methodId, void **a)
    {
        methodId = QObject::qt_metacall(call, methodId, a);
        if (methodId < 0)
            return methodId;

        if (call == QMetaObject::InvokeMetaMethod) {
            if (methodId == 0) {
                appendArgs(a);
            }
            --methodId;
        }
        return methodId;
    }

    // The QList<QVariantList> API we actually use
    int count() const
    {
        QMutexLocker m(&lock);
        return savedArgs.count();
    }
    void clear()
    {
        QMutexLocker m(&lock);
        savedArgs.clear();
    }

private:
    void initArgs(const QMetaMethod &member)
    {
        QList<QByteArray> params = member.parameterTypes();
        for (int i = 0; i < params.count(); ++i) {
            int tp = QMetaType::type(params.at(i).constData());
            if (tp == QMetaType::Void)
                qWarning("Don't know how to handle '%s', use qRegisterMetaType to register it.",
                         params.at(i).constData());
            args << tp;
        }
    }

    void appendArgs(void **a)
    {
        QMutexLocker m(&lock);
        QList<QVariant> list;
        for (int i = 0; i < args.count(); ++i) {
            QMetaType::Type type = static_cast<QMetaType::Type>(args.at(i));
            list << QVariant(type, a[i + 1]);
        }
        savedArgs.append(list);
    }

    // the full, normalized signal name
    QByteArray sig;
    // holds the QMetaType types for the argument list of the signal
    QList<int> args;

    mutable QMutex lock;
    // Different API
    QList< QVariantList> savedArgs;
};

class tst_QContactAsync : public QObject
{
    Q_OBJECT

public:
    tst_QContactAsync();
    virtual ~tst_QContactAsync();

public slots:
    void initTestCase();
    void cleanupTestCase();

private:
    void addManagers(QStringList includes = QStringList()); // add standard managers to the data

private slots:
    void testDestructor();
    void testDestructor_data() { addManagers(QStringList(QString("maliciousplugin"))); }

    void contactFetch();
    void contactFetch_data() { addManagers(); }
    void contactFetchById();
    void contactFetchById_data() { addManagers(); }
    void contactIdFetch();
    void contactIdFetch_data() { addManagers(); }
    void contactRemove();
    void contactRemove_data() { addManagers(); }
    void contactSave();
    void contactSave_data() { addManagers(); }
    void contactPartialSave();
    void contactPartialSave_data() { addManagers(); }
    void contactPartialSaveAsync();
    void contactPartialSaveAsync_data() {addManagers();}

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

    void testQuickDestruction();
    void testQuickDestruction_data() { addManagers(QStringList(QString("maliciousplugin"))); }

    void threadDelivery();
    void threadDelivery_data() { addManagers(QStringList(QString("maliciousplugin"))); }
protected slots:
    void resultsAvailableReceived();
    void deleteRequest();

private:
    bool compareContactLists(QList<QContact> lista, QList<QContact> listb);
    bool compareContacts(QContact ca, QContact cb);
    bool containsIgnoringTimestamps(const QList<QContact>& list, const QContact& c);
    bool compareIgnoringTimestamps(const QContact& ca, const QContact& cb);
    QContactManager* prepareModel(const QString& uri);

    Qt::HANDLE m_mainThreadId;
    Qt::HANDLE m_resultsAvailableSlotThreadId;
    QScopedPointer<QContactManagerDataHolder> managerDataHolder;
};

tst_QContactAsync::tst_QContactAsync()
{
    // ensure we can load all of the plugins we need to.
    QString path = QApplication::applicationDirPath() + "/dummyplugin/plugins";
    QApplication::addLibraryPath(path);

    qRegisterMetaType<QContactAbstractRequest::State>("QContactAbstractRequest::State");
}

tst_QContactAsync::~tst_QContactAsync()
{
}

void tst_QContactAsync::initTestCase()
{
    managerDataHolder.reset(new QContactManagerDataHolder());
}

void tst_QContactAsync::cleanupTestCase()
{
    managerDataHolder.reset(0);
}

bool tst_QContactAsync::compareContactLists(QList<QContact> lista, QList<QContact> listb)
{
    // NOTE: This compare is contact order insensitive.  
    
    // Remove matching contacts
    foreach (QContact a, lista) {
        foreach (QContact b, listb) {
            if (compareContacts(a, b)) {
                lista.removeOne(a);
                listb.removeOne(b);
                break;
            }
        }
    }    
    return (lista.count() == 0 && listb.count() == 0);
}

bool tst_QContactAsync::compareContacts(QContact ca, QContact cb)
{
    // NOTE: This compare is contact detail order insensitive.
    
    if (ca.localId() != cb.localId())
        return false;
    
    QList<QContactDetail> aDetails = ca.details();
    QList<QContactDetail> bDetails = cb.details();

    // Remove matching details
    foreach (QContactDetail ad, aDetails) {
        foreach (QContactDetail bd, bDetails) {
            if (ad == bd) {
                ca.removeDetail(&ad);
                cb.removeDetail(&bd);
                break;
            }
            
            // Special handling for timestamp
            if (ad.definitionName() == QContactTimestamp::DefinitionName &&
                bd.definitionName() == QContactTimestamp::DefinitionName) {
                QContactTimestamp at = static_cast<QContactTimestamp>(ad);
                QContactTimestamp bt = static_cast<QContactTimestamp>(bd);
                if (at.created().toString() == bt.created().toString() &&
                    at.lastModified().toString() == bt.lastModified().toString()) {
                    ca.removeDetail(&ad);
                    cb.removeDetail(&bd);
                    break;
                }
                    
            }            
        }
    }
    return (ca == cb);
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
    QContactManager* cm = prepareModel(uri);
    QContactFetchRequest* req = new QContactFetchRequest;
    req->setManager(cm);

    QContactManager* cm2 = prepareModel(uri);
    QContactFetchRequest* req2 = new QContactFetchRequest;
    req2->setManager(cm2);

    // first, delete manager then request
    delete cm;
    delete req;

    // second, delete request then manager
    delete req2;
    delete cm2;
}

void tst_QContactAsync::deleteRequest()
{
    // Delete the sender (request) - check that it doesn't crash in this common coding error
    delete sender();
}

void tst_QContactAsync::contactFetch()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(prepareModel(uri));

    QContactFetchRequest cfr;
    QVERIFY(cfr.type() == QContactAbstractRequest::ContactFetchRequest);

    // initial state - not started, no manager.
    QVERIFY(!cfr.isActive());
    QVERIFY(!cfr.isFinished());
    QVERIFY(!cfr.start());
    QVERIFY(!cfr.cancel());
    QVERIFY(!cfr.waitForFinished());

    // "all contacts" retrieval
    QContactFilter fil;
    cfr.setManager(cm.data());
    QCOMPARE(cfr.manager(), cm.data());
    QVERIFY(!cfr.isActive());
    QVERIFY(!cfr.isFinished());
    QVERIFY(!cfr.cancel());
    QVERIFY(!cfr.waitForFinished());
    qRegisterMetaType<QContactFetchRequest*>("QContactFetchRequest*");
    QThreadSignalSpy spy(&cfr, SIGNAL(stateChanged(QContactAbstractRequest::State)));
    cfr.setFilter(fil);
    QCOMPARE(cfr.filter(), fil);
    QVERIFY(!cfr.cancel()); // not started

    QVERIFY(cfr.start());
    //QVERIFY(cfr.isFinished() || !cfr.start());  // already started. // thread scheduling means this is untestable
    QVERIFY((cfr.isActive() && cfr.state() == QContactAbstractRequest::ActiveState) || cfr.isFinished());
    QVERIFY(cfr.waitForFinished());
    QVERIFY(cfr.isFinished());

    QVERIFY(spy.count() >= 1); // active + finished progress signals
    spy.clear();

    QList<QContactLocalId> contactIds = cm->contactIds();
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
    QVERIFY((cfr.isActive() && cfr.state() == QContactAbstractRequest::ActiveState) || cfr.isFinished());
    //QVERIFY(cfr.isFinished() || !cfr.start());  // already started. // thread scheduling means this is untestable
    QVERIFY(cfr.waitForFinished());
    QVERIFY(cfr.isFinished());
    QVERIFY(spy.count() >= 1); // active + finished progress signals
    spy.clear();

    contactIds = cm->contactIds(dfil);
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
    QVERIFY((cfr.isActive() && cfr.state() == QContactAbstractRequest::ActiveState) || cfr.isFinished());
    //QVERIFY(cfr.isFinished() || !cfr.start());  // already started. // thread scheduling means this is untestable
    QVERIFY(cfr.waitForFinished());
    QVERIFY(cfr.isFinished());

    QVERIFY(spy.count() >= 1); // active + finished progress signals
    spy.clear();

    contactIds = cm->contactIds(sorting);
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
    QContactFetchHint fetchHint;
    fetchHint.setDetailDefinitionsHint(QStringList(QContactName::DefinitionName));
    cfr.setFetchHint(fetchHint);
    QCOMPARE(cfr.fetchHint().detailDefinitionsHint(), QStringList(QContactName::DefinitionName));
    QVERIFY(!cfr.cancel()); // not started
    QVERIFY(cfr.start());
    QVERIFY((cfr.isActive() && cfr.state() == QContactAbstractRequest::ActiveState) || cfr.isFinished());
    //QVERIFY(cfr.isFinished() || !cfr.start());  // already started. // thread scheduling means this is untestable
    QVERIFY(cfr.waitForFinished());
    QVERIFY(cfr.isFinished());

    QVERIFY(spy.count() >= 1); // active + finished progress signals
    spy.clear();

    contactIds = cm->contactIds(sorting);
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
        QList<QContactDetail> retrievedDetails = retrievedRestricted.details();
        QList<QContactDetail> expectedDetails = currRestricted.details();
        foreach (const QContactDetail& det, expectedDetails) {
            // ignore backend synthesised details
            // again, this requires a "default contact details" function to work properly.
            if (det.definitionName() == QContactDisplayLabel::DefinitionName
                || det.definitionName() == QContactTimestamp::DefinitionName) {
                continue;
            }

            // everything else in the expected contact should be in the retrieved one.
            QVERIFY(retrievedDetails.contains(det));
        }
    }

    // cancelling
    sorting.clear();
    cfr.setFilter(fil);
    cfr.setSorting(sorting);
    cfr.setFetchHint(QContactFetchHint());

    int bailoutCount = MAX_OPTIMISTIC_SCHEDULING_LIMIT; // attempt to cancel 40 times.  If it doesn't work due to threading, bail out.
    while (true) {
        QVERIFY(!cfr.cancel()); // not started
        FILL_QUEUE_WITH_FETCH_REQUESTS();
        QVERIFY(cfr.start());
        if (!cfr.cancel()) {
            // due to thread scheduling, async cancel might be attempted
            // after the request has already finished.. so loop and try again.
            spy.clear();
            cfr.waitForFinished();
            sorting.clear();
            cfr.setFilter(fil);
            cfr.setSorting(sorting);
            cfr.setFetchHint(QContactFetchHint());
            cfr.setFetchHint(QContactFetchHint());
            bailoutCount -= 1;
            if (!bailoutCount) {
//                qWarning("Unable to test cancelling due to thread scheduling!");
                bailoutCount = MAX_OPTIMISTIC_SCHEDULING_LIMIT;
                break;
            }
            continue;
        }

        // if we get here, then we are cancelling the request.
        QVERIFY(cfr.waitForFinished());
        QVERIFY(cfr.isCanceled());

        QVERIFY(spy.count() >= 1); // active + cancelled progress signals
        spy.clear();
        break;
    }

    // restart, and wait for progress after cancel.
    while (true) {
        QVERIFY(!cfr.cancel()); // not started
        FILL_QUEUE_WITH_FETCH_REQUESTS();
        QVERIFY(cfr.start());
        if (!cfr.cancel()) {
            // due to thread scheduling, async cancel might be attempted
            // after the request has already finished.. so loop and try again.
            cfr.waitForFinished();
            sorting.clear();
            cfr.setFilter(fil);
            cfr.setSorting(sorting);
            cfr.setFetchHint(QContactFetchHint());
            bailoutCount -= 1;
            spy.clear();
            if (!bailoutCount) {
                //qWarning("Unable to test cancelling due to thread scheduling!");
                bailoutCount = MAX_OPTIMISTIC_SCHEDULING_LIMIT;
                break;
            }
            continue;
        }
        cfr.waitForFinished();
        QVERIFY(spy.count() >= 1); // active + cancelled progress signals
        spy.clear();
        QVERIFY(!cfr.isActive());
        QVERIFY(cfr.state() == QContactAbstractRequest::CanceledState);
        break;
    }

    // Now test deletion in the first slot called
    QContactFetchRequest *cfr2 = new QContactFetchRequest();
    QPointer<QObject> obj(cfr2);
    cfr2->setManager(cm.data());
    connect(cfr2, SIGNAL(stateChanged(QContactAbstractRequest::State)), this, SLOT(deleteRequest()));
    QVERIFY(cfr2->start());
    int i = 100;
    // at this point we can't even call wait for finished..
    while(obj && i > 0) {
        QTest::qWait(50); // force it to process events at least once.
        i--;
    }
    QVERIFY(obj == NULL);
}

void tst_QContactAsync::contactFetchById()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(prepareModel(uri));

    QContactFetchByIdRequest cfr;
    QVERIFY(cfr.type() == QContactAbstractRequest::ContactFetchByIdRequest);

    // initial state - not started, no manager.
    QVERIFY(!cfr.isActive());
    QVERIFY(!cfr.isFinished());
    QVERIFY(!cfr.start());
    QVERIFY(!cfr.cancel());
    QVERIFY(!cfr.waitForFinished());

    // get all contact ids
    QList<QContactLocalId> contactIds(cm->contactIds());

    // "all contacts" retrieval
    cfr.setManager(cm.data());
    cfr.setLocalIds(contactIds);
    QCOMPARE(cfr.manager(), cm.data());
    QVERIFY(!cfr.isActive());
    QVERIFY(!cfr.isFinished());
    QVERIFY(!cfr.cancel());
    QVERIFY(!cfr.waitForFinished());
    qRegisterMetaType<QContactFetchByIdRequest*>("QContactFetchByIdRequest*");
    QThreadSignalSpy spy(&cfr, SIGNAL(stateChanged(QContactAbstractRequest::State)));
    QVERIFY(!cfr.cancel()); // not started

    QVERIFY(cfr.start());
    //QVERIFY(cfr.isFinished() || !cfr.start());  // already started. // thread scheduling means this is untestable
    QVERIFY((cfr.isActive() && cfr.state() == QContactAbstractRequest::ActiveState) || cfr.isFinished());
    QVERIFY(cfr.waitForFinished());
    QVERIFY(cfr.isFinished());

    QVERIFY(spy.count() >= 1); // active + finished progress signals
    spy.clear();

    QList<QContact> contacts = cfr.contacts();
    QCOMPARE(contactIds.size(), contacts.size());
    for (int i = 0; i < contactIds.size(); i++) {
        QContact curr = cm->contact(contactIds.at(i));
        QVERIFY(contacts.at(i) == curr);
    }
}


void tst_QContactAsync::contactIdFetch()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(prepareModel(uri));
    QContactLocalIdFetchRequest cfr;
    QVERIFY(cfr.type() == QContactAbstractRequest::ContactLocalIdFetchRequest);

    // initial state - not started, no manager.
    QVERIFY(!cfr.isActive());
    QVERIFY(!cfr.isFinished());
    QVERIFY(!cfr.start());
    QVERIFY(!cfr.cancel());
    QVERIFY(!cfr.waitForFinished());

    // "all contacts" retrieval
    QContactFilter fil;
    cfr.setManager(cm.data());
    QCOMPARE(cfr.manager(), cm.data());
    QVERIFY(!cfr.isActive());
    QVERIFY(!cfr.isFinished());
    QVERIFY(!cfr.cancel());
    QVERIFY(!cfr.waitForFinished());
    qRegisterMetaType<QContactLocalIdFetchRequest*>("QContactLocalIdFetchRequest*");

    QThreadSignalSpy spy(&cfr, SIGNAL(stateChanged(QContactAbstractRequest::State)));
    cfr.setFilter(fil);
    QCOMPARE(cfr.filter(), fil);
    QVERIFY(!cfr.cancel()); // not started
    QVERIFY(cfr.start());

    QVERIFY((cfr.isActive() &&cfr.state() == QContactAbstractRequest::ActiveState) || cfr.isFinished());
    //QVERIFY(cfr.isFinished() || !cfr.start());  // already started. // thread scheduling means this is untestable
    QVERIFY(cfr.waitForFinished());
    QVERIFY(cfr.isFinished());

    QVERIFY(spy.count() >= 1); // active + finished progress signals
    spy.clear();

    QList<QContactLocalId> contactIds = cm->contactIds();
    QList<QContactLocalId> result = cfr.ids();
    QCOMPARE(contactIds, result);

    // asynchronous detail filtering
    QContactDetailFilter dfil;
    dfil.setDetailDefinitionName(QContactUrl::DefinitionName, QContactUrl::FieldUrl);
    cfr.setFilter(dfil);
    QVERIFY(cfr.filter() == dfil);
    QVERIFY(!cfr.cancel()); // not started

    QVERIFY(cfr.start());
    QVERIFY((cfr.isActive() && cfr.state() == QContactAbstractRequest::ActiveState) || cfr.isFinished());
    //QVERIFY(cfr.isFinished() || !cfr.start());  // already started. // thread scheduling means this is untestable
    QVERIFY(cfr.waitForFinished());
    QVERIFY(cfr.isFinished());

    QVERIFY(spy.count() >= 1); // active + finished progress signals
    spy.clear();

    contactIds = cm->contactIds(dfil);
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
    QVERIFY((cfr.isActive() && cfr.state() == QContactAbstractRequest::ActiveState) || cfr.isFinished());
    //QVERIFY(cfr.isFinished() || !cfr.start());  // already started. // thread scheduling means this is untestable
    QVERIFY(cfr.waitForFinished());
    QVERIFY(cfr.isFinished());

    QVERIFY(spy.count() >= 1); // active + finished progress signals
    spy.clear();

    contactIds = cm->contactIds(sorting);
    result = cfr.ids();
    QCOMPARE(contactIds, result);

    // cancelling
    sorting.clear();
    cfr.setFilter(fil);
    cfr.setSorting(sorting);

    int bailoutCount = MAX_OPTIMISTIC_SCHEDULING_LIMIT; // attempt to cancel 40 times.  If it doesn't work due to threading, bail out.
    while (true) {
        QVERIFY(!cfr.cancel()); // not started
        FILL_QUEUE_WITH_FETCH_REQUESTS();
        QVERIFY(cfr.start());
        if (!cfr.cancel()) {
            // due to thread scheduling, async cancel might be attempted
            // after the request has already finished.. so loop and try again.
            cfr.waitForFinished();
            sorting.clear();
            cfr.setFilter(fil);
            cfr.setSorting(sorting);
            bailoutCount -= 1;
            spy.clear();
            if (!bailoutCount) {
//                qWarning("Unable to test cancelling due to thread scheduling!");
                bailoutCount = MAX_OPTIMISTIC_SCHEDULING_LIMIT;
                break;
            }
            continue;
        }

        // if we get here, then we are cancelling the request.
        QVERIFY(cfr.waitForFinished());
        QVERIFY(cfr.isCanceled());

        QVERIFY(spy.count() >= 1); // active + cancelled progress signals
        spy.clear();

        break;
    }

    // restart, and wait for progress after cancel.
    while (true) {
        QVERIFY(!cfr.cancel()); // not started
        FILL_QUEUE_WITH_FETCH_REQUESTS();
        QVERIFY(cfr.start());
        if (!cfr.cancel()) {
            // due to thread scheduling, async cancel might be attempted
            // after the request has already finished.. so loop and try again.
            cfr.waitForFinished();
            sorting.clear();
            cfr.setFilter(fil);
            cfr.setSorting(sorting);
            bailoutCount -= 1;
            if (!bailoutCount) {
//                qWarning("Unable to test cancelling due to thread scheduling!");
                bailoutCount = MAX_OPTIMISTIC_SCHEDULING_LIMIT;
                break;
            }
            continue;
        }
        cfr.waitForFinished();
        QVERIFY(cfr.isCanceled());

        QVERIFY(spy.count() >= 1); // active + cancelled progress signals
        spy.clear();
        break;
    }

}

void tst_QContactAsync::contactRemove()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(prepareModel(uri));
    QContactRemoveRequest crr;
    QVERIFY(crr.type() == QContactAbstractRequest::ContactRemoveRequest);

    // initial state - not started, no manager.
    QVERIFY(!crr.isActive());
    QVERIFY(!crr.isFinished());
    QVERIFY(!crr.start());
    QVERIFY(!crr.cancel());
    QVERIFY(!crr.waitForFinished());

    // specific contact set
    crr.setContactId(QContactLocalId(3));
    QVERIFY(crr.contactIds() == QList<QContactLocalId>() << QContactLocalId(3));

    // specific contact removal via detail filter
    int originalCount = cm->contactIds().size();
    QContactDetailFilter dfil;
    dfil.setDetailDefinitionName(QContactUrl::DefinitionName, QContactUrl::FieldUrl);
    crr.setContactIds(cm->contactIds(dfil));
    crr.setManager(cm.data());
    QCOMPARE(crr.manager(), cm.data());
    QVERIFY(!crr.isActive());
    QVERIFY(!crr.isFinished());
    QVERIFY(!crr.cancel());
    QVERIFY(!crr.waitForFinished());
    qRegisterMetaType<QContactRemoveRequest*>("QContactRemoveRequest*");
    QThreadSignalSpy spy(&crr, SIGNAL(stateChanged(QContactAbstractRequest::State)));
    QVERIFY(!crr.cancel()); // not started

    QVERIFY(!cm->contactIds(dfil).isEmpty());

    QVERIFY(crr.start());

    QVERIFY((crr.isActive() &&crr.state() == QContactAbstractRequest::ActiveState) || crr.isFinished());
    //QVERIFY(crr.isFinished() || !crr.start());  // already started. // thread scheduling means this is untestable
    QVERIFY(crr.waitForFinished());
    QVERIFY(crr.isFinished());

    QVERIFY(spy.count() >= 1); // active + finished progress signals
    spy.clear();

    QCOMPARE(cm->contactIds().size(), originalCount - 1);
    QVERIFY(cm->contactIds(dfil).isEmpty());

    // remove all contacts
    dfil.setDetailDefinitionName(QContactDisplayLabel::DefinitionName); // delete everything.
    crr.setContactIds(cm->contactIds(dfil));
    
    QVERIFY(!crr.cancel()); // not started
    QVERIFY(crr.start());

    QVERIFY((crr.isActive() && crr.state() == QContactAbstractRequest::ActiveState) || crr.isFinished());
    //QVERIFY(crr.isFinished() || !crr.start());  // already started. // thread scheduling means this is untestable
    QVERIFY(crr.waitForFinished());
    QVERIFY(crr.isFinished());

    QCOMPARE(cm->contactIds().size(), 0); // no contacts should be left.
    QVERIFY(spy.count() >= 1); // active + finished progress signals
    spy.clear();

    // cancelling
    QContact temp;
    QContactName nameDetail;
    nameDetail.setFirstName("Should not be removed");
    temp.saveDetail(&nameDetail);
    cm->saveContact(&temp);
    crr.setContactIds(cm->contactIds(dfil));

    int bailoutCount = MAX_OPTIMISTIC_SCHEDULING_LIMIT; // attempt to cancel 40 times.  If it doesn't work due to threading, bail out.
    while (true) {
        QVERIFY(!crr.cancel()); // not started
        FILL_QUEUE_WITH_FETCH_REQUESTS();
        QVERIFY(spy.count() == 0);
        QVERIFY(crr.start());
        if (!crr.cancel()) {
            // due to thread scheduling, async cancel might be attempted
            // after the request has already finished.. so loop and try again.
            crr.waitForFinished();
            crr.setContactIds(cm->contactIds(dfil));
            temp.setId(QContactId());
            if (!cm->saveContact(&temp)) {
                QSKIP("Unable to save temporary contact for remove request cancellation test!", SkipSingle);
            }
            bailoutCount -= 1;
            if (!bailoutCount) {
//                qWarning("Unable to test cancelling due to thread scheduling!");
                bailoutCount = MAX_OPTIMISTIC_SCHEDULING_LIMIT;
                break;
            }
            spy.clear();
            continue;
        }

        // if we get here, then we are cancelling the request.
        QVERIFY(crr.waitForFinished());
        QVERIFY(crr.isCanceled());
        QCOMPARE(cm->contactIds().size(), 1);
        QCOMPARE(cm->contactIds(), crr.contactIds());
        QVERIFY(spy.count() >= 1); // active + cancelled progress signals
        spy.clear();
        break;
    }

    // restart, and wait for progress after cancel.
    while (true) {
        QVERIFY(!crr.cancel()); // not started
        FILL_QUEUE_WITH_FETCH_REQUESTS();
        QVERIFY(crr.start());
        if (!crr.cancel()) {
            // due to thread scheduling, async cancel might be attempted
            // after the request has already finished.. so loop and try again.
            crr.waitForFinished();
            crr.setContactIds(cm->contactIds(dfil));
            temp.setId(QContactId());
            cm->saveContact(&temp);
            bailoutCount -= 1;
            if (!bailoutCount) {
//                qWarning("Unable to test cancelling due to thread scheduling!");
                bailoutCount = MAX_OPTIMISTIC_SCHEDULING_LIMIT;
                break;
            }
            spy.clear();
            continue;
        }
        crr.waitForFinished();
        QVERIFY(crr.isCanceled());
        QCOMPARE(cm->contactIds().size(), 1);
        QCOMPARE(cm->contactIds(), crr.contactIds());
        QVERIFY(spy.count() >= 1); // active + cancelled progress signals
        spy.clear();
        break;
    }

}

void tst_QContactAsync::contactSave()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(prepareModel(uri));
    QContactSaveRequest csr;
    QVERIFY(csr.type() == QContactAbstractRequest::ContactSaveRequest);

    // initial state - not started, no manager.
    QVERIFY(!csr.isActive());
    QVERIFY(!csr.isFinished());
    QVERIFY(!csr.start());
    QVERIFY(!csr.cancel());
    QVERIFY(!csr.waitForFinished());

    // save a new contact
    int originalCount = cm->contactIds().size();
    QContact testContact;
    QContactName nameDetail;
    nameDetail.setFirstName("Test Contact");
    testContact.saveDetail(&nameDetail);
    QList<QContact> saveList;
    saveList << testContact;
    csr.setManager(cm.data());
    QCOMPARE(csr.manager(), cm.data());
    QVERIFY(!csr.isActive());
    QVERIFY(!csr.isFinished());
    QVERIFY(!csr.cancel());
    QVERIFY(!csr.waitForFinished());
    qRegisterMetaType<QContactSaveRequest*>("QContactSaveRequest*");
    QThreadSignalSpy spy(&csr, SIGNAL(stateChanged(QContactAbstractRequest::State)));
    csr.setContact(testContact);
    QCOMPARE(csr.contacts(), saveList);
    QVERIFY(!csr.cancel()); // not started
    QVERIFY(csr.start());

    QVERIFY((csr.isActive() && csr.state() == QContactAbstractRequest::ActiveState) || csr.isFinished());
    //QVERIFY(csr.isFinished() || !csr.start());  // already started. // thread scheduling means this is untestable
    QVERIFY(csr.waitForFinished());
    QVERIFY(csr.isFinished());
    QVERIFY(spy.count() >= 1); // active + finished progress signals
    spy.clear();

    QList<QContact> expected = csr.contacts();
    QCOMPARE(expected.size(), 1);
    QList<QContact> result;
    result << cm->contact(expected.first().id().localId());
    //some backends add extra fields, so this doesn't work:
    //QCOMPARE(result, expected);
    // XXX: really, we should use isSuperset() from tst_QContactManager, but this will do for now:
    QVERIFY(result.first().detail<QContactName>() == nameDetail);
    QCOMPARE(cm->contactIds().size(), originalCount + 1);

    // update a previously saved contact
    QContactPhoneNumber phn;
    phn.setNumber("12345678");
    testContact = result.first();
    testContact.saveDetail(&phn);
    saveList.clear();
    saveList << testContact;
    csr.setContacts(saveList);
    QCOMPARE(csr.contacts(), saveList);
    QVERIFY(!csr.cancel()); // not started
    QVERIFY(csr.start());

    QVERIFY((csr.isActive() && csr.state() == QContactAbstractRequest::ActiveState) || csr.isFinished());
    //QVERIFY(csr.isFinished() || !csr.start());  // already started. // thread scheduling means this is untestable
    QVERIFY(csr.waitForFinished());

    QVERIFY(csr.isFinished());
    QVERIFY(spy.count() >= 1); // active + finished progress signals
    spy.clear();

    expected = csr.contacts();
    result.clear();
    result << cm->contact(expected.first().id().localId());
    //QVERIFY(compareContactLists(result, expected));

    //here we can't compare the whole contact details, testContact would be updated by async call because we just use QThreadSignalSpy to receive signals.
    //QVERIFY(containsIgnoringTimestamps(result, testContact));
    // XXX: really, we should use isSuperset() from tst_QContactManager, but this will do for now:
    QVERIFY(result.first().detail<QContactPhoneNumber>().number() == phn.number());
    
    QCOMPARE(cm->contactIds().size(), originalCount + 1);

    // cancelling
    QContact temp = testContact;
    QContactUrl url;
    url.setUrl("should not get saved");
    temp.saveDetail(&url);
    saveList.clear();
    saveList << temp;
    csr.setContacts(saveList);

    int bailoutCount = MAX_OPTIMISTIC_SCHEDULING_LIMIT; // attempt to cancel 40 times.  If it doesn't work due to threading, bail out.
    while (true) {
        QVERIFY(!csr.cancel()); // not started
        FILL_QUEUE_WITH_FETCH_REQUESTS();
        QVERIFY(csr.start());
        if (!csr.cancel()) {
            // due to thread scheduling, async cancel might be attempted
            // after the request has already finished.. so loop and try again.
            csr.waitForFinished();
            saveList = csr.contacts();
            if (cm->contactIds().size() > (originalCount + 1) && !cm->removeContact(saveList.at(0).localId())) {
                QSKIP("Unable to remove saved contact to test cancellation of contact save request", SkipSingle);
            }
            saveList.clear();
            saveList << temp;
            csr.setContacts(saveList);
            bailoutCount -= 1;
            if (!bailoutCount) {
//                qWarning("Unable to test cancelling due to thread scheduling!");
                bailoutCount = MAX_OPTIMISTIC_SCHEDULING_LIMIT;
                break;
            }
            spy.clear();
            continue;
        }

        // if we get here, then we are cancelling the request.
        QVERIFY(csr.waitForFinished());
        QVERIFY(csr.isCanceled());
        QVERIFY(spy.count() >= 1); // active + cancelled progress signals
        spy.clear();

        // verify that the changes were not saved
        expected.clear();
        QList<QContactLocalId> allContacts = cm->contactIds();
        for (int i = 0; i < allContacts.size(); i++) {
            expected.append(cm->contact(allContacts.at(i)));
        }
        QVERIFY(!expected.contains(temp));
        QCOMPARE(cm->contactIds().size(), originalCount + 1);
        break;
    }
    // restart, and wait for progress after cancel.

    while (true) {
        QVERIFY(!csr.cancel()); // not started
        FILL_QUEUE_WITH_FETCH_REQUESTS();
        QVERIFY(csr.start());
        if (!csr.cancel()) {
            // due to thread scheduling, async cancel might be attempted
            // after the request has already finished.. so loop and try again.
            csr.waitForFinished();
            saveList = csr.contacts();
            if (cm->contactIds().size() > (originalCount + 1) && !cm->removeContact(saveList.at(0).localId())) {
                QSKIP("Unable to remove saved contact to test cancellation of contact save request", SkipSingle);
            }
            saveList.clear();
            saveList << temp;
            csr.setContacts(saveList);
            bailoutCount -= 1;
            if (!bailoutCount) {
//                qWarning("Unable to test cancelling due to thread scheduling!");
                bailoutCount = MAX_OPTIMISTIC_SCHEDULING_LIMIT;
                break;
            }
            spy.clear();
            continue;
        }
        csr.waitForFinished(); // now wait until finished (if it hasn't already).
        QVERIFY(csr.isCanceled());
        QVERIFY(spy.count() >= 1); // active + cancelled progress signals
        spy.clear();

        // verify that the changes were not saved
        expected.clear();
        QList<QContactLocalId> allContacts = cm->contactIds();
        for (int i = 0; i < allContacts.size(); i++) {
            expected.append(cm->contact(allContacts.at(i)));
        }
        QVERIFY(!expected.contains(temp));
        QCOMPARE(cm->contactIds().size(), originalCount + 1);
        break;
    }
}

void tst_QContactAsync::contactPartialSave()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(prepareModel(uri));

    QList<QContact> contacts(cm->contacts());
    QList<QContact> originalContacts(contacts);
    QCOMPARE(contacts.count(), 3);

    QContactLocalId aId = contacts[0].localId();
    QContactLocalId bId = contacts[1].localId();

    // Test 1: saving a contact with a changed detail masked out does nothing
    QContactPhoneNumber phn(contacts[0].detail<QContactPhoneNumber>());
    phn.setNumber("new number");
    contacts[0].saveDetail(&phn);

    QContactSaveRequest csr;
    csr.setManager(cm.data());
    csr.setContacts(contacts);
    csr.setDefinitionMask(QStringList(QContactEmailAddress::DefinitionName));
    qRegisterMetaType<QContactSaveRequest*>("QContactSaveRequest*");
    QThreadSignalSpy spy(&csr, SIGNAL(stateChanged(QContactAbstractRequest::State)));
    QVERIFY(csr.start());
    QVERIFY((csr.isActive() && csr.state() == QContactAbstractRequest::ActiveState) || csr.isFinished());
    QVERIFY(csr.waitForFinished());
    QVERIFY(csr.isFinished());
    QVERIFY(spy.count() >= 1); // active + finished progress signals
    spy.clear();

    QCOMPARE(csr.error(), QContactManager::NoError);
    QVERIFY(csr.errorMap().isEmpty());
    contacts[0] = cm->contact(aId);
    QCOMPARE(contacts[0].detail<QContactPhoneNumber>().number(),
            originalContacts[0].detail<QContactPhoneNumber>().number());

    // Test 2: saving a contact with a changed detail in the mask changes it
    QContactEmailAddress email;
    email.setEmailAddress("me@example.com");
    contacts[1].saveDetail(&email);
    csr.setContacts(contacts);
    csr.setDefinitionMask(QStringList(QContactEmailAddress::DefinitionName));
    QVERIFY(csr.start());
    QVERIFY(csr.waitForFinished());
    QCOMPARE(csr.error(), QContactManager::NoError);
    QVERIFY(csr.errorMap().isEmpty());
    contacts[1] = cm->contact(bId);
    QCOMPARE(contacts[1].detail<QContactEmailAddress>().emailAddress(), QString("me@example.com"));

    // 3) Remove an email address and a phone number
    QCOMPARE(contacts[1].details<QContactPhoneNumber>().count(), 1);
    QCOMPARE(contacts[1].details<QContactEmailAddress>().count(), 1);
    QVERIFY(contacts[1].removeDetail(&email));
    phn = contacts[1].detail<QContactPhoneNumber>();
    QVERIFY(contacts[1].removeDetail(&phn));
    QVERIFY(contacts[1].details<QContactEmailAddress>().count() == 0);
    QVERIFY(contacts[1].details<QContactPhoneNumber>().count() == 0);
    csr.setContacts(contacts);
    csr.setDefinitionMask(QStringList(QContactEmailAddress::DefinitionName));
    QVERIFY(csr.start());
    QVERIFY(csr.waitForFinished());
    QCOMPARE(csr.error(), QContactManager::NoError);
    QVERIFY(csr.errorMap().isEmpty());
    contacts[1] = cm->contact(bId);
    QCOMPARE(contacts[1].details<QContactEmailAddress>().count(), 0);
    QCOMPARE(contacts[1].details<QContactPhoneNumber>().count(), 1);

    // 4 - New contact, no details in the mask
    QContact newContact;
    newContact.saveDetail(&email);
    newContact.saveDetail(&phn);
    contacts.append(newContact);
    csr.setContacts(contacts);
    csr.setDefinitionMask(QStringList(QContactOnlineAccount::DefinitionName));
    QVERIFY(csr.start());
    QVERIFY(csr.waitForFinished());
    QCOMPARE(csr.error(), QContactManager::NoError);
    QVERIFY(csr.errorMap().isEmpty());
    contacts = csr.contacts();
    QCOMPARE(contacts.size()-1, 3);  // Just check that we are dealing with the contact at index 3
    QContactLocalId dId = contacts[3].localId();
    contacts[3] = cm->contact(dId);
    QVERIFY(contacts[3].details<QContactEmailAddress>().count() == 0); // not saved
    QVERIFY(contacts[3].details<QContactPhoneNumber>().count() == 0); // not saved

    // 5 - New contact, some details in the mask
    QVERIFY(newContact.localId() == 0);
    QVERIFY(newContact.details<QContactEmailAddress>().count() == 1);
    QVERIFY(newContact.details<QContactPhoneNumber>().count() == 1);
    contacts.append(newContact);
    csr.setContacts(contacts);
    csr.setDefinitionMask(QStringList(QContactPhoneNumber::DefinitionName));
    QVERIFY(csr.start());
    QVERIFY(csr.waitForFinished());
    QCOMPARE(csr.error(), QContactManager::NoError);
    QVERIFY(csr.errorMap().isEmpty());
    contacts = csr.contacts();
    QCOMPARE(contacts.size()-1, 4);  // Just check that we are dealing with the contact at index 4
    QContactLocalId eId = contacts[4].localId();
    contacts[4] = cm->contact(eId);
    QCOMPARE(contacts[4].details<QContactEmailAddress>().count(), 0); // not saved
    QCOMPARE(contacts[4].details<QContactPhoneNumber>().count(), 1); // saved

    // 6) Have a bad manager uri in the middle followed by a save error
    QContactId id3(contacts[3].id());
    QContactId badId(id3);
    badId.setManagerUri(QString());
    contacts[3].setId(badId);
    QContactDetail badDetail("BadDetail");
    badDetail.setValue("BadField", "BadValue");
    contacts[4].saveDetail(&badDetail);
    csr.setContacts(contacts);
    csr.setDefinitionMask(QStringList("BadDetail"));
    QVERIFY(csr.start());
    QVERIFY(csr.waitForFinished());
    QVERIFY(csr.error() != QContactManager::NoError);
    QMap<int, QContactManager::Error> errorMap(csr.errorMap());
    QCOMPARE(errorMap.count(), 2);
    QCOMPARE(errorMap[3], QContactManager::DoesNotExistError);
    QCOMPARE(errorMap[4], QContactManager::InvalidDetailError);

    // 7) Have a non existing contact in the middle followed by a save error
    badId = id3;
    badId.setLocalId(987234); // something nonexistent (hopefully)
    contacts[3].setId(badId);
    csr.setContacts(contacts);
    csr.setDefinitionMask(QStringList("BadDetail"));
    QVERIFY(csr.start());
    QVERIFY(csr.waitForFinished());
    QVERIFY(csr.error() != QContactManager::NoError);
    errorMap = csr.errorMap();
    QCOMPARE(errorMap.count(), 2);
    QCOMPARE(errorMap[3], QContactManager::DoesNotExistError);
    QCOMPARE(errorMap[4], QContactManager::InvalidDetailError);

    // 8) A list entirely of new contacts, with no details in the mask
    QList<QContact> contacts2;
    QVERIFY(newContact.localId() == 0);
    QVERIFY(newContact.details<QContactEmailAddress>().count() == 1);
    QVERIFY(newContact.details<QContactPhoneNumber>().count() == 1);
    contacts2.append(newContact);
    csr.setContacts(contacts2);
    csr.setDefinitionMask(QStringList(QContactOrganization::DefinitionName));
    QVERIFY(csr.start());
    QVERIFY(csr.waitForFinished());
    QCOMPARE(csr.error(), QContactManager::NoError);
    QVERIFY(csr.errorMap().isEmpty());
    contacts2 = csr.contacts();
    QCOMPARE(contacts2.size(), 1);
    contacts2[0] = cm->contact(contacts2[0].localId());
    QCOMPARE(contacts2[0].details<QContactEmailAddress>().count(), 0); // not saved
    QCOMPARE(contacts2[0].details<QContactPhoneNumber>().count(), 0); // saved

    // 9) A list entirely of new contacts, with some details in the mask
    contacts2.clear();
    QVERIFY(newContact.localId() == 0);
    QVERIFY(newContact.details<QContactEmailAddress>().count() == 1);
    QVERIFY(newContact.details<QContactPhoneNumber>().count() == 1);
    contacts2.append(newContact);
    csr.setContacts(contacts2);
    csr.setDefinitionMask(QStringList(QContactPhoneNumber::DefinitionName));
    QVERIFY(csr.start());
    QVERIFY(csr.waitForFinished());
    QCOMPARE(csr.error(), QContactManager::NoError);
    QVERIFY(csr.errorMap().isEmpty());
    contacts2 = csr.contacts();
    QCOMPARE(contacts2.size(), 1);
    contacts2[0] = cm->contact(contacts2[0].localId());
    QCOMPARE(contacts2[0].details<QContactEmailAddress>().count(), 0); // not saved
    QCOMPARE(contacts2[0].details<QContactPhoneNumber>().count(), 1); // saved

    // 10) A list entirely of new contacts, with some details in the mask
    contacts2.clear();
    QVERIFY(newContact.localId() == 0);
    QVERIFY(newContact.details<QContactEmailAddress>().count() == 1);
    QVERIFY(newContact.details<QContactPhoneNumber>().count() == 1);
    contacts2.append(newContact);
    contacts2.append(newContact);
    contacts2[0].setId(badId);
    contacts2[1].saveDetail(&badDetail);
    csr.setContacts(contacts2);
    csr.setDefinitionMask(QStringList("BadDetail"));
    QVERIFY(csr.start());
    QVERIFY(csr.waitForFinished());
    QVERIFY(csr.error() != QContactManager::NoError);
    errorMap = csr.errorMap();
    QCOMPARE(errorMap.count(), 2);
    QCOMPARE(errorMap[0], QContactManager::DoesNotExistError);
    QCOMPARE(errorMap[1], QContactManager::InvalidDetailError);
}

void tst_QContactAsync::contactPartialSaveAsync()
{
    QFETCH(QString, uri);
    QContactManager* cm = QContactManager::fromUri(uri);

    QList<QContact> contacts;

    int numContacts = 10;
    // add contacts
    for (int i = 0; i < numContacts; i++) {
        QContact c;
        QContactName name;
        name.setFirstName("John");
        name.setMiddleName(QString::number(i));
        name.setLastName("Doe");
        QContactPhoneNumber phone;
        QString number = "555-100"+QString::number(i);
        phone.setNumber(number);
        c.saveDetail(&name);
        c.saveDetail(&phone);
        contacts.append(c);
    }

    QContactSaveRequest *saveRequest = new QContactSaveRequest();
    saveRequest->setManager(cm);
    saveRequest->setContacts(contacts);
    saveRequest->start();
    saveRequest->waitForFinished(20000);
    QVERIFY(saveRequest->isFinished());
    QCOMPARE(saveRequest->contacts().count(), numContacts);
    delete saveRequest;
    qRegisterMetaType<QContactAbstractRequest::State>("QContactAbstractRequest::State");

    saveRequest = new QContactSaveRequest();
    saveRequest->setManager(cm);
    saveRequest->setContacts(contacts);
    saveRequest->setDefinitionMask(QStringList(QContactTag::DefinitionName));
    saveRequest->start();
    QTest::qWait(1000);
    QVERIFY(saveRequest->isFinished());
}

void tst_QContactAsync::definitionFetch()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(prepareModel(uri));
    QContactDetailDefinitionFetchRequest dfr;
    QVERIFY(dfr.type() == QContactAbstractRequest::DetailDefinitionFetchRequest);
    QVERIFY(dfr.contactType() == QString(QLatin1String(QContactType::TypeContact))); // ensure ctor sets contact type correctly.
    dfr.setContactType(QContactType::TypeContact);
    QVERIFY(dfr.contactType() == QString(QLatin1String(QContactType::TypeContact)));

    // initial state - not started, no manager.
    QVERIFY(!dfr.isActive());
    QVERIFY(!dfr.isFinished());
    QVERIFY(!dfr.start());
    QVERIFY(!dfr.cancel());
    QVERIFY(!dfr.waitForFinished());

    // "all definitions" retrieval
    dfr.setManager(cm.data());
    QCOMPARE(dfr.manager(), cm.data());
    QVERIFY(!dfr.isActive());
    QVERIFY(!dfr.isFinished());
    QVERIFY(!dfr.cancel());
    QVERIFY(!dfr.waitForFinished());
    qRegisterMetaType<QContactDetailDefinitionFetchRequest*>("QContactDetailDefinitionFetchRequest*");
    QThreadSignalSpy spy(&dfr, SIGNAL(stateChanged(QContactAbstractRequest::State)));
    dfr.setDefinitionNames(QStringList());
    QVERIFY(!dfr.cancel()); // not started
    QVERIFY(dfr.start());

    QVERIFY((dfr.isActive() && dfr.state() == QContactAbstractRequest::ActiveState) || dfr.isFinished());
    //QVERIFY(dfr.isFinished() || !dfr.start());  // already started. // thread scheduling means this is untestable
    QVERIFY(dfr.waitForFinished());
    QVERIFY(dfr.isFinished());
    QVERIFY(spy.count() >= 1); // active + finished progress signals
    spy.clear();

    QMap<QString, QContactDetailDefinition> defs = cm->detailDefinitions();
    QMap<QString, QContactDetailDefinition> result = dfr.definitions();
    QCOMPARE(defs, result);

    // specific definition retrieval
    QStringList specific;
    specific << QContactUrl::DefinitionName;
    dfr.setDefinitionName(QContactUrl::DefinitionName);
    QVERIFY(dfr.definitionNames() == specific);
    QVERIFY(!dfr.cancel()); // not started
    QVERIFY(dfr.start());

    QVERIFY((dfr.isActive() && dfr.state() == QContactAbstractRequest::ActiveState) || dfr.isFinished());
    //QVERIFY(dfr.isFinished() || !dfr.start());  // already started. // thread scheduling means this is untestable
    QVERIFY(dfr.waitForFinished());
    QVERIFY(dfr.isFinished());
    QVERIFY(spy.count() >= 1); // active + finished progress signals
    spy.clear();

    defs.clear();
    defs.insert(QContactUrl::DefinitionName, cm->detailDefinition(QContactUrl::DefinitionName));
    result = dfr.definitions();
    QCOMPARE(defs, result);

    // cancelling
    dfr.setDefinitionNames(QStringList());

    int bailoutCount = MAX_OPTIMISTIC_SCHEDULING_LIMIT; // attempt to cancel 40 times.  If it doesn't work due to threading, bail out.
    while (true) {
        QVERIFY(!dfr.cancel()); // not started
        FILL_QUEUE_WITH_FETCH_REQUESTS();
        QVERIFY(dfr.start());
        if (!dfr.cancel()) {
            // due to thread scheduling, async cancel might be attempted
            // after the request has already finished.. so loop and try again.
            dfr.waitForFinished();
            dfr.setDefinitionNames(QStringList());
            bailoutCount -= 1;
            if (!bailoutCount) {
//                qWarning("Unable to test cancelling due to thread scheduling!");
                bailoutCount = MAX_OPTIMISTIC_SCHEDULING_LIMIT;
                break;
            }
            spy.clear();
            continue;
        }

        // if we get here, then we are cancelling the request.
        QVERIFY(dfr.waitForFinished());
        QVERIFY(dfr.isCanceled());
        QVERIFY(spy.count() >= 1); // active + cancelled progress signals
        spy.clear();
        break;
    }

    // restart, and wait for progress after cancel.
    while (true) {
        QVERIFY(!dfr.cancel()); // not started
        FILL_QUEUE_WITH_FETCH_REQUESTS();
        QVERIFY(dfr.start());
        if (!dfr.cancel()) {
            // due to thread scheduling, async cancel might be attempted
            // after the request has already finished.. so loop and try again.
            dfr.waitForFinished();
            dfr.setDefinitionNames(QStringList());
            bailoutCount -= 1;
            if (!bailoutCount) {
//                qWarning("Unable to test cancelling due to thread scheduling!");
                bailoutCount = MAX_OPTIMISTIC_SCHEDULING_LIMIT;
                break;
            }
            spy.clear();
            continue;
        }
        dfr.waitForFinished();
        QVERIFY(dfr.isCanceled());
        QVERIFY(spy.count() >= 1); // active + cancelled progress signals
        spy.clear();

        break;
    }

}

void tst_QContactAsync::definitionRemove()
{
    QFETCH(QString, uri);

    QScopedPointer<QContactManager> cm(prepareModel(uri));
    if (!cm->hasFeature(QContactManager::MutableDefinitions)) {
       QSKIP("This contact manager does not support mutable definitions, can't remove a definition!", SkipSingle);
    }
    QContactDetailDefinitionRemoveRequest drr;
    QVERIFY(drr.type() == QContactAbstractRequest::DetailDefinitionRemoveRequest);
    QVERIFY(drr.contactType() == QString(QLatin1String(QContactType::TypeContact))); // ensure ctor sets contact type correctly.
    drr.setContactType(QContactType::TypeContact);
    drr.setDefinitionNames(QStringList());
    QVERIFY(drr.contactType() == QString(QLatin1String(QContactType::TypeContact)));

    // initial state - not started, no manager.
    QVERIFY(!drr.isActive());
    QVERIFY(!drr.isFinished());
    QVERIFY(!drr.start());
    QVERIFY(!drr.cancel());
    QVERIFY(!drr.waitForFinished());

    // specific group removal
    int originalCount = cm->detailDefinitions().keys().size();
    QStringList removeIds;
    removeIds << cm->detailDefinitions().keys().first();
    drr.setDefinitionName(cm->detailDefinitions().keys().first());
    drr.setManager(cm.data());
    QCOMPARE(drr.manager(), cm.data());
    QVERIFY(!drr.isActive());
    QVERIFY(!drr.isFinished());
    QVERIFY(!drr.cancel());
    QVERIFY(!drr.waitForFinished());
    qRegisterMetaType<QContactDetailDefinitionRemoveRequest*>("QContactDetailDefinitionRemoveRequest*");
    QThreadSignalSpy spy(&drr, SIGNAL(stateChanged(QContactAbstractRequest::State)));
    QVERIFY(drr.definitionNames() == removeIds);
    QVERIFY(!drr.cancel()); // not started
    QVERIFY(drr.start());

    QVERIFY((drr.isActive() && drr.state() == QContactAbstractRequest::ActiveState) || drr.isFinished());
    //QVERIFY(drr.isFinished() || !drr.start());  // already started. // thread scheduling means this is untestable
    QVERIFY(drr.waitForFinished());
    QVERIFY(drr.isFinished());
    QVERIFY(spy.count() >= 1); // active + finished progress signals
    spy.clear();

    QCOMPARE(cm->detailDefinitions().keys().size(), originalCount - 1);
    cm->detailDefinition(removeIds.first()); // check that it has already been removed.
    QCOMPARE(cm->error(), QContactManager::DoesNotExistError);

    // remove (asynchronously) a nonexistent group - should fail.
    drr.setDefinitionNames(removeIds);
    QVERIFY(!drr.cancel()); // not started
    QVERIFY(drr.start());

    QVERIFY((drr.isActive() && drr.state() == QContactAbstractRequest::ActiveState) || drr.isFinished());
    //QVERIFY(drr.isFinished() || !drr.start());  // already started. // thread scheduling means this is untestable
    QVERIFY(drr.waitForFinished());
    QVERIFY(drr.isFinished());
    QVERIFY(spy.count() >= 1); // active + finished progress signals
    spy.clear();

    QCOMPARE(cm->detailDefinitions().keys().size(), originalCount - 1); // hasn't changed
    QCOMPARE(drr.error(), QContactManager::DoesNotExistError);

    // remove with list containing one valid and one invalid id.
    removeIds << cm->detailDefinitions().keys().first();
    drr.setDefinitionNames(removeIds);
    QVERIFY(!drr.cancel()); // not started
    QVERIFY(drr.start());

    QVERIFY((drr.isActive() && drr.state() == QContactAbstractRequest::ActiveState) || drr.isFinished());
    //QVERIFY(drr.isFinished() || !drr.start());  // already started. // thread scheduling means this is untestable
    QVERIFY(drr.waitForFinished());
    QVERIFY(drr.isFinished());
    QVERIFY(spy.count() >= 1); // active + finished signals
    spy.clear();

    QCOMPARE(cm->detailDefinitions().keys().size(), originalCount - 2); // only one more has been removed
    QVERIFY(drr.errorMap().count() == 1);
    QVERIFY(drr.errorMap().keys().contains(0));
    QCOMPARE(drr.errorMap().value(0), QContactManager::DoesNotExistError);

    // remove with empty list - nothing should happen.
    removeIds.clear();
    drr.setDefinitionNames(removeIds);
    QVERIFY(!drr.cancel()); // not started
    QVERIFY(drr.start());

    QVERIFY(drr.isActive() || drr.isFinished());
    //QVERIFY(drr.isFinished() || !drr.start());  // already started. // thread scheduling means this is untestable
    QVERIFY(drr.waitForFinished());

    QVERIFY(drr.isFinished());
    QVERIFY(spy.count() >= 1); // active + finished progress signals
    spy.clear();

    QCOMPARE(cm->detailDefinitions().keys().size(), originalCount - 2); // hasn't changed
    QCOMPARE(drr.error(), QContactManager::NoError);  // no error but no effect.

    // cancelling
    removeIds.clear();
    removeIds << cm->detailDefinitions().keys().first();
    drr.setDefinitionNames(removeIds);

    int bailoutCount = MAX_OPTIMISTIC_SCHEDULING_LIMIT; // attempt to cancel 40 times.  If it doesn't work due to threading, bail out.
    while (true) {
        QVERIFY(!drr.cancel()); // not started
        FILL_QUEUE_WITH_FETCH_REQUESTS();
        QVERIFY(drr.start());
        if (!drr.cancel()) {
            // due to thread scheduling, async cancel might be attempted
            // after the request has already finished.. so loop and try again.
            drr.waitForFinished();
            drr.setDefinitionNames(removeIds);

            QCOMPARE(cm->detailDefinitions().keys().size(), originalCount - 3); // finished
            bailoutCount -= 1;
            if (!bailoutCount) {
//                qWarning("Unable to test cancelling due to thread scheduling!");
                bailoutCount = MAX_OPTIMISTIC_SCHEDULING_LIMIT;
                break;
            }
            spy.clear();
            // XXX should be readded
            continue;
        }

        // if we get here, then we are cancelling the request.
        QVERIFY(drr.waitForFinished());
        QVERIFY(drr.isCanceled());
        QVERIFY(spy.count() >= 1); // active + cancelled progress signals
        spy.clear();

        QCOMPARE(cm->detailDefinitions().keys().size(), originalCount - 2); // hasn't changed
        break;
    }

    // restart, and wait for progress after cancel.
    while (true) {
        QVERIFY(!drr.cancel()); // not started
        FILL_QUEUE_WITH_FETCH_REQUESTS();
        QVERIFY(drr.start());
        if (!drr.cancel()) {
            // due to thread scheduling, async cancel might be attempted
            // after the request has already finished.. so loop and try again.
            drr.waitForFinished();
            drr.setDefinitionNames(removeIds);
            bailoutCount -= 1;
            if (!bailoutCount) {
//                qWarning("Unable to test cancelling due to thread scheduling!");
                bailoutCount = MAX_OPTIMISTIC_SCHEDULING_LIMIT;
                break;
            }
            spy.clear();
            continue;
        }
        drr.waitForFinished();
        QVERIFY(drr.isCanceled());
        QVERIFY(spy.count() >= 1); // active + cancelled progress signals
        spy.clear();

        QCOMPARE(cm->detailDefinitions().keys().size(), originalCount - 3); // hasn't changed
        break;
    }

}

void tst_QContactAsync::definitionSave()
{
    QFETCH(QString, uri);

    QScopedPointer<QContactManager> cm(prepareModel(uri));

    if (!cm->hasFeature(QContactManager::MutableDefinitions)) {

       QSKIP("This contact manager does not support mutable definitions, can't save a definition!", SkipSingle);
    }
    
    QContactDetailDefinitionSaveRequest dsr;
    QVERIFY(dsr.type() == QContactAbstractRequest::DetailDefinitionSaveRequest);
    QVERIFY(dsr.contactType() == QString(QLatin1String(QContactType::TypeContact))); // ensure ctor sets contact type correctly
    dsr.setContactType(QContactType::TypeContact);
    QVERIFY(dsr.contactType() == QString(QLatin1String(QContactType::TypeContact)));

    // initial state - not started, no manager.
    QVERIFY(!dsr.isActive());
    QVERIFY(!dsr.isFinished());
    QVERIFY(!dsr.start());
    QVERIFY(!dsr.cancel());
    QVERIFY(!dsr.waitForFinished());

    // save a new detail definition
    int originalCount = cm->detailDefinitions().keys().size();
    QContactDetailDefinition testDef;
    testDef.setName("TestDefinitionId");
    QMap<QString, QContactDetailFieldDefinition> fields;
    QContactDetailFieldDefinition f;
    f.setDataType(QVariant::String);
    fields.insert("TestDefinitionField", f);
    testDef.setFields(fields);
    QList<QContactDetailDefinition> saveList;
    saveList << testDef;
    dsr.setManager(cm.data());
    QCOMPARE(dsr.manager(), cm.data());
    QVERIFY(!dsr.isActive());
    QVERIFY(!dsr.isFinished());
    QVERIFY(!dsr.cancel());
    QVERIFY(!dsr.waitForFinished());
    qRegisterMetaType<QContactDetailDefinitionSaveRequest*>("QContactDetailDefinitionSaveRequest*");
    QThreadSignalSpy spy(&dsr, SIGNAL(stateChanged(QContactAbstractRequest::State)));
    dsr.setDefinition(testDef);
    QCOMPARE(dsr.definitions(), saveList);
    QVERIFY(!dsr.cancel()); // not started
    QVERIFY(dsr.start());

    QVERIFY((dsr.isActive() && dsr.state() == QContactAbstractRequest::ActiveState) || dsr.isFinished());
    //QVERIFY(dsr.isFinished() || !dsr.start());  // already started. // thread scheduling means this is untestable
    QVERIFY(dsr.waitForFinished());
    QVERIFY(dsr.isFinished());
    QVERIFY(spy.count() >= 1); // active + finished progress signals
    spy.clear();

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

    QVERIFY((dsr.isActive() && dsr.state() == QContactAbstractRequest::ActiveState) || dsr.isFinished());
    //QVERIFY(dsr.isFinished() || !dsr.start());  // already started. // thread scheduling means this is untestable
    QVERIFY(dsr.waitForFinished());
    QVERIFY(dsr.isFinished());
    QVERIFY(spy.count() >= 1); // active + finished progress signals
    spy.clear();

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

    int bailoutCount = MAX_OPTIMISTIC_SCHEDULING_LIMIT; // attempt to cancel 40 times.  If it doesn't work due to threading, bail out.
    while (true) {
        QVERIFY(!dsr.cancel()); // not started
        FILL_QUEUE_WITH_FETCH_REQUESTS();
        QVERIFY(dsr.start());
        if (!dsr.cancel()) {
            // due to thread scheduling, async cancel might be attempted
            // after the request has already finished.. so loop and try again.
            dsr.waitForFinished();
            saveList.clear();
            saveList << testDef;
            dsr.setDefinitions(saveList);
            cm->removeDetailDefinition(testDef.name());
            bailoutCount -= 1;
            if (!bailoutCount) {
//                qWarning("Unable to test cancelling due to thread scheduling!");
                bailoutCount = MAX_OPTIMISTIC_SCHEDULING_LIMIT;
                break;
            }
            spy.clear();
            continue;
        }

        // if we get here, then we are cancelling the request.
        QVERIFY(dsr.waitForFinished());
        QVERIFY(dsr.isCanceled());
        QVERIFY(spy.count() >= 1); // active + cancelled progress signals
        spy.clear();

        // verify that the changes were not saved
        QList<QContactDetailDefinition> allDefs = cm->detailDefinitions().values();
        QVERIFY(!allDefs.contains(testDef));
        QCOMPARE(cm->detailDefinitions().values().size(), originalCount + 1);

        break;
    }

    // restart, and wait for progress after cancel.
    while (true) {
        QVERIFY(!dsr.cancel()); // not started
        FILL_QUEUE_WITH_FETCH_REQUESTS();
        QVERIFY(dsr.start());
        if (!dsr.cancel()) {
            // due to thread scheduling, async cancel might be attempted
            // after the request has already finished.. so loop and try again.
            dsr.waitForFinished();
            saveList.clear();
            saveList << testDef;
            dsr.setDefinitions(saveList);
            cm->removeDetailDefinition(testDef.name());
            bailoutCount -= 1;
            if (!bailoutCount) {
//                qWarning("Unable to test cancelling due to thread scheduling!");
                bailoutCount = MAX_OPTIMISTIC_SCHEDULING_LIMIT;
                break;
            }
            spy.clear();
            continue;
        }
        dsr.waitForFinished();
        QVERIFY(dsr.isCanceled());
        QVERIFY(spy.count() >= 1); // active + cancelled progress signals
        spy.clear();

        // verify that the changes were not saved
        QList<QContactDetailDefinition> allDefs = cm->detailDefinitions().values();
        QVERIFY(!allDefs.contains(testDef));
        QCOMPARE(cm->detailDefinitions().values().size(), originalCount + 1);

        break;
    }

}

void tst_QContactAsync::relationshipFetch()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(prepareModel(uri));

    if (!cm->hasFeature(QContactManager::Relationships)) {
       QSKIP("This contact manager does not support relationships!", SkipSingle);
    }
    
    if (cm->managerName() == "symbian") {
        QSKIP("This contact manager does not support the required relationship types for this test to pass!", SkipSingle);
    }
    
    QContactRelationshipFetchRequest rfr;
    QVERIFY(rfr.type() == QContactAbstractRequest::RelationshipFetchRequest);

    // initial state - not started, no manager.
    QVERIFY(!rfr.isActive());
    QVERIFY(!rfr.isFinished());
    QVERIFY(!rfr.start());
    QVERIFY(!rfr.cancel());
    QVERIFY(!rfr.waitForFinished());

    // "all relationships" retrieval
    rfr.setManager(cm.data());
    QCOMPARE(rfr.manager(), cm.data());
    QVERIFY(!rfr.isActive());
    QVERIFY(!rfr.isFinished());
    QVERIFY(!rfr.cancel());
    QVERIFY(!rfr.waitForFinished());
    qRegisterMetaType<QContactRelationshipFetchRequest*>("QContactRelationshipFetchRequest*");
    QThreadSignalSpy spy(&rfr, SIGNAL(stateChanged(QContactAbstractRequest::State)));
    QVERIFY(!rfr.cancel()); // not started
    QVERIFY(rfr.start());

    QVERIFY((rfr.isActive() && rfr.state() == QContactAbstractRequest::ActiveState) || rfr.isFinished());
    //QVERIFY(rfr.isFinished() || !rfr.start());  // already started. // thread scheduling means this is untestable
    QVERIFY(rfr.waitForFinished());

    QVERIFY(rfr.isFinished());
    QVERIFY(spy.count() >= 1); // active + finished progress signals
    spy.clear();

    QList<QContactRelationship> rels = cm->relationships();
    QList<QContactRelationship> result = rfr.relationships();
    QCOMPARE(rels, result);

    // specific relationship type retrieval
    rfr.setRelationshipType(QContactRelationship::HasManager);
    QVERIFY(!rfr.cancel()); // not started
    QVERIFY(rfr.start());

    QVERIFY((rfr.isActive() && rfr.state() == QContactAbstractRequest::ActiveState) || rfr.isFinished());
    //QVERIFY(rfr.isFinished() || !rfr.start());  // already started. // thread scheduling means this is untestable
    QVERIFY(rfr.waitForFinished());
    QVERIFY(rfr.isFinished());
    QVERIFY(spy.count() >= 1); // active + finished progress signals
    spy.clear();

    rels = cm->relationships(QContactRelationship::HasManager);
    result = rfr.relationships();
    QCOMPARE(rels, result);

    // specific source contact retrieval
    rfr.setRelationshipType(QString());
    QList<QContactLocalId> contacts = cm->contactIds();
    QContactId aId;
    foreach (const QContactLocalId& currId, contacts) {
        QContact curr = cm->contact(currId);
        if (curr.detail(QContactName::DefinitionName).value(QContactName::FieldFirstName) == QString("Aaron")) {
            aId = curr.id();
            break;
        }
    }
    rfr.setFirst(aId);
    QVERIFY(!rfr.cancel()); // not started
    QVERIFY(rfr.start());

    QVERIFY((rfr.isActive() && rfr.state() == QContactAbstractRequest::ActiveState) || rfr.isFinished());
    //QVERIFY(rfr.isFinished() || !rfr.start());  // already started. // thread scheduling means this is untestable
    QVERIFY(rfr.waitForFinished());
    QVERIFY(rfr.isFinished());
    QVERIFY(spy.count() >= 1); // active + finished progress signals
    spy.clear();

    rels = cm->relationships(aId, QContactRelationship::First);
    result = rfr.relationships();
    QCOMPARE(rels, result);

    // specific participant retrieval #1 - destination participant
    rfr.setFirst(QContactId());
    contacts = cm->contactIds();
    QContactId bId;
    foreach (const QContactLocalId& currId, contacts) {
        QContact curr = cm->contact(currId);
        if (curr.detail(QContactName::DefinitionName).value(QContactName::FieldFirstName) == QString("Bob")) {
            bId = curr.id();
            break;
        }
    }
    rfr.setSecond(bId);
    
    QVERIFY(!rfr.cancel()); // not started
    QVERIFY(rfr.start());

    QVERIFY((rfr.isActive() && rfr.state() == QContactAbstractRequest::ActiveState) || rfr.isFinished());
    //QVERIFY(rfr.isFinished() || !rfr.start());  // already started. // thread scheduling means this is untestable
    QVERIFY(rfr.waitForFinished());
    QVERIFY(rfr.isFinished());
    QVERIFY(spy.count() >= 1); // active + finished progress signals
    spy.clear();

    // retrieve rels where second = id of B, and ensure that we get the same results
    rels = cm->relationships(bId, QContactRelationship::Second);
    result = rfr.relationships();
    QCOMPARE(rels, result);

    // specific participant retrieval #2 - source participant
    rfr.setFirst(QContactId());
    contacts = cm->contactIds();
    QContactId cId;
    foreach (const QContactLocalId& currId, contacts) {
        QContact curr = cm->contact(currId);
        if (curr.detail(QContactName::DefinitionName).value(QContactName::FieldFirstName) == QString("Borris")) {
            cId = curr.id();
            break;
        }
    }
    rfr.setSecond(cId);
    
    QVERIFY(!rfr.cancel()); // not started
    QVERIFY(rfr.start());

    QVERIFY((rfr.isActive() && rfr.state() == QContactAbstractRequest::ActiveState) || rfr.isFinished());
    //QVERIFY(rfr.isFinished() || !rfr.start());  // already started. // thread scheduling means this is untestable
    QVERIFY(rfr.waitForFinished());
    QVERIFY(rfr.isFinished());
    QVERIFY(spy.count() >= 1); // active + finished progress signals
    spy.clear();

    // retrieve rels where first = id of C and compare the results
    rfr.setFirst(cId);
    rfr.setSecond(QContactId());
    QVERIFY(rfr.start());
    QVERIFY(rfr.waitForFinished());
    result = rfr.relationships();
    rels = cm->relationships(cId, QContactRelationship::First);
    QCOMPARE(rels, result);

    // cancelling
    rfr.setRelationshipType(QString());

    int bailoutCount = MAX_OPTIMISTIC_SCHEDULING_LIMIT; // attempt to cancel 40 times.  If it doesn't work due to threading, bail out.
    while (true) {
        QVERIFY(!rfr.cancel()); // not started
        FILL_QUEUE_WITH_FETCH_REQUESTS();
        QVERIFY(rfr.start());
        if (!rfr.cancel()) {
            // due to thread scheduling, async cancel might be attempted
            // after the request has already finished.. so loop and try again.
            rfr.waitForFinished();
            rfr.setRelationshipType(QString());
            bailoutCount -= 1;
            if (!bailoutCount) {
//                qWarning("Unable to test cancelling due to thread scheduling!");
                bailoutCount = MAX_OPTIMISTIC_SCHEDULING_LIMIT;
                break;
            }
            spy.clear();
            continue;
        }

        // if we get here, then we are cancelling the request.
        QVERIFY(rfr.waitForFinished());
        QVERIFY(rfr.isCanceled());
        QVERIFY(spy.count() >= 1); // active + cancelled progress signals
        spy.clear();
        break;
    }

    // restart, and wait for progress after cancel.
    while (true) {
        QVERIFY(!rfr.cancel()); // not started
        FILL_QUEUE_WITH_FETCH_REQUESTS();
        QVERIFY(rfr.start());
        if (!rfr.cancel()) {
            // due to thread scheduling, async cancel might be attempted
            // after the request has already finished.. so loop and try again.
            rfr.waitForFinished();
            rfr.setRelationshipType(QString());
            bailoutCount -= 1;
            if (!bailoutCount) {
//                qWarning("Unable to test cancelling due to thread scheduling!");
                bailoutCount = MAX_OPTIMISTIC_SCHEDULING_LIMIT;
                break;
            }
            spy.clear();
            continue;
        }
        rfr.waitForFinished();
        QVERIFY(rfr.isCanceled());
        QVERIFY(spy.count() >= 1); // active + cancelled progress signals
        spy.clear();
        break;
    }
}

void tst_QContactAsync::relationshipRemove()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(prepareModel(uri));
    
    if (!cm->hasFeature(QContactManager::Relationships)) {
       QSKIP("This contact manager does not support relationships!", SkipSingle);
    }
    
    if (cm->managerName() == "symbian") {
        QSKIP("This contact manager does not support the required relationship types for this test to pass!", SkipSingle);
    }
    
    QContactRelationshipRemoveRequest rrr;
    QVERIFY(rrr.type() == QContactAbstractRequest::RelationshipRemoveRequest);

    // initial state - not started, no manager.
    QVERIFY(!rrr.isActive());
    QVERIFY(!rrr.isFinished());
    QVERIFY(!rrr.start());
    QVERIFY(!rrr.cancel());
    QVERIFY(!rrr.waitForFinished());

    QList<QContactLocalId> contacts = cm->contactIds();
    QContactId aId, bId, cId;
    foreach (const QContactLocalId& currId, contacts) {
        QContact curr = cm->contact(currId);
        if (curr.detail(QContactName::DefinitionName).value(QContactName::FieldFirstName) == QString("Aaron")) {
            aId = curr.id();
            continue;
        }
        if (curr.detail(QContactName::DefinitionName).value(QContactName::FieldFirstName) == QString("Bob")) {
            bId = curr.id();
            continue;
        }
        if (curr.detail(QContactName::DefinitionName).value(QContactName::FieldFirstName) == QString("Borris")) {
            cId = curr.id();
            continue;
        }
    }

    // specific source, destination and type removal
    QList<QContactRelationship> relationships;
    QContactRelationship r;
    r.setFirst(aId);
    r.setSecond(cId);
    r.setRelationshipType(QContactRelationship::HasAssistant);
    relationships.push_back(r);

    rrr.setRelationships(relationships);
    rrr.setManager(cm.data());
    qRegisterMetaType<QContactRelationshipRemoveRequest*>("QContactRelationshipRemoveRequest*");
    QThreadSignalSpy spy(&rrr, SIGNAL(stateChanged(QContactAbstractRequest::State)));
    QCOMPARE(rrr.manager(), cm.data());
    QVERIFY(!rrr.isActive());
    QVERIFY(!rrr.isFinished());
    QVERIFY(!rrr.cancel());
    QVERIFY(!rrr.waitForFinished());
    
    QVERIFY(!rrr.cancel()); // not started
    QVERIFY(rrr.start());

    QVERIFY((rrr.isActive() && rrr.state() == QContactAbstractRequest::ActiveState) || rrr.isFinished());
    //QVERIFY(rrr.isFinished() || !rrr.start());  // already started. // thread scheduling means this is untestable
    QVERIFY(rrr.waitForFinished());
    QVERIFY(rrr.isFinished());
    QVERIFY(spy.count() >= 1); // active + finished progress signals
    spy.clear();
    QCOMPARE(cm->relationships(QContactRelationship::HasAssistant, cId, QContactRelationship::Second).size(), 1);

    // remove (asynchronously) a nonexistent relationship - should fail.
    r.setFirst(cId);
    r.setSecond(aId);
    r.setRelationshipType(QContactRelationship::HasManager);
    relationships.clear();
    relationships.push_back(r);
    rrr.setRelationship(r);
    QVERIFY(rrr.relationships() == relationships);
    rrr.setManager(cm.data());
    QVERIFY(!rrr.cancel()); // not started
    QVERIFY(rrr.start());

    QVERIFY((rrr.isActive() && rrr.state() == QContactAbstractRequest::ActiveState) || rrr.isFinished());
    //QVERIFY(rrr.isFinished() || !rrr.start());  // already started. // thread scheduling means this is untestable
    QVERIFY(rrr.waitForFinished());
    QVERIFY(rrr.isFinished());
    QVERIFY(spy.count() >= 1); // active + finished progress signals
    spy.clear();

    QCOMPARE(cm->relationships(QContactRelationship::HasManager, cId, QContactRelationship::First).size(), 0);
//    QCOMPARE(rrr.error(), QContactManager::DoesNotExistError);

    // cancelling
    r.setFirst(cId);
    r.setSecond(QContactId());
    relationships.clear();
    relationships.push_back(r);
    

    int bailoutCount = MAX_OPTIMISTIC_SCHEDULING_LIMIT; // attempt to cancel 40 times.  If it doesn't work due to threading, bail out.
    while (true) {
        QVERIFY(!rrr.cancel()); // not started
        FILL_QUEUE_WITH_FETCH_REQUESTS();
        QVERIFY(rrr.start());
        if (!rrr.cancel()) {
            // due to thread scheduling, async cancel might be attempted
            // after the request has already finished.. so loop and try again.
            rrr.waitForFinished();
            rrr.setRelationships(relationships);
            bailoutCount -= 1;
            if (!bailoutCount) {
//                qWarning("Unable to test cancelling due to thread scheduling!");
                bailoutCount = MAX_OPTIMISTIC_SCHEDULING_LIMIT;
                break;
            }
            spy.clear();
            continue;
        }

        // if we get here, then we are cancelling the request.
        QVERIFY(rrr.waitForFinished());
        QVERIFY(rrr.isCanceled());
        QVERIFY(spy.count() >= 1); // active + cancelled progress signals
        spy.clear();

        QVERIFY(cm->relationships(cId).size() != 0); // didn't remove them.
        break;
    }

    // restart, and wait for progress after cancel.
    while (true) {
        QVERIFY(!rrr.cancel()); // not started
        FILL_QUEUE_WITH_FETCH_REQUESTS();
        QVERIFY(rrr.start());
        if (!rrr.cancel()) {
            // due to thread scheduling, async cancel might be attempted
            // after the request has already finished.. so loop and try again.
            rrr.waitForFinished();
            rrr.setRelationships(relationships);
            bailoutCount -= 1;
            if (!bailoutCount) {
//                qWarning("Unable to test cancelling due to thread scheduling!");
                bailoutCount = MAX_OPTIMISTIC_SCHEDULING_LIMIT;
                break;
            }
            spy.clear();
            continue;
        }
        rrr.waitForFinished();
        QVERIFY(rrr.isCanceled());
        QVERIFY(spy.count() >= 1); // active + cancelled progress signals
        spy.clear();

        QVERIFY(cm->relationships(cId).size() != 0); // didn't remove them.
        break;
    }
}

void tst_QContactAsync::relationshipSave()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(prepareModel(uri));
    
    if (!cm->hasFeature(QContactManager::Relationships)) {
       QSKIP("This contact manager does not support relationships!", SkipSingle);
    }
    
    if (cm->managerName() == "symbian") {
        QSKIP("This contact manager does not support the required relationship types for this test to pass!", SkipSingle);
    }    
    
    QContactRelationshipSaveRequest rsr;
    QVERIFY(rsr.type() == QContactAbstractRequest::RelationshipSaveRequest);

    // initial state - not started, no manager.
    QVERIFY(!rsr.isActive());
    QVERIFY(!rsr.isFinished());
    QVERIFY(!rsr.start());
    QVERIFY(!rsr.cancel());
    QVERIFY(!rsr.waitForFinished());

    QList<QContactLocalId> contacts = cm->contactIds();
    QContactId cId, aId, bId;
    foreach (const QContactLocalId& currId, contacts) {
        QContact curr = cm->contact(currId);
        if (curr.detail(QContactName::DefinitionName).value(QContactName::FieldFirstName) == QString("Borris")) {
            cId = curr.id();
        } else if (curr.detail(QContactName::DefinitionName).value(QContactName::FieldFirstName) == QString("Bob")) {
            bId = curr.id();
        } else if (curr.detail(QContactName::DefinitionName).value(QContactName::FieldFirstName) == QString("Aaron")) {
            aId = curr.id();
        }
    }

    // save a new relationship
    int originalCount = cm->relationships(aId).size();
    QContactRelationship testRel;
    testRel.setFirst(aId);
    testRel.setRelationshipType(QContactRelationship::HasSpouse);
    testRel.setSecond(bId);
    QList<QContactRelationship> saveList;
    saveList << testRel;
    rsr.setManager(cm.data());
    QCOMPARE(rsr.manager(), cm.data());
    QVERIFY(!rsr.isActive());
    QVERIFY(!rsr.isFinished());
    QVERIFY(!rsr.cancel());
    QVERIFY(!rsr.waitForFinished());
    qRegisterMetaType<QContactRelationshipSaveRequest*>("QContactRelationshipSaveRequest*");
    QThreadSignalSpy spy(&rsr, SIGNAL(stateChanged(QContactAbstractRequest::State)));
    rsr.setRelationship(testRel);
    QCOMPARE(rsr.relationships(), saveList);
    QVERIFY(!rsr.cancel()); // not started
    QVERIFY(rsr.start());

    QVERIFY((rsr.isActive() && rsr.state() == QContactAbstractRequest::ActiveState) || rsr.isFinished());
    //QVERIFY(rsr.isFinished() || !rsr.start());  // already started. // thread scheduling means this is untestable
    QVERIFY(rsr.waitForFinished());
    QVERIFY(rsr.isFinished());
    QVERIFY(spy.count() >= 1); // active + finished progress signals
    spy.clear();

    QList<QContactRelationship> expected = cm->relationships(QContactRelationship::HasSpouse, aId, QContactRelationship::First);
    QList<QContactRelationship> result = rsr.relationships();
    QCOMPARE(expected, result);
    QVERIFY(result.contains(testRel));
    QCOMPARE(cm->relationships(aId).size(), originalCount + 1); // should be one extra

    // save a new relationship
    testRel.setSecond(cId);
    saveList.clear();
    saveList << testRel;
    rsr.setRelationships(saveList);
    QCOMPARE(rsr.relationships(), saveList);
    QVERIFY(!rsr.cancel()); // not started
    QVERIFY(rsr.start());

    QVERIFY((rsr.isActive() && rsr.state() == QContactAbstractRequest::ActiveState) || rsr.isFinished());
    //QVERIFY(rsr.isFinished() || !rsr.start());  // already started. // thread scheduling means this is untestable
    QVERIFY(rsr.waitForFinished());
    QVERIFY(rsr.isFinished());
    QVERIFY(spy.count() >= 1); // active + finished progress signals
    spy.clear();

    expected.clear();
    expected = cm->relationships(QContactRelationship::HasSpouse, aId, QContactRelationship::First);
    result = rsr.relationships();
    QCOMPARE(result, QList<QContactRelationship>() << testRel);
    QVERIFY(expected.contains(testRel));
    QCOMPARE(cm->relationships(aId).size(), originalCount + 2); // should now be two extra

    // cancelling
    testRel.setSecond(aId); // shouldn't get saved (circular anyway)
    saveList.clear();
    saveList << testRel;
    rsr.setRelationships(saveList);
    int bailoutCount = MAX_OPTIMISTIC_SCHEDULING_LIMIT; // attempt to cancel 40 times.  If it doesn't work due to threading, bail out.
    while (true) {
        QVERIFY(!rsr.cancel()); // not started
        FILL_QUEUE_WITH_FETCH_REQUESTS();
        QVERIFY(rsr.start());
        if (!rsr.cancel()) {
            // due to thread scheduling, async cancel might be attempted
            // after the request has already finished.. so loop and try again.
            rsr.waitForFinished();
            saveList.clear();
            saveList << testRel;
            rsr.setRelationships(saveList);
            cm->removeRelationship(testRel); // probably shouldn't have been saved anyway (circular)
            bailoutCount -= 1;
            if (!bailoutCount) {
//                qWarning("Unable to test cancelling due to thread scheduling!");
                bailoutCount = MAX_OPTIMISTIC_SCHEDULING_LIMIT;
                break;
            }
            spy.clear();
            continue;
        }

        // if we get here, then we are cancelling the request.
        QVERIFY(rsr.waitForFinished());
        QVERIFY(rsr.isCanceled());
        QVERIFY(spy.count() >= 1); // active + cancelled progress signals
        spy.clear();

        // verify that the changes were not saved
        QList<QContactRelationship> aRels = cm->relationships(aId, QContactRelationship::First);
        QVERIFY(!aRels.contains(testRel));
        QCOMPARE(cm->relationships(aId).size(), originalCount + 2); // should still only be two extra

        break;
    }

    // restart, and wait for progress after cancel.
    while (true) {
        QVERIFY(!rsr.cancel()); // not started
        FILL_QUEUE_WITH_FETCH_REQUESTS();
        QVERIFY(rsr.start());
        if (!rsr.cancel()) {
            // due to thread scheduling, async cancel might be attempted
            // after the request has already finished.. so loop and try again.
            rsr.waitForFinished();
            saveList.clear();
            saveList << testRel;
            rsr.setRelationships(saveList);
            cm->removeRelationship(testRel); // probably shouldn't have been saved anyway (circular)
            bailoutCount -= 1;
            if (!bailoutCount) {
//                qWarning("Unable to test cancelling due to thread scheduling!");
                bailoutCount = MAX_OPTIMISTIC_SCHEDULING_LIMIT;
                break;
            }
            spy.clear();
            continue;
        }
        rsr.waitForFinished();
        QVERIFY(rsr.isCanceled());
        QVERIFY(spy.count() >= 1); // active + cancelled progress signals
        spy.clear();

        // verify that the changes were not saved
        QList<QContactRelationship> aRels = cm->relationships(aId, QContactRelationship::First);
        QVERIFY(!aRels.contains(testRel));
        QCOMPARE(cm->relationships(aId).size(), originalCount + 2); // should still only be two extra

        break;
    }
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

    // ensure that the base class implementation of requestDestroyed() is called
    QContactFetchRequest *destroyedRequest = new QContactFetchRequest;
    destroyedRequest->setManager(&cm);
    destroyedRequest->setFilter(fil);
    QVERIFY(!destroyedRequest->start());
    delete destroyedRequest;

    // now use a malicious manager that deliberately calls
    // things in a different thread
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

    QContactLocalIdFetchRequest cifr;
    cifr.setFilter(fil);
    cifr.setManager(&mcm);
    QVERIFY(cifr.start());

    QContactRemoveRequest crr;
    crr.setContactIds(mcm.contactIds(fil));
    crr.setManager(&mcm);
    QVERIFY(crr.start());

    QContactSaveRequest csr;
    csr.setContacts(emptyCList);
    csr.setManager(&mcm);
    QVERIFY(csr.start());

    {
    QContactDetailDefinitionFetchRequest dfr;
    dfr.setDefinitionNames(emptyDNList);
    dfr.setManager(&mcm);
    QVERIFY(dfr.start());
    }

    {
    QContactDetailDefinitionFetchRequest dfr;
    dfr.setDefinitionNames(emptyDNList);
    dfr.setManager(&mcm);
    }

    QContactDetailDefinitionSaveRequest dsr;
    dsr.setDefinitions(emptyDList);
    dsr.setManager(&mcm);
    QVERIFY(dsr.start());

    QContactDetailDefinitionRemoveRequest drr;
    drr.setDefinitionNames(emptyDNList);
    drr.setManager(&mcm);
    QVERIFY(drr.start());
}

void tst_QContactAsync::testQuickDestruction()
{
    QFETCH(QString, uri);

    // in this test, we create a manager, fire off a request, and delete the manager, all in quick succession
    // this is to test for segfaults etc.
    for (int i = 0; i < 10; i++) {
        QContactFetchRequest cfr;
        QContactManager *cm = prepareModel(uri);
        cfr.setManager(cm);
        cfr.start();
        delete cm;
    }
    // in this test, we create a manager, fire off a request, delete the request, then delete the manager, all in quick succession
    // this is to test for segfaults, etc.
    for (int i = 0; i < 10; i++) {
        QContactFetchRequest *cfr = new QContactFetchRequest;
        QContactManager *cm = prepareModel(uri);
        cfr->setManager(cm);
        cfr->start();
        delete cfr;
        delete cm;
    }
    // in this test, we create a manager, fire off a request, delete the manager, then delete the request, all in quick succession
    // this is to test for segfaults, etc.
    for (int i = 0; i < 10; i++) {
        QContactFetchRequest *cfr = new QContactFetchRequest;
        QContactManager *cm = prepareModel(uri);
        cfr->setManager(cm);
        cfr->start();
        delete cm;
        delete cfr;
    }
    // in this test, we create a manager, fire off a request, and delete the request, all in quick succession
    // this is to test for segfaults, etc.
    QContactManager *cm = prepareModel(uri);
    for (int i = 0; i < 10; i++) {
        QContactFetchRequest *cfr = new QContactFetchRequest;
        cfr->setManager(cm);
        cfr->start();
        delete cfr;
    }
    delete cm;
}

void tst_QContactAsync::threadDelivery()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(prepareModel(uri));
    m_mainThreadId = cm->thread()->currentThreadId();
    m_resultsAvailableSlotThreadId = m_mainThreadId;

    // now perform a fetch request and check that the progress is delivered to the correct thread.
    QContactFetchRequest *req = new QContactFetchRequest;
    req->setManager(cm.data());
    connect(req, SIGNAL(resultsAvailable()), this, SLOT(resultsAvailableReceived()));
    req->start();

    int totalWaitTime = 0;
    QTest::qWait(1); // force it to process events at least once.
    while (req->state() != QContactAbstractRequest::FinishedState) {
        // ensure that the progress signal was delivered to the main thread.
        QCOMPARE(m_mainThreadId, m_resultsAvailableSlotThreadId);

        QTest::qWait(5); // spin until done
        totalWaitTime += 5;

        // break after 30 seconds.
        if (totalWaitTime > 30000) {
            delete req;
            QSKIP("Asynchronous request not complete after 30 seconds!", SkipSingle);
        }
    }

    // ensure that the progress signal was delivered to the main thread.
    QCOMPARE(m_mainThreadId, m_resultsAvailableSlotThreadId);
    delete req;
}

void tst_QContactAsync::resultsAvailableReceived()
{
    QContactFetchRequest *req = qobject_cast<QContactFetchRequest *>(QObject::sender());
    if (req)
        m_resultsAvailableSlotThreadId = req->thread()->currentThreadId();
    else
        qWarning() << "resultsAvailableReceived() : request deleted; unable to set thread id!";
}

void tst_QContactAsync::addManagers(QStringList stringlist)
{
    QTest::addColumn<QString>("uri");

    // retrieve the list of available managers
    QStringList managers = QContactManager::availableManagers();

    // remove ones that we know will not pass
    if (!stringlist.contains("invalid"))
        managers.removeAll("invalid");
    if (!stringlist.contains("maliciousplugin"))
        managers.removeAll("maliciousplugin");
    if (!stringlist.contains("testdummy"))
        managers.removeAll("testdummy");
    if (!stringlist.contains("symbiansim"))
        managers.removeAll("symbiansim"); // SIM backend does not support all the required details for tests to pass.
    if (!stringlist.contains("social"))
        managers.removeAll("social");
    if (!stringlist.contains("simcard"))
        managers.removeAll("simcard");
    if (!stringlist.contains("com.nokia.messaging.contacts.engines.mail.contactslookup"))
        managers.removeAll("com.nokia.messaging.contacts.engines.mail.contactslookup");

    foreach(QString mgr, managers) {
        QMap<QString, QString> params;
        QTest::newRow(QString("mgr='%1'").arg(mgr).toLatin1().constData()) << QContactManager::buildUri(mgr, params);
        if (mgr == "memory") {
            params.insert("id", "tst_QContactManager");
            QTest::newRow(QString("mgr='%1', params").arg(mgr).toLatin1().constData()) << QContactManager::buildUri(mgr, params);
        }
    }
}

QContactManager* tst_QContactAsync::prepareModel(const QString& managerUri)
{
    QContactManager* cm = QContactManager::fromUri(managerUri);

    // XXX TODO: ensure that this is the case:
    // there should be no contacts in the database.
    QList<QContactLocalId> toRemove = cm->contactIds();
    foreach (const QContactLocalId& removeId, toRemove)
        cm->removeContact(removeId);

    QContact a, b, c;
    QContactName aNameDetail;
    aNameDetail.setFirstName("Aaron");
    aNameDetail.setLastName("Aaronson");
    a.saveDetail(&aNameDetail);
    QContactName bNameDetail;
    bNameDetail.setFirstName("Bob");
    bNameDetail.setLastName("Aaronsen");
    b.saveDetail(&bNameDetail);
    QContactName cNameDetail;
    cNameDetail.setFirstName("Borris");
    cNameDetail.setLastName("Aaronsun");
    c.saveDetail(&cNameDetail);

    QContactPhoneNumber phn;
    phn.setNumber("0123");
    c.saveDetail(&phn);
    phn.setNumber("3456");
    b.saveDetail(&phn);
    phn.setNumber("6789");
    a.saveDetail(&phn);

    QContactUrl url;
    url.setUrl("http://test.nokia.com");
    a.saveDetail(&url);

    cm->saveContact(&a);
    cm->saveContact(&b);
    cm->saveContact(&c);
    
    if (!cm->hasFeature(QContactManager::Relationships)) {
        return cm;
    }
    
    if (cm->managerName() == "symbian") {
        // Symbian backend does not support other relationships than HasMember (which is same as groups)
        return cm;
    }

    QContactRelationship arb;
    arb.setFirst(a.id());
    arb.setSecond(b.id());
    arb.setRelationshipType(QContactRelationship::HasManager);
    cm->saveRelationship(&arb);

    QContactRelationship brc;
    brc.setFirst(b.id());
    brc.setSecond(c.id());
    brc.setRelationshipType(QContactRelationship::HasAssistant);
    cm->saveRelationship(&brc);

    QContactRelationship cra;
    cra.setFirst(c.id());
    cra.setSecond(a.id());
    cra.setRelationshipType(QContactRelationship::HasSpouse);
    cm->saveRelationship(&cra);

    QContactRelationship arc;
    arc.setFirst(a.id());
    arc.setSecond(c.id());
    arc.setRelationshipType(QContactRelationship::HasAssistant);
    cm->saveRelationship(&arc);

    QContactRelationship crb;
    crb.setFirst(c.id());
    crb.setSecond(b.id());
    crb.setRelationshipType(QContactRelationship::IsSameAs);
    cm->saveRelationship(&crb);

    return cm;

    // TODO: cleanup once test is complete
}

QTEST_MAIN(tst_QContactAsync)
#include "tst_qcontactasync.moc"
