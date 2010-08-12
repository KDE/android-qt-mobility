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

#include "qorganizeritemmanagerengine.h"

#include "qorganizeritemdetaildefinition.h"
#include "qorganizeritemdetailfielddefinition.h"
#include "qorganizeritemdetails.h"
#include "qorganizeritemsortorder.h"
#include "qorganizeritemfilters.h"
#include "qorganizeritemabstractrequest.h"
#include "qorganizeritemabstractrequest_p.h"
#include "qorganizeritemrequests.h"
#include "qorganizeritemrequests_p.h"
#include "qorganizeritem.h"
#include "qorganizeritemfetchhint.h"

#include "qorganizeritem_p.h"
#include "qorganizeritemdetail_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerItemManagerEngine
  \brief The QOrganizerItemManagerEngine class provides the interface for all
  implementations of the organizer item manager backend functionality.

  \inmodule QtOrganizer
  \ingroup organizer-backends

  Instances of this class are usually provided by a
  \l QOrganizerItemManagerEngineFactory, which is loaded from a plugin.

  The default implementation of this interface provides a basic
  level of functionality for some functions so that specific engines
  can simply implement the functionality that is supported by
  the specific organizer items engine that is being adapted.

  More information on writing a organizer items engine plugin is available in
  the \l{Qt Organizer Manager Engines} documentation.

  \sa QOrganizerItemManager, QOrganizerItemManagerEngineFactory
 */

/*!
  \fn QOrganizerItemManagerEngine::QOrganizerItemManagerEngine()

  A default, empty constructor.
 */

/*!
  \fn QOrganizerItemManagerEngine::dataChanged()

  This signal is emitted some time after changes occur to the data managed by this
  engine, and the engine is unable to determine which changes occurred, or if the
  engine considers the changes to be radical enough to require clients to reload all data.

  If this signal is emitted, no other signals may be emitted for the associated changes.

  As it is possible that other processes (or other devices) may have caused the
  changes, the timing can not be determined.

  \sa itemsAdded(), itemsChanged(), itemsRemoved()
 */

/*!
  \fn QOrganizerItemManagerEngine::itemsAdded(const QList<QOrganizerItemLocalId>& organizeritemIds);

  This signal is emitted some time after a set of organizer items has been added to
  this engine where the \l dataChanged() signal was not emitted for those changes.
  As it is possible that other processes (or other devices) may
  have added the organizer items, the timing cannot be determined.

  The list of ids of organizer items added is given by \a organizeritemIds.  There may be one or more
  ids in the list.

  \sa dataChanged()
 */

/*!
  \fn QOrganizerItemManagerEngine::itemsChanged(const QList<QOrganizerItemLocalId>& organizeritemIds);

  This signal is emitted some time after a set of organizer items has been modified in
  this engine where the \l dataChanged() signal was not emitted for those changes.
  As it is possible that other processes (or other devices) may
  have modified the organizer items, the timing cannot be determined.

  The list of ids of changed organizer items is given by \a organizeritemIds.  There may be one or more
  ids in the list.

  \sa dataChanged()
 */

/*!
  \fn QOrganizerItemManagerEngine::itemsRemoved(const QList<QOrganizerItemLocalId>& organizeritemIds);

  This signal is emitted some time after a set of organizer items has been removed from
  this engine where the \l dataChanged() signal was not emitted for those changes.
  As it is possible that other processes (or other devices) may
  have removed the organizer items, the timing cannot be determined.

  The list of ids of removed organizer items is given by \a organizeritemIds.  There may be one or more
  ids in the list.

  \sa dataChanged()
 */

/*! Returns the manager name for this QOrganizerItemManagerEngine */
QString QOrganizerItemManagerEngine::managerName() const
{
    return QString(QLatin1String("base"));
}

/*!
  Returns the parameters with which this engine was constructed.  Note that
  the engine may have discarded unused or invalid parameters at the time of
  construction, and these will not be returned.
 */
QMap<QString, QString> QOrganizerItemManagerEngine::managerParameters() const
{
    return QMap<QString, QString>(); // default implementation requires no parameters.
}

/*!
  Returns the unique URI of this manager, which is built from the manager name and the parameters
  used to construct it.
 */
QString QOrganizerItemManagerEngine::managerUri() const
{
    return QOrganizerItemManager::buildUri(managerName(), managerParameters());
}

/*!
  Return the list of organizer item instances which match the given \a filter, sorted according to
  the given \a sortOrders.  The client may instruct the manager that it does not require all
  possible information about each instance by specifying a fetch hint \a fetchHint; the manager can
  choose to ignore the fetch hint, but if it does so, it must return all possible information about
  each instance.

  Items of type QOrganizerItemType::TypeEvent or QOrganizerItemType::TypeTodo should NOT be included
  in the list of returned items.  Instead, these items should be expanded according to their
  recurrence specification and the resultant occurrence items (of type
  QOrganizerItemType::TypeEventOccurrence and QOrganizerItemType::TypeTodoOccurrence) should be
  returned.

  The occurrence-typed returned items should have a QOrganizerItemInstanceOrigin detail that points
  to the generator and the original instance that the event would have occurred on (if it is an
  exception).  No returned item should contain a QOrganizerItemRecurrence detail.

  If there are no instances matching the criteria, an empty list should be returned.
  Any error which occurs should be saved to \a error.
  */
