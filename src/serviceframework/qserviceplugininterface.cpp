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

#include "qserviceplugininterface.h"

QTM_BEGIN_NAMESPACE
/*!
    \class QServicePluginInterface
    \ingroup servicefw
    \brief The QServicePluginInterface class defines the interface
    that every plug-in based service must implement.
*/

/*!
    \internal
*/
QServicePluginInterface::QServicePluginInterface() 
{
}

/*!
    \internal
*/
QServicePluginInterface::~QServicePluginInterface() 
{
}
    
/*!
    \fn QObject* QServicePluginInterface::createInstance(const QServiceInterfaceDescriptor& descriptor, QServiceContext* context,
                                    QAbstractSecuritySession* securitySession)

    Creates a new instance of the service specified by \a descriptor. The service
    may use the given \a context and \a securitySession. \a context and \a securitySession object are owned 
    by the client of the service.

    This function returns a null pointer if the plug-in doesn't
    support the given \a descriptor.
*/

/*!
    \fn bool QServicePluginInterface::installService()

    This function is called by QServiceManager as part of the service registration process. It can be
    used to initialize the environment or the creation of external settings files which may be required
    during the execution of the service.
    
    The default implementation for this function does nothing.

    \sa QServiceManager::addService()
*/
void QServicePluginInterface::installService() 
{
}

/*!
    \fn bool QServicePluginInterface::uninstallService() 

    This function is called bu QServiceManager as part of the deregistration process for services. This
    gives the service the possibility to perform cleanup operations such as the removal of setting files
    on the hard drive.

    The default implementation for this function does nothing.

    \sa QServiceManager::removeService()
*/

void QServicePluginInterface::uninstallService() 
{
}

QTM_END_NAMESPACE
