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

#include "qgallerytyperequest.h"
#include "qgalleryabstractrequest_p.h"

#include "qgallerynullresultset_p.h"
#include "qgalleryresource.h"

QTM_BEGIN_NAMESPACE

class QGalleryTypeRequestPrivate : public QGalleryAbstractRequestPrivate
{
    Q_DECLARE_PUBLIC(QGalleryTypeRequest)
public:
    QGalleryTypeRequestPrivate(QAbstractGallery *gallery)
        : QGalleryAbstractRequestPrivate(gallery, QGalleryAbstractRequest::TypeRequest)
        , autoUpdate(false)
        , resultSet(0)
        , internalResultSet(0)
    {
        internalResultSet = &nullResultSet;
    }

    void _q_itemsInserted(int index, int)
    {
        if (index == 0)
            resultSet->fetch(0);
    }

    void _q_itemsRemoved(int index, int)
    {
        if (index == 0)
            resultSet->fetch(0);
    }

    void _q_itemsMoved(int from, int to, int)
    {
        if (from == 0 || to == 0)
            resultSet->fetch(0);
    }

    void _q_currentItemChanged()
    {
        emit q_func()->typeChanged();

        if (!propertyKeys.isEmpty())
            emit q_func()->metaDataChanged(propertyKeys);
    }

    void _q_metaDataChanged(int index, int, const QList<int> &keys)
    {
        if (index == 0)
            emit q_func()->metaDataChanged(keys);
    }

    bool autoUpdate;
    QGalleryResultSet *resultSet;
    QGalleryResultSet *internalResultSet;
    QGalleryNullResultSet nullResultSet;
    QStringList propertyNames;
    QString itemType;
    QList<int> propertyKeys;
};

/*!
    \class QGalleryTypeRequest

    \ingroup gallery
    \ingroup gallery-requests

    \inmodule QtGallery
    \since 1.1

    \brief The QGalleryTypeRequest class provides an interface for requesting
    the properties of a type from a gallery.

    QGalleryItemRequest executes a query which returns information summarizing
    items of the type specified in \l itemType. The query will return
    \l {metaData()}{meta-data} values which describe the type as a whole such
    as the total number of items of that type.

    When the request has finished and if the type is one recognized by the
    gallery the \l valid property will be true, if not it will be false.

    If the \l autoUpdate property is true when the request is executed it will
    enter an \l Idle state on finishing and will refresh the queried
    information if the type changes.  If the gallery can't provide updates
    it will instead go immediately to the \l Finished state.  Automatic updates
    can be canceled by calling cancel() on a idle request.

    \sa QDocumentGallery
*/

/*!
    Constructs a new gallery type request.

    The \a parent is passed to QObject.
*/


QGalleryTypeRequest::QGalleryTypeRequest(QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryTypeRequestPrivate(0), parent)
{
}
/*!
    Contructs a new type request for the given \a gallery.

    The \a parent is passed to QObject.
*/

QGalleryTypeRequest::QGalleryTypeRequest(QAbstractGallery *gallery, QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryTypeRequestPrivate(gallery), parent)
{
}

/*!
    Destroys a gallery type request.
*/

QGalleryTypeRequest::~QGalleryTypeRequest()
{
}
/*!
    \property QGalleryTypeRequest::propertyNames

    \brief A list of names of meta-data properties a request should return
    values for.
    \since 1.1
*/


QStringList QGalleryTypeRequest::propertyNames() const
{
    return d_func()->propertyNames;
}

void QGalleryTypeRequest::setPropertyNames(const QStringList &names)
{
    if (d_func()->propertyNames != names) {
        d_func()->propertyNames = names;

        emit propertyNamesChanged();
    }
}

/*!
    \fn QGalleryTypeRequest::propertyNamesChanged()

    Signals that the value of \l propertyNames has changed.
    \since 1.1
*/

/*!
    \property QGalleryTypeRequest::autoUpdate

    \brief Whether a the results of a request should be updated after a request
    has finished.

    If this is true the request will go into the Idle state when the request has
    finished rather than returning to Inactive.
    \since 1.1
*/


bool QGalleryTypeRequest::autoUpdate() const
{
    return d_func()->autoUpdate;
}

void QGalleryTypeRequest::setAutoUpdate(bool enabled)
{
    if (d_func()->autoUpdate != enabled) {
        d_func()->autoUpdate = enabled;

        emit autoUpdateChanged();
    }
}

/*!
    \fn QGalleryTypeRequest::autoUpdateChanged()

    Signals that the value of \l autoUpdate has changed.
    \since 1.1
*/

