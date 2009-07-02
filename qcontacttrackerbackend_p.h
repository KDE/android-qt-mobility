/* * This file is part of qtcontacts-tracker *
 * Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 * Contact: Aleksandar Stojiljkovic <aleksandar.stojiljkovic@nokia.com>
 * This software, including documentation, is protected by copyright controlled by
 * Nokia Corporation. All rights are reserved. Copying, including reproducing, storing,
 * adapting or translating, any or all of this material requires the prior written consent
 * of Nokia Corporation. This material also contains confidential information which may
 * not be disclosed to others without the prior written consent of Nokia.
 */

#ifndef QCONTACTTRACKERBACKEND_P_H
#define QCONTACTTRACKERBACKEND_P_H

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

#include <QSharedData>

#include "qcontact.h"
#include "qcontactname.h"
#include "qcontactphonenumber.h"
#include "qcontactmanager.h"
#include "qcontactmanager_p.h"

class QContactTrackerEngineData : public QSharedData
{
public:
    QContactTrackerEngineData()
        : QSharedData(), m_refCount(QAtomicInt(1))
    {
    }

    QContactTrackerEngineData(const QContactTrackerEngineData& other)
        : QSharedData(other), m_refCount(QAtomicInt(1))
    {
    }

    ~QContactTrackerEngineData() {}

    QAtomicInt m_refCount;

    static QMap<QString, QContactDetailDefinition> m_definitions;
    static QUniqueId m_lastUsedId;
};

class QTCONTACTS_EXPORT QContactTrackerEngine : public QContactManagerEngine
{
public:
    QContactTrackerEngine(const QMap<QString, QString>& parameters);
    QContactTrackerEngine(const QContactTrackerEngine& other);
    ~QContactTrackerEngine();
    QContactTrackerEngine& operator=(const QContactTrackerEngine& other);
    QContactManagerEngine* clone();
    void deref();

    /* Contacts - Accessors and Mutators */
    QList<QUniqueId> contacts() const;
    QList<QUniqueId> contactsWithDetail(const QString& definitionId, const QVariant& value) const;
    QContact contact(const QUniqueId& contactId) const;
    bool saveContact(QContact* contact, bool batch = false);
    bool removeContact(const QUniqueId& contactId, bool batch = false);
    QList<QContactManager::Error> saveContacts(QList<QContact>* contacts);
    QList<QContactManager::Error> removeContacts(QList<QUniqueId>* contactIds);

    /* Groups - Accessors and Mutators */
    QList<QUniqueId> groups() const;
    QContactGroup group(const QUniqueId& groupId) const;
    bool saveGroup(QContactGroup* group);
    bool removeGroup(const QUniqueId& groupId);

    /* Definitions - Accessors and Mutators */
    QStringList detailDefinitions() const;
    QContactDetailDefinition detailDefinition(const QString& definitionId) const;
    bool saveDetailDefinition(const QContactDetailDefinition& def);
    bool removeDetailDefinition(const QContactDetailDefinition& def);

    /* Capabilities reporting */
    QStringList capabilities() const;
    QStringList fastFilterableDefinitions() const;
    QList<QVariant::Type> supportedDataTypes() const;

    /* Error reporting */
    QContactManager::Error error() const;

private:
    QContactManager::Error m_error;

private:
    QSharedDataPointer<QContactTrackerEngineData> d;
};

class Q_DECL_EXPORT ContactTrackerFactory : public QObject, public QContactManagerEngineFactory
{
    Q_OBJECT
    Q_INTERFACES(QContactManagerEngineFactory)
    public:
        QContactManagerEngine* engine(const QMap<QString, QString>& parameters, QContactManager::Error&);
        QString managerId() const;
};

#endif
