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

#include "qorganizerrecurrencetransform.h"
#include "qtorganizer.h"

#include <CRecurrence.h>
#include <CRecurrenceRule.h>

QTM_USE_NAMESPACE

OrganizerRecurrenceTransform::OrganizerRecurrenceTransform()
    : m_rtype(0)
{
}

OrganizerRecurrenceTransform::~OrganizerRecurrenceTransform()
{
    beginTransformToCrecurrence(); // frees the resources
}

void OrganizerRecurrenceTransform::beginTransformToCrecurrence()
{
    std::vector< CRecurrenceRule* >::iterator i;
    for (i = m_vRRuleList.begin(); i != m_vRRuleList.end(); ++i)
        delete *i;

    m_rtype = 0;
    m_vRRuleList.clear();
    m_vRecDateList.clear();
    m_vExceptionDateList.clear();
}

CRecurrence* OrganizerRecurrenceTransform::crecurrence(bool *success) const
{
    if (success)
        *success = false;

    if (m_vRRuleList.empty() && m_vRecDateList.empty() && m_vExceptionDateList.empty())
    {
        if (success)
            *success = true;
        return 0; // success, but no recurrence information
    }

    CRecurrence *retn = new CRecurrence();

    // Set the recurrence type
    retn->setRtype(m_rtype);

    // Add the recursion and exception rules
    if (!m_vRRuleList.empty()) {
        // Make a copy of the rules
        std::vector< CRecurrenceRule* > copyOfRules;
        std::vector< CRecurrenceRule* >::const_iterator i;
        for (i = m_vRRuleList.begin(); i != m_vRRuleList.end(); ++i) {
            CRecurrenceRule *curr = *i;
            CRecurrenceRule *copyOfRule = new CRecurrenceRule(*curr);
            copyOfRules.push_back(copyOfRule);
        }

        // Setting the rules transfers ownership of all the rules
        if (!retn->setRecurrenceRule(copyOfRules)) {
            // If failed, the ownership was not transferred
            std::vector< CRecurrenceRule* >::iterator i;
            for (i = copyOfRules.begin(); i != copyOfRules.end(); ++i)
                delete *i;

            delete retn;
            return 0; // failed
        }
    }

    // Set the recursion dates
    if (!m_vRecDateList.empty()) {
        if (!retn->setRDays(m_vRecDateList)) {
            delete retn;
            return 0; // failed
        }
    }

    // Set the exception dates
    if (!m_vExceptionDateList.empty()) {
        if (!retn->setEDays(m_vExceptionDateList)) {
            delete retn;
            return 0; // failed
        }
    }

    if (success)
        *success = true;

    return retn;
}

void OrganizerRecurrenceTransform::addQOrganizerItemRecurrenceRule(const QOrganizerItemRecurrenceRule& rule)
{
    QString icalRule = qrecurrenceRuleToIcalRecurrenceRule(rule);

    // Store the new rule to the rule vector
    CRecurrenceRule *crecrule = new CRecurrenceRule();
    crecrule->setRuleType(RECURRENCE_RULE);
    crecrule->setRrule(icalRule.toStdString());
    m_vRRuleList.push_back(crecrule);

    // Update the recursion type, set according to the most frequent rule
    int ruleRtype = qfrequencyToRtype(rule.frequency());
    m_rtype = m_rtype ? ( ruleRtype < m_rtype ? ruleRtype : m_rtype ) : ruleRtype;
}

void OrganizerRecurrenceTransform::addQOrganizerItemExceptionRule(const QOrganizerItemRecurrenceRule& rule)
{
    QString icalRule = qrecurrenceRuleToIcalRecurrenceRule(rule);

    // Store the new rule to the rule vector
    CRecurrenceRule *crecrule = new CRecurrenceRule();
    crecrule->setRuleType(EXCEPTION_RULE);
    crecrule->setRrule(icalRule.toStdString());
    m_vRRuleList.push_back(crecrule);
}

