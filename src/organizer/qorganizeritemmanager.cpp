/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** OrganizerItem: Nokia Corporation (qt-info@nokia.com)
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

#include "qorganizeritemmanager.h"

#include "qorganizeritem_p.h"
#include "qorganizeritemfilter.h"
#include "qorganizeritemdetaildefinition.h"
#include "qorganizeritemmanager_p.h"
#include "qorganizeritemfetchhint.h"

#include <QSharedData>
#include <QPair>
#include <QSet>

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerItemManager
  \brief The QOrganizerItemManager class provides an interface which allows clients with access to organizer item information stored in a particular backend.

  \inmodule QtOrganizer
  \ingroup organizer-main

  This class provides an abstraction of a datastore or aggregation of datastores which contains organizer item information.
  It provides methods to retrieve and manipulate organizer item information and supported schema definitions.
  It also provides metadata and error information reporting.

  The functions provided by QOrganizerItemManager are purely synchronous; to access the same functionality in an
  asynchronous manner, clients should use the use-case-specific classes derived from QOrganizerItemAbstractRequest.

  Some functionality provided by QOrganizerItemManager directly is not accessible using the asynchronous API; see
  the \l{Organizer Synchronous API}{synchronous} and \l{Organizer Asynchronous API}{asynchronous} API
  information from the \l{Organizer}{organizer module} API documentation.
 */

/*!
  \fn QOrganizerItemManager::dataChanged()
  This signal is emitted by the manager if its internal state changes, and it is unable to determine the changes
  which occurred, or if the manager considers the changes to be radical enough to require clients to reload all data.
  If this signal is emitted, no other signals will be emitted for the associated changes.
 */

/*!
  \fn QOrganizerItemManager::itemsAdded(const QList<QOrganizerItemLocalId>& itemIds)
  This signal is emitted at some point once the items identified by \a itemIds have been added to a datastore managed by this manager.
  This signal will not be emitted if the dataChanged() signal was previously emitted for these changes.
 */

/*!
  \fn QOrganizerItemManager::itemsChanged(const QList<QOrganizerItemLocalId>& itemIds)
  This signal is emitted at some point once the items identified by \a itemIds have been modified in a datastore managed by this manager.
  This signal will not be emitted if the dataChanged() signal was previously emitted for these changes.
 */

/*!
  \fn QOrganizerItemManager::itemsRemoved(const QList<QOrganizerItemLocalId>& itemIds)
  This signal is emitted at some point once the items identified by \a itemIds have been removed from a datastore managed by this manager.
  This signal will not be emitted if the dataChanged() signal was previously emitted for these changes.
 */

/*!
  \fn QOrganizerItemManager::collectionsAdded(const QList<QOrganizerCollectionLocalId>& collectionIds)
  This signal is emitted at some point once the collections identified by \a collectionIds have been added to a datastore managed by this manager.
  This signal will not be emitted if the dataChanged() signal was previously emitted for these changes.
 */

/*!
  \fn QOrganizerItemManager::collectionsChanged(const QList<QOrganizerCollectionLocalId>& collectionIds)
  This signal is emitted at some point once the metadata for the collections identified by \a collectionIds have been modified in a datastore managed by this manager.
  This signal is not emitted if one of the items in this collection has changed - itemsChanged() will be emitted instead.
  This signal will not be emitted if the dataChanged() signal was previously emitted for these changes.
 */

/*!
  \fn QOrganizerItemManager::collectionsRemoved(const QList<QOrganizerCollectionLocalId>& collectionIds)
  This signal is emitted at some point once the collections identified by \a collectionIds have been removed from a datastore managed by this manager.
  This signal will not be emitted if the dataChanged() signal was previously emitted for these changes.
 */


#define makestr(x) (#x)
#define makename(x) makestr(x)

/*!
    Returns a list of available manager ids that can be used when constructing
    a QOrganizerItemManager.  If an empty id is specified to the constructor, the
    first value in this list will be used instead.
  */
