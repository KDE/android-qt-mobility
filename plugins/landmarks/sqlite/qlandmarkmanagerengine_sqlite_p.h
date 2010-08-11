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

#ifndef QLANDMARKMANAGERENGINE_SQLITE_P_H
#define QLANDMARKMANAGERENGINE_SQLITE_P_H

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

#include <qlandmarkmanagerengine.h>
#include "databasefilewatcher_p.h"

#include <QSqlDatabase>
#include <QHash>
#include "databaseoperations_p.h"

QTM_USE_NAMESPACE
using namespace DatabaseOperations;

class QLandmarkManagerEngineSqlite : public QLandmarkManagerEngine
{
    Q_OBJECT
public:
    QLandmarkManagerEngineSqlite(const QString &filename = QString());
    ~QLandmarkManagerEngineSqlite();

    /* URI reporting */
    QString managerName() const;
    QMap<QString, QString> managerParameters() const;
    int managerVersion() const;

    /* Filtering */
    QList<QLandmarkId> landmarkIds(const QLandmarkFilter& filter,
                                   int limit, int offset,
                                   const QList<QLandmarkSortOrder>& sortOrders,
                                   QLandmarkManager::Error *error,
                                   QString *errorString) const;
    QList<QLandmarkCategoryId> categoryIds(int limit, int offset,
                                           const QLandmarkNameSort &nameSort,
                                           QLandmarkManager::Error *error,
                                           QString *errorString) const;

    /* Retrieval */
    QLandmark landmark(const QLandmarkId &landmarkId,
                       QLandmarkManager::Error *error,
                       QString *errorString) const;
    QList<QLandmark> landmarks(const QList<QLandmarkId> &landmarkIds,QMap<int, QLandmarkManager::Error> *errorMap,
                                       QLandmarkManager::Error *error, QString *errorString) const;
    QList<QLandmark> landmarks(const QLandmarkFilter& filter,
                               int limit, int offset,
                               const QList<QLandmarkSortOrder>& sortOrders,
                               QLandmarkManager::Error *error,
                               QString *errorString) const;

    QLandmarkCategory category(const QLandmarkCategoryId &landmarkCategoryId,
                               QLandmarkManager::Error *error,
                               QString *errorString) const;

    QList<QLandmarkCategory> categories(const QList<QLandmarkCategoryId> &landmarkCategoryIds,
                                        QMap<int, QLandmarkManager::Error> *errorMap,
                                        QLandmarkManager::Error *error, QString *errorString) const;

    QList<QLandmarkCategory> categories(int limit, int offset,
                                        const QLandmarkNameSort &nameSort,
                                        QLandmarkManager::Error *error,
                                        QString *errorString) const;

    /*saving and removing*/
    bool saveLandmark(QLandmark* landmark,
                      QLandmarkManager::Error *error,
                      QString *errorString);
    bool saveLandmarks(QList<QLandmark> * landmark,
                       QMap<int, QLandmarkManager::Error> *errorMap,
                       QLandmarkManager::Error *error,
                       QString *errorString);
    bool removeLandmark(const QLandmarkId &landmarkId,
                        QLandmarkManager::Error *error,
                        QString *errorString);
    bool removeLandmarks(const QList<QLandmarkId> &landmarkId,
                         QMap<int, QLandmarkManager::Error> *errorMap,
                         QLandmarkManager::Error *error,
                         QString *errorString);

    bool saveCategory(QLandmarkCategory* category,
                      QLandmarkManager::Error *error,
                      QString *errorString);
    bool removeCategory(const QLandmarkCategoryId &categoryId,
                        QLandmarkManager::Error *error,
                        QString *errorString);

    bool importLandmarks(QIODevice *device,
                         const QString &format,
                         QLandmarkManager::TransferOption option,
                         const QLandmarkCategoryId &categoryId,
                         QLandmarkManager::Error *error,
                         QString *errorString);
    bool exportLandmarks(QIODevice *device,
                         const QString &format,
                         QList<QLandmarkId> landmarkIds,
                         QLandmarkManager::TransferOption option,
                         QLandmarkManager::Error *error,
                         QString *errorString) const;
    QStringList supportedFormats(QLandmarkManager::TransferOperation operation, QLandmarkManager::Error *error, QString *errorString) const;

    QLandmarkManager::SupportLevel filterSupportLevel(const QLandmarkFilter &filter, QLandmarkManager::Error *error, QString *errorString) const;
    QLandmarkManager::SupportLevel sortOrderSupportLevel(const QList<QLandmarkSortOrder> &sortOrders, QLandmarkManager::Error *error, QString *errorString) const;
    bool isFeatureSupported(QLandmarkManager::LandmarkFeature feature, QLandmarkManager::Error *error, QString *errorString) const;

    QStringList landmarkAttributeKeys(QLandmarkManager::Error *error, QString *errorString) const;
    QStringList categoryAttributeKeys(QLandmarkManager::Error *error, QString *errorString) const;

