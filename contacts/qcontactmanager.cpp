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
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcontactmanager.h"

#include "qcontact_p.h"
#include "qcontactfilter.h"
#include "qcontactdetaildefinition.h"
#include "qcontactmanager_p.h"
#include "qcontactmanagerinfo.h"

#include <QSharedData>
#include <QPair>
#include <QSet>

/*!
 * \class QContactManager
 *
 * This class provides adding, updating and removal of contacts.
 * It also provides definitions for fields that can be found in contacts.
 */

/*!
 * \fn QContactManager::dataChanged()
 * signal is emitted by the manager if its internal state changes, and it is unable to determine the changes
 * which occurred, or if the manager considers the changes to be radical enough to require clients to reload all data.
 * If this signal is emitted, no other signals will be emitted for the associated changes.
 */

/*!
 * \fn QContactManager::contactsAdded(const QList<QUniqueId>& contactIds)
 * This signal is emitted at some point once the contacts identified by \a contactIds have been added to a datastore managed by this manager.
 * This signal must not be emitted if the dataChanged() signal was previously emitted for these changes.
 */

/*!
 * \fn QContactManager::contactsChanged(const QList<QUniqueId>& contactIds)
 * This signal is emitted at some point once the contacts identified by \a contactIds have been modified in a datastore managed by this manager.
 * This signal must not be emitted if the dataChanged() signal was previously emitted for these changes.
 */

/*!
 * \fn QContactManager::contactsRemoved(const QList<QUniqueId>& contactIds)
 * This signal is emitted at some point once the contacts identified by \a contactIds have been removed from a datastore managed by this manager.
 * This signal must not be emitted if the dataChanged() signal was previously emitted for these changes.
 */


/*!
    Returns a list of available manager ids that can be used when constructing
    a QContactManager.  If an empty id is specified to the constructor, the
    first value in this list will be used instead.
  */
QStringList QContactManager::availableManagers()
{
    QStringList ret;
    ret << QLatin1String("memory") << QLatin1String("invalid");
    QContactManagerData::loadFactories();
    ret.append(QContactManagerData::m_engines.keys());
    return ret;
}

/*! Splits the given \a uri into the manager, store, and parameters that it describes, and places the information into the memory addressed by \a pManagerId and \a pParams respectively.  Returns true if \a uri could be split successfully, otherwise returns false */
bool QContactManager::splitUri(const QString& uri, QString* pManagerId, QMap<QString, QString>* pParams)
{
    // Format: qtcontacts:<managerid>:<key>=<value>&<key>=<value>
    // 1) parameters are currently a qstringlist.. should they be a map?
    // 2) is the uri going to be escaped?  my guess would be "probably not"
    // 3) hence, do we assume that the prefix, managerid and storeid cannot contain `:'
    // 4) similarly, that neither keys nor values can contain `=' or `&'

    QStringList colonSplit = uri.split(QLatin1Char(':'));
    QString prefix = colonSplit.value(0);

    if (prefix != QLatin1String("qtcontacts"))
        return false;

    QString managerName = colonSplit.value(1);

    if (managerName.trimmed().isEmpty())
        return false;

    QString firstParts = prefix + QLatin1Char(':') + managerName + QLatin1Char(':');
    QString paramString = uri.mid(firstParts.length());

    QMap<QString, QString> outParams;

    // Now we have to decode each parameter
    if (!paramString.isEmpty()) {
        QStringList params = paramString.split(QRegExp(QLatin1String("&(?!(amp;|equ;))")), QString::KeepEmptyParts);
        // If we have an empty string for paramstring, we get one entry in params,
        // so skip that case.
        for(int i = 0; i < params.count(); i++) {
            /* This should be something like "foo&amp;bar&equ;=grob&amp;" */
            QStringList paramChunk = params.value(i).split(QLatin1String("="), QString::KeepEmptyParts);

            if (paramChunk.count() != 2)
                return false;

            QString arg = paramChunk.value(0);
            QString param = paramChunk.value(1);
            arg.replace(QLatin1String("&equ;"), QLatin1String("="));
            arg.replace(QLatin1String("&amp;"), QLatin1String("&"));
            param.replace(QLatin1String("&equ;"), QLatin1String("="));
            param.replace(QLatin1String("&amp;"), QLatin1String("&"));
            if (arg.isEmpty())
                return false;
            outParams.insert(arg, param);
        }
    }

    if (pParams)
        *pParams = outParams;
    if (pManagerId)
        *pManagerId = managerName;
    return true;
}

