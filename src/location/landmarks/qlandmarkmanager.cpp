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

#include "qlandmarkmanager.h"
#include "qlandmarkmanager_p.h"

#include <QStringList>
#include <QString>
#include <QIODevice>
#include <QFile>
#include <QUrl>
#include <QPluginLoader>
#include <QDebug>
#include "qlandmarkcategoryid.h"
#include "qlandmarkcategory.h"
#include "qlandmark.h"
#include "qlandmarkmanagerengine.h"
#include "qlandmarkmanagerenginefactory.h"
#include "qlandmarkidfilter.h"

QTM_USE_NAMESPACE

Q_DEFINE_LATIN1_CONSTANT(QLandmarkManager::Gpx, "Gpx");
Q_DEFINE_LATIN1_CONSTANT(QLandmarkManager::Lmx, "Lmx");
Q_DEFINE_LATIN1_CONSTANT(QLandmarkManager::Kml, "Kml");
Q_DEFINE_LATIN1_CONSTANT(QLandmarkManager::Kmz, "Kmz");

/*!
    \class QLandmarkManager
    \brief The QLandmarkManager class provides an interface for storage
    and retrieval of landmarks from a landmark store.

    \inmodule QtLocation

    \ingroup landmarks-main

    The QLandmarkManager is the starting class to use when working with landmarks.
    If effectively represents a landmark datastore and it provides the synchronous operations for the
    creation, retrieval, updating and deletion of both landmarks and categories.  For asynchronous operations
    use the \l {Asynchronous Landmark Requests} {request classes} which use the manager as a parameter.
    The manager provides notifications whenever landmarks or categories are added, updated or removed.
    These are important for the cases where another process is making modifications to the same manager/datastore.

    Each manager is identified by a manager name which typically takes the form of a reverse domain string
    such as \c com.nokia.qt.landmarks.engines.sqlite.  However every supported platform provides a default
    manager which may be instantiated without having to provide a name like so:
    \snippet doc/src/snippets/qtlandmarksdocsample/qtlandmarksdocsample.cpp Instantiate default QLandmarkManager

    \section1 Retrieval operations
    To retrieve a set of landmarks we provide may provide a QLandmarkFilter, QLandmarkSortOrder and limit and offset as necessary.
    The QLandmarkFilter defines the criteria for selecting landmarks e.g. a QLandmarkCategoryFilter may be used
    to choose landmarks that belong to a certain category or a QLandmarkProximityFilter to choose landmarks
    within a certain range from a given location.  A QLandmarkSortOrder order defines how the results should
    be sorted.  The limit allows specification of the maximum number of items to
    return and the offset defines the index of the first item.  The following demonstrates how to search for the 5 nearest
    landmarks to a given coordinate.

    \snippet doc/src/snippets/qtlandmarksdocsample/qtlandmarksdocsample.cpp Retrieve landmarks by proximity synchronously

    The set of parameters described above are not always necessary as defaults are provided, if we wanted to retrieve
    all landmarks, then the appropriate call is:

    \snippet doc/src/snippets/qtlandmarksdocsample/qtlandmarksdocsample.cpp Retrieve all landmarks synchronously
    \bold {Note:} Landmark retrieval is potentially a long operation, the synchronous API provided by the manager
    is subject to blocking.  It is generally recommended that the QLandmarkFetchRequest be used because
    it behaves asynchronously.

    Categories may be retrieved in a similar manner:
    \snippet doc/src/snippets/qtlandmarksdocsample/qtlandmarksdocsample.cpp Retrieve categories synchronously simple

    \section1 Saving and deleting
    Saving and deleting landmarks and categories are fairly straightforward.  To add a new landmark or category
    simply instantiate a QLandmark or QLandmarkCategory, set its data fields(e.g name, coordiante etc) and pass
    a pointer to the appropriate save operation.  For example:

    \snippet doc/src/snippets/qtlandmarksdocsample/qtlandmarksdocsample.cpp Add landmark synchronously simple

    We pass the landmark by pointer bcause it will be assigned a new QLandmarkId when the function call is done.
    Saving a landmark with a valid id already set will update the existing landmark.

    Removal of landmark may be done as follows:
    \snippet doc/src/snippets/qtlandmarksdocsample/qtlandmarksdocsample.cpp Remove landmark synchronously simple

    \section1 Importing and exporting
    Import and exporting are potentially long operations, to perform these operations asynchronously
    see QLandmarkImportRequest and QLandmarkExportRequest.  The simplest way to perform import and export
    operations is to specify a filename:

    \snippet doc/src/snippets/qtlandmarksdocsample/qtlandmarksdocsample.cpp ImportExport landmark simple

    \i {At the time of the QtMobility 1.1 tech preview, only importing of landmarks by supplying a
       file format is functional.  For the supported platforms, the file format that may be supplied
       is "GpxV1.1" which means only Gpx version 1.1 files are known to work.  Exporting is still
       under development and so are notifications.}

*/

