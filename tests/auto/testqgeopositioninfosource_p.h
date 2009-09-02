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
#ifndef TESTQGEOPOSITIONINFOSOURCE_P_H
#define TESTQGEOPOSITIONINFOSOURCE_P_H

#include <QTest>
#include <QObject>

class QGeoPositionInfoSource;

class TestQGeoPositionInfoSource : public QObject
{
    Q_OBJECT

protected:
    virtual QGeoPositionInfoSource *createTestSource() = 0;

    // MUST be called by subclasses if they override respective test slots
    void base_initTestCase();
    void base_init();
    void base_cleanup();
    void base_cleanupTestCase();

private:
    QGeoPositionInfoSource *m_source;

private slots:
    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    void constructor_withParent();

    void constructor_noParent();

    void updateInterval();

    void setPreferredPositioningMethods();
    void setPreferredPositioningMethods_data();

    void preferredPositioningMethods();

    void createSource();

    void setUpdateInterval();
    void setUpdateInterval_data();

    void lastKnownPosition();

    void minimumUpdateInterval();

    void startUpdates_testIntervals();

    void startUpdates_testDefaultInterval();

    void startUpdates_testZeroInterval();

    void stopUpdates();

    void stopUpdates_withoutStart();

    void requestUpdate();
    void requestUpdate_data();

    void requestUpdate_validTimeout();

    void requestUpdate_timeoutLessThanMinimumInterval();

    void requestUpdate_repeatedCalls();

    void requestUpdate_withUpdateInterval();
};


#endif