/*! Returns a URI that completely describes a manager implementation, datastore, and the parameters with which to instantiate the manager, from the given \a managerName and \a params */
QString QContactManager::buildUri(const QString& managerName, const QMap<QString, QString>& params)
{
    QString ret(QLatin1String("qtcontacts:%1:%2"));
    // we have to escape each param
    QStringList escapedParams;
    QStringList keys = params.keys();
    for (int i=0; i < keys.size(); i++) {
        QString key = keys.at(i);
        QString arg = params.value(key);
        arg = arg.replace(QLatin1Char('&'), QLatin1String("&amp;"));
        arg = arg.replace(QLatin1Char('='), QLatin1String("&equ;"));
        key = key.replace(QLatin1Char('&'), QLatin1String("&amp;"));
        key = key.replace(QLatin1Char('='), QLatin1String("&equ;"));
        key = key + QLatin1Char('=') + arg;
        escapedParams.append(key);
    }
    return ret.arg(managerName, escapedParams.join(QLatin1String("&")));
}

/*!
 * Constructs a QContactManager whose implementation, store and parameters are specified in the given \a storeUri,
 * and whose parent object is \a parent.
 */
QContactManager* QContactManager::fromUri(const QString& storeUri, QObject* parent)
{
    if (storeUri.isEmpty()) {
        return new QContactManager(QString(), QMap<QString, QString>(), parent);
    } else {
        QString id;
        QMap<QString, QString> parameters;
        if (splitUri(storeUri, &id, &parameters)) {
            return new QContactManager(id, parameters, parent);
        } else {
            // invalid
            return new QContactManager(QLatin1String("invalid"), QMap<QString, QString>(), parent);
        }
    }
}

/*!
 * Constructs a QContactManager whose implementation is identified by \a managerName with the given \a parameters.
 *
 * The \a parent QObject will be used as the parent of this QContactManager.
 *
 * If an empty \a managerName is specified, the default implementation for the platform will
 * be used.
 */
QContactManager::QContactManager(const QString& managerName, const QMap<QString, QString>& parameters, QObject* parent)
    : QObject(parent),
    d(new QContactManagerData)
{
    d->createEngine(managerName, parameters);
    connect(d->m_engine, SIGNAL(dataChanged()), this, SIGNAL(dataChanged()));
    connect(d->m_engine, SIGNAL(contactsAdded(QList<QUniqueId>)), this, SIGNAL(contactsAdded(QList<QUniqueId>)));
    connect(d->m_engine, SIGNAL(contactsChanged(QList<QUniqueId>)), this, SIGNAL(contactsChanged(QList<QUniqueId>)));
    connect(d->m_engine, SIGNAL(contactsRemoved(QList<QUniqueId>)), this, SIGNAL(contactsRemoved(QList<QUniqueId>)));
}

/*! Frees the memory used by the QContactManager */
QContactManager::~QContactManager()
{
    delete d->m_info;
    delete d;
}

