/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QCoreApplication>
#include <QFile>

#include <qnetworkconfigmanager.h>
#include <qnetworkconfiguration.h>
#include <qnetworksession.h>

#include <QDebug>


#define NO_DISCOVERED_CONFIGURATIONS_ERROR 1
#define SESSION_OPEN_ERROR 2

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    QNetworkConfigurationManager manager;
    QList<QNetworkConfiguration> discovered =
        manager.allConfigurations(QNetworkConfiguration::Discovered);

    if (discovered.isEmpty())
        return NO_DISCOVERED_CONFIGURATIONS_ERROR;

    QFile standardOutput;
    standardOutput.open(1, QIODevice::WriteOnly);
    QTextStream output(&standardOutput);

    qDebug() << "Lackey started";

    QNetworkSession *session = 0;
    do {
        if (session) {
            delete session;
            session = 0;
        }

        qDebug() << "Discovered configurations:" << discovered.count();

        if (discovered.isEmpty()) {
            qDebug() << "No more configurations";
            break;
        }

        qDebug() << "Taking first configuration";

        QNetworkConfiguration config = discovered.takeFirst();
        if ((config.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active) {
            qDebug() << config.name() << "is active";
            continue;
        }

        qDebug() << "Creating session for" << config.name() << config.identifier();

        session = new QNetworkSession(config);
        output << "Starting session for " << config.identifier() << '\n';
        output.flush();

        session->open();
        session->waitForOpened();
    } while (!(session && session->isActive()));

    qDebug() << "loop done";

    if (!session) {
        qDebug() << "Could not start session";
        return SESSION_OPEN_ERROR;
    }

    output << "Started session " << session->configuration().identifier() << '\n';
    output.flush();

    QFile input;
    input.open(0, QIODevice::ReadOnly);

    // Wait for command from test code.
    input.readLine();

    session->stop();

    delete session;

    return 0;
}
