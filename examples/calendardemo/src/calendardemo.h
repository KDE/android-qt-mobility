/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef CALENDARDEMO_H
#define CALENDARDEMO_H

#include <QtGui/QMainWindow>
#include <QDate>
#include <qmobilityglobal.h>
#include <qorganizeritemsaverequest.h>
#include <qorganizeritemremoverequest.h>
class QProgressDialog;

QTM_BEGIN_NAMESPACE
class QOrganizerManager;
class QOrganizerItem;
QTM_END_NAMESPACE
QTM_USE_NAMESPACE

#define ORGANIZER_ITEM_ROLE Qt::UserRole+1
#define ORGANIZER_CALENDAR_ROLE Qt::UserRole+2

class QStackedWidget;
class MonthPage;
class DayPage;
class EventEditPage;
class TodoEditPage;
class JournalEditPage;
class EventOccurrenceEditPage;
class AddCalendarPage;
class EditCalendarsPage;

class CalendarDemo : public QMainWindow
{
    Q_OBJECT

public:
    CalendarDemo(QWidget *parent = 0);
    ~CalendarDemo();

public Q_SLOTS:
    void activateMonthPage();
    void activateDayPage();
    void activateEditPage(const QOrganizerItem &item);
    void activatePreviousPage();
    void addNewEvent();
    void addNewTodo();
    void addNewJournal();
    void changeManager(QOrganizerManager *manager);
    void updateSelectedDay(const QDate& date);

private Q_SLOTS:
    void switchView();
    void editItem();
    void removeItem();
    void addEvents();
    void importItems();
    void exportItems();
    void deleteAllEntries();
    void addCalendar();
    void editCalendar();
    void editExistingCalendar(QOrganizerManager *manager, QOrganizerCollection* calendar);
    void saveReqStateChanged(QOrganizerAbstractRequest::State);
    void removeReqStateChanged(QOrganizerAbstractRequest::State);

private:
    void buildMenu();

    QDate m_currentDate;
    QOrganizerManager *m_manager;
    QStackedWidget *m_stackedWidget;
    MonthPage *m_monthPage;
    DayPage *m_dayPage;
    EventEditPage *m_eventEditPage;
    TodoEditPage *m_todoEditPage;
    JournalEditPage *m_journalEditPage;
    EventOccurrenceEditPage *m_eventOccurrenceEditPage;
    AddCalendarPage *m_addCalendarPage;
    EditCalendarsPage *m_editCalendarsPage;

    int m_previousPage;
    QOrganizerItem m_previousItem;

    QAction *m_switchViewAction;

    QOrganizerItemSaveRequest m_saveReq;
    QOrganizerItemRemoveRequest m_remReq;
    QProgressDialog *m_progressDlg;
};

#endif // CALENDARDEMO_H
