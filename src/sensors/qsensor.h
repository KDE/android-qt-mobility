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

#ifndef QSENSOR_H
#define QSENSOR_H

#include <qmobilityglobal.h>
#include <QObject>
#include <QByteArray>
#include <QMetaType>
#include <QVariant>

QTM_BEGIN_NAMESPACE

class QSensorPrivate;
class QSensorBackend;
class QSensorReading;
class QSensorReadingPrivate;
class QSensorFilter;

typedef quint64 qtimestamp;

class Q_SENSORS_EXPORT QSensor : public QObject
{
    friend class QSensorBackend;

    Q_OBJECT
    Q_ENUMS(UpdatePolicy)
    Q_FLAGS(UpdatePolicies)
    Q_PROPERTY(QByteArray sensorid READ identifier WRITE setIdentifier)
    Q_PROPERTY(QByteArray type READ type WRITE setType)
    Q_PROPERTY(bool connected READ isConnected)
    Q_PROPERTY(UpdatePolicies supportedUpdatePolicies READ supportedUpdatePolicies)
    Q_PROPERTY(UpdatePolicy updatePolicy READ updatePolicy WRITE setUpdatePolicy)
    Q_PROPERTY(int updateInterval READ updateInterval WRITE setUpdateInterval)
    Q_PROPERTY(QSensorReading* reading READ reading NOTIFY readingChanged)
    Q_PROPERTY(bool running READ isActive WRITE setActive)
public:
    explicit QSensor(QObject *parent = 0);
    virtual ~QSensor();

    QByteArray identifier() const;
    void setIdentifier(const QByteArray &identifier);

    QByteArray type() const;
    void setType(const QByteArray &type);

    Q_INVOKABLE bool connect();
    bool isConnected() const;

    bool isActive() const;
    void setActive(bool running);

    bool isSignalEnabled() const;
    void setSignalEnabled(bool enabled);

    enum UpdatePolicy {
        Undefined         = 0x00, // If the sensor has no specific policy

        // Used by irregularly updating sensors
        OnChangeUpdates   = 0x01,

        // Used by continuously updating sensors
        OccasionalUpdates = 0x02,
        InfrequentUpdates = 0x04,
        FrequentUpdates   = 0x08,

        // For more control
        TimedUpdates      = 0x10, // Every x milliseconds (may not be supported by all sensors)
        PolledUpdates     = 0x20  // As often as polled (may not be supported by all sensors)
    };
    Q_DECLARE_FLAGS(UpdatePolicies, UpdatePolicy)

    // What policies does the sensor support
    UpdatePolicies supportedUpdatePolicies() const;

    // Set the desired update policy (default is defined by the sensor)
    // Use documentation to determine the policies that the sensor
    // supports.
    void setUpdatePolicy(UpdatePolicy policy);
    void setUpdateInterval(int interval);

    // Retrieve the policy
    UpdatePolicy updatePolicy() const;
    int updateInterval() const;

    // Filters modify the reading
    void addFilter(QSensorFilter *filter);
    void removeFilter(QSensorFilter *filter);

    // Poll for sensor change (only if using PolledUpdates)
    void poll();

    // The readings are exposed via this object
    QSensorReading *reading() const;

    // Information about available sensors
    static QList<QByteArray> sensorTypes();
    static QList<QByteArray> sensorsForType(const QByteArray &type);
    static QByteArray defaultSensorForType(const QByteArray &type);

public Q_SLOTS:
    // Start receiving values from the sensor
    void start();

    // Stop receiving values from the sensor
    void stop();

Q_SIGNALS:
    void readingChanged();

protected:
    // called by the back end
    QSensorPrivate *d_func() const { return d.data(); }

private:
    QScopedPointer<QSensorPrivate> d;
    Q_DISABLE_COPY(QSensor)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QSensor::UpdatePolicies)

class Q_SENSORS_EXPORT QSensorFilter
{
    friend class QSensor;
public:
    virtual bool filter(QSensorReading *reading) = 0;
protected:
    QSensorFilter();
    ~QSensorFilter();
    virtual void setSensor(QSensor *sensor);
    QSensor *m_sensor;
};

class Q_SENSORS_EXPORT QSensorReading : public QObject
{
    friend class QSensorBackend;

    Q_OBJECT
    Q_PROPERTY(QtMobility::qtimestamp timestamp READ timestamp)
public:
    virtual ~QSensorReading();

    qtimestamp timestamp() const;
    void setTimestamp(qtimestamp timestamp);

    // Access properties of sub-classes by numeric index
    // For name-based access use QObject::property()
    int valueCount() const;
    // default impl is slow but sub-classes can make faster implementations
    virtual QVariant value(int index) const;

protected:
    explicit QSensorReading(QObject *parent, QSensorReadingPrivate *d);
    QScopedPointer<QSensorReadingPrivate> *d_ptr() { return &d; }
    virtual void copyValuesFrom(QSensorReading *other) = 0;

private:
    QScopedPointer<QSensorReadingPrivate> d;
    Q_DISABLE_COPY(QSensorReading)
};

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

#define IMPLEMENT_READING(classname)\
        IMPLEMENT_READING_D(classname, classname ## Private)

#define IMPLEMENT_READING_D(classname, pclassname)\
    classname::classname(QObject *parent)\
        : QSensorReading(parent, new pclassname)\
        , d(d_ptr())\
        {}\
    classname::~classname() {}\
    void classname::copyValuesFrom(QSensorReading *_other)\
    {\
        /* No need to verify types, only called by QSensorBackend */\
        classname *other = static_cast<classname *>(_other);\
        pclassname *my_ptr = static_cast<pclassname*>(d_ptr()->data());\
        pclassname *other_ptr = static_cast<pclassname*>(other->d_ptr()->data());\
        /* Do a direct copy of the private class */\
        *(my_ptr) = *(other_ptr);\
    }

QTM_END_NAMESPACE

Q_DECLARE_METATYPE(QtMobility::qtimestamp)

#endif

