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

/* Part of the Maps Demo tutorial */
/* You probably want to read it first, look under Tutorials in
   the Mobility documentation */

#include "mainwindow.h"

#include <QApplication>
#include <QList>
#include <QString>
#include <QUrl>
#include <QSettings>
#include <QProcessEnvironment>
#include <QNetworkProxyFactory>

#include "qgeoserviceprovider.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // not in the tutorial text: set up a proxy server from
    // a QSettings file if necessary (useful on Linux)

    QApplication::setOrganizationName("Nokia");
    QApplication::setApplicationName("MapsDemo");

    QSettings settings;

    QVariant value = settings.value("http.proxy");
    if (value.isValid()) {
        QUrl url(value.toString(), QUrl::TolerantMode);
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(url.host());
        proxy.setPort(url.port(8080));
        QNetworkProxy::setApplicationProxy(proxy);
    }

    // launch the main window
    MainWindow mw;
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_SIMULATOR)
    mw.showMaximized();
#else
    mw.resize(360,640);
    mw.show();
#endif
    return a.exec();
}
