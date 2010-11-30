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

#include <qmobilityglobal.h>
#include <QTimer>
#include "instancemanager_p.h"
#include "qremoteserviceregisterentry_p.h"

QTM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(InstanceManager, typeRegister);

/*!
    \internal

    Returns the instance manager for the service process
*/
InstanceManager* InstanceManager::instance()
{
    return typeRegister();
}

InstanceManager::InstanceManager(QObject *parent)
    : QObject(parent)
{
}

InstanceManager::~InstanceManager()
{
    QList<QRemoteServiceRegister::Entry> allEntries = metaMap.keys();
    while (!allEntries.isEmpty()) {
        ServiceIdentDescriptor descr = metaMap.take(allEntries.takeFirst());
        if (descr.entryData->instanceType == QRemoteServiceRegister::GlobalInstance) {
            if (descr.globalInstance)
               QTimer::singleShot(0, descr.globalInstance, SLOT(deleteLater())); // Symbian issue, use timer
            descr.globalInstance = 0;
        } else {
            QList<QUuid> allUuids = descr.individualInstances.keys();
            while (!allUuids.isEmpty()) {
                QTimer::singleShot(0, descr.individualInstances.take(allUuids.takeFirst()), SLOT(deleteLater())); // Symbian issue
            }
        }
    }

}

/*!
    \internal
    
    Adds an entry to the map of service identifiers
*/
bool InstanceManager::addType(const QRemoteServiceRegister::Entry& e)
{
    QMutexLocker ml(&lock);

    if (metaMap.contains(e)) {
        qWarning() << "Service" << e.serviceName() << "(" << e.interfaceName()
            << ", " << e.version() << ")" << "already registered";
    } else {
        ServiceIdentDescriptor d;
        d.entryData = e.d;
        metaMap.insert(e, d);
        return true;
    }
    return false;
}

/*!
    \internal

    Returns the metaobject of a registered service object identified by its \a entry
*/
const QMetaObject* InstanceManager::metaObject(const QRemoteServiceRegister::Entry& entry) const
{
    QMutexLocker ml(&lock);
    if (metaMap.contains(entry)) {
        return metaMap[entry].entryData->meta;
    } else {
        return 0;
    }
}

/*!
   \internal

   Returns a list of all the registered entries
*/
QList<QRemoteServiceRegister::Entry> InstanceManager::allEntries() const
{
    QMutexLocker ml(&lock);
    return metaMap.keys();
}

/*!
    \internal

    Instance manager takes ownership of service instance. Returns a null pointer
    if \a entry cannot be mapped to a known meta object. The \a instanceId will
    contain the unique ID for the new service instance.
*/
QObject* InstanceManager::createObjectInstance(const QRemoteServiceRegister::Entry& entry, QUuid& instanceId)
{
    instanceId = QUuid();
    QMutexLocker ml(&lock);
    if (!metaMap.contains(entry))
        return 0;

    QObject* service = 0;
    ServiceIdentDescriptor& descr = metaMap[entry];

    if (descr.entryData->instanceType == QRemoteServiceRegister::GlobalInstance) {
        if (descr.globalInstance) {
            service = descr.globalInstance;
            instanceId = descr.globalId;
            descr.globalRefCount++;
        } else {
            service = (*descr.entryData->cptr)();
            if (!service)
                return 0;

            descr.globalInstance = service;
            descr.globalId = instanceId = QUuid::createUuid();
            descr.globalRefCount = 1;
        }
    } else {
        service = (*descr.entryData->cptr)();
        if (!service)
            return 0;
        instanceId = QUuid::createUuid();
        descr.individualInstances.insert(instanceId, service);
    }

    return service;
}

/*!
    \internal

    The associated service object instance will be deleted in the service process.
    Removes an instance with \a instanceId from a map of remote service descriptors
    using the \a entry as the key.

    Emits instanceClosed() and allInstancesClosed() if no more instances are open
*/
void InstanceManager::removeObjectInstance(const QRemoteServiceRegister::Entry& entry, const QUuid& instanceId)
{
    QMutexLocker ml(&lock);
    if (!metaMap.contains(entry))
        return;

    ServiceIdentDescriptor& descr = metaMap[entry];
    if (descr.entryData->instanceType == QRemoteServiceRegister::GlobalInstance) {        
        if (descr.globalRefCount < 1)
            return;

        if (descr.globalRefCount == 1) {            
            if (descr.globalInstance)
                QTimer::singleShot(0, descr.globalInstance, SLOT(deleteLater()));
            descr.globalInstance = 0;
            descr.globalId = QUuid();
            descr.globalRefCount = 0;
            emit instanceClosed(entry);
            emit instanceClosed(entry, instanceId);    //internal use
        } else {
            descr.globalRefCount--;
        }
    } else {
        QObject* service = descr.individualInstances.take(instanceId);
        if (service) {
            QTimer::singleShot(0, service, SLOT(deleteLater())); // symbian issue
            emit instanceClosed(entry);
            emit instanceClosed(entry, instanceId);    //internal use
        }
    }

    // Check that no instances are open
    if (totalInstances() < 1)
        emit allInstancesClosed();
}

/*!
    \internal

    Provides a count of how many global and private instances are currently open
*/
int InstanceManager::totalInstances() const
{
    int total = 0;

    QList<QRemoteServiceRegister::Entry> allEntries = metaMap.keys();
    foreach (const QRemoteServiceRegister::Entry& entry, allEntries) {
        ServiceIdentDescriptor descr = metaMap[entry];
        total += descr.globalRefCount;
        total += descr.individualInstances.size();
    }
   
    return total;
}

#include "moc_instancemanager_p.cpp"

QTM_END_NAMESPACE
