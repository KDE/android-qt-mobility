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
#include "modestengine_maemo_p.h"
#include "maemohelpers_p.h"
#include "qmessageaccount.h"
#include "qmessageaccount_p.h"
#include "qmessageaccountfilter.h"
#include "qmessageaccountfilter_p.h"
#include "qmessagefolder_p.h"
#include "qmessageservice.h"
#include "qmessageservice_maemo_p.h"
#include "qmessagecontentcontainer_maemo_p.h"
#include <QUrl>
#include <QtDBus>
#include <QFileSystemWatcher>
#include <dbus/dbus.h>
#include <QDebug>

#include </usr/include/libmodest-dbus-client-1.0/libmodest-dbus-client/libmodest-dbus-api.h>
#include </usr/include/sys/inotify.h>
#include </usr/include/sys/ioctl.h>
#include </usr/include/sys/fcntl.h>

// Marshall the ModestStringMap data into a D-Bus argument
QDBusArgument &operator<<(QDBusArgument &argument,
                          const QtMobility::ModestStringMap &map)
{
    QtMobility::ModestStringMap::const_iterator iter;

    argument.beginMap (QVariant::String, QVariant::String);
    for (iter = map.constBegin(); iter != map.constEnd(); iter++) {
        argument.beginMapEntry();
        argument << iter.key() << iter.value();
        argument.endMapEntry();
    }
    argument.endMap();

    return argument;
}

// Retrieve the ModestStringMap data from the D-Bus argument
const QDBusArgument &operator>>(const QDBusArgument &argument,
                                QtMobility::ModestStringMap &map)
{
    map.clear();

    argument.beginMap();
    while (!argument.atEnd()) {
        QString key, value;
        argument.beginMapEntry();
        argument >> key >> value;
        argument.endMapEntry();
        map[key] = value;
    }
    argument.endMap();

    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument,
                          const QtMobility::ModestUnreadMessageDBusStruct &unreadMessage)
{
    argument.beginStructure();
    argument << unreadMessage.timeStamp;
    argument << unreadMessage.subject;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument,
                                QtMobility::ModestUnreadMessageDBusStruct &unreadMessage)
{
    argument.beginStructure();
    argument >> unreadMessage.timeStamp;
    argument >> unreadMessage.subject;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument,
                          const QtMobility::ModestAccountsUnreadMessagesDBusStruct &unreadMessages)
{
    argument.beginStructure();
    argument << unreadMessages.accountId;
    argument << unreadMessages.accountName;
    argument << unreadMessages.accountProtocol;
    argument << unreadMessages.unreadCount;
    argument << unreadMessages.unreadMessages;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument,
                                QtMobility::ModestAccountsUnreadMessagesDBusStruct &unreadMessages)
{
    argument.beginStructure();
    argument >> unreadMessages.accountId;
    argument >> unreadMessages.accountName;
    argument >> unreadMessages.accountProtocol;
    argument >> unreadMessages.unreadCount;
    argument >> unreadMessages.unreadMessages;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const QtMobility::ModestMessage &message)
{
    argument.beginStructure();
    argument << message.id;
    argument << message.subject;
    argument << message.folder;
    argument << message.sender;
    argument << message.size;
    argument << message.hasAttachment;
    argument << message.isUnread;
    argument << message.timeStamp;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, QtMobility::ModestMessage &message)
{
    argument.beginStructure();
    argument >> message.id;
    argument >> message.subject;
    argument >> message.folder;
    argument >> message.sender;
    argument >> message.size;
    argument >> message.hasAttachment;
    argument >> message.isUnread;
    argument >> message.timeStamp;
    argument.endStructure();
    return argument;
}

QTM_BEGIN_NAMESPACE

/* configuration key definitions for modest */
#define MODESTENGINE_ACCOUNT_NAMESPACE         "/apps/modest/accounts"
#define MODESTENGINE_SERVER_ACCOUNT_NAMESPACE  "/apps/modest/server_accounts"
#define MODESTENGINE_DEFAULT_ACCOUNT           "/apps/modest/default_account"
#define MODESTENGINE_ACCOUNT_ENABLED           "enabled"
#define MODESTENGINE_ACCOUNT_DISPLAY_NAME      "display_name"
#define MODESTENGINE_ACCOUNT_FULLNAME          "fullname"
#define MODESTENGINE_ACCOUNT_EMAIL             "email"
#define MODESTENGINE_ACCOUNT_STORE_ACCOUNT     "store_account"
#define MODESTENGINE_ACCOUNT_TRANSPORT_ACCOUNT "transport_account"
#define MODESTENGINE_ACCOUNT_PROTOCOL          "proto"
#define MODESTENGINE_ACCOUNT_USERNAME          "username"
#define MODESTENGINE_ACCOUNT_HOSTNAME          "hostname"
#define MODESTENGINE_ACCOUNT_PORT              "port"

// The modest engine has a new plugin, we need service names for it
#define MODESTENGINE_QTM_PLUGIN_PATH           "/com/nokia/Qtm/Modest/Plugin"
#define MODESTENGINE_QTM_PLUGIN_NAME           "com.nokia.Qtm.Modest.Plugin"

typedef enum {
    MODEST_DBUS_SEARCH_SUBJECT   = (1 << 0),
    MODEST_DBUS_SEARCH_SENDER    = (1 << 1),
    MODEST_DBUS_SEARCH_RECIPIENT = (1 << 2),
    MODEST_DBUS_SEARCH_SIZE      = (1 << 3),
    MODEST_DBUS_SEARCH_BODY      = (1 << 6)
} ModestDBusSearchFlags;

// Specific priority settings to translate to modest priorities
#define MODESTENGINE_HIGH_PRIORITY             2
#define MODESTENGINE_NORMAL_PRIORITY           0
#define MODESTENGINE_LOW_PRIORITY              1

Q_GLOBAL_STATIC(ModestEngine,modestEngine);

ModestEngine::ModestEngine()
 : m_queryIds(0)
{
    g_type_init();
    m_gconfclient = gconf_client_get_default();
    if (!m_gconfclient) {
        qWarning("qtmessaging: could not get gconf client");
    } else {
        updateEmailAccounts();
    }

    // Setup DBus Interface for Modest
    m_ModestDBusInterface = new QDBusInterface(MODEST_DBUS_SERVICE,
                                               MODEST_DBUS_OBJECT,
                                               MODEST_DBUS_IFACE,
                                               QDBusConnection::sessionBus(),
                                               this);

    // Get notifications of Incoming Messages
    m_ModestDBusInterface->connection().connect(MODEST_DBUS_SERVICE,
                                                MODEST_DBUS_OBJECT,
                                                MODEST_DBUS_IFACE,
                                                MODEST_DBUS_SIGNAL_FOLDER_UPDATED,
                                                this, SLOT(folderUpdatedSlot(QDBusMessage)));

    // Get notifications of message Read/Unread state changes
    m_ModestDBusInterface->connection().connect(MODEST_DBUS_SERVICE,
                                                MODEST_DBUS_OBJECT,
                                                MODEST_DBUS_IFACE,
                                                MODEST_DBUS_SIGNAL_MSG_READ_CHANGED,
                                               this, SLOT(messageReadChangedSlot(QDBusMessage)));

    // Setup Qt Mobility Modest Plugin based DBus Interface for Modest
    m_QtmPluginDBusInterface = new QDBusInterface(MODESTENGINE_QTM_PLUGIN_NAME,
                                                  MODESTENGINE_QTM_PLUGIN_PATH,
                                                  MODESTENGINE_QTM_PLUGIN_NAME,
                                                  QDBusConnection::sessionBus(),
                                                  this);

    m_QtmPluginDBusInterface->connection().connect(MODESTENGINE_QTM_PLUGIN_NAME,
                                                   MODESTENGINE_QTM_PLUGIN_PATH,
                                                   MODESTENGINE_QTM_PLUGIN_NAME,
                                                   "HeadersReceived",
                                                   this, SLOT(searchMessagesHeadersReceivedSlot(QDBusMessage)));

    m_QtmPluginDBusInterface->connection().connect(MODESTENGINE_QTM_PLUGIN_NAME,
                                                   MODESTENGINE_QTM_PLUGIN_PATH,
                                                   MODESTENGINE_QTM_PLUGIN_NAME,
                                                   "HeadersFetched",
                                                   this, SLOT(searchMessagesHeadersFetchedSlot(QDBusMessage)));

    qDBusRegisterMetaType< ModestStringMap >();
    qDBusRegisterMetaType< ModestStringMapList >();

    qRegisterMetaType<INotifyWatcher::FileNotification>();

    qRegisterMetaType<ModestUnreadMessageDBusStruct>();
    qRegisterMetaType<ModestAccountsUnreadMessagesDBusStruct>();
    qDBusRegisterMetaType<ModestMessage>();

    watchAllKnownEmailFolders();

    /*foreach (QMessageAccount value, iAccounts) {
qWarning() << "**** Account ****";
qWarning() << "                 id : " << value.id().toString();
qWarning() << "               name : " << value.name();
qWarning() << "       messageTypes : " << value.messageTypes();
    }*/

    /*bool isFiltered;
    bool isSorted;
    QMessageFolderIdList ids = this->queryFolders(QMessageFolderFilter(), QMessageFolderSortOrder(), 0, 0, isFiltered, isSorted);
    for (int i=0; i < ids.count(); i++) {
qWarning() << "**** Folder ****";
qWarning() << "                 id : " << this->folder(ids[i]).id().toString();
qWarning() << "               name : " << this->folder(ids[i]).name();
qWarning() << "    parentAccountId : " << this->folder(ids[i]).parentAccountId().toString();
qWarning() << "     parentFolderId : " << this->folder(ids[i]).parentFolderId().toString();
qWarning() << "               path : " << this->folder(ids[i]).path();
    }*/
}

