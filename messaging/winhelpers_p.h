/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef QMESSAGEWINHELPERPRIVATE_H
#define QMESSAGEWINHELPERPRIVATE_H

#include <QHash>
#include <QMap>
#include <QPair>
#include <QSharedPointer>
#include <QString>
#include <QByteArray>
#include <MAPIUtil.h>
#include <qmessagestore.h>
#include <QVector>

// As per http://support.microsoft.com/kb/816477
#ifndef PR_MSG_EDITOR_FORMAT
#define PR_MSG_EDITOR_FORMAT PROP_TAG( PT_LONG, 0x5909 )
#define EDITOR_FORMAT_DONTKNOW ((ULONG)0)
#define EDITOR_FORMAT_PLAINTEXT ((ULONG)1)
#define EDITOR_FORMAT_HTML ((ULONG)2)
#define EDITOR_FORMAT_RTF ((ULONG)3)
#endif

#ifndef PR_ATTACH_CONTENT_ID
// This is not available in my SDK version...
#define PR_ATTACH_CONTENT_ID PROP_TAG( PT_UNICODE, 0x3712 )
#endif

#ifndef PR_BODY_HTML
#define PR_BODY_HTML PROP_TAG( PT_TSTRING, 0x1013 )
#endif

#define mapiRelease(a) if(a){a->Release();a=0;}

class MapiEntryId : public QByteArray
{
public:
    MapiEntryId():QByteArray(){}
    MapiEntryId(LPENTRYID mapiEntryId, ULONG entryIdLength)
    :
    QByteArray(reinterpret_cast<const char*>(mapiEntryId),entryIdLength){}

    MapiEntryId(LPBYTE mapiEntryId, ULONG entryIdLength)
    :
    QByteArray(reinterpret_cast<const char*>(mapiEntryId),entryIdLength){}
};

class MapiRecordKey : public QByteArray
{
public:
    MapiRecordKey():QByteArray(){}
    MapiRecordKey(LPENTRYID mapiRecordKey, ULONG entryIdLength)
    :
    QByteArray(reinterpret_cast<const char*>(mapiRecordKey),entryIdLength){}

    MapiRecordKey(LPBYTE mapiRecordKey, ULONG entryIdLength)
    :
    QByteArray(reinterpret_cast<const char*>(mapiRecordKey),entryIdLength){}
};

class MapiFolder;
class MapiStore;
class MapiSession;
typedef QSharedPointer<MapiFolder> MapiFolderPtr;
typedef QSharedPointer<MapiStore> MapiStorePtr;
typedef QSharedPointer<MapiSession> MapiSessionPtr;

namespace WinHelpers {

QString QStringFromLpctstr(LPCTSTR lpszValue);
void LptstrFromQString(const QString &value, LPTSTR *lpsz);

typedef QPair<QMessageId, ULONG> AttachmentLocator;
QMessageContentContainer fromLocator(const WinHelpers::AttachmentLocator &l);

ULONG createNamedProperty(IMAPIProp *object, const QString &name);
ULONG getNamedPropertyTag(IMAPIProp *object, const QString &name);

bool setNamedProperty(IMAPIProp *object, ULONG tag, const QString &value);
QString getNamedProperty(IMAPIProp *object, ULONG tag);

QByteArray contentTypeFromExtension(const QString &extension);

class Lptstr : public QVector<TCHAR>
{
        public:
            Lptstr(int length) : QVector(length){}
            operator TCHAR* (){ return QVector::data(); }
};

Lptstr LptstrFromQString(const QString &src);

class MapiInitializer;

typedef QSharedPointer<MapiInitializer> MapiInitializationToken;

MapiInitializationToken initializeMapi();

class MapiInitializer
{
    friend MapiInitializationToken WinHelpers::initializeMapi();

private:
    MapiInitializer();
    MapiInitializer &operator=(const MapiInitializer &);

public:
    ~MapiInitializer();

    bool _initialized;
};

}

class MapiFolder {

public:
    enum SaveOption { DontSaveMessage = 0, SaveMessage };
    enum PostSendAction { DoNothing = 0, DeleteAfterSend, MoveAfterSend };

public:
    static MapiFolderPtr createFolder(QMessageStore::ErrorCode *lastError, const MapiStorePtr &store, IMAPIFolder *folder, const MapiRecordKey &recordKey, const QString &name, const MapiEntryId &entryId, bool hasSubFolders, uint messageCount);

    ~MapiFolder();

    MapiFolderPtr nextSubFolder(QMessageStore::ErrorCode *lastError, const MapiStore &store);

    QMessageIdList queryMessages(QMessageStore::ErrorCode *lastError, const QMessageFilter &filter = QMessageFilter(), const QMessageOrdering &ordering = QMessageOrdering(), uint limit = 0, uint offset = 0) const;
    uint countMessages(QMessageStore::ErrorCode *lastError, const QMessageFilter &filter = QMessageFilter()) const;