/*!
 * \enum QContactManager::Error
 *
 * This enum specifies an error that occurred during the most recent operation:
 *
 * \value NoError The most recent operation was successful
 * \value DoesNotExistError The most recent operation failed because the requested contact or detail definition does not exist
 * \value AlreadyExistsError The most recent operation failed because the specified contact or detail definition already exists
 * \value InvalidDetailError The most recent operation failed because the specified contact contains details which do not conform to their definition
 * \value InvalidRelationshipError The most recent operation failed because the specified relationship is circular or references an invalid local contact
 * \value LockedError The most recent operation failed because the datastore specified is currently locked
 * \value DetailAccessError The most recent operation failed because a detail was modified or removed and its access method does not allow that
 * \value PermissionsError The most recent operation failed because the caller does not have permission to perform the operation
 * \value OutOfMemoryError The most recent operation failed due to running out of memory
 * \value NotSupportedError The most recent operation failed because the requested operation is not supported in the specified store
 * \value BadArgumentError The most recent operation failed because one or more of the parameters to the operation were invalid
 * \value UnspecifiedError The most recent operation failed for an undocumented reason
 */

/*! Return the error code of the most recent operation */
QContactManager::Error QContactManager::error() const
{
    return d->m_error;
}

/*! Return the list of added contact ids, sorted according to the given list of \a sortOrders */
QList<QUniqueId> QContactManager::contacts(const QList<QContactSortOrder>& sortOrders) const
{
    return d->m_engine->contacts(sortOrders, d->m_error);
}

/*!
 * Returns a list of contact ids that match the given \a filter, sorted according to the given list of \a sortOrders.
 *
 * Depending on the backend, this filtering operation may involve retrieving all the contacts.
 */
QList<QUniqueId> QContactManager::contacts(const QContactFilter &filter, const QList<QContactSortOrder>& sortOrders) const
{
    return d->m_engine->contacts(filter, sortOrders, d->m_error);
}

/*! 
 * Returns a list of ids of contacts of the given \a contactType, sorted according to the given list of \a sortOrders.
 */
QList<QUniqueId> QContactManager::contacts(const QString& contactType, const QList<QContactSortOrder>& sortOrders) const
{
    return d->m_engine->contacts(contactType, sortOrders, d->m_error);
}

/*! Returns the contact in the database identified by \a contactId */
QContact QContactManager::contact(const QUniqueId& contactId) const
{
    return d->m_engine->contact(contactId, d->m_error);
}

/*!
 * Add the given \a contact to the database if \a contact has an
 * id of zero, or update the existing contact in the database if \a contact
 * has a non-zero id and currently exists in the database, or fails
 * if the \a contact has a non-zero id which does not exist in the database.
 * Returns false on failure, or true on
 * success.  On successful save of a contact with an id of zero, its
 * id will be set to a new, valid id.
 *
 * If the \a contact contains one or more details whose definitions have
 * not yet been saved with the manager, the operation will fail and calling
 * error() will return \c QContactManager::UnsupportedError.
 *
 * If the id of the \a contact is non-zero but does not exist in the
 * manager, the operation will fail and calling error() will return
 * \c QContactManager::DoesNotExistError.
 */
bool QContactManager::saveContact(QContact* contact)
{
    return d->m_engine->saveContact(contact, d->m_error);
}

/*!
 * Remove the contact identified by \a contactId from the database.
 * Returns true if the contact was removed successfully, otherwise
 * returns false.
 */
bool QContactManager::removeContact(const QUniqueId& contactId)
{
    return d->m_engine->removeContact(contactId, d->m_error);
}

/*!
 * Adds the list of contacts given by \a contactList to the database.
 * Returns a list of the error codes corresponding to the contacts in
 * the \a contactList.  The \l QContactManager::error() function will
 * only return \c QContactManager::NoError if all contacts were saved
 * successfully.
 *
 * For each newly saved contact that was successful, the uid of the contact
 * in the list will be updated with the new value.  If a failure occurs
 * when saving a new contact, the id will be cleared.  If a failure occurs
 * when updating a contact that already exists, then TODO.
 *
 * \sa QContactManager::saveContact()
 */
QList<QContactManager::Error> QContactManager::saveContacts(QList<QContact>* contactList)
{
    return d->m_engine->saveContacts(contactList, d->m_error);
}

