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

#include "qservicemanager.h"
#include "qserviceplugininterface.h"
#include "qabstractsecuritysession.h"
#ifdef Q_OS_SYMBIAN
    #include "databasemanager_s60_p.h"
#else
    #include "databasemanager_p.h"
#endif

#include <QObject>
#include <QPluginLoader>
#include <QFile>
#include <QCoreApplication>
#include <QDir>

QTM_BEGIN_NAMESPACE


static QString qservicemanager_resolveLibraryPath(const QString &libNameOrPath)
{
    if (QFile::exists(libNameOrPath))
        return libNameOrPath;

    // try to find plug-in via QLibrary
    QStringList paths = QCoreApplication::libraryPaths();
#ifdef QTM_PLUGIN_PATH
    paths << QLatin1String(QTM_PLUGIN_PATH)+QLatin1String("/serviceframework");
#endif
    for (int i=0; i<paths.count(); i++) {
        QString libPath = QDir::toNativeSeparators(paths[i]) + QDir::separator() + libNameOrPath;
        
#ifdef Q_OS_SYMBIAN
        QFileInfo fi(libPath);
        if (fi.suffix() == QLatin1String("dll"))
            libPath = fi.completeBaseName() + QLatin1String(".qtplugin");
        else
            libPath += QLatin1String(".qtplugin");

        QLibrary lib(libPath);
        if (QFile::exists(libPath) && lib.load()) {
            lib.unload();
            return libPath;
        }
#else       
        QLibrary lib(libPath);  
        if (lib.load()) {
            lib.unload();
            return lib.fileName();
        }
#endif        
    }
    return QString();
}


class QServicePluginCleanup : public QObject
{
    Q_OBJECT
public:
    QServicePluginCleanup(QPluginLoader *loader, QObject *parent = 0)
        : QObject(parent),
          m_loader(loader)
    {
    }

    ~QServicePluginCleanup()
    {
        if (m_loader) {
            m_loader->unload();
            delete m_loader;
        }
    }

    QPluginLoader *m_loader;
};

class QServiceManagerPrivate : public QObject
{
    Q_OBJECT
public:
    QServiceManager *manager;
    DatabaseManager *dbManager;
    QService::Scope scope;
    QServiceManager::Error error;

    QServiceManagerPrivate(QServiceManager *parent = 0)
        : QObject(parent),
          manager(parent),
          dbManager(new DatabaseManager)
    {
        connect(dbManager, SIGNAL(serviceAdded(QString, DatabaseManager::DbScope)),
                SLOT(serviceAdded(QString, DatabaseManager::DbScope)));
        connect(dbManager, SIGNAL(serviceRemoved(QString, DatabaseManager::DbScope)),
                SLOT(serviceRemoved(QString, DatabaseManager::DbScope)));
    }

    ~QServiceManagerPrivate()
    {
        delete dbManager;
    }

    void setError(QServiceManager::Error err)
    {
        error = err;
    }

    void setError()
    {
        switch (dbManager->lastError().code()) {
            case DBError::NoError:
                error = QServiceManager::NoError;
                break;
            case DBError::DatabaseNotOpen:
            case DBError::InvalidDatabaseConnection:
            case DBError::CannotCreateDbDir:
            case DBError::CannotOpenServiceDb:
            case DBError::NoWritePermissions:
            case DBError::InvalidDatabaseFile:
                error = QServiceManager::StorageAccessError;
                break;
            case DBError::LocationAlreadyRegistered:
                error = QServiceManager::ServiceAlreadyExists;
                break;
            case DBError::IfaceImplAlreadyRegistered:
                error = QServiceManager::ImplementationAlreadyExists;
                break;
            case DBError::NotFound:
                error = QServiceManager::ComponentNotFound;
                break;
            case DBError::InvalidDescriptorScope:
                error = QServiceManager::InvalidServiceInterfaceDescriptor;
                break;
            case DBError::SqlError:
            case DBError::IfaceIDNotExternal:
            case DBError::ExternalIfaceIDFound:
            case DBError::UnknownError:
                error = QServiceManager::UnknownError;
                break;
        }
    }

private slots:
    void serviceAdded(const QString &service, DatabaseManager::DbScope dbScope)
    {
        QService::Scope s = (dbScope == DatabaseManager::SystemScope ?
                QService::SystemScope : QService::UserScope);
        emit manager->serviceAdded(service, s);
    }

