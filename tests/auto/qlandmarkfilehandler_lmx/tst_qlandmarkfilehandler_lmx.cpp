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

//TESTED_COMPONENT=src/location

#include "qgeoaddress.h"
#include "qgeocoordinate.h"
#include "qlandmarkcategory.h"

#include <qtest.h>

#include <QBuffer>
#include <QDebug>
#include <QFile>
#include <QMetaType>
#include <QThread>

#define private public
#include <qlandmarkmanager.h>
#include "qlandmarkmanager_p.h"
#include "qlandmarkfilehandler_lmx_p.h"
#include "qlandmarkmanagerdataholder.h"

QTM_USE_NAMESPACE

class TestThread : public QThread
{
public:
    volatile bool m_cancel;
    TestThread(): m_cancel(false){}

        protected:
    void run() {

        msleep(50);
        m_cancel = true;

    }
};

Q_DECLARE_METATYPE(QList<QLandmark>);
Q_DECLARE_METATYPE(QList<QLandmarkCategory>);

class tst_QLandmarkFileHandler_Lmx : public QObject
{
    Q_OBJECT

private:
    QLandmarkManager *m_manager;
    QLandmarkFileHandlerLmx *m_handler;
    QString m_exportFile;
    QScopedPointer<QLandmarkManagerDataHolder> managerDataHolder;

private slots:

    void clean() {
        QList<QLandmarkId> lmIds = m_manager->landmarkIds();
        for(int i=0; i < lmIds.count(); ++i) {
            QVERIFY(m_manager->removeLandmark(lmIds.at(i)));
        }

        QList<QLandmarkCategoryId> catIds = m_manager->categoryIds();
        for (int i=0; i < catIds.count(); ++i) {
            if (!m_manager->isReadOnly(catIds.at(i)))
                QVERIFY(m_manager->removeCategory(catIds.at(i)));
        }
        QTest::qWait(20);//try ensure notifications for these
                         //deletions are made prior to each test function.
    }

    void init() {
        m_exportFile = "exportfile";
        m_manager = new QLandmarkManager;
        m_handler = new QLandmarkFileHandlerLmx;
        clean();
    }

    void cleanup() {
        clean();
        delete m_handler;
        delete m_manager;

        if (QFile::exists(m_exportFile))
            QFile::remove(m_exportFile);
    }

    void initTestCase();
    void cleanupTestCase();

    void fileImport() {
        QFETCH(QString, fileIn);
        QFETCH(QString, fileOut);
        QFETCH(QString, exportPrefix);
        QFETCH(QList<QLandmark>, landmarks);
        QFETCH(QList<QLandmarkCategory>, categories);

        QHash<QString,QString> categoryNameIdHash; //name to local id
        for (int i = 0; i < categories.size(); ++i) {
            categoryNameIdHash.insert(categories[i].name(),categories[i].categoryId().localId());
        }

        QFile file(fileIn);
        file.open(QIODevice::ReadOnly);

         //set the category ids for the landmarks from the handler
        QVERIFY(m_handler->importData(&file));
        QList<QLandmark> handlerLandmarks = m_handler->landmarks();
        QList<QStringList> handlerLandmarkCategoryNames = m_handler->landmarkCategoryNames();
        for (int i=0; i < handlerLandmarks.count(); ++i) {
            for(int j=0; j < handlerLandmarkCategoryNames.at(i).count(); ++j) {
                QLandmarkCategoryId catId;
                catId.setManagerUri(m_manager->managerUri());
                catId.setLocalId(categoryNameIdHash.value(handlerLandmarkCategoryNames.at(i).at(j)));
                handlerLandmarks[i].addCategoryId(catId);
            }

        }

        file.close();
        QCOMPARE(handlerLandmarks, landmarks);
    }

    void fileImport_data() {
        commonData();
    }


