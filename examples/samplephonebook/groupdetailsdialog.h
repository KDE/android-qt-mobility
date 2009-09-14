/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef GROUPDETAILSDIALOG_H
#define GROUPDETAILSDIALOG_H

#include <QDialog>
#include "qcontact.h"

class QPushButton;
class QListWidget;
class QListWidgetItem;
class QLineEdit;
class QContactManager;

class GroupDetailsDialog : public QDialog
{
    Q_OBJECT

public:
    GroupDetailsDialog(QContactManager *contactManager, const QContact &contact);
    QList<QUniqueId> groups();

private:
    void repopulateGroupList();

public slots:
    void show();
    void groupItemChanged(QListWidgetItem * item);

private slots:
    void okButtonClicked();
    void cancelButtonClicked();

private:
    QListWidget *listWidget;
    QLineEdit *contactNameEdit;
    QPushButton *cancelButton;
    QPushButton *okButton;
    QContactManager *cm;
    QContact localContact;
};

#endif // GROUPDETAILSDIALOG_H