    void serviceRemoved(const QString &service, DatabaseManager::DbScope dbScope)
    {
        QService::Scope s = (dbScope == DatabaseManager::SystemScope ?
                QService::SystemScope : QService::UserScope);
        emit manager->serviceRemoved(service, s);
    }
};

/*!
    \class QServiceManager
    \ingroup servicefw
    \brief The QServiceManager class enables the loading of service plugins
    and the (de)registration of services.

    A service is a stand-alone component that can be used by multiple clients.
    Each service implementation must derive from QObject. Clients request a
    reference to a service via \l loadInterface() or \l loadLocalTypedInterface().

    Services are separate deliveries in the form of plug-ins. New services can be (de)registered
    at any time via \l addService() and \l removeService() respectively. Such an event is
    published via the \l serviceAdded() and \l serviceRemoved() signal.
    Each service plug-in must implement QServicePluginInterface.

    Each plug-in may support multiple interfaces and may even provide multiple implementations
    for the same interface. Individual implementations are identified via
    QServiceInterfaceDescriptor. For a more detailed explanation of services and how they relate to
    interface and their implementations please see QServiceInterfaceDescriptor.

    \sa QServicePluginInterface, QServiceContext, QAbstractSecuritySession
*/

/*!
    \enum QServiceManager::Error
    Defines the possible errors for the service manager.

    \value NoError No error occurred.
    \value StorageAccessError The service data storage is not accessible. This could be because the caller does not have the required permissions.
    \value InvalidServiceLocation The service was not found at its specified \l{QServiceInterfaceDescriptor::Location}{location}.
    \value InvalidServiceXml The XML defining the service metadata is invalid.
    \value InvalidServiceInterfaceDescriptor The service interface descriptor is invalid, or refers to an interface implementation that cannot be accessed in the current scope.
    \value ServiceAlreadyExists Another service has previously been registered with the same \l{QServiceInterfaceDescriptor::Location}{location}.
    \value ImplementationAlreadyExists Another service that implements the same interface version has previously been registered.
    \value PluginLoadingFailed The service plugin cannot be loaded.
    \value ComponentNotFound The service or interface implementation has not been registered.
    \value ServiceCapabilityDenied The security session does not allow the service based on its capabilities.
    \value UnknownError An unknown error occurred.
*/

/*!
    \fn void QServiceManager::serviceAdded(const QString& serviceName, QService::Scope scope)

    This signal is emited whenever a new service with the given
    \a serviceName has been registered with the service manager.
    \a scope indicates where the service was added.

    If the manager scope is QService::SystemScope, it will not receive
    notifications about services added in the user scope.

    \sa addService()
*/

/*!
    \fn void QServiceManager::serviceRemoved(const QString& serviceName, QService::Scope scope)

    This signal is emited whenever a service with the given
    \a serviceName has been deregistered with the service manager.
    \a scope indicates where the service was added.

    If the manager scope is QService::SystemScope, it will not receive
    notifications about services removed in the user scope.

    \sa removeService()
*/

/*!
    Creates a service manager with the given \a parent.

    The scope will default to QService::UserScope.
*/
QServiceManager::QServiceManager(QObject *parent)
    : QObject(parent),
      d(new QServiceManagerPrivate(this))
{
    d->scope = QService::UserScope;
}

/*!
    Creates a service manager with the given \a scope and \a parent.
*/
QServiceManager::QServiceManager(QService::Scope scope, QObject *parent)
    : QObject(parent),
      d(new QServiceManagerPrivate(this))
{
    d->scope = scope;
}

/*!
    Destroys the service manager.
*/
QServiceManager::~QServiceManager()
{
    delete d;
}

/*!
    Returns the scope used for registering and searching of services.
*/
QService::Scope QServiceManager::scope() const
{
    return d->scope;
}

/*!
    Returns a list of the services that provide the interface specified by
    \a interfaceName. If \a interfaceName is empty, this function returns
    a list of all available services in this manager's scope.
*/
QStringList QServiceManager::findServices(const QString& interfaceName) const
{
    d->setError(NoError);
    QStringList services;
    services = d->dbManager->getServiceNames(interfaceName,
            d->scope == QService::SystemScope ? DatabaseManager::SystemScope : DatabaseManager::UserScope);
    d->setError();
    return services;
}

