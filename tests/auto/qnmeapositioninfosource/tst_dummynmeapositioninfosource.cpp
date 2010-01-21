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

#include "qnmeapositioninfosourceproxyfactory.h"
#include "../testqgeopositioninfosource_p.h"
#include "../qlocationtestutils_p.h"

#include <qnmeapositioninfosource.h>
#include <QSignalSpy>
#include <QTest>

QTM_USE_NAMESPACE
Q_DECLARE_METATYPE(QNmeaPositionInfoSource::UpdateMode)
Q_DECLARE_METATYPE(QGeoPositionInfo)

class DummyNmeaPositionInfoSource : public QNmeaPositionInfoSource
{
    Q_OBJECT

public:
    DummyNmeaPositionInfoSource(QNmeaPositionInfoSource::UpdateMode mode, QObject *parent = 0);

protected:
    virtual bool parsePosInfoFromNmeaData(const char *data,
                                          int size,
                                          QGeoPositionInfo *posInfo,
                                          bool *hasFix);

private:
    int callCount;
};

DummyNmeaPositionInfoSource::DummyNmeaPositionInfoSource(QNmeaPositionInfoSource::UpdateMode mode, QObject *parent) :
        QNmeaPositionInfoSource(mode, parent),
        callCount(0)
{
}

bool DummyNmeaPositionInfoSource::parsePosInfoFromNmeaData(const char* data,
                                                           int size,
                                                           QGeoPositionInfo *posInfo,
                                                           bool *hasFix)
{
    Q_UNUSED(data);
    Q_UNUSED(size);

    posInfo->setCoordinate(QGeoCoordinate(callCount * 1.0, callCount * 1.0, callCount * 1.0));
    posInfo->setDateTime(QDateTime::currentDateTime().toUTC());
    *hasFix = true;
    ++callCount;

    return true;
}

class tst_DummyNmeaPositionInfoSource : public QObject
{
    Q_OBJECT

public:
    tst_DummyNmeaPositionInfoSource();

private slots:
    void initTestCase();
    void testOverloadedParseFunction();
};


tst_DummyNmeaPositionInfoSource::tst_DummyNmeaPositionInfoSource() {}

void tst_DummyNmeaPositionInfoSource::initTestCase()
{
    qRegisterMetaType<QGeoPositionInfo>();
}

void tst_DummyNmeaPositionInfoSource::testOverloadedParseFunction()
{
    DummyNmeaPositionInfoSource source(QNmeaPositionInfoSource::RealTimeMode);
    QNmeaPositionInfoSourceProxyFactory factory;
    QNmeaPositionInfoSourceProxy *proxy = static_cast<QNmeaPositionInfoSourceProxy*>(factory.createProxy(&source));

    QSignalSpy spy(proxy->source(), SIGNAL(positionUpdated(QGeoPositionInfo)));

    QGeoPositionInfo pos;

    proxy->source()->startUpdates();

    proxy->feedBytes(QString("The parser converts\n").toAscii());

    QTRY_VERIFY_WITH_TIMEOUT((spy.count() == 1), 10000);
    pos = spy.at(0).at(0).value<QGeoPositionInfo>();

    QVERIFY((pos.coordinate().latitude() == 0.0)
        && (pos.coordinate().longitude() == 0.0)
        && (pos.coordinate().altitude() == 0.0));

    spy.clear();

    proxy->feedBytes(QString("any data it receives\n").toAscii());

    QTRY_VERIFY_WITH_TIMEOUT((spy.count() == 1), 10000);
    pos = spy.at(0).at(0).value<QGeoPositionInfo>();

    QVERIFY((pos.coordinate().latitude() == 1.0)
        && (pos.coordinate().longitude() == 1.0)
        && (pos.coordinate().altitude() == 1.0));

    spy.clear();

    proxy->feedBytes(QString("into positions\n").toAscii());

    QTRY_VERIFY_WITH_TIMEOUT((spy.count() == 1), 10000);
    pos = spy.at(0).at(0).value<QGeoPositionInfo>();

    QVERIFY((pos.coordinate().latitude() == 2.0)
        && (pos.coordinate().longitude() == 2.0)
        && (pos.coordinate().altitude() == 2.0));

    spy.clear();
}

#include "tst_dummynmeapositioninfosource.moc"

QTEST_MAIN(tst_DummyNmeaPositionInfoSource);
