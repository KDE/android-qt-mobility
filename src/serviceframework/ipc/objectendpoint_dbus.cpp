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

#include "objectendpoint_dbus_p.h"
#include "instancemanager_p.h"
#include "qmetaobjectbuilder_p.h"
#include "proxyobject_p.h"
#include "qsignalintercepter_p.h"
#include <QTimer>
#include <QEventLoop>
#include <QVarLengthArray>

QTM_BEGIN_NAMESPACE

class Response
{
public:
    Response() : isFinished(false), result(0) 
    { }

    bool isFinished;
    void* result;
};

typedef QHash<QUuid, Response*> Replies;
Q_GLOBAL_STATIC(Replies, openRequests);

class ServiceSignalIntercepter : public QSignalIntercepter
{
    //Do not put Q_OBJECT here
public:
    ServiceSignalIntercepter(QObject* sender, const QByteArray& signal,
            ObjectEndPoint* parent)
        : QSignalIntercepter(sender, signal, parent), endPoint(parent)
    {
        
    }

    void setMetaIndex(int index)
    {
        metaIndex = index;
    }

protected:
    void activated( const QList<QVariant>& args )
    {
        endPoint->invokeRemote(metaIndex, args, QMetaType::Void);
    }
private:
    ObjectEndPoint* endPoint;
    int metaIndex;
};

struct ClientInstance {
    QString clientId;
    QRemoteServiceRegister::Entry entry;
    QUuid instanceId;
    int ref;
};

class ObjectEndPointPrivate
{
public:
    ObjectEndPointPrivate()
    {
    }

    ~ObjectEndPointPrivate()
    {
    }

    // Used on client and service side
    ObjectEndPoint::Type endPointType;
    ObjectEndPoint* parent;

    // Used to calculate the registered paths on DBus
    QRemoteServiceRegister::Entry entry;
    QUuid serviceInstanceId;

    // Service side local client ownership list
    QList<ClientInstance> clientList;
};

/*!
    Client to service communication only used for establishing an object request since the returned
    proxy is an interface to that object registered on QtDBus. Client communicates directly to QtDBus
    for method and property access. Signals are automatically relayed from QtDBus to the proxy object.
*/
ObjectEndPoint::ObjectEndPoint(Type type, QServiceIpcEndPoint* comm, QObject* parent)
    : QObject(parent), dispatch(comm), service(0), iface(0)
{
    Q_ASSERT(dispatch);
    d = new ObjectEndPointPrivate;
    d->parent = this;
    d->endPointType = type;

    dispatch->setParent(this);
    connect(dispatch, SIGNAL(readyRead()), this, SLOT(newPackageReady()));
    if (type == Client) {
        // client waiting for construct proxy and registers DBus custom type
        qDBusRegisterMetaType<QTM_PREPEND_NAMESPACE(QServiceUserTypeDBus)>();
        qRegisterMetaType<QTM_PREPEND_NAMESPACE(QServiceUserTypeDBus)>();
        return;
    } else {
        connect(InstanceManager::instance(), 
                SIGNAL(instanceClosed(QRemoteServiceRegister::Entry,QUuid)),
                this, SLOT(unregisterObjectDBus(QRemoteServiceRegister::Entry,QUuid)));
        
        if (dispatch->packageAvailable())
            QTimer::singleShot(0, this, SLOT(newPackageReady()));
    }
}

ObjectEndPoint::~ObjectEndPoint()
{
    if(iface)
       delete  iface;
    delete d;
}

/*!
    Removes all instances of the client from the instance manager
*/
void ObjectEndPoint::disconnected(const QString& clientId, const QString& instanceId)
{
    // Service Side
    Q_ASSERT(d->endPointType != ObjectEndPoint::Client);

    for (int i=d->clientList.size()-1; i>=0; i--) {
        // Find right client process
        if (d->clientList[i].clientId == clientId) {
            if (d->clientList[i].ref-- == 1) {
                QRemoteServiceRegister::Entry entry = d->clientList[i].entry;
                QUuid instance = d->clientList[i].instanceId;

                if (instance.toString() == instanceId) {
                    // Remove an instance from the InstanceManager and local list
                    InstanceManager::instance()->removeObjectInstance(entry, instance);
                    d->clientList.removeAt(i);
                }
            }
        }
    }
}

/*!
    Unregisters the DBus object
*/
void ObjectEndPoint::unregisterObjectDBus(const QRemoteServiceRegister::Entry& entry, const QUuid& id)
{
    uint hash = qHash(id.toString());
    QString objPath = "/" + entry.interfaceName() + "/" + entry.version() +
        "/" + QString::number(hash);
    objPath.replace(QString("."), QString("/"));
    QDBusConnection::sessionBus().unregisterObject(objPath, QDBusConnection::UnregisterTree);
}

