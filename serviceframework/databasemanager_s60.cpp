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

#include "databasemanager_s60_p.h"
#include "clientservercommon.h"
#include <qserviceinterfacedescriptor_p.h>
#include <qserviceinterfacedescriptor.h>
#include <qservicefilter.h>
#include <QProcess>
#include <QDebug>
#include <s32mem.h>


QT_BEGIN_NAMESPACE

/*
    \class DatabaseManager
    The database manager is responsible for receiving queries about
    services and managing user and system scope databases in order to
    respond to those queries.

    It provides operations for
    - registering and unregistering services
    - querying for services and interfaces
    - setting and getting default interface implementations

    and provides notifications by emitting signals for added
    or removed services.

    Implementation note:
    When one of the above operations is first invoked a connection with the
    appropriate database(s) is opened.  This connection remains
    open until the DatabaseManager is destroyed.

    If the system scope database cannot be opened when performing
    user scope operations.  The operations are carried out as per normal
    but only acting on the user scope database.  Each operation invokation
    will try to open a connection with the system scope database.

    Terminology note:
    When referring to user scope regarding operations, it generally
    means access to both the user and system databases with the
    data from both combined into a single dataset.
    When referring to a user scope database it means the
    user database only.
*/

/*
   Constructor
*/
DatabaseManager::DatabaseManager()
{
    iSession.Connect();
    iDatabaseManagerSignalMonitor = new DatabaseManagerSignalMonitor(*this, iSession);
}

/*
   Destructor
*/
DatabaseManager::~DatabaseManager()
{
    delete iDatabaseManagerSignalMonitor;
    iSession.Close();
}

/*
    Adds the details \a  service into the service database corresponding to
    \a scope.

    Returns true if the operation succeeded and false otherwise.
    The last error is set when this function is called.
*/
bool DatabaseManager::registerService(ServiceMetaDataResults &service, DbScope scope)
{
    return iSession.RegisterService(service, static_cast<RDatabaseManagerSession::DbScope>(scope));
}

/*
    Removes the details of \serviceName from the database corresponding to \a
    scope.

    Returns true if the operation succeeded, false otherwise.
    The last error is set when this function is called.
*/
bool DatabaseManager::unregisterService(const QString &serviceName, DbScope scope)
{
    return iSession.UnregisterService(serviceName, scope);
}

/*
    Retrieves a list of interface descriptors that fulfill the constraints specified
    by \a filter at a given \a scope.

    The last error is set when this function is called.
*/
QList<QServiceInterfaceDescriptor>  DatabaseManager::getInterfaces(const QServiceFilter &filter, DbScope scope)
{
    return iSession.Interfaces(filter, static_cast<RDatabaseManagerSession::DbScope>(scope));
}


/*
    Retrieves a list of the names of services that provide the interface
    specified by \a interfaceName.

    The last error is set when this function is called.
*/
QStringList DatabaseManager::getServiceNames(const QString &interfaceName, DbScope scope)
{
    return iSession.ServiceNames(interfaceName, static_cast<RDatabaseManagerSession::DbScope>(scope));
}

/*
    Returns the default interface implementation descriptor for a given
    \a interfaceName and \a scope.

    The last error is set when this function is called.
*/
QServiceInterfaceDescriptor DatabaseManager::interfaceDefault(const QString &interfaceName, DbScope scope)
{
    return iSession.InterfaceDefault(interfaceName, static_cast<RDatabaseManagerSession::DbScope>(scope));
}

/*
    Sets the default interface implemenation for \a interfaceName to the matching
    interface implementation provided by \a service.

    If \a service provides more than one interface implementation, the newest
    version of the interface is set as the default.

    Returns true if the operation was succeeded, false otherwise
    The last error is set when this function is called.
*/
bool DatabaseManager::setInterfaceDefault(const QString &serviceName, const
        QString &interfaceName, DbScope scope) {
        return iSession.SetInterfaceDefault(serviceName, interfaceName, static_cast<RDatabaseManagerSession::DbScope>(scope));
}