/*!
    \property QGalleryTypeRequest::itemType

    \brief the type a request should return the properties of.

    \since 1.1
*/

QString QGalleryTypeRequest::itemType() const
{
    return d_func()->itemType;
}

void QGalleryTypeRequest::setItemType(const QString &itemType)
{
    if (d_func()->itemType != itemType) {
        d_func()->itemType = itemType;

        emit itemTypeChanged();
    }
}

/*!
    \fn QGalleryTypeRequest::itemTypeChanged()

    Signals that the \l itemType property has changed.
    \since 1.1
*/

/*!
    Returns the result set containing the meta-data of a type.
    \since 1.1
*/

QGalleryResultSet *QGalleryTypeRequest::resultSet() const
{
    return d_func()->resultSet;
}

/*!
    \fn QGalleryTypeRequest::resultSetChanged(QGalleryResultSet *resultSet)

    Signals that the \a resultSet containing the meta-data of a type has
    changed.
    \since 1.1
*/

/*!
    \fn QGalleryTypeRequest::typeChanged()

    Signals that the properties of a type have changed.
    \since 1.1
*/

/*!
    Returns the key of \a property.
    \since 1.1
*/

int QGalleryTypeRequest::propertyKey(const QString &property) const
{
    return d_func()->internalResultSet->propertyKey(property);
}

/*!
    Returns the attributes of the property identified by \a key.
    \since 1.1
*/

QGalleryProperty::Attributes QGalleryTypeRequest::propertyAttributes(int key) const
{
    return d_func()->internalResultSet->propertyAttributes(key);
}

/*!
    Returns the type of the property identified by \a key.
    \since 1.1
*/

QVariant::Type QGalleryTypeRequest::propertyType(int key) const
{
    return d_func()->internalResultSet->propertyType(key);
}

/*!
    \property QGalleryTypeRequest::valid

    \brief Whether the request currently holds valid type information.
    \since 1.1
*/

bool QGalleryTypeRequest::isValid() const
{
    return d_func()->internalResultSet->isValid();
}

/*!
    Returns the value of a meta-data property identified by \a key.
    \since 1.1
*/


QVariant QGalleryTypeRequest::metaData(int key) const
{
    return d_func()->internalResultSet->metaData(key);
}

/*!
    Returns the value of a meta-data \a property.
    \since 1.1
*/

QVariant QGalleryTypeRequest::metaData(const QString &property) const
{
    return d_func()->internalResultSet->metaData(
            d_func()->internalResultSet->propertyKey(property));
}

/*!
    \fn QGalleryTypeRequest::metaDataChanged(const QList<int> &keys)

    Signals that the values of meta-data properties identified by \a keys
    have changed.
    \since 1.1
*/

/*!
    \reimp
    \since 1.1
*/

void QGalleryTypeRequest::setResponse(QGalleryAbstractResponse *response)
{
    Q_D(QGalleryTypeRequest);

    const bool wasValid = d->internalResultSet->isValid();

    d->resultSet = qobject_cast<QGalleryResultSet *>(response);
    d->propertyKeys.clear();

    if (d->resultSet) {
        d->internalResultSet = d->resultSet;

        connect(d->resultSet, SIGNAL(itemsInserted(int,int)), this, SLOT(_q_itemsInserted(int,int)));
        connect(d->resultSet, SIGNAL(itemsRemoved(int,int)), this, SLOT(_q_itemsRemoved(int,int)));
        connect(d->resultSet, SIGNAL(itemsMoved(int,int,int)),
                this, SLOT(_q_itemsMoved(int,int,int)));
        connect(d->resultSet, SIGNAL(metaDataChanged(int,int,QList<int>)),
                this, SLOT(_q_metaDataChanged(int,int,QList<int>)));
        connect(d->resultSet, SIGNAL(currentItemChanged()), this, SLOT(_q_currentItemChanged()));

        typedef QStringList::const_iterator iterator;
        for (iterator it = d->propertyNames.constBegin(), end = d->propertyNames.constEnd();
                it != end;
                ++it) {
            const int propertyKey = d->resultSet->propertyKey(*it);

            if (propertyKey != -1)
                d->propertyKeys.append(propertyKey);
        }
    } else {
        d->internalResultSet = &d->nullResultSet;
    }

    emit resultSetChanged(d->resultSet);

    if (d->internalResultSet->itemCount() > 0)
        d->internalResultSet->fetch(0);
    else if (wasValid)
        emit typeChanged();
}

#include "moc_qgallerytyperequest.cpp"

QTM_END_NAMESPACE