/*!
    Client requests proxy object. The proxy is owned by calling
    code and this object must clean itself up upon destruction of
    proxy.
*/
QObject* ObjectEndPoint::constructProxy(const QRemoteServiceRegister::Entry& entry)
{
    // Client side 
    Q_ASSERT(d->endPointType == ObjectEndPoint::Client);

    // Request a serialized meta object
    QServicePackage p;
    p.d = new QServicePackagePrivate();
    p.d->messageId = QUuid::createUuid();
    p.d->entry = entry;

    Response* response = new Response();
    openRequests()->insert(p.d->messageId, response);

    dispatch->writePackage(p);
    waitForResponse(p.d->messageId);

    // Get the proxy based on the meta object
    if (response->isFinished) {
        if (response->result == 0)
            qWarning() << "Request for remote service failed";
        else 
            service = reinterpret_cast<QServiceProxy* >(response->result);
    } else {
        qDebug() << "response passed but not finished";
    }

    openRequests()->take(p.d->messageId);
    delete response;

    if (!service)
        return 0;
        
    // Connect all DBus interface signals to the proxy slots
    const QMetaObject *mo = service->metaObject();
    while (mo && strcmp(mo->className(), "QObject")) {
        for (int i = mo->methodOffset(); i < mo->methodCount(); i++) {
            const QMetaMethod mm = mo->method(i);
            if (mm.methodType() == QMetaMethod::Signal) {
                QByteArray sig(mm.signature());

                bool customType = false;

                QList<QByteArray> params = mm.parameterTypes();
                for (int arg = 0; arg < params.size(); arg++) {
                    const QByteArray& type = params[arg];
                    int variantType = QVariant::nameToType(type);
                    if (variantType == QVariant::UserType) {
                        sig.replace(QByteArray(type), QByteArray("QDBusVariant")); 
                        customType = true;
                    }
                }

                int serviceIndex = iface->metaObject()->indexOfSignal(sig);
                QByteArray signal = QByteArray("2").append(sig);

                if (serviceIndex > 0) {
                    if (customType) {
                        QObject::connect(iface, signal.constData(), signalsObject, signal.constData());

                        ServiceSignalIntercepter *intercept = 
                            new ServiceSignalIntercepter((QObject*)signalsObject, signal, this);
                        intercept->setMetaIndex(i);
                    } else {
                        QObject::connect(iface, signal.constData(), service, signal.constData());
                    }
                }
            }
        }
        mo = mo->superClass();
    }

    return service;
}

/*!
    Received a new package from the DBus client-server controller.
    Once an object request is handled there is only direct communication to the DBus object so
    no other package types should be received on this layer.
*/
void ObjectEndPoint::newPackageReady()
{
    // Client and service side
    while(dispatch->packageAvailable())
    {
        QServicePackage p = dispatch->nextPackage();
        if (!p.isValid())
            continue;

        if (p.d->packageType == QServicePackage::ObjectCreation) {
            objectRequest(p);
        } else {
            qWarning() << "Unknown package type received.";
        }
    }
}