void OrganizerRecurrenceTransform::addQOrganizerItemRecurrenceDate(const QDate &date)
{
    QString icalDate = qrecurrenceDateToIcalRecurrenceDate(date);

    // Store the new date to the recurrence date vector
    m_vRecDateList.push_back(icalDate.toStdString());

    // A new recurrence rule must also be set, because the Fremantle calendar
    // does not care of the recurrence dates
    CRecurrenceRule *crecrule = new CRecurrenceRule();
    crecrule->setRuleType(RECURRENCE_RULE);
    crecrule->setRrule(createSingleDayRuleFor(date).toStdString());
    m_vRRuleList.push_back(crecrule);

    if (!m_rtype)
        m_rtype = qfrequencyToRtype(QOrganizerItemRecurrenceRule::Yearly);
}

void OrganizerRecurrenceTransform::addQOrganizerItemExceptionDate(const QDate &date)
{
    QString icalDate = qrecurrenceDateToIcalRecurrenceDate(date);

    // Store the new date to the exception date vector
    m_vExceptionDateList.push_back(icalDate.toStdString());

    // A new exception rule must also be set, because the Fremantle calendar
    // does not care of the exception dates
    CRecurrenceRule *crecrule = new CRecurrenceRule();
    crecrule->setRuleType(EXCEPTION_RULE);
    crecrule->setRrule(createSingleDayRuleFor(date).toStdString());
    m_vRRuleList.push_back(crecrule);
}

QString OrganizerRecurrenceTransform::qrecurrenceRuleToIcalRecurrenceRule(const QOrganizerItemRecurrenceRule& rule) const
{
    QStringList icalRule;

    icalRule << qfrequencyToIcalFrequency(rule.frequency());

    if (rule.count() > 0)
        icalRule << qcountToIcalCount(rule.count());
    else if (rule.endDate() != QDate())
        icalRule << qendDateToIcalUntil(rule.endDate().addDays(-1)); // in Maemo5 the end date is inclusive

    icalRule << qintervalToIcalInterval(rule.interval());

    QList<QOrganizerItemRecurrenceRule::Month> months(rule.months());
    if (!months.isEmpty())
        icalRule << qmonthsToIcalByMonth(months);

    QList<Qt::DayOfWeek> daysOfWeek(rule.daysOfWeek());
    if (!daysOfWeek.isEmpty())
        icalRule << qdaysOfWeekToIcalByDay(daysOfWeek);

    QList<int> daysOfMonth(rule.daysOfMonth());
    if (!daysOfMonth.isEmpty())
        icalRule << qdaysOfMonthToIcalByMonthDay(daysOfMonth);

    QList<int> daysOfYear(rule.daysOfYear());
    if (!daysOfYear.isEmpty())
        icalRule << qdaysOfYearToIcalByYearDay(daysOfYear);

    QList<int> weeksOfYear(rule.weeksOfYear());
    QList<int>::iterator weekOfYear;
    for (weekOfYear = weeksOfYear.begin(); weekOfYear != weeksOfYear.end(); ++weekOfYear)
        --(*weekOfYear); // in Maemo5 the week numbers start from zero

    if (!weeksOfYear.isEmpty())
        icalRule << qweeksOfYearToIcalByWeekNo(weeksOfYear);

    QList<int> positions(rule.positions());
    if (!positions.isEmpty())
        icalRule << qpositionsToIcalBySetPos(positions);

    icalRule << qweekStartToIcalWkst(rule.weekStart());

    return icalRule.join(";");
}

QString OrganizerRecurrenceTransform::qfrequencyToIcalFrequency(QOrganizerItemRecurrenceRule::Frequency frequency) const
{
    switch( frequency )
    {
    case QOrganizerItemRecurrenceRule::Daily: return QString("FREQ=DAILY");
    case QOrganizerItemRecurrenceRule::Weekly: return QString("FREQ=WEEKLY");
    case QOrganizerItemRecurrenceRule::Monthly: return QString("FREQ=MONTHLY");
    case QOrganizerItemRecurrenceRule::Yearly: return QString("FREQ=YEARLY");
    }
}

