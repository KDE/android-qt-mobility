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

#include "monitordialog.h"
#include "ui_dialog.h"
#include <QDebug>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    m_rxDBusMsg()
{
    ui->setupUi(this);
    this->setWindowTitle("Monitoring Call Data");
    ui->lstRxMsg->setModel(&m_rxDBusMsg);
    telephonyCallList = new QTelephonyCallList(this);
    connect(telephonyCallList
            , SIGNAL(activeCallAdded(QTelephonyCallInfo))
            , SLOT(activeCallAdded(QTelephonyCallInfo)));
    connect(telephonyCallList
            , SIGNAL(activeCallStatusChanged(QTelephonyCallInfo))
            , SLOT(activeCallStatusChanged(QTelephonyCallInfo)));

    QString newentry = "waiting for notifications";
    QStringList vl = m_rxDBusMsg.stringList();
    vl.append(newentry);
    m_rxDBusMsg.setStringList(vl);

}

Dialog::~Dialog()
{
    if(telephonyCallList)
        delete telephonyCallList;
    delete ui;
}

void Dialog::changeEvent(QEvent *e)
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

void Dialog::activeCallAdded(const QTelephonyCallInfo& call)
{
    QString newentry = "call Added: " + call.remotePartyIdentifier();
    QStringList vl = m_rxDBusMsg.stringList();
    vl.append(newentry);
    m_rxDBusMsg.setStringList(vl);
}

void Dialog::activeCallStatusChanged(const QTelephonyCallInfo& call)
{
    QString newentry = "- status ";
    if(call.status() == QTelephony::Dialing)
        newentry += "Dialing";
    else if(call.status() == QTelephony::Alerting)
        newentry += "Alerting";
    else if(call.status() == QTelephony::Connected)
        newentry += "Connected";
    else if(call.status() == QTelephony::Disconnecting)
        newentry += "Disconnecting";

    QStringList vl = m_rxDBusMsg.stringList();
    vl.append(newentry);
    m_rxDBusMsg.setStringList(vl);
}