/*!
    \enum QLandmarkManager::Error
    Defines the possible errors for the landmark manager.
    \value NoError No error occurred.
    \value DoesNotExistError The most recent operation failed because the requested landmark or category does not exist.
    \value AlreadyExistsError The most recent operation failed because the specified landmark or category already exists.
    \value LockedError The most recent operation failed because the datastore specified is currently locked.
    \value PermissionsError The most recent operation failed because the caller does not have permission to perform the operation.
    \value OutOfMemoryError The most recent operation failed due to running out of memory.
    \value VersionMismatchError The most recent operation failed because the backend of the manager is not of the required version.
    \value NotSupportedError The most recent operation failed because the requested operation is not supported in the specified store.
    \value BadArgumentError The most recent operation failed because one or more of the parameters to the operation were invalid.
    \value InvalidManagerError The most recent operation failed because the manager failed to initialize correctly and is invalid.
           This could be due using a manager name that is not recognised/available. A landmark request object will return this error if
           if is assigned a null manager pointer.
    \value ParsingError The most recent operation failed because the imported file could not be parsed.
    \value CancelError The most recent operation failed to complete due to user cancelation.
    \value UnknownError The most recent operation failed for an unknown reason.
*/

/*!
    \enum QLandmarkManager::TransferOption
    Defines the possible options when transfering landmarks during import or export.
    \value IncludeCategoryData During an import category data is included.  If an imported category doesn't exist
                               the category is created.  If the imported category name matches an existing
                               category name, then the landmark is added to that category.  For exports, categories
                               are included in the exported file if the file format allows it.
    \value ExcludeCategoryData Landmarks are imported or exported without any categories assigned to the landmarks.
    \value AttachSingleCategory Only relevant for import operations.  When landmarks are imported they are
                                all assigned to a given category.
*/

/*!
    \enum QLandmarkManager::LandmarkFeature
    Defines the possible features the landmark manager can support.
    \value ExtendedAttributes The manager supports extra attributes above the standard cross platform attributes.
                              These attributes are specific to the manager backend implementation.
    \value CustomAttributes The manager supports applications associating arbitrary custom attributes to
                            landmarks and categories.
    \value ImportExport The manager supports import and/or export operations
    \value Notifications The manager will emit notification signals when landmarks/categories have
                         been added/modified/removed from the datastore it manages.
*/

/*!
    \enum QLandmarkManager::SupportLevel
    Defines the possible support levels the manager can provide for a given filter or sort order list.
    \value Native The manager natively supports the filter or sort order list.
    \value Emulated The manager emulates the behaviour of the filter or sort order list.
                     Emulated behaviour will inherently be slower than a natively supported implementation.
    \value None The manager does not support the filter or sort order list at all.
*/

/*!
    \enum QLandmarkManager::TransferOperation
    Defines the type of transfer.
    \value ImportOperation
    \value ExportOperation
*/

/*!
    Constructs a QLandmarkManager. The default implementation for the platform will be used.

    The \a parent QObject will be used as the parent of this QLandmarkManager.
*/
QLandmarkManager::QLandmarkManager(QObject *parent)
        : QObject(parent),
          d_ptr(new QLandmarkManagerPrivate())
{
    Q_D(QLandmarkManager);
    d->q_ptr = this;
    QString managerName;

#ifdef Q_OS_SYMBIAN
#else
    managerName = "com.nokia.qt.landmarks.engines.sqlite";
#endif

    d->createEngine(managerName);
    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager, name: %1").arg(managerName);
        qWarning() << "Invalid QLandmarkManager instantiated with name: " << managerName;
    }
}

/*!
    Constructs a QLandmarkManager whose implementation is identified by \a managerName with the given
    \a parameters.

    The \a parent QObject will be used as the parent of this QLandmarkManager.

    If an empty \a managerName is specified, the default implementation for the platform will be used.
*/
QLandmarkManager::QLandmarkManager(const QString &managerName, const QMap<QString, QString> &parameters, QObject *parent)
        : QObject(parent),
          d_ptr(new QLandmarkManagerPrivate())
{
    Q_D(QLandmarkManager);
    d->q_ptr = this;
    d->createEngine(managerName, parameters);

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager, name: %1").arg(managerName);
        qWarning() << "Invalid QLandmarkManager instantiated with name: " << managerName;
    }
}

/*!
    Frees the memory used by the QLandmarkManager
*/
QLandmarkManager::~QLandmarkManager()
{
    Q_D(QLandmarkManager);

    if (d->engine)
        delete d->engine;

    delete d;
}

/*!
    Adds the given \a landmark to the database if \a landmark has a
    default-constructed identifer, or an identifier with the manager
    URI set to the URI of this manager and an empty id.

    If the manager URI of the identifier of the \a landmark is neither
    empty nor equal to the URI of this manager, or the id member of the
    identifier is not empty, but does not exist in the manager,
    the operation will fail and calling error() will return
    \c QLandmarkManager::DoesNotExistError.

    Alternatively, the function will update the existing landmark in the
    database if \a landmark has a non-empty id and currently exists
    within the database.

    Returns false on failure or true on success.  On successful save
    of a landmark with an empty id, it will be assigned a valid
    id and have its manager URI set to the URI of this manager.
*/
bool QLandmarkManager::saveLandmark(QLandmark *landmark)
{
    Q_D(QLandmarkManager);

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return false;
    }

    return d->engine->saveLandmark(landmark,
                                   &(d->errorCode),
                                   &(d->errorString));
}

/*!
    Adds the list of \a landmarks to the database.
    Returns true if the landmarks were saved successfully, otherwise returns
    false.

    The manager might populate \a errorMap (the map of indices of the
    \a landmarks list to the error which occurred when saving the landmark
    at that index) for every index for which the landmark could not be
    saved.

    The \l QLandmarkManager::error() function will only return \c
    QLandmarkManager::NoError if all landmarks were saved successfully.


    For each new landmark that was successfully saved, a landmark identifier
    is assigned to that landmark.
*/
bool QLandmarkManager::saveLandmarks(QList<QLandmark> *landmarks,
                                     QMap<int, QLandmarkManager::Error> *errorMap)
{
    Q_D(QLandmarkManager);

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return false;
    }

    return d->engine->saveLandmarks(landmarks,
                                    errorMap,
                                    &(d->errorCode),
                                    &(d->errorString));
}