/*!
    Returns a list of the interfaces that match the specified \a filter.
*/
QList<QServiceInterfaceDescriptor> QServiceManager::findInterfaces(const QServiceFilter& filter) const
{
    d->setError(NoError);
    QList<QServiceInterfaceDescriptor> descriptors = d->dbManager->getInterfaces(filter,
            d->scope == QService::SystemScope ? DatabaseManager::SystemScope : DatabaseManager::UserScope);
    if (descriptors.isEmpty() && d->dbManager->lastError().code() != DBError::NoError) {
        d->setError();
        return QList<QServiceInterfaceDescriptor>();
    }
    return descriptors;
}

/*!
    Returns a list of the interfaces provided by the service named
    \a serviceName. If \a serviceName is empty, this function returns
    a list of all available interfaces in this manager's scope.
*/
QList<QServiceInterfaceDescriptor> QServiceManager::findInterfaces(const QString& serviceName) const
{
    QServiceFilter filter;
    if (!serviceName.isEmpty())
        filter.setServiceName(serviceName);
    return findInterfaces(filter);
}

/*!
    Loads and returns the interface specified by \a interfaceName, as
    provided by the default service for this interface, using the given
    \a context and \a session. \a context and \a session object are owned
    by the caller of this function.

    The caller takes ownership of the returned pointer.

    This function returns a null pointer if the requested service cannot be found.

    The security session object is not mandatory. If the session pointer is null,
    the service manager will not perform any checks. Therefore it is assumed that
    the service manager client is trusted as it controls whether service capabilities
    are enforced during service loading.

    \sa setInterfaceDefault(), interfaceDefault()
*/
QObject* QServiceManager::loadInterface(const QString& interfaceName, QServiceContext* context, QAbstractSecuritySession* session)
{
    return loadInterface(interfaceDefault(interfaceName), context, session);
}

/*!
    Loads and returns the interface specified by \a descriptor using the
    given \a context and \a session. \a context and \a session object are owned
    by the caller of this function.

    The caller takes ownership of the returned pointer.

    This function returns a null pointer if the requested service cannot be found.

    The security session object is not mandatory. If the session pointer is null,
    the service manager will not perform any checks. Therefore it is assumed that
    the service manager client is trusted as it controls whether service capabilities
    are enforced during service loading.
*/
QObject* QServiceManager::loadInterface(const QServiceInterfaceDescriptor& descriptor, QServiceContext* context, QAbstractSecuritySession* session)
{
    d->setError(NoError);
    if (!descriptor.isValid()) {
        d->setError(InvalidServiceInterfaceDescriptor);
        return 0;
    }

    const QStringList serviceCaps = descriptor.attribute(QServiceInterfaceDescriptor::Capabilities).toStringList();
    if ( session && !session->isAllowed(serviceCaps) ) {
        d->setError(ServiceCapabilityDenied);
        return 0;
    }

    QString serviceFilePath = qservicemanager_resolveLibraryPath(
            descriptor.attribute(QServiceInterfaceDescriptor::Location).toString());
    if (serviceFilePath.isEmpty()) {
        d->setError(InvalidServiceLocation);
        return 0;
    }

    QPluginLoader *loader = new QPluginLoader(serviceFilePath);
    //pluginIFace is same for all service instances of the same plugin
    //calling loader->unload deletes pluginIFace automatically if now other
    //service instance is around
    QServicePluginInterface *pluginIFace = qobject_cast<QServicePluginInterface *>(loader->instance());
    if (pluginIFace) {
        QObject *obj = pluginIFace->createInstance(descriptor, context, session);
        if (obj) {
            QServicePluginCleanup *cleanup = new QServicePluginCleanup(loader);
            QObject::connect(obj, SIGNAL(destroyed()), cleanup, SLOT(deleteLater()));
            return obj;
        }
    }

    loader->unload();
    delete loader;
    d->setError(PluginLoadingFailed);

    return 0;
}

/*!
    \fn T* QServiceManager::loadLocalTypedInterface(const QString& interfaceName, QServiceContext* context, QAbstractSecuritySession* session)

    Loads the service object implementing \a interfaceName,
    as provided by the default service for this interface, using the given
    \a context and \a session. \a context and \a session object are owned
    by the caller of this function. The template class must be derived from QObject.

    If \a interfaceName is not a known interface the returned pointer will be null.

    Note that using this function implies that service and client share
    the implamentation of T which means that service and client become tightly coupled.
    This may cause issue during later updates as certain changes may require code changes
    to the service and client.

    The caller takes ownership of the returned pointer.

    The security session object is not mandatory. If the session pointer is null,
    the service manager will not perform any checks. Therefore it is assumed that
    the service manager client is trusted as it controls whether service capabilities
    are enforced during service loading.
    
    \sa setInterfaceDefault(), interfaceDefault()
*/


