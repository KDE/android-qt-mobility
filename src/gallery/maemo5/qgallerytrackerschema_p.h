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

#ifndef QGALLERYTRACKERSCHEMA_P_H
#define QGALLERYTRACKERSCHEMA_P_H

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

#include "qgalleryqueryrequest.h"
#include "qgalleryfilter.h"
#include "qgalleryproperty.h"

#include "qdocumentgallery.h"

#include <QtCore/qstringlist.h>

QTM_BEGIN_NAMESPACE


class QGalleryDBusInterfaceFactory;
class QGalleryTrackerImageColumn;
class QGalleryTrackerValueColumn;

struct QGalleryTrackerResultSetArguments;
struct QGalleryTrackerTypeResultSetArguments;

class QM_AUTOTEST_EXPORT QGalleryTrackerSchema
{
public:
    QGalleryTrackerSchema(const QString &itemType);
    ~QGalleryTrackerSchema();

    static QGalleryTrackerSchema fromItemId(const QString &itemId);

    bool isItemType() const { return m_itemIndex >= 0; }
    bool isAggregateType() const { return m_aggregateIndex >= 0; }

    QString itemType() const;

    static QString uriFromItemId(QDocumentGallery::Error *error, const QVariant &itemId);

    static int serviceUpdateId(const QString &service);

    QStringList supportedPropertyNames() const;
    QGalleryProperty::Attributes propertyAttributes(const QString &propertyName) const;

    QDocumentGallery::Error prepareItemResponse(
            QGalleryTrackerResultSetArguments *arguments,
            QGalleryDBusInterfaceFactory *dbus,
            const QString &itemId,
            const QStringList &propertyNames) const;

    QDocumentGallery::Error prepareQueryResponse(
            QGalleryTrackerResultSetArguments *arguments,
            QGalleryDBusInterfaceFactory *dbus,
            QGalleryQueryRequest::Scope scope,
            const QString &rootItem,
            const QGalleryFilter &filter,
            const QStringList &propertyNames,
            const QStringList &sortPropertyNames) const;

    QDocumentGallery::Error prepareTypeResponse(
            QGalleryTrackerTypeResultSetArguments *arguments,
            QGalleryDBusInterfaceFactory *dbus) const;

private:
    QGalleryTrackerSchema(int itemIndex, int aggregateIndex)
        : m_itemIndex(itemIndex), m_aggregateIndex(aggregateIndex) {}

    QDocumentGallery::Error buildFilterQuery(
            QString *query,
            QGalleryQueryRequest::Scope scope,
            const QString &scopeItemId,
            const QGalleryFilter &filter) const;

    void populateItemArguments(
            QGalleryTrackerResultSetArguments *arguments,
            QGalleryDBusInterfaceFactory *dbus,
            const QString &query,
            const QStringList &propertyNames,
            const QStringList &sortPropertyNames) const;
    void populateAggregateArguments(
            QGalleryTrackerResultSetArguments *arguments,
            QGalleryDBusInterfaceFactory *dbus,
            const QString &query,
            const QStringList &propertyNames,
            const QStringList &sortPropertyNames) const;

    const int m_itemIndex;
    const int m_aggregateIndex;
};

QTM_END_NAMESPACE

#endif
