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

#ifndef DATABASEMANAGER_H_
#define DATABASEMANAGER_H_

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
#include "servicedatabase_p.h"
#include <QObject>

#ifdef QT_SFW_SERVICEDATABASE_GENERATE
#undef QM_AUTOTEST_EXPORT
#define QM_AUTOTEST_EXPORT
#endif

QT_BEGIN_HEADER
QTM_BEGIN_NAMESPACE

class DatabaseFileWatcher;
class QM_AUTOTEST_EXPORT DatabaseManager : public QObject
{
    Q_OBJECT

    public:
        enum DbScope{UserScope, SystemScope, UserOnlyScope};
        DatabaseManager(void);
        virtual ~DatabaseManager();

        bool registerService(ServiceMetaDataResults &service, DbScope scope);
        bool unregisterService(const QString &serviceName, DbScope scope);
        bool serviceInitialized(const QString &serviceName, DbScope scope);

        QList<QServiceInterfaceDescriptor> getInterfaces(const QServiceFilter &filter, DbScope scope);
        QStringList getServiceNames(const QString &interfaceName, DbScope scope);

        QServiceInterfaceDescriptor interfaceDefault(const QString &interfaceName, DbScope scope);
        bool setInterfaceDefault(const QString &serviceName, const QString &interfaceName, DbScope scope);
        bool setInterfaceDefault(const QServiceInterfaceDescriptor &interface, DbScope scope);

        DBError lastError(){ return m_lastError;}

        void setChangeNotificationsEnabled(DbScope scope, bool enabled);

    signals:
        void serviceAdded(const QString &serviceName, DatabaseManager::DbScope scope);
        void serviceRemoved(const QString &serviceName, DatabaseManager::DbScope scope);

    private:
        void initDbPath(DbScope scope);
        bool openDb(DbScope scope);

        ServiceDatabase *m_userDb;
        ServiceDatabase *m_systemDb;
        DBError m_lastError;

        friend class DatabaseFileWatcher;
        DatabaseFileWatcher *m_fileWatcher;
        QServiceInterfaceDescriptor latestDescriptor(const QList<QServiceInterfaceDescriptor> &descriptors);

        bool m_hasAccessedUserDb;
        bool m_alreadyWarnedOpenError;
};


class QM_AUTOTEST_EXPORT DatabaseFileWatcher : public QObject
{
    Q_OBJECT
public:
    DatabaseFileWatcher(DatabaseManager *parent = 0);

    void setEnabled(ServiceDatabase *database, bool enabled);

private slots:
    void databaseChanged(const QString &path);
    void databaseDirectoryChanged(const QString &path);

private:
    void notifyChanges(ServiceDatabase *database, DatabaseManager::DbScope scope);
    QString closestExistingParent(const QString &path);
    void restartDirMonitoring(const QString &dbPath, const QString &previousDirPath);

    DatabaseManager *m_manager;
    QFileSystemWatcher *m_watcher;
    QHash<QString, QStringList> m_knownServices;
    QStringList m_monitoredDbPaths;
};

QTM_END_NAMESPACE
QT_END_HEADER

#endif