QStringList QOrganizerItemManager::availableManagers()
{
    QStringList ret;
    ret << QLatin1String("memory") << QLatin1String("invalid");
    QOrganizerItemManagerData::loadFactories();
    ret.append(QOrganizerItemManagerData::m_engines.keys());

    // now swizzle the default engine to pole position
#if defined(Q_ORGANIZER_DEFAULT_ENGINE)
    if (ret.removeAll(QLatin1String(makename(Q_ORGANIZER_DEFAULT_ENGINE)))) {
        ret.prepend(QLatin1String(makename(Q_ORGANIZER_DEFAULT_ENGINE)));
    }
#endif

    return ret;
}

/*!
  Splits the given \a uri into the manager, store, and parameters that it describes, and places the information into the memory addressed by \a pManagerId and \a pParams respectively.  Returns true if \a uri could be split successfully, otherwise returns false
 */
bool QOrganizerItemManager::parseUri(const QString& uri, QString* pManagerId, QMap<QString, QString>* pParams)
{
    // Format: qtorganizer:<managerid>:<key>=<value>&<key>=<value>
    // 1) parameters are currently a qstringlist.. should they be a map?
    // 2) is the uri going to be escaped?  my guess would be "probably not"
    // 3) hence, do we assume that the prefix, managerid and storeid cannot contain `:'
    // 4) similarly, that neither keys nor values can contain `=' or `&'

    QStringList colonSplit = uri.split(QLatin1Char(':'));
    QString prefix = colonSplit.value(0);

    if (prefix != QLatin1String("qtorganizer"))
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

/*! Returns a URI that completely describes a manager implementation, datastore, and the parameters with which to instantiate the manager, from the given \a managerName, \a params and an optional \a implementationVersion */
QString QOrganizerItemManager::buildUri(const QString& managerName, const QMap<QString, QString>& params, int implementationVersion)
{
    QString ret(QLatin1String("qtorganizer:%1:%2"));
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

    if (implementationVersion != -1) {
        QString versionString = QString(QLatin1String(QTORGANIZER_IMPLEMENTATION_VERSION_NAME));
        versionString += QString::fromAscii("=");
        versionString += QString::number(implementationVersion);
        escapedParams.append(versionString);
    }

    return ret.arg(managerName, escapedParams.join(QLatin1String("&")));
}

/*!
  Constructs a QOrganizerItemManager whose implementation, store and parameters are specified in the given \a storeUri,
  and whose parent object is \a parent.
 */
QOrganizerItemManager* QOrganizerItemManager::fromUri(const QString& storeUri, QObject* parent)
{
    if (storeUri.isEmpty()) {
        return new QOrganizerItemManager(QString(), QMap<QString, QString>(), parent);
    } else {
        QString id;
        QMap<QString, QString> parameters;
        if (parseUri(storeUri, &id, &parameters)) {
            return new QOrganizerItemManager(id, parameters, parent);
        } else {
            // invalid
            return new QOrganizerItemManager(QLatin1String("invalid"), QMap<QString, QString>(), parent);
        }
    }
}

/*!
  Constructs a QOrganizerItemManager whose parent QObject is \a parent.
  The default implementation for the platform will be created.
 */
QOrganizerItemManager::QOrganizerItemManager(QObject* parent)
    : QObject(parent)
{
    createEngine(QString(), QMap<QString, QString>());
}

/*!
  Constructs a QOrganizerItemManager whose implementation is identified by \a managerName with the given \a parameters.

  The \a parent QObject will be used as the parent of this QOrganizerItemManager.

  If an empty \a managerName is specified, the default implementation for the platform will
  be used.
 */
QOrganizerItemManager::QOrganizerItemManager(const QString& managerName, const QMap<QString, QString>& parameters, QObject* parent)
    : QObject(parent),
    d(new QOrganizerItemManagerData)
{
    createEngine(managerName, parameters);
}

void QOrganizerItemManager::createEngine(const QString& managerName, const QMap<QString, QString>& parameters)
{
    d->createEngine(managerName, parameters);
    connect(d->m_engine, SIGNAL(dataChanged()), this, SIGNAL(dataChanged()));
    connect(d->m_engine, SIGNAL(itemsAdded(QList<QOrganizerItemLocalId>)), this, SIGNAL(itemsAdded(QList<QOrganizerItemLocalId>)));
    connect(d->m_engine, SIGNAL(itemsChanged(QList<QOrganizerItemLocalId>)), this, SIGNAL(itemsChanged(QList<QOrganizerItemLocalId>)));
    connect(d->m_engine, SIGNAL(itemsRemoved(QList<QOrganizerItemLocalId>)), this, SIGNAL(itemsRemoved(QList<QOrganizerItemLocalId>)));
    connect(d->m_engine, SIGNAL(collectionsAdded(QList<QOrganizerCollectionLocalId>)), this, SIGNAL(collectionsAdded(QList<QOrganizerCollectionLocalId>)));
    connect(d->m_engine, SIGNAL(collectionsChanged(QList<QOrganizerCollectionLocalId>)), this, SIGNAL(collectionsChanged(QList<QOrganizerCollectionLocalId>)));
    connect(d->m_engine, SIGNAL(collectionsRemoved(QList<QOrganizerCollectionLocalId>)), this, SIGNAL(collectionsRemoved(QList<QOrganizerCollectionLocalId>)));
}

/*!
  Constructs a QOrganizerItemManager whose backend has the name \a managerName and version \a implementationVersion, where the manager
  is constructed with the provided \a parameters.

  The \a parent QObject will be used as the parent of this QOrganizerItemManager.

  If an empty \a managerName is specified, the default implementation for the platform will be instantiated.
  If the specified implementation version is not available, the manager with the name \a managerName with the default implementation version is instantiated.
 */
QOrganizerItemManager::QOrganizerItemManager(const QString& managerName, int implementationVersion, const QMap<QString, QString>& parameters, QObject* parent)
    : QObject(parent),
    d(new QOrganizerItemManagerData)
{
    QMap<QString, QString> params = parameters;
    params[QString(QLatin1String(QTORGANIZER_IMPLEMENTATION_VERSION_NAME))] = QString::number(implementationVersion);
    createEngine(managerName, params);
}

/*! Frees the memory used by the QOrganizerItemManager */
QOrganizerItemManager::~QOrganizerItemManager()
{
    delete d;
}

/*!
  \enum QOrganizerItemManager::Error

  This enum specifies an error that occurred during the most recent operation:

  \value NoError The most recent operation was successful
  \value DoesNotExistError The most recent operation failed because the requested organizer item or detail definition does not exist
  \value AlreadyExistsError The most recent operation failed because the specified organizer item or detail definition already exists
  \value InvalidDetailError The most recent operation failed because the specified organizer detail definition already exists
  \value LockedError The most recent operation failed because the datastore specified is currently locked
  \value DetailAccessError The most recent operation failed because a detail was modified or removed and its access method does not allow that
  \value PermissionsError The most recent operation failed because the caller does not have permission to perform the operation
  \value OutOfMemoryError The most recent operation failed due to running out of memory
  \value NotSupportedError The most recent operation failed because the requested operation is not supported in the specified store
  \value BadArgumentError The most recent operation failed because one or more of the parameters to the operation were invalid
  \value UnspecifiedError The most recent operation failed for an undocumented reason
  \value VersionMismatchError The most recent operation failed because the backend of the manager is not of the required version
  \value LimitReachedError The most recent operation failed because the limit for that type of object has been reached
  \value InvalidItemTypeError The most recent operation failed for an undocumented reason
  \value InvalidCollectionError The most recent operation failed because the collection is invalid
  \value InvalidOccurrenceError The most recent operation failed because it was an attempt to save an occurrence without a correct InstanceOrigin detail
 */

/*! Return the error code of the most recent operation */
QOrganizerItemManager::Error QOrganizerItemManager::error() const
{
    return d->m_error;
}

/*!
  Returns per-input error codes for the most recent operation.
  This function only returns meaningful information if the most
  recent operation was a batch operation.
  Each key in the map is the index of the element in the input list
  for which the error (whose error code is stored in the value for
  that key in the map) occurred during the batch operation.
  \sa error(), saveItems(), removeItems()
 */
QMap<int, QOrganizerItemManager::Error> QOrganizerItemManager::errorMap() const
{
    return d->m_errorMap;
}

/*!
  Return the list of organizer item instances which match the given \a filter, sorted according to the given \a sortOrders.
  The client may instruct the manager that it does not require all possible information about each instance by specifying a fetch hint \a fetchHint;
  the manager can choose to ignore the fetch hint, but if it does so, it must return all possible information about each instance.
  */
QList<QOrganizerItem> QOrganizerItemManager::itemInstances(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint) const
{
    d->m_error = QOrganizerItemManager::NoError;
    d->m_errorMap.clear();
    return d->m_engine->itemInstances(filter, sortOrders, fetchHint, &d->m_error);
}

/*!
  Return the list of a maximum of \a maxCount organizer item instances which are occurrences of the given \a generator recurring item, which
  occur between the given \a periodStart date and the given \a periodEnd date.

  If \a periodStart is after \a periodEnd, the operation will fail.
  If \a maxCount is negative, it is backend specific as to how many occurrences will be returned.
  Some backends may return no instances, others may return some limited number of occurrences.
  */
QList<QOrganizerItem> QOrganizerItemManager::itemInstances(const QOrganizerItem& generator, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount) const
{
    d->m_error = QOrganizerItemManager::NoError;
    d->m_errorMap.clear();
    return d->m_engine->itemInstances(generator, periodStart, periodEnd, maxCount, &d->m_error);
}


/*!
  Return the list of organizer item ids, sorted according to the given list of \a sortOrders
 */
QList<QOrganizerItemLocalId> QOrganizerItemManager::itemIds(const QList<QOrganizerItemSortOrder>& sortOrders) const
{
    d->m_error = QOrganizerItemManager::NoError;
    d->m_errorMap.clear();
    return d->m_engine->itemIds(QOrganizerItemFilter(), sortOrders, &d->m_error);
}

/*!
  Returns a list of organizer item ids that match the given \a filter, sorted according to the given list of \a sortOrders.
  Depending on the backend, this filtering operation may involve retrieving all the organizeritems.
 */
QList<QOrganizerItemLocalId> QOrganizerItemManager::itemIds(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders) const
{
    d->m_error = QOrganizerItemManager::NoError;
    d->m_errorMap.clear();
    return d->m_engine->itemIds(filter, sortOrders, &d->m_error);
}

/*!
  Returns the list of organizeritems stored in the manager sorted according to the given list of \a sortOrders.

  The \a fetchHint parameter describes the optimization hints that a manager may take.
  If the \a fetchHint is the default constructed hint, all existing details and relationships
  in the matching organizeritems will be returned.  A client should not make changes to an organizer item which has
  been retrieved using a fetch hint other than the default fetch hint.  Doing so will result in information
  loss when saving the organizer item back to the manager (as the "new" restricted organizer item will
  replace the previously saved organizer item in the backend).

  \sa QOrganizerItemFetchHint
 */
QList<QOrganizerItem> QOrganizerItemManager::items(const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint) const
{
    d->m_error = QOrganizerItemManager::NoError;
    d->m_errorMap.clear();
    return d->m_engine->items(QOrganizerItemFilter(), sortOrders, fetchHint, &d->m_error);
}

/*!
  Returns a list of organizeritems that match the given \a filter, sorted according to the given list of \a sortOrders.

  Depending on the manager implementation, this filtering operation might be slow and involve retrieving all the
  organizeritems and testing them against the supplied filter - see the \l isFilterSupported() function.

  The \a fetchHint parameter describes the optimization hints that a manager may take.
  If the \a fetchHint is the default constructed hint, all existing details and relationships
  in the matching organizeritems will be returned.  A client should not make changes to an organizer item which has
  been retrieved using a fetch hint other than the default fetch hint.  Doing so will result in information
  loss when saving the organizer item back to the manager (as the "new" restricted organizer item will
  replace the previously saved organizer item in the backend).

  \sa QOrganizerItemFetchHint
 */
QList<QOrganizerItem> QOrganizerItemManager::items(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint) const
{
    d->m_error = QOrganizerItemManager::NoError;
    d->m_errorMap.clear();
    return d->m_engine->items(filter, sortOrders, fetchHint, &d->m_error);
}

/*!
  Returns the organizer item in the database identified by \a organizeritemId.

  If the organizer item does not exist, an empty, default constructed QOrganizerItem will be returned,
  and the error returned by \l error() will be \c QOrganizerItemManager::DoesNotExistError.

  The \a fetchHint parameter describes the optimization hints that a manager may take.
  If the \a fetchHint is the default constructed hint, all existing details and relationships
  in the matching organizer item will be returned.  A client should not make changes to an organizer item which has
  been retrieved using a fetch hint other than the default fetch hint.  Doing so will result in information
  loss when saving the organizer item back to the manager (as the "new" restricted organizer item will
  replace the previously saved organizer item in the backend).

  \sa QOrganizerItemFetchHint
 */
QOrganizerItem QOrganizerItemManager::item(const QOrganizerItemLocalId& organizeritemId, const QOrganizerItemFetchHint& fetchHint) const
{
    d->m_error = QOrganizerItemManager::NoError;
    d->m_errorMap.clear();
    return d->m_engine->item(organizeritemId, fetchHint, &d->m_error);
}

/*!
  Adds the given \a organizeritem to the database if \a organizeritem has a
  default-constructed id, or an id with the manager URI set to the URI of
  this manager and a local id of zero.  It will be saved in the collection specified
  by \a collectionId if the specified collection exists, or if no \a collectionId is
  specified, or the \a collectionId is the default (zero) collection id, it will be
  saved in the collection in which the item is currently saved (if it is not a new
  item) or in the default collection (if it is a new item).

  Each collection may have a different schema, so if the item cannot be saved
  in the given collection due to invalid details, the function will return false.
  An item which is valid in one collection may be invalid in another collection, in the
  same manager.

  If the manager URI of the id of the \a organizeritem is neither empty nor equal to the URI of
  this manager, or local id of the \a organizeritem is non-zero but does not exist in the
  manager, the operation will fail and calling error() will return
  \c QOrganizerItemManager::DoesNotExistError.

  Alternatively, the function will update the existing organizer item in the database if \a organizeritem
  has a non-zero id and currently exists in the database.

  If the \a organizeritem contains one or more details whose definitions have
  not yet been saved with the manager, the operation will fail and calling
  error() will return \c QOrganizerItemManager::UnsupportedError.

  Returns false on failure, or true on
  success.  On successful save of an organizer item with an id of zero, its
  id will be set to a new, valid id with the manager URI set to the URI of
  this manager, and the local id set to a new, valid local id.
  The manager will automatically synthesize the display label of the organizer item when it is saved.
  The manager is not required to fetch updated details of the organizer item on save,
  and as such, clients should fetch an organizer item if they want the most up-to-date information
  by calling \l QOrganizerItemManager::item().

  \sa managerUri()
 */
bool QOrganizerItemManager::saveItem(QOrganizerItem* organizeritem, const QOrganizerCollectionLocalId& collectionId)
{
    d->m_errorMap.clear();
    if (organizeritem) {
        d->m_error = QOrganizerItemManager::NoError;
        return d->m_engine->saveItem(organizeritem, collectionId, &d->m_error);
    } else {
        d->m_error = QOrganizerItemManager::BadArgumentError;
        return false;
    }
}

/*!
  Remove the organizer item identified by \a organizeritemId from the database.
  Returns true if the organizer item was removed successfully, otherwise
  returns false.
 */
bool QOrganizerItemManager::removeItem(const QOrganizerItemLocalId& organizeritemId)
{
    d->m_error = QOrganizerItemManager::NoError;
    d->m_errorMap.clear();
    return d->m_engine->removeItem(organizeritemId, &d->m_error);
}

/*!
  Adds the list of organizeritems given by \a organizeritems list to the database, in
  the collection identified by the given \a collectionId.
  Returns true if the organizeritems were saved successfully, otherwise false.

  If the given \a collectionId does not exist, the function will return false.
  If the given \a collectionId is the default (zero) id, the items will be saved
  in the collection in which they are currently saved (if they are not new items) or
  in the default collection (if they are new items).
  If the given \a collectionId does exist, all items will be saved in the collection
  identified by the given \a collectionId.

  Each collection may have a different schema, so if any of the items cannot be saved
  in the given collection due to invalid details, the function will return false.
  An item which is valid in one collection may be invalid in another collection, in the
  same manager.

  Calling \l errorMap() will return the per-input errors for the latest batch function.
  The \l QOrganizerItemManager::error() function will only return \c QOrganizerItemManager::NoError
  if all organizeritems were saved successfully.

  For each newly saved organizer item that was successful, the id of the organizeritem
  in the \a organizeritems list will be updated with the new value.  If a failure occurs
  when saving a new organizeritem, the id will be cleared.

  \sa QOrganizerItemManager::saveItem()
 */
bool QOrganizerItemManager::saveItems(QList<QOrganizerItem>* organizeritems, const QOrganizerCollectionLocalId& collectionId)
{
    d->m_errorMap.clear();
    if (!organizeritems) {
        d->m_error = QOrganizerItemManager::BadArgumentError;
        return false;
    }

    d->m_error = QOrganizerItemManager::NoError;
    return d->m_engine->saveItems(organizeritems, collectionId, &d->m_errorMap, &d->m_error);
}

/*!
  Remove every organizer item whose id is contained in the list of organizeritems ids
  \a organizeritemIds.  Returns true if all organizeritems were removed successfully,
  otherwise false.

  Calling \l errorMap() will return the per-input errors for the latest batch function.
  The \l QOrganizerItemManager::error() function will
  only return \c QOrganizerItemManager::NoError if all organizeritems were removed
  successfully.

  If the given list of organizer item ids \a organizeritemIds is empty, the function will return false
  and calling error() will return \c QOrganizerItemManager::BadArgumentError.  If the list is non-empty
  and contains ids which do not identify a valid organizer item in the manager, the function will
  remove any organizeritems which are identified by ids in the \a organizeritemIds list, insert
  \c QOrganizerItemManager::DoesNotExist entries into the error map for the indices of invalid ids
  in the \a organizeritemIds list, return false, and set the overall operation error to
  \c QOrganizerItemManager::DoesNotExistError.

  \sa QOrganizerItemManager::removeItem()
 */
bool QOrganizerItemManager::removeItems(const QList<QOrganizerItemLocalId>& organizeritemIds)
{
    d->m_errorMap.clear();
    if (organizeritemIds.isEmpty()) {
        d->m_error = QOrganizerItemManager::BadArgumentError;
        return false;
    }

    d->m_error = QOrganizerItemManager::NoError;
    return d->m_engine->removeItems(organizeritemIds, &d->m_errorMap, &d->m_error);
}

/*!
  Returns the id of the default collection managed by this manager
 */
QOrganizerCollectionLocalId QOrganizerItemManager::defaultCollectionId() const
{
    d->m_error = QOrganizerItemManager::NoError;
    d->m_errorMap.clear();
    return d->m_engine->defaultCollectionId(&d->m_error);
}

/*!
  Returns the ids of collections managed by this manager.
 */
QList<QOrganizerCollectionLocalId> QOrganizerItemManager::collectionIds() const
{
    d->m_error = QOrganizerItemManager::NoError;
    d->m_errorMap.clear();
    return d->m_engine->collectionIds(&d->m_error);
}

/*!
  Returns the collections managed by this manager which
  have an id contained in the list of collection ids \a collectionIds.
  If the list of collection ids \a collectionIds is empty or
  not specified, this function will return an empty list of collections.

  If any of the ids in the given list of \a collectionIds is invalid (does not
  exist in the manager), an error will be inserted into the \a error map at that
  index.  Calling \l errorMap() will return the per-input errors for the latest
  batch function.

  XXX TODO: does the return list get filled with "blank" collections for errors?
 */
QList<QOrganizerCollection> QOrganizerItemManager::collections(const QList<QOrganizerCollectionLocalId>& collectionIds) const
{
    d->m_error = QOrganizerItemManager::NoError;
    d->m_errorMap.clear();
    return d->m_engine->collections(collectionIds, &d->m_errorMap, &d->m_error);
}

/*!
  Saves the given \a collection in the manager.
  Returns true on success, false on failure.

  Some managers do not allow modifications to collections,
  and thus attempting to save a collection will always fail
  when attempted in such a manager.

  Some managers do not allow adding new collections,
  and thus attempting to save a new collection will always fail
  when attempted in such a manager.

  Some managers provide front-ends to read-only datastores, and
  attempting to save a new collection in such a manager will
  always fail.
 */
bool QOrganizerItemManager::saveCollection(QOrganizerCollection* collection)
{
    d->m_errorMap.clear();
    if (collection) {
        d->m_error = QOrganizerItemManager::NoError;
        return d->m_engine->saveCollection(collection, &d->m_error);
    } else {
        d->m_error = QOrganizerItemManager::BadArgumentError;
        return false;
    }
}

/*!
  Removes the collection identified by the given \a collectionId (and all items in the collection)
  from the manager if the given \a collectionId exists.
  Returns true on success, false on failure.

  Attempting to remove the default collection will fail and calling \l error() will return
  QOrganizerItemManager::PermissionsError.
 */
bool QOrganizerItemManager::removeCollection(const QOrganizerCollectionLocalId& collectionId)
{
    d->m_error = QOrganizerItemManager::NoError;
    d->m_errorMap.clear();
    return d->m_engine->removeCollection(collectionId, &d->m_error);
}

/*!
  Returns a pruned or modified version of the \a original organizer item which is valid and can be saved in the manager.
  The returned organizer item might have entire details removed or arbitrarily changed.
 */
QOrganizerItem QOrganizerItemManager::compatibleItem(const QOrganizerItem& original)
{
    d->m_error = QOrganizerItemManager::NoError;
    d->m_errorMap.clear();
    return d->m_engine->compatibleItem(original, &d->m_error);
}

/*!
  Returns a pruned or modified version of the \a original organizer collection which is valid and can be saved in the manager.
  The returned organizer collection might have meta data removed or arbitrarily changed.
 */
QOrganizerCollection QOrganizerItemManager::compatibleCollection(const QOrganizerCollection& original)
{
    d->m_error = QOrganizerItemManager::NoError;
    d->m_errorMap.clear();
    return d->m_engine->compatibleCollection(original, &d->m_error);
}

/*!
  Returns a map of identifier to detail definition for the registered detail definitions which are valid for organizeritems whose type is the given \a organizeritemType
  which are valid for the organizeritems in this store
 */
QMap<QString, QOrganizerItemDetailDefinition> QOrganizerItemManager::detailDefinitions(const QString& organizeritemType) const
{
    d->m_errorMap.clear();
    if (!supportedItemTypes().contains(organizeritemType)) {
        d->m_error = QOrganizerItemManager::InvalidItemTypeError;
        return QMap<QString, QOrganizerItemDetailDefinition>();
    }

    d->m_error = QOrganizerItemManager::NoError;
    return d->m_engine->detailDefinitions(organizeritemType, &d->m_error);
}

/*! Returns the definition identified by the given \a definitionName that is valid for the organizeritems whose type is the given \a organizeritemType in this store, or a default-constructed QOrganizerItemDetailDefinition if no such definition exists */
QOrganizerItemDetailDefinition QOrganizerItemManager::detailDefinition(const QString& definitionName, const QString& organizeritemType) const
{
    d->m_errorMap.clear();
    if (!supportedItemTypes().contains(organizeritemType)) {
        d->m_error = QOrganizerItemManager::InvalidItemTypeError;
        return QOrganizerItemDetailDefinition();
    }

    d->m_error = QOrganizerItemManager::NoError;
    return d->m_engine->detailDefinition(definitionName, organizeritemType, &d->m_error);
}

/*! Persists the given definition \a def in the database, which is valid for organizeritems whose type is the given \a organizeritemType.  Returns true if the definition was saved successfully, otherwise returns false */
bool QOrganizerItemManager::saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& organizeritemType)
{
    d->m_errorMap.clear();
    if (!supportedItemTypes().contains(organizeritemType)) {
        d->m_error = QOrganizerItemManager::InvalidItemTypeError;
        return false;
    }

    d->m_error = QOrganizerItemManager::NoError;
    return d->m_engine->saveDetailDefinition(def, organizeritemType, &d->m_error);
}

