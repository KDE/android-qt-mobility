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

#include <QtGui/QApplication>
#include "mainwindow.h"

#include <QUrl>
#include <QVariant>
#include <QNetworkProxyFactory>

int main(int argc, char * argv[])
{
    QApplication a(argc, argv);

    //QString urlEnv = QProcessEnvironment::systemEnvironment().value("http_proxy");
    QString urlEnv = qgetenv("http_proxy");
    if (!urlEnv.isEmpty()) {
        QUrl url = QUrl(urlEnv, QUrl::TolerantMode);
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(url.host());
        proxy.setPort(url.port(8080));
        QNetworkProxy::setApplicationProxy(proxy);
    } else
        QNetworkProxyFactory::setUseSystemConfiguration(true);

    QVariantHash parameters;

    QStringList args = QApplication::arguments();

    while (!args.isEmpty()) {
        QString word = args.takeFirst();
        if (word[0] == QChar('-')) {
            word.remove(0, 1);
            if (args.isEmpty() || args.first()[0] == QChar('-')) {
                parameters[word] = true;
            }
            else {
                QString value = args.takeFirst();
                if (value == "true" || value == "on" || value == "enabled") {
                    parameters[word] = true;
                }
                else if (value == "false" || value == "off" || value == "disabled") {
                    parameters[word] = false;
                }
                else {
                    bool ok = false;
                    double realValue = value.toDouble(&ok);
                    if (ok) {
                        parameters[word] = realValue;
                    }
                    else {
                        parameters[word] = value;
                    }
                }
            }
        }
    }

    MainWindow w(parameters);
#if defined(Q_OS_SYMBIAN) || defined(Q_OS_WINCE_WM) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    w.showMaximized();
#else
    w.show();
#endif

    return a.exec();
}
