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

#include "contacteditor.h"

#include <QtGui>

ContactEditor::ContactEditor(QWidget *parent)
        :QWidget(parent)
{
    m_manager = 0;
    m_contactId = QContactLocalId(0);

#ifdef Q_OS_SYMBIAN    
    // In symbian "save" and "cancel" buttons are softkeys.
    m_saveBtn = new QAction("Save", this);
    m_saveBtn->setSoftKeyRole(QAction::PositiveSoftKey);
    addAction(m_saveBtn);
    connect(m_saveBtn, SIGNAL(triggered(bool)), this, SLOT(saveClicked()));
    m_cancelBtn = new QAction("Cancel", this);
    m_cancelBtn->setSoftKeyRole(QAction::NegativeSoftKey);
    addAction(m_cancelBtn);
    connect(m_cancelBtn, SIGNAL(triggered(bool)), this, SLOT(cancelClicked()));     
#else
    m_saveBtn = new QPushButton("Save", this);
    connect(m_saveBtn, SIGNAL(clicked()), this, SLOT(saveClicked()));
    m_cancelBtn = new QPushButton("Cancel", this);
    connect(m_cancelBtn, SIGNAL(clicked()), this, SLOT(cancelClicked()));
#endif

    m_nameEdit = new QLineEdit(this);
    m_phoneEdit = new QLineEdit(this);
    m_emailEdit = new QLineEdit(this);
    m_addrEdit = new QLineEdit(this);
    m_avatarBtn = new QPushButton("Add image", this);
    m_avatarBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(m_avatarBtn, SIGNAL(clicked()), this, SLOT(avatarClicked()));

    QFormLayout *detailsLayout = new QFormLayout;
    detailsLayout->addRow(new QLabel("Name", this));
    detailsLayout->addRow(m_nameEdit);
    detailsLayout->addRow(new QLabel("Phone", this));
    detailsLayout->addRow(m_phoneEdit);
    detailsLayout->addRow(new QLabel("Email", this));
    detailsLayout->addRow(m_emailEdit);
    detailsLayout->addRow(new QLabel("Address", this));
    detailsLayout->addRow(m_addrEdit);
    detailsLayout->addRow(new QLabel("Avatar", this));
    detailsLayout->addRow(m_avatarBtn);
    detailsLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    detailsLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    
    QScrollArea *detailsScrollArea = new QScrollArea(this);
    detailsScrollArea->setWidgetResizable(true);
    QWidget *detailsContainer = new QWidget(detailsScrollArea);
    detailsContainer->setLayout(detailsLayout);
    detailsScrollArea->setWidget(detailsContainer);

#ifndef Q_OS_SYMBIAN    
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(m_saveBtn);
    btnLayout->addWidget(m_cancelBtn);
#endif

    QVBoxLayout *editLayout = new QVBoxLayout;
    editLayout->addWidget(detailsScrollArea);
#ifndef Q_OS_SYMBIAN
    editLayout->addLayout(btnLayout);
#endif

    setLayout(editLayout);
}

ContactEditor::~ContactEditor()
{
}