/*!
 * Remove the list of contacts identified in \a idList.
 * Returns a list of the error codes corresponding to the contact ids in
 * the \a idList.  The \l QContactManager::error() function will
 * only return \c QContactManager::NoError if all contacts were removed
 * successfully.
 *
 * For each contact that was removed succesfully, the corresponding
 * id in the list will be retained but set to zero.  The id of contacts
 * that were not successfully removed will be left alone.
 *
 * \sa QContactManager::removeContact()
 */
QList<QContactManager::Error> QContactManager::removeContacts(QList<QUniqueId>* idList)
{
    return d->m_engine->removeContacts(idList, d->m_error);
}

/*! Returns a display label for a \a contact which is synthesised from its details in a platform-specific manner */
QString QContactManager::synthesiseDisplayLabel(const QContact& contact) const
{
    return d->m_engine->synthesiseDisplayLabel(contact, d->m_error);
}

/*!
 * Sets the id of the "self" contact to the given \a contactId.
 * Returns true if the "self" contact id was set successfully.
 * If the given \a contactId does not identify a contact
 * stored in this manager, the error will be set to
 * \c QContactManager::DoesNotExistError and the function will
 * return false; if the backend does not support the
 * concept of a "self" contact then the error will be set to
 * \c QContactManager::NotSupportedError and the function will
 * return false.
 */
bool QContactManager::setSelfContactId(const QUniqueId& contactId)
{
    return d->m_engine->setSelfContactId(contactId, d->m_error);
}

/*!
 * Returns the id of the "self" contact which has previously been set.
 * If no "self" contact has been set, or if the self contact was removed
 * from the manager after being set, or if the backend does not support
 * the concept of a "self" contact, an invalid id will be returned
 * and the error will be set to \c QContactManager::DoesNotExistError.
 */
QUniqueId QContactManager::selfContactId() const
{
    return d->m_engine->selfContactId(d->m_error);
}

/*!
 * Returns the first relationship of the given \a relationshipType which has the specified \a sourceId contact.
 * If \a sourceId is the zero id, the first relationship of the given \a relationshipType will be returned.
 * If \a relationshipType is empty, the first relationship with the given \a sourceId will be returned.
 * If \a sourceId is the zero id and \a relationshipType is empty, the first relationship in the database
 * will be returned.
 *
 * If no matching relationships are managed by this manager, a new relationship with the given \a sourceId and
 * \a relationshipType set (but no destination contacts) will be returned.
 */
QContactRelationship QContactManager::relationship(const QUniqueId& sourceId, const QString& relationshipType) const
{
    return d->m_engine->relationship(sourceId, relationshipType, d->m_error);
}

/*!
 * Returns all relationships of the given \a relationshipType which the contact identified by \a sourceId has.
 * If the \a source is the zero id, a list of all of the relationships of the given \a relationshipType is returned.
 * If the \a relationshipType is empty, relationships of any type are returned.
 */
QList<QContactRelationship> QContactManager::relationships(const QUniqueId& sourceId, const QString& relationshipType) const
{
    return d->m_engine->relationships(sourceId, relationshipType, d->m_error);
}

/*!
 * Returns all relationships of the specified \a relationshipType in which the contact identified by \a participantUri is a source or involved participant.
 * If \a participantUri consists of an empty manager URI and the zero id, all relationships of the given \a relationshipType are returned.  If the \a relationshipType
 * is empty, all relationships in which the contact identified by \a participantUri is a source or involved participant are returned.
 */
QList<QContactRelationship> QContactManager::relationships(const QString& relationshipType, const QPair<QString, QUniqueId>& participantUri) const
{
    return d->m_engine->relationships(relationshipType, participantUri, d->m_error);
}

/*!
 * Returns all relationships of any type in which the contact identified by \a participantUri is a source or involved participant.
 */
QList<QContactRelationship> QContactManager::relationships(const QPair<QString, QUniqueId>& participantUri) const
{
    return d->m_engine->relationships(participantUri, d->m_error);
}