    void dataExport() {
        QFETCH(QString, fileIn);
        QFETCH(QString, fileOut);
        QFETCH(QString, exportPrefix);
        QFETCH(QList<QLandmark>, landmarks);
        QFETCH(QList<QLandmarkCategory>, categories);

        QHash<QString,QString> categoryIdNameHash;//local id to name
        foreach(const QLandmarkCategory &category, categories) {
                categoryIdNameHash.insert(category.categoryId().localId(),category.name());
        }

        m_handler->setLandmarks(landmarks);
        m_handler->setCategoryIdNameHash(categoryIdNameHash);

        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        QVERIFY(m_handler->exportData(&buffer, exportPrefix));
        QByteArray dataExported = buffer.buffer();
        buffer.close();

        QFile file(fileOut);
        file.open(QIODevice::ReadOnly);

        QByteArray testData = file.readAll();
        file.close();

        QCOMPARE(dataExported, testData);
    }

    void dataExport_data() {
        commonData();
    }

    void cancelExport()
    {
        TestThread cancelThread;
        QLandmarkFileHandlerLmx handler(&(cancelThread.m_cancel));
        QLandmark lm;
        QList<QLandmark> lms;
        for (int i=0; i < 5000; ++i) {
            lm.setName(QString("LM%1").arg(i));
            lm.setUrl(QUrl("url"));
            lm.setCoordinate(QGeoCoordinate(54,23));
            QGeoAddress address;
            address.setStreet("main street");
            lm.setAddress(address);
            lms.append(lm);
        }

        handler.setLandmarks(lms);
        cancelThread.start();
        QFile file(m_exportFile);

        QVERIFY(!handler.exportData(&file));

        QCOMPARE(handler.errorCode(), QLandmarkManager::CancelError);
        cancelThread.wait();
    }

    void fileImportErrors() {
        QFETCH(QString, file);
        QFETCH(QString, error);

        QString filename = ":/data/errors/";
        filename += file;
        QFile fileIn(filename);
        fileIn.open(QIODevice::ReadOnly);

        bool result = m_handler->importData(&fileIn);
        QVERIFY(!result);
        QCOMPARE(m_handler->errorString(), error);
    }