/*
    Sets the interface implementation specified by \a descriptor to be the default
    implementation for the particular interface specified in the descriptor.

    Returns true if the operation succeeded, false otherwise.
    The last error is set when this function is called.
*/
bool DatabaseManager::setInterfaceDefault(const QServiceInterfaceDescriptor &descriptor, DbScope scope)
{
    return iSession.SetInterfaceDefault(descriptor, static_cast<RDatabaseManagerSession::DbScope>(scope));
}

/*
    Sets whether change notifications for added and removed services are
    \a enabled or not at a given \a scope.
*/
void DatabaseManager::setChangeNotificationsEnabled(DbScope scope, bool enabled)
{
    iSession.SetChangeNotificationsEnabled(scope, enabled);
}

DatabaseManagerSignalMonitor::DatabaseManagerSignalMonitor(
    DatabaseManager& databaseManager, RDatabaseManagerSession& databaseManagerSession) :
    CActive(EPriorityNormal),
    iDatabaseManager(databaseManager), iDatabaseManagerSession(databaseManagerSession)
{
    CActiveScheduler::Add(this);
    issueNotifyServiceSignal();
}

DatabaseManagerSignalMonitor::~DatabaseManagerSignalMonitor()
{
    Cancel();
}

void DatabaseManagerSignalMonitor::issueNotifyServiceSignal()
{
    iDatabaseManagerSession.NotifyServiceSignal(iStatus);
    SetActive();
}

DBError DatabaseManager::lastError()
{
    return iSession.LastError();
}

void DatabaseManagerSignalMonitor::DoCancel()
{
    iDatabaseManagerSession.CancelNotifyServiceSignal();
}

void DatabaseManagerSignalMonitor::RunL()
{
    switch (iStatus.Int())
    {
        case ENotifySignalComplete:
        {
            QString serviceName = QString::fromUtf16(iDatabaseManagerSession.iServiceName.Ptr(), iDatabaseManagerSession.iServiceName.Length());
            DatabaseManager::DbScope scope = (DatabaseManager::DbScope)iDatabaseManagerSession.iScope();
            
            if ((DatabaseManager::State)iDatabaseManagerSession.iState() == DatabaseManager::EAdded)
            {
                emit iDatabaseManager.serviceAdded(serviceName, scope);
            }
            else if ((DatabaseManager::State)iDatabaseManagerSession.iState() == DatabaseManager::ERemoved) 
            {
                emit iDatabaseManager.serviceRemoved(serviceName, scope);
            }
            issueNotifyServiceSignal();
            break;
        }
        default:
        {
            
        }
        break;
    }
}


RDatabaseManagerSession::RDatabaseManagerSession()
    : RSessionBase()
    {   
    }

TVersion RDatabaseManagerSession::Version() const
    {
    return TVersion(KServerMajorVersionNumber, KServerMinorVersionNumber, KServerBuildVersionNumber);
    }

TInt RDatabaseManagerSession::Connect()
    {
    TInt err = StartServer();
    if (err == KErrNone)
    {
        err = CreateSession(KDatabaseManagerServerName, Version()); //Default message slots
    }
    return err;
    }

TInt RDatabaseManagerSession::StartServer()
    {
    TInt ret = KErrNone;
    TFindServer findServer(KDatabaseManagerServerName);
    TFullName name;
    if (findServer.Next(name) != KErrNone)
    {
#ifdef __WINS__
        iServerThread = new CDatabaseManagerServerThread();
        iServerThread->start();
        iServerThread->wait(1);
#else
        TRequestStatus status;
        RProcess dbServer;
        ret = dbServer.Create(KDatabaseManagerServerName, KNullDesC);
        if(ret != KErrNone)
            {
            return ret;
            }
        dbServer.Rendezvous(status);
        if(status != KRequestPending)
            {
            dbServer.Kill(KErrNone);
            dbServer.Close();
            return KErrGeneral;
            }
        else
            {
            dbServer.Resume();
            }

        User::WaitForRequest(status);
        if(status != KErrNone)
            {
            dbServer.Close();
            return status.Int();
            }
        dbServer.Close();
#endif
    }
    return ret;
    }

