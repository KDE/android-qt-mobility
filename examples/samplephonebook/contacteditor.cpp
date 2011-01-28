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

#include "contacteditor.h"

#include <QtGui>
#include <QDesktopServices>

const int MAX_THUMBNAIL_DISPLAY_SIZE = 120;

ContactEditor::ContactEditor(QWidget *parent)
        :QWidget(parent)
{
    m_manager = 0;
    m_contactId = QContactLocalId(0);
    m_observer = 0;

    m_nameEdit = new QLineEdit(this);
    m_phoneEdit = new QLineEdit(this);
    m_emailEdit = new QLineEdit(this);
    m_addrEdit = new QLineEdit(this);
    m_thumbnailBtn = new QPushButton(tr("Set picture"), this);
    m_clearThumbnailBtn = new QPushButton(tr("Clear"), this);
    m_thumbnailView = new QLabel(this);
    connect(m_thumbnailBtn, SIGNAL(clicked()), this, SLOT(thumbnailClicked()));
    connect(m_clearThumbnailBtn, SIGNAL(clicked()), this, SLOT(clearThumbnailClicked()));

    QFormLayout *detailsLayout = new QFormLayout;
    QLabel *nameLabel = new QLabel(tr("Name"), this);
    QLabel *phoneLabel = new QLabel(tr("Phone"), this);
    QLabel *emailLabel = new QLabel(tr("Email"), this);
    QLabel *addressLabel = new QLabel(tr("Address"), this);
    QLabel *thumbnailLabel = new QLabel(tr("Picture"), this);
    QHBoxLayout *thumbnailBtnLayout = new QHBoxLayout;
    thumbnailBtnLayout->addWidget(m_thumbnailBtn);
    thumbnailBtnLayout->addWidget(m_clearThumbnailBtn);
    if (QApplication::desktop()->availableGeometry().width() < 360) {
        // Narrow screen: put label on separate line to textbox
        detailsLayout->addRow(nameLabel);
        detailsLayout->addRow(m_nameEdit);
        detailsLayout->addRow(phoneLabel);
        detailsLayout->addRow(m_phoneEdit);
        detailsLayout->addRow(emailLabel);
        detailsLayout->addRow(m_emailEdit);
        detailsLayout->addRow(addressLabel);
        detailsLayout->addRow(m_addrEdit);
        detailsLayout->addRow(thumbnailLabel);
        detailsLayout->addRow(thumbnailBtnLayout);
        detailsLayout->addRow(m_thumbnailView);
    } else {
        // Wide screen: put label on same line as textbox
        detailsLayout->addRow(nameLabel, m_nameEdit);
        detailsLayout->addRow(phoneLabel, m_phoneEdit);
        detailsLayout->addRow(emailLabel, m_emailEdit);
        detailsLayout->addRow(addressLabel, m_addrEdit);
        detailsLayout->addRow(thumbnailLabel, thumbnailBtnLayout);
        detailsLayout->addRow("", m_thumbnailView);
    }
    detailsLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    detailsLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    QScrollArea *detailsScrollArea = new QScrollArea(this);
    detailsScrollArea->setWidgetResizable(true);
    QWidget *detailsContainer = new QWidget(detailsScrollArea);
    detailsContainer->setLayout(detailsLayout);
    detailsScrollArea->setWidget(detailsContainer);

    QVBoxLayout *editLayout = new QVBoxLayout;
    editLayout->addWidget(detailsScrollArea);

#ifdef Q_OS_SYMBIAN
    // In symbian "save" and "cancel" buttons are softkeys.
    m_saveBtn = new QAction(tr("Save"), this);
    m_saveBtn->setSoftKeyRole(QAction::PositiveSoftKey);
    addAction(m_saveBtn);
    connect(m_saveBtn, SIGNAL(triggered(bool)), this, SLOT(saveClicked()));
    m_cancelBtn = new QAction(tr("Cancel"), this);
    m_cancelBtn->setSoftKeyRole(QAction::NegativeSoftKey);
    addAction(m_cancelBtn);
    connect(m_cancelBtn, SIGNAL(triggered(bool)), this, SLOT(cancelClicked()));
#else
    m_saveBtn = new QPushButton(tr("&Save"), this);
    m_saveBtn->setDefault(true);
    connect(m_saveBtn, SIGNAL(clicked()), this, SLOT(saveClicked()));
    m_cancelBtn = new QPushButton(tr("&Cancel"), this);
    connect(m_cancelBtn, SIGNAL(clicked()), this, SLOT(cancelClicked()));
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(m_saveBtn);
    btnLayout->addWidget(m_cancelBtn);
    editLayout->addLayout(btnLayout);
#endif

    setLayout(editLayout);
}