/*!
    Remove the landmark identified by \a landmarkId from the database.

    Returns true if the landmark was removed successfully, otherwise
    returnse false.
*/
bool QLandmarkManager::removeLandmark(const QLandmarkId &landmarkId)
{
    Q_D(QLandmarkManager);

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return false;
    }

    return d->engine->removeLandmark(landmarkId,
                                     &(d->errorCode),
                                     &(d->errorString));
}

/*!
    Removes every landmark whose identifier is contained in the list
    of \a landmarkIds.  Returns true if all landmarks were removed
    successfully, otherwise false.

    The manager might populate \a errorMap (the map of indices of the
    \a landmarkIds list to the error which occurred when saving the landmark
    at that index) for every index for which the landmark could not be
    removed.
    The \l QLandmarkManager::error() function will only return
    \c QLandmarkManager::NoError if all landmarks were removed successfully.

    \sa QLandmarkManager::removeLandmark()
*/
bool QLandmarkManager::removeLandmarks(const QList<QLandmarkId> &landmarkIds,
                                       QMap<int, QLandmarkManager::Error> *errorMap)
{
    Q_D(QLandmarkManager);

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return false;
    }

    return d->engine->removeLandmarks(landmarkIds,
                                      errorMap,
                                      &(d->errorCode),
                                      &(d->errorString));
}


/*!
    Adds the given \a category to the database if \a category has a
    default-constructed identifier, or an identifier with the manager
    URI set to the URI of this manager and an empty id.

    If the manager URI of the identifier of the \a category is neither
    empty nor equal to the URI  of this manager, or the id member of the
    identifier is not empty, but does not exist in the manager,
    the operation will fail and calling error() will return
    \c QLandmarkManager::DoesNotExistError.

    Alternatively, the function will update the existing category in the
    database if \a category has a non-empty id and currently exists
    within the database.

    Returns false on failure or true on success.  On successful save
    of a category with an invalid id, it will be assigned a valid
    id and have its manager URI set to the URI of this manager.
*/
bool QLandmarkManager::saveCategory(QLandmarkCategory *category)
{
    Q_D(QLandmarkManager);

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return false;
    }

    return d->engine->saveCategory(category,
                                   &(d->errorCode),
                                   &(d->errorString));
}

/*!
    Remove the category identified by \a categoryId from the database.
    The categoryId is cleared(and becomes invalid) on successful
    removal.  An unsuccessful removal will leave the identifer alone.

    Returns true if the category was removed successfully, otherwise
    returnse false.
*/
bool QLandmarkManager::removeCategory(const QLandmarkCategoryId &categoryId)
{
    Q_D(QLandmarkManager);

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return false;
    }

    return d->engine->removeCategory(categoryId,
                                     &(d->errorCode),
                                     &(d->errorString));
}

/*!
    Returns the cateory in the database identified by \a categoryId.
*/
QLandmarkCategory QLandmarkManager::category(const QLandmarkCategoryId &categoryId) const
{
    Q_D(const QLandmarkManager);

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return QLandmarkCategory();
    }

    QLandmarkCategory cat = d->engine->category(categoryId,
                            &(d->errorCode),
                            &(d->errorString));

    if (d->errorCode != NoError)
        return QLandmarkCategory();

    return cat;
}

/*!
     Returns a list of categories which match the given \a categoryIds.  The manager will populate \a errorMap
    (the map of indices of the \a categoryIds list to an error) only with the indexes where the category could not
    be retrieved.

    The \c QLandmarkManager::error() function will only return \c QLandmarkManager::NoError if
    all categories were successfully retrieved.
 */
QList<QLandmarkCategory> QLandmarkManager::categories(const QList<QLandmarkCategoryId> &categoryIds,
                                                      QMap<int, QLandmarkManager::Error> *errorMap) const
{
    Q_D(const QLandmarkManager);

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return QList<QLandmarkCategory>();
    }

    QList<QLandmarkCategory> cats = d->engine->categories(categoryIds,
                                    errorMap,
                                    &(d->errorCode),
                                    &(d->errorString));

    return cats;
}

/*!
    Returns a list of categories.The \a limit defines the maximum number of categories
    to return and the \a offset defines the index offset of the first category.
    A \a limit of -1 means all categories should be returned.  The categories
    are returned in the order as designated by \a nameSort.
*/
QList<QLandmarkCategory> QLandmarkManager::categories(int limit, int offset, const QLandmarkNameSort &nameSort) const
{
    Q_D(const QLandmarkManager);

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return QList<QLandmarkCategory>();
    }

    QList<QLandmarkCategory> cats = d->engine->categories(limit, offset, nameSort,
                                    &(d->errorCode),
                                    &(d->errorString));

    if (d->errorCode != NoError)
        return QList<QLandmarkCategory>();

    return cats;
}