ModestEngine::~ModestEngine()
{
    g_object_unref(m_gconfclient);
    m_gconfclient=NULL;
}

ModestEngine* ModestEngine::instance()
{
    return modestEngine();
}

void ModestEngine::updateEmailAccounts() const
{
    iDefaultEmailAccountId = QMessageAccountId();
    iAccounts.clear();

    GSList *accounts;
    GError *error = NULL;

    accounts = gconf_client_all_dirs(m_gconfclient, MODESTENGINE_ACCOUNT_NAMESPACE, &error);
    if (error) {
#ifdef MESSAGING_DEBUG
        qDebug("qtmessaging: failed to list '%s': '%s'", MODESTENGINE_ACCOUNT_NAMESPACE, error->message);
#endif
        g_error_free(error);
    } else {
        gchar *default_account_id = gconf_client_get_string(m_gconfclient, MODESTENGINE_DEFAULT_ACCOUNT, &error);
        if (error) {
            qWarning("qtmessaging: failed to get '%s': %s", MODESTENGINE_DEFAULT_ACCOUNT, error->message);
            g_error_free(error);
        }

        const size_t prefix_len = strlen(MODESTENGINE_ACCOUNT_NAMESPACE) + 1;

        GSList *iter = accounts;
        while (iter) {
            if (!(iter->data)) {
                iter = iter->next;
                continue;
            }

            const gchar* account_key = (const gchar*)iter->data;
            // account_key = /apps/modest/server_accounts/<account id>
            // => take account id from account_key & unescape account id
            gchar* unescaped_account_id = gconf_unescape_key(account_key+prefix_len, strlen(account_key)-prefix_len);

            gboolean account_ok = FALSE;
            // Check if account is enabled
            if (account_key) {
                gchar* key = g_strconcat(account_key, "/", MODESTENGINE_ACCOUNT_ENABLED, NULL);
                account_ok = gconf_client_get_bool(m_gconfclient, key, NULL);
                g_free(key);
            }

            // Check if account store is defined
            if (account_ok) {
                gchar* key = g_strconcat(account_key, "/", MODESTENGINE_ACCOUNT_STORE_ACCOUNT, NULL);
                gchar* server_account_name = gconf_client_get_string(m_gconfclient, key, NULL);
                if (server_account_name) {
                    gchar* escaped_server_account_name = gconf_escape_key(server_account_name, strlen(server_account_name));
                    g_free(server_account_name);
                    gchar* store_account_key = g_strconcat(MODESTENGINE_SERVER_ACCOUNT_NAMESPACE, "/", escaped_server_account_name, NULL);
                    if (!gconf_client_dir_exists(m_gconfclient, store_account_key, NULL)) {
                        account_ok = FALSE;
                    }
                    g_free(store_account_key);
                    g_free(escaped_server_account_name);
                }
                g_free(key);
            }

            // Check if account transport is defined
            if (account_ok) {
                gchar* key = g_strconcat(account_key, "/", MODESTENGINE_ACCOUNT_TRANSPORT_ACCOUNT, NULL);
                gchar* server_account_name = gconf_client_get_string(m_gconfclient, key, NULL);
                if (server_account_name) {
                    gchar* escaped_server_account_name = gconf_escape_key(server_account_name, strlen(server_account_name));
                    g_free(server_account_name);
                    gchar* transport_account_key = g_strconcat(MODESTENGINE_SERVER_ACCOUNT_NAMESPACE, "/", escaped_server_account_name, NULL);
                    if (!gconf_client_dir_exists(m_gconfclient, transport_account_key, NULL)) {
                        account_ok = FALSE;
                    }
                    g_free(transport_account_key);
                    g_free(escaped_server_account_name);
                }
                g_free(key);
            }

            if (account_ok) {
                gchar* escaped_account_id = gconf_escape_key(unescaped_account_id, strlen(unescaped_account_id));
                QString accountId = "MO_"+QString::fromUtf8(escaped_account_id);
                g_free(escaped_account_id);

                gchar* name_key = g_strconcat(account_key, "/", MODESTENGINE_ACCOUNT_DISPLAY_NAME, NULL);
                gchar* account_name = gconf_client_get_string(m_gconfclient, name_key, NULL);
                QString accountName = QString::fromUtf8(account_name);
                g_free(account_name);
                g_free(name_key);

                gchar* email_key = g_strconcat(account_key, "/", MODESTENGINE_ACCOUNT_EMAIL, NULL);
                gchar* email = gconf_client_get_string(m_gconfclient, email_key, NULL);
                QString accountAddress = QString::fromUtf8(email);
                g_free(email);
                g_free(email_key);

                QMessageAccount account = QMessageAccountPrivate::from(QMessageAccountId(accountId),
                                                                       accountName,
                                                                       QMessageAddress(QMessageAddress::Email, accountAddress),
                                                                       QMessage::Email);
                iAccounts.insert(accountId, account);

                // Check if newly added account is default account
                if (!strncmp(default_account_id, unescaped_account_id, strlen(default_account_id))) {
                    iDefaultEmailAccountId = accountId;
                }
            }

            g_free(unescaped_account_id);
            g_free(iter->data);
            iter->data = NULL;
            iter = g_slist_next(iter);
        }
        // strings were freed in while loop
        // => it's enough to just free accounts list
        g_slist_free(accounts);
        g_free(default_account_id);
    }
}

QMessageAccountIdList ModestEngine::queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountSortOrder &sortOrder,
                                                  uint limit, uint offset, bool &isFiltered, bool &isSorted) const
{
    QMessageAccountIdList accountIds;

    updateEmailAccounts();
    foreach (QMessageAccount value, iAccounts) {
        accountIds.append(value.id());
    }

    MessagingHelper::filterAccounts(accountIds, filter);
    isFiltered = true;

    isSorted = false;

    return accountIds;
}

int ModestEngine::countAccounts(const QMessageAccountFilter &filter) const
{
    bool isFiltered, isSorted;
    return queryAccounts(filter, QMessageAccountSortOrder(), 0, 0, isFiltered, isSorted).count();
}

QMessageAccount ModestEngine::account(const QMessageAccountId &id) const
{
    updateEmailAccounts();
    return iAccounts[id.toString()];
}

QMessageAccountId ModestEngine::defaultAccount() const
{
    updateEmailAccounts();
    return iDefaultEmailAccountId;
}

QFileInfoList ModestEngine::localFolders() const
{
    QDir dir(localRootFolder());
    dir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
    QFileInfoList fileInfoList = dir.entryInfoList();
    appendLocalSubFolders(fileInfoList, 0);
    return fileInfoList;
}

void ModestEngine::appendLocalSubFolders(QFileInfoList& fileInfoList, int startIndex) const
{
    int endIndex = fileInfoList.count();
    for (int i=startIndex; i < endIndex; i++) {
        QDir dir(fileInfoList[i].absoluteFilePath());
        if (dir.exists()) {
            dir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
            QFileInfoList dirs = dir.entryInfoList();
            for (int j = 0; j < dirs.count(); j++) {
                QString fileName = dirs[j].fileName();
                if (fileName != "cur" && fileName != "new" && fileName != "tmp") {
                    fileInfoList.append(dirs[j]);
                }
            }
        }
    }
    if (fileInfoList.count() > endIndex) {
        appendLocalSubFolders(fileInfoList, endIndex);
    }
}