ContactEditor::~ContactEditor()
{
    delete m_observer;
}

void ContactEditor::setCurrentContact(QContactManager* manager, QContactLocalId currentId)
{
    m_manager = manager;
    m_contactId = currentId;
    m_newThumbnailPath = QString();

    // Clear UI
    m_nameEdit->clear();
    m_phoneEdit->clear();
    m_emailEdit->clear();
    m_addrEdit->clear();

    if (manager == 0) {
        m_saveBtn->setEnabled(false);
        return;
    }

    // enable the UI.
    m_saveBtn->setEnabled(true);

    // otherwise, build from the contact details.
    QContact contact;
    if (m_contactId != QContactLocalId(0))
        contact = manager->contact(m_contactId);

    // Set an observer on the contact to watch it for changes from another process
    delete m_observer;
    m_observer = new QContactObserver(manager, m_contactId);
    connect(m_observer, SIGNAL(contactChanged()), this, SLOT(contactChanged()));

    updateUi(contact);
}

void ContactEditor::updateUi(const QContact& contact)
{
    // Disable fields & buttons according to what the backend supports
    QMap<QString, QContactDetailDefinition> defs = m_manager->detailDefinitions(QContactType::TypeContact);

    // name
    if (m_contactId != QContactLocalId(0))
        m_nameEdit->setText(contact.displayLabel());

    // phonenumber
    QContactPhoneNumber phn = contact.detail(QContactPhoneNumber::DefinitionName);
    m_phoneEdit->setText(phn.value(QContactPhoneNumber::FieldNumber));

    // email
    if (defs.contains(QContactEmailAddress::DefinitionName)) {
        QContactEmailAddress em = contact.detail(QContactEmailAddress::DefinitionName);
        m_emailEdit->setText(em.value(QContactEmailAddress::FieldEmailAddress));
        m_emailEdit->setReadOnly(false);
    } else {
        m_emailEdit->setText("<not supported>");
        m_emailEdit->setReadOnly(true);
    }

    // address
    if (defs.contains(QContactAddress::DefinitionName)) {
        QContactAddress adr = contact.detail(QContactAddress::DefinitionName);
        m_addrEdit->setText(adr.value(QContactAddress::FieldStreet)); // ugly hack.
        m_addrEdit->setReadOnly(false);
    } else {
        m_addrEdit->setText("<not supported>");
        m_addrEdit->setReadOnly(true);
    }

    // thumbnail viewer
    if (defs.contains(QContactThumbnail::DefinitionName)) {
        m_thumbnailBtn->setEnabled(true);
        QContactThumbnail thumb = contact.detail(QContactThumbnail::DefinitionName);
        m_thumbnailView->clear();
        m_thumbnail = thumb.thumbnail();
        if (m_thumbnail.isNull()) {
            m_thumbnailView->clear();
            m_clearThumbnailBtn->setDisabled(true);
        } else {
            setThumbnailPixmap(QPixmap::fromImage(m_thumbnail));
        }
    } else {
        m_thumbnailBtn->setDisabled(true);
        m_clearThumbnailBtn->setDisabled(true);
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
    } else if (nameDef.fields().keys().contains(QContactName::FieldFirstName)) {
        return QString(QLatin1String(QContactName::FieldFirstName));
    } else {
        return QString();
    }
}