/*! Removes the detail definition identified by \a definitionName from the database, which is valid for organizeritems whose type is the given \a organizeritemType.  Returns true if the definition was removed successfully, otherwise returns false */
bool QOrganizerItemManager::removeDetailDefinition(const QString& definitionName, const QString& organizeritemType)
{
    d->m_errorMap.clear();
    if (!supportedItemTypes().contains(organizeritemType)) {
        d->m_error = QOrganizerItemManager::InvalidItemTypeError;
        return false;
    }

    d->m_error = QOrganizerItemManager::NoError;
    return d->m_engine->removeDetailDefinition(definitionName, organizeritemType, &d->m_error);
}

/*!
  \enum QOrganizerItemManager::ManagerFeature
  This enum describes the possible features that a particular manager may support
  \value MutableDefinitions The manager supports saving, updating or removing detail definitions.  Some built-in definitions may still be immutable
  \value ChangeLogs The manager supports reporting of timestamps of changes, and filtering and sorting by those timestamps
  \value Anonymous The manager is isolated from other managers
 */

/*!
  Returns true if the given feature \a feature is supported by the manager, for the specified type of organizer item \a organizeritemType
 */
bool QOrganizerItemManager::hasFeature(QOrganizerItemManager::ManagerFeature feature, const QString& organizeritemType) const
{
    return d->m_engine->hasFeature(feature, organizeritemType);
}