void RDatabaseManagerSession::Close()
    {
#ifdef __WINS__
    iServerThread->quit();
    iServerThread->wait();
    delete iServerThread;
    iServerThread = NULL;
#endif
    RSessionBase::Close();
    }

bool RDatabaseManagerSession::RegisterService(ServiceMetaDataResults& aService, TInt aScope)
    {
    QByteArray serviceByteArray;
    QDataStream in(&serviceByteArray, QIODevice::WriteOnly);
    in.setVersion(QDataStream::Qt_4_5);
    in << aService;
    TPtrC8 ptr8((TUint8*)(serviceByteArray.constData()), serviceByteArray.size());
    TIpcArgs args(&ptr8, aScope, &iError);
    SendReceive(ERegisterServiceRequest, args);

    return (iError() == DBError::NoError);
    }

bool RDatabaseManagerSession::UnregisterService(const QString& aServiceName, TInt aScope)
    {  
    TPtrC serviceNamePtr(reinterpret_cast<const TUint16*>(aServiceName.utf16()));
    TIpcArgs args(&serviceNamePtr, aScope, &iError);
    SendReceive(EUnregisterServiceRequest, args);
    
    return (iError() == DBError::NoError);
    }

QList<QServiceInterfaceDescriptor> RDatabaseManagerSession::Interfaces(const QServiceFilter& aFilter, TInt aScope)
    {
    QByteArray filterByteArray;
    QDataStream in(&filterByteArray, QIODevice::WriteOnly);
    in.setVersion(QDataStream::Qt_4_5);
    in << aFilter;   
    TPtrC8 ptr8((TUint8*)(filterByteArray.constData()), filterByteArray.size());
    TPckgBuf<TInt> lengthPckg(0);
    TIpcArgs args(&ptr8, aScope, &lengthPckg, &iError);
    SendReceive(EGetInterfacesSizeRequest, args);

    HBufC8* descriptorListBuf = HBufC8::New(lengthPckg());
    TPtr8 ptrToBuf(descriptorListBuf->Des());
    TIpcArgs args2(&ptrToBuf);
    SendReceive(EGetInterfacesRequest, args2);

    QByteArray descriptorListByteArray((const char*)ptrToBuf.Ptr(), ptrToBuf.Length());
    QDataStream out(descriptorListByteArray);
    QList<QServiceInterfaceDescriptor> descriptorList;
    out >> descriptorList;

    delete descriptorListBuf;
    
    return descriptorList;
    }

QStringList RDatabaseManagerSession::ServiceNames(const QString& aInterfaceName, TInt aScope)
    {
    TPtrC interfaceNamePtr(reinterpret_cast<const TUint16*>(aInterfaceName.utf16()));
    HBufC* interfaceNamebuf = HBufC::New(interfaceNamePtr.Length());
    interfaceNamebuf->Des().Copy(interfaceNamePtr);
    TPckgBuf<TInt> lengthPckg(0);
    TIpcArgs args(interfaceNamebuf, aScope, &lengthPckg, &iError);
    SendReceive(EGetServiceNamesSizeRequest, args);
    
    HBufC8* serviceNamesBuf = HBufC8::New(lengthPckg());
    TPtr8 ptrToBuf(serviceNamesBuf->Des());
    TIpcArgs args2(&ptrToBuf);
    SendReceive(EGetServiceNamesRequest, args2);
    
    QByteArray nameListByteArray((const char*)ptrToBuf.Ptr(), ptrToBuf.Length());
    QDataStream out(nameListByteArray);
    QStringList nameList;
    out >> nameList;

    delete interfaceNamebuf;
    delete serviceNamesBuf;
    
    return nameList;
    }

