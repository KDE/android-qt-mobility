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

#include "qversitorganizerexporter.h"
#include "qversitorganizerexporter_p.h"
#include "qtorganizer.h"
#include "versitutils_p.h"
#include "qversitorganizerdefs_p.h"
#include "qversitdocument.h"
#include "qversitproperty.h"
#include "qversitpluginloader_p.h"

QTM_USE_NAMESPACE

QVersitOrganizerExporterPrivate::QVersitOrganizerExporterPrivate(const QString& profile) :
    mDetailHandler(NULL)
{
    int versitPropertyCount =
        sizeof(versitOrganizerDetailMappings)/sizeof(VersitDetailMapping);
    for (int i = 0; i < versitPropertyCount; i++) {
        mPropertyMappings.insert(
                QLatin1String(versitOrganizerDetailMappings[i].detailDefinitionName),
                QPair<QString,QString>(
                    QLatin1String(versitOrganizerDetailMappings[i].detailFieldName),
                    QLatin1String(versitOrganizerDetailMappings[i].versitPropertyName)));
    }

    mPluginDetailHandlers = QVersitPluginLoader::instance()->createOrganizerHandlers(profile);
    mTimeZoneHandler = QVersitPluginLoader::instance()->timeZoneHandler();
}

QVersitOrganizerExporterPrivate::~QVersitOrganizerExporterPrivate()
{
}

bool QVersitOrganizerExporterPrivate::exportItem(
    const QOrganizerItem& item,
    QVersitDocument* document,
    QVersitOrganizerExporter::Error* error)
{
    if (item.type() == QOrganizerItemType::TypeEvent
        || item.type() == QOrganizerItemType::TypeEventOccurrence) {
        document->setComponentType(QLatin1String("VEVENT"));
    } else if (item.type() == QOrganizerItemType::TypeTodo
        || item.type() == QOrganizerItemType::TypeTodoOccurrence) {
        document->setComponentType(QLatin1String("VTODO"));
    } else if (item.type() == QOrganizerItemType::TypeJournal) {
        document->setComponentType(QLatin1String("VJOURNAL"));
    } else {
        *error = QVersitOrganizerExporter::UnknownComponentTypeError;
        return false;
    }
    QList<QOrganizerItemDetail> allDetails = item.details();
    if (allDetails.isEmpty()) {
        *error = QVersitOrganizerExporter::EmptyOrganizerError;
        return false;
    }
    foreach (const QOrganizerItemDetail& detail, allDetails) {
        exportDetail(item, detail, document);
    }

    // run plugin handlers
    foreach (QVersitOrganizerExporterDetailHandler* handler, mPluginDetailHandlers) {
        handler->itemProcessed(item, document);
    }
    // run the handler, if set
    if (mDetailHandler) {
        mDetailHandler->itemProcessed(item, document);
    }
    if (item.type() == QOrganizerItemType::TypeEventOccurrence
            && !documentContainsUidAndRecurrenceId(*document)) {
        *error = QVersitOrganizerExporter::UnderspecifiedOccurrenceError;
        return false;
    }
    return true;
}

void QVersitOrganizerExporterPrivate::exportDetail(
        const QOrganizerItem& item,
        const QOrganizerItemDetail& detail,
        QVersitDocument* document)
{
    QList<QVersitProperty> removedProperties;
    QList<QVersitProperty> generatedProperties;
    QSet<QString> processedFields;

    if (detail.definitionName() == QOrganizerEventTimeRange::DefinitionName) {
        encodeEventTimeRange(detail, *document, &removedProperties, &generatedProperties, &processedFields);
    } else if (detail.definitionName() == QOrganizerTodoTimeRange::DefinitionName) {
        encodeTodoTimeRange(detail, *document, &removedProperties, &generatedProperties, &processedFields);
    } else if (detail.definitionName() == QOrganizerJournalTimeRange::DefinitionName) {
        encodeJournalTimeRange(detail, *document, &removedProperties, &generatedProperties, &processedFields);
    } else if (detail.definitionName() == QOrganizerItemTimestamp::DefinitionName) {
        encodeTimestamp(detail, *document, &removedProperties, &generatedProperties, &processedFields);
    } else if (detail.definitionName() == QOrganizerItemRecurrence::DefinitionName) {
        encodeRecurrence(item, detail, *document, &removedProperties, &generatedProperties, &processedFields);
    } else if (detail.definitionName() == QOrganizerItemPriority::DefinitionName) {
        encodePriority(detail, *document, &removedProperties, &generatedProperties, &processedFields);
    } else if (detail.definitionName() == QOrganizerItemInstanceOrigin::DefinitionName) {
        encodeInstanceOrigin(detail, *document, &removedProperties, &generatedProperties, &processedFields);
    } else if (detail.definitionName() == QOrganizerTodoProgress::DefinitionName) {
        encodeTodoProgress(detail, *document, &removedProperties, &generatedProperties, &processedFields);
    } else if (detail.definitionName() == QOrganizerItemComment::DefinitionName) {
        encodeComment(detail, &generatedProperties, &processedFields);
    } else if (mPropertyMappings.contains(detail.definitionName())) {
        encodeSimpleProperty(detail, *document, &removedProperties, &generatedProperties, &processedFields);
    }

    // run the plugin handler
    foreach (QVersitOrganizerExporterDetailHandler* handler, mPluginDetailHandlers) {
        handler->detailProcessed(item, detail, *document,
                                 &processedFields, &removedProperties, &generatedProperties);
    }
    // run the detail handler, if set
    if (mDetailHandler) {
        mDetailHandler->detailProcessed(item, detail, *document,
                                        &processedFields, &removedProperties, &generatedProperties);
    }

    foreach(const QVersitProperty& property, removedProperties) {
        document->removeProperty(property);
    }
    foreach(const QVersitProperty& property, generatedProperties) {
        document->addProperty(property);
    }
}