/*!
    Returns a list of category identifiers.
    The \a limit defines the maximum number of ids to return and the \a offset defines the index offset
    of the first id.  A \a limit of -1 means ids for all categories should be returned.
    The identifiers are returned in order as designed by \a nameSort.
*/
QList<QLandmarkCategoryId> QLandmarkManager::categoryIds(int limit, int offset, const QLandmarkNameSort &nameSort) const
{
    Q_D(const QLandmarkManager);

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return QList<QLandmarkCategoryId>();
    }

    QList<QLandmarkCategoryId> ids = d->engine->categoryIds(limit, offset, nameSort,
                                                            &(d->errorCode),
                                                            &(d->errorString));

    if (d->errorCode != NoError)
        return QList<QLandmarkCategoryId>();

    return ids;
}

/*!
    Returns the landmark in the database identified by \a landmarkId
*/
QLandmark QLandmarkManager::landmark(const QLandmarkId &landmarkId) const
{
    Q_D(const QLandmarkManager);

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return QLandmark();
    }

    QLandmark lm = d->engine->landmark(landmarkId,
                                       &(d->errorCode),
                                       &(d->errorString));

    if (d->errorCode != NoError)
        return QLandmark();

    return lm;
}

/*!
    Returns a list of landmarks which match the given \a filter and are sorted according to the \a sortOrders.
    The \a limit defines the maximum number of landmarks to return and the \a offset defines the index offset
    of the first landmark.  A \a limit of -1 means all matching landmarks should be returned.
*/
QList<QLandmark> QLandmarkManager::landmarks(const QLandmarkFilter &filter, int limit, int offset,
                                             const QList<QLandmarkSortOrder> &sortOrders) const
{
    Q_D(const QLandmarkManager);

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return QList<QLandmark>();
    }

    QList<QLandmark> lms = d->engine->landmarks(filter,
                           limit,
                           offset,
                           sortOrders,
                           &(d->errorCode),
                           &(d->errorString));

    if (d->errorCode != NoError)
        return QList<QLandmark>();

    return lms;
}

/*!
    Returns a list of landmarks which match the given \a filter and are sorted according to the \a sortOrder.
    The \a limit defines the maximum number of landmarks to return and the \a offset defines the index offset
    of the first landmark.  A \a limit of -1 means all matching landmarks should be returned.
*/
QList<QLandmark> QLandmarkManager::landmarks(const QLandmarkFilter &filter, int limit, int offset,
                                             const QLandmarkSortOrder &sortOrder) const
{
    Q_D(const QLandmarkManager);

     if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return QList<QLandmark>();
    }

    QList<QLandmarkSortOrder> sortOrders;
    if (sortOrder.type() != QLandmarkSortOrder::DefaultSort)
        sortOrders.append(sortOrder);

    QList<QLandmark> lms = d->engine->landmarks(filter,
                           limit,
                           offset,
                           sortOrders,
                           &(d->errorCode),
                           &(d->errorString));

    if (d->errorCode != NoError)
        return QList<QLandmark>();

    return lms;
}

/*!
    Returns a list of landmarks which match the given \a landmarkIds.  The manager will populate \a errorMap
    (the map of indices of the \a landmarkIds list an error) only with the indexes where the landmark could not
    be retrieved.

    The \c QLandmarkManager::error() function will only return \c QLandmarkManager::NoError if
    all landmarks were successfully retrieved.

*/
QList<QLandmark> QLandmarkManager::landmarks(const QList<QLandmarkId> &landmarkIds, QMap<int, QLandmarkManager::Error> *errorMap) const
{
    Q_D(const QLandmarkManager);

     if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return QList<QLandmark>();
    }

    QList<QLandmark> lms = d->engine->landmarks(landmarkIds,
                                                errorMap,
                                                &(d->errorCode),
                                                &(d->errorString));
    return lms;
}

/*!
    Returns a list of landmark identifiers which match the given \a filter and are sorted according to
    the given \a sortOrders. The \a limit defines the maximum number of landmark ids to return and the
    \a offset defines the index offset of the first landmark id.
    A \a limit of -1 means that ids of all matching landmarks should be returned.
*/
QList<QLandmarkId> QLandmarkManager::landmarkIds(const QLandmarkFilter &filter,
                                                int limit, int offset,
                                                 const QList<QLandmarkSortOrder> &sortOrders) const
{
    Q_D(const QLandmarkManager);

     if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return QList<QLandmarkId>();
    }

    QList<QLandmarkId> ids = d->engine->landmarkIds(filter,
                             limit,
                             offset,
                             sortOrders,
                             &(d->errorCode),
                             &(d->errorString));

    if (d->errorCode != NoError)
        return QList<QLandmarkId>();

    return ids;
}

/*!
    Returns a list of landmark identifiers which match the given \a filter and are sorted according to
    the given \a sortOrder. The \a limit defines the maximum number of landmark ids to return and the
    \a offset defines the index offset of the first landmark id.
    A \a limit of -1 means that ids of all matching landmarks should be returned.

    This is a convenience function.
*/
QList<QLandmarkId> QLandmarkManager::landmarkIds(const QLandmarkFilter &filter,
                                                 int limit, int offset,
                                                 const QLandmarkSortOrder &sortOrder) const
{
    Q_D(const QLandmarkManager);

     if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return QList<QLandmarkId>();
    }

    QList<QLandmarkSortOrder> sortOrders;
    sortOrders.append(sortOrder);

    QList<QLandmarkId> ids = d->engine->landmarkIds(filter,
                                                    limit,
                                                    offset,
                                                    sortOrders,
                                                    &(d->errorCode),
                                                    &(d->errorString));

    if (d->errorCode != NoError)
        return QList<QLandmarkId>();

    return ids;
}