void ModestEngine::appendIMAPSubFolders(QFileInfoList& fileInfoList, int startIndex) const
{
    int endIndex = fileInfoList.count();
    for (int i=startIndex; i < endIndex; i++) {
        QDir dir(fileInfoList[i].absoluteFilePath()+QString("/subfolders"));
        if (dir.exists()) {
            dir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
            fileInfoList.append(dir.entryInfoList());
        }
    }
    if (fileInfoList.count() > endIndex) {
        appendIMAPSubFolders(fileInfoList, endIndex);
    }
}

QString ModestEngine::localRootFolder() const
{
    return QDir::home().absolutePath()+QString("/.modest/local_folders");
}

QString ModestEngine::accountRootFolder(QMessageAccountId& accountId) const
{
    QString modestAccountId = modestAccountIdFromAccountId(accountId);

    QString userName;
    QString hostName;
    QString port;
    QString protocol;

    gchar* store_account_key = g_strconcat(MODESTENGINE_ACCOUNT_NAMESPACE, "/", modestAccountId.toUtf8().data(), "/", MODESTENGINE_ACCOUNT_STORE_ACCOUNT, NULL);
    gchar* store_account_name = gconf_client_get_string(m_gconfclient, store_account_key, NULL);
    g_free(store_account_key);
    gchar* escaped_store_account_name = gconf_escape_key(store_account_name, strlen(store_account_name));
    g_free(store_account_name);

    gchar* username_key = g_strconcat(MODESTENGINE_SERVER_ACCOUNT_NAMESPACE, "/", escaped_store_account_name, "/", MODESTENGINE_ACCOUNT_USERNAME, NULL);
    gchar* account_username = gconf_client_get_string(m_gconfclient, username_key, NULL);
    userName = QString::fromUtf8(account_username);
    g_free(account_username);
    g_free(username_key);

    gchar* hostname_key = g_strconcat(MODESTENGINE_SERVER_ACCOUNT_NAMESPACE, "/", escaped_store_account_name, "/", MODESTENGINE_ACCOUNT_HOSTNAME, NULL);
    gchar* account_hostname = gconf_client_get_string(m_gconfclient, hostname_key, NULL);
    hostName = QString::fromUtf8(account_hostname);
    g_free(account_hostname);
    g_free(hostname_key);

    gchar* port_key = g_strconcat(MODESTENGINE_SERVER_ACCOUNT_NAMESPACE, "/", escaped_store_account_name, "/", MODESTENGINE_ACCOUNT_PORT, NULL);
    gint account_port = gconf_client_get_int(m_gconfclient, port_key, NULL);
    port = QString::number(account_port);
    g_free(port_key);

    gchar* protocol_key = g_strconcat(MODESTENGINE_SERVER_ACCOUNT_NAMESPACE, "/", escaped_store_account_name, "/", MODESTENGINE_ACCOUNT_PROTOCOL, NULL);
    gchar* account_protocol = gconf_client_get_string(m_gconfclient, protocol_key, NULL);
    protocol = QString::fromUtf8(account_protocol);
    g_free(account_protocol);
    g_free(protocol_key);

    g_free(escaped_store_account_name);

    if (protocol == "pop") {
        return QDir::home().absolutePath()+"/.modest/cache/mail/"+protocol+"/"+userName+"__"+hostName+"_"+port;
    } else if (protocol == "imap") {
        return QDir::home().absolutePath()+"/.modest/cache/mail/"+protocol+"/"+userName+"__"+hostName+"_"+port+"/folders";
    }
    return QString();
}

QFileInfoList ModestEngine::accountFolders(QMessageAccountId& accountId) const
{
    QFileInfoList fileInfoList;

    EmailProtocol protocol = accountEmailProtocol(accountId);

    if (protocol == ModestEngine::EmailProtocolPop3) {
        QFileInfo fileInfo = QFileInfo(accountRootFolder(accountId)+"/cache");
        fileInfoList.append(fileInfo);
    } else if (protocol == ModestEngine::EmailProtocolIMAP) {
        QDir dir(accountRootFolder(accountId));
        dir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
        fileInfoList = dir.entryInfoList();
        appendIMAPSubFolders(fileInfoList, 0);
    }

    return fileInfoList;
}

ModestEngine::EmailProtocol ModestEngine::accountEmailProtocol(QMessageAccountId& accountId) const
{
    EmailProtocol protocol = EmailProtocolUnknown;

    QString modestAccountId = modestAccountIdFromAccountId(accountId);

    gchar* store_account_key = g_strconcat(MODESTENGINE_ACCOUNT_NAMESPACE, "/", modestAccountId.toUtf8().data(), "/", MODESTENGINE_ACCOUNT_STORE_ACCOUNT, NULL);
    gchar* store_account_name = gconf_client_get_string(m_gconfclient, store_account_key, NULL);
    g_free(store_account_key);
    gchar* escaped_store_account_name = gconf_escape_key(store_account_name, strlen(store_account_name));
    g_free(store_account_name);

    gchar* protocol_key = g_strconcat(MODESTENGINE_SERVER_ACCOUNT_NAMESPACE, "/", escaped_store_account_name, "/", MODESTENGINE_ACCOUNT_PROTOCOL, NULL);
    gchar* account_protocol = gconf_client_get_string(m_gconfclient, protocol_key, NULL);
    if (QString("pop") == account_protocol) {
        protocol = EmailProtocolPop3;
    } else if (QString("imap") == account_protocol) {
        protocol = EmailProtocolIMAP;
    }
    g_free(account_protocol);
    g_free(protocol_key);

    g_free(escaped_store_account_name);

    return protocol;
}

QString ModestEngine::accountEmailProtocolAsString(const QMessageAccountId& accountId) const
{
    QString protocol;

    QString modestAccountId = modestAccountIdFromAccountId(accountId);

    gchar* store_account_key = g_strconcat(MODESTENGINE_ACCOUNT_NAMESPACE, "/", modestAccountId.toUtf8().data(), "/", MODESTENGINE_ACCOUNT_STORE_ACCOUNT, NULL);
    gchar* store_account_name = gconf_client_get_string(m_gconfclient, store_account_key, NULL);
    g_free(store_account_key);
    gchar* escaped_store_account_name = gconf_escape_key(store_account_name, strlen(store_account_name));
    g_free(store_account_name);

    gchar* protocol_key = g_strconcat(MODESTENGINE_SERVER_ACCOUNT_NAMESPACE, "/", escaped_store_account_name, "/", MODESTENGINE_ACCOUNT_PROTOCOL, NULL);
    gchar* account_protocol = gconf_client_get_string(m_gconfclient, protocol_key, NULL);
    protocol = QString::fromUtf8(account_protocol);
    g_free(account_protocol);
    g_free(protocol_key);

    g_free(escaped_store_account_name);

    return protocol;
}

QString ModestEngine::accountUsername(QMessageAccountId& accountId) const
{
    QString username;

    QString modestAccountId = modestAccountIdFromAccountId(accountId);

    gchar* store_account_key = g_strconcat(MODESTENGINE_ACCOUNT_NAMESPACE, "/", modestAccountId.toUtf8().data(), "/", MODESTENGINE_ACCOUNT_STORE_ACCOUNT, NULL);
    gchar* store_account_name = gconf_client_get_string(m_gconfclient, store_account_key, NULL);
    g_free(store_account_key);
    gchar* escaped_store_account_name = gconf_escape_key(store_account_name, strlen(store_account_name));
    g_free(store_account_name);

    gchar* username_key = g_strconcat(MODESTENGINE_SERVER_ACCOUNT_NAMESPACE, "/", escaped_store_account_name, "/", MODESTENGINE_ACCOUNT_USERNAME, NULL);
    gchar* account_username = gconf_client_get_string(m_gconfclient, username_key, NULL);
    username = QString::fromUtf8(account_username);
    g_free(account_username);
    g_free(username_key);

    g_free(escaped_store_account_name);

    return username;
}

QString ModestEngine::accountHostname(QMessageAccountId& accountId) const
{
    QString host;

    QString modestAccountId = modestAccountIdFromAccountId(accountId);

    gchar* store_account_key = g_strconcat(MODESTENGINE_ACCOUNT_NAMESPACE, "/", modestAccountId.toUtf8().data(), "/", MODESTENGINE_ACCOUNT_STORE_ACCOUNT, NULL);
    gchar* store_account_name = gconf_client_get_string(m_gconfclient, store_account_key, NULL);
    g_free(store_account_key);
    gchar* escaped_store_account_name = gconf_escape_key(store_account_name, strlen(store_account_name));
    g_free(store_account_name);

    gchar* host_key = g_strconcat(MODESTENGINE_SERVER_ACCOUNT_NAMESPACE, "/", escaped_store_account_name, "/", MODESTENGINE_ACCOUNT_HOSTNAME, NULL);
    gchar* account_host = gconf_client_get_string(m_gconfclient, host_key, NULL);
    host = QString::fromUtf8(account_host);
    g_free(account_host);
    g_free(host_key);

    g_free(escaped_store_account_name);

    return host;
}