void QVersitOrganizerExporterPrivate::encodeEventTimeRange(
        const QOrganizerItemDetail& detail,
        const QVersitDocument& document,
        QList<QVersitProperty>* removedProperties,
        QList<QVersitProperty>* generatedProperties,
        QSet<QString>* processedFields)
{
    QOrganizerEventTimeRange etr = static_cast<QOrganizerEventTimeRange>(detail);
    QVersitProperty property =
        VersitUtils::takeProperty(document, QLatin1String("DTSTART"), removedProperties);
    property.setName(QLatin1String("DTSTART"));
    property.setValue(encodeDateTime(etr.startDateTime()));
    *generatedProperties << property;

    property =
        VersitUtils::takeProperty(document, QLatin1String("DTEND"), removedProperties);
    property.setName(QLatin1String("DTEND"));
    property.setValue(encodeDateTime(etr.endDateTime()));
    *generatedProperties << property;
    *processedFields << QOrganizerEventTimeRange::FieldStartDateTime
                     << QOrganizerEventTimeRange::FieldEndDateTime;
}

void QVersitOrganizerExporterPrivate::encodeTodoTimeRange(
        const QOrganizerItemDetail& detail,
        const QVersitDocument& document,
        QList<QVersitProperty>* removedProperties,
        QList<QVersitProperty>* generatedProperties,
        QSet<QString>* processedFields)
{
    QOrganizerTodoTimeRange ttr = static_cast<QOrganizerTodoTimeRange>(detail);
    QVersitProperty property =
        VersitUtils::takeProperty(document, QLatin1String("DTSTART"), removedProperties);
    property.setName(QLatin1String("DTSTART"));
    property.setValue(encodeDateTime(ttr.startDateTime()));
    *generatedProperties << property;

    property =
        VersitUtils::takeProperty(document, QLatin1String("DUE"), removedProperties);
    property.setName(QLatin1String("DUE"));
    property.setValue(encodeDateTime(ttr.dueDateTime()));
    *generatedProperties << property;
    *processedFields << QOrganizerTodoTimeRange::FieldStartDateTime
                     << QOrganizerTodoTimeRange::FieldDueDateTime;
}

void QVersitOrganizerExporterPrivate::encodeJournalTimeRange(
        const QOrganizerItemDetail& detail,
        const QVersitDocument& document,
        QList<QVersitProperty>* removedProperties,
        QList<QVersitProperty>* generatedProperties,
        QSet<QString>* processedFields)
{
    QOrganizerJournalTimeRange jtr = static_cast<QOrganizerJournalTimeRange>(detail);
    QVersitProperty property =
        VersitUtils::takeProperty(document, QLatin1String("DTSTART"), removedProperties);
    property.setName(QLatin1String("DTSTART"));
    property.setValue(encodeDateTime(jtr.entryDateTime()));
    *generatedProperties << property;
    *processedFields << QOrganizerJournalTimeRange::FieldEntryDateTime;
}

