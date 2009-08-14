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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MALICIOUSPLUGINTARGET
#define MALICIOUSPLUGINTARGET contacts_maliciousplugin
#endif

#define makestr(x) (#x)
#define makename(x) makestr(x)

#include "qcontact_p.h"
#include "qcontactgroup_p.h"
#include "qcontactmanager.h"
#include "qcontactmanager_p.h"

class MaliciousAsyncManagerEngine : public QContactManagerEngine
{
public:
    MaliciousAsyncManagerEngine() : QContactManagerEngine() {}
    void deref() {} // leak?

    QString synthesiseDisplayLabel(const QContact& contact, QContactManager::Error& error) const
    {
        Q_UNUSED(contact);
        error = QContactManager::NotSupportedError;
        return QString();
    }

    bool startRequest(QContactAbstractRequest* req)
    {
        QContactManager::Error errorResult = QContactManager::NoError;
        QList<QContactManager::Error> errorsResult;
        QList<QUniqueId> idResult;
        QList<QContact> contactResult;
        QList<QContactGroup> groupResult;
        QList<QContactDetailDefinition> defResult;
        QMap<QString, QContactDetailDefinition> defMapResult;

        // maliciously attempt to update the request with every result type
        updateRequestStatus(req, errorResult, errorsResult, QContactAbstractRequest::Active, false);
        updateRequest(req, idResult, errorResult, errorsResult, QContactAbstractRequest::Active, false);
        updateRequest(req, contactResult, errorResult, errorsResult, QContactAbstractRequest::Active, false);
        updateRequest(req, groupResult, errorResult, errorsResult, QContactAbstractRequest::Active, false);
        updateRequest(req, defResult, errorResult, errorsResult, QContactAbstractRequest::Active);
        updateRequest(req, defMapResult, errorResult, errorsResult, QContactAbstractRequest::Active, false);

        return true;
    }

    bool cancelRequest(QContactAbstractRequest *req)
    {
        QContactManager::Error errorResult = QContactManager::NoError;
        QList<QContactManager::Error> errorsResult;
        updateRequestStatus(req, errorResult, errorsResult, QContactAbstractRequest::Cancelled, false);
        return true;
    }
};

class MaliciousEngineFactory : public QObject, public QContactManagerEngineFactory
{
    Q_OBJECT
    Q_INTERFACES(QContactManagerEngineFactory)
    public:
        QContactManagerEngine* engine(const QMap<QString, QString>& parameters, QContactManager::Error& error);
        QString managerName() const;

    private:
        MaliciousAsyncManagerEngine mame;
};

QString MaliciousEngineFactory::managerName() const
{
#ifdef MALICIOUSPLUGINNAME
    return QString(makename(MALICIOUSPLUGINNAME));
#else
    return QString("maliciousplugin");
#endif
}
Q_EXPORT_PLUGIN2(contacts_maliciousplugin, MaliciousEngineFactory);

QContactManagerEngine* MaliciousEngineFactory::engine(const QMap<QString, QString>& parameters, QContactManager::Error& error)
{
    Q_UNUSED(parameters);
    error = QContactManager::NoError;
    return &mame;
}

#include "maliciousplugin.moc"
