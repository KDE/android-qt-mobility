/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "subscriberdialog.h"

#include <QApplication>

#include <qvaluespace.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    bool createDefault = true;
    bool createPublisher = false;
    bool createSubscriber = false;

    for (int i = 1; i < argc; ++i) {
        if (argv[i] == QLatin1String("-server")) {
            QValueSpace::initValueSpaceServer();
        } else if (argv[i] == QLatin1String("-publisher")) {
            createPublisher = true;
            createDefault = false;
        } else if (argv[i] == QLatin1String("-subscriber")) {
            createSubscriber = true;
            createDefault = false;
        }
    }

    PublisherDialog *publisher = 0;
    if (createDefault || createPublisher) {
        publisher = new PublisherDialog;
        QObject::connect(publisher, SIGNAL(rejected()), &app, SLOT(quit()));
#ifndef QTM_EXAMPLES_SMALL_SCREEN
        publisher->show();
#endif
    }

    SubscriberDialog *subscriber = 0;
    if (createDefault || createSubscriber) {
        subscriber = new SubscriberDialog;
        QObject::connect(subscriber, SIGNAL(rejected()), &app, SLOT(quit()));
#ifndef QTM_EXAMPLES_SMALL_SCREEN
        subscriber->show();
#else
        subscriber->showMaximized();
#endif
    }

#ifdef QTM_EXAMPLES_SMALL_SCREEN
    QObject::connect(publisher, SIGNAL(switchRequested()), subscriber, SLOT(showMaximized()));
    QObject::connect(publisher, SIGNAL(switchRequested()), subscriber, SLOT(repaint()));
    QObject::connect(publisher, SIGNAL(switchRequested()), publisher, SLOT(hide()));

    QObject::connect(subscriber, SIGNAL(switchRequested()), publisher, SLOT(showMaximized()));
    QObject::connect(subscriber, SIGNAL(switchRequested()), publisher, SLOT(repaint()));
    QObject::connect(subscriber, SIGNAL(switchRequested()), subscriber, SLOT(hide()));
#endif

    int result = app.exec();

    if (publisher)
        delete publisher;
    if (subscriber)
        delete subscriber;

    return result;
}