/*!
  Returns the list of data types supported by the manager
 */
QList<int> QOrganizerItemManager::supportedDataTypes() const
{
    return d->m_engine->supportedDataTypes();
}

/*!
  Returns true if the given \a filter is supported natively by the
  manager, and false if the filter behaviour would be emulated.

  Note: In some cases, the behaviour of an unsupported filter
  cannot be emulated.  For example, a filter that requests organizeritems
  that have changed since a given time depends on having that information
  available.  In these cases, the filter will fail.
 */
bool QOrganizerItemManager::isFilterSupported(const QOrganizerItemFilter& filter) const
{
    return d->m_engine->isFilterSupported(filter);
}

/*!
  Returns the list of organizer item types which are supported by this manager.
  This is a convenience function, equivalent to retrieving the allowable values
  for the \c QOrganizerItemType::FieldType field of the QOrganizerItemType definition
  which is valid in this manager.
 */
QStringList QOrganizerItemManager::supportedItemTypes() const
{
    return d->m_engine->supportedItemTypes();
}

/*!
  Returns the engine backend implementation version number
 */
int QOrganizerItemManager::managerVersion() const
{
    return d->m_engine->managerVersion();
}

/*! Returns the manager name for this QOrganizerItemManager */
QString QOrganizerItemManager::managerName() const
{
    return d->m_engine->managerName();
}

/*! Return the parameters relevant to the creation of this QOrganizerItemManager */
QMap<QString, QString> QOrganizerItemManager::managerParameters() const
{
    QMap<QString, QString> params = d->m_engine->managerParameters();

    params.remove(QString::fromAscii(QTORGANIZER_VERSION_NAME));
    params.remove(QString::fromAscii(QTORGANIZER_IMPLEMENTATION_VERSION_NAME));
    return params;
}

/*!
  Return the uri describing this QOrganizerItemManager, consisting of the manager name and any parameters.
 */
QString QOrganizerItemManager::managerUri() const
{
    return d->m_engine->managerUri();
}

#include "moc_qorganizeritemmanager.cpp"

QTM_END_NAMESPACE