QMessageFolderIdList ModestEngine::queryFolders(const QMessageFolderFilter &filter, const QMessageFolderSortOrder &sortOrder,
                                                uint limit, uint offset, bool &isFiltered, bool &isSorted) const
{
    QMessageFolderIdList folderIds;

    //QDBusMessage msg = m_ModestDBusInterface->call(MODEST_DBUS_METHOD_GET_FOLDERS);
    QFileInfoList localFolders = this->localFolders();
    QString localRootFolder = this->localRootFolder();

    foreach (QMessageAccount value, iAccounts) {
        QMessageAccountId accountId = value.id();
        QString rootFolder = accountRootFolder(accountId);
        QFileInfoList folders = this->accountFolders(accountId);

        for (int i=0; i < folders.count(); i++) {
            QString filePath = folders[i].absoluteFilePath();
            QString id = accountId.toString()+"&"+accountEmailProtocolAsString(accountId)+"&"+filePath.right(filePath.size()-rootFolder.size()-1);
            id = id.remove("/subfolders");
            folderIds.append(QMessageFolderId(id));
        }

        // Each account sees local folders as account folders
        for (int i=0; i < localFolders.count(); i++) {
            QString filePath = localFolders[i].absoluteFilePath();
            QString id = accountId.toString()+"&"+"maildir"+"&"+filePath.right(filePath.size()-localRootFolder.size()-1);
            folderIds.append(QMessageFolderId(id));
        }
    }

    // Local folders are listed for account with id : "LOCAL"
    for (int i=0; i < localFolders.count(); i++) {
        QString filePath = localFolders[i].absoluteFilePath();
        QString id = QString("LOCAL&")+"maildir"+"&"+filePath.right(filePath.size()-localRootFolder.size()-1);
        folderIds.append(QMessageFolderId(id));
    }

    MessagingHelper::filterFolders(folderIds, filter);
    isFiltered = true;
    isSorted = false;
    return folderIds;
}

int ModestEngine::countFolders(const QMessageFolderFilter &filter) const
{
    bool isFiltered, isSorted;
    return queryFolders(filter, QMessageFolderSortOrder(), 0, 0, isFiltered, isSorted).count();
}

QMessageFolder ModestEngine::folder(const QMessageFolderId &id) const
{
    QString idString = id.toString();
    int endOfAccountId = idString.indexOf('&');
    int endOfProtocolString = idString.lastIndexOf('&');
    QString accountId = idString.left(endOfAccountId);
    QString protocolString = idString.mid(endOfAccountId+1, endOfProtocolString-endOfAccountId-1);
    QString folder = idString.right(idString.length()-idString.lastIndexOf('&')-1);
    QMessageFolderId parentId;
    QString name;
    if (folder.lastIndexOf('/') == -1) {
        // Folder does not have subfolders
        name = folder;
        if ((protocolString == "pop") && (name == "cache")) {
            name = "Inbox";
        }
    } else {
        // Folder has subfolders
        name = folder.right(folder.length()-folder.lastIndexOf('/')-1);
        parentId = idString.left(idString.lastIndexOf('/'));
    }
    return QMessageFolderPrivate::from(id, QMessageAccountId(accountId), parentId, name, folder);
}

void ModestEngine::watchAllKnownEmailFolders()
{
    QFileInfoList localFolders = this->localFolders();

    // Changes in local Email folders can be monitored using directory
    // monitoring. <=> All messages are stored as individual files.
    for (int i=0; i < localFolders.count(); i++) {
        m_MailFoldersWatcher.addDirectory(localFolders[i].absoluteFilePath()+"/cur", IN_MOVED_TO | IN_DELETE);
    }

    // Changes in remote Email folders must be monitored using file monitoring.
    // That's because message headers are stored into summary.mmap file (and summary.mmap
    // file maybe the only place that contains message information).
    // => summary.mmap files will be monitored
    foreach (QMessageAccount value, iAccounts) {
        QMessageAccountId accountId = value.id();
        QString rootFolder = accountRootFolder(accountId);
        EmailProtocol protocol = accountEmailProtocol(accountId);
        QFileInfoList folders = this->accountFolders(accountId);

        for (int i=0; i < folders.count(); i++) {
            if (protocol == ModestEngine::EmailProtocolPop3) {
                QString folder = folders[i].absoluteFilePath();
                m_MailFoldersWatcher.addDirectory(folder, IN_CREATE);
            } else if (protocol == ModestEngine::EmailProtocolIMAP) {
                m_MailFoldersWatcher.addDirectory(folders[i].absoluteFilePath(), IN_MOVED_TO | IN_DELETE);
            }
        }

    }

    connect(&m_MailFoldersWatcher, SIGNAL(fileChanged(int, const QString&, uint)),
            this, SLOT(fileChangedSlot(int, const QString&, uint)));
}

void ModestEngine::fileChangedSlot(int watchDescriptor, const QString& filePath, uint events)
{
    int filenameBegin = filePath.lastIndexOf('/')+1;
    QString fileName = filePath.mid(filenameBegin,filePath.lastIndexOf('.')-filenameBegin);
    if (fileName != "summary") {
        if (events & (IN_MOVED_TO | IN_CREATE)) {
            notification(QMessageFolderId(), messageIdFromModestMessageFilePath(filePath), ModestEngine::Added);
        } else if (events & IN_DELETE) {
            notification(QMessageFolderId(), messageIdFromModestMessageFilePath(filePath), ModestEngine::Removed);
        }
    }
}

bool ModestEngine::sendEmail(QMessage &message)
{
    QMessageAddressList addresses;
    QMessageAddress address;
    QMessageContentContainerIdList attachmentIds;
    QMessageContentContainerId bodyId;
    QMessageContentContainer body;
    QString value;
    QMessageAccountId accountId;
    QMessageAccount account;
    ModestStringMap senderInfo;
    ModestStringMap recipients;
    ModestStringMap messageData;
    ModestStringMapList attachments;
    ModestStringMapList images;
    uint priority = 0;
    ModestStringMap headers;
    QMessage::StatusFlags messageStatus;

    qDebug() << __PRETTY_FUNCTION__ << "Sending message";

    // XXX: Room for convenience function here...
    accountId = message.parentAccountId();
    if (accountId.isValid() == false) {
        qWarning () << "Account ID is invalid";
        return false;
    }

    senderInfo["account-name"] = modestAccountIdFromAccountId(accountId);

    address = message.from();
    value = address.recipient();

    if (value.isEmpty() == false && value.isNull() == false) {
        senderInfo["from"] = value;
    }

    qDebug() << "Digging \"to\" field";

    addresses = message.to();
    value.clear();
    for (int i = 0; i < addresses.length(); i++) {
        address = addresses[i];

        if (value.isEmpty()) {
            value = address.recipient();
        } else {
            value.append (",");
            value.append (address.recipient());
        }
    }

    if (value.isEmpty() == false && value.isNull() == false) {
        recipients["to"] = value;
    }

    qDebug() << "Digging \"cc\" field";

    addresses = message.cc();
    value.clear();
    for (int i = 0; i < addresses.length(); i++) {
        address = addresses[i];

        if (value.isEmpty()) {
            value = address.recipient();
        } else {
            value.append (",");
            value.append (address.recipient());
        }
    }

    if (value.isEmpty() == false && value.isNull() == false) {
        recipients["cc"] = value;
    }

    qDebug() << "Digging \"bcc\" field";

    addresses = message.bcc();
    value.clear();
    for (int i = 0; i < addresses.length(); i++) {
        address = addresses[i];

        if (value.isEmpty()) {
            value = address.recipient();
        } else {
            value.append (",");
            value.append (address.recipient());
        }
    }

    if (value.isEmpty() == false && value.isNull() == false) {
        recipients["bcc"] = value;
    }

    qDebug() << "Digging \"subject\" field";

    value = message.subject();

    if (value.isEmpty() == false && value.isNull() == false) {
        messageData["subject"] = value;
    }

    qDebug() << "Digging body content";

    bodyId = message.bodyId();
    if (bodyId.isValid()) {
        body = message.find (bodyId);
    } else {
        body = message;
    }

    value = body.contentType();
    qDebug() << value;

    if (value == "text") {
        QString key, data;
        bool hasContent = false;

        value = body.contentSubType();
        qDebug() << value;

        if ((hasContent = body.isContentAvailable()) == true) {
            data = body.textContent();
        }

        if (value == "plain") {
            key = "plain-body";
        } else if (value == "html") {
            key = "html-body";
        }

        if (key.isEmpty() == false && key.isNull() == false && hasContent) {
            messageData[key] = data;
        }
    }

    messageStatus = message.status();

    if (messageStatus & QMessage::HasAttachments) {
        attachmentIds = message.attachmentIds();

        foreach (QMessageContentContainerId identifier, attachmentIds) {
            ModestStringMap attachmentData;
            QMessageContentContainer attachmentCont;

            if (identifier.isValid() == false) continue;

            attachmentCont = message.find (identifier);

            if (attachmentCont.isContentAvailable () == false) continue;

            attachmentData.clear();

            value = attachmentCont.contentType();

            if (value.isEmpty() == false) {
                value.append("/");
                value.append (attachmentCont.contentSubType());
                attachmentData["mime-type"] = value;

                qDebug() << "mime-type: " << value;
            }

            value = QMessageContentContainerPrivate::attachmentFilename (
                attachmentCont);

            if (value.isEmpty() == false) {
                attachmentData["filename"] = value;
                qDebug() << "filename: " << value;
            }

            qDebug() << "Charset: " << attachmentCont.contentCharset();
            qDebug() << "Headers: " << attachmentCont.headerFields();

            if (attachmentData.isEmpty() == false) {
                attachmentData["content-id"] = identifier.toString();
                attachments.append (attachmentData);
            }
        }
    }

    switch (message.priority()) {
    case QMessage::HighPriority:
        priority = MODESTENGINE_HIGH_PRIORITY;
        break;

    default:
    case QMessage::NormalPriority:
        priority = MODESTENGINE_NORMAL_PRIORITY;
        break;

    case QMessage::LowPriority:
        priority = MODESTENGINE_LOW_PRIORITY;
        break;
    }

    qDebug() << "Sending D-BUS message";

    QDBusPendingCall call = m_QtmPluginDBusInterface->asyncCall (
            "SendEmail",
            QVariant::fromValue (senderInfo),
            QVariant::fromValue (recipients),
            QVariant::fromValue (messageData),
            QVariant::fromValue (attachments),
            QVariant::fromValue (images),
            priority,
            QVariant::fromValue (headers));

    qDebug() << "Message sent";

    if (call.isError()) {
        qWarning() << "Call failed! " << call.error();
        return false;
    }

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher (call, this);

    connect (watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
             this, SLOT(sendEmailCallEnded(QDBusPendingCallWatcher*)));

    return true;
}

