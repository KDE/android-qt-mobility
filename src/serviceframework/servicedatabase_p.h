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

#ifndef QSERVICEDATABASE_H_
#define QSERVICEDATABASE_H_

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qmobilityglobal.h"
#include <QtSql>
#include <QList>
#include "servicemetadata_p.h"
#include "qservicefilter.h"
#include "dberror_p.h"


QT_BEGIN_HEADER
QTM_BEGIN_NAMESPACE

class QServiceInterfaceDescriptor;

class QM_AUTOTEST_EXPORT ServiceDatabase : public QObject
{
    Q_OBJECT

    public:
        ServiceDatabase(void);

        virtual ~ServiceDatabase();

        bool open();
        bool close();

        bool isOpen() const;
        void setDatabasePath(const QString &databasePath);
        QString databasePath() const;

        bool registerService(const ServiceMetaDataResults &service, const QString &securityToken = QString());
        bool unregisterService(const QString &serviceName, const QString &securityToken = QString());
        bool serviceInitialized(const QString &serviceName, const QString &securityToken = QString());

        QList<QServiceInterfaceDescriptor> getInterfaces(const QServiceFilter &filter);
        QServiceInterfaceDescriptor getInterface(const QString &interfaceID);
        QString getInterfaceID(const QServiceInterfaceDescriptor &interface);
        QStringList getServiceNames(const QString &interfaceName);

        QServiceInterfaceDescriptor interfaceDefault(const QString &interfaceName,
                                    QString *interfaceID = 0, bool inTransaction = false);
        bool setInterfaceDefault(const QServiceInterfaceDescriptor &interface,
                                const QString &externalInterfaceID = QString());
        QList<QPair<QString,QString> > externalDefaultsInfo();
        bool removeExternalDefaultServiceInterface(const QString &interfaceID);

        DBError lastError() const { return m_lastError; }

Q_SIGNALS:
        void serviceAdded(const QString& serviceName);
        void serviceRemoved(const QString& serviceName);

#ifdef QTM_BUILD_UNITTESTS
    public:
#else
    private:
#endif
        enum TransactionType{Read, Write};

        bool createTables();
        bool dropTables();
        bool checkTables();

        bool checkConnection();

        bool executeQuery(QSqlQuery *query, const QString &statement, const QList<QVariant> &bindValues = QList<QVariant>());
        QString getInterfaceID(QSqlQuery *query, const QServiceInterfaceDescriptor &interface);
        bool insertInterfaceData(QSqlQuery *query, const QServiceInterfaceDescriptor &anInterface, const QString &serviceID);

        bool beginTransaction(QSqlQuery *query, TransactionType);
        bool commitTransaction(QSqlQuery *query);
        bool rollbackTransaction(QSqlQuery *query);

        bool populateInterfaceProperties(QServiceInterfaceDescriptor *descriptor, const QString &interfaceID);
        bool populateServiceProperties(QServiceInterfaceDescriptor *descriptor, const QString &serviceID);

        QString m_databasePath;
        QString m_connectionName;
        bool m_isDatabaseOpen;
        bool m_inTransaction;
        DBError m_lastError;
};

QTM_END_NAMESPACE
QT_END_HEADER

#endif /*QSERVICEDATABASE_H_*/