    void removeMessages(QMessageStore::ErrorCode *lastError, const QMessageIdList &ids);
//    MapiEntryId messageEntryId(QMessageStore::ErrorCode *lastError, const MapiRecordKey &messagekey);
#ifdef QMESSAGING_OPTIONAL_FOLDER
    QMessageFolderId id() const;
#endif

    bool isValid() const { return _valid; }
    IMAPIFolder* folder() const { return _folder; }
    MapiRecordKey recordKey() const { return _key; }
    MapiRecordKey storeKey() const;
#ifdef _WIN32_WCE
    MapiEntryId storeEntryId() const;
#endif
    QString name() const { return _name; }
    MapiEntryId entryId() const { return _entryId; }
    bool hasSubFolders() const { return _hasSubFolders; }
    uint messageCount() const { return _messageCount; }

    LPMAPITABLE subFolders(QMessageStore::ErrorCode *lastError) { if (!_init) findSubFolders(lastError); return _subFolders; }
    IMessage *createMessage(QMessageStore::ErrorCode* lastError);
    IMessage *createMessage(QMessageStore::ErrorCode* lastError, const QMessage& source, const MapiSessionPtr session, PostSendAction postSendAction = MoveAfterSend, SaveOption saveOption = SaveMessage );

    IMessage *openMessage(QMessageStore::ErrorCode *lastError, const MapiEntryId &entryId);

    QMessage::StandardFolder standardFolder() const;

private:
    MapiFolder();
    MapiFolder(const MapiStorePtr &store, IMAPIFolder *folder, const MapiRecordKey &recordKey, const QString &name, const MapiEntryId &entryId, bool hasSubFolders, uint messageCount);

    void findSubFolders(QMessageStore::ErrorCode *lastError);

    friend class MapiStore;

    QWeakPointer<MapiFolder> _self;
    MapiStorePtr _store;
    bool _valid;
    IMAPIFolder* _folder;
    MapiRecordKey _key;
    QString _name;
    LPMAPITABLE _subFolders;
    uint _itemCount;
    MapiEntryId _entryId;
    bool _hasSubFolders;
    uint _messageCount;
    bool _init;
};

class MapiStore {
public:
    static MapiStorePtr createStore(QMessageStore::ErrorCode *lastError, const MapiSessionPtr &session, LPMDB store, const MapiRecordKey &key, const MapiEntryId &entryId, const QString &name, bool cachedMode);

    ~MapiStore();

    MapiFolderPtr findFolder(QMessageStore::ErrorCode *lastError, const MapiRecordKey &key);
    MapiFolderPtr findFolder(QMessageStore::ErrorCode *lastError, QMessage::StandardFolder sf);
#ifdef QMESSAGING_OPTIONAL_FOLDER
    QMessageFolderIdList folderIds(QMessageStore::ErrorCode *lastError);
    QMessageFolder folderFromId(QMessageStore::ErrorCode *lastError, const QMessageFolderId &folderId);
#endif

    MapiFolderPtr openFolder(QMessageStore::ErrorCode *lastError, const MapiEntryId& id) const;
    MapiFolderPtr openFolderWithKey(QMessageStore::ErrorCode *lastError, const MapiRecordKey& key) const;

    bool isValid() const { return _valid; }
    QMessageAccountId id() const;
    MapiEntryId entryId() const { return _entryId; }
    QString name() const { return _name; }
    IMsgStore* store() const { return _store; }
    MapiRecordKey recordKey() const { return _key; }
    QMessage::TypeFlags types() const;
    QMessageAddress address() const;

    MapiFolderPtr rootFolder(QMessageStore::ErrorCode *lastError) const;
    MapiFolderPtr receiveFolder(QMessageStore::ErrorCode *lastError) const;

    IMessage *openMessage(QMessageStore::ErrorCode *lastError, const MapiEntryId &entryId);

    QMessageIdList queryMessages(QMessageStore::ErrorCode *lastError, const QMessageFilter &filter, const QMessageOrdering &ordering, uint limit, uint offset) const;

    void notifyEvents(ULONG mask);

private:
    MapiStore();
    MapiStore(const MapiSessionPtr &session, LPMDB store, const MapiRecordKey &key, const MapiEntryId &entryId, const QString &name, bool cachedMode);

    IMAPIFolder *openMapiFolder(QMessageStore::ErrorCode *lastError, const MapiEntryId &entryId) const;

    bool setAdviseSink(ULONG mask, IMAPIAdviseSink *sink);

    class AdviseSink : public IMAPIAdviseSink
    {
        MapiStore *_store;
        LONG _refCount;

    public:
        AdviseSink(MapiStore *store) : _store(store), _refCount(0) {}

        STDMETHOD(QueryInterface)(REFIID riid, LPVOID FAR* ppvObj);
        STDMETHOD_(ULONG, AddRef)();
        STDMETHOD_(ULONG, Release)();

        STDMETHOD_(ULONG, OnNotify)(ULONG cNotification, LPNOTIFICATION lpNotifications);
    };

