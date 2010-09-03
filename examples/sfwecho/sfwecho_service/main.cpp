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
#include <qremoteserviceclassregister.h>
#include <qremoteservicecontrol.h>
#include "qservicemanager.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>

QTM_USE_NAMESPACE

class EchoSharedService : public QObject
{
    Q_OBJECT
    //Q_SERVICE(EchoSharedService, "EchoService", "com.nokia.qt.example.sfwecho", "1.1")

public:
    EchoSharedService(QObject* parent = 0)
        : QObject(parent)
    {
    }

Q_SIGNALS:
    void broadcastMessage(const QString &msg, const QDateTime &timestamp);

public slots:
    void sendMessage(const QString &msg)
    {
        emit broadcastMessage(msg, QDateTime::currentDateTime());
    }

private:

};

class EchoUniqueService : public QObject
{
    Q_OBJECT
    //Q_SERVICE(EchoUniqueService, "EchoService", "com.nokia.qt.example.sfwecho", "1.0")

public:
    EchoUniqueService(QObject* parent = 0)
        : QObject(parent)
    {
    }

Q_SIGNALS:
    void broadcastMessage(const QString &msg, const QDateTime &timestamp);

public slots:
    void sendMessage(const QString &msg)
    {
        emit broadcastMessage(msg, QDateTime::currentDateTime());
    }

private:

};

void unregisterExampleService()
{
    QServiceManager m;
    m.removeService("EchoService");
}

void registerExampleService()
{
    unregisterExampleService();
    QServiceManager m;
    const QString path = QCoreApplication::applicationDirPath() + "/xmldata/sfwechoservice.xml";
    if (!m.addService(path)) {
        qWarning() << "Cannot register EchoService" << path;
    } else {
        qDebug() << "Registered: " << path;
    }
}

Q_DECLARE_METATYPE(QMetaType::Type);

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    registerExampleService();

    //QRemoteServiceClassRegister::registerType<EchoSharedService>(QRemoteServiceClassRegister::SharedInstance);
    //QRemoteServiceClassRegister::registerType<EchoUniqueService>(QRemoteServiceClassRegister::UniqueInstance);
    QRemoteServiceControl* control = new QRemoteServiceControl();

    QRemoteServiceControl::Entry shared = control->createServiceEntry<EchoSharedService>(
        "EchoService", "com.nokia.qt.example.sfwecho", "1.1");
    shared.setInstanciationType(QRemoteServiceClassRegister::SharedInstance);
    control->registerService(shared);

    QRemoteServiceControl::Entry unique = control->createServiceEntry<EchoUniqueService>(
        "EchoService", "com.nokia.qt.example.sfwecho", "1.0");
    unique.setInstanciationType(QRemoteServiceClassRegister::UniqueInstance);
    control->registerService(unique);

    control->publishServices("sfwecho_service");

    int res =  app.exec();

    delete control;    
    return res;
}



#include "main.moc"