QList<QOrganizerItem> QOrganizerItemManagerEngine::itemInstances(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const
{
    Q_UNUSED(filter);
    Q_UNUSED(sortOrders);
    Q_UNUSED(fetchHint);

    *error = QOrganizerItemManager::NotSupportedError;
    return QList<QOrganizerItem>();
}


/*!
  Return the list of a maximum of \a maxCount organizer item instances which are occurrences of the
  given \a generator recurring item, which occur between the given \a periodStart date and the given
  \a periodEnd date.

  If \a periodStart is after \a periodEnd, the operation will fail, and \a error will be set to \c
  QOrganizerItemManager::BadArgumentError.
  If \a maxCount is negative, it is backend specific as to how many occurrences will be returned.
  Some backends may return no instances, others may return some limited number of occurrences.

  If the \a generator is an item of type QOrganizerItemType::TypeEvent, a list of items of type
  QOrganizerItemType::TypeEventOccurrence will be returned, representing the expansion of the
  generator according to its QOrganizerItemRecurrence detail.  Similarly, a \a generator of type
  QOrganizerItemType::TypeTodo will result in a list of QOrganizerItemType::TypeTodoOccurrence
  items.  If the \a generator is of any other type, it is returned by itself from the backend.

  The occurrence-typed items returned should have a QOrganizerItemInstanceOrigin detail that refers
  to the generator and the original instance that the event would have occurred on (if it is an
  exception).  No returned item should contain a QOrganizerItemRecurrence detail.

  If the \a generator does not exist in the backend, or if there are no instances matching the
  criteria, an empty list should be returned.
  */
QList<QOrganizerItem> QOrganizerItemManagerEngine::itemInstances(const QOrganizerItem& generator, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount, QOrganizerItemManager::Error* error) const
{
    Q_UNUSED(generator);
    Q_UNUSED(periodStart);
    Q_UNUSED(periodEnd);
    Q_UNUSED(maxCount);

    *error = QOrganizerItemManager::NotSupportedError;
    return QList<QOrganizerItem>();
}

/*!
  Returns a list of organizer item ids that match the given \a filter, sorted according to the given
  list of \a sortOrders.  Depending on the backend, this filtering operation may involve retrieving
  all the organizer items.  Any error which occurs will be saved in \a error.
 */
QList<QOrganizerItemLocalId> QOrganizerItemManagerEngine::itemIds(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, QOrganizerItemManager::Error* error) const
{
    Q_UNUSED(filter);
    Q_UNUSED(sortOrders);

    *error = QOrganizerItemManager::NotSupportedError;
    return QList<QOrganizerItemLocalId>();
}

/*!
  Returns the list of organizer items which match the given \a filter stored in the manager sorted according to the given list of \a sortOrders.

  Any operation error which occurs will be saved in \a error.

  The \a fetchHint parameter describes the optimization hints that a manager may take.
  If the \a fetchHint is the default constructed hint, all existing details and relationships
  in the matching organizer items will be returned.  A client should not make changes to an item which has
  been retrieved using a fetch hint other than the default fetch hint.  Doing so will result in information
  loss when saving the item back to the manager (as the "new" restricted item will
  replace the previously saved item in the backend).

  Items of type EventOccurrence and TodoOccurrence should only be returned when they represent an
  exceptional occurrence; ie. if the client has specifically saved the item occurrence in the
  manager.  Occurrence-typed items that are generated purely from a recurrence specification of
  another detail should not be returned in this list.

  All items returned should have a non-zero local ID.

  \sa QOrganizerItemFetchHint
 */
QList<QOrganizerItem> QOrganizerItemManagerEngine::items(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const
{
    Q_UNUSED(filter);
    Q_UNUSED(sortOrders);
    Q_UNUSED(fetchHint);
    *error = QOrganizerItemManager::NotSupportedError;
    return QList<QOrganizerItem>();
}

/*!
  Returns the organizer item in the database identified by \a organizeritemId.

  If the item does not exist, an empty, default constructed QOrganizerItem will be returned,
  and the \a error will be set to  \c QOrganizerItemManager::DoesNotExistError.

  Any operation error which occurs will be saved in \a error.

  The \a fetchHint parameter describes the optimization hints that a manager may take.
  If the \a fetchHint is the default constructed hint, all existing details and relationships
  in the matching item will be returned.  A client should not make changes to an item which has
  been retrieved using a fetch hint other than the default fetch hint.  Doing so will result in information
  loss when saving the item back to the manager (as the "new" restricted item will
  replace the previously saved item in the backend).

  \sa QOrganizerItemFetchHint
 */
QOrganizerItem QOrganizerItemManagerEngine::item(const QOrganizerItemLocalId& organizeritemId, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const
{
    Q_UNUSED(organizeritemId);
    Q_UNUSED(fetchHint);
    *error = QOrganizerItemManager::NotSupportedError;
    return QOrganizerItem();
}

/*!
  Returns true if the given \a feature is supported by this engine for organizer items of the given \a organizeritemType
 */
bool QOrganizerItemManagerEngine::hasFeature(QOrganizerItemManager::ManagerFeature feature, const QString& organizeritemType) const
{
    Q_UNUSED(feature);
    Q_UNUSED(organizeritemType);

    return false;
}

/*!
  Given an input \a filter, returns the canonical version of the filter.

  Some of the following transformations may be applied:
  \list
   \o Any QOrganizerItemInvalidFilters contained in a union filter will be removed
   \o Any default QOrganizerItemFilters contained in an intersection filter will be removed
   \o Any QOrganizerItemIntersectionFilters with a QOrganizerItemInvalidFilter contained will be
     replaced with a QOrganizerItemInvalidFilter
   \o Any QOrganizerItemUnionFilters with a default QOrganizerItemFilter contained will be replaced
     with a default QOrganizerItemFilter
   \o An empty QOrganizerItemIntersectionFilter will be replaced with a QOrganizerItemDefaultFilter
   \o An empty QOrganizerItemUnionFilter will be replaced with a QOrganizerItemInvalidFilter
   \o An empty QOrganizerItemLocalIdFilter will be replaced with a QOrganizerItemInvalidFilter
   \o An intersection or union filter with a single entry will be replaced by that entry
   \o A QOrganizerItemDetailFilter or QOrganizerItemDetailRangeFilter with no definition name will be replaced with a QOrganizerItemInvalidFilter
   \o A QOrganizerItemDetailRangeFilter with no range specified will be converted to a QOrganizerItemDetailFilter
  \endlist
*/
QOrganizerItemFilter QOrganizerItemManagerEngine::canonicalizedFilter(const QOrganizerItemFilter &filter)
{
    switch(filter.type()) {
        case QOrganizerItemFilter::IntersectionFilter:
        {
            QOrganizerItemIntersectionFilter f(filter);
            QList<QOrganizerItemFilter> filters = f.filters();
            QList<QOrganizerItemFilter>::iterator it = filters.begin();

            // XXX in theory we can remove duplicates in a set filter
            while (it != filters.end()) {
                QOrganizerItemFilter canon = canonicalizedFilter(*it);
                if (canon.type() == QOrganizerItemFilter::DefaultFilter) {
                    it = filters.erase(it);
                } else if (canon.type() == QOrganizerItemFilter::InvalidFilter) {
                    return QOrganizerItemInvalidFilter();
                } else {
                    *it = canon;
                    ++it;
                }
            }

            if (filters.count() == 0)
                return QOrganizerItemFilter();
            if (filters.count() == 1)
                return filters.first();

            f.setFilters(filters);
            return f;
        }
        // unreachable

        case QOrganizerItemFilter::UnionFilter:
        {
            QOrganizerItemUnionFilter f(filter);
            QList<QOrganizerItemFilter> filters = f.filters();
            QList<QOrganizerItemFilter>::iterator it = filters.begin();

            // XXX in theory we can remove duplicates in a set filter
            while (it != filters.end()) {
                QOrganizerItemFilter canon = canonicalizedFilter(*it);
                if (canon.type() == QOrganizerItemFilter::InvalidFilter) {
                    it = filters.erase(it);
                } else if (canon.type() == QOrganizerItemFilter::DefaultFilter) {
                    return QOrganizerItemFilter();
                } else {
                    *it = canon;
                    ++it;
                }
            }

            if (filters.count() == 0)
                return QOrganizerItemInvalidFilter();
            if (filters.count() == 1)
                return filters.first();

            f.setFilters(filters);
            return f;
        }
        // unreachable

        case QOrganizerItemFilter::LocalIdFilter:
        {
            QOrganizerItemLocalIdFilter f(filter);
            if (f.ids().count() == 0)
                return QOrganizerItemInvalidFilter();
        }
        break; // fall through to return at end

        case QOrganizerItemFilter::OrganizerItemDetailRangeFilter:
        {
            QOrganizerItemDetailRangeFilter f(filter);
            if (f.detailDefinitionName().isEmpty())
                return QOrganizerItemInvalidFilter();
            if (f.minValue() == f.maxValue()
                && f.rangeFlags() == (QOrganizerItemDetailRangeFilter::ExcludeLower | QOrganizerItemDetailRangeFilter::ExcludeUpper))
                return QOrganizerItemInvalidFilter();
            if ((f.minValue().isNull() && f.maxValue().isNull()) || (f.minValue() == f.maxValue())) {
                QOrganizerItemDetailFilter df;
                df.setDetailDefinitionName(f.detailDefinitionName(), f.detailFieldName());
                df.setMatchFlags(f.matchFlags());
                df.setValue(f.minValue());
                return df;
            }
        }
        break; // fall through to return at end

        case QOrganizerItemFilter::OrganizerItemDetailFilter:
        {
            QOrganizerItemDetailFilter f(filter);
            if (f.detailDefinitionName().isEmpty())
                return QOrganizerItemInvalidFilter();
        }
        break; // fall through to return at end

        default:
            break; // fall through to return at end
    }
    return filter;
}


/*!
  Returns a whether the supplied \a filter can be implemented
  natively by this engine.  If not, the base class implementation
  will emulate the functionality.
 */
bool QOrganizerItemManagerEngine::isFilterSupported(const QOrganizerItemFilter& filter) const
{
    Q_UNUSED(filter);

    return false;
}

/*!
  Returns the list of data types supported by this engine.
 */
QList<QVariant::Type> QOrganizerItemManagerEngine::supportedDataTypes() const
{
    return QList<QVariant::Type>();
}

/*!
  Returns the list of item types which are supported by this engine.
  This is a convenience function, equivalent to retrieving the allowable values
  for the \c QOrganizerItemType::FieldType field of the QOrganizerItemType definition
  which is valid in this engine.
 */
QStringList QOrganizerItemManagerEngine::supportedItemTypes() const
{
    QOrganizerItemManager::Error error;
    // XXX TODO: ensure that the TYPE field value for EVERY SINGLE TYPE contains all possible types...
    QList<QVariant> allowableVals = detailDefinition(QOrganizerItemType::DefinitionName, QOrganizerItemType::TypeNote, &error).fields().value(QOrganizerItemType::FieldType).allowableValues();
    QStringList retn;
    for (int i = 0; i < allowableVals.size(); i++)
        retn += allowableVals.at(i).toString();
    return retn;
}

/*!
  \fn int QOrganizerItemManagerEngine::managerVersion() const

  Returns the engine backend implementation version number
 */

/*! Returns the base schema definitions */
QMap<QString, QMap<QString, QOrganizerItemDetailDefinition> > QOrganizerItemManagerEngine::schemaDefinitions()
{
    // This implementation provides the base schema.
    // The schema documentation (organizeritemsschema.qdoc)
    // MUST BE KEPT UP TO DATE as definitions are added here.

    // the map we will eventually return
    QMap<QString, QMap<QString, QOrganizerItemDetailDefinition> > retnSchema;
    QMap<QString, QOrganizerItemDetailDefinition> retn; // each type has a different schema.

    // local variables for reuse
    QMap<QString, QOrganizerItemDetailFieldDefinition> fields;
    QOrganizerItemDetailFieldDefinition f;
    QOrganizerItemDetailDefinition d;

    // build the schema for the NOTEs  =============================
    retn.clear();

    // type
    d.setName(QOrganizerItemType::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList()
                         << QString(QLatin1String(QOrganizerItemType::TypeEvent))
                         << QString(QLatin1String(QOrganizerItemType::TypeEventOccurrence))
                         << QString(QLatin1String(QOrganizerItemType::TypeJournal))
                         << QString(QLatin1String(QOrganizerItemType::TypeNote))
                         << QString(QLatin1String(QOrganizerItemType::TypeTodo))
                         << QString(QLatin1String(QOrganizerItemType::TypeTodoOccurrence)));
    fields.insert(QOrganizerItemType::FieldType, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // guid
    d.setName(QOrganizerItemGuid::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemGuid::FieldGuid, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // timestamp
    d.setName(QOrganizerItemTimestamp::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::DateTime);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemTimestamp::FieldModificationTimestamp, f);
    fields.insert(QOrganizerItemTimestamp::FieldCreationTimestamp, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // display label
    d.setName(QOrganizerItemDisplayLabel::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemDisplayLabel::FieldLabel, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // description
    d.setName(QOrganizerItemDescription::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemDescription::FieldDescription, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // comment
    d.setName(QOrganizerItemComment::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemComment::FieldComment, f);
    d.setFields(fields);
    d.setUnique(false);
    retn.insert(d.name(), d);

    retnSchema.insert(QOrganizerItemType::TypeNote, retn);

    // and then again for EVENTs =============================
    retn.clear();

    // type
    d.setName(QOrganizerItemType::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList()
                         << QString(QLatin1String(QOrganizerItemType::TypeEvent))
                         << QString(QLatin1String(QOrganizerItemType::TypeEventOccurrence))
                         << QString(QLatin1String(QOrganizerItemType::TypeJournal))
                         << QString(QLatin1String(QOrganizerItemType::TypeNote))
                         << QString(QLatin1String(QOrganizerItemType::TypeTodo))
                         << QString(QLatin1String(QOrganizerItemType::TypeTodoOccurrence)));
    fields.insert(QOrganizerItemType::FieldType, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // guid
    d.setName(QOrganizerItemGuid::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemGuid::FieldGuid, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // timestamp
    d.setName(QOrganizerItemTimestamp::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::DateTime);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemTimestamp::FieldModificationTimestamp, f);
    fields.insert(QOrganizerItemTimestamp::FieldCreationTimestamp, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // display label
    d.setName(QOrganizerItemDisplayLabel::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemDisplayLabel::FieldLabel, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // description
    d.setName(QOrganizerItemDescription::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemDescription::FieldDescription, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // recurrence information
    d.setName(QOrganizerItemRecurrence::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::List);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemRecurrence::FieldExceptionDates, f);
    fields.insert(QOrganizerItemRecurrence::FieldRecurrenceDates, f);
    fields.insert(QOrganizerItemRecurrence::FieldExceptionRules, f);
    fields.insert(QOrganizerItemRecurrence::FieldRecurrenceRules, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // reminder
    d.setName(QOrganizerItemReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldReminderType, f);
    fields.insert(QOrganizerItemReminder::FieldTimeDelta, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    f.setDataType(QVariant::DateTime);
    fields.insert(QOrganizerItemReminder::FieldDateTime, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same event
    retn.insert(d.name(), d);

    // audible reminder
    d.setName(QOrganizerItemAudibleReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldReminderType, f);
    fields.insert(QOrganizerItemReminder::FieldTimeDelta, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    f.setDataType(QVariant::DateTime);
    fields.insert(QOrganizerItemReminder::FieldDateTime, f);
    f.setDataType(QVariant::Url);
    fields.insert(QOrganizerItemAudibleReminder::FieldDataUrl, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same event
    retn.insert(d.name(), d);

    // email reminder
    d.setName(QOrganizerItemEmailReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldReminderType, f);
    fields.insert(QOrganizerItemReminder::FieldTimeDelta, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    f.setDataType(QVariant::DateTime);
    fields.insert(QOrganizerItemReminder::FieldDateTime, f);
    f.setDataType(QVariant::String);
    fields.insert(QOrganizerItemEmailReminder::FieldBody, f);
    fields.insert(QOrganizerItemEmailReminder::FieldSubject, f);
    f.setDataType(QVariant::StringList);
    fields.insert(QOrganizerItemEmailReminder::FieldRecipients, f);
    f.setDataType(QVariant::List);
    fields.insert(QOrganizerItemEmailReminder::FieldAttachments, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same event
    retn.insert(d.name(), d);

    // visual reminder
    d.setName(QOrganizerItemVisualReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldReminderType, f);
    fields.insert(QOrganizerItemReminder::FieldTimeDelta, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    f.setDataType(QVariant::DateTime);
    fields.insert(QOrganizerItemReminder::FieldDateTime, f);
    f.setDataType(QVariant::String);
    fields.insert(QOrganizerItemVisualReminder::FieldMessage, f);
    f.setDataType(QVariant::Url);
    fields.insert(QOrganizerItemVisualReminder::FieldDataUrl, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same event
    retn.insert(d.name(), d);

    // event time range
    d.setName(QOrganizerEventTimeRange::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::DateTime);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerEventTimeRange::FieldStartDateTime, f);
    fields.insert(QOrganizerEventTimeRange::FieldEndDateTime, f);
    f.setDataType(QVariant::Bool);
    fields.insert(QOrganizerEventTimeRange::FieldTimeSpecified, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // priority
    d.setName(QOrganizerItemPriority::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList()
                         << static_cast<int>(QOrganizerItemPriority::UnknownPriority)
                         << static_cast<int>(QOrganizerItemPriority::HighestPriority)
                         << static_cast<int>(QOrganizerItemPriority::ExtremelyHighPriority)
                         << static_cast<int>(QOrganizerItemPriority::VeryHighPriority)
                         << static_cast<int>(QOrganizerItemPriority::HighPriority)
                         << static_cast<int>(QOrganizerItemPriority::MediumPriority)
                         << static_cast<int>(QOrganizerItemPriority::LowPriority)
                         << static_cast<int>(QOrganizerItemPriority::VeryLowPriority)
                         << static_cast<int>(QOrganizerItemPriority::ExtremelyLowPriority)
                         << static_cast<int>(QOrganizerItemPriority::LowestPriority));
    fields.insert(QOrganizerItemPriority::FieldPriority, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // location
    d.setName(QOrganizerItemLocation::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemLocation::FieldAddress, f);
    fields.insert(QOrganizerItemLocation::FieldGeoLocation, f);
    fields.insert(QOrganizerItemLocation::FieldLocationName, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // comment
    d.setName(QOrganizerItemComment::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemComment::FieldComment, f);
    d.setFields(fields);
    d.setUnique(false);
    retn.insert(d.name(), d);

    retnSchema.insert(QOrganizerItemType::TypeEvent, retn);

    // and then again for EVENTOCCURRENCEs =============================
    retn.clear();

    // type
    d.setName(QOrganizerItemType::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList()
                         << QString(QLatin1String(QOrganizerItemType::TypeEvent))
                         << QString(QLatin1String(QOrganizerItemType::TypeEventOccurrence))
                         << QString(QLatin1String(QOrganizerItemType::TypeJournal))
                         << QString(QLatin1String(QOrganizerItemType::TypeNote))
                         << QString(QLatin1String(QOrganizerItemType::TypeTodo))
                         << QString(QLatin1String(QOrganizerItemType::TypeTodoOccurrence)));
    fields.insert(QOrganizerItemType::FieldType, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // guid
    d.setName(QOrganizerItemGuid::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemGuid::FieldGuid, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // timestamp
    d.setName(QOrganizerItemTimestamp::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::DateTime);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemTimestamp::FieldModificationTimestamp, f);
    fields.insert(QOrganizerItemTimestamp::FieldCreationTimestamp, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // display label
    d.setName(QOrganizerItemDisplayLabel::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemDisplayLabel::FieldLabel, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // description
    d.setName(QOrganizerItemDescription::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemDescription::FieldDescription, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // instance origin
    d.setName(QOrganizerItemInstanceOrigin::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemInstanceOrigin::FieldParentLocalId, f);
    f.setDataType(QVariant::Date);
    fields.insert(QOrganizerItemInstanceOrigin::FieldOriginalDate, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // event time range
    d.setName(QOrganizerEventTimeRange::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::DateTime);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerEventTimeRange::FieldStartDateTime, f);
    fields.insert(QOrganizerEventTimeRange::FieldEndDateTime, f);
    f.setDataType(QVariant::Bool);
    fields.insert(QOrganizerEventTimeRange::FieldTimeSpecified, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // priority
    d.setName(QOrganizerItemPriority::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList()
                         << static_cast<int>(QOrganizerItemPriority::UnknownPriority)
                         << static_cast<int>(QOrganizerItemPriority::VeryLowPriority)
                         << static_cast<int>(QOrganizerItemPriority::LowPriority)
                         << static_cast<int>(QOrganizerItemPriority::MediumPriority)
                         << static_cast<int>(QOrganizerItemPriority::HighPriority)
                         << static_cast<int>(QOrganizerItemPriority::VeryHighPriority));
    fields.insert(QOrganizerItemPriority::FieldPriority, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // location
    d.setName(QOrganizerItemLocation::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemLocation::FieldAddress, f);
    fields.insert(QOrganizerItemLocation::FieldGeoLocation, f);
    fields.insert(QOrganizerItemLocation::FieldLocationName, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // reminder
    d.setName(QOrganizerItemReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldReminderType, f);
    fields.insert(QOrganizerItemReminder::FieldTimeDelta, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    f.setDataType(QVariant::DateTime);
    fields.insert(QOrganizerItemReminder::FieldDateTime, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same event occurrence
    retn.insert(d.name(), d);

    // audible reminder
    d.setName(QOrganizerItemAudibleReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldReminderType, f);
    fields.insert(QOrganizerItemReminder::FieldTimeDelta, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    f.setDataType(QVariant::DateTime);
    fields.insert(QOrganizerItemReminder::FieldDateTime, f);
    f.setDataType(QVariant::Url);
    fields.insert(QOrganizerItemAudibleReminder::FieldDataUrl, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same event occurrence
    retn.insert(d.name(), d);

    // email reminder
    d.setName(QOrganizerItemEmailReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldReminderType, f);
    fields.insert(QOrganizerItemReminder::FieldTimeDelta, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    f.setDataType(QVariant::DateTime);
    fields.insert(QOrganizerItemReminder::FieldDateTime, f);
    f.setDataType(QVariant::String);
    fields.insert(QOrganizerItemEmailReminder::FieldBody, f);
    fields.insert(QOrganizerItemEmailReminder::FieldSubject, f);
    f.setDataType(QVariant::StringList);
    fields.insert(QOrganizerItemEmailReminder::FieldRecipients, f);
    f.setDataType(QVariant::List);
    fields.insert(QOrganizerItemEmailReminder::FieldAttachments, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same event occurrence
    retn.insert(d.name(), d);

    // visual reminder
    d.setName(QOrganizerItemVisualReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldReminderType, f);
    fields.insert(QOrganizerItemReminder::FieldTimeDelta, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    f.setDataType(QVariant::DateTime);
    fields.insert(QOrganizerItemReminder::FieldDateTime, f);
    f.setDataType(QVariant::String);
    fields.insert(QOrganizerItemVisualReminder::FieldMessage, f);
    f.setDataType(QVariant::Url);
    fields.insert(QOrganizerItemVisualReminder::FieldDataUrl, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same event occurrence
    retn.insert(d.name(), d);

    // comment
    d.setName(QOrganizerItemComment::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemComment::FieldComment, f);
    d.setFields(fields);
    d.setUnique(false);
    retn.insert(d.name(), d);

    retnSchema.insert(QOrganizerItemType::TypeEventOccurrence, retn);

    // and then again for TODOs =============================
    retn.clear();

    // type
    d.setName(QOrganizerItemType::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList()
                         << QString(QLatin1String(QOrganizerItemType::TypeEvent))
                         << QString(QLatin1String(QOrganizerItemType::TypeEventOccurrence))
                         << QString(QLatin1String(QOrganizerItemType::TypeJournal))
                         << QString(QLatin1String(QOrganizerItemType::TypeNote))
                         << QString(QLatin1String(QOrganizerItemType::TypeTodo))
                         << QString(QLatin1String(QOrganizerItemType::TypeTodoOccurrence)));
    fields.insert(QOrganizerItemType::FieldType, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // guid
    d.setName(QOrganizerItemGuid::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemGuid::FieldGuid, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // timestamp
    d.setName(QOrganizerItemTimestamp::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::DateTime);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemTimestamp::FieldModificationTimestamp, f);
    fields.insert(QOrganizerItemTimestamp::FieldCreationTimestamp, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // display label
    d.setName(QOrganizerItemDisplayLabel::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemDisplayLabel::FieldLabel, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // description
    d.setName(QOrganizerItemDescription::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemDescription::FieldDescription, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // recurrence information
    d.setName(QOrganizerItemRecurrence::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::List);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemRecurrence::FieldExceptionDates, f);
    fields.insert(QOrganizerItemRecurrence::FieldRecurrenceDates, f);
    fields.insert(QOrganizerItemRecurrence::FieldExceptionRules, f);
    fields.insert(QOrganizerItemRecurrence::FieldRecurrenceRules, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // reminder
    d.setName(QOrganizerItemReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldReminderType, f);
    fields.insert(QOrganizerItemReminder::FieldTimeDelta, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    f.setDataType(QVariant::DateTime);
    fields.insert(QOrganizerItemReminder::FieldDateTime, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same todo
    retn.insert(d.name(), d);

    // audible reminder
    d.setName(QOrganizerItemAudibleReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldReminderType, f);
    fields.insert(QOrganizerItemReminder::FieldTimeDelta, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    f.setDataType(QVariant::DateTime);
    fields.insert(QOrganizerItemReminder::FieldDateTime, f);
    f.setDataType(QVariant::Url);
    fields.insert(QOrganizerItemAudibleReminder::FieldDataUrl, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same todo
    retn.insert(d.name(), d);

    // email reminder
    d.setName(QOrganizerItemEmailReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldReminderType, f);
    fields.insert(QOrganizerItemReminder::FieldTimeDelta, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    f.setDataType(QVariant::DateTime);
    fields.insert(QOrganizerItemReminder::FieldDateTime, f);
    f.setDataType(QVariant::String);
    fields.insert(QOrganizerItemEmailReminder::FieldBody, f);
    fields.insert(QOrganizerItemEmailReminder::FieldSubject, f);
    f.setDataType(QVariant::StringList);
    fields.insert(QOrganizerItemEmailReminder::FieldRecipients, f);
    f.setDataType(QVariant::List);
    fields.insert(QOrganizerItemEmailReminder::FieldAttachments, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same todo
    retn.insert(d.name(), d);

    // visual reminder
    d.setName(QOrganizerItemVisualReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldReminderType, f);
    fields.insert(QOrganizerItemReminder::FieldTimeDelta, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    f.setDataType(QVariant::DateTime);
    fields.insert(QOrganizerItemReminder::FieldDateTime, f);
    f.setDataType(QVariant::String);
    fields.insert(QOrganizerItemVisualReminder::FieldMessage, f);
    f.setDataType(QVariant::Url);
    fields.insert(QOrganizerItemVisualReminder::FieldDataUrl, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same todo
    retn.insert(d.name(), d);

    // todo progress
    d.setName(QOrganizerTodoProgress::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::DateTime);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerTodoProgress::FieldFinishedDateTime, f);
    f.setDataType(QVariant::Int);
    fields.insert(QOrganizerTodoProgress::FieldPercentageComplete, f);
    fields.insert(QOrganizerTodoProgress::FieldStatus, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // todo time range
    d.setName(QOrganizerTodoTimeRange::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::DateTime);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerTodoTimeRange::FieldStartDateTime, f);
    fields.insert(QOrganizerTodoTimeRange::FieldDueDateTime, f);
    f.setDataType(QVariant::Bool);
    fields.insert(QOrganizerTodoTimeRange::FieldTimeSpecified, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // priority
    d.setName(QOrganizerItemPriority::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList()
                         << static_cast<int>(QOrganizerItemPriority::UnknownPriority)
                         << static_cast<int>(QOrganizerItemPriority::VeryLowPriority)
                         << static_cast<int>(QOrganizerItemPriority::LowPriority)
                         << static_cast<int>(QOrganizerItemPriority::MediumPriority)
                         << static_cast<int>(QOrganizerItemPriority::HighPriority)
                         << static_cast<int>(QOrganizerItemPriority::VeryHighPriority));
    fields.insert(QOrganizerItemPriority::FieldPriority, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // comment
    d.setName(QOrganizerItemComment::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemComment::FieldComment, f);
    d.setFields(fields);
    d.setUnique(false);
    retn.insert(d.name(), d);

    retnSchema.insert(QOrganizerItemType::TypeTodo, retn);

    // and then again for TODOOCCURRENCEs =============================
    retn.clear();

    // type
    d.setName(QOrganizerItemType::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList()
                         << QString(QLatin1String(QOrganizerItemType::TypeEvent))
                         << QString(QLatin1String(QOrganizerItemType::TypeEventOccurrence))
                         << QString(QLatin1String(QOrganizerItemType::TypeJournal))
                         << QString(QLatin1String(QOrganizerItemType::TypeNote))
                         << QString(QLatin1String(QOrganizerItemType::TypeTodo))
                         << QString(QLatin1String(QOrganizerItemType::TypeTodoOccurrence)));
    fields.insert(QOrganizerItemType::FieldType, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // guid
    d.setName(QOrganizerItemGuid::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemGuid::FieldGuid, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // timestamp
    d.setName(QOrganizerItemTimestamp::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::DateTime);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemTimestamp::FieldModificationTimestamp, f);
    fields.insert(QOrganizerItemTimestamp::FieldCreationTimestamp, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // display label
    d.setName(QOrganizerItemDisplayLabel::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemDisplayLabel::FieldLabel, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // description
    d.setName(QOrganizerItemDescription::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemDescription::FieldDescription, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // instance origin
    d.setName(QOrganizerItemInstanceOrigin::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemInstanceOrigin::FieldParentLocalId, f);
    f.setDataType(QVariant::Date);
    fields.insert(QOrganizerItemInstanceOrigin::FieldOriginalDate, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // todo progress
    d.setName(QOrganizerTodoProgress::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::DateTime);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerTodoProgress::FieldFinishedDateTime, f);
    f.setDataType(QVariant::Int);
    fields.insert(QOrganizerTodoProgress::FieldPercentageComplete, f);
    fields.insert(QOrganizerTodoProgress::FieldStatus, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // reminder
    d.setName(QOrganizerItemReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldReminderType, f);
    fields.insert(QOrganizerItemReminder::FieldTimeDelta, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    f.setDataType(QVariant::DateTime);
    fields.insert(QOrganizerItemReminder::FieldDateTime, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same todo occurrence
    retn.insert(d.name(), d);

    // audible reminder
    d.setName(QOrganizerItemAudibleReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldReminderType, f);
    fields.insert(QOrganizerItemReminder::FieldTimeDelta, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    f.setDataType(QVariant::DateTime);
    fields.insert(QOrganizerItemReminder::FieldDateTime, f);
    f.setDataType(QVariant::Url);
    fields.insert(QOrganizerItemAudibleReminder::FieldDataUrl, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same todo occurrence
    retn.insert(d.name(), d);

    // email reminder
    d.setName(QOrganizerItemEmailReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldReminderType, f);
    fields.insert(QOrganizerItemReminder::FieldTimeDelta, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    f.setDataType(QVariant::DateTime);
    fields.insert(QOrganizerItemReminder::FieldDateTime, f);
    f.setDataType(QVariant::String);
    fields.insert(QOrganizerItemEmailReminder::FieldBody, f);
    fields.insert(QOrganizerItemEmailReminder::FieldSubject, f);
    f.setDataType(QVariant::StringList);
    fields.insert(QOrganizerItemEmailReminder::FieldRecipients, f);
    f.setDataType(QVariant::List);
    fields.insert(QOrganizerItemEmailReminder::FieldAttachments, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same todo occurrence
    retn.insert(d.name(), d);

    // visual reminder
    d.setName(QOrganizerItemVisualReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldReminderType, f);
    fields.insert(QOrganizerItemReminder::FieldTimeDelta, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    f.setDataType(QVariant::DateTime);
    fields.insert(QOrganizerItemReminder::FieldDateTime, f);
    f.setDataType(QVariant::String);
    fields.insert(QOrganizerItemVisualReminder::FieldMessage, f);
    f.setDataType(QVariant::Url);
    fields.insert(QOrganizerItemVisualReminder::FieldDataUrl, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same todo occurrence
    retn.insert(d.name(), d);

    // todo time range
    d.setName(QOrganizerTodoTimeRange::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::DateTime);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerTodoTimeRange::FieldStartDateTime, f);
    fields.insert(QOrganizerTodoTimeRange::FieldDueDateTime, f);
    f.setDataType(QVariant::Bool);
    fields.insert(QOrganizerTodoTimeRange::FieldTimeSpecified, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // priority
    d.setName(QOrganizerItemPriority::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList()
                         << static_cast<int>(QOrganizerItemPriority::UnknownPriority)
                         << static_cast<int>(QOrganizerItemPriority::VeryLowPriority)
                         << static_cast<int>(QOrganizerItemPriority::LowPriority)
                         << static_cast<int>(QOrganizerItemPriority::MediumPriority)
                         << static_cast<int>(QOrganizerItemPriority::HighPriority)
                         << static_cast<int>(QOrganizerItemPriority::VeryHighPriority));
    fields.insert(QOrganizerItemPriority::FieldPriority, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // comment
    d.setName(QOrganizerItemComment::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemComment::FieldComment, f);
    d.setFields(fields);
    d.setUnique(false);
    retn.insert(d.name(), d);

    retnSchema.insert(QOrganizerItemType::TypeTodoOccurrence, retn);

    // and then again for JOURNALs =============================
    retn.clear();

    // type
    d.setName(QOrganizerItemType::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList()
                         << QString(QLatin1String(QOrganizerItemType::TypeEvent))
                         << QString(QLatin1String(QOrganizerItemType::TypeEventOccurrence))
                         << QString(QLatin1String(QOrganizerItemType::TypeJournal))
                         << QString(QLatin1String(QOrganizerItemType::TypeNote))
                         << QString(QLatin1String(QOrganizerItemType::TypeTodo))
                         << QString(QLatin1String(QOrganizerItemType::TypeTodoOccurrence)));
    fields.insert(QOrganizerItemType::FieldType, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // guid
    d.setName(QOrganizerItemGuid::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemGuid::FieldGuid, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // timestamp
    d.setName(QOrganizerItemTimestamp::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::DateTime);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemTimestamp::FieldModificationTimestamp, f);
    fields.insert(QOrganizerItemTimestamp::FieldCreationTimestamp, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // display label
    d.setName(QOrganizerItemDisplayLabel::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemDisplayLabel::FieldLabel, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // description
    d.setName(QOrganizerItemDescription::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemDescription::FieldDescription, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // journal time range
    d.setName(QOrganizerJournalTimeRange::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::DateTime);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerJournalTimeRange::FieldEntryDateTime, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // comment
    d.setName(QOrganizerItemComment::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemComment::FieldComment, f);
    d.setFields(fields);
    d.setUnique(false);
    retn.insert(d.name(), d);

    retnSchema.insert(QOrganizerItemType::TypeJournal, retn);

    return retnSchema;
}

/*!
  Checks that the given item \a organizeritem does not have details which
  don't conform to a valid definition, violate uniqueness constraints,
  or contain values for nonexistent fields, and that the values contained are
  of the correct type for each field, and are allowable values for that field.

  Note that this function is unable to ensure that the access constraints
  (such as CreateOnly and ReadOnly) are observed; backend specific code
  must be written if you wish to enforce these constraints.

  Returns true if the \a organizeritem is valid according to the definitions for
  its details, otherwise returns false.

  Any errors encountered during this operation should be stored to
  \a error.
 */
bool QOrganizerItemManagerEngine::validateItem(const QOrganizerItem& organizeritem, QOrganizerItemManager::Error* error) const
{
    QList<QString> uniqueDefinitionIds;

    // check that each detail conforms to its definition as supported by this manager.
    foreach (const QOrganizerItemDetail& detail, organizeritem.details()) {
        QVariantMap values = detail.variantValues();
        QOrganizerItemDetailDefinition def = detailDefinition(detail.definitionName(), organizeritem.type(), error);
        // check that the definition is supported
        if (*error != QOrganizerItemManager::NoError) {
            *error = QOrganizerItemManager::InvalidDetailError;
            return false; // this definition is not supported.
        }

        // check uniqueness
        if (def.isUnique()) {
            if (uniqueDefinitionIds.contains(def.name())) {
                *error = QOrganizerItemManager::AlreadyExistsError;
                return false; // can't have two of a unique detail.
            }
            uniqueDefinitionIds.append(def.name());
        }

        QMapIterator<QString,QVariant> fieldIt(values);
        while (fieldIt.hasNext()) {
            fieldIt.next();
            const QString& key = fieldIt.key();
            const QVariant& variant = fieldIt.value();
            // check that no values exist for nonexistent fields.
            if (!def.fields().contains(key)) {
                *error = QOrganizerItemManager::InvalidDetailError;
                return false; // value for nonexistent field.
            }

            QOrganizerItemDetailFieldDefinition field = def.fields().value(key);
            // check that the type of each value corresponds to the allowable field type
            if (static_cast<int>(field.dataType()) != variant.userType()) {
                *error = QOrganizerItemManager::InvalidDetailError;
                return false; // type doesn't match.
            }

            // check that the value is allowable
            // if the allowable values is an empty list, any are allowed.
            if (!field.allowableValues().isEmpty()) {
                // if the field datatype is a list, check that it contains only allowable values
                if (field.dataType() == QVariant::List || field.dataType() == QVariant::StringList) {
                    QList<QVariant> innerValues = variant.toList();
                    QListIterator<QVariant> it(innerValues);
                    while (it.hasNext()) {
                        if (!field.allowableValues().contains(it.next())) {
                            *error = QOrganizerItemManager::InvalidDetailError;
                            return false; // value not allowed.
                        }
                    }
                } else if (!field.allowableValues().contains(variant)) {
                    // the datatype is not a list; the value wasn't allowed.
                    *error = QOrganizerItemManager::InvalidDetailError;
                    return false; // value not allowed.
                }
            }
        }
    }

    return true;
}

/*!
  Checks that the given detail definition \a definition seems valid,
  with a correct id, defined fields, and any specified value types
  are supported by this engine.  This function is called before
  trying to save a definition.

  Returns true if the \a definition seems valid, otherwise returns
  false.

  Any errors encountered during this operation should be stored to
  \a error.
 */
bool QOrganizerItemManagerEngine::validateDefinition(const QOrganizerItemDetailDefinition& definition, QOrganizerItemManager::Error* error) const
{
    if (definition.name().isEmpty()) {
        *error = QOrganizerItemManager::BadArgumentError;
        return false;
    }

    if (definition.fields().count() == 0) {
        *error = QOrganizerItemManager::BadArgumentError;
        return false;
    }

    // Check each field now
    QList<QVariant::Type> types = supportedDataTypes();
    QMapIterator<QString, QOrganizerItemDetailFieldDefinition> it(definition.fields());
    while(it.hasNext()) {
        it.next();
        if (it.key().isEmpty()) {
            *error = QOrganizerItemManager::BadArgumentError;
            return false;
        }

        if (!types.contains(it.value().dataType())) {
            *error = QOrganizerItemManager::BadArgumentError;
            return false;
        }

        // Check that each allowed value is the same type
        for (int i=0; i < it.value().allowableValues().count(); i++) {
            if (it.value().allowableValues().at(i).type() != it.value().dataType()) {
                *error = QOrganizerItemManager::BadArgumentError;
                return false;
            }
        }
    }
    *error = QOrganizerItemManager::NoError;
    return true;
}

/*!
  Returns the registered detail definitions which are valid for organizer items whose type is of the given \a organizeritemType in this engine.

  Any errors encountered during this operation should be stored to
  \a error.
 */
QMap<QString, QOrganizerItemDetailDefinition> QOrganizerItemManagerEngine::detailDefinitions(const QString& organizeritemType, QOrganizerItemManager::Error* error) const
{
    Q_UNUSED(organizeritemType);
    *error = QOrganizerItemManager::NotSupportedError;
    return QMap<QString, QOrganizerItemDetailDefinition>();
}

/*!
  Returns the definition identified by the given \a definitionName that
  is valid for organizer items whose type is of the given \a organizeritemType in this store, or a default-constructed QOrganizerItemDetailDefinition
  if no such definition exists

  Any errors encountered during this operation should be stored to
  \a error.
 */
QOrganizerItemDetailDefinition QOrganizerItemManagerEngine::detailDefinition(const QString& definitionName, const QString& organizeritemType, QOrganizerItemManager::Error* error) const
{
    QMap<QString, QOrganizerItemDetailDefinition> definitions = detailDefinitions(organizeritemType, error);
    if (definitions.contains(definitionName))  {
        *error = QOrganizerItemManager::NoError;
        return definitions.value(definitionName);
    } else {
        *error = QOrganizerItemManager::DoesNotExistError;
        return QOrganizerItemDetailDefinition();
    }
}

/*!
  Persists the given definition \a def in the database, which is valid for organizer items whose type is the given \a organizeritemType.

  Returns true if the definition was saved successfully, and otherwise returns false.

  The backend must emit the appropriate signals to inform clients of changes
  to the database resulting from this operation.

  Any errors encountered during this operation should be stored to
  \a error.
 */
bool QOrganizerItemManagerEngine::saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& organizeritemType, QOrganizerItemManager::Error* error)
{
    Q_UNUSED(def);
    Q_UNUSED(organizeritemType);

    *error = QOrganizerItemManager::NotSupportedError;
    return false;
}

/*!
  Removes the definition identified by the given \a definitionName from the database, where it was valid for organizer items whose type was the given \a organizeritemType.

  Returns true if the definition was removed successfully, otherwise returns false.

  The backend must emit the appropriate signals to inform clients of changes
  to the database resulting from this operation.

  Any errors encountered during this operation should be stored to
  \a error.
 */
bool QOrganizerItemManagerEngine::removeDetailDefinition(const QString& definitionName, const QString& organizeritemType, QOrganizerItemManager::Error* error)
{
    Q_UNUSED(definitionName);
    Q_UNUSED(organizeritemType);

    *error = QOrganizerItemManager::NotSupportedError;
    return false;
}

/*!
  Sets the access constraints of \a detail to the supplied \a constraints.

  This function is provided to allow engine implementations to report the
  access constraints of retrieved details, without generally allowing the
  access constraints to be modified after retrieval.

  Application code should not call this function, since validation of the
  detail will happen in the engine in any case.
 */
void QOrganizerItemManagerEngine::setDetailAccessConstraints(QOrganizerItemDetail *detail, QOrganizerItemDetail::AccessConstraints constraints)
{
    if (detail) {
        QOrganizerItemDetailPrivate::setAccessConstraints(detail, constraints);
    }
}


/*!
  Adds the given \a organizeritem to the database if \a organizeritem has a
  default-constructed id, or an id with the manager URI set to the URI of
  this manager and a local id of zero, otherwise updates the organizer item in
  the database which has the same id to be the given \a organizeritem.
  If the id is non-zero but does not identify any item stored in the
  manager, the function will return false and \a error will be set to
  \c QOrganizerItemManager::DoesNotExistError.

  Returns true if the save operation completed successfully, otherwise
  returns false.  Any error which occurs will be saved in \a error.

  The default implementation will convert this into a call to saveItems.

  \sa managerUri()
 */
bool QOrganizerItemManagerEngine::saveItem(QOrganizerItem* organizeritem, QOrganizerItemManager::Error* error)
{
    // Convert to a list op
    if (organizeritem) {
        QList<QOrganizerItem> list;
        list.append(*organizeritem);

        QMap<int, QOrganizerItemManager::Error> errors;
        bool ret = saveItems(&list, &errors, error);

        if (errors.count() > 0)
            *error = errors.begin().value();

        *organizeritem = list.value(0);
        return ret;
    } else {
        *error = QOrganizerItemManager::BadArgumentError;
        return false;
    }
}

/*!
  Remove the item identified by \a organizeritemId from the database,
  and also removes any relationships in which the item was involved.
  Returns true if the item was removed successfully, otherwise
  returns false.

  Any error which occurs will be saved in \a error.

  The default implementation will convert this into a call to removeItems.
 */
bool QOrganizerItemManagerEngine::removeItem(const QOrganizerItemLocalId& organizeritemId, QOrganizerItemManager::Error* error)
{
    // Convert to a list op
    QList<QOrganizerItemLocalId> list;
    list.append(organizeritemId);

    QMap<int, QOrganizerItemManager::Error> errors;
    bool ret = removeItems(list, &errors, error);

    if (errors.count() > 0)
        *error = errors.begin().value();

    return ret;
}

/*!
  Adds the list of organizer items given by \a organizeritems list to the database.
  Returns true if the organizer items were saved successfully, otherwise false.

  The engine might populate \a errorMap (the map of indices of the \a organizeritems list to
  the error which occurred when saving the item at that index) for
  every index for which the item could not be saved, if it is able.
  The \l QOrganizerItemManager::error() function will only return \c QOrganizerItemManager::NoError
  if all organizer items were saved successfully.

  For each newly saved item that was successful, the id of the item
  in the \a organizeritems list will be updated with the new value.  If a failure occurs
  when saving a new item, the id will be cleared.

  Any errors encountered during this operation should be stored to
  \a error.

  \sa QOrganizerItemManager::saveItem()
 */
bool QOrganizerItemManagerEngine::saveItems(QList<QOrganizerItem>* organizeritems, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error)
{
    Q_UNUSED(organizeritems);
    Q_UNUSED(errorMap);
    *error = QOrganizerItemManager::NotSupportedError;
    return false;
}

/*!
  Remove every item whose id is contained in the list of organizer items ids
  \a organizeritemIds.  Returns true if all organizer items were removed successfully,
  otherwise false.

  Any item that was removed successfully will have the relationships
  in which it was involved removed also.

  The manager might populate \a errorMap (the map of indices of the \a organizeritemIds list to
  the error which occurred when saving the item at that index) for every
  index for which the item could not be removed, if it is able.
  The \l QOrganizerItemManager::error() function will
  only return \c QOrganizerItemManager::NoError if all organizer items were removed
  successfully.

  If the list contains ids which do not identify a valid item in the manager, the function will
  remove any organizer items which are identified by ids in the \a organizeritemIds list, insert
  \c QOrganizerItemManager::DoesNotExist entries into the \a errorMap for the indices of invalid ids
  in the \a organizeritemIds list, return false, and set the overall operation error to
  \c QOrganizerItemManager::DoesNotExistError.

  Any errors encountered during this operation should be stored to
  \a error.

  \sa QOrganizerItemManager::removeItem()
 */
bool QOrganizerItemManagerEngine::removeItems(const QList<QOrganizerItemLocalId>& organizeritemIds, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error)
{
    Q_UNUSED(organizeritemIds);
    Q_UNUSED(errorMap);
    *error = QOrganizerItemManager::NotSupportedError;
    return false;
}

/*!
  Returns a pruned or modified version of the \a original item which is valid and can be saved in the manager.
  The returned item might have details removed or arbitrarily changed.  The cache of relationships
  in the item are ignored entirely when considering compatibility with the backend, as they are
  saved and validated separately.  Any error which occurs will be saved to \a error.
 */
QOrganizerItem QOrganizerItemManagerEngine::compatibleItem(const QOrganizerItem& original, QOrganizerItemManager::Error* error) const
{
    QOrganizerItem conforming;
    conforming.setType(original.type());
    conforming.setId(original.id());
    QOrganizerItemManager::Error tempError;
    QList<QString> uniqueDefinitionIds;
    foreach (QOrganizerItemDetail detail, original.details()) {
        // check that the detail conforms to the definition in this manager.
        // if so, then add it to the conforming item to be returned.  if not, prune it.

        QVariantMap values = detail.variantValues();
        QOrganizerItemDetailDefinition def = detailDefinition(detail.definitionName(), original.type(), &tempError);
        // check that the definition is supported
        if (tempError != QOrganizerItemManager::NoError) {
            continue; // this definition is not supported.
        }

        // check uniqueness
        if (def.isUnique()) {
            if (uniqueDefinitionIds.contains(def.name())) {
                continue; // can't have two of a unique detail.
            }
            uniqueDefinitionIds.append(def.name());
        }

        QMapIterator<QString,QVariant> fieldIt(values);
        while (fieldIt.hasNext()) {
            fieldIt.next();
            const QString& key = fieldIt.key();
            const QVariant& variant = fieldIt.value();
            // prune values for nonexistent fields.
            if (!def.fields().contains(key)) {
                detail.removeValue(key);
            }

            QOrganizerItemDetailFieldDefinition field = def.fields().value(key);
            // prune values that do not correspond to the allowable field type
            if (static_cast<int>(field.dataType()) != variant.userType()) {
                detail.removeValue(key);
            }

            // check that the value is allowable
            // if the allowable values is an empty list, any are allowed.
            if (!field.allowableValues().isEmpty()) {
                // if the field datatype is a list, remove non-allowable values
                if (field.dataType() == QVariant::List || field.dataType() == QVariant::StringList) {
                    QList<QVariant> innerValues = variant.toList();
                    QMutableListIterator<QVariant> it(innerValues);
                    while (it.hasNext()) {
                        if (!field.allowableValues().contains(it.next())) {
                            it.remove();
                        }
                    }
                    if (innerValues.isEmpty()) {
                        detail.removeValue(key);
                    } else {
                        detail.setValue(key, innerValues);
                    }
                } else if (!field.allowableValues().contains(variant)) {
                    detail.removeValue(key);
                }
            }
        }

        // if it hasn't been pruned away to nothing, save it in the conforming item
        if (!detail.isEmpty()) {
            conforming.saveDetail(&detail);
        }
    }

    *error = QOrganizerItemManager::NoError;
    if (conforming.isEmpty())
        *error = QOrganizerItemManager::DoesNotExistError;
    return conforming;
}

/*!
  Compares \a first against \a second.  If the types are
  strings (QVariant::String), the \a sensitivity argument controls
  case sensitivity when comparing.

  Returns:
  <0 if \a first is less than \a second
   0 if \a first is equal to \a second
  >0 if \a first is greater than \a second.

  The results are undefined if the variants are different types, or
  cannot be compared.
 */
int QOrganizerItemManagerEngine::compareVariant(const QVariant& first, const QVariant& second, Qt::CaseSensitivity sensitivity)
{
    switch(first.type()) {
        case QVariant::Int:
            return first.toInt() - second.toInt();

        case QVariant::LongLong:
            return first.toLongLong() - second.toLongLong();

        case QVariant::Bool:
        case QVariant::Char:
        case QVariant::UInt:
            return first.toUInt() - second.toUInt();

        case QVariant::ULongLong:
            return first.toULongLong() - second.toULongLong();

       case QVariant::String:
            return first.toString().compare(second.toString(), sensitivity);

        case QVariant::Double:
            {
                const double a = first.toDouble();
                const double b = second.toDouble();
                return (a < b) ? -1 : ((a == b) ? 0 : 1);
            }

        case QVariant::DateTime:
            {
                const QDateTime a = first.toDateTime();
                const QDateTime b = second.toDateTime();
                return (a < b) ? -1 : ((a == b) ? 0 : 1);
            }

        case QVariant::Date:
            return first.toDate().toJulianDay() - second.toDate().toJulianDay();

        case QVariant::Time:
            {
                const QTime a = first.toTime();
                const QTime b = second.toTime();
                return (a < b) ? -1 : ((a == b) ? 0 : 1);
            }

        default:
            return 0;
    }
}

/*!
  Returns true if the supplied item \a organizeritem matches the supplied filter \a filter.

  This function will test each condition in the filter, possibly recursing.
 */
bool QOrganizerItemManagerEngine::testFilter(const QOrganizerItemFilter &filter, const QOrganizerItem &organizeritem)
{
    switch(filter.type()) {
        case QOrganizerItemFilter::InvalidFilter:
        case QOrganizerItemFilter::ActionFilter:
            return false;

        case QOrganizerItemFilter::DefaultFilter:
            return true;

        case QOrganizerItemFilter::LocalIdFilter:
            {
                const QOrganizerItemLocalIdFilter idf(filter);
                if (idf.ids().contains(organizeritem.id().localId()))
                    return true;
            }
            // Fall through to end
            break;

        case QOrganizerItemFilter::OrganizerItemDetailFilter:
            {
                const QOrganizerItemDetailFilter cdf(filter);
                if (cdf.detailDefinitionName().isEmpty())
                    return false;

                /* See if this organizer item has one of these details in it */
                const QList<QOrganizerItemDetail>& details = organizeritem.details(cdf.detailDefinitionName());

                if (details.count() == 0)
                    return false; /* can't match */

                /* See if we need to check the values */
                if (cdf.detailFieldName().isEmpty())
                    return true;  /* just testing for the presence of a detail of the specified definition */

                /* Now figure out what tests we are doing */
                const bool valueTest = cdf.value().isValid();
                const bool presenceTest = !valueTest;

                /* See if we need to test any values at all */
                if (presenceTest) {
                    for(int j=0; j < details.count(); j++) {
                        const QOrganizerItemDetail& detail = details.at(j);

                        /* Check that the field is present and has a non-empty value */
                        if (detail.variantValues().contains(cdf.detailFieldName()) && !detail.value(cdf.detailFieldName()).isEmpty())
                            return true;
                    }
                    return false;
                }

                /* Case sensitivity, for those parts that use it */
                Qt::CaseSensitivity cs = (cdf.matchFlags() & QOrganizerItemFilter::MatchCaseSensitive) ? Qt::CaseSensitive : Qt::CaseInsensitive;

                /* See what flags are requested, since we're looking at a value */
                if (cdf.matchFlags() & (QOrganizerItemFilter::MatchEndsWith | QOrganizerItemFilter::MatchStartsWith | QOrganizerItemFilter::MatchContains | QOrganizerItemFilter::MatchFixedString)) {
                    /* We're strictly doing string comparisons here */
                    bool matchStarts = (cdf.matchFlags() & 7) == QOrganizerItemFilter::MatchStartsWith;
                    bool matchEnds = (cdf.matchFlags() & 7) == QOrganizerItemFilter::MatchEndsWith;
                    bool matchContains = (cdf.matchFlags() & 7) == QOrganizerItemFilter::MatchContains;

                    /* Value equality test */
                    for(int j=0; j < details.count(); j++) {
                        const QOrganizerItemDetail& detail = details.at(j);
                        const QString& var = detail.value(cdf.detailFieldName());
                        const QString& needle = cdf.value().toString();
                        if (matchStarts && var.startsWith(needle, cs))
                            return true;
                        if (matchEnds && var.endsWith(needle, cs))
                            return true;
                        if (matchContains && var.contains(needle, cs))
                            return true;
                        if (QString::compare(var, needle, cs) == 0)
                            return true;
                    }
                    return false;
                } else {
                    /* Nope, testing the values as a variant */
                    /* Value equality test */
                    for(int j = 0; j < details.count(); j++) {
                        const QOrganizerItemDetail& detail = details.at(j);
                        const QVariant& var = detail.variantValue(cdf.detailFieldName());
                        if (!var.isNull() && compareVariant(var, cdf.value(), cs) == 0)
                            return true;
                    }
                }
            }
            break;

        case QOrganizerItemFilter::OrganizerItemDateTimePeriodFilter:
            {
                const QOrganizerItemDateTimePeriodFilter dpf(filter);

                if (organizeritem.type() == QOrganizerItemType::TypeEvent) {
                    QOrganizerEvent event = organizeritem;
                    return (dpf.startPeriod().isNull() || event.startDateTime() >= dpf.startPeriod())
                            &&
                           (dpf.endPeriod().isNull() || event.endDateTime() <= dpf.endPeriod());
                }
                if (organizeritem.type() == QOrganizerItemType::TypeEventOccurrence) {
                   QOrganizerEventOccurrence eo = organizeritem;
                   return (dpf.startPeriod().isNull() || eo.startDateTime() >= dpf.startPeriod())
                           &&
                          (dpf.endPeriod().isNull() || eo.endDateTime() <= dpf.endPeriod());
                }
                if (organizeritem.type() == QOrganizerItemType::TypeTodo) {
                   QOrganizerTodo todo = organizeritem;
                   return (dpf.startPeriod().isNull() || todo.startDateTime() >= dpf.startPeriod())
                           &&
                          (dpf.endPeriod().isNull() || todo.dueDateTime() <= dpf.endPeriod());
                }
                if (organizeritem.type() == QOrganizerItemType::TypeTodoOccurrence) {
                    QOrganizerTodoOccurrence tdo = organizeritem;
                    return (dpf.startPeriod().isNull() || tdo.startDateTime() >= dpf.startPeriod())
                            &&
                           (dpf.endPeriod().isNull() || tdo.dueDateTime() <= dpf.endPeriod());
                }
                if (organizeritem.type() == QOrganizerItemType::TypeJournal) {
                    QOrganizerJournal journal = organizeritem;
                    return (dpf.startPeriod().isNull() || journal.dateTime() >= dpf.startPeriod())
                            &&
                           (dpf.endPeriod().isNull() || journal.dateTime() <= dpf.endPeriod());
                }
                if (organizeritem.type() == QOrganizerItemType::TypeNote) {
                    //for note, there is no such start/end datetime to be used, so we use the timestamp detail.
                    QOrganizerItemTimestamp timestamp = organizeritem.detail<QOrganizerItemTimestamp>();
                    return (dpf.startPeriod().isNull() || timestamp.lastModified() >= dpf.startPeriod())
                            &&
                           (dpf.endPeriod().isNull() || timestamp.lastModified() <= dpf.endPeriod());
                }
            }
            break;
        case QOrganizerItemFilter::OrganizerItemDetailRangeFilter:
            {
                const QOrganizerItemDetailRangeFilter cdf(filter);
                if (cdf.detailDefinitionName().isEmpty())
                    return false; /* we do not know which field to check */

                /* See if this organizer item has one of these details in it */
                const QList<QOrganizerItemDetail>& details = organizeritem.details(cdf.detailDefinitionName());

                if (details.count() == 0)
                    return false; /* can't match */

                /* Check for a detail presence test */
                if (cdf.detailFieldName().isEmpty())
                    return true;

                /* See if this is a field presence test */
                if (!cdf.minValue().isValid() && !cdf.maxValue().isValid()) {
                    for(int j=0; j < details.count(); j++) {
                        const QOrganizerItemDetail& detail = details.at(j);
                        if (detail.variantValues().contains(cdf.detailFieldName()))
                            return true;
                    }
                    return false;
                }

                /* open or closed interval testing support */
                const int minComp = cdf.rangeFlags() & QOrganizerItemDetailRangeFilter::ExcludeLower ? 1 : 0;
                const int maxComp = cdf.rangeFlags() & QOrganizerItemDetailRangeFilter::IncludeUpper ? 1 : 0;

                const bool testMin = cdf.minValue().isValid();
                const bool testMax = cdf.maxValue().isValid();

                /* At this point we know that at least of testMin & testMax is true */

                /* Case sensitivity, for those parts that use it */
                Qt::CaseSensitivity cs = (cdf.matchFlags() & QOrganizerItemFilter::MatchCaseSensitive) ? Qt::CaseSensitive : Qt::CaseInsensitive;

                /* See what flags are requested, since we're looking at a value */
                if (cdf.matchFlags() & (QOrganizerItemFilter::MatchEndsWith | QOrganizerItemFilter::MatchStartsWith | QOrganizerItemFilter::MatchContains | QOrganizerItemFilter::MatchFixedString)) {
                    /* We're strictly doing string comparisons here */
                    //bool matchStarts = (cdf.matchFlags() & 7) == QOrganizerItemFilter::MatchStartsWith;
                    bool matchEnds = (cdf.matchFlags() & 7) == QOrganizerItemFilter::MatchEndsWith;
                    bool matchContains = (cdf.matchFlags() & 7) == QOrganizerItemFilter::MatchContains;

                    /* Min/Max and contains do not make sense */
                    if (matchContains)
                        return false;

                    QString minVal = cdf.minValue().toString();
                    QString maxVal = cdf.maxValue().toString();

                    /* Starts with is the normal compare case, endsWith is a bit trickier */
                    for(int j=0; j < details.count(); j++) {
                        const QOrganizerItemDetail& detail = details.at(j);
                        const QString& var = detail.value(cdf.detailFieldName());
                        if (!matchEnds) {
                            // MatchStarts or MatchFixedString
                            if (testMin && QString::compare(var, minVal, cs) < minComp)
                                continue;
                            if (testMax && QString::compare(var, maxVal, cs) >= maxComp)
                                continue;
                            return true;
                        } else {
                            /* Have to test the length of min & max */
                            // using refs means the parameter order is backwards, so negate the result of compare
                            if (testMin && -QString::compare(minVal, var.rightRef(minVal.length()), cs) < minComp)
                                continue;
                            if (testMax && -QString::compare(maxVal, var.rightRef(maxVal.length()), cs) >= maxComp)
                                continue;
                            return true;
                        }
                    }
                    // Fall through to end
                } else {
                    /* Nope, testing the values as a variant */
                    for(int j=0; j < details.count(); j++) {
                        const QOrganizerItemDetail& detail = details.at(j);
                        const QVariant& var = detail.variantValue(cdf.detailFieldName());

                        if (testMin && compareVariant(var, cdf.minValue(), cs) < minComp)
                            continue;
                        if (testMax && compareVariant(var, cdf.maxValue(), cs) >= maxComp)
                            continue;
                        return true;
                    }
                    // Fall through to end
                }
            }
            break;

        case QOrganizerItemFilter::ChangeLogFilter:
            {
                QOrganizerItemChangeLogFilter ccf(filter);

                // See what we can do...
                QOrganizerItemTimestamp ts = organizeritem.detail(QOrganizerItemTimestamp::DefinitionName);

                // See if timestamps are even supported
                if (ts.isEmpty())
                    break;

                if (ccf.eventType() == QOrganizerItemChangeLogFilter::EventAdded)
                    return ccf.since() <= ts.created();
                if (ccf.eventType() == QOrganizerItemChangeLogFilter::EventChanged)
                    return ccf.since() <= ts.lastModified();

                // You can't emulate a removed..
                // Fall through to end
            }
            break;

        case QOrganizerItemFilter::IntersectionFilter:
            {
                /* XXX In theory we could reorder the terms to put the native tests first */
                const QOrganizerItemIntersectionFilter bf(filter);
                const QList<QOrganizerItemFilter>& terms = bf.filters();
                if (terms.count() > 0) {
                    for(int j = 0; j < terms.count(); j++) {
                        if (!testFilter(terms.at(j), organizeritem)) {
                            return false;
                        }
                    }
                    return true;
                }
                // Fall through to end
            }
            break;

        case QOrganizerItemFilter::UnionFilter:
            {
                /* XXX In theory we could reorder the terms to put the native tests first */
                const QOrganizerItemUnionFilter bf(filter);
                const QList<QOrganizerItemFilter>& terms = bf.filters();
                if (terms.count() > 0) {
                    for(int j = 0; j < terms.count(); j++) {
                        if (testFilter(terms.at(j), organizeritem)) {
                            return true;
                        }
                    }
                    return false;
                }
                // Fall through to end
            }
            break;
    }
    return false;
}


/*!
  Compares two organizer items (\a a and \a b) using the given list of \a sortOrders.  Returns a negative number if \a a should appear
  before \a b according to the sort order, a positive number if \a a should appear after \a b according to the sort order,
  and zero if the two are unable to be sorted.
 */
int QOrganizerItemManagerEngine::compareItem(const QOrganizerItem& a, const QOrganizerItem& b, const QList<QOrganizerItemSortOrder>& sortOrders)
{
    QList<QOrganizerItemSortOrder> copy = sortOrders;
    while (copy.size()) {
        // retrieve the next sort order in the list
        QOrganizerItemSortOrder sortOrder = copy.takeFirst();
        if (!sortOrder.isValid())
            break;

        // obtain the values which this sort order concerns
        const QVariant& aVal = a.detail(sortOrder.detailDefinitionName()).variantValue(sortOrder.detailFieldName());
        const QVariant& bVal = b.detail(sortOrder.detailDefinitionName()).variantValue(sortOrder.detailFieldName());

        bool aIsNull = false;
        bool bIsNull = false;

        // treat empty strings as null qvariants.
        if ((aVal.type() == QVariant::String && aVal.toString().isEmpty()) || aVal.isNull()) {
            aIsNull = true;
        }
        if ((bVal.type() == QVariant::String && bVal.toString().isEmpty()) || bVal.isNull()) {
            bIsNull = true;
        }

        // early exit error checking
        if (aIsNull && bIsNull)
            continue; // use next sort criteria.
        if (aIsNull)
            return (sortOrder.blankPolicy() == QOrganizerItemSortOrder::BlanksFirst ? -1 : 1);
        if (bIsNull)
            return (sortOrder.blankPolicy() == QOrganizerItemSortOrder::BlanksFirst ? 1 : -1);

        // real comparison
        int comparison = compareVariant(aVal, bVal, sortOrder.caseSensitivity()) * (sortOrder.direction() == Qt::AscendingOrder ? 1 : -1);
        if (comparison == 0)
            continue;
        return comparison;
    }

    return 0; // or according to id? return (a.id() < b.id() ? -1 : 1);
}


/*!
  Performs insertion sort of the item \a toAdd into the \a sorted list, according to the provided \a sortOrders list.
  The first QOrganizerItemSortOrder in the list has the highest priority; if the item \a toAdd is deemed equal to another
  in the \a sorted list, the second QOrganizerItemSortOrder in the list is used (and so on until either the item is inserted
  or there are no more sort order objects in the list).
 */
void QOrganizerItemManagerEngine::addSorted(QList<QOrganizerItem>* sorted, const QOrganizerItem& toAdd, const QList<QOrganizerItemSortOrder>& sortOrders)
{
    if (sortOrders.count() > 0) {
        for (int i = 0; i < sorted->size(); i++) {
            // check to see if the new item should be inserted here
            int comparison = compareItem(sorted->at(i), toAdd, sortOrders);
            if (comparison > 0) {
                sorted->insert(i, toAdd);
                return;
            }
        }
    }

    // hasn't been inserted yet?  append to the list.
    sorted->append(toAdd);
}

/*! Sorts the given list of organizer items \a cs according to the provided \a sortOrders */
QList<QOrganizerItemLocalId> QOrganizerItemManagerEngine::sortItems(const QList<QOrganizerItem>& cs, const QList<QOrganizerItemSortOrder>& sortOrders)
{
    QList<QOrganizerItemLocalId> sortedIds;
    QList<QOrganizerItem> sortedOrganizerItems;
    if (!sortOrders.isEmpty()) {
        foreach (const QOrganizerItem& c, cs) {
            QOrganizerItemManagerEngine::addSorted(&sortedOrganizerItems, c, sortOrders);
        }

        foreach(const QOrganizerItem& c, sortedOrganizerItems) {
            sortedIds.append(c.localId());
        }
    } else {
        foreach(const QOrganizerItem& c, cs) {
            sortedIds.append(c.localId());
        }
    }
    return sortedIds;
}

/*!
  Notifies the manager engine that the given request \a req has been destroyed.

  This notifies the engine that:
  \list
  \o the client doesn't care about the request any more.  The engine can still complete it,
     but completion is not required.
  \o it can't reliably access any properties of the request pointer any more.  The pointer will
     be invalid once this function returns.
  \endlist
  
  This means that if there is a worker thread, the engine needs to let that thread know that the
  request object is not valid and block until that thread acknowledges it.  One way to do this is to
  have a QSet<QOrganizerItemAbstractRequest*> (or QMap<QOrganizerItemAbstractRequest,
  MyCustomRequestState>) that tracks active requests, and insert into that set in startRequest, and
  remove in requestDestroyed (or when it finishes or is cancelled).  Protect that set/map with a
  mutex, and make sure you take the mutex in the worker thread before calling any of the
  QOrganizerItemAbstractRequest::updateXXXXXXRequest functions.  And be careful of lock ordering
  problems :D
 */
void QOrganizerItemManagerEngine::requestDestroyed(QOrganizerItemAbstractRequest* req)
{
    Q_UNUSED(req);
}

/*!
  Asks the manager engine to begin the given request \a req which is currently in a (re)startable
  state.  Returns true if the request was started successfully, else returns false.

  Generally, the engine queues the request and processes it at some later time (probably in another
  thread).
  
  Once a request is started, the engine should call the updateRequestState and/or the specific
  updateXXXXXRequest functions to mark it in the active state.
  
  If the engine is particularly fast, or the operation involves only in memory data, the request can
  be processed and completed without queueing it.
  
  Note that when the client is threaded, and the request might live on a different thread, the
  engine needs to be careful with locking.  In particular, the request might be deleted while the
  engine is still working on it.  In this case, the requestDestroyed function will be called while
  the request is still valid, and that function should block until the worker thread (etc.) has been
  notified not to touch that request any more.

  \sa QOrganizerItemAbstractRequest::start()
 */
bool QOrganizerItemManagerEngine::startRequest(QOrganizerItemAbstractRequest* req)
{
    Q_UNUSED(req);
    return false;
}

/*!
  Asks the manager engine to cancel the given request \a req which was
  previously started and is currently in a cancellable state.
  Returns true if cancellation of the request was started successfully,
  otherwise returns false.

  \sa startRequest(), QOrganizerItemAbstractRequest::cancel()
 */
bool QOrganizerItemManagerEngine::cancelRequest(QOrganizerItemAbstractRequest* req)
{
    Q_UNUSED(req);
    return false;
}

/*!
  Blocks until the manager engine has completed the given request \a req
  which was previously started, or until \a msecs milliseconds have passed.
  Returns true if the request was completed, and false if the request was not in the
  \c QOrganizerItemAbstractRequest::Active state or no progress could be reported.

  It is important that this function is implemented by the engine, at least merely as a delay, since
  clients may call it in a loop.

  \sa startRequest()
 */
bool QOrganizerItemManagerEngine::waitForRequestFinished(QOrganizerItemAbstractRequest* req, int msecs)
{
    Q_UNUSED(req);
    Q_UNUSED(msecs);
    return false;
}

/*!
  Updates the given asynchronous request \a req by setting the new \a state
  of the request.  If the new state is different, the stateChanged() signal
  will be emitted by the request.
 */
void QOrganizerItemManagerEngine::updateRequestState(QOrganizerItemAbstractRequest* req, QOrganizerItemAbstractRequest::State state)
{
    if (req->d_ptr->m_state != state) {
        req->d_ptr->m_state = state;
        emit req->stateChanged(state);
    }
}

/*!
  Updates the given QOrganizerItemInstanceFetchRequest \a req with the latest results \a result, and operation error \a error.
  In addition, the state of the request will be changed to \a newState.

  It then causes the request to emit its resultsAvailable() signal to notify clients of the request progress.

  If the new request state is different from the previous state, the stateChanged() signal will also be emitted from the request.
 */
void QOrganizerItemManagerEngine::updateItemInstanceFetchRequest(QOrganizerItemInstanceFetchRequest* req, const QList<QOrganizerItem>& result, QOrganizerItemManager::Error error, QOrganizerItemAbstractRequest::State newState)
{
    QOrganizerItemInstanceFetchRequestPrivate* rd = static_cast<QOrganizerItemInstanceFetchRequestPrivate*>(req->d_ptr);
    req->d_ptr->m_error = error;
    rd->m_organizeritems = result;
    bool emitState = rd->m_state != newState;
    rd->m_state = newState;
    emit req->resultsAvailable();
    if (emitState)
        emit req->stateChanged(newState);
}

/*!
  Updates the given QOrganizerItemLocalIdFetchRequest \a req with the latest results \a result, and operation error \a error.
  In addition, the state of the request will be changed to \a newState.

  It then causes the request to emit its resultsAvailable() signal to notify clients of the request progress.

  If the new request state is different from the previous state, the stateChanged() signal will also be emitted from the request.
 */
void QOrganizerItemManagerEngine::updateItemLocalIdFetchRequest(QOrganizerItemLocalIdFetchRequest* req, const QList<QOrganizerItemLocalId>& result, QOrganizerItemManager::Error error, QOrganizerItemAbstractRequest::State newState)
{
    QOrganizerItemLocalIdFetchRequestPrivate* rd = static_cast<QOrganizerItemLocalIdFetchRequestPrivate*>(req->d_ptr);
    req->d_ptr->m_error = error;
    rd->m_ids = result;
    bool emitState = rd->m_state != newState;
    rd->m_state = newState;
    emit req->resultsAvailable();
    if (emitState)
        emit req->stateChanged(newState);
}

/*!
  Updates the given QOrganizerItemFetchRequest \a req with the latest results \a result, and operation error \a error.
  In addition, the state of the request will be changed to \a newState.

  It then causes the request to emit its resultsAvailable() signal to notify clients of the request progress.

  If the new request state is different from the previous state, the stateChanged() signal will also be emitted from the request.
 */
void QOrganizerItemManagerEngine::updateItemFetchRequest(QOrganizerItemFetchRequest* req, const QList<QOrganizerItem>& result, QOrganizerItemManager::Error error, QOrganizerItemAbstractRequest::State newState)
{
    QOrganizerItemFetchRequestPrivate* rd = static_cast<QOrganizerItemFetchRequestPrivate*>(req->d_ptr);
    req->d_ptr->m_error = error;
    rd->m_organizeritems = result;
    bool emitState = rd->m_state != newState;
    rd->m_state = newState;
    emit req->resultsAvailable();
    if (emitState)
        emit req->stateChanged(newState);
}

/*!
  Updates the given QOrganizerItemRemoveRequest \a req with the operation error \a error, and map of input index to individual error \a errorMap.
  In addition, the state of the request will be changed to \a newState.

  It then causes the request to emit its resultsAvailable() signal to notify clients of the request progress.

  If the new request state is different from the previous state, the stateChanged() signal will also be emitted from the request.
 */
void QOrganizerItemManagerEngine::updateItemRemoveRequest(QOrganizerItemRemoveRequest* req, QOrganizerItemManager::Error error, const QMap<int, QOrganizerItemManager::Error>& errorMap, QOrganizerItemAbstractRequest::State newState)
{
    QOrganizerItemRemoveRequestPrivate* rd = static_cast<QOrganizerItemRemoveRequestPrivate*>(req->d_ptr);
    req->d_ptr->m_error = error;
    rd->m_errors = errorMap;
    bool emitState = rd->m_state != newState;
    rd->m_state = newState;
    emit req->resultsAvailable();
    if (emitState)
        emit req->stateChanged(newState);
}

/*!
  Updates the given QOrganizerItemSaveRequest \a req with the latest results \a result, operation error \a error, and map of input index to individual error \a errorMap.
  In addition, the state of the request will be changed to \a newState.

  It then causes the request to emit its resultsAvailable() signal to notify clients of the request progress.

  If the new request state is different from the previous state, the stateChanged() signal will also be emitted from the request.
 */
void QOrganizerItemManagerEngine::updateItemSaveRequest(QOrganizerItemSaveRequest* req, const QList<QOrganizerItem>& result, QOrganizerItemManager::Error error, const QMap<int, QOrganizerItemManager::Error>& errorMap, QOrganizerItemAbstractRequest::State newState)
{
    QOrganizerItemSaveRequestPrivate* rd = static_cast<QOrganizerItemSaveRequestPrivate*>(req->d_ptr);
    req->d_ptr->m_error = error;
    rd->m_errors = errorMap;
    rd->m_organizeritems = result;
    bool emitState = rd->m_state != newState;
    rd->m_state = newState;
    emit req->resultsAvailable();
    if (emitState)
        emit req->stateChanged(newState);
}

/*!
  Updates the given QOrganizerItemDetailDefinitionSaveRequest \a req with the latest results \a result, operation error \a error, and map of input index to individual error \a errorMap.
  In addition, the state of the request will be changed to \a newState.

  It then causes the request to emit its resultsAvailable() signal to notify clients of the request progress.

  If the new request state is different from the previous state, the stateChanged() signal will also be emitted from the request.
 */
void QOrganizerItemManagerEngine::updateDefinitionSaveRequest(QOrganizerItemDetailDefinitionSaveRequest* req, const QList<QOrganizerItemDetailDefinition>& result, QOrganizerItemManager::Error error, const QMap<int, QOrganizerItemManager::Error>& errorMap, QOrganizerItemAbstractRequest::State newState)
{
    QOrganizerItemDetailDefinitionSaveRequestPrivate* rd = static_cast<QOrganizerItemDetailDefinitionSaveRequestPrivate*>(req->d_ptr);
    req->d_ptr->m_error = error;
    rd->m_errors = errorMap;
    rd->m_definitions = result;
    bool emitState = rd->m_state != newState;
    rd->m_state = newState;
    emit req->resultsAvailable();
    if (emitState)
        emit req->stateChanged(newState);
}

/*!
  Updates the given QOrganizerItemDetailDefinitionRemoveRequest \a req with the operation error \a error, and map of input index to individual error \a errorMap.
  In addition, the state of the request will be changed to \a newState.

  It then causes the request to emit its resultsAvailable() signal to notify clients of the request progress.

  If the new request state is different from the previous state, the stateChanged() signal will also be emitted from the request.
 */
void QOrganizerItemManagerEngine::updateDefinitionRemoveRequest(QOrganizerItemDetailDefinitionRemoveRequest* req, QOrganizerItemManager::Error error, const QMap<int, QOrganizerItemManager::Error>& errorMap, QOrganizerItemAbstractRequest::State newState)
{
    QOrganizerItemDetailDefinitionRemoveRequestPrivate* rd = static_cast<QOrganizerItemDetailDefinitionRemoveRequestPrivate*>(req->d_ptr);
    req->d_ptr->m_error = error;
    rd->m_errors = errorMap;
    bool emitState = rd->m_state != newState;
    rd->m_state = newState;
    emit req->resultsAvailable();
    if (emitState)
        emit req->stateChanged(newState);
}

/*!
  Updates the given QOrganizerItemDetailDefinitionFetchRequest \a req with the latest results \a result, operation error \a error, and map of input index to individual error \a errorMap.
  In addition, the state of the request will be changed to \a newState.

  It then causes the request to emit its resultsAvailable() signal to notify clients of the request progress.

  If the new request state is different from the previous state, the stateChanged() signal will also be emitted from the request.
 */
void QOrganizerItemManagerEngine::updateDefinitionFetchRequest(QOrganizerItemDetailDefinitionFetchRequest* req, const QMap<QString, QOrganizerItemDetailDefinition>& result, QOrganizerItemManager::Error error, const QMap<int, QOrganizerItemManager::Error>& errorMap, QOrganizerItemAbstractRequest::State newState)
{
    QOrganizerItemDetailDefinitionFetchRequestPrivate* rd = static_cast<QOrganizerItemDetailDefinitionFetchRequestPrivate*>(req->d_ptr);
    req->d_ptr->m_error = error;
    rd->m_errors = errorMap;
    rd->m_definitions = result;
    bool emitState = rd->m_state != newState;
    rd->m_state = newState;
    emit req->resultsAvailable();
    if (emitState)
        emit req->stateChanged(newState);
}


#include "moc_qorganizeritemmanagerengine.cpp"

QTM_END_NAMESPACE
