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

#include "qgallerydocumentlist.h"

/*!
    \class QGalleryResource

    \ingroup gallery

    \brief The QGalleryResource class provides a resource identifier for a
    gallery document.
*/

/*!
    Constructs a null resource.
*/

QGalleryResource::QGalleryResource()
{
}

/*!
    Constructs a resource from a \a url and a set of \a attributes.
*/

QGalleryResource::QGalleryResource(const QUrl &url, QMap<int, QVariant> attributes)
    : m_url(url)
    , m_attributes(attributes)
{
}

/*!
    Constructs a copy of a \a resource.
*/

QGalleryResource::QGalleryResource(const QGalleryResource &resource)
    : m_url(resource.m_url)
    , m_attributes(resource.m_attributes)
{
}

/*!
    Destroys a resource.
*/

QGalleryResource::~QGalleryResource()
{
}

/*!
    Assigns the value of \a resource to another resource.
*/

QGalleryResource &QGalleryResource::operator =(const QGalleryResource &resource)
{
    m_url = resource.m_url;
    m_attributes = resource.m_attributes;

    return *this;
}

/*!
    Compares \a resource to another resource.

    Returns true if both resources are the same, and false otherwise.
*/

bool QGalleryResource::operator ==(const QGalleryResource &resource) const
{
    return m_url == resource.m_url && m_attributes == resource.m_attributes;
}

/*!
    Compares \a resource to another resource.

    Returns true if the resource are not the same, and false otherwise.
*/

bool QGalleryResource::operator !=(const QGalleryResource &resource) const
{
    return m_url != resource.m_url || m_attributes != resource.m_attributes;
}

/*!
    Returns the URL of a resource.
*/

QUrl QGalleryResource::url() const
{
    return m_url;
}

/*!
    Returns the attributes of a resource.
*/

QMap<int, QVariant> QGalleryResource::attributes() const
{
    return m_attributes;
}

/*!
    Returns the value of the resource attribute identified by \a key.
*/

QVariant QGalleryResource::attribute(int key) const
{
    return m_attributes.value(key);
}

/*!
    \class QGalleryDocumentList

    \ingroup gallery

    \brief The QGalleryDocumentList class provides an abstract interface for a
    list of documents from a gallery.
*/

/*!
    \enum QGalleryDocumentList::MetaDataFlag

    Provides information about the state of a meta-data value.

    \value ReadMetaReadable
    \value MetaDataWritable
    \value MetaDataWriting
    \value MetaDataWriteError
*/

/*!
    Constructs a new gallery document list.

    The \a parent is passed to QObject.
*/
QGalleryDocumentList::QGalleryDocumentList(QObject *parent)
    : QObject(parent)
{
}

/*!
    Destroys a gallery document list.
*/
QGalleryDocumentList::~QGalleryDocumentList()
{
}

/*!
    \fn QGalleryDocumentList::keys() const

    Returns a list of meta-data keys a document list has values for.
*/

/*!
    \fn QGalleryDocumentList::toString(int key) const

    Returns the string representation of a meta-data \a key.
*/

/*!
    \fn QGalleryDocumentList::count() const

    Returns the number of documents in a document list.
*/

/*!
    \fn QGalleryDocumentList::id(int index) const

    Returns the id of the document at \a index.
*/

/*!
    \fn QGalleryDocumentList::url(int index) const

    Returns the URL of the document at \a index.
*/

/*!
    \fn QGalleryDocumentList::type(int index) const

    Returns the type of the document at \a index.
*/

/*!
    \fn QGalleryDocumentList::parentId(int index) const

    Returns the id of the parent of the document at \a index.
*/

/*!
    \fn QGalleryDocumentList::resources(int index) const

    Returns a list of resources for the document at \a index.
*/

/*!
    \fn QGalleryDocumentList::metaData(int index, int key) const

    Returns the meta-data \a key value of the document at \a index.
*/

/*!
    \fn QGalleryDocumentList::setMetaData(int index, int key, const QVariant &value)

    Sets the meta-data \a key \a value of the document at \a index.
*/

/*!
    \fn QGalleryDocumentList::metaDataFlags(int index, int key) const

    Returns the state fo the meta-data \a key value at \a index.
*/

/*!
    \fn QGalleryDocumentList::inserted(int index, int count)

    Signals that \a count documents have been inserted into a document list
    starting at \a index.
*/

/*!
    \fn QGalleryDocumentList::removed(int index, int count)

    Signals that \a count documents have been removed from a document list
    starting at \a index.
*/

/*!
    \fn QGalleryDocumentList::moved(int from, int to, int count)

    Signals that \a count documents have been moved \a from one starting index
    \a to another.
*/

/*!
    \fn QGalleryDocumentList::metaDataChanged(int index, int count, const QList<int> &keys)

    Signals that the meta-data identified by \a keys of \a count documents
    starting at \a index has changed.
*/