QString OrganizerRecurrenceTransform::qcountToIcalCount(int count) const
{
    return QString("COUNT=") + QString::number(count);
}

QString OrganizerRecurrenceTransform::qintervalToIcalInterval(int interval) const
{
    return QString("INTERVAL=") + QString::number(interval);
}

QString OrganizerRecurrenceTransform::qendDateToIcalUntil(QDate endDate) const
{
    return QString("UNTIL=") + endDate.toString("yyyyMMdd") + "T235959Z";
}

QString OrganizerRecurrenceTransform::qdaysOfWeekToIcalByDay(const QList<Qt::DayOfWeek> &daysOfWeek) const
{
    QStringList slist;
    foreach (Qt::DayOfWeek day, daysOfWeek)
        slist << qweekdayToIcalWeekday( day );
    return QString("BYDAY=") + slist.join(",");
}

QString OrganizerRecurrenceTransform::qweekStartToIcalWkst(Qt::DayOfWeek dayOfWeek) const
{
    QString dayOfWeekString(qweekdayToIcalWeekday(dayOfWeek));
    if (dayOfWeekString.isNull())
        dayOfWeekString = QString("MO"); // set Monday as default
    return QString("WKST=") + dayOfWeekString;
}

QString OrganizerRecurrenceTransform::qweekdayToIcalWeekday(Qt::DayOfWeek dayOfWeek) const
{
    QMap<QString, Qt::DayOfWeek> mapping = icalRecurrenceWeekDayQdayOfWeekMapping();
    return mapping.key(dayOfWeek);
}

QString OrganizerRecurrenceTransform::qdaysOfMonthToIcalByMonthDay(const QList<int> &daysOfMonth) const
{
    return QString("BYMONTHDAY=") + listOfNumbers(daysOfMonth);
}

QString OrganizerRecurrenceTransform::qdaysOfYearToIcalByYearDay(const QList<int> &daysOfYear) const
{
    return QString("BYYEARDAY=") + listOfNumbers(daysOfYear);
}

QString OrganizerRecurrenceTransform::qmonthsToIcalByMonth(const QList<QOrganizerItemRecurrenceRule::Month> &months) const
{
    QList<int> monthList;
    foreach (QOrganizerItemRecurrenceRule::Month month, months)
        monthList << static_cast<int>(month);
    return QString("BYMONTH=") + listOfNumbers(monthList);
}

QString OrganizerRecurrenceTransform::qweeksOfYearToIcalByWeekNo(const QList<int>& weeksOfYear) const
{
    return QString("BYWEEKNO=") + listOfNumbers(weeksOfYear);
}

QString OrganizerRecurrenceTransform::qpositionsToIcalBySetPos(const QList<int>& positions) const
{
    return QString("BYSETPOS=") + listOfNumbers(positions);
}

QString OrganizerRecurrenceTransform::listOfNumbers(const QList<int> &list) const
{
    // The calendar backend wants the lists to be sorted
    QList<int> sortedList = list;
    qSort(sortedList.begin(), sortedList.end());

    // Return a list of numbers, separated by comma
    QStringList slist;
    foreach (int number, sortedList)
        slist << QString::number( number );
    return slist.join(",");
}

int OrganizerRecurrenceTransform::qfrequencyToRtype(QOrganizerItemRecurrenceRule::Frequency frequency) const
{
    // The recursion types the native calendar application
    // uses are found by experiment:
    const int NATIVE_CAL_APP_DAILY = 1;
    const int NATIVE_CAL_APP_WEEKLY = 3;
    const int NATIVE_CAL_APP_MONTHLY = 5;
    const int NATIVE_CAL_APP_YEARLY = 6;

    switch( frequency )
    {
    case QOrganizerItemRecurrenceRule::Daily: return NATIVE_CAL_APP_DAILY;
    case QOrganizerItemRecurrenceRule::Weekly: return NATIVE_CAL_APP_WEEKLY;
    case QOrganizerItemRecurrenceRule::Monthly: return NATIVE_CAL_APP_MONTHLY;
    case QOrganizerItemRecurrenceRule::Yearly: return NATIVE_CAL_APP_YEARLY;
    default: return NATIVE_CAL_APP_YEARLY;
    }
}

