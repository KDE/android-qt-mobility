/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#ifndef QOFONOSERVICE_H
#define QOFONOSERVICE_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qmobilityglobal.h"
#include <QtDBus/qdbusabstractinterface.h>
#include <QtDBus/qdbuscontext.h>

struct QOfonoProperties
{
    QDBusObjectPath path;
    QVariantMap properties;
};
Q_DECLARE_METATYPE(QOfonoProperties)

typedef QList<QOfonoProperties> QOfonoPropertyMap;
Q_DECLARE_METATYPE(QOfonoPropertyMap)

QTM_BEGIN_NAMESPACE

class QOfonoManagerInterface : public QDBusAbstractInterface
{
    Q_OBJECT

public:
    QOfonoManagerInterface( QObject *parent = 0);
    ~QOfonoManagerInterface();

    QDBusObjectPath currentModem();
    QList<QDBusObjectPath> getModems();

Q_SIGNALS:
    void propertyChangedContext(const QString &path, const QString &item, const QDBusVariant &value);

protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);

private:
    QVariant getProperty(const QString &property);
};

class QOfonoDBusHelper: public QObject, protected QDBusContext
{
    Q_OBJECT

public:
    QOfonoDBusHelper(QObject *parent = 0);
    ~QOfonoDBusHelper();

public Q_SLOTS:
    void propertyChanged(const QString &item, const QDBusVariant &value);

Q_SIGNALS:
    void propertyChangedContext(const QString &path, const QString &item, const QDBusVariant &value);
};

class QOfonoModemInterface : public QDBusAbstractInterface
{
    Q_OBJECT

public:
    QOfonoModemInterface(const QString &dbusModemPathName, QObject *parent = 0);
    ~QOfonoModemInterface();

    bool isPowered();
    QString getSerial();

private:
    QVariant getProperty(const QString &property);
};

class QOfonoNetworkRegistrationInterface : public QDBusAbstractInterface
{
    Q_OBJECT

public:
    QOfonoNetworkRegistrationInterface(const QString &dbusModemPathName, QObject *parent = 0);
    ~QOfonoNetworkRegistrationInterface();

    int getSignalStrength();
    QList<QDBusObjectPath> getOperators();
    QString getOperatorName();
    QString getStatus();
    QString getTechnology();
    quint16 getLac();
    quint32 getCellId();

protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);

private:
    QVariant getProperty(const QString &property);

Q_SIGNALS:
    void propertyChangedContext(const QString &path, const QString &item, const QDBusVariant &value);
};

class QOfonoNetworkOperatorInterface : public QDBusAbstractInterface
{
    Q_OBJECT

public:
    QOfonoNetworkOperatorInterface(const QString &dbusPathName, QObject *parent = 0);
    ~QOfonoNetworkOperatorInterface();

    QString getMcc();
    QString getMnc();
    QString getStatus();
    QStringList getTechnologies();

private:
    QVariant getProperty(const QString &property);
};

class QOfonoSimInterface : public QDBusAbstractInterface
{
    Q_OBJECT

public:
    QOfonoSimInterface(const QString &dbusModemPathName, QObject *parent = 0);
    ~QOfonoSimInterface();

    bool isPresent();
    QString getHomeMcc();
    QString getHomeMnc();
    QString getImsi();
    QString pinRequired();

private:
    QVariant getProperty(const QString &property);
};

class QOfonoConnectionManagerInterface : public QDBusAbstractInterface
{
    Q_OBJECT

public:
    QOfonoConnectionManagerInterface(const QString &dbusPathName, QObject *parent = 0);
    ~QOfonoConnectionManagerInterface();

    QString bearer();

private:
    QVariant getProperty(const QString &);
};

QTM_END_NAMESPACE

#endif //QOFONOSERVICE_H
