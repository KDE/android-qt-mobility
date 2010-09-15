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
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEGALLERYITEM_H
#define QDECLARATIVEGALLERYITEM_H

#include <qgalleryitemrequest.h>

#include "qdeclarativedocumentgallery.h"

#include <QtCore/qpointer.h>
#include <QtCore/qurl.h>
#include <QtDeclarative/qdeclarative.h>
#include <QtDeclarative/qdeclarativepropertymap.h>

QTM_BEGIN_NAMESPACE

class QDeclarativeGalleryItem : public QObject, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)
    Q_ENUMS(Status)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
    Q_PROPERTY(QStringList properties READ propertyNames WRITE setPropertyNames NOTIFY propertyNamesChanged)
    Q_PROPERTY(bool autoUpdate READ autoUpdate WRITE setAutoUpdate NOTIFY autoUpdateChanged)
    Q_PROPERTY(QVariant item READ itemId WRITE setItemId NOTIFY itemIdChanged)
    Q_PROPERTY(bool available READ available NOTIFY availableChanged)
    Q_PROPERTY(QUrl itemUrl READ itemUrl NOTIFY availableChanged)
    Q_PROPERTY(QObject *metaData READ metaData NOTIFY metaDataChanged)
public:
    enum Status
    {
        Null        = QGalleryAbstractRequest::Inactive,
        Active      = QGalleryAbstractRequest::Active,
        Finished    = QGalleryAbstractRequest::Finished,
        Idle        = QGalleryAbstractRequest::Idle,
        Cancelling  = QGalleryAbstractRequest::Cancelling,
        Cancelled   = QGalleryAbstractRequest::Cancelled,
        Error       = QGalleryAbstractRequest::Error
    };

    ~QDeclarativeGalleryItem();

    Status status() const { return m_status; }

    qreal progress() const
    {
        const int max = m_request.maximumProgress();
        return max > 0 ? qreal(m_request.currentProgress()) / max : qreal(0.0);
    }

    QString errorMessage() const { return m_errorMessage; }

    QStringList propertyNames() { return m_request.propertyNames(); }
    void setPropertyNames(const QStringList &names) {
        if (!m_complete) m_request.setPropertyNames(names); emit propertyNamesChanged(); }

    bool autoUpdate() const { return m_request.autoUpdate(); }
    void setAutoUpdate(bool enabled) { m_request.setAutoUpdate(enabled); emit autoUpdateChanged(); }

    QVariant itemId() const { return m_request.itemId(); }
    void setItemId(const QVariant &itemId) {
        m_request.setItemId(itemId); if (m_complete) m_request.execute(); emit itemIdChanged(); }

    bool available() const { return m_request.isValid(); }

    QUrl itemUrl() const { return m_request.itemUrl(); }

    QObject *metaData() const { return m_metaData; }

    void componentComplete();

public Q_SLOTS:
    void reload() { m_request.execute(); }
    void cancel() { m_request.cancel(); }
    void clear() { m_request.clear(); }

Q_SIGNALS:
    void statusChanged();
    void progressChanged();
    void errorMessageChanged();
    void availableChanged();
    void metaDataChanged();

    void galleryChanged();
    void propertyNamesChanged();
    void autoUpdateChanged();
    void itemIdChanged();

protected:
    explicit QDeclarativeGalleryItem(QObject *parent = 0);

    QGalleryItemRequest m_request;
    QDeclarativePropertyMap *m_metaData;
    QHash<int, QString> m_propertyKeys;
    QString m_errorMessage;
    Status m_status;
    bool m_complete;

private Q_SLOTS:
    void _q_statusChanged();
    void _q_itemChanged();
    void _q_metaDataChanged(const QList<int> &keys);
    void _q_valueChanged(const QString &key, const QVariant &value) {
        m_request.setMetaData(key, value); }
};

class QDeclarativeDocumentGalleryItem : public QDeclarativeGalleryItem
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeDocumentGallery::ItemType itemType READ itemType NOTIFY itemTypeChanged)
public:
    explicit QDeclarativeDocumentGalleryItem(QObject *parent = 0);
    ~QDeclarativeDocumentGalleryItem();

    void classBegin();

    QDeclarativeDocumentGallery::ItemType itemType() const;

Q_SIGNALS:
    void itemTypeChanged();
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeDocumentGalleryItem))

#endif