    QWeakPointer<MapiStore> _self;
    MapiSessionPtr _session;
    bool _valid;
    IMsgStore* _store;
    MapiRecordKey _key;
    MapiEntryId _entryId;
    QString _name;
    bool _cachedMode;
    ULONG _adviseConnection;

    static QHash<MapiEntryId, QWeakPointer<MapiFolder> > _folderMap;
};

class MapiSession : public QObject
{
    Q_OBJECT

public:
    static MapiSessionPtr createSession(QMessageStore::ErrorCode *lastError);

    ~MapiSession();

    bool isValid() const { return (_mapiSession != 0); }

    MapiStorePtr findStore(QMessageStore::ErrorCode *lastError, const QMessageAccountId &id = QMessageAccountId(), bool cachedMode = true) const;
    MapiStorePtr defaultStore(QMessageStore::ErrorCode *lastError, bool cachedMode = true) const { return findStore(lastError,QMessageAccountId(),cachedMode); }
    QList<MapiStorePtr> allStores(QMessageStore::ErrorCode *lastError, bool cachedMode = true) const;

    MapiStorePtr openStore(QMessageStore::ErrorCode *lastError, const MapiEntryId& id, bool cachedMode = true) const;
    MapiStorePtr openStoreWithKey(QMessageStore::ErrorCode *lastError, const MapiRecordKey& key, bool cachedMode = true) const;

    QMessageAccountId defaultAccountId(QMessageStore::ErrorCode *lastError, QMessage::Type type) const;

    MapiRecordKey messageRecordKey(QMessageStore::ErrorCode *lastError, const QMessageId &id);
    MapiRecordKey folderRecordKey(QMessageStore::ErrorCode *lastError, const QMessageId &id);

#ifdef _WIN32_WCE
    MapiEntryId folderEntryId(QMessageStore::ErrorCode *lastError, const QMessageId &id);
#endif

    QMessageFolder folder(QMessageStore::ErrorCode *lastError, const QMessageFolderId& id) const;
    QMessage message(QMessageStore::ErrorCode *lastError, const QMessageId& id) const;

    bool updateMessageProperties(QMessageStore::ErrorCode *lastError, QMessage *msg) const;
    bool updateMessageRecipients(QMessageStore::ErrorCode *lastError, QMessage *msg) const;
    bool updateMessageBody(QMessageStore::ErrorCode *lastError, QMessage *msg) const;
    bool updateMessageAttachments(QMessageStore::ErrorCode *lastError, QMessage *msg) const;

    QByteArray attachmentData(QMessageStore::ErrorCode *lastError, const QMessageId& id, ULONG number) const;

    QMessageIdList queryMessages(QMessageStore::ErrorCode *lastError, const QMessageFilter &filter, const QMessageOrdering &ordering, uint limit, uint offset) const;

    void updateMessage(QMessageStore::ErrorCode* lastError, const QMessage& source);

    void removeMessages(QMessageStore::ErrorCode *lastError, const QMessageIdList &ids);

    bool showForm(IMessage* message, IMAPIFolder* folder, IMsgStore* store);

    IMAPISession* session() const { return _mapiSession; }

    bool flushQueues();

    QMessageStore::NotificationFilterId registerNotificationFilter(QMessageStore::ErrorCode *lastError, const QMessageFilter &filter);
    void unregisterNotificationFilter(QMessageStore::ErrorCode *lastError, QMessageStore::NotificationFilterId filterId);

    void notify(MapiStore *store, ULONG notificationCount, NOTIFICATION *notifications);

signals:
    void messageAdded(const QMessageId &id, const QMessageStore::NotificationFilterIdSet &matchingFilterIds);
    void messageRemoved(const QMessageId &id, const QMessageStore::NotificationFilterIdSet &matchingFilterIds);
    void messageUpdated(const QMessageId &id, const QMessageStore::NotificationFilterIdSet &matchingFilterIds);

public slots:
    void dispatchNotifications();

private:
    MapiSession();
    MapiSession(QMessageStore::ErrorCode *lastError);

    IMsgStore *openMapiStore(QMessageStore::ErrorCode *lastError, const MapiEntryId &entryId, bool cachedMode = true) const;
    IMessage *openMapiMessage(QMessageStore::ErrorCode *lastError, const QMessageId &id) const;

    void addRecipients(LPMESSAGE message, const QMessageAddressList& addressList, unsigned long mapiAddressType);
    void addAttachment(LPMESSAGE message, const QMessageContentContainer& attachmentContainer);

private:
    QWeakPointer<MapiSession> _self;
    WinHelpers::MapiInitializationToken _token;
    IMAPISession* _mapiSession;
    QMessageStore::NotificationFilterId _filterId;
    QMap<QMessageStore::NotificationFilterId, QMessageFilter> _filters;
    bool _registered;

    static QWeakPointer<MapiSession> _session;

    static QHash<MapiEntryId, QSharedPointer<MapiStore> > _storeMap;
};

#endif