void QVersitOrganizerExporterPrivate::encodeTimestamp(
        const QOrganizerItemDetail& detail,
        const QVersitDocument& document,
        QList<QVersitProperty>* removedProperties,
        QList<QVersitProperty>* generatedProperties,
        QSet<QString>* processedFields)
{
    QOrganizerItemTimestamp timestamp = static_cast<QOrganizerItemTimestamp>(detail);
    QVersitProperty property =
        VersitUtils::takeProperty(document, QLatin1String("CREATED"), removedProperties);
    property.setName(QLatin1String("CREATED"));
    property.setValue(encodeDateTime(timestamp.created().toUTC()));
    *generatedProperties << property;

    property =
        VersitUtils::takeProperty(document, QLatin1String("LAST-MODIFIED"), removedProperties);
    property.setName(QLatin1String("LAST-MODIFIED"));
    property.setValue(encodeDateTime(timestamp.lastModified().toUTC()));
    *generatedProperties << property;
    *processedFields << QOrganizerItemTimestamp::FieldCreationTimestamp
                     << QOrganizerItemTimestamp::FieldModificationTimestamp;
}

void QVersitOrganizerExporterPrivate::encodeRecurrence(
        const QOrganizerItem& item,
        const QOrganizerItemDetail& detail,
        const QVersitDocument& document,
        QList<QVersitProperty>* removedProperties,
        QList<QVersitProperty>* generatedProperties,
        QSet<QString>* processedFields)
{
    QOrganizerItemRecurrence recurrence = static_cast<QOrganizerItemRecurrence>(detail);
    QList<QOrganizerItemRecurrenceRule> rrules = recurrence.recurrenceRules();
    QList<QOrganizerItemRecurrenceRule> exrules = recurrence.exceptionRules();
    QList<QDate> rdates = recurrence.recurrenceDates();
    QList<QDate> exdates = recurrence.exceptionDates();
    if (!rrules.isEmpty()) {
        foreach (const QOrganizerItemRecurrenceRule& rrule, rrules) {
            encodeRecurRule(QLatin1String("RRULE"), rrule, generatedProperties);
        }
        *processedFields << QOrganizerItemRecurrence::FieldRecurrenceRules;
    }
    if (!exrules.isEmpty()) {
        foreach (const QOrganizerItemRecurrenceRule& exrule, exrules) {
            encodeRecurRule(QLatin1String("EXRULE"), exrule, generatedProperties);
        }
        *processedFields << QOrganizerItemRecurrence::FieldExceptionRules;
    }
    if (!rdates.isEmpty()) {
        encodeRecurDates(QLatin1String("RDATE"), item, rdates, document, removedProperties, generatedProperties);
        *processedFields << QOrganizerItemRecurrence::FieldRecurrenceDates;
    }
    if (!exdates.isEmpty()) {
        encodeRecurDates(QLatin1String("EXDATE"), item, exdates, document, removedProperties, generatedProperties);
        *processedFields << QOrganizerItemRecurrence::FieldExceptionDates;
    }
}

/*! Encodes an iCalendar recurrence specification from \a rule. */
void QVersitOrganizerExporterPrivate::encodeRecurRule(
        const QString& propertyName,
        const QOrganizerItemRecurrenceRule& rule,
        QList<QVersitProperty>* generatedProperties)
{
    QVersitProperty property;
    property.setName(propertyName);
    QString value = QLatin1String("FREQ=");
    switch (rule.frequency()) {
        case QOrganizerItemRecurrenceRule::Daily:
            value.append(QLatin1String("DAILY"));
            break;
        case QOrganizerItemRecurrenceRule::Weekly:
            value.append(QLatin1String("WEEKLY"));
            break;
        case QOrganizerItemRecurrenceRule::Monthly:
            value.append(QLatin1String("MONTHLY"));
            break;
        case QOrganizerItemRecurrenceRule::Yearly:
            value.append(QLatin1String("YEARLY"));
            break;
    }
    if (rule.count() > 0) {
        value.append(QLatin1String(";COUNT="));
        value.append(QString::number(rule.count()));
    }
    if (rule.endDate().isValid()) {
        value.append(QLatin1String(";UNTIL="));
        value.append(rule.endDate().toString(QLatin1String("yyyyMMdd")));
    }
    if (rule.interval() > 1) {
        value.append(QLatin1String(";INTERVAL="));
        value.append(QString::number(rule.interval()));
    }
    if (!rule.daysOfWeek().isEmpty()) {
        value.append(QLatin1String(";BYDAY="));
        bool first = true;
        foreach (Qt::DayOfWeek day, rule.daysOfWeek()) {
            if (!first)
                value.append(QLatin1String(","));
            first = false;
            value.append(weekString(day));
        }
    }
    if (!rule.daysOfMonth().isEmpty()) {
        value.append(QLatin1String(";BYMONTHDAY="));
        appendIntList(&value, rule.daysOfMonth());
    }
    if (!rule.daysOfYear().isEmpty()) {
        value.append(QLatin1String(";BYYEARDAY="));
        appendIntList(&value, rule.daysOfYear());
    }
    if (!rule.weeksOfYear().isEmpty()) {
        value.append(QLatin1String(";BYWEEKNO="));
        appendIntList(&value, rule.weeksOfYear());
    }
    if (!rule.months().isEmpty()) {
        value.append(QLatin1String(";BYMONTH="));
        bool first = true;
        foreach (QOrganizerItemRecurrenceRule::Month month, rule.months()) {
            if (!first)
                value.append(QLatin1String(","));
            first = false;
            value.append(QString::number(month));
        }
    }
    if (!rule.positions().isEmpty()) {
        value.append(QLatin1String(";BYSETPOS="));
        appendIntList(&value, rule.positions());
    }
    if (rule.weekStart() != Qt::Monday && rule.weekStart() > 0) {
        value.append(QLatin1String(";WKST="));
        value.append(weekString(rule.weekStart()));
    }
    property.setValue(value);
    *generatedProperties << property;
}

