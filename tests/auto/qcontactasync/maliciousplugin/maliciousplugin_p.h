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

#ifndef QCONTACTMALICIOUSPLUGIN_P_H
#define QCONTACTMALICIOUSPLUGIN_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qcontactmanager.h"
#include "qcontactmanagerengine.h"
#include "qcontactmanagerenginefactory.h"

#include <QObject>
#include <QString>

QTM_USE_NAMESPACE

class MaliciousAsyncManagerEngine : public QContactManagerEngine
{
    Q_OBJECT

public:
    MaliciousAsyncManagerEngine();

    QString synthesizedDisplayLabel(const QContact& contact, QContactManager::Error* error) const;
    QString managerName() const;
    bool startRequest(QContactAbstractRequest* req);
    bool cancelRequest(QContactAbstractRequest *req);

    QMap<QString, QString> managerParameters() const {return QMap<QString, QString>();}
    int managerVersion() const {return 0;}

    QList<QContactLocalId> contactIds(const QContactFilter& filter, const QList<QContactSortOrder>& sort, QContactManager::Error* error) const
    {
        return QContactManagerEngine::contactIds(filter, sort, error);
    }

    QList<QContact> contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sort, const QContactFetchHint& fetch, QContactManager::Error* error) const
    {
        return QContactManagerEngine::contacts(filter, sort, fetch, error);
    }

    QContact contact(const QContactLocalId& id, const QContactFetchHint& fetch, QContactManager::Error* error) const
    {
        return QContactManagerEngine::contact(id, fetch, error);
    }

    bool saveContacts(QList<QContact>* contacts, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error)
    {
        return QContactManagerEngine::saveContacts(contacts, errorMap, error);
    }

    bool removeContacts(const QList<QContactLocalId>& contacts, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error)
    {
        return QContactManagerEngine::removeContacts(contacts, errorMap, error);
    }

    /* "Self" contact id (MyCard) */
    bool setSelfContactId(const QContactLocalId& id, QContactManager::Error* error)
    {
        return QContactManagerEngine::setSelfContactId(id, error);
    }

    QContactLocalId selfContactId(QContactManager::Error* error) const
    {
        return QContactManagerEngine::selfContactId(error);
    }

    /* Relationships between contacts */
    QList<QContactRelationship> relationships(const QString& relType, const QContactId& id, QContactRelationship::Role role, QContactManager::Error* error) const
    {
        return QContactManagerEngine::relationships(relType, id, role, error);
    }

    bool saveRelationships(QList<QContactRelationship>* relationships, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error)
    {
        return QContactManagerEngine::saveRelationships(relationships, errorMap, error);
    }

    bool removeRelationships(const QList<QContactRelationship>& relationships, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error)
    {
        return QContactManagerEngine::removeRelationships(relationships, errorMap, error);
    }

    /* Validation for saving */
    QContact compatibleContact(const QContact& contact, QContactManager::Error* error) const
    {
        return QContactManagerEngine::compatibleContact(contact, error);
    }

    bool validateContact(const QContact& contact, QContactManager::Error* error) const
    {
        return QContactManagerEngine::validateContact(contact, error);
    }

    bool validateDefinition(const QContactDetailDefinition& def, QContactManager::Error* error) const
    {
        return QContactManagerEngine::validateDefinition(def, error);
    }

    /* Definitions - Accessors and Mutators */
    QMap<QString, QContactDetailDefinition> detailDefinitions(const QString& contactType, QContactManager::Error* error) const
    {
        return QContactManagerEngine::detailDefinitions(contactType, error);
    }

    QContactDetailDefinition detailDefinition(const QString& definitionId, const QString& contactType, QContactManager::Error* error) const
    {
        return QContactManagerEngine::detailDefinition(definitionId, contactType, error);
    }

    bool saveDetailDefinition(const QContactDetailDefinition& def, const QString& contactType, QContactManager::Error* error)
    {
        return QContactManagerEngine::saveDetailDefinition(def, contactType, error);
    }

    bool removeDetailDefinition(const QString& defName, const QString& contactType, QContactManager::Error* error)
    {
        return QContactManagerEngine::removeDetailDefinition(defName, contactType, error);
    }

    /* Asynchronous Request Support */
    void requestDestroyed(QContactAbstractRequest* req) {QContactManagerEngine::requestDestroyed(req);}
    bool waitForRequestFinished(QContactAbstractRequest* req, int msecs) {return QContactManagerEngine::waitForRequestFinished(req, msecs);}

    /* Capabilities reporting */
    bool hasFeature(QContactManager::ManagerFeature feat, const QString& contactType) const
    {
        return QContactManagerEngine::hasFeature(feat, contactType);
    }

    bool isRelationshipTypeSupported(const QString& relType, const QString& ctype) const
    {
        return QContactManagerEngine::isRelationshipTypeSupported(relType, ctype);
    }

    bool isFilterSupported(const QContactFilter& fil) const
    {
        return QContactManagerEngine::isFilterSupported(fil);
    }

    QList<QVariant::Type> supportedDataTypes() const
    {
        return QContactManagerEngine::supportedDataTypes();
    }
    QStringList supportedContactTypes() const
    {
        return QContactManagerEngine::supportedContactTypes();
    }

};

class  Q_DECL_EXPORT MaliciousEngineFactory : public QObject, public QContactManagerEngineFactory
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QContactManagerEngineFactory)

    public:
        QContactManagerEngine* engine(const QMap<QString, QString>& parameters, QContactManager::Error* error);
        QString managerName() const;

    private:
        MaliciousAsyncManagerEngine mame;
};

#endif

