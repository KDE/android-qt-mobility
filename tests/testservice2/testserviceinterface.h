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
#ifndef TESTSERVICEINTERFACE_H
#define TESTSERVICEINTERFACE_H

#include <QObject>

class ISimpleTypeTest
{
public:
    virtual ~ISimpleTypeTest() {}
    virtual void callSlotAndSetName(const QString &s) = 0;
    virtual QString callSlotAndReturnName() const = 0;
};
Q_DECLARE_INTERFACE(ISimpleTypeTest, "com.nokia.qt.test.ISimpleTypeTest/1");


class IComplexTypeTest
{
public:
    virtual ~IComplexTypeTest() {}
    virtual QObject* embeddedTestService() = 0;

};
Q_DECLARE_INTERFACE(IComplexTypeTest, "com.nokia.qt.test.IComplexTypeTest/1");


#endif //TESTSERVICEINTERFACE_H