    void fileImportErrors_data() {
        QTest::addColumn<QString>("file");
        QTest::addColumn<QString>("error");

        QTest::newRow("No root element")
        << "lmx/noroot.xml"
        << "Expected a root element named \"lmx\" (no root element found).";
        QTest::newRow("Wrong root element")
        << "lmx/wrongroot.xml"
        << "The root element is expected to have the name \"lmx\" (root element was named \"notlmx\").";
        QTest::newRow("Two root elements")
        << "lmx/tworoots.xml"
        << "A single root element named \"lmx\" was expected (second root element was named \"otherlmx\").";
        QTest::newRow("Wrong options to lmx choice")
        << "lmx/wrongchoice.xml"
        << "The element \"lmx\" expected a single child element named either \"landmark\" or \"landmarkCollection\" (child element was named \"notlandmark\").";
        QTest::newRow("Two children to lmx")
        << "lmx/twochildren.xml"
        <<  "The element \"lmx\" expected a single child element (second child element was named \"landmark\").";
        QTest::newRow("No landmarks in collection")
        << "collection/nolandmarks.xml"
        << "The element \"landmarkCollection\" expected 1 or more child elements named \"landmark\" (0 found).";
        QTest::newRow("Wrong order of collection elements")
        << "collection/wrongorder.xml"
        << "The element \"landmarkCollection\" did not expect a child element named \"description\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid first collection element")
        << "collection/invalidfirst.xml"
        << "The element \"landmarkCollection\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid middle collection element")
        << "collection/invalidmid.xml"
        << "The element \"landmarkCollection\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid last collection element")
        << "collection/invalidlast.xml"
        << "The element \"landmarkCollection\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Non-double radius element")
        << "landmark/nondoubleradius.xml"
        << "The element \"coverageRadius\" expected a value convertable to type real (value was \"twenty three\").";
        QTest::newRow("Negative radius element")
        << "landmark/negativeradius.xml"
        << "The element \"coverageRadius\" is expected to have a non-negative value (value was \"-23.0\").";
        QTest::newRow("Wrong order of landmark elements")
        << "landmark/wrongorder.xml"
        << "The element \"landmark\" did not expect a child element named \"name\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid first landmark element")
        << "landmark/invalidfirst.xml"
        << "The element \"landmark\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid middle landmark element")
        << "landmark/invalidmid.xml"
        << "The element \"landmark\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid last landmark element")
        << "landmark/invalidlast.xml"
        << "The element \"landmark\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("No latitude element")
        << "coord/nolat.xml"
        << "The element \"coordinates\" did not have the required child element \"latitude\".";
        QTest::newRow("No longitude element")
        << "coord/nolon.xml"
        << "The element \"coordinates\" did not have the required child element \"longitude\".";
        QTest::newRow("Non-double latitude element")
        << "coord/nondoublelat.xml"
        << "The element \"latitude\" expected a value convertable to type double (value was \"one\").";
        QTest::newRow("Non-double longitude element")
        << "coord/nondoublelon.xml"
        << "The element \"longitude\" expected a value convertable to type double (value was \"two\").";
        QTest::newRow("Latitude element out of range")
        << "coord/outofrangelat.xml"
        << "The element \"latitude\" fell outside of the bounds -90.0 <= latitude <= 90.0 (value was \"1000.0\").";
        QTest::newRow("Longitude element out of range")
        << "coord/outofrangelon.xml"
        << "The element \"longitude\" fell outside of the bounds -180.0 <= longitude < 180.0 (value was \"2000.0\").";
        QTest::newRow("Non-double altitude element")
        << "coord/nondoublealt.xml"
        << "The element \"altitude\" expected a value convertable to type float (value was \"three\").";
        QTest::newRow("Wrong order of coordinate elements")
        << "coord/wrongorder.xml"
        << "The element \"coordinate\" did not expect a child element named \"altitude\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid first coordinate element")
        << "coord/invalidfirst.xml"
        << "The element \"coordinate\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid middle coordinate element")
        << "coord/invalidmid.xml"
        << "The element \"coordinate\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid last coordinate element")
        << "coord/invalidlast.xml"
        << "The element \"coordinate\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Duplicate address elements")
        << "address/duplicate.xml"
        << "The element \"addressInfo\" did not expect more then one occurrence of the child element named \"state\".";
        QTest::newRow("Invalid first address element")
        << "address/invalidfirst.xml"
        << "The element \"addressInfo\" did not expect a child element named \"invalid\".";
        QTest::newRow("Invalid middle address element")
        << "address/invalidmid.xml"
        << "The element \"addressInfo\" did not expect a child element named \"invalid\".";
        QTest::newRow("Invalid last address element")
        << "address/invalidlast.xml"
        << "The element \"addressInfo\" did not expect a child element named \"invalid\".";
        QTest::newRow("No URL in link element")
        << "link/nourl.xml"
        << "The element \"mediaLink\" did not have the required child element \"url\".";
        QTest::newRow("Wrong order of link elements")
        << "link/wrongorder.xml"
        << "The element \"url\" did not expect a child element named \"name\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid first link element")
        << "link/invalidfirst.xml"
        << "The element \"url\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid middle link element")
        << "link/invalidmid.xml"
        << "The element \"url\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid last link element")
        << "link/invalidlast.xml"
        << "The element \"url\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("No name in category element")
        << "category/noname.xml"
        << "The element \"category\" did not have the required child element \"name\".";
        QTest::newRow("Non-unsigned int id element")
        << "category/nonintid.xml"
        << "The element \"id\" expected a value convertable to type unsigned short (value was \"two\").";
        QTest::newRow("Wrong order of category elements")
        << "category/wrongorder.xml"
        << "The element \"category\" did not expect a child element named \"id\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid first category element")
        << "category/invalidfirst.xml"
        << "The element \"category\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid middle category element")
        << "category/invalidmid.xml"
        << "The element \"category\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid last category element")
        << "category/invalidlast.xml"
        << "The element \"category\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
    }

    void absoluteURI() {
        QFETCH(QString, uri);
        QFETCH(bool, expectedResult);

        QCOMPARE(isURIAbsolute(uri), expectedResult);
    }

    void absoluteURI_data() {
        QTest::addColumn<QString>("uri");
        QTest::addColumn<bool>("expectedResult");

        QTest::newRow("Valid input: uri") << "http://www.ics.uci.edu/pub/ietf/uri/#Related" << true;
        QTest::newRow("Valid input: uri") << "http://www.test.com" << true;
        QTest::newRow("Valid input: ftp") << "ftp://www.test.com" << true;
        QTest::newRow("Valid input: empty") << "" << true;
        QTest::newRow("Invalid: scheme-specific part only") << "www.invalid.com" << false;
        QTest::newRow("Invalid input: scheme only") << "http:" << false;
        QTest::newRow("Invalid input: scheme part only w/ : ") << ":www.invalid.com" << false;
    }