/*! Joins \a list together with commas and appends the result to \a str.
 */
void QVersitOrganizerExporterPrivate::appendIntList(QString* str, const QList<int>& list) {
    bool first = true;
    foreach (int n, list) {
        if (!first)
            str->append(QLatin1String(","));
        first = false;
        str->append(QString::number(n));
    }
}

/*! Converts \a day to a two-character iCalendar string */
QString QVersitOrganizerExporterPrivate::weekString(Qt::DayOfWeek day) {
    switch (day) {
        case Qt::Monday:
            return QLatin1String("MO");
        case Qt::Tuesday:
            return QLatin1String("TU");
        case Qt::Wednesday:
            return QLatin1String("WE");
        case Qt::Thursday:
            return QLatin1String("TH");
        case Qt::Friday:
            return QLatin1String("FR");
        case Qt::Saturday:
            return QLatin1String("SA");
        case Qt::Sunday:
            return QLatin1String("SU");
        default:
            return QString();
    }
}

void QVersitOrganizerExporterPrivate::encodeRecurDates(
        const QString& propertyName,
        const QOrganizerItem& item,
        const QList<QDate>& dates,
        const QVersitDocument& document,
        QList<QVersitProperty>* removedProperties,
        QList<QVersitProperty>* generatedProperties)
{
    QVersitProperty property;
    property = VersitUtils::takeProperty(document, propertyName, removedProperties);
    property.setName(propertyName);
    property.insertParameter(QLatin1String("VALUE"), QLatin1String("DATE"));
    QString value = property.value();
    bool valueIsEmpty = value.isEmpty();

    foreach (const QDate& dt, dates) {
        QString str;
        QTime eventTime = static_cast<QOrganizerEvent>(item).startDateTime().time();
        if (dt.isValid()) {
            str = dt.toString(QLatin1String("yyyyMMdd"));
            if (!valueIsEmpty)
                value += QLatin1Char(',');
            value += str;
            valueIsEmpty = false;
        }
    }
    property.setValue(value);
    *generatedProperties << property;
}

void QVersitOrganizerExporterPrivate::encodePriority(
        const QOrganizerItemDetail& detail,
        const QVersitDocument& document,
        QList<QVersitProperty>* removedProperties,
        QList<QVersitProperty>* generatedProperties,
        QSet<QString>* processedFields)
{
    QOrganizerItemPriority priority = static_cast<QOrganizerItemPriority>(detail);
    QVersitProperty property =
        VersitUtils::takeProperty(document, QLatin1String("PRIORITY"), removedProperties);
    property.setName(QLatin1String("PRIORITY"));
    property.setValue(QString::number(priority.priority()));
    *generatedProperties << property;
    *processedFields << QOrganizerItemPriority::FieldPriority;
}

void QVersitOrganizerExporterPrivate::encodeInstanceOrigin(
        const QOrganizerItemDetail& detail,
        const QVersitDocument& document,
        QList<QVersitProperty>* removedProperties,
        QList<QVersitProperty>* generatedProperties,
        QSet<QString>* processedFields)
{
    QOrganizerItemInstanceOrigin instanceOrigin = static_cast<QOrganizerItemInstanceOrigin>(detail);
    QVersitProperty property =
        VersitUtils::takeProperty(document, QLatin1String("RECURRENCE-ID"), removedProperties);
    property.setName(QLatin1String("RECURRENCE-ID"));
    property.setValue(instanceOrigin.originalDate().toString(QLatin1String("yyyyMMdd")));
    *generatedProperties << property;
    *processedFields << QOrganizerItemInstanceOrigin::FieldOriginalDate;
}