    bool isExtendedAttributesEnabled(QLandmarkManager::Error *error, QString *errorString) const;;
    void setExtendedAttributesEnabled(bool enabled, QLandmarkManager::Error *error, QString *errorString);

    bool isCustomAttributesEnabled(QLandmarkManager::Error *error, QString *errorString) const;
    void setCustomAttributesEnabled(bool enabled, QLandmarkManager::Error *error, QString *errorString);

    virtual bool isReadOnly(QLandmarkManager::Error *error, QString *errorString) const;
    virtual bool isReadOnly(const QLandmarkId &landmarkId, QLandmarkManager::Error *error, QString *errorString) const;
    virtual bool isReadOnly(const QLandmarkCategoryId &categoryId, QLandmarkManager::Error *error, QString *errorString) const;

    /* Asynchronous Request Support */
    void requestDestroyed(QLandmarkAbstractRequest* request);
    bool startRequest(QLandmarkAbstractRequest* request);
    bool cancelRequest(QLandmarkAbstractRequest* request);
    bool waitForRequestFinished(QLandmarkAbstractRequest* request, int msecs);

public slots:
    void updateLandmarkIdFetchRequest(QLandmarkIdFetchRequest* req, const QList<QLandmarkId>& result,
            QLandmarkManager::Error error, const QString &errorString, QLandmarkAbstractRequest::State newState);
    void updateLandmarkFetchRequest(QLandmarkFetchRequest* req, const QList<QLandmark>& result,
            QLandmarkManager::Error error, const QString &errorString, QLandmarkAbstractRequest::State newState);
    void updateLandmarkFetchByIdRequest(QLandmarkFetchByIdRequest* req, const QList<QLandmark>& result,
                                   QLandmarkManager::Error error, const QString &errorString, const ERROR_MAP &errorMap, QLandmarkAbstractRequest::State newState);
    void updateLandmarkSaveRequest(QLandmarkSaveRequest* req, const QList<QLandmark>& result,
                                    QLandmarkManager::Error error, const QString &errorString, const ERROR_MAP &errorMap, QLandmarkAbstractRequest::State newState);
    void updateLandmarkRemoveRequest(QLandmarkRemoveRequest* req, QLandmarkManager::Error error,
                                    const QString &errorString, const ERROR_MAP &errorMap, QLandmarkAbstractRequest::State newState);
    void updateLandmarkCategoryIdFetchRequest(QLandmarkCategoryIdFetchRequest *req, const QList<QLandmarkCategoryId>& result,
            QLandmarkManager::Error error, const QString &errorString, QLandmarkAbstractRequest::State newState);
    void updateLandmarkCategoryFetchRequest(QLandmarkCategoryFetchRequest *req, const QList<QLandmarkCategory>& result,
                                                   QLandmarkManager::Error error, const QString &errorString, QLandmarkAbstractRequest::State newState);
    void updateLandmarkCategoryFetchByIdRequest(QLandmarkCategoryFetchByIdRequest* req, const QList<QLandmarkCategory> &categories, QLandmarkManager::Error error,
                                     const QString &errorString, const ERROR_MAP &errorMap, QLandmarkAbstractRequest::State newState);
    void updateLandmarkCategorySaveRequest(QLandmarkCategorySaveRequest* req, const QList<QLandmarkCategory> &categories, QLandmarkManager::Error error,
                                     const QString &errorString, const ERROR_MAP &errorMap, QLandmarkAbstractRequest::State newState);
    void updateLandmarkCategoryRemoveRequest(QLandmarkCategoryRemoveRequest* req, QLandmarkManager::Error error,
                                           const QString &errorString, const ERROR_MAP &errorMap, QLandmarkAbstractRequest::State newState);
    void updateLandmarkImportRequest(QLandmarkImportRequest *req, QLandmarkManager::Error error, const QString &errorString,
                                            QLandmarkAbstractRequest::State newState);
    void updateLandmarkExportRequest(QLandmarkExportRequest *req, QLandmarkManager::Error error, const QString &errorString,
                                     QLandmarkAbstractRequest::State newState);
    void updateRequestState(QLandmarkAbstractRequest *req, QLandmarkAbstractRequest::State state);

private slots:
    void databaseChanged();

public:
    static QList<QLandmarkId> sortLandmarks(const QList<QLandmark>& landmarks, const QList<QLandmarkSortOrder> &sortOrders) {
        return QLandmarkManagerEngine::sortLandmarks(landmarks,sortOrders);
    }

protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);

private:

    void setChangeNotificationsEnabled(bool enabled);

    QString m_dbFilename;
    QString m_dbConnectionName;
    QHash<QLandmarkAbstractRequest *, QueryRun *> m_requestRunHash;
    DatabaseFileWatcher *m_dbWatcher;
    qreal m_latestTimestamp;
    friend class DatabaseOperations::QueryRun;
    bool m_isExtendedAttributesEnabled;
    bool m_isCustomAttributesEnabled;
};

#endif // QLANDMARKMANAGERENGINE_SQLITE_P_H