QString OrganizerRecurrenceTransform::qrecurrenceDateToIcalRecurrenceDate(const QDate &date) const
{
    return QString("VALUE=DATE:") + date.toString("yyyyMMdd");
}

void OrganizerRecurrenceTransform::transformToQrecurrence(CRecurrence *crecurrence)
{
    m_lRecurrenceRules.clear();
    m_lExceptionRules.clear();
    m_lRecurrenceDates.clear();
    m_lExceptionDates.clear();

    if (!crecurrence)
        return;

    // Recurrence and exception rules
    std::vector< CRecurrenceRule* > rules = crecurrence->getRecurrenceRule();
    std::vector< CRecurrenceRule* >::const_iterator i;
    for (i = rules.begin(); i != rules.end(); ++i)
    {
        CRecurrenceRule *rule = *i;
        if (rule)
        {
            QOrganizerItemRecurrenceRule qrule(icalRecurrenceRuleToQrecurrenceRule(rule));
            if (rule->getRuleType() == RECURRENCE_RULE)
                m_lRecurrenceRules << qrule;
            else if(rule->getRuleType() == EXCEPTION_RULE)
                m_lExceptionRules << qrule;
        }
    }

    // Recurrence dates
    std::vector< std::string > recurrenceDates = crecurrence->getRDays();
    std::vector< std::string >::const_iterator recurrenceDate;
    for (recurrenceDate = recurrenceDates.begin(); recurrenceDate != recurrenceDates.end(); ++recurrenceDate)
    {
        m_lRecurrenceDates << convertRDate(QString::fromStdString(*recurrenceDate));
    }

    // Exception dates
    std::vector< std::string > exceptionDates = crecurrence->getEDays();
    std::vector< std::string >::const_iterator exceptionDate;
    for (exceptionDate = exceptionDates.begin(); exceptionDate != exceptionDates.end(); ++exceptionDate)
    {
        m_lExceptionDates << convertRDate(QString::fromStdString(*exceptionDate));
    }
}

QList<QDate> OrganizerRecurrenceTransform::convertRDate(const QString &rdate) const
{
    QStringList splitted = rdate.split(":");
    QList<QDate> dates;

    if (splitted.first().contains("VALUE=DATE")) {
        QStringList values = splitted.last().split(",");
        for (int i = 0; i < values.count(); i++) {
            QDate date = convertString(values[i]);
            dates << date;
        }
    } else if (splitted.first().contains("VALUE=PERIOD")) {
        QStringList values = splitted.last().split(",");
        for (int i = 0; i < values.count(); i++) {
            QString value = values[i].split('/').first();
            if (!values.isEmpty()) {
                QDate date = convertString(value);
                dates << date;
            }
        }
    } else if (splitted.first().startsWith("RDATE:")) {
         QDate date = convertString(splitted.last());
         dates << date;
    }
    // TODO: What to do if a timezone has also been defined f.ex "RDATE;TZID=US-EASTERN:19970714T083000"?

    return dates;
}

QDate OrganizerRecurrenceTransform::convertString(const QString &rdate) const
{
    QString time = rdate;
    bool isUtc = false;
    if (time.endsWith('Z')) {
        time.chop(1);
        isUtc = true;
    }

    QDateTime dateTime;
    if (time.contains('T')) {
        time.remove('T');
        dateTime = QDateTime::fromString(time, "yyyyMMddHHmmss");
    } else {
        dateTime = QDateTime::fromString(time, "yyyyMMdd");
    }

    if (isUtc)
        dateTime = dateTime.toUTC();

    return dateTime.date();
}

