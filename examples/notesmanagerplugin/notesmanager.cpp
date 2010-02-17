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
** You may use this fi
#include <QList>le in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
#include <QSqlRecord>
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
*#include <QStringList>
 *
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore>
#include <QDebug>

#include "notesmanager.h"

NotesManager::NotesManager(QObject *parent)
    : QObject(parent)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("todoDB");
    db.open();

    QSqlQuery create;
    create.exec("CREATE TABLE todolist(id INTEGER PRIMARY KEY, notes VARCHAR(255), date VARCHAR(255))");

    nextAlarm();

    QTimer *timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(checkAlarm()));
    timer->start(1000);
}

void NotesManager::nextAlarm()
{
    QSqlQuery alarmQuery("SELECT * FROM todolist WHERE date > DATETIME('now', 'localtime') ORDER BY date");
    if (alarmQuery.next()) { 
        setAlarm(QDateTime::fromString(alarmQuery.value(2).toString(), "yyyy-MM-dd HH:mm:ss"));
        setMessage(alarmQuery.value(1).toString());
    }
}

void NotesManager::checkAlarm()
{
    QString currStr = QDateTime::currentDateTime().toString(Qt::ISODate);
    QDateTime curr = QDateTime::fromString(currStr, Qt::ISODate);

    //qDebug() << "CHECKING..." << getAlarm() << " against now " << curr;
    if (getAlarm() == curr)
        emit soundAlarm(getAlarm());

    nextAlarm();
}

QDateTime NotesManager::getAlarm() const
{
    return m_alarm;
}

void NotesManager::setAlarm(const QDateTime& alarm)
{
    m_alarm = alarm;
}

QString NotesManager::getMessage() const
{
    return m_message;
}

void NotesManager::setMessage(const QString& message)
{
    m_message = message;
}

void NotesManager::addNote(const QString& note, const QDateTime& alarm)
{
    QString alert = alarm.toString("yyyy-MM-dd HH:mm:ss");
    QSqlQuery query("INSERT INTO todolist(notes, date) VALUES ('" + note + "', '" + alert + "')");
}

void NotesManager::removeNote(int id)
{
    QSqlQuery query("DELETE FROM todolist WHERE id='" + QString::number(id) + "'");
}

QList<Note> NotesManager::getNotes(const QString& search) const
{
    QList<Note> list;

    QString queryString = "SELECT * FROM todolist";
    if (search != "") queryString += " WHERE notes LIKE '%" + search + "%'"; 
    queryString += " ORDER BY date";

    QSqlQuery query(queryString);
    while (query.next()) {
        Note entry;
        entry.index = query.value(0).toInt();
        entry.message = query.value(1).toString();
        entry.alert = QDateTime::fromString(query.value(2).toString(), "yyyy-MM-dd HH:mm:ss");

        list << entry;
    }

    return list;
}