/*!
    Reads landmarks from the given \a device and saves them.  The data from the \a device
    is expected to adhere to the provided \a format.  If no \a format is provided,
    the manager tries to auto detect the \a format.

    The \a option can be used to control whether categories in the imported
    file will be added during the import.  If the \c AttachSingleCategory option is used, then
    all the landmarks in the import file are assigned to the category identified by
    \a categoryId, in all other cirumstances \a categoryId is ignored.  If \a categoryId
    doesn't exist when using \c AttachSingleCategory, QLandmarkManager::DoesNotExist error is returned.  Note that
    some file formats may not support categories at all.

    Returns true if all landmarks could be imported, otherwise
    returns false.  It may be possible that only a subset of
    landmarks are imported depending upon the backed implementation.

    The current default managers for the maemo and desktop platforms
    support GPX version 1.1, and the format to use is \c GpxV1.1.
*/
bool QLandmarkManager::importLandmarks(QIODevice *device, const QString &format, QLandmarkManager::TransferOption option, const QLandmarkCategoryId &categoryId)
{
    Q_D(QLandmarkManager);

     if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return false;
    }

    return d->engine->importLandmarks(device,
                                      format,
                                      option,
                                      categoryId,
                                      &(d->errorCode),
                                      &(d->errorString));
}

/*!
    Convenience function that will read landmarks from \a fileName in
    the expected \a format. If no \a format is provided, the manager tries
    to auto detect the \a format.  Internally a QFile is opened with
    QIODevice::ReadOnly permissions.

    The \a option can be used to control whether categories in the imported
    file will be added during the import.  If the \c AttachSingleCategory option is used, then
    all the landmarks in the import file are assigned to the category identified by
    \a categoryId, in all other cirumstances \a categoryId is ignored.  If \a categoryId
    doesn't exist when using \c AttachSingleCategory, QLandmarkManager::DoesNotExist error is returned.  Note that
    some file formats may not support categories at all.

    Returns true if all landmarks could be imported, otherwise
    returns false.  It may be possible that only a subset of landmarks
    are imported.

    The current default managers for the maemo and desktop platforms
    support GPX version 1.1, and the format to use is \c GpxV1.1.
*/
bool QLandmarkManager::importLandmarks(const QString &fileName, const QString &format, QLandmarkManager::TransferOption option, const QLandmarkCategoryId &categoryId)
{
    QFile file(fileName);
    return importLandmarks(&file, format,option,categoryId);
}

/*!
    Writes landmarks to the given \a device.  The landmarks will be written
    according to the specified \a format.  If  \a landmarkIds is empty, then
    all landmarks will be exported, otherwise only those landmarks that
    match \a landmarkIds will be exported.

    The \a option can be used to control whether categories will be exported or not.
    Note that the \c AttachSingleCategory option has no meaning during
    export and the manager will export as if \a option was \c IncludeCategoryData.
    Also, be aware that some file formats may not support categories at all and for
    these formats, the \a option is always treated as if it was \c ExcludeCategoryData.

    Returns true if all specified landmarks were successfully exported,
    otherwise returns false.  It may be possible that only a subset
    of landmarks are exported.
*/
bool QLandmarkManager::exportLandmarks(QIODevice *device, const QString &format, QList<QLandmarkId> landmarkIds, QLandmarkManager::TransferOption option) const
{
    Q_D(const QLandmarkManager);

     if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return false;
    }

    return d->engine->exportLandmarks(device,
                                      format,
                                      landmarkIds,
                                      option,
                                      &(d->errorCode),
                                      &(d->errorString));
}

/*!
    Returns the file formats supported for the given transfer \a operation. ie import or export.
*/
QStringList QLandmarkManager::supportedFormats(QLandmarkManager::TransferOperation operation) const
{
    Q_D(const QLandmarkManager);

     if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return QStringList();
    }

    return d->engine->supportedFormats(operation, &(d->errorCode), &(d->errorString));
}

/*!
    Convenience function that will write landmarks to \a fileName in the expected
    \a format. Internally a QFile is opened with QIODevice::WriteOnly permissions.
    If \a landmarkIds is empty, then all landmarks will be exported, otherwise
    only those landmarks that match \a landmarkIds will be exported.

    The \a option can be used to control whether categories will be exported or not.
    Note that the \c AttachSingleCategory option has no meaning during
    export and the manager will export as if \a option was \c IncludeCategoryData.
    Also, be aware that some file formats may not support categories at all and for
    these formats, the \a option is always treated as if it was \c ExcludeCategoryData.

    Returns true if all specified landmarks were successfully exported,
    otherwise returns false.  It may be possible that only a subset
    of landmarks are exported.
*/
bool QLandmarkManager::exportLandmarks(const QString &fileName, const QString &format, QList<QLandmarkId> landmarkIds, QLandmarkManager::TransferOption option) const
{
    QFile file(fileName);

    return exportLandmarks(&file, format,landmarkIds, option);
}

/*!
    Returns the error code of the most recent operation.  All functions will modify the error based on whether the
    operation successfully or not.
*/
QLandmarkManager::Error QLandmarkManager::error() const
{
    Q_D(const QLandmarkManager);
    return d->errorCode;
}

/*!
    Returns a short human-readable description of the error that occurred
    in the most recent operation.
*/
QString QLandmarkManager::errorString() const
{
    Q_D(const QLandmarkManager);
    return d->errorString;
}

