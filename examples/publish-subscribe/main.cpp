/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact:  Nokia Corporation (qt-info@nokia.com)**
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
** will be met:  http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "providerdialog.h"
#include "consumerdialog.h"

#include <QApplication>

#include <qvaluespace.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    bool createDefault = true;
    bool createProvider = false;
    bool createConsumer = false;

    for (int i = 1; i < argc; ++i) {
        if (argv[i] == QLatin1String("-manager")) {
            QValueSpace::initValuespaceManager();
        } else if (argv[i] == QLatin1String("-provider")) {
            createProvider = true;
            createDefault = false;
        } else if (argv[i] == QLatin1String("-consumer")) {
            createConsumer = true;
            createDefault = false;
        }
    }

    ProviderDialog *provider = 0;
    if (createDefault || createProvider) {
        provider = new ProviderDialog;
        QObject::connect(provider, SIGNAL(rejected()), &app, SLOT(quit()));
        provider->show();
    }

    ConsumerDialog *consumer = 0;
    if (createDefault || createConsumer) {
        consumer = new ConsumerDialog;
        QObject::connect(consumer, SIGNAL(rejected()), &app, SLOT(quit()));
        consumer->show();
    }

    app.exec();

    if (provider)
        delete provider;
    if (consumer)
        delete consumer;
}