QList<QOrganizerItemRecurrenceRule> OrganizerRecurrenceTransform::recurrenceRules() const
{
    return m_lRecurrenceRules;
}

QList<QOrganizerItemRecurrenceRule> OrganizerRecurrenceTransform::exceptionRules() const
{
    return m_lExceptionRules;
}

QList<QDate> OrganizerRecurrenceTransform::recurrenceDates() const
{
    return m_lRecurrenceDates;
}

QList<QDate> OrganizerRecurrenceTransform::exceptionDates() const
{
    return m_lExceptionDates;
}

QOrganizerItemRecurrenceRule OrganizerRecurrenceTransform::icalRecurrenceRuleToQrecurrenceRule(CRecurrenceRule *rule ) const
{
    QOrganizerItemRecurrenceRule retn;
    // Parse rrule
    if (!rule->rruleParser(rule->getRrule()))
        return retn; // parsing failed

    // Now it's enough to just set the fields
    retn.setFrequency(icalFrequencyToQfrequency(rule->getFrequency()));

    time_t until = rule->getUntil();
    if (until > 0) {
        QDateTime endDate(QDateTime::fromTime_t(until));
        retn.setEndDate(endDate.date().addDays(1)); // in Maemo5 the end date is inclusive
    }

    int count = rule->getCount();
    if (count > 0)
        retn.setCount(count);

    retn.setInterval(rule->getInterval());

    QList<QOrganizerItemRecurrenceRule::Month> qMonths;
    std::vector< short > months = rule->getMonth();
    std::vector< short >::const_iterator month;
    for (month = months.begin(); month != months.end(); ++month)
        qMonths << static_cast<QOrganizerItemRecurrenceRule::Month>(*month);
    retn.setMonths(qMonths);

    QList<Qt::DayOfWeek> qDaysOfWeek;
    std::vector< short > daysOfWeek = rule->getWeekDay();
    std::vector< short >::const_iterator dayOfWeek;
    for (dayOfWeek = daysOfWeek.begin(); dayOfWeek != daysOfWeek.end(); ++dayOfWeek)
        qDaysOfWeek << icalWeekdayToQdayOfWeek(*dayOfWeek);
    retn.setDaysOfWeek(qDaysOfWeek);

    QList<int> qDaysOfMonth;
    std::vector< short > daysOfMonth = rule->getMonthDay();
    std::vector< short >::const_iterator dayOfMonth;
    for (dayOfMonth = daysOfMonth.begin(); dayOfMonth != daysOfMonth.end(); ++dayOfMonth)
        qDaysOfMonth << static_cast<int>(*dayOfMonth);
    retn.setDaysOfMonth(qDaysOfMonth);

    QList<int> qDaysOfYear;
    std::vector< short > daysOfYear = rule->getYearDay();
    std::vector< short >::const_iterator dayOfYear;
    for (dayOfYear = daysOfYear.begin(); dayOfYear != daysOfYear.end(); ++dayOfYear)
        qDaysOfYear << static_cast<int>(*dayOfYear);
    retn.setDaysOfYear(qDaysOfYear);

    QList<int> qWeeksOfYear;
    std::vector< short > weekNumbers = rule->getWeekNumber();
    std::vector< short >::const_iterator weekNumber;
    for (weekNumber = weekNumbers.begin(); weekNumber != weekNumbers.end(); ++weekNumber)
        qWeeksOfYear << static_cast<int>(*weekNumber + 1); // in Maemo5 the week numbers start from zero
    retn.setWeeksOfYear(qWeeksOfYear);

    QList<int> qPositions;
    std::vector< short > positions = rule->getPos();
    std::vector< short >::const_iterator position;
    for (position = positions.begin(); position != positions.end(); ++position)
        qPositions << static_cast<int>(*position);
    retn.setPositions(qPositions);

    // Parse the start of week here as it does not seem to become
    // parsed correctly with the rruleParser
    QString qrule(QString::fromStdString(rule->getRrule()));
    const QString weekstartField("WKST=");
    int weekstartAt = qrule.indexOf(weekstartField);
    QString weekday;
    if (weekstartAt>=0)
        weekday = qrule.mid(weekstartAt + weekstartField.length(), 2);
    retn.setWeekStart(icalRecurrenceTypeWeekdayToQdayOfWeek(weekday));

    return retn;
}