/*!
    \fn T* QServiceManager::loadLocalTypedInterface(const QServiceInterfaceDescriptor& serviceDescriptor, QServiceContext* context, QAbstractSecuritySession* session)

    Loads the service object identified by \a serviceDescriptor
    using the given \a context and \a session. \a context and \a session object are owned
    by the caller of this function. The template class must be derived from QObject.

    If the \a serviceDescriptor is not valid the returned pointer will be null.

    Note that using this function implies that service and client share
    the implamentation of T which means that service and client become tightly coupled.
    This may cause issue during later updates as certain changes may require code changes
    to the service and client.

    The caller takes ownership of the returned pointer.

    The security session object is not mandatory. If the session pointer is null,
    the service manager will not perform any checks. Therefore it is assumed that
    the service manager client is trusted as it controls whether service capabilities
    are enforced during service loading.
*/

/*!
    Registers the service defined by the XML file at \a xmlFilePath.
    Returns true if the registration succeeded, and false otherwise.

    If a previously unkown interface is added the newly registered service automatically
    becomes the new default service provider for the new interface.

    A service plugin cannot be added if another service is already registered
    with the same plugin file path.  A service plugin also cannot be added if
    the service is already registered and implements any of the same interface
    versions that the new plugin implements.

    \sa removeService(), setInterfaceDefault()
*/
bool QServiceManager::addService(const QString& xmlFilePath)
{
    QFile *f = new QFile(xmlFilePath);
    bool b = addService(f);
    delete f;
    return b;
}

/*!
    Registers the service defined by the XML data from the given \a device.
    Returns true if the registration succeeded, and false otherwise. If a
    previously unkown interface is added the newly registered service
    automatically becomes the new default service provider for the new
    interface.

    Registering a service also causes QServicePluginInterface::installService()
    to be called on the service. If the service plugin is not accessible
    (e.g. if the plugin file is not found) and \c installService() cannot
    be invoked on the service, the registration fails and this method returns
    false.

    A service plugin cannot be added if another service is already registered
    with the same plugin file path.  A service plugin also cannot be added if
    the service is already registered and implements any of the same interface
    versions that the new plugin implements.

    Services are always added based on the \l scope() of the current
    service manager instance.

    \sa removeService(), setInterfaceDefault()
*/
bool QServiceManager::addService(QIODevice *device)
{
    d->setError(NoError);
    ServiceMetaData parser(device);
    if (!parser.extractMetadata()) {
        d->setError(InvalidServiceXml);
        return false;
    }
    const ServiceMetaDataResults data = parser.parseResults();

    DatabaseManager::DbScope scope = d->scope == QService::UserScope ?
            DatabaseManager::UserOnlyScope : DatabaseManager::SystemScope;
    ServiceMetaDataResults results = parser.parseResults();
    bool result = d->dbManager->registerService(results, scope);
    if (result) {
        QPluginLoader *loader = new QPluginLoader(qservicemanager_resolveLibraryPath(data.location));
        QServicePluginInterface *pluginIFace = qobject_cast<QServicePluginInterface *>(loader->instance());
        if (pluginIFace) {
            pluginIFace->installService();
        } else {
            d->setError(PluginLoadingFailed);
            result = false;
            d->dbManager->unregisterService(data.name, scope);
        }
        loader->unload();
        delete loader;
    } else {
        d->setError();
    }
    return result;
}

/*!
    Unregisters the service specified by \a serviceName.

    Returns true if the unregistration succeeded, and false otherwise.

    If a default service implementation is removed and there are other implementations
    for the same interface, the service manager chooses the implementation with the
    highest version number as the new default.  If there is more than one serivce
    with the same version number, the service manager makes a random choice with
    regards to the new default implementation. If this is
    not the desired behaviour the default selection should be updated
    via setInterfaceDefault().

    Services are always removed based on the \l scope() of the current
    service manager instance.

    \sa addService()
*/
bool QServiceManager::removeService(const QString& serviceName)
{
    d->setError(NoError);
    if (serviceName.isEmpty()) {
        d->setError(ComponentNotFound);
        return false;
    }

    // Call QServicePluginInterface::uninstallService() on all plugins that
    // match this service

    QSet<QString> pluginPathsSet;
    QList<QServiceInterfaceDescriptor> descriptors = findInterfaces(serviceName);
    for (int i=0; i<descriptors.count(); i++)
        pluginPathsSet << descriptors[i].attribute(QServiceInterfaceDescriptor::Location).toString();

    QList<QString> pluginPaths = pluginPathsSet.toList();
    for (int i=0; i<pluginPaths.count(); i++) {
        QPluginLoader *loader = new QPluginLoader(qservicemanager_resolveLibraryPath(pluginPaths[i]));
        QServicePluginInterface *pluginIFace = qobject_cast<QServicePluginInterface *>(loader->instance());
        if (pluginIFace)
            pluginIFace->uninstallService();
        else
            qWarning() << "QServiceManager: unable to invoke uninstallService() on removed service";
        loader->unload();
        delete loader;
    }

    if (!d->dbManager->unregisterService(serviceName, d->scope == QService::UserScope ?
            DatabaseManager::UserOnlyScope : DatabaseManager::SystemScope)) {
        d->setError();
        return false;
    }
    return true;
}