bool ModestEngine::composeEmail(const QMessage &message)
{
    QString mailString("mailto:");
    QList<QMessageAddress> list(message.to());
    if (!list.empty()){
        for (int i = 0; i < list.size(); ++i) {
            if (i == 0) {
                mailString += list.at(i).addressee();
            } else {
                mailString += QString("%2C%20");
                mailString += list.at(i).addressee();
            }
        }
    }

    bool firstParameter = true;

    QString subject = message.subject();
    if (!subject.isEmpty()) {
        if (firstParameter) {
            mailString += QString("?");
            firstParameter = false;
        } else {
            mailString += QString("&");
        }
        mailString += QString("subject=");
        mailString += subject;
    }

    QString body = message.textContent();
    if (!body.isEmpty()) {
        if (firstParameter) {
            mailString += QString("?");
            firstParameter = false;
        } else {
            mailString += QString("&");
        }
        mailString += QString("body=");
        mailString += QString(body);
    }

    QUrl url(mailString);

    QDBusInterface dbusif(MODEST_DBUS_SERVICE, MODEST_DBUS_OBJECT, MODEST_DBUS_IFACE);
    QDBusMessage msg = dbusif.call(MODEST_DBUS_METHOD_MAIL_TO, url.toString());

    return true;
}

QMessage ModestEngine::message(const QMessageId &id) const
{
    return QMessage();
}

void ModestEngine::filterMessages(QMessageIdList messageIds, QMessageFilterPrivate::SortedMessageFilterList filterList, int start) const
{
    if (filterList.count() > start) {
        for (int i=0; i < messageIds.count(); i++) {
            QMessage msg = message(messageIds[i]);
            for (int j=start; j < filterList.count(); j++) {
                QMessageFilterPrivate* pf = QMessageFilterPrivate::implementation(filterList[j]);
                if (!pf->filter(msg)) {
                    messageIds.removeAt(i);
                    i--;
                    break;
                }
            }
        }
    }
}

bool ModestEngine::queryMessages(QMessageService& messageService, const QMessageFilter &filter, const QString &body,
                                 QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder,
                                 uint limit, uint offset) const
{
    MessageQueryInfo queryInfo;
    queryInfo.queryId = ++m_queryIds;
    if (queryInfo.queryId == 100000) {
        m_queryIds = 1;
        queryInfo.queryId = 1;
    }
    queryInfo.body = body;
    queryInfo.matchFlags = matchFlags;
    queryInfo.filter = filter;
    queryInfo.sortOrder = sortOrder;
    queryInfo.limit = limit;
    queryInfo.offset = offset;
    queryInfo.messageService = &messageService;
    queryInfo.currentFilterListIndex = 0;
    queryInfo.handledFiltersCount = 0;
    m_pendingMessageQueries.append(queryInfo);

    if (!queryAndFilterMessages(m_pendingMessageQueries[m_pendingMessageQueries.count()-1])) {
        QMessageServicePrivate::implementation(messageService)->setFinished(false);
        m_pendingMessageQueries.removeAt(m_pendingMessageQueries.count()-1);
        return false;
    }

    QTimer::singleShot(1000, (ModestEngine*)this, SLOT(tempSlot()));

    return true;
}

void ModestEngine::tempSlot()
{
    searchMessagesHeadersFetchedSlot(QDBusMessage());
}

void ModestEngine::queryAndFilterMessagesReady(int queryId, QMessageIdList ids) const
{
    int index = -1;
    for (int i=0; i < m_pendingMessageQueries.count(); i++) {
        if (m_pendingMessageQueries[i].queryId == queryId) {
            index = i;
            break;
        }
    }

    MessageQueryInfo &queryInfo = m_pendingMessageQueries[index];

    QMessageFilterPrivate* pf = QMessageFilterPrivate::implementation(queryInfo.filter);
    if (pf->_filterList.count() == 0) {
        if (queryInfo.handledFiltersCount == 0) {
            QMessageFilterPrivate::SortedMessageFilterList filters;
            filters.append(queryInfo.filter);
            filterMessages(ids, filters, 0);
        }
        QMessageServicePrivate *privateService = QMessageServicePrivate::implementation(*queryInfo.messageService);
        privateService->messagesFound(ids);
        privateService->setFinished(true);
        m_pendingMessageQueries.removeAt(index);
    } else {
        if (queryInfo.handledFiltersCount < pf->_filterList[queryInfo.currentFilterListIndex].count()) {
            filterMessages(ids, pf->_filterList[queryInfo.currentFilterListIndex], queryInfo.handledFiltersCount);
        }

        // Append new ids to resultset
        for (int i=0; i < ids.count(); i++) {
            if (!queryInfo.ids.contains(ids[i])) {
                queryInfo.ids.append(ids[i]);
            }
        }

        queryInfo.currentFilterListIndex++;
        if (queryInfo.currentFilterListIndex < pf->_filterList.count()) {
            if (!queryAndFilterMessages(queryInfo)) {
                QMessageServicePrivate::implementation(*queryInfo.messageService)->setFinished(false);
                m_pendingMessageQueries.removeAt(index);
            }
        } else {
            QMessageServicePrivate *privateService = QMessageServicePrivate::implementation(*queryInfo.messageService);
            privateService->messagesFound(ids);
            privateService->setFinished(true);
            m_pendingMessageQueries.removeAt(index);
        }
    }

}

