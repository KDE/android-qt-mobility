/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include <QDebug>
#include <qdeclarativeextensionplugin.h>

#include <qdeclarativeengine.h>
#include <qdeclarative.h>
#include "qdeclarativebluetoothimageprovider_p.h"


QTM_USE_NAMESPACE

// This is run in a low priority thread.
QImage BluetoothThumbnailImageProvider::requestImage(const QString &id, QSize *size, const QSize &req_size)
{
    if (m_thumbnails.contains(id)) {
        if (size)
            *size = req_size;
        return m_thumbnails.value(id).scaled(req_size);
    }

    /* url format:
        image://bluetoothicons/{hosttype}
     */


    QImage image(
            req_size.width() > 0 ? req_size.width() : 100,
            req_size.height() > 0 ? req_size.height() : 50,
            QImage::Format_RGB32);

    QString imageUrl;

    if(id == "default")
        imageUrl = QLatin1String(":/default.svg");

    imageUrl = imageUrl.isEmpty() ? QLatin1String(":/default.svg") : imageUrl;
    image.load(imageUrl);

    if (size)
        *size = image.size();

    m_thumbnails.insert(id, image);

    return image;
}

BluetoothThumbnailImageProvider::BluetoothThumbnailImageProvider()
    :QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
{

}

BluetoothThumbnailImageProvider::~BluetoothThumbnailImageProvider()
{
}