/*!
    Returns whether the manager supports the given \a feature.
*/
bool QLandmarkManager::isFeatureSupported(QLandmarkManager::LandmarkFeature feature) const
{
    Q_D(const QLandmarkManager);

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return false;
    }

    return d->engine->isFeatureSupported(feature, &(d->errorCode), &(d->errorString));
}

/*!
    Returns the support level the manager provides for the given \a filter.
*/
QLandmarkManager::SupportLevel QLandmarkManager::filterSupportLevel(const QLandmarkFilter &filter) const
{
    Q_D(const QLandmarkManager);

     if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return QLandmarkManager::None;
    }

    return d->engine->filterSupportLevel(filter, &(d->errorCode), &(d->errorString));
}

/*!
    Returns the support level the manager provides for the given list of \a sortOrders.
*/
QLandmarkManager::SupportLevel QLandmarkManager::sortOrderSupportLevel(const QList<QLandmarkSortOrder>& sortOrders) const
{
    Q_D(const QLandmarkManager);

     if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return QLandmarkManager::None;
    }

    return d->engine->sortOrderSupportLevel(sortOrders, &(d->errorCode), &(d->errorString));
}

/*!
    Returns true if the manager is entirely read-only.  Meaning
    landmarks and categories cannot be added, modified or removed.
*/
bool QLandmarkManager::isReadOnly() const
{
    Q_D(const QLandmarkManager);
    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return true;
    }

    return d->engine->isReadOnly(&(d->errorCode), &(d->errorString));
}

/*!
    Returns true if the landmark identified by \a landmarkId
    considered read-only by the manager.

    If the \a landmarkId does not refer to an existing landmark,
    it is considered writable unless the manager is exclusively read-only.
*/
bool QLandmarkManager::isReadOnly(const QLandmarkId &landmarkId) const
{
    Q_D(const QLandmarkManager);
    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return true;
    }

    return d->engine->isReadOnly(landmarkId, &(d->errorCode), &(d->errorString));
}

/*!
    Returns true if the category identified by \a categoryId is
    considered read-only by the manager.

    If \a categoryId does not refer to an existing category,
    it is considered writable unless the manager is exclusively read-only.
*/
bool QLandmarkManager::isReadOnly(const QLandmarkCategoryId &categoryId) const
{
    Q_D(const QLandmarkManager);
    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return true;
    }

    return d->engine->isReadOnly(categoryId, &(d->errorCode), &(d->errorString));
}

/*!
    Returns the list of attribute keys the landmarks will have.
    If extended attributes are enabled (provided manager supported them)
    landmarks will possess  extra keys in addition to the standard cross platform keys.

    Note: When saving a landark with extended attributes, all attributes much match
    those of the manager otherwise a QLandmarkManager::BadArgumentError is set.
    If the landmark does not have one of the extended attributes in its list,
    then that particular attribute is ignored.  This differs from the semantics
    for custom attributes where if a custom attribute is not present then it
    is removed when the landmark is saved.
*/
QStringList QLandmarkManager::landmarkAttributeKeys() const
{
    Q_D(const QLandmarkManager);

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
    }

    return d->engine->landmarkAttributeKeys(&(d->errorCode), &(d->errorString));
}

/*!
    Returns the list of attribute keys the categories will have.
    If extended attributes are enabled (provided the manager supports them),
    categories will possess extra keys in addition to the standad cross platform keys.
*/
QStringList QLandmarkManager::categoryAttributeKeys() const
{
    Q_D(const QLandmarkManager);

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
    }
    return  d->engine->categoryAttributeKeys(&(d->errorCode), &(d->errorString));
}

/*!
    Returns whether extended attributes specific to this manager are enabled or not.
    If extended attributes are enabled, retrieved landmarks will have
    extra attribute keys accessible through the QLandmark::attribute() function.
    Extended attributes must be enabled to save any landmarks which possess
    extended attributes.  This same behaviour will also apply to categories
    if extended category attributes are supported.
*/
bool QLandmarkManager::isExtendedAttributesEnabled() const
{
    Q_D(const QLandmarkManager);

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
    }
    return  d->engine->isExtendedAttributesEnabled(&(d->errorCode), &(d->errorString));
}

/*!
    Sets whether extended attributes are \a enabled or not
*/
void QLandmarkManager::setExtendedAttributesEnabled(bool enabled)
{
    Q_D(QLandmarkManager);

   if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
    }

    d->engine->setExtendedAttributesEnabled(enabled, &(d->errorCode), &(d->errorString));
}

/*!
    Returns whether custom attributes are enabled or not. Custom attributes
    are arbitrary attributes created by the application for a landmark or category.
    If custom attributes are enabled (and the manager supports them),
    retrieved landmarks and categories will have extra attributes accessible
    using QLandmark::customAttributes().  Custom attributes must be enabled
    to save any landmarks with possess custom attributes.  This same behaviour
    applies to categories if custom category attributes are supported.
*/
bool QLandmarkManager::isCustomAttributesEnabled() const
{
    Q_D(const QLandmarkManager);

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
    }

    return d->engine->isCustomAttributesEnabled(&(d->errorCode), &(d->errorString));
}

/*!
    Sets whether custom attributes are \a enabled or not.
*/
void QLandmarkManager::setCustomAttributesEnabled(bool enabled)
{
    Q_D(QLandmarkManager);

        if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
    }

    d->engine->setCustomAttributesEnabled(enabled, &(d->errorCode), &(d->errorString));
}