void QVersitOrganizerExporterPrivate::encodeTodoProgress(
        const QOrganizerItemDetail& detail,
        const QVersitDocument& document,
        QList<QVersitProperty>* removedProperties,
        QList<QVersitProperty>* generatedProperties,
        QSet<QString>* processedFields)
{
    QOrganizerTodoProgress todoProgress = static_cast<QOrganizerTodoProgress>(detail);

    if (todoProgress.finishedDateTime().isValid()) {
        QVersitProperty property =
            VersitUtils::takeProperty(document, QLatin1String("COMPLETED"), removedProperties);
        property.setName(QLatin1String("COMPLETED"));
        property.setValue(todoProgress.finishedDateTime().toString(QLatin1String("yyyyMMddTHHmmss")));
        *generatedProperties << property;
        *processedFields << QOrganizerTodoProgress::FieldFinishedDateTime;
    }

    if (todoProgress.hasValue(QOrganizerTodoProgress::FieldPercentageComplete)) {
        QVersitProperty property =
            VersitUtils::takeProperty(document, QLatin1String("PERCENT-COMPLETE"), removedProperties);
        property.setName(QLatin1String("PERCENT-COMPLETE"));
        property.setValue(QString::number(todoProgress.percentageComplete()));
        *generatedProperties << property;
        *processedFields << QOrganizerTodoProgress::FieldPercentageComplete;
    }

    if (todoProgress.hasValue(QOrganizerTodoProgress::FieldStatus)) {
        QVersitProperty property =
            VersitUtils::takeProperty(document, QLatin1String("STATUS"), removedProperties);
        property.setName(QLatin1String("STATUS"));
        switch (todoProgress.status()) {
            case QOrganizerTodoProgress::StatusNotStarted:
                property.setValue(QLatin1String("NEEDS-ACTION"));
                break;
            case QOrganizerTodoProgress::StatusInProgress:
                property.setValue(QLatin1String("IN-PROCESS"));
                break;
            case QOrganizerTodoProgress::StatusComplete:
                property.setValue(QLatin1String("COMPLETED"));
                break;
            default:
                return;
        }
        *generatedProperties << property;
        *processedFields << QOrganizerTodoProgress::FieldStatus;
    }
}

void QVersitOrganizerExporterPrivate::encodeComment(
        const QOrganizerItemDetail& detail,
        QList<QVersitProperty>* generatedProperties,
        QSet<QString>* processedFields)
{
    QOrganizerItemComment comment = static_cast<QOrganizerItemComment>(detail);
    QVersitProperty property;
    property.setName(QLatin1String("COMMENT"));
    property.setValue(comment.comment());
    *generatedProperties << property;
    *processedFields << QOrganizerItemComment::FieldComment;
}

void QVersitOrganizerExporterPrivate::encodeSimpleProperty(
        const QOrganizerItemDetail& detail,
        const QVersitDocument& document,
        QList<QVersitProperty>* removedProperties,
        QList<QVersitProperty>* generatedProperties,
        QSet<QString>* processedFields)
{
    QPair<QString, QString> fieldPropertyMap = mPropertyMappings[detail.definitionName()];
    const QString& fieldName = fieldPropertyMap.first;
    const QString& propertyName = fieldPropertyMap.second;
    QVersitProperty property =
        VersitUtils::takeProperty(document, propertyName, removedProperties);
    property.setName(propertyName);
    property.setValue(detail.value(fieldName));
    *generatedProperties << property;
    *processedFields << fieldName;
}

/*! Formats \a dateTime in ISO 8601 basic format */
QString QVersitOrganizerExporterPrivate::encodeDateTime(const QDateTime& dateTime)
{
    if (dateTime.timeSpec() == Qt::UTC)
        return dateTime.toString(QLatin1String("yyyyMMddTHHmmssZ"));
    else
        return dateTime.toString(QLatin1String("yyyyMMddTHHmmss"));
}

/*!
 * Returns true if and only if \a document has both the UID and the RECURRENCE-ID properties
 */
bool QVersitOrganizerExporterPrivate::documentContainsUidAndRecurrenceId(const QVersitDocument &document)
{
    bool hasUid = false;
    bool hasRecurId = false;
    foreach (const QVersitProperty& property, document.properties()) {
        const QString& name = property.name();
        if (name == QLatin1String("UID")) {
            if (hasRecurId)
                return true;
            hasUid = true;
        } else if (name == QLatin1String("RECURRENCE-ID")) {
            if (hasUid)
                return true;
            hasRecurId = true;
        }
    }
    return false;
}
