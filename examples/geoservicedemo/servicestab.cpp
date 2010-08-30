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

#include "servicestab.h"
#include "placepresenter.h"

#include <QTreeWidget>
#include <QLineEdit>
#include <QString>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QTimer>
#include <qgeoserviceprovider.h>

ServicesTab::ServicesTab(QWidget *parent) :
        QWidget(parent)
{
    m_requestBtn = new QPushButton(tr("Set Service Provider"));
    m_requestBtn->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    QObject::connect(m_requestBtn, SIGNAL(clicked(bool)), this, SLOT(on_btnRequest_clicked()));

    m_resultTree = new QTreeWidget();
    QStringList labels;
    labels << tr("Services") << tr("Available");
    m_resultTree->setHeaderLabels(labels);
    m_resultTree->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_resultTree->setColumnWidth(0, 275);

    QHBoxLayout *firstrow = new QHBoxLayout;
    firstrow->setSpacing(0);
    firstrow->setContentsMargins(0, 0, 0, 0);
    firstrow->addWidget(m_requestBtn);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(2);
    mainLayout->setContentsMargins(2, 1, 2, 1);
    mainLayout->addLayout(firstrow);
    mainLayout->addWidget(m_resultTree);
    setLayout(mainLayout);

    m_requestBtn->setDisabled(true);
}

void ServicesTab::initialize()
{
    QTreeWidgetItem* top = new QTreeWidgetItem(m_resultTree);
    top->setText(0, tr("Loading Service Providers..."));

    listServiceProviders();

    if (m_resultTree->topLevelItemCount() == 0) {
        m_requestBtn->setDisabled(true);
        QTreeWidgetItem* top = new QTreeWidgetItem(m_resultTree);
        top->setText(0, tr("No providers available"));
    } else {
        m_requestBtn->setDisabled(false);
        on_btnRequest_clicked();
    }
}

ServicesTab::~ServicesTab()
{
}

void ServicesTab::on_btnRequest_clicked()
{
    if (m_resultTree->currentItem() == NULL) {
        if (m_resultTree->topLevelItemCount() > 0)
            m_resultTree->setCurrentItem(m_resultTree->topLevelItem(0));
        else
            return;
    }

    QTreeWidgetItem *serviceItem = m_resultTree->currentItem();
    while (serviceItem->parent() != NULL)
        serviceItem = m_resultTree->currentItem()->parent();

    QString serviceId = serviceItem->text(0);
    emit serviceProviderChanged(serviceId);

    for (int i = 0; i < m_resultTree->topLevelItemCount(); ++i) {
        QTreeWidgetItem* top = m_resultTree->topLevelItem(i);
        if (top->text(0) == serviceId)
            top->setText(1, tr("CurrentProvider"));
        else
            top->setText(1, tr(""));
    }
}

void ServicesTab::listServiceProviders()
{
    m_resultTree->clear();
    QList<QString> providers = QGeoServiceProvider::availableServiceProviders();

    QString providerId;
    foreach(providerId, providers) {
        QTreeWidgetItem* top = new QTreeWidgetItem(m_resultTree);
        top->setText(0, providerId);
        QGeoServiceProvider* serviceProvider = new QGeoServiceProvider(providerId);

        QTreeWidgetItem* searchItem = new QTreeWidgetItem(top);
        searchItem->setText(0, "Search");
        QGeoSearchManager* searchManager = serviceProvider->searchManager();
        if (searchManager)
            searchItem->setText(1, tr("true"));
        else
            searchItem->setText(1, tr("false"));

        QTreeWidgetItem* mappingItem = new QTreeWidgetItem(top);
        mappingItem->setText(0, "Mapping");
        QGeoMappingManager* mappingManager = serviceProvider->mappingManager();
        if (mappingManager)
            mappingItem->setText(1, tr("true"));
        else
            mappingItem->setText(1, tr("false"));

        QTreeWidgetItem* routingItem = new QTreeWidgetItem(top);
        routingItem->setText(0, "Routing");
        QGeoRoutingManager* routingManager = serviceProvider->routingManager();
        if (routingManager)
            routingItem->setText(1, tr("true"));
        else
            routingItem->setText(1, tr("false"));

        delete serviceProvider;
    }
}