void ContactEditor::setCurrentContact(QContactManager* manager, QContactLocalId currentId)
{
    m_manager = manager;
    m_contactId = currentId;
    m_newAvatarPath = QString();

    if (manager == 0 || currentId == 0) {
        // clear the UI and return.
        m_nameEdit->setText("");
        m_phoneEdit->setText("");
        m_emailEdit->setText("");
        m_addrEdit->setText("");
        m_avatarBtn->setText("Add image");
        m_avatarBtn->setIcon(QIcon());

        if (manager == 0)
            m_saveBtn->setEnabled(false);

        return;
    }

    // enable the UI.
    m_saveBtn->setEnabled(true);

    // otherwise, build from the contact details.
    QContact curr = manager->contact(m_contactId);
    QContactName nm = curr.detail(QContactName::DefinitionName);
    QContactPhoneNumber phn = curr.detail(QContactPhoneNumber::DefinitionName);
    QContactEmailAddress em = curr.detail(QContactEmailAddress::DefinitionName);
    QContactAddress adr = curr.detail(QContactAddress::DefinitionName);
    QContactAvatar av = curr.detail(QContactAvatar::DefinitionName);

    m_nameEdit->setText(manager->synthesizeDisplayLabel(curr));
    m_phoneEdit->setText(phn.value(QContactPhoneNumber::FieldNumber));
    m_emailEdit->setText(em.value(QContactEmailAddress::FieldEmailAddress));
    m_addrEdit->setText(adr.value(QContactAddress::FieldStreet)); // ugly hack.
    
    m_avatarBtn->setText(QString());
    m_avatarBtn->setIcon(QIcon());
    
    if (av.pixmap().isNull()) {
        if (av.avatar().isEmpty()) {
            m_avatarBtn->setText("Add image");
        } else {
            m_avatarBtn->setIcon(QIcon(QPixmap(av.avatar())));
        }
    } else {
        m_newAvatarPath = av.avatar();
        m_avatarBtn->setIcon(QIcon(av.pixmap()));
    }
}

QString ContactEditor::nameField()
{
    // return the field which the name data should be saved in.
    if (!m_manager)
        return QString();

    QMap<QString, QContactDetailDefinition> defs = m_manager->detailDefinitions(QContactType::TypeContact);
    QContactDetailDefinition nameDef = defs.value(QContactName::DefinitionName);
    if (nameDef.fields().keys().contains(QContactName::FieldCustomLabel)) {
        return QString(QLatin1String(QContactName::FieldCustomLabel));
    } else if (nameDef.fields().keys().contains(QContactName::FieldFirst)) {
        return QString(QLatin1String(QContactName::FieldFirst));
    } else {
        return QString();
    }
}

void ContactEditor::avatarClicked()
{
    // put up a file dialog, and update the new avatar path.
    QString fileName = QFileDialog::getOpenFileName(this,
       tr("Select Avatar Image"), ".", tr("Image Files (*.png *.jpg *.bmp)"));
    
    if (!fileName.isEmpty()) {
        m_newAvatarPath = fileName;
        m_avatarBtn->setText(QString());
        m_avatarBtn->setIcon(QIcon(m_newAvatarPath));
    }
}

void ContactEditor::saveClicked()
{
    if (!m_manager) {
        qWarning() << "No manager selected; cannot save.";
    } else {
        QContact curr;
        if (m_contactId != QContactLocalId(0))
            curr = m_manager->contact(m_contactId);
        QContactName nm = curr.detail(QContactName::DefinitionName);
        QContactPhoneNumber phn = curr.detail(QContactPhoneNumber::DefinitionName);
        QContactEmailAddress em = curr.detail(QContactEmailAddress::DefinitionName);
        QContactAddress adr = curr.detail(QContactAddress::DefinitionName);
        QContactAvatar av = curr.detail(QContactAvatar::DefinitionName);

        QString saveNameField = nameField();
        if (!saveNameField.isEmpty()) {
            // if the name has changed (ie, is different to the synthed label) then save it as a custom label.
            if (m_nameEdit->text() != m_manager->synthesizeDisplayLabel(curr)) {
                nm.setValue(nameField(), m_nameEdit->text());
            }
        }
        phn.setNumber(m_phoneEdit->text());
        em.setEmailAddress(m_emailEdit->text());
        adr.setStreet(m_addrEdit->text());
        av.setAvatar(m_newAvatarPath);

        QPixmap pix(m_newAvatarPath);
        bool null = pix.isNull();
        av.setPixmap(pix);

        curr.saveDetail(&nm);
        curr.saveDetail(&phn);
        curr.saveDetail(&em);
        curr.saveDetail(&adr);
        curr.saveDetail(&av);

        bool success = m_manager->saveContact(&curr);
        if (success)
            QMessageBox::information(this, "Success!", "Contact saved successfully!");
        else
            QMessageBox::information(this, "Failed!", "Failed to save contact!");
    }

    emit showListPage();
}

void ContactEditor::cancelClicked()
{
    emit showListPage();
}
