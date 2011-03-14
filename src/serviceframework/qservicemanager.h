/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSERVICEMANAGER_H
#define QSERVICEMANAGER_H

#include "qmobilityglobal.h"

#include "qservice.h"
#include "qserviceinterfacedescriptor.h"
#include "qservicefilter.h"

#include <QObject>
#include <QList>
#include <QStringList>
#include <QDebug>


QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QServiceContext;
class QAbstractSecuritySession;
class QServiceFilter;
class QServiceManagerPrivate;
class Q_SERVICEFW_EXPORT QServiceManager : public QObject
{
    Q_OBJECT
public:

   enum Error {
        NoError,
        StorageAccessError,
        InvalidServiceLocation,
        InvalidServiceXml,
        InvalidServiceInterfaceDescriptor,
        ServiceAlreadyExists,
        ImplementationAlreadyExists,
        PluginLoadingFailed,
        ComponentNotFound,
        ServiceCapabilityDenied,
        UnknownError = 100
    };

    explicit QServiceManager(QObject *parent = 0);
    explicit QServiceManager(QService::Scope scope, QObject *parent = 0);
    ~QServiceManager();

    QService::Scope scope() const;

    QStringList findServices(const QString& interfaceName = QString()) const;
    QList<QServiceInterfaceDescriptor> findInterfaces(const QServiceFilter& filter = QServiceFilter()) const;
    QList<QServiceInterfaceDescriptor> findInterfaces(const QString& serviceName) const;

    QObject* loadInterface(const QString& interfaceName, QServiceContext* context = 0, QAbstractSecuritySession* session = 0);
    QObject* loadInterface(const QServiceInterfaceDescriptor& descriptor, QServiceContext* context = 0, QAbstractSecuritySession* session = 0);

    template <class T>
    T* loadLocalTypedInterface(const QString& interfaceName, QServiceContext* context = 0, QAbstractSecuritySession* session = 0)
    {
        return loadLocalTypedInterface<T>(interfaceDefault(interfaceName), context, session);
    }

    template <class T>
    T* loadLocalTypedInterface(const QServiceInterfaceDescriptor& descriptor, QServiceContext* context = 0, QAbstractSecuritySession* session = 0)
    {
        T* instance = 0;
        if (descriptor.isValid()) {
            QObject* obj = loadInterface(descriptor, context, session);
            if (!obj) return 0;

            //TODO this should really be
            //instance = qobject_cast<T *>(loadInterface(descriptor, context, session));
            //check why qobject_cast fails
            const char* templateClassName = reinterpret_cast<T *>(0)->staticMetaObject.className();
            const QMetaObject* source = obj->metaObject();
            do {
                if (strcmp(templateClassName,source->className())==0) {
                    instance = static_cast<T *>(obj);
                    break;
                }
                source = source->superClass();
            } while (source != 0);
            if (!instance)
                delete obj;
        }
        return instance;
    }

    bool addService(const QString& xmlFilePath);
    bool addService(QIODevice* xmlDevice);
    bool removeService(const QString& serviceName);

    bool setInterfaceDefault(const QString &service, const QString &interfaceName);
    bool setInterfaceDefault(const QServiceInterfaceDescriptor& descriptor);

    QServiceInterfaceDescriptor interfaceDefault(const QString& interfaceName) const;

    Error error() const;

protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);

Q_SIGNALS:
    void serviceAdded(const QString& serviceName, QService::Scope scope);
    void serviceRemoved(const QString& serviceName, QService::Scope scope);

private:
    friend class QServiceManagerPrivate;
    QServiceManagerPrivate* d;
};


QTM_END_NAMESPACE

QT_END_HEADER

#endif
