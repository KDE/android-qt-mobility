/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef SFWNOTES_H
#define SFWNOTES_H

#include <QWidget>
#include <QObject>
#include <QDateTime>
#include <qmobilityglobal.h>

#if defined (Q_OS_SYMBIAN) || defined(Q_OS_WINCE) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
#include "ui_sfwnotes_mobile.h"
#else
#include "ui_sfwnotes.h"
#endif

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
    void refreshList(bool view = true);
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

