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
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
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
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativegeocodemodel_p.h"

#include <qgeosearchmanager.h>

QTM_BEGIN_NAMESPACE

QDeclarativeGeocodeModel::QDeclarativeGeocodeModel(QObject *parent)
    : QDeclarativeGeoSearchModel(parent),
      complete_(false) {}

QDeclarativeGeocodeModel::~QDeclarativeGeocodeModel() {}

void QDeclarativeGeocodeModel::setAddress(QDeclarativeGeoAddress *address)
{
    if (address_.address() == address->address())
        return;

    address_.setAddress(address->address());

    emit addressChanged(&address_);

    if (complete_)
        update();
}

QDeclarativeGeoAddress* QDeclarativeGeocodeModel::address()
{
    return &address_;
}

void QDeclarativeGeocodeModel::componentComplete()
{
    if (!searchManager())
        return;

    complete_ = true;
    update();
}

void QDeclarativeGeocodeModel::update()
{
    if (searchManager() && !address_.address().isEmpty())
        searchManager()->geocode(address_.address());
}

QVariant QDeclarativeGeocodeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() > places().count())
        return QVariant();

    QGeoPlace place = places().at(index.row());

    if (role == Qt::DisplayRole)
        return place.coordinate().toString();

    return QDeclarativeGeoSearchModel::data(index, role);
}

QVariant QDeclarativeGeocodeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section != 0)
        return QVariant();

    if (role == Qt::DisplayRole)
        return QString("Coordinate");

    return QDeclarativeGeoSearchModel::headerData(section, orientation, role);
}


#include "moc_qdeclarativegeocodemodel_p.cpp"

QTM_END_NAMESPACE
