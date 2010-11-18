/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

//API
#include "qdocumentgallery.h"
#include "qabstractgallery_p.h"

#include "qgalleryitemrequest.h"
#include "qgalleryqueryrequest.h"
#include "qgallerytyperequest.h"
//Qt
#include <QtCore/qmetaobject.h>

//Backend
#include "qgallerymdsutility_p.h"
#include "qmdegallerytyperesultset_p.h"
#include "qmdegalleryitemresultset_p.h"
#include "qmdegalleryqueryresultset_p.h"
#include "qmdegallerycategoryresultset_p.h"
#include "qmdesession_p.h"

QTM_BEGIN_NAMESPACE

class QDocumentGalleryPrivate : public QAbstractGalleryPrivate
{
public:
    QGalleryAbstractResponse* createTypeResponse(QGalleryTypeRequest *request);
    QGalleryAbstractResponse* createItemResponse(QGalleryItemRequest *request);
    QGalleryAbstractResponse* createQueryResponse(QGalleryQueryRequest *request);

    QMdeSession *m_session;
};

QGalleryAbstractResponse* QDocumentGalleryPrivate::createTypeResponse(QGalleryTypeRequest *request)
{
    // Fill up response class with with request data (create proper query)
    QMDEGalleryTypeResultSet *response = new QMDEGalleryTypeResultSet(m_session, request);
    return response;

}

QGalleryAbstractResponse* QDocumentGalleryPrivate::createItemResponse(QGalleryItemRequest *request)
{
    // Fill up response class with with request data (create proper query)
    if (request->itemId().type() == QVariant::String) {
        const QString itemType = QMDEGalleryCategoryResultSet::itemIdType(
                request->itemId().toString());

        QMDEGalleryCategoryResultSet *response = new QMDEGalleryCategoryResultSet(
                m_session,
                itemType,
                request->propertyNames(),
                QStringList(),
                request->itemId(),
                QGalleryFilter(),
                0,
                0);
        return response;
    } else {
        QMDEGalleryItemResultSet *response = new QMDEGalleryItemResultSet(m_session, request);
        return response;
    }
}

QGalleryAbstractResponse* QDocumentGalleryPrivate::createQueryResponse(QGalleryQueryRequest *request)
{
    // Fill up response class with with request data (create proper query)
    if (QMDEGalleryCategoryResultSet::isCategoryType(request->rootType())) {
        QMDEGalleryCategoryResultSet *response = new QMDEGalleryCategoryResultSet(
                m_session,
                request->rootType(),
                request->propertyNames(),
                request->sortPropertyNames(),
                request->rootItem(),
                request->filter(),
                request->offset(),
                request->limit());
        return response;
    } else {
        QMDEGalleryQueryResultSet *response = new QMDEGalleryQueryResultSet(m_session, request);
        return response;
    }
}

QDocumentGallery::QDocumentGallery(QObject *parent)
: QAbstractGallery(*new QDocumentGalleryPrivate, parent)
{
    Q_D(QDocumentGallery); // access to private class
    d->m_session = new QMdeSession(this);
}

QDocumentGallery::~QDocumentGallery()
{

}

bool QDocumentGallery::isRequestSupported(QGalleryAbstractRequest::RequestType type) const
{
    switch (type) {
    case QGalleryAbstractRequest::QueryRequest:
    case QGalleryAbstractRequest::ItemRequest:
    case QGalleryAbstractRequest::TypeRequest:
        return true;
        default:
        return false;
    }
}

QStringList QDocumentGallery::itemTypePropertyNames(const QString &itemType) const
{
    QStringList list;
    QDocumentGalleryMDSUtility::GetDataFieldsForItemType( list, itemType );
    return list;
}

QGalleryProperty::Attributes QDocumentGallery::propertyAttributes(
    const QString &propertyName, const QString &itemType) const
{
    if(propertyName.isNull() || propertyName.isEmpty() || itemType.isEmpty() || itemType.isNull() )
        return QGalleryProperty::Attributes();
    else if (KErrNotFound == QDocumentGalleryMDSUtility::GetPropertyKey(propertyName) )
        return QGalleryProperty::Attributes();
    else if (!QDocumentGalleryMDSUtility::isItemTypeSupported(itemType))
        return QGalleryProperty::Attributes();
    else if(propertyName == QDocumentGallery::url.name() || 
            propertyName == QDocumentGallery::fileName.name() ||
            propertyName == QDocumentGallery::fileExtension.name())
        return (QGalleryProperty::CanRead | QGalleryProperty::CanSort | QGalleryProperty::CanFilter);
    else if(propertyName == QDocumentGallery::filePath.name())
        return (QGalleryProperty::CanRead | QGalleryProperty::CanFilter );
    else
        return (QGalleryProperty::CanRead | QGalleryProperty::CanWrite | QGalleryProperty::CanSort | QGalleryProperty::CanFilter );

}

QGalleryAbstractResponse* QDocumentGallery::createResponse(QGalleryAbstractRequest *request)
{
    Q_D(QDocumentGallery); // access to private class

    switch (request->type()) {
    case QGalleryAbstractRequest::ItemRequest:
        return d->createItemResponse(static_cast<QGalleryItemRequest *>(request));
    case QGalleryAbstractRequest::TypeRequest:
        return d->createTypeResponse(static_cast<QGalleryTypeRequest *>(request));
    case QGalleryAbstractRequest::QueryRequest:
        return d->createQueryResponse(static_cast<QGalleryQueryRequest *>(request));
    default:
        return 0;
    }
}

#include "moc_qdocumentgallery.cpp"

QTM_END_NAMESPACE