bool ModestEngine::queryAndFilterMessages(MessageQueryInfo &msgQueryInfo) const
{
    QStringList accountIds;
    QStringList folderUris;
    QDateTime startDate;
    QDateTime endDate;

    QMessageFilterPrivate::SortedMessageFilterList filters;
    QMessageFilterPrivate* pf = QMessageFilterPrivate::implementation(msgQueryInfo.filter);
    if (pf->_filterList.count() == 0) {
        // One single filter to be handled
        filters.append(msgQueryInfo.filter);
    } else {
        // Multiple filters to be handled
        // Filters have been sorted into matrix of filters:
        // - there is AND operation between every filter in one row
        // - there is OR operation between every row
        // => Every row can be handled individually
        // => Final result set can be created by combining result sets
        //    from individual row based queries
        filters = pf->_filterList[msgQueryInfo.currentFilterListIndex];
    }

    int handledFiltersCount = 0;
    pf = QMessageFilterPrivate::implementation(filters[handledFiltersCount]);
    if ((filters.count() == 1) &&
        (pf->_field == QMessageFilterPrivate::None) &&
        (pf->_filterList.count() == 0)) {
        if (pf->_notFilter) {
            // There is only one filter: empty ~QMessageFilter()
            // => return empty QMessageIdList
            queryAndFilterMessagesReady(msgQueryInfo.queryId, QMessageIdList());
            return true;
        } else {
            // There is only one filter: empty QMessageFilter()
            // => return all messages
            handledFiltersCount++;
        }
    }

    bool handled = true;
    while (handledFiltersCount < filters.count() && handled) {
        handled = false;
        pf = QMessageFilterPrivate::implementation(filters[handledFiltersCount]);
        switch (pf->_field) {
        case QMessageFilterPrivate::Id:
            break;
        case QMessageFilterPrivate::ParentAccountId:
            {
            if (pf->_comparatorType == QMessageFilterPrivate::Equality) {
                QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Equal) {
                    if (pf->_value.toString().length() > 0) {
                        accountIds.append(modestAccountIdFromAccountId(pf->_value.toString()));
                        handled = true;
                    }
                }
            }
            break;
        }
        case QMessageFilterPrivate::ParentFolderId:
            {
            if (pf->_comparatorType == QMessageFilterPrivate::Equality) {
                QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Equal) {
                    if (pf->_value.toString().length() > 0) {
                        folderUris.append(modestFolderIdFromFolderId(pf->_value.toString()));
                        handled = true;
                    }
                }
            }
            break;
            }
        case QMessageFilterPrivate::AncestorFolderIds:
            break;
        case QMessageFilterPrivate::Type:
            break;
        case QMessageFilterPrivate::StandardFolder:
            break;
        case QMessageFilterPrivate::ParentAccountIdFilter:
            break;
        case QMessageFilterPrivate::ParentFolderIdFilter:
            break;
        case QMessageFilterPrivate::TimeStamp:
            {
            if (pf->_comparatorType == QMessageFilterPrivate::Equality) {
                QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Equal) {
                    endDate = pf->_value.toDateTime();
                    startDate = pf->_value.toDateTime();
                    handled = true;
                }
            }
            if (pf->_comparatorType == QMessageFilterPrivate::Relation) {
                QMessageDataComparator::RelationComparator cmp(static_cast<QMessageDataComparator::RelationComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::LessThan) {
                    endDate = pf->_value.toDateTime();
                } else if (cmp == QMessageDataComparator::LessThanEqual) {
                    endDate = pf->_value.toDateTime();
                    handled = true;
                } if (cmp == QMessageDataComparator::GreaterThanEqual) {
                    startDate = pf->_value.toDateTime();
                    handled = true;
                }
            }
            break;
            }
        case QMessageFilterPrivate::ReceptionTimeStamp:
            {
            if (pf->_comparatorType == QMessageFilterPrivate::Equality) {
                QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Equal) {
                    endDate = pf->_value.toDateTime();
                    startDate = pf->_value.toDateTime();
                    handled = true;
                }
            }
            if (pf->_comparatorType == QMessageFilterPrivate::Relation) {
                QMessageDataComparator::RelationComparator cmp(static_cast<QMessageDataComparator::RelationComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::LessThan) {
                    endDate = pf->_value.toDateTime();
                } else if (cmp == QMessageDataComparator::LessThanEqual) {
                    endDate = pf->_value.toDateTime();
                    handled = true;
                } if (cmp == QMessageDataComparator::GreaterThanEqual) {
                    startDate = pf->_value.toDateTime();
                    handled = true;
                }
            }
            break;
            }
        case QMessageFilterPrivate::Sender:
            break;
        case QMessageFilterPrivate::Recipients:
            break;
        case QMessageFilterPrivate::Subject:
            break;
        case QMessageFilterPrivate::Status:
            break;
        case QMessageFilterPrivate::Priority:
            break;
        case QMessageFilterPrivate::Size:
            break;
        case QMessageFilterPrivate::None:
            break;
        }
        handledFiltersCount++;
    }

    msgQueryInfo.handledFiltersCount = handledFiltersCount;

    return searchMessages(msgQueryInfo, accountIds, folderUris, startDate, endDate);
}

bool ModestEngine::searchMessages(MessageQueryInfo &msgQueryInfo, const QStringList& accountIds,
                                  const QStringList& folderUris, const QDateTime& startDate,
                                  const QDateTime& endDate) const
{
    qulonglong sDate = 0;
    if (startDate.isValid()) {
        sDate = startDate.toTime_t();
    }
    qulonglong eDate = 0;
    if (endDate.isValid()) {
        eDate = endDate.toTime_t();
    }

qWarning() << "accountIds : " << accountIds;
qWarning() << "folderUris : " << folderUris;
qWarning() << "sDate : " << sDate;
qWarning() << "eDate : " << eDate;

    QDBusMessage reply = m_QtmPluginDBusInterface->call("GetHeaders",
                                                        accountIds,
                                                        folderUris,
                                                        sDate,
                                                        eDate,
                                                        false);

qWarning() << "reply : " << reply;
    return true;
}

void ModestEngine::searchMessagesHeadersReceivedSlot(QDBusMessage msg)
{
    qWarning() << "HeadersReceived";
    qWarning() << msg;
}

void ModestEngine::searchMessagesHeadersFetchedSlot(QDBusMessage msg)
{
    qWarning() << "HeadersFetched";
    qWarning() << msg;
    if (m_pendingMessageQueries.count()) {
        int index = 0;
        QMessageServicePrivate* pMsgService = QMessageServicePrivate::implementation(*m_pendingMessageQueries[index].messageService);
        pMsgService->messagesFound(m_pendingMessageQueries[index].ids);
        pMsgService->setFinished(true);
        m_pendingMessageQueries.removeAt(index);
    }
}

bool ModestEngine::queryMessages(QMessageService& messageService, const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    return queryMessages(messageService, filter, QString(), 0, sortOrder, limit, offset);
}

bool ModestEngine::countMessages(QMessageService& messageService, const QMessageFilter &filter)
{
    return false;
}

void
ModestEngine::sendEmailCallEnded(QDBusPendingCallWatcher *watcher)
{
    if (watcher->isError ()) {
        // TODO: Emit a failure
        qWarning() << "Failed to send email via modest: " << watcher->error();
    } else {
        // TODO: Emit a success (or put to outbox)
        qDebug() << "Message should be outboxed now...";
    }
}

void ModestEngine::folderUpdatedSlot(QDBusMessage msg)
{
    QList<QVariant> arguments = msg.arguments();
    QMessageAccountId accountId = QMessageAccountId("MO_"+escapeString(arguments.takeFirst().toString()));
    QString folder = arguments.takeFirst().toString();
    QString protocol = accountEmailProtocolAsString(accountId);
    if ((protocol == "pop") && (folder == "INBOX")) {
        folder = "cache";
    }
    QMessageFolderId updatedFolderId = QMessageFolderId(accountId.toString()+"&"+protocol+"&"+folder);

    int messagesPerAccount = 5;
    QDBusPendingCall pendingCall = m_ModestDBusInterface->asyncCall(MODEST_DBUS_METHOD_GET_UNREAD_MESSAGES,
                                                                    messagesPerAccount);
    QDBusPendingCallWatcher* pendingCallWatcher = new QDBusPendingCallWatcher(pendingCall);
    pendingCallWatcher->setProperty("folderId", updatedFolderId.toString());
    connect(pendingCallWatcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
            this, SLOT(pendingGetUnreadMessagesFinishedSlot(QDBusPendingCallWatcher*)));
}

void ModestEngine::pendingGetUnreadMessagesFinishedSlot(QDBusPendingCallWatcher* pendingCallWatcher)
{
    QDBusMessage msg = pendingCallWatcher->reply();
    QVariant variant = msg.arguments().takeFirst();
    QDBusArgument argument = variant.value<QDBusArgument>();
    QList<ModestAccountsUnreadMessagesDBusStruct> accountsWithUnreadMessages;
    argument >> accountsWithUnreadMessages;

    QMessageFolderId folderId = QMessageFolderId(pendingCallWatcher->property("folderId").toString());
    QDateTime now = QDateTime::currentDateTime();

    for (int i=0; i < accountsWithUnreadMessages.count(); i++) {
        for (int j=0; j < accountsWithUnreadMessages[i].unreadMessages.count(); j++) {
            ModestUnreadMessageDBusStruct unreadMessage = accountsWithUnreadMessages[i].unreadMessages[j];
            QDateTime time = QDateTime::fromTime_t(unreadMessage.timeStamp);
            if (time.secsTo(now) < 60) {
                searchNewMessages(unreadMessage.subject, QString(), time, time, MODEST_DBUS_SEARCH_SUBJECT, 0);
            }
        }
    }
}

