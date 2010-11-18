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
#ifndef EVENTEDITPAGE_H_
#define EVENTEDITPAGE_H_

#include <QWidget>
#include <QDate>
#include <qmobilityglobal.h>
#include <qorganizeritemid.h>
#include <qorganizerevent.h>

QTM_BEGIN_NAMESPACE
class QOrganizerManager;
class QOrganizerItemSaveRequest;
class QOrganizerEvent;
class QOrganizerItem;
QTM_END_NAMESPACE
QTM_USE_NAMESPACE

class QComboBox;
class QLineEdit;
class QDateTimeEdit;
class QVBoxLayout;
class QString;
class QSpinBox;
class QDateEdit;

class EventEditPage : public QWidget
{
    Q_OBJECT

public:
    EventEditPage(QWidget *parent = 0);
    ~EventEditPage();

public Q_SLOTS:
    void cancelClicked();
    void saveClicked();
    void eventChanged(QOrganizerManager *manager, const QOrganizerEvent &event);
    void frequencyChanged(const QString&);
    void countChanged(int);
    void untilChanged(QDate);
    void endConditionChanged(const QString&);
    void alarmIndexChanged(const QString);
    void setCountField();
    void setRepeatUntilField();

Q_SIGNALS:
    void showDayPage();

protected: // from QWidget
    void showEvent(QShowEvent *event);

private:
    QOrganizerManager *m_manager;
    QOrganizerEvent m_organizerEvent;
    QList<QOrganizerItem> m_listOfEvents;
    QOrganizerItemSaveRequest *m_saveItemRequest;
    QVBoxLayout *m_repeatControls;
    QAction *m_saveOrNextSoftKey;
    QComboBox *m_alarmComboBox;
    QComboBox *m_typeComboBox;
    QLineEdit *m_subjectEdit;
    QDateTimeEdit *m_startTimeEdit;
    QDateTimeEdit *m_endTimeEdit;
    QComboBox *m_endConditionComboBox;
    QSpinBox *m_countSpinBox;
    QDateEdit *m_repeatUntilDate;
    QComboBox *m_calendarComboBox;
    QList<QOrganizerCollection> m_collections;
};

#endif // EVENTEDITPAGE_H_