/*!
    Service finds existing objects or spawns new object instances and registers them on DBus using a
    hash of the unique instance ID. This registered object has a special metaobject representation
    of the service that is compatible with the QDBus type system.
    
    Client receives a package containing the information to connect an interface to the registered
    DBus object.
*/
void ObjectEndPoint::objectRequest(const QServicePackage& p)
{
    if (p.d->responseType != QServicePackage::NotAResponse ) {
        // Client side
        Q_ASSERT(d->endPointType == ObjectEndPoint::Client);

        d->serviceInstanceId = p.d->instanceId;
        d->entry = p.d->entry;

        Response* response = openRequests()->value(p.d->messageId);
        if (p.d->responseType == QServicePackage::Failed) {
            response->result = 0;
            response->isFinished = true;
            QTimer::singleShot(0, this, SIGNAL(pendingRequestFinished()));
            qWarning() << "Service instantiation failed";
            return;
        }

        // Deserialize meta object and create proxy object
        QServiceProxy* proxy = new QServiceProxy(p.d->payload.toByteArray(), this);
        response->result = reinterpret_cast<void *>(proxy);
        response->isFinished = true;

        // Create DBUS interface by using a hash of the service instance ID
        QString serviceName = "com.nokia.qtmobility.sfw." + p.d->entry.serviceName();
        uint hash = qHash(d->serviceInstanceId.toString());
        QString objPath = "/" + p.d->entry.interfaceName() + "/" + p.d->entry.version() + "/" + QString::number(hash);
        objPath.replace(QString("."), QString("/"));
     
#ifdef DEBUG
        qDebug() << "Client Interface ObjectPath:" << objPath;
#endif
        // Instantiate our DBus interface and its corresponding signals object
        if(!iface)
            iface = new QDBusInterface(serviceName, objPath, "", QDBusConnection::sessionBus(), this);
        signalsObject = new QServiceMetaObjectDBus(iface, true);

        // Wake up waiting proxy construction code
        QTimer::singleShot(0, this, SIGNAL(pendingRequestFinished()));

    } else {
        // Service side
        Q_ASSERT(d->endPointType == ObjectEndPoint::Service);

        QServicePackage response = p.createResponse();
        InstanceManager* iManager = InstanceManager::instance();

        // Instantiate service object from type register
        service = iManager->createObjectInstance(p.d->entry, d->serviceInstanceId);
        if (!service) {
            qWarning() << "Cannot instantiate service object";
            dispatch->writePackage(response);
            return;
        }

        // Start DBus connection and register proxy service
        if (!QDBusConnection::sessionBus().isConnected()) {
            qWarning() << "Cannot connect to DBus";
        }

        // DBus registration path uses a hash of the service instance ID
        QString serviceName = "com.nokia.qtmobility.sfw." + p.d->entry.serviceName();
        uint hash = qHash(d->serviceInstanceId.toString());
        QString objPath = "/" + p.d->entry.interfaceName() + "/" + p.d->entry.version() + "/" + QString::number(hash);
        objPath.replace(QString("."), QString("/"));

        QServiceMetaObjectDBus *serviceDBus = new QServiceMetaObjectDBus(service);
        QDBusConnection::sessionBus().registerObject(objPath, serviceDBus, QDBusConnection::ExportAllContents);

        QString clientId = p.d->payload.toString();

        int exists = 0;
        for (int i=d->clientList.size()-1; i>=0; i--) {
            // Find right client process
            if (d->clientList[i].clientId == clientId) {
                d->clientList[i].ref++;
                exists = 1;
                break;
            }
        }

        if (!exists) {
            // Add new instance to client ownership list
            ClientInstance c;
            c.clientId = clientId;
            c.entry = p.d->entry;
            c.instanceId = d->serviceInstanceId;
            c.ref = 1;
            d->clientList << c;
        }



#ifdef DEBUG
        qDebug() << "Service Interface ObjectPath:" << objPath;
        
        const QMetaObject *s_meta = service->metaObject();
        qDebug() << "+++++++++++++++++++++SERVICE+++++++++++++++++++++++";
        qDebug() << s_meta->className();
        qDebug() << "METHOD COUNT: " << s_meta->methodCount();
        for (int i=0; i<s_meta->methodCount(); i++) {
            QMetaMethod mm = s_meta->method(i);

            QString type;
            switch(mm.methodType()) {
                case QMetaMethod::Method:
                    type = "Q_INVOKABLE";
                    break;
                case QMetaMethod::Signal:
                    type = "SIGNAL";
                    break;
                case QMetaMethod::Slot:
                    type = "SLOT";
                    break;
                default:
                    break;
            }

            QString returnType = mm.typeName();
            if (returnType == "") returnType = "void";

            qDebug() << "METHOD" << type << ":" << returnType << mm.signature();
        }
        qDebug() << "++++++++++++++++++++++++++++++++++++++++++++++++++++";
        if(!iface)
            iface = new QDBusInterface(serviceName, objPath, "", QDBusConnection::sessionBus(), this);
        const QMetaObject *i_meta = iface->metaObject();
        qDebug() << "++++++++++++++++++++DBUS SERVICE++++++++++++++++++++";
        qDebug() << i_meta->className();
        qDebug() << "METHOD COUNT: " << i_meta->methodCount();
        for (int i=0; i<i_meta->methodCount(); i++) {
            QMetaMethod mm = i_meta->method(i);

            QString type;
            switch(mm.methodType()) {
                case QMetaMethod::Method:
                    type = "Q_INVOKABLE";
                    break;
                case QMetaMethod::Signal:
                    type = "SIGNAL";
                    break;
                case QMetaMethod::Slot:
                    type = "SLOT";
                    break;
                default:
                    break;
            }

            QString returnType = mm.typeName();
            if (returnType == "") returnType = "void";

            qDebug() << "METHOD" << type << ":" << returnType << mm.signature();
        }
        qDebug() << "+++++++++++++++++++++++++++++++++++++++++++++++++++";
#endif
        
        // Get meta object from type register
        const QMetaObject* meta = iManager->metaObject(p.d->entry);
        if (!meta) {
            qDebug() << "Unknown type" << p.d->entry;
            dispatch->writePackage(response);
            return;
        }

        // Serialize meta object
        QByteArray data;
        QDataStream stream( &data, QIODevice::WriteOnly | QIODevice::Append );
        QMetaObjectBuilder builder(meta);
        builder.serialize(stream);
        
        // Send meta object and instance ID to the client for processing 
        d->entry = p.d->entry;
        response.d->instanceId = d->serviceInstanceId;
        response.d->entry = p.d->entry;
        response.d->responseType = QServicePackage::Success;
        response.d->payload = QVariant(data);
        dispatch->writePackage(response);
    }
}