/*!
    Sets the default interface implementation for \a interfaceName to the
    matching interface implementation provided by \a service.

    If \a service provides more than one interface implementation for
    \a interfaceName, the newest version of the interface is set as the
    default.

    Returns true if the operation succeeded, and false otherwise.

    \bold {Note:} When in system scope, the \a service must be a system-wide
    service rather than a user-specific service; otherwise, this will fail.
*/
bool QServiceManager::setInterfaceDefault(const QString &service, const QString &interfaceName)
{
    d->setError(NoError);
    if (service.isEmpty() || interfaceName.isEmpty()) {
        d->setError(ComponentNotFound);
        return false;
    }
    DatabaseManager::DbScope scope = d->scope == QService::SystemScope ?
            DatabaseManager::SystemScope : DatabaseManager::UserScope;
    if (!d->dbManager->setInterfaceDefault(service, interfaceName, scope)) {
        d->setError();
        return false;
    }
    return true;
}

/*!
    \overload

    Sets the interface implementation specified by \a descriptor to be the
    default implementation for the particular interface specified in the
    descriptor.

    Returns true if the operation succeeded, and false otherwise.

    \bold {Note:} When in system scope, the \a descriptor must refer to a
    system-wide service rather than a user-specific service; otherwise, this
    will fail.
*/
bool QServiceManager::setInterfaceDefault(const QServiceInterfaceDescriptor& descriptor)
{
    d->setError(NoError);
    DatabaseManager::DbScope scope = d->scope == QService::SystemScope ?
            DatabaseManager::SystemScope : DatabaseManager::UserScope;
    if (!d->dbManager->setInterfaceDefault(descriptor, scope)) {
        d->setError();
        return false;
    }
    return true;
}

/*!
    Returns the default interface implementation for the given \a interfaceName.
*/
QServiceInterfaceDescriptor QServiceManager::interfaceDefault(const QString& interfaceName) const
{
    d->setError(NoError);
    DatabaseManager::DbScope scope = d->scope == QService::SystemScope ?
            DatabaseManager::SystemScope : DatabaseManager::UserScope;
    QServiceInterfaceDescriptor info = d->dbManager->interfaceDefault(interfaceName, scope);
    if (d->dbManager->lastError().code() != DBError::NoError) {
        d->setError();
        return QServiceInterfaceDescriptor();
    }
    return info;
}

/*!
    Returns the type of error that last occurred.
*/
QServiceManager::Error QServiceManager::error() const
{
    return d->error;
}

/*!
    \internal
*/
void QServiceManager::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(serviceAdded(QString,QService::Scope))
            || QLatin1String(signal) == SIGNAL(serviceRemoved(QString,QService::Scope))) {
        if (d->scope != QService::SystemScope)
            d->dbManager->setChangeNotificationsEnabled(DatabaseManager::UserScope, true);
        d->dbManager->setChangeNotificationsEnabled(DatabaseManager::SystemScope, true);
    }
}

/*!
    \internal
*/
void QServiceManager::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(serviceAdded(QString,QService::Scope))
            || QLatin1String(signal) == SIGNAL(serviceRemoved(QString,QService::Scope))) {
        if (receivers(SIGNAL(serviceAdded(QString,QService::Scope))) == 0
                && receivers(SIGNAL(serviceRemoved(QString,QService::Scope))) == 0) {
            if (d->scope != QService::SystemScope)
                d->dbManager->setChangeNotificationsEnabled(DatabaseManager::UserScope, false);
            d->dbManager->setChangeNotificationsEnabled(DatabaseManager::SystemScope, false);
        }
    }
}

#include "moc_qservicemanager.cpp"
#include "qservicemanager.moc"
QTM_END_NAMESPACE