QServiceInterfaceDescriptor RDatabaseManagerSession::InterfaceDefault(const QString& aInterfaceName, TInt aScope)
    {
    TPtrC interfaceNamePtr(reinterpret_cast<const TUint16*>(aInterfaceName.utf16()));
    HBufC* interfaceNameBuf = HBufC::New(interfaceNamePtr.Length());
    interfaceNameBuf->Des().Copy(interfaceNamePtr);
    TPckgBuf<TInt> lengthPckg(0);
    TIpcArgs args(interfaceNameBuf, aScope, &lengthPckg, &iError);
    SendReceive(EInterfaceDefaultSizeRequest, args);
    
    HBufC8* interfaceDescriptorBuf = HBufC8::New(lengthPckg());
    TPtr8 ptrToBuf(interfaceDescriptorBuf->Des());    
    TIpcArgs args2(&ptrToBuf);
    SendReceive(EInterfaceDefaultRequest, args2);

    QByteArray interfaceDescriptorByteArray((const char*)interfaceDescriptorBuf->Ptr(), interfaceDescriptorBuf->Length());
    QDataStream out(interfaceDescriptorByteArray);
    QServiceInterfaceDescriptor interfaceDescriptor;
    out >> interfaceDescriptor;
    
    delete interfaceNameBuf;
    delete interfaceDescriptorBuf;
    
    return interfaceDescriptor;
    }

bool RDatabaseManagerSession::SetInterfaceDefault(const QString &aServiceName, const QString &aInterfaceName, TInt aScope)
    {
    TPtrC serviceNamePtr(reinterpret_cast<const TUint16*>(aServiceName.utf16()));
    TPtrC interfaceNamePtr(reinterpret_cast<const TUint16*>(aInterfaceName.utf16()));    
    TIpcArgs args(&serviceNamePtr, &interfaceNamePtr, aScope, &iError);
    SendReceive(ESetInterfaceDefault, args);
    
    return (iError() == DBError::NoError);
    }

bool RDatabaseManagerSession::SetInterfaceDefault(const QServiceInterfaceDescriptor &aInterface, TInt aScope)
    {
    QByteArray interfaceByteArray;
    QDataStream in(&interfaceByteArray, QIODevice::WriteOnly);
    in.setVersion(QDataStream::Qt_4_5);
    in << aInterface;
    TPtrC8 ptr8((TUint8 *)(interfaceByteArray.constData()), interfaceByteArray.size());
    TIpcArgs args(&ptr8, aScope, &iError);
    SendReceive(ESetInterfaceDefault2, args);

    return (iError() == DBError::NoError);
    }

DBError RDatabaseManagerSession::LastError()
    {
    DBError error;
    error.setError((DBError::ErrorCode)iError());
    return error;
    }

void RDatabaseManagerSession::SetChangeNotificationsEnabled(TInt aScope, bool aEnabled)
    {
    TIpcArgs args(aScope, (aEnabled ? 1 : 0), &iError);
    SendReceive(ESetChangeNotificationsEnabledRequest, args);
    }

void RDatabaseManagerSession::NotifyServiceSignal(TRequestStatus& aStatus)
    {
    iArgs.Set(0, &iServiceName);
    iArgs.Set(1, &iScope);
    iArgs.Set(2, &iState);
    iArgs.Set(3, &iError);
    SendReceive(ENotifyServiceSignalRequest, iArgs, aStatus);
    }

void RDatabaseManagerSession::CancelNotifyServiceSignal() const
    {
    SendReceive(ECancelNotifyServiceSignalRequest, TIpcArgs(NULL));
    }



#ifdef __WINS__
    #include "databasemanagerserver.h"

    CDatabaseManagerServerThread::CDatabaseManagerServerThread()
        {
        }

    void CDatabaseManagerServerThread::run()
        {
        CDatabaseManagerServer *dbManagerServer = new CDatabaseManagerServer();
        __ASSERT_ALWAYS(dbManagerServer != NULL, CDatabaseManagerServer::PanicServer(ESrvCreateServer));

        TInt err = dbManagerServer->Start(KDatabaseManagerServerName);
        if (err != KErrNone)
        {
            CDatabaseManagerServer::PanicServer(ESvrStartServer);
        }
        RThread::Rendezvous(KErrNone);

        exec();
        delete dbManagerServer;
        }
#endif

QT_END_NAMESPACE
