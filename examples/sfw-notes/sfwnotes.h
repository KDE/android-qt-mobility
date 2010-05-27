/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#ifndef SFWNOTES_H
#define SFWNOTES_H

#include <QWidget>
#include <QObject>
#include <QDateTime>
#include <qmobilityglobal.h>

#include "ui_sfwnotes.h"

QTM_BEGIN_NAMESPACE
class QServiceManager;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class ToDoTool : public QWidget, public Ui_ToDoTool
{
    Q_OBJECT
public:
    ToDoTool(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~ToDoTool();

private slots:
    void on_nextButton_clicked();
    void on_prevButton_clicked();
    void on_addButton_clicked();
    void on_deleteButton_clicked();
    void on_searchButton_clicked();
    void soundAlarm(const QDateTime &alarm);

private:
    void init();
    void refreshList();
    void refreshNotes();
    void registerExampleServices();
    void unregisterExampleServices();

    QServiceManager *serviceManager;
    QObject *notesManager;

    QList<QObject*> ret;

    QString searchWord;
    int currentNote;
    int totalNotes;

};

#endif