/*!
    Returns the created service instance Id
*/
QString ObjectEndPoint::getInstanceId() const
{
    Q_ASSERT(d->endPointType == ObjectEndPoint::Client);
   
    return d->serviceInstanceId.toString();
}

/*!
    Client side property call that directly accesses properties through the DBus interface.
    Read and reset have special hardcoded DBus methods due to the nature of QtDBus properties
    without an adaptor class incorrectly forwarding the metacall type
*/
QVariant ObjectEndPoint::invokeRemoteProperty(int metaIndex, const QVariant& arg, int /*returnType*/, QMetaObject::Call c )
{
    Q_ASSERT(d->endPointType == ObjectEndPoint::Client);
    
    const QMetaObject *imeta = service->metaObject();
    QMetaProperty property = imeta->property(metaIndex);
    
    if (c == QMetaObject::WriteProperty) {
        // Writing property, direct property DBus call
        if (!iface->setProperty(property.name(), arg)) {
            qWarning() << "Service property write call failed";
        }

    } else if (c == QMetaObject::ResetProperty) {
        // Resetting property, direct special method DBus call
        QVariantList args;
        args << QVariant(property.name());
        QDBusMessage msg = iface->callWithArgumentList(QDBus::Block, "propertyReset", args);
        if (msg.type() == QDBusMessage::InvalidMessage) { 
            qWarning() << "Service property reset call failed";
        }

    } else if (c == QMetaObject::ReadProperty) {
        // Reading property, direct special method DBus call
        QVariantList args;
        args << QVariant(property.name());
        QDBusMessage msg = iface->callWithArgumentList(QDBus::Block, "propertyRead", args);
        if (msg.type() == QDBusMessage::ReplyMessage) { 
            QVariantList retList = msg.arguments();
            return retList[0];
        } else {
            qWarning() << "Service property read call failed" << msg.errorMessage();
        }
    } else {
        qWarning() << "Invalid property call";
    }
 
    return QVariant();
}

/*!
    Client side method call that converts an argument of type to its corresponding value as a
    valid type supported by the QtDBus type system. 
    
    Supports conversion from a QVariant, QList, QMap, QHash, and custom user-defined types. 
*/
QVariant ObjectEndPoint::toDBusVariant(const QByteArray& type, const QVariant& arg)
{
    QVariant dbusVariant = arg;

    int variantType = QVariant::nameToType(type);
    if (variantType == QVariant::UserType) {
        variantType = QMetaType::type(type);

        if (type == "QVariant") {
            // Wrap QVariants in a QDBusVariant
            QDBusVariant replacement(arg);
            dbusVariant = QVariant::fromValue(replacement);
        } else {
            // Wrap custom types in a QDBusVariant of the type name and 
            // a buffer of its variant-wrapped data
            QByteArray buffer;
            QDataStream stream(&buffer, QIODevice::ReadWrite | QIODevice::Append);
            stream << arg;

            QServiceUserTypeDBus customType;
            customType.typeName = type;
            customType.variantBuffer = buffer;

            QDBusVariant replacement(QVariant::fromValue(customType));
            dbusVariant = QVariant::fromValue(replacement);
        }
    }

    return dbusVariant;
}

