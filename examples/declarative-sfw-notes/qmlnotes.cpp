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
#include <QtDeclarative/QDeclarativeView>
#include <QtDeclarative/QDeclarativeEngine>
#ifndef Q_OS_SYMBIAN
#include <qservicemanager.h>

QTM_USE_NAMESPACE

void unregisterExampleService()
{
    QServiceManager m;
    m.removeService("NotesManagerService");
}

void registerExampleService()
{
    unregisterExampleService();

    QServiceManager m;
    const QString path = QCoreApplication::applicationDirPath() + "/xmldata/notesmanagerservice.xml";
    if (!m.addService(path)) {
        qWarning() << "Unable to register notes manager service";
    }
}
#endif

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

#ifndef Q_OS_SYMBIAN
    registerExampleService();
#endif

#if defined(Q_WS_MAEMO_5) | defined(Q_WS_MAEMO_6)
    const QString mainQmlApp = QLatin1String("qrc:/maemo-sfw-notes.qml");
#else
    const QString mainQmlApp = QLatin1String("qrc:/declarative-sfw-notes.qml");
#endif
    QDeclarativeView view;
    view.setSource(QUrl(mainQmlApp));
    view.setResizeMode(QDeclarativeView::SizeRootObjectToView);
    // Qt.quit() called in embedded .qml by default only emits
    // quit() signal, so do this (optionally use Qt.exit()).
    QObject::connect(view.engine(), SIGNAL(quit()), qApp, SLOT(quit()));
#if defined(Q_OS_SYMBIAN)
    view.showFullScreen();
#else // Q_OS_SYMBIAN
    view.show();
#endif // Q_OS_SYMBIAN

    int ret = application.exec();
#ifndef Q_OS_SYMBIAN
    unregisterExampleService();
#endif
    return ret;
}
