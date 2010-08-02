/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "geocodingtab.h"
#include "placepresenter.h"

#include <QTreeWidget>
#include <QLineEdit>
#include <QString>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <qgeoaddress.h>

GeocodingTab::GeocodingTab(QWidget *parent) :
        QWidget(parent),
        m_searchManager(NULL)
{
    m_obloc = new QLineEdit("Deutschland, M�nchen");
    m_obloc->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    QLabel *countrylbl = new QLabel(tr("Country:"));
    m_country = new QLineEdit("");
    m_country->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QLabel *statelbl = new QLabel(tr("State:"));
    m_state = new QLineEdit("");
    m_state->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QLabel *citylbl = new QLabel(tr("City:"));
    m_city = new QLineEdit("");
    m_city->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QLabel *ziplbl = new QLabel(tr("Zip:"));
    m_zip = new QLineEdit("");
    m_zip->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QLabel *streetlbl = new QLabel(tr("Street:"));
    m_street = new QLineEdit("");
    m_street->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QLabel *streetnumlbl = new QLabel(tr("#:"));
    m_streetNumber = new QLineEdit("");
    m_streetNumber->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    requestBtn = new QPushButton(tr("Geocoding"));
    requestBtn->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    requestBtn->setDisabled(true);
    QObject::connect(requestBtn, SIGNAL(clicked(bool)), this, SLOT(on_btnRequest_clicked()));

    m_resultTree = new QTreeWidget();
    QStringList labels;
    labels << tr("Elements") << tr("Value");
    m_resultTree->setHeaderLabels(labels);
    m_resultTree->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_resultTree->setColumnWidth(0, 150);

    QHBoxLayout *firstrow = new QHBoxLayout;
    firstrow->setSpacing(0);
    firstrow->setContentsMargins(0, 0, 0, 0);
    firstrow->addWidget(m_obloc);
    firstrow->addWidget(requestBtn);

    QHBoxLayout *secondrow = new QHBoxLayout;
    secondrow->setSpacing(0);
    secondrow->setContentsMargins(0, 0, 0, 0);
    secondrow->addWidget(streetlbl);
    secondrow->addWidget(m_street);
    secondrow->addWidget(streetnumlbl);
    secondrow->addWidget(m_streetNumber);
    secondrow->addWidget(ziplbl);
    secondrow->addWidget(m_zip);

    QHBoxLayout *thirdrow = new QHBoxLayout;
    thirdrow->setSpacing(0);
    thirdrow->setContentsMargins(0, 0, 0, 0);
    thirdrow->addWidget(citylbl);
    thirdrow->addWidget(m_city);
    thirdrow->addWidget(statelbl);
    thirdrow->addWidget(m_state);
    thirdrow->addWidget(countrylbl);
    thirdrow->addWidget(m_country);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(2);
    mainLayout->setContentsMargins(2, 1, 2, 1);
    mainLayout->addLayout(firstrow);
    mainLayout->addLayout(secondrow);
    mainLayout->addLayout(thirdrow);
    mainLayout->addWidget(m_resultTree);
    setLayout(mainLayout);
}

GeocodingTab::~GeocodingTab()
{
}

void GeocodingTab::initialize(QGeoSearchManager *searchManager)
{
    m_resultTree->clear();
    m_searchManager = searchManager;
    if (m_searchManager) {
        QObject::connect(m_searchManager, SIGNAL(finished(QGeoSearchReply*)), this,
                         SLOT(replyFinished(QGeoSearchReply*)));
        QObject::connect(m_searchManager,
                         SIGNAL(error(QGeoSearchReply*, QGeoSearchReply::Error, QString)), this,
                         SLOT(resultsError(QGeoSearchReply*, QGeoSearchReply::Error, QString)));
        if(m_searchManager->supportsGeocoding())
            requestBtn->setDisabled(false);
    }
    else
        requestBtn->setDisabled(true);

}

void GeocodingTab::on_btnRequest_clicked()
{
    if (m_searchManager) {
        QString s = m_obloc->text();

        m_resultTree->clear();

        if (!s.isEmpty()) {
            m_searchManager->placeSearch(s, QGeoSearchManager::SearchGeocode);
        } else {
            QGeoAddress address;
            address.setCountry(m_country->text());
            address.setState(m_state->text());
            address.setCity(m_city->text());
            address.setPostCode(m_zip->text());
            address.setStreet(m_street->text());
            address.setStreetNumber(m_streetNumber->text());
            m_searchManager->geocode(address);
        }
    } else {
        QMessageBox::warning(this, tr("Geocoding"), tr("No search manager available."));
    }
}

void GeocodingTab::replyFinished(QGeoSearchReply* reply)
{
    if (!isHidden() && reply->error() == QGeoSearchReply::NoError) {
        PlacePresenter presenter(m_resultTree, reply);
        presenter.show();
        reply->deleteLater();
    }
}

void GeocodingTab::resultsError(QGeoSearchReply* reply, QGeoSearchReply::Error errorCode, QString errorString)
{
    Q_UNUSED(errorCode)

    QTreeWidgetItem* top = new QTreeWidgetItem(m_resultTree);
    top->setText(0, tr("Error"));
    top->setText(1, errorString);
    reply->deleteLater();
}