void ModestEngine::searchNewMessages(const QString& searchString, const QString& folderToSearch,
                                  const QDateTime& startDate, const QDateTime& endDate,
                                  int searchflags, uint minimumMessageSize) const
{
    qlonglong sDate = 0;
    if (startDate.isValid()) {
        sDate = startDate.toTime_t();
    }
    qlonglong eDate = 0;
    if (endDate.isValid()) {
        eDate = endDate.toTime_t();
    }

    QDBusInterface interface(MODEST_DBUS_SERVICE, MODEST_DBUS_OBJECT, MODEST_DBUS_IFACE);
    QDBusPendingCall pendingCall = interface.asyncCall(MODEST_DBUS_METHOD_SEARCH,
                                                       searchString,
                                                       folderToSearch,
                                                       sDate,
                                                       eDate,
                                                       searchflags,
                                                       minimumMessageSize);
    QDBusPendingCallWatcher* pendingCallWatcher = new QDBusPendingCallWatcher(pendingCall);
    connect(pendingCallWatcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
            this, SLOT(pendingSearchFinishedSlot(QDBusPendingCallWatcher*)));
}

void ModestEngine::pendingSearchFinishedSlot(QDBusPendingCallWatcher* pendingCallWatcher)
{
    QDBusMessage msg = pendingCallWatcher->reply();
    QVariant variant = msg.arguments().takeFirst();
    QDBusArgument argument = variant.value<QDBusArgument>();
    QList<ModestMessage> messages;
    argument >> messages;

    for (int i=0; i < messages.count(); i++) {
        notification(QMessageFolderId(), messageIdFromModestMessageId(messages[i].id), ModestEngine::Added);
    }
}

void ModestEngine::messageReadChangedSlot(QDBusMessage msg)
{
    QString changedMessageId = msg.arguments().takeFirst().toString();
    notification(QMessageFolderId(), messageIdFromModestMessageId(changedMessageId), ModestEngine::Updated);
}

void ModestEngine::notification(const QMessageFolderId& folderId, const QMessageId& messageId,
                                NotificationType notificationType) const
{
qWarning() << "!***************** messageNotification BEGIN ******************";
qWarning() << "         folderId : " << folderId.toString();
qWarning() << "        messageId : " << messageId.toString();
if (notificationType == ModestEngine::Added) {
    qWarning() << "                    Added";
} else if (notificationType == ModestEngine::Updated) {
    qWarning() << "                    Updated";
} else if (notificationType == ModestEngine::Removed) {
    qWarning() << "                    Removed";
}
qWarning() << "!***************** messageNotification END ********************";
}

QMessageAccountId ModestEngine::accountIdFromModestMessageId(const QString& modestMessageId) const
{
    // Modest messageId format is following:
    //   <email protocol>://<username>@<hostname>:<port>...
    QMessageAccountId accountId;

    int usernameBegin = modestMessageId.indexOf("//")+2;
    int usernameEnd = modestMessageId.indexOf("@");
    QString username = QUrl::fromPercentEncoding(modestMessageId.mid(usernameBegin, usernameEnd-usernameBegin).toUtf8());
    int hostnameBegin = usernameEnd+1;
    int hostnameEnd = modestMessageId.indexOf(':',hostnameBegin);
    QString hostname = QUrl::fromPercentEncoding(modestMessageId.mid(hostnameBegin, hostnameEnd-hostnameBegin).toUtf8());

    foreach (QMessageAccount value, iAccounts) {
        QMessageAccountId accId = value.id();
        if ((accountUsername(accId) == username) &&
            (accountHostname(accId) == hostname)) {
            accountId = accId;
        }
    }

    if (!accountId.isValid()) {
        if (modestMessageId.left(modestMessageId.indexOf(':')) == "maildir") {
            accountId = QMessageAccountId("LOCAL");
        }
    }

    return accountId;
}

QMessageFolderId ModestEngine::folderIdFromModestMessageId(const QString& modestMessageId,
                                                           const QMessageAccountId accountId) const
{
    // Modest messageId format is following:
    //   <email protocol>://<username>@<hostname>:<port>...
    QMessageFolderId folderId;
    QString folderIdAsString;

    if (!accountId.isValid()) {
        folderIdAsString = accountIdFromModestMessageId(modestMessageId).toString();
    } else {
        folderIdAsString = accountId.toString();
    }

    int protocolEnd = modestMessageId.indexOf(':');
    QString protocol = modestMessageId.left(protocolEnd);
    folderIdAsString += "&" + protocol;
    if (protocol == "pop") {
        folderIdAsString += "&cache";
    } else if (protocol == "imap") {
        int pathBegin = modestMessageId.indexOf('/',modestMessageId.lastIndexOf(':'))+1;
        int pathEnd = modestMessageId.lastIndexOf('/');
        folderIdAsString += "&" + modestMessageId.mid(pathBegin, pathEnd-pathBegin);
    } else if (protocol == "maildir") {
        int pathBegin = modestMessageId.indexOf('#')+1;
        int pathEnd = modestMessageId.lastIndexOf('/');
        folderIdAsString += "&" + modestMessageId.mid(pathBegin, pathEnd-pathBegin);
    }
    folderId = QMessageFolderId(QUrl::fromPercentEncoding(folderIdAsString.toUtf8()));

    return folderId;
}

QString ModestEngine::modestAccountIdFromAccountId(const QMessageAccountId& accountId) const
{
    // Just remove "MO_" prefix from the beginning of id string
    return accountId.toString().remove(0,3);
}

QString ModestEngine::modestFolderIdFromFolderId(const QMessageFolderId& folderId) const
{
    QString modestFolderId;

    QString folderIdString = folderId.toString();
    int protocolBegin = folderIdString.indexOf('&');
    int protocolEnd = folderIdString.lastIndexOf('&');

    modestFolderId = folderIdString.mid(protocolEnd+1);
    QString protocol = folderIdString.mid(protocolBegin+1,protocolEnd-protocolBegin-1);
    if ((protocol == "pop") && (modestFolderId == "cache")) {
        modestFolderId = "INBOX";
    }

    return modestFolderId;
}

QString ModestEngine::modestFolderUriFromFolderId(const QMessageFolderId& folderId) const
{
}

QString ModestEngine::modestMessageIdFromMessageId(const QMessageId& messageId) const
{
}

QMessageAccountId ModestEngine::accountIdFromModestAccountId(const QString& accountId) const
{
    // Just add "MO_" prefix to the beginning of id string
    return QMessageAccountId("MO_"+accountId);
}

QMessageFolderId ModestEngine::folderIdFromModestFolderId(const QMessageAccountId& accountId, const QString& modestFolderId) const
{
    // Format: <accountId>&<email protocol>&<Modest folderId>
    QMessageFolderId folderId;

    QString protocol = accountEmailProtocolAsString(accountId);
    if ((protocol == "pop") && (modestFolderId == "INBOX")) {
        folderId = QMessageFolderId(accountId.toString()+"&"+protocol+"&cache");
    } else {
        folderId = QMessageFolderId(accountId.toString()+"&"+protocol+"&"+modestFolderId);
    }

    return folderId;
}

QMessageId ModestEngine::messageIdFromModestMessageId(const QString& messageId) const
{
    QString messageIdString = folderIdFromModestMessageId(messageId).toString();
    int idPartBegin = messageId.lastIndexOf('/');
    messageIdString += messageId.mid(idPartBegin, idPartBegin-messageId.length());
    return QMessageId(messageIdString);
}

QMessageId ModestEngine::messageIdFromModestMessageFilePath(const QString& messageFilePath) const
{
    QString messageIdString;

    QString filePath = messageFilePath;
    QString localRootFolder = this->localRootFolder();
    if (filePath.startsWith(localRootFolder)) {
        messageIdString = "LOCAL&maildir&";
        filePath.remove(0,localRootFolder.length()+1);
        filePath.remove("/cur");
        messageIdString += filePath.left(filePath.lastIndexOf('!'));
    } else {
        foreach (QMessageAccount value, iAccounts) {
            QMessageAccountId accountId = value.id();
            QString rootFolder = accountRootFolder(accountId);
            if (filePath.startsWith(rootFolder)) {
                messageIdString = accountId.toString()+"&"+accountEmailProtocolAsString(accountId)+"&";
                filePath.remove(0,rootFolder.length()+1);
                filePath.remove("/subfolders");
                messageIdString += filePath.left(filePath.lastIndexOf('.'));
            }
        }
    }

    return QMessageId(messageIdString);
}