/* TODO: exports
    void dataExportErrors() {
        QFETCH(QLandmarkCategoryId, catId);
        QFETCH(QString, error);

        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);

        QList<QLandmark> landmarks;
        QLandmark lm;
        lm.addCategoryId(catId);
        landmarks.append(lm);
        m_handler->setLandmarks(landmarks);

        QVERIFY(!m_handler->exportData(&buffer, ""));
        QCOMPARE(m_handler->errorString(), error);

        buffer.close();
    }

    void dataExportErrors_data() {
        QTest::addColumn<QLandmarkCategoryId>("catId");
        QTest::addColumn<QString>("error");

        QLandmarkCategoryId catId1;

        QLandmarkCategoryId catId2;
        catId2.setLocalId("1");
        catId2.setManagerUri("wrongUri");

        QMap<QString, QString> map;
        map["filename"] = "test.db";
        m_manager = new QLandmarkManager("com.nokia.qt.landmarks.engines.sqlite", map);

        QLandmarkCategoryId catId3;
        catId3.setLocalId("100");
        catId3.setManagerUri(m_manager->managerUri());

        delete m_manager;

        QTest::newRow("invalid id")
                << catId1
                << "The category with id \"\" from manager \"\" is invalid.";
        QTest::newRow("wrong manager uri")
                << catId2
                << "Category id comes from different landmark manager.";
        QTest::newRow("non existent id")
                << catId3
                << "None of the existing categories match the given category id.";
    }
*/

/* TODO: category imports
    void categoryImports() {

        QFile file1(":/data/category-id-unknown.xml");
        file1.open(QIODevice::ReadOnly);

        QVERIFY(!m_handler->importData(&file1));
        QCOMPARE(m_handler->errorString(), QString("None of the existing categories match the given category id."));

        file1.close();

        QFile file2(":/data/category-id-empty.xml");
        file2.open(QIODevice::ReadOnly);

        QVERIFY(m_handler->importData(&file2));

        QCOMPARE(m_handler->landmarks().size(), 1);

        QLandmark lm = m_handler->landmarks().at(0);

        QCOMPARE(lm.categoryIds().size(), 1);

        QLandmarkCategoryId catId = lm.categoryIds().at(0);

        QCOMPARE(catId.isValid(), true);

        QLandmarkManager::Error error;
        QLandmarkCategory cat = m_manager->category(catId);
        error = m_manager->error();

        QCOMPARE(error, QLandmarkManager::NoError);
        QCOMPARE(cat.name(), QString("cat0"));

        file2.close();
    }*/

