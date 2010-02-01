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

#include "phonebook.h"
#include "contactlistpage.h"
#include "contacteditor.h"
#include "filterpage.h"

#include <QtGui>

PhoneBook::PhoneBook(QWidget *parent)
        : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget(this);
   
    m_editorPage = new ContactEditor(centralWidget);
    connect(m_editorPage, SIGNAL(showListPage()), this, SLOT(activateList()));

    m_filterPage = new FilterPage(centralWidget);
    connect(m_filterPage, SIGNAL(showListPage(QContactFilter)), this, SLOT(activateList(QContactFilter)));

    m_listPage = new ContactListPage(centralWidget);
    connect(m_listPage, SIGNAL(showEditorPage(QContactLocalId)), this, SLOT(activateEditor(QContactLocalId)));
    connect(m_listPage, SIGNAL(showFilterPage(QContactFilter)), this, SLOT(activateFind()));
    connect(m_listPage, SIGNAL(managerChanged(QContactManager*)), this, SLOT(managerChanged(QContactManager*)));

    m_stackedWidget = new QStackedWidget(centralWidget);
    m_stackedWidget->addWidget(m_listPage);
    m_stackedWidget->addWidget(m_editorPage);
    m_stackedWidget->addWidget(m_filterPage);
    m_stackedWidget->setCurrentIndex(0);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addWidget(m_stackedWidget);
    centralWidget->setLayout(centralLayout);
    
    setCentralWidget(centralWidget);
}

PhoneBook::~PhoneBook()
{
}

void PhoneBook::activateEditor(QContactLocalId contactId)
{
    m_editorPage->setCurrentContact(m_manager, contactId);
    m_stackedWidget->setCurrentIndex(1); // list = 0, editor = 1, find = 2.
}

void PhoneBook::activateList(const QContactFilter& filter)
{  
    m_currentFilter = filter;
    activateList(); // call base now.
}

void PhoneBook::activateList()
{
    m_listPage->rebuildList(m_currentFilter);
    m_stackedWidget->setCurrentIndex(0); // list = 0, editor = 1, find = 2.
}

void PhoneBook::activateFind()
{
    m_stackedWidget->setCurrentIndex(2); // list = 0, editor = 1, find = 2.
}

void PhoneBook::managerChanged(QContactManager *manager)
{
    m_manager = manager;
    m_editorPage->setCurrentContact(m_manager, 0); // must reset the manager of the editor.
}