QString ModestEngine::unescapeString(const QString& string)
{
    QString unescapedString;

    QByteArray str = string.toUtf8();
    gchar* unescaped_string = gconf_unescape_key(str.data(), str.length());
    unescapedString = QString::fromUtf8(unescaped_string);
    g_free(unescaped_string);

    return unescapedString;
}

QString ModestEngine::escapeString(const QString& string)
{
    QString escapedString;

    QByteArray str = string.toUtf8();
    gchar* escaped_string = gconf_escape_key(str.data(), str.length());
    escapedString = QString::fromUtf8(escaped_string);
    g_free(escaped_string);

    return escapedString;
}

INotifyWatcher::INotifyWatcher()
{
    // Initialize inotify instance
    // => returned file descriptor is associated with
    //    a new inotify event queue
    // O_CLOEXEC flag makes sure that file descriptor
    //           is closed if execution is transfered
    //           from this process to a new program
    //           (Check more info from 'execve' documentation)
#ifdef IN_CLOEXEC
    m_inotifyFileDescriptor = inotify_init1(IN_CLOEXEC);
#else
    m_inotifyFileDescriptor = inotify_init();
    if (m_inotifyFileDescriptor >= 0) {
        ::fcntl(m_inotifyFileDescriptor, F_SETFD, FD_CLOEXEC);
    }
#endif
    if (m_inotifyFileDescriptor >= 0) {
        // Change thread affinity for this object to this
        // thread.
        // => Event processing (for this objects events) will
        //    be done in this thread
        moveToThread(this);
    }
}

INotifyWatcher::~INotifyWatcher()
{
    // Tell the thread's event loop to exit
    // => thread returns from the call to exec()
    exit();

    // Wait until this thread has finished execution
    // <=> waits until thread returns from run()
    wait();

    // Remove all watches from inotify instance watch list
    QMapIterator<int, QString> i(m_dirs);
    while (i.hasNext()) {
        inotify_rm_watch(m_inotifyFileDescriptor, i.next().key());
    }
    QMapIterator<int, QString> j(m_files);
    while (j.hasNext()) {
        inotify_rm_watch(m_inotifyFileDescriptor, j.next().key());
    }

    // Close file descriptor that's referring to inotify instance
    // => The underlying inotify object and its resources are freed
    ::close(m_inotifyFileDescriptor);
}

void INotifyWatcher::run()
{
    // Start listening inotify
    QSocketNotifier socketNotifier(m_inotifyFileDescriptor, QSocketNotifier::Read, this);
    connect(&socketNotifier, SIGNAL(activated(int)), SLOT(notifySlot()));

    // Enter the thread event loop
    (void) exec();
}

int INotifyWatcher::addFile(const QString& path, uint eventsToWatch)
{
    int watchDescriptor = 0;
    QMutexLocker locker(&m_mutex);

    if (m_inotifyFileDescriptor >= 0) {
        int watchDescriptor = 0;
        if (eventsToWatch == 0) {
            watchDescriptor = inotify_add_watch(m_inotifyFileDescriptor,
                                                QFile::encodeName(path),
                                                0 | IN_ATTRIB
                                                  | IN_MODIFY
                                                  | IN_MOVE
                                                  | IN_MOVE_SELF
                                                  | IN_DELETE_SELF);
        } else {
            watchDescriptor = inotify_add_watch(m_inotifyFileDescriptor,
                                                QFile::encodeName(path),
                                                eventsToWatch);
        }
        if (watchDescriptor > 0) {
            m_files.insert(watchDescriptor, path);
        } else {
            watchDescriptor = 0;
        }
    }

    // Start thread (if thread is not already running)
    start();

    return watchDescriptor;
}

int INotifyWatcher::addDirectory(const QString& path, uint eventsToWatch)
{
    int watchDescriptor = 0;
    QMutexLocker locker(&m_mutex);

    if (m_inotifyFileDescriptor >= 0) {
        int watchDescriptor = 0;
        if (eventsToWatch == 0) {
            watchDescriptor = inotify_add_watch(m_inotifyFileDescriptor,
                                                QFile::encodeName(path),
                                                0 | IN_ATTRIB
                                                  | IN_MOVE
                                                  | IN_CREATE
                                                  | IN_DELETE
                                                  | IN_DELETE_SELF);
        } else {
            watchDescriptor = inotify_add_watch(m_inotifyFileDescriptor,
                                                QFile::encodeName(path),
                                                eventsToWatch);
        }
        if (watchDescriptor > 0) {
            m_dirs.insert(watchDescriptor, path);
        } else {
            watchDescriptor = 0;
        }
    }

    // Start thread (if thread is not already running)
    start();

    return watchDescriptor;
}

QStringList INotifyWatcher::directories() const
{
    return m_dirs.values();
}

QStringList INotifyWatcher::files() const
{
    return m_dirs.values();
}

void INotifyWatcher::notifySlot()
{
    QMutexLocker locker(&m_mutex);

    int bufferSize = 0;
    ioctl(m_inotifyFileDescriptor, FIONREAD, (char*) &bufferSize);
    QVarLengthArray<char, 4096> buffer(bufferSize);
    bufferSize = read(m_inotifyFileDescriptor, buffer.data(), bufferSize);
    const char* at = buffer.data();
    const char* const end = at + bufferSize;

    QMap<int, INotifyEvent> eventsForWatchedFile;
    QMap<QString, INotifyEvent> eventsForFileInWatchedDirectory;
    while (at < end) {
        const inotify_event *event = reinterpret_cast<const inotify_event *>(at);
        if (m_files.contains(event->wd)) {
            // File event handling
            if (eventsForWatchedFile.contains(event->wd)) {
                // There is already unhandled event for this file in queue
                // => Mask is ORed to existing event
                eventsForWatchedFile[event->wd].mask |= event->mask;
            } else {
                // There is no event for this file in queue
                // => New event is created
                INotifyEvent inotifyEvent;
                inotifyEvent.watchDescriptor = event->wd;
                inotifyEvent.mask = event->mask;
                inotifyEvent.fileName = QString::fromAscii(event->name, event->len);
                eventsForWatchedFile.insert(event->wd, inotifyEvent);
            }
        } else {
            // Directory event handling
            QString changeForFileInDirectory = QString::fromAscii(event->name, event->len);
            // Remove unnecessary postfix (starting with '!') from the file name
            changeForFileInDirectory = changeForFileInDirectory.left(changeForFileInDirectory.lastIndexOf('!'));
            if (!changeForFileInDirectory.isEmpty()) {
                QString eventId = QString::number(event->wd)+changeForFileInDirectory;
                if (eventsForFileInWatchedDirectory.contains(eventId)) {
                    // There is already unhandled event for this file in queue
                    // => Mask is ORed to existing event
                    eventsForFileInWatchedDirectory[eventId].mask |= event->mask;
                } else {
                    // There is no event for this file in queue
                    // => New event is created
                    INotifyEvent inotifyEvent;
                    inotifyEvent.watchDescriptor = event->wd;
                    inotifyEvent.mask = event->mask;
                    inotifyEvent.fileName = QString::fromAscii(event->name, event->len);
                    eventsForFileInWatchedDirectory.insert(eventId, inotifyEvent);
                }
            }
        }
        at += sizeof(inotify_event) + event->len;
    }

    QMap<int, INotifyEvent>::const_iterator it = eventsForWatchedFile.constBegin();
    while (it != eventsForWatchedFile.constEnd()) {
        INotifyEvent event = *it;
        QString file = m_files.value(event.watchDescriptor);
        if (!file.isEmpty()) {
            emit fileChanged(event.watchDescriptor, file, event.mask);
        }
        ++it;
    }

    QMap<QString, INotifyEvent>::const_iterator jt = eventsForFileInWatchedDirectory.constBegin();
    while (jt != eventsForFileInWatchedDirectory.constEnd()) {
        INotifyEvent event = *jt;
        QString file = m_dirs.value(event.watchDescriptor)+"/"+event.fileName;
        emit fileChanged(event.watchDescriptor, file, event.mask);
        ++jt;
    }
}

#include "moc_modestengine_maemo_p.cpp"

QTM_END_NAMESPACE