/*!
 * Saves the given \a relationship in the database.  If the relationship already exists in the database, but the destination
 * contacts in the relationship have changed, the relationship in the database will be updated with the new information.
 * If the relationship already exists in the database with no differences this function will return \c false and the error will be set
 * to \c QContactManager::AlreadyExistsError.  If the relationship is saved or updated successfully, this function will return
 * \c true and error will be set to \c QContactManager::NoError.
 * The given relationship is invalid if it is circular (one of the destination contacts is also the source contact), or
 * if it references a non-existent local contact (either source or destination).  If the given \a relationship is invalid,
 * the function will return \c false and the error will be set to \c QContactManager::InvalidRelationshipError.
 * If the given \a relationship could not be saved in the database (due to backend limitations)
 * the function will return \c false and error will be set to \c QContactManager::NotSupportedError.
 *
 * If any destination contact manager URI is not set in the \a relationship, these will be
 * automatically set to the URI of this manager, before the relationship is saved.
 */
bool QContactManager::saveRelationship(QContactRelationship* relationship)
{
    return d->m_engine->saveRelationship(relationship, d->m_error);
}

/*!
 * Saves the given \a relationships in the database and returns a list of error codes.
 */
QList<QContactManager::Error> QContactManager::saveRelationships(QList<QContactRelationship>* relationships)
{
    return d->m_engine->saveRelationships(relationships, d->m_error);
}

/*!
 * Removes the given \a relationship from the manager.  If the relationship exists in the manager, the relationship
 * will be removed, the error will be set to \c QContactManager::NoError and this function will return true.  If no such
 * relationship exists in the manager, the error will be set to \c QContactManager::DoesNotExistError and this function
 * will return false.
 *
 * The priority of the relationship is ignored when determining existence of the relationship.
 */
bool QContactManager::removeRelationship(const QContactRelationship& relationship)
{
    return d->m_engine->removeRelationship(relationship, d->m_error);
}

/*!
 * Removes the given \a relationships from the database and returns a list of error codes.
 */
QList<QContactManager::Error> QContactManager::removeRelationships(const QList<QContactRelationship>& relationships)
{
    return d->m_engine->removeRelationships(relationships, d->m_error);
}

/*!
 * Returns a map of identifier to detail definition for the registered detail definitions
 * which are valid for the contacts in this store
 */
QMap<QString, QContactDetailDefinition> QContactManager::detailDefinitions() const
{
    return d->m_engine->detailDefinitions(d->m_error);
}

/*! Returns the definition identified by the given \a definitionName that is valid for the contacts in this store, or a default-constructed QContactDetailDefinition if no such definition exists */
QContactDetailDefinition QContactManager::detailDefinition(const QString& definitionName) const
{
    return d->m_engine->detailDefinition(definitionName, d->m_error);
}

/*! Persists the given definition \a def in the database.  Returns true if the definition was saved successfully, otherwise returns false */
bool QContactManager::saveDetailDefinition(const QContactDetailDefinition& def)
{
    return d->m_engine->saveDetailDefinition(def, d->m_error);
}

/*! Removes the detail definition identified by \a definitionName from the database.  Returns true if the definition was removed successfully, otherwise returns false */
bool QContactManager::removeDetailDefinition(const QString& definitionName)
{
    return d->m_engine->removeDetailDefinition(definitionName, d->m_error);
}

/*!
    Returns an object describing the supported functionality of this QContactManager.

    \sa QContactManagerInfo
 */
QContactManagerInfo* QContactManager::information() const
{
    if (!d->m_info) {
        d->m_info = new QContactManagerInfo;
        d->m_info->d = d;
    }
    return d->m_info;
}

/*! Returns the manager id for this QContactManager */
QString QContactManager::managerName() const
{
    return d->m_managerName;
}

/*! Return the parameters supplied to this QContactManager */
QMap<QString, QString>QContactManager::managerParameters() const
{
    return d->m_params;
}

/*!
 * Return the uri describing this QContactManager, including
 * managerName, managerStoreId and any parameters.
 */
QString QContactManager::managerUri() const
{
    return d->m_uri;
}