/*!
    Returns the manager name for this QLandmarkManager.

    The manager name usually takes the format of a reverse domain string.  An example
    of a manager name is \c com.nokia.qt.landmarks.engines.sqlite
*/
QString QLandmarkManager::managerName() const
{
    Q_D(const QLandmarkManager);

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return QString();
    }

    d->errorCode = QLandmarkManager::NoError;
    d->errorString = "";
    return d->engine->managerName();
}

/*!
    Return the parameters relevant to the creation of this QLandmarkManager.

    The parameters may be viewed as a set of key-value pairs.  Each manager
    may have a different set of parameters depending upon its backend implementation.
*/
QMap<QString, QString> QLandmarkManager::managerParameters() const
{
    Q_D(const QLandmarkManager);

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return QMap<QString, QString>();
    }

    d->errorCode = QLandmarkManager::NoError;
    d->errorString = "";
    return d->engine->managerParameters();
}

/*!
  Return the uri describing this QLandmarkManager, consisting of the manager name and any parameters.
 */
QString QLandmarkManager::managerUri() const
{
    Q_D(const QLandmarkManager);

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return QString();
    }

    d->errorCode = QLandmarkManager::NoError;
    d->errorString = "";
    return d->engine->managerUri();
}

/*!
  Returns the engine backend implementation version number.
*/
int QLandmarkManager::managerVersion() const
{
    Q_D(const QLandmarkManager);

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return 0;
    }

    d->errorCode = QLandmarkManager::NoError;
    d->errorString = "";
    return d->engine->managerVersion();
}

/*!
    Returns a list of available manager names that can
    be used when constructing a QLandmarkManager
*/
QStringList QLandmarkManager::availableManagers()
{
    return QLandmarkManagerPrivate::factories().keys();
}

/*! Returns a URI that completely describes a manager implementation, datastore,
    and the parameters with which to instantiate the manager,
    from the given \a managerName, \a params and an optional \a implementationVersion */
QString QLandmarkManager::buildUri(const QString& managerName, const QMap<QString, QString>& params, int implementationVersion)
{
    QString ret(QLatin1String("qtlandmarks:%1:%2"));
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
        QString versionString = QString(QLatin1String(QTLANDMARKS_IMPLEMENTATION_VERSION_NAME));
        versionString += QString::fromAscii("=");
        versionString += QString::number(implementationVersion);
        escapedParams.append(versionString);
    }

    return ret.arg(managerName, escapedParams.join(QLatin1String("&")));
}

/*!
  Constructs a QLandmarkManager whose implementation, store and parameters are specified in the given \a storeUri,
  and whose parent object is \a parent.
 */
QLandmarkManager* QLandmarkManager::fromUri(const QString& storeUri, QObject* parent)
{
    if (storeUri.isEmpty()) {
        return new QLandmarkManager(QString(), QMap<QString, QString>(), parent);
    } else {
        QString id;
        QMap<QString, QString> parameters;
        if (parseUri(storeUri, &id, &parameters)) {
            return new QLandmarkManager(id, parameters, parent);
        } else {
            // invalid
            return NULL;
        }
    }
}

/*!
  Splits the given \a uri into the manager, store, and parameters that it describes, and places the information
  into the memory addressed by \a pManagerId and \a pParams respectively.  Returns true if \a uri could be split successfully,
  otherwise returns false
 */