private:
    void commonData() {
        QTest::addColumn<QString>("fileIn");
        QTest::addColumn<QString>("fileOut");
        QTest::addColumn<QString>("exportPrefix");
        QTest::addColumn<QList<QLandmark> >("landmarks");
        QTest::addColumn<QList<QLandmarkCategory> >("categories");

        QList<QLandmarkCategory> cats;
        QList<QLandmarkCategoryId> catIds;

        m_manager = new QLandmarkManager;

        QLandmarkCategory cat0;
        cat0.setName("cat0");
        m_manager->saveCategory(&cat0);

        QLandmarkCategory cat1;
        cat1.setName("cat1");
        m_manager->saveCategory(&cat1);

        QLandmarkCategory cat2;
        cat2.setName("cat2");
        m_manager->saveCategory(&cat2);

        delete m_manager;
        QFile file("test.db");
        file.remove();

        cats << cat0 << cat1 << cat2;
        catIds << cat0.categoryId() << cat1.categoryId() << cat2.categoryId();

        QList<QLandmark> w;

        QLandmark w0;

        w0.setName("w0");
        w0.setDescription("Test data");
        w0.setCoordinate(QGeoCoordinate(1.0, 2.0, 3.0));
        w0.setRadius(4.0);
        QGeoAddress a0;
        a0.setStreet("1 Main St");
        a0.setCity("Brisbane");
        a0.setState("Queensland");
        a0.setCountry("Australia");
        a0.setPostcode("4000");
        w0.setAddress(a0);
        w0.setPhoneNumber("123456789");
        w0.setUrl(QUrl("http://example.com/testUrl"));
        w0.setCategoryIds(catIds);
        w << w0;

        QTest::newRow("convert-single")
        << ":/data/convert-single-in.xml"
        << ":/data/convert-single-out.xml"
        << ""
        << w
        << cats;

        QTest::newRow("convert-single-prefixed")
        << ":/data/convert-single-prefixed-in.xml"
        << ":/data/convert-single-prefixed-out.xml"
        << "lm"
        << w
        << cats;

        QLandmark w1;
        w << w1;

        QLandmark w2;
        w2.setName("w0");
        w << w2;

        QLandmark w3;
        w3.setDescription("Test data");
        w << w3;

        QLandmark w4;
        w4.setCoordinate(QGeoCoordinate(1.0, 2.0));
        w << w4;

        QLandmark w5;
        w5.setCoordinate(QGeoCoordinate(1.0, 2.0, 3.0));
        w << w5;

        QLandmark w6;
        w6.setRadius(4.0);
        w << w6;

        QLandmark w7;
        QGeoAddress a7;
        a7.setStreet("1 Main St");
        w7.setAddress(a7);
        w << w7;

        QLandmark w8;
        QGeoAddress a8;
        a8.setCity("Brisbane");
        w8.setAddress(a8);
        w << w8;

        QLandmark w9;
        QGeoAddress a9;
        a9.setState("Queensland");
        w9.setAddress(a9);
        w << w9;

        QLandmark w10;
        QGeoAddress a10;
        a10.setCountry("Australia");
        w10.setAddress(a10);
        w << w10;

        QLandmark w11;
        QGeoAddress a11;
        a11.setPostcode("4000");
        w11.setAddress(a11);
        w << w11;

        QLandmark w12;
        w12.setPhoneNumber("123456789");
        w << w12;

        QLandmark w13;
        QGeoAddress a13;
        a13.setStreet("1 Main St");
        a13.setCity("Brisbane");
        a13.setState("Queensland");
        a13.setCountry("Australia");
        a13.setPostcode("4000");
        w13.setAddress(a13);
        w13.setPhoneNumber("123456789");
        w << w13;

        QLandmark w14;
        w14.setUrl(QUrl("http://example.com/testUrl"));
        w << w14;

        QLandmark w15;
        w15.setCategoryIds(catIds);
        w << w15;

        QTest::newRow("convert-collection")
        << ":/data/convert-collection-in.xml"
        << ":/data/convert-collection-out.xml"
        << ""
        << w
        << cats;
    }
};

void tst_QLandmarkFileHandler_Lmx::initTestCase()
{
    managerDataHolder.reset(new QLandmarkManagerDataHolder());
}

void tst_QLandmarkFileHandler_Lmx::cleanupTestCase() {
    managerDataHolder.reset(0);
    if (QFile::exists(m_exportFile))
        QFile::remove(m_exportFile);
}

QTEST_MAIN(tst_QLandmarkFileHandler_Lmx)
#include "tst_qlandmarkfilehandler_lmx.moc"
