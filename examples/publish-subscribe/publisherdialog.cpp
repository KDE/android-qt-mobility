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

#include "publisherdialog.h"
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
#include "ui_publisherdialog_hor.h"
#else
#include "ui_publisherdialog.h"
#endif

#include <qvaluespacepublisher.h>

#ifdef QTM_EXAMPLES_SMALL_SCREEN
#include <QPushButton>
#endif

#include <QDebug>

PublisherDialog::PublisherDialog(QWidget *parent)
:   QDialog(parent), ui(new Ui::PublisherDialog), publisher(0)
{
    ui->setupUi(this);

#ifdef QTM_EXAMPLES_SMALL_SCREEN
    QPushButton *switchButton =
        ui->buttonBox->addButton(tr("Switch"), QDialogButtonBox::ActionRole);
    connect(switchButton, SIGNAL(clicked()), this, SIGNAL(switchRequested()));
#endif

    //! [1]
    connect(ui->connectButton, SIGNAL(clicked()), this, SLOT(createNewObject()));
    connect(ui->intValue, SIGNAL(valueChanged(int)), this, SLOT(intValueChanged(int)));
    connect(ui->unsetIntButton, SIGNAL(clicked()), this, SLOT(unsetIntValue()));
    connect(ui->setStringButton, SIGNAL(clicked()), this, SLOT(setStringValue()));
    connect(ui->setByteArrayButton, SIGNAL(clicked()), this, SLOT(setByteArrayValue()));
    //! [1]

    //! [3]
    createNewObject();
    //! [3]
}

PublisherDialog::~PublisherDialog()
{
    delete ui;
    delete publisher;
}

void PublisherDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

//! [0]
void PublisherDialog::intValueChanged(int value)
{
    publisher->setValue("intValue", value);
}

void PublisherDialog::unsetIntValue()
{
    publisher->resetValue("intValue");
}

void PublisherDialog::setStringValue()
{
    publisher->setValue("stringValue", ui->stringValue->text());
}

void PublisherDialog::setByteArrayValue()
{
    publisher->setValue("byteArrayValue", ui->byteArrayValue->text().toAscii());
}
//! [0]

//! [2]
void PublisherDialog::createNewObject()
{
    if (publisher)
        delete publisher;

    publisher = new QValueSpacePublisher(QValueSpace::WritableLayer, ui->basePath->text());
    if (publisher->isConnected()) {
        ui->setters->setEnabled(true);
        intValueChanged(ui->intValue->value());
        setStringValue();
        setByteArrayValue();
    } else {
        ui->setters->setEnabled(false);
    }
}
//! [2]