bool QLandmarkManager::parseUri(const QString& uri, QString* pManagerId, QMap<QString, QString>* pParams)
{
    // Format: qtlandmarks:<managerid>:<key>=<value>&<key>=<value>
    // - it is assumed the prefix(qtlandmarks) and managerid cannot contain ':'
    // - it is assumed keys and values do not contain '=' or '&'
    //   but can contain &amp; and &equ;
    QStringList colonSplit = uri.split(QLatin1Char(':'));
    QString prefix = colonSplit.value(0);

    if (prefix != QLatin1String("qtlandmarks"))
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

/*!
    \internal
*/
void QLandmarkManager::connectNotify(const char *signal)
{
    if (d_ptr->engine) {
        if (QLatin1String(signal) == SIGNAL(landmarksAdded(QList<QLandmarkId>))) {
            connect(d_ptr->engine,SIGNAL(landmarksAdded(QList<QLandmarkId>)),
                    this, SIGNAL(landmarksAdded(QList<QLandmarkId>)));
        } else if (QLatin1String(signal) == SIGNAL(landmarksChanged(QList<QLandmarkId>))) {
            connect(d_ptr->engine,SIGNAL(landmarksChanged(QList<QLandmarkId>)),
                    this, SIGNAL(landmarksChanged(QList<QLandmarkId>)));
        } else if (QLatin1String(signal) == SIGNAL(landmarksRemoved(QList<QLandmarkId>))) {
            connect(d_ptr->engine,SIGNAL(landmarksRemoved(QList<QLandmarkId>)),
                    this, SIGNAL(landmarksRemoved(QList<QLandmarkId>)));
        } else if (QLatin1String(signal) == SIGNAL(categoriesAdded(QList<QLandmarkCategoryId>))) {
            connect(d_ptr->engine,SIGNAL(categoriesAdded(QList<QLandmarkCategoryId>)),
                    this, SIGNAL(categoriesAdded(QList<QLandmarkCategoryId>)));
        } else if (QLatin1String(signal) == SIGNAL(categoriesChanged(QList<QLandmarkCategoryId>))) {
            connect(d_ptr->engine,SIGNAL(categoriesChanged(QList<QLandmarkCategoryId>)),
                    this, SIGNAL(categoriesChanged(QList<QLandmarkCategoryId>)));
        } else if (QLatin1String(signal) == SIGNAL(categoriesRemoved(QList<QLandmarkCategoryId>))) {
            connect(d_ptr->engine,SIGNAL(categoriesRemoved(QList<QLandmarkCategoryId>)),
                    this, SIGNAL(categoriesRemoved(QList<QLandmarkCategoryId>)));
        }
    }
    QObject::connectNotify(signal);
}

/*!
    \internal
*/
void QLandmarkManager::disconnectNotify(const char *signal)
{
    if (d_ptr->engine) {
        if (QLatin1String(signal) == SIGNAL(landmarksAdded(QList<QLandmarkId>))) {
            disconnect(d_ptr->engine,SIGNAL(landmarksAdded(QList<QLandmarkId>)),
                    this, SIGNAL(landmarksAdded(QList<QLandmarkId>)));
        } else if (QLatin1String(signal) == SIGNAL(landmarksChanged(QList<QLandmarkId>))) {
            disconnect(d_ptr->engine,SIGNAL(landmarksChanged(QList<QLandmarkId>)),
                    this, SIGNAL(landmarksChanged(QList<QLandmarkId>)));
        } else if (QLatin1String(signal) == SIGNAL(landmarksRemoved(QList<QLandmarkId>))) {
            disconnect(d_ptr->engine,SIGNAL(landmarksRemoved(QList<QLandmarkId>)),
                    this, SIGNAL(landmarksRemoved(QList<QLandmarkId>)));
        } else if (QLatin1String(signal) == SIGNAL(categoriesAdded(QList<QLandmarkCategoryId>))) {
            disconnect(d_ptr->engine,SIGNAL(categoriesAdded(QList<QLandmarkCategoryId>)),
                    this, SIGNAL(categoriesAdded(QList<QLandmarkCategoryId>)));
        } else if (QLatin1String(signal) == SIGNAL(categoriesChanged(QList<QLandmarkCategoryId>))) {
            disconnect(d_ptr->engine,SIGNAL(categoriesChanged(QList<QLandmarkCategoryId>)),
                    this, SIGNAL(categoriesChanged(QList<QLandmarkCategoryId>)));
        } else if (QLatin1String(signal) == SIGNAL(categoriesRemoved(QList<QLandmarkCategoryId>))) {
            disconnect(d_ptr->engine,SIGNAL(categoriesRemoved(QList<QLandmarkCategoryId>)),
                    this, SIGNAL(categoriesRemoved(QList<QLandmarkCategoryId>)));
        }
    }
    QObject::disconnectNotify(signal);
}

QLandmarkManagerEngine *QLandmarkManager::engine()
{
    return d_ptr->engine;
}

/*!
    \fn QLandmarkManager::dataChanged()
    This signal is emitted by the manager if its internal state changes and it is unable to precisely determine
    the changes which occurred, or if the manager considers the changes to be radical enough to require clients to reload
    all data.  If the signal is emitted, no other signals will be emitted for the associated changes.
*/

/*!
    \fn void QLandmarkManager::landmarksAdded(const QList<QLandmarkId> &landmarkIds)

    This signal is emitted when landmarks (identified by \a landmarkIds) have been added to the datastore managed by this manager.
    This signal is not emitted if the dataChanged() signal was previously emitted for these changes.
    \sa landmarksChanged(), landmarksRemoved()
*/

/*!
    \fn void QLandmarkManager::landmarksChanged(const QList<QLandmarkId> &landmarkIds)

    This signal is emitted when landmarks (identified by \a landmarkIds) have been modified in the datastore managed by this manager.
    This signal is not emitted if the dataChanged() signal was previously emitted for these changes.
    \sa landmarksAdded(), landmarksRemoved()
*/

/*!
    \fn void QLandmarkManager::landmarksRemoved(const QList<QLandmarkId> &landmarkIds)

    This signal is emitted when landmarks (identified by \a landmarkIds) have been removed from the datastore managed by this manager.
    This signal is not emitted if the dataChanged() signal was previously emitted for these changes.
    \sa landmarksAdded(), landmarksChanged()
*/

/*!
    \fn void QLandmarkManager::categoriesAdded(const QList<QLandmarkCategoryId> &categoryIds)

    This signal is emitted when categories (identified by \a categoryIds) have been added to the datastore managed by this manager.
    This signal is not emitted if the dataChanged() signal was previously emitted for these changes.
    \sa categoriesChanged(), categoriesRemoved()
*/

/*!
    \fn void QLandmarkManager::categoriesChanged(const QList<QLandmarkCategoryId> &categoryIds)

    This signal is emitted when categories (identified by \a categoryIds) have been modified in the datastore managed by this manager.
    This signal is not emitted if the dataChanged() signal was previously emitted for these changes.
    \sa categoriesAdded(), categoriesRemoved()
*/

/*!
    \fn void QLandmarkManager::categoriesRemoved(const QList<QLandmarkCategoryId> &categoryIds)

    This signal is emitted when categories (identified by \a categoryIds) have been removed from the datastore managed by this manager.
    This signal is not emitted if the dataChanged() signal was previously emitted for these changes.
    \sa categoriesAdded(), categoriesChanged()
*/

#include "moc_qlandmarkmanager.cpp"
