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

#ifndef MALICIOUSPLUGINTARGET
#define MALICIOUSPLUGINTARGET contacts_maliciousplugin
#endif

#ifndef MALICIOUSPLUGINNAME
#define MALICIOUSPLUGINNAME maliciousplugin
#endif

#define makestr(x) (#x)
#define makename(x) makestr(x)

#include "maliciousplugin_p.h"
#include "qcontactmanager.h"

MaliciousAsyncManagerEngine::MaliciousAsyncManagerEngine()
        : QContactManagerEngine()
{
}

void MaliciousAsyncManagerEngine::deref()
{
    // does this leak?
}

QString MaliciousAsyncManagerEngine::synthesizedDisplayLabel(const QContact& contact, QContactManager::Error& error) const
{
    Q_UNUSED(contact);
    error = QContactManager::NotSupportedError;
    return QString();
}

QString MaliciousAsyncManagerEngine::managerName() const
{
    return QString(makename(MALICIOUSPLUGINNAME));
}

bool MaliciousAsyncManagerEngine::startRequest(QContactAbstractRequest* req)
{
    // maliciously attempt to update the request with every result type
    updateRequestState(req, QContactAbstractRequest::ActiveState);
    // XXX TODO: call the request-type specific update functions
/*
    //QContactManager::Error errorResult = QContactManager::NoError;
    QList<QContactManager::Error> errorsResult;
    QList<QContactLocalId> idResult;
    QList<QContact> contactResult;
    QList<QContactDetailDefinition> defResult;
    QMap<QString, QContactDetailDefinition> defMapResult;
    QList<QContactRelationship> relResult;

    updateContactLocalIdFetchRequest(req, idResult, errorResult, errorsResult, QContactAbstractRequest::ActiveState, false);
    updateContactFetchRequest(req, contactResult, errorResult, errorsResult, QContactAbstractRequest::ActiveState, false);
    updateDefinitionSaveRequest(req, defResult, errorResult, errorsResult, QContactAbstractRequest::ActiveState);
    updateDefinitionFetchRequest(req, defMapResult, errorResult, errorsResult, QContactAbstractRequest::ActiveState, false);
    updateRequest(req, relResult, errorResult, errorsResult, QContactAbstractRequest::ActiveState, false);
*/
    QContactManagerEngine::startRequest(req);
    return true;
}

bool MaliciousAsyncManagerEngine::cancelRequest(QContactAbstractRequest *req)
{
    updateRequestState(req, QContactAbstractRequest::CanceledState);
    QContactManagerEngine::cancelRequest(req);
    return true;
}


QString MaliciousEngineFactory::managerName() const
{
    return QString(makename(MALICIOUSPLUGINNAME));
}
Q_EXPORT_PLUGIN2(MALICIOUSPLUGINTARGET, MaliciousEngineFactory);

QContactManagerEngine* MaliciousEngineFactory::engine(const QMap<QString, QString>& parameters, QContactManager::Error& error)
{
    Q_UNUSED(parameters);
    error = QContactManager::NoError;
    return &mame;
}
