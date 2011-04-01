/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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


#include <QApplication>
#include <QtDeclarative>
#include <QDeclarativeExtensionPlugin>
#include <QDebug>
#include "qdeclarativebtdiscoverymodel_p.h"
#include "qdeclarativebtimageprovider_p.h"
#include "qdeclarativebluetoothservice_p.h"
#include "qdeclarativebluetoothsocket_p.h"
#include "qdeclarativenearfieldsocket_p.h"

QT_USE_NAMESPACE

QTM_USE_NAMESPACE
class QConnectivityQmlPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    void registerTypes(const char *uri)
    {
        Q_ASSERT(uri == QLatin1String("QtMobility.connectivity"));
        //qRegisterMetaType<QContactLocalId>("QContactLocalId");
        qmlRegisterType<QDeclarativeBluetoothDiscoveryModel>(uri, 1, 2, "BluetoothDiscoveryModel");
        qmlRegisterType<QDeclarativeBluetoothService>(uri, 1, 2, "BluetoothService");
        qmlRegisterType<QDeclarativeBluetoothSocket>(uri, 1, 2, "BluetoothSocket");

        qmlRegisterType<QDeclarativeNearFieldSocket>(uri, 1, 2, "NearFieldSocket");

        //details
    }

    void initializeEngine(QDeclarativeEngine *engine, const char *uri) {
        Q_UNUSED(uri);
        engine->addImageProvider("bluetoothicons", new BluetoothThumbnailImageProvider);
    }
};

#include "plugin.moc"

Q_EXPORT_PLUGIN2(qconnectivityqmlplugin, QConnectivityQmlPlugin);