/*!
    Client side method call that directly accesses the object through the DBus interface.
    All arguments and return types are processed and converted accordingly so that all functions
    satisfy the QtDBus type system.
*/
QVariant ObjectEndPoint::invokeRemote(int metaIndex, const QVariantList& args, int returnType)
{
    QMetaMethod method = service->metaObject()->method(metaIndex);

    Q_ASSERT(d->endPointType == ObjectEndPoint::Client);
   
    // Check is this is a signal relay
    if (method.methodType() == QMetaMethod::Signal) {
        // Convert custom arguments
        QVariantList convertedList;
        QList<QByteArray> params = method.parameterTypes();
        for (int i = 0; i < params.size(); i++) {
            const QByteArray& type = params[i];
            int variantType = QVariant::nameToType(type);
            if (variantType == QVariant::UserType) {
                variantType = QMetaType::type(type);
                
                QDBusVariant dbusVariant = qvariant_cast<QDBusVariant>(args[i]);
                QVariant variant = dbusVariant.variant();

                if (type == "QVariant") {
                    convertedList << variant; 
                } else {
                    QByteArray buffer = variant.toByteArray();
                    QDataStream stream(&buffer, QIODevice::ReadWrite);
                    QVariant *customType = new QVariant(variantType, (const void*)0);
                    QMetaType::load(stream, QMetaType::type("QVariant"), customType);
                    convertedList << *customType;
                }
            } else {
                convertedList << args[i];
            }
        }

        // Signal relay
        const int numArgs = convertedList.size();
        QVarLengthArray<void *, 32> a( numArgs+1 );
        a[0] = 0;

        const QList<QByteArray> pTypes = method.parameterTypes();
        for ( int arg = 0; arg < numArgs; ++arg ) {
            if (pTypes.at(arg) == "QVariant") {
                a[arg+1] = (void *)&( convertedList[arg] );
            } else {
                a[arg+1] = (void *)( convertedList[arg].data() );
            }
        }

        // Activate the service proxy signal call
        QMetaObject::activate(service, metaIndex, a.data());
        return QVariant();
    }

    // Method call so process arguments and convert if not a supported DBus type
    QVariantList convertedList;
    QList<QByteArray> params = method.parameterTypes();
    for (int i = 0; i < params.size(); i++) {
        QVariant converted = toDBusVariant(params[i], args[i]);
        convertedList << converted;
    }

    bool validDBus = false;
    QDBusMessage msg;
  
    // Find the method name and try a direct DBus call
    QString methodName(method.signature());
    methodName.truncate(methodName.indexOf("("));
    
    if (method.methodType() == QMetaMethod::Slot || method.methodType() == QMetaMethod::Method) {
        // Slot or Invokable method
        msg = iface->callWithArgumentList(QDBus::Block, methodName, convertedList);
        if (msg.type() == QDBusMessage::ReplyMessage) { 
            validDBus = true;
        }
    }

    // DBus call should only fail for methods with invalid type definitions 
    if (validDBus) {
	if (returnType == QMetaType::Void) {
	    // Void method call
            return QVariant();
	}
	else {
            // Use DBus message return value
            QVariantList retList = msg.arguments();

            // Process return
            const QByteArray& retType = QByteArray(method.typeName());
            int variantType = QVariant::nameToType(retType);
                if (variantType == QVariant::UserType) {
                    variantType = QMetaType::type(retType);

                    if (retType == "QVariant") {
                        // QVariant return from QDBusVariant wrapper
                        QDBusVariant dbusVariant = qvariant_cast<QDBusVariant>(retList[0]);
                        return dbusVariant.variant();
                    } else {
                        // Custom return type
                        QDBusVariant dbusVariant = qvariant_cast<QDBusVariant>(retList[0]);
                        QVariant convert = dbusVariant.variant();

                        QServiceUserTypeDBus customType = qdbus_cast<QServiceUserTypeDBus>(convert);
                        QByteArray buffer = customType.variantBuffer;
                        QDataStream stream(&buffer, QIODevice::ReadWrite);

                        // Load our buffered variant-wrapped custom return
                        QVariant *customReturn = new QVariant(variantType, (const void*)0);
                        QMetaType::load(stream, QMetaType::type("QVariant"), customReturn);

                        return QVariant(variantType, customReturn->data());
                    }
            } else {
                // Standard return type
                return retList[0];
            }
        }
    } else {  
        qWarning( "%s::%s cannot be called.", service->metaObject()->className(), method.signature());
    }

    return QVariant();
}

/*!
    Client side waits for service side requested 
*/
void ObjectEndPoint::waitForResponse(const QUuid& requestId)
{
    Q_ASSERT(d->endPointType == ObjectEndPoint::Client);

    if (openRequests()->contains(requestId) ) {
        Response* response = openRequests()->value(requestId);
        QEventLoop* loop = new QEventLoop( this );
        connect(this, SIGNAL(pendingRequestFinished()), loop, SLOT(quit())); 

        while(!response->isFinished) {
            loop->exec();
        }
 
        delete loop;
    }
}

#include "moc_objectendpoint_dbus_p.cpp"

QTM_END_NAMESPACE