void ContactEditor::setThumbnailPixmap(const QPixmap &pixmap)
{
    if (pixmap.isNull())
        return;
    QPixmap scaled = pixmap.scaled(QSize(MAX_THUMBNAIL_DISPLAY_SIZE, MAX_THUMBNAIL_DISPLAY_SIZE),
                                   Qt::KeepAspectRatio,
                                   Qt::SmoothTransformation);
    m_thumbnailView->setPixmap(scaled);
    m_thumbnailView->setMaximumSize(scaled.size());
    m_clearThumbnailBtn->setEnabled(true);
}

void ContactEditor::clearThumbnailClicked()
{
    m_thumbnailView->clear();
    m_thumbnail = QImage();
    m_newThumbnailPath.clear();
    m_clearThumbnailBtn->setDisabled(true);
}

void ContactEditor::thumbnailClicked()
{
    // put up a file dialog, and update the new thumbnail path.
    QString docPath = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
    if (docPath.isEmpty())
        docPath = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
    if (docPath.isEmpty())
        docPath = ".";
    QString fileName = QFileDialog::getOpenFileName(this,
       tr("Select Contact Picture"), docPath, tr("Image Files (*.png *.jpg *.bmp)"));

    if (!fileName.isEmpty()) {
        m_newThumbnailPath = fileName;
        m_thumbnail = QImage(m_newThumbnailPath);
        setThumbnailPixmap(QPixmap::fromImage(m_thumbnail));
    }
}

void ContactEditor::saveClicked()
{
    // Stop observing the contact
    if (m_observer) {
        m_observer->disconnect();
    }

    if (!m_manager) {
        qWarning() << "No manager selected; cannot save.";
    } else {
        QContact curr;
        if (m_contactId != QContactLocalId(0))
            curr = m_manager->contact(m_contactId);

        if (m_nameEdit->text().isEmpty()) {
            QMessageBox::information(this, "Failed!", "You must give a name for the contact!");
            return;
        }

        if (m_nameEdit->text() != m_manager->synthesizedContactDisplayLabel(curr)) {
            // if the name has changed (ie, is different to the synthed label) then save it as a custom label.
            QString saveNameField = nameField();
            if (!saveNameField.isEmpty()) {
                QContactName nm = curr.detail(QContactName::DefinitionName);
                nm.setValue(saveNameField, m_nameEdit->text());
                curr.saveDetail(&nm);
            }
        }

        QContactPhoneNumber phn = curr.detail(QContactPhoneNumber::DefinitionName);
        phn.setNumber(m_phoneEdit->text());
        curr.saveDetail(&phn);

        if (!m_emailEdit->isReadOnly()) {
            QContactEmailAddress em = curr.detail(QContactEmailAddress::DefinitionName);
            em.setEmailAddress(m_emailEdit->text());
            curr.saveDetail(&em);
        }

        if (!m_addrEdit->isReadOnly()) {
            QContactAddress adr = curr.detail(QContactAddress::DefinitionName);
            adr.setStreet(m_addrEdit->text());
            curr.saveDetail(&adr);
        }

        if (m_thumbnailBtn->isEnabled()) {
            QContactThumbnail thumb = curr.detail(QContactThumbnail::DefinitionName);
            QImage img(m_thumbnail);
            thumb.setThumbnail(img);
            curr.saveDetail(&thumb);
        }

        curr = m_manager->compatibleContact(curr);
        bool success = m_manager->saveContact(&curr);
        if (!success)
            QMessageBox::information(this, "Failed!", QString("Failed to save contact!\n(error code %1)").arg(m_manager->error()));
    }

    emit showListPage();
}

void ContactEditor::cancelClicked()
{
    emit showListPage();
}

void ContactEditor::contactChanged()
{
    QMessageBox::StandardButton button =
        QMessageBox::question(this, "Contact updated",
                "The contact has been changed by some other process.\n\nDo you want to reload it?",
                QMessageBox::Yes | QMessageBox::No);
    if (button == QMessageBox::Yes) {
        updateUi(m_manager->contact(m_contactId));
    }
}
