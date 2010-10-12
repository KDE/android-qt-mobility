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

#include "qorganizerevent.h"
#include "qtorganizer.h"

QTM_USE_NAMESPACE

/*!
  \class QOrganizerEvent
  \brief The QOrganizerEvent class provides an event in time which may reoccur
  \inmodule QtOrganizer
  \ingroup organizer-items

  A QOrganizerEvent is an item which occurs at a particular point in time
  and may be associated with a location or have other details.  It may have
  a set of recurrence rules or dates on which the event occurs associated
  with it, and also exceptions to those recurrences.
 */

/*! Sets the start date time of the event to \a startDateTime */
void QOrganizerEvent::setStartDateTime(const QDateTime& startDateTime)
{
    QOrganizerEventTime etr = detail<QOrganizerEventTime>();
    etr.setStartDateTime(startDateTime);
    saveDetail(&etr);
}

/*! Returns the date time at which the first instance of the event starts */
QDateTime QOrganizerEvent::startDateTime() const
{
    QOrganizerEventTime etr = detail<QOrganizerEventTime>();
    return etr.startDateTime();
}

/*! Sets the end date time of the event to \a endDateTime */
void QOrganizerEvent::setEndDateTime(const QDateTime& endDateTime)
{
    QOrganizerEventTime etr = detail<QOrganizerEventTime>();
    etr.setEndDateTime(endDateTime);
    saveDetail(&etr);
}

/*! Returns the date time at which the first instance of the event ends */
QDateTime QOrganizerEvent::endDateTime() const
{
    QOrganizerEventTime etr = detail<QOrganizerEventTime>();
    return etr.endDateTime();
}

/*! Sets whether the time component of the start datetime or end datetime are significant. */
void QOrganizerEvent::setTimeSpecified(bool isTimeSpecified)
{
    QOrganizerEventTime etr = detail<QOrganizerEventTime>();
    etr.setTimeSpecified(isTimeSpecified);
    saveDetail(&etr);
}

/*! Returns whether the time component of the start datetime or end datetime are significant. */
bool QOrganizerEvent::isTimeSpecified() const
{
    QOrganizerEventTime etr = detail<QOrganizerEventTime>();
    return etr.isTimeSpecified();
}

/*! Sets the list of dates \a rdates to be dates on which the event occurs */
void QOrganizerEvent::setRecurrenceDates(const QSet<QDate>& rdates)
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    rec.setRecurrenceDates(rdates);
    saveDetail(&rec);
}

/*! Sets a single date \a rdate to be the date on which the event occurs.
    \see setRecurrenceDates() */
void QOrganizerEvent::setRecurrenceDate(const QDate& rdate)
{
    setRecurrenceDates(QSet<QDate>() << rdate);
}

/*! Returns the list of dates which have been explicitly set as dates on which the event occurs */
QSet<QDate> QOrganizerEvent::recurrenceDates() const
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    return rec.recurrenceDates();
}

/*! Sets the list of recurrence rules \a rrules to be the rules which define when the event occurs, other than those dates
    specified explicitly via setRecurrenceDates(). */
void QOrganizerEvent::setRecurrenceRules(const QSet<QOrganizerRecurrenceRule>& rrules)
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    rec.setRecurrenceRules(rrules);
    saveDetail(&rec);
}

/*! Sets a single recurrence rule \a rrule to be the rule which define when the event occurs, other than those dates
    specified explicitly via setRecurrenceDates().
    \see setRecurrenceRules()*/
void QOrganizerEvent::setRecurrenceRule(const QOrganizerRecurrenceRule& rrule)
{
    setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << rrule);
}

/*! Returns the list of recurrence rules which define when the event occurs */
QSet<QOrganizerRecurrenceRule> QOrganizerEvent::recurrenceRules() const
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    return rec.recurrenceRules();
}

/*! Returns the recurrence rule which has been explicitly set for the event.
    Note: if there are more than one dates exist, the order of the dates is undefined, so any one could be returned.*/
QOrganizerRecurrenceRule QOrganizerEvent::recurrenceRule() const
{
    QSet<QOrganizerRecurrenceRule> rrules = recurrenceRules();
    if (!rrules.isEmpty())
        return *rrules.begin();
    return QOrganizerRecurrenceRule();
}

/*! Sets the given list of dates \a exdates to be dates on which the event explicitly does not occur,
    when the recurrence rules suggest that the event should occur on those dates. */
void QOrganizerEvent::setExceptionDates(const QSet<QDate>& exdates)
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    rec.setExceptionDates(exdates);
    saveDetail(&rec);
}

/*! Sets the given single date \a exdate to be the date on which the event explicitly does not occur,
    when the recurrence rules suggest that the event should occur on this dates.
    \see setExceptionDates() */
void QOrganizerEvent::setExceptionDate(const QDate& exdate)
{
    setExceptionDates(QSet<QDate>() << exdate);
}

/*! Returns the list of dates on which the event explicitly does not occur despite
    the recurrence rules for the event */
QSet<QDate> QOrganizerEvent::exceptionDates() const
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    return rec.exceptionDates();
}

/*! Sets the given list of recurrence rules \a exrules to be the rules which define when
    the event does not occur. */
void QOrganizerEvent::setExceptionRules(const QSet<QOrganizerRecurrenceRule>& exrules)
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    rec.setExceptionRules(exrules);
    saveDetail(&rec);
}

/*! Sets the given single recurrence rules\a xrule to be the rule which define when
    the event does not occur.
    \see setExceptionRules.*/
void QOrganizerEvent::setExceptionRule(const QOrganizerRecurrenceRule& xrule)
{
    setExceptionRules(QSet<QOrganizerRecurrenceRule>() << xrule);
}

/*! Returns the list of exception rules for the event */
QSet<QOrganizerRecurrenceRule> QOrganizerEvent::exceptionRules() const
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    return rec.exceptionRules();
}

/*! Returns the exception rule which has been explicitly set for the event.
    Note: if there are more than one dates exist, the order of the dates is undefined, so any one could be returned.*/
QOrganizerRecurrenceRule QOrganizerEvent::exceptionRule() const
{
    QSet<QOrganizerRecurrenceRule> xrules = exceptionRules();
    if (!xrules.isEmpty())
        return *xrules.begin();
    return QOrganizerRecurrenceRule();
}
/*! Sets the priority of this event to \a priority */
void QOrganizerEvent::setPriority(QOrganizerItemPriority::Priority priority)
{
    QOrganizerItemPriority pd = detail<QOrganizerItemPriority>();
    pd.setPriority(priority);
    saveDetail(&pd);
}

/*! Returns the priority of the event */
QOrganizerItemPriority::Priority QOrganizerEvent::priority() const
{
    QOrganizerItemPriority pd = detail<QOrganizerItemPriority>();
    return pd.priority();
}

/*! Returns the label of the location at which the event occurs, if known */
QString QOrganizerEvent::location() const
{
    QOrganizerItemLocation ld = detail<QOrganizerItemLocation>();
    return ld.label();
}

/*! Sets the label of the location at which the event occurs to \a location */
void QOrganizerEvent::setLocation(const QString& location)
{
    QOrganizerItemLocation ld = detail<QOrganizerItemLocation>();
    ld.setLabel(location);
    saveDetail(&ld);
}
