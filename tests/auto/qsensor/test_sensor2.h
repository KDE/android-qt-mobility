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

#ifndef TEST_SENSOR2_H
#define TEST_SENSOR2_H

#define UNIT_TEST
#include "qsensor.h"

QTM_BEGIN_NAMESPACE

#undef DECLARE_READING
#undef DECLARE_READING_D

template <typename T>
class qTypedWrapper
{
public:
    qTypedWrapper(QScopedPointer<QSensorReadingPrivate> *_ptr)
        : ptr(_ptr)
    {
    }

    T *operator->() const
    {
        return static_cast<T*>(ptr->data());
    }

private:
    QScopedPointer<QSensorReadingPrivate> *ptr;
};

#define DECLARE_READING(classname)\
        DECLARE_READING_D(classname, classname ## Private)

#define DECLARE_READING_D(classname, pclassname)\
    public:\
        classname(QObject *parent = 0);\
        virtual ~classname();\
        void copyValuesFrom(QSensorReading *other);\
    private:\
        qTypedWrapper<pclassname> d;

QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class TestSensor2ReadingPrivate;

class TestSensor2Reading : public QSensorReading
{
    Q_OBJECT
    Q_PROPERTY(int test READ test)
    DECLARE_READING(TestSensor2Reading)
public:
    int test() const;
    void setTest(int test);
};

class TestSensor2Filter : public QSensorFilter
{
public:
    virtual bool filter(TestSensor2Reading *reading) = 0;
private:
    bool filter(QSensorReading *reading) { return filter(static_cast<TestSensor2Reading*>(reading)); }
};

class TestSensor2 : public QSensor
{
    Q_OBJECT
public:
    explicit TestSensor2(QObject *parent = 0) : QSensor(TestSensor2::type, parent) {}
    virtual ~TestSensor2() {}
    TestSensor2Reading *reading() const { return static_cast<TestSensor2Reading*>(QSensor::reading()); }
    static char const * const type;
};

#endif