QOrganizerItemRecurrenceRule::Frequency OrganizerRecurrenceTransform::icalFrequencyToQfrequency(FREQUENCY frequency) const
{
    switch( frequency )
    {
    case DAILY_RECURRENCE: return QOrganizerItemRecurrenceRule::Daily;
    case WEEKLY_RECURRENCE: return QOrganizerItemRecurrenceRule::Weekly;
    case MONTHLY_RECURRENCE: return QOrganizerItemRecurrenceRule::Monthly;
    case YEARLY_RECURRENCE: return QOrganizerItemRecurrenceRule::Yearly;
    default:
        // No corresponding frequencies defined for the rest of cases
        return QOrganizerItemRecurrenceRule::Daily;
    }
}

Qt::DayOfWeek OrganizerRecurrenceTransform::icalWeekdayToQdayOfWeek(short weekday, bool *status) const
{
    // The Fremantle calendar representation of weekdays is 1=Sunday, 2=Monday,..., 7=Saturday
    QList<Qt::DayOfWeek> week;
    week << Qt::Sunday << Qt::Monday << Qt::Tuesday << Qt::Wednesday << Qt::Thursday << Qt::Friday << Qt::Saturday;
    if (weekday >= 1 && weekday <= 7) {
        if (status)
            *status = true;
        return week[weekday-1];
    }
    else {
        if (status)
            *status = false;
        return Qt::Monday; // This should never happen and should be interpreted as an error
    }
}

Qt::DayOfWeek OrganizerRecurrenceTransform::icalRecurrenceTypeWeekdayToQdayOfWeek(const QString &weekday) const
{
    QMap<QString, Qt::DayOfWeek> mapping = icalRecurrenceWeekDayQdayOfWeekMapping();
    if (mapping.contains(weekday))
        return mapping.value(weekday);
    else
        return Qt::Monday; // Return Monday as a default start of week
}

QMap<QString, Qt::DayOfWeek> OrganizerRecurrenceTransform::icalRecurrenceWeekDayQdayOfWeekMapping() const
{
    QMap<QString,Qt::DayOfWeek> mapping;
    mapping["MO"] = Qt::Monday;
    mapping["TU"] = Qt::Tuesday;
    mapping["WE"] = Qt::Wednesday;
    mapping["TH"] = Qt::Thursday;
    mapping["FR"] = Qt::Friday;
    mapping["SA"] = Qt::Saturday;
    mapping["SU"] = Qt::Sunday;
    return mapping;
}

QString OrganizerRecurrenceTransform::createSingleDayRuleFor(const QDate& date) const
{
    QStringList retn;

    // set as yearly recurrence
    retn << qfrequencyToIcalFrequency(QOrganizerItemRecurrenceRule::Yearly);

    // set months to date's month
    QList<QOrganizerItemRecurrenceRule::Month> months;
    months << static_cast<QOrganizerItemRecurrenceRule::Month>(date.month());
    retn << qmonthsToIcalByMonth(months);

    // set days to date's day
    QList<int> daysOfMonth;
    daysOfMonth << date.day();
    retn << qdaysOfMonthToIcalByMonthDay(daysOfMonth);

    // set weekday to weekday of the target date
    QList<Qt::DayOfWeek> daysOfWeek;
    daysOfWeek << static_cast<Qt::DayOfWeek>(date.dayOfWeek());
    retn << qdaysOfWeekToIcalByDay(daysOfWeek);

    // set interval
    retn << qintervalToIcalInterval(1);

    // set end date
    retn << qendDateToIcalUntil(date);

    return retn.join(";");
}
