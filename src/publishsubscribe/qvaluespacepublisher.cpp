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

#include "qvaluespacepublisher.h"
#include "qvaluespace.h"
#include "qvaluespace_p.h"
#include "qvaluespacemanager_p.h"

#include <QtCore/qcoreapplication.h>

#include <QtCore/qdebug.h>

QTM_BEGIN_NAMESPACE

/*!
    \class QValueSpacePublisher

    \brief The QValueSpacePublisher class allows applications to publish values in the Value Space.

    \ingroup publishsubscribe

    When multiple Value Space layers are available QValueSpacePublisher only publishes values to
    the layer with the highest order.  The layers that QValueSpacePublisher can access can be
    limited by specifying either a \l {QValueSpace::LayerOption}{filter} or a QUuid at construction
    time.

    The lifetime of values published in the Value Space is specified by the particular Value Space
    layer that the value is published in.  For details on the different behaviors see
    QValueSpace::LayerOption.

    Once a value is published all applications in the system will have read access to it via the
    QValueSpaceSubscriber class.

    Although, logically, the Value Space is a simple collection of
    hierarchical paths, these paths can conceptually be visualized as a set of
    objects with attributes.  For example, rather than viewing the following list
    as 12 distinct Value Space paths:

    \code
    /Device/Network/Interfaces/eth0/Name
    /Device/Network/Interfaces/eth0/Type
    /Device/Network/Interfaces/eth0/Status
    /Device/Network/Interfaces/eth0/BytesSent
    /Device/Network/Interfaces/eth0/BytesReceived
    /Device/Network/Interfaces/eth0/Time
    /Device/Network/Interfaces/ppp0/Name
    /Device/Network/Interfaces/ppp0/Type
    /Device/Network/Interfaces/ppp0/Status
    /Device/Network/Interfaces/ppp0/BytesSent
    /Device/Network/Interfaces/ppp0/BytesReceived
    /Device/Network/Interfaces/ppp0/Time
    \endcode

    it can be thought of as describing two Value Space objects,
    \c { { /Device/Network/Interfaces/eth0, /Device/Network/Interfaces/ppp0 } },
    each with the six attributes \c { {Name, Type, Status, BytesSent,
    BytesReceived, Time} }.  The QValueSpacePublisher class encapsulates this
    abstraction.

    For performance reasons the setting of and removing of attributes is buffered
    internally by the QValueSpacePublisher and applied as a batch sometime later.
    Normally this occurs the next time the application enters the Qt event loop,
    but this behavior should not be relied upon.  If an application must
    synchronize with others, the QValueSpacePublisher::sync() and QValueSpacePublisher::syncAll()
    functions can be used to force the application of changes.  This call is generally unnecessary,
    and should be used sparingly to prevent unnecessary load on the system.

    \sa QValueSpaceSubscriber
*/

/*!
    \fn void QValueSpacePublisher::interestChanged(const QString &attribute,
                                                   bool interested)

    Signal that is emitted when interest in \a attribute changes.  If \a interested is true at
    least on QValueSpaceSubscriber is interested in the value of \a attribute.
*/

class QValueSpacePublisherPrivate
{
public:
    QValueSpacePublisherPrivate(const QString &_path,
                               QValueSpace::LayerOptions filter = QValueSpace::UnspecifiedLayer);
    QValueSpacePublisherPrivate(const QString &_path, const QUuid &uuid);

    QString path;

    QAbstractValueSpaceLayer *layer;
    QAbstractValueSpaceLayer::Handle handle;

    bool hasSet;
    bool hasWatch;
};

QValueSpacePublisherPrivate::QValueSpacePublisherPrivate(const QString &_path,
                                                       QValueSpace::LayerOptions filter)
:   layer(0), handle(QAbstractValueSpaceLayer::InvalidHandle), hasSet(false), hasWatch(false)
{
    path = qCanonicalPath(_path);

    if ((filter & QValueSpace::PermanentLayer &&
         filter & QValueSpace::TransientLayer) ||
        (filter & QValueSpace::WritableLayer &&
         filter & QValueSpace::ReadOnlyLayer)) {
        return;
    }

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();

    for (int ii = 0; ii < layers.count(); ++ii) {
        if (filter == QValueSpace::UnspecifiedLayer ||
            layers.at(ii)->layerOptions() & filter) {
            QAbstractValueSpaceLayer::Handle h =
                    layers.at(ii)->item(QAbstractValueSpaceLayer::InvalidHandle, path);

            if (h != QAbstractValueSpaceLayer::InvalidHandle) {
                layer = layers.at(ii);
                handle = h;
                break;
            }
        }
    }
}

QValueSpacePublisherPrivate::QValueSpacePublisherPrivate(const QString &_path, const QUuid &uuid)
:   layer(0), handle(QAbstractValueSpaceLayer::InvalidHandle), hasSet(false), hasWatch(false)
{
    path = qCanonicalPath(_path);

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();

    for (int ii = 0; ii < layers.count(); ++ii) {
        if (layers.at(ii)->id() == uuid) {
            layer = layers.at(ii);
            handle = layer->item(QAbstractValueSpaceLayer::InvalidHandle, path);
            break;
        }
    }
}

/*!
    Constructs a QValueSpacePublisher with the specified \a parent that publishes values under
    \a path.
*/
QValueSpacePublisher::QValueSpacePublisher(const QString &path, QObject *parent)
:   QObject(parent), d(new QValueSpacePublisherPrivate(path))
{
    QValueSpaceManager::instance()->init();
}

/*!
    Constructs a QValueSpacePublisher with the specified \a parent that publishes values under
    \a path.  The \a filter parameter is used to limit which layer this QValueSpacePublisher will
    access.

    The constructed Value Space publisher will access the \l {QAbstractValueSpaceLayer}{layer} with
    the highest \l {QAbstractValueSpaceLayer::order()}{order} that matches \a filter and for which
    \a path is a valid path.

    If no suitable \l {QAbstractValueSpaceLayer}{layer} is found, the constructed
    QValueSpacePublisher will be unconnected.

    \sa isConnected()
*/
QValueSpacePublisher::QValueSpacePublisher(QValueSpace::LayerOptions filter,
                                           const QString &path,
                                           QObject *parent)
:   QObject(parent), d(new QValueSpacePublisherPrivate(path, filter))
{
    QValueSpaceManager::instance()->init();
}

/*!
    Constructs a QValueSpacePublisher with the specified \a parent that publishes values under
    \a path.  Only the layer identified by \a uuid will be accessed by this publisher.

    Use of this constructor is not platform agnostic.  If possible use one of the constructors that
    take a QAbstractValueSpaceLayer::LayerOptions parameter instead.

    If a layer with a matching \a uuid is not found, the constructed QValueSpacePublisher will be
    unconnected.

    \sa isConnected()
*/

QValueSpacePublisher::QValueSpacePublisher(const QUuid &uuid, const QString &path, QObject *parent)
:   QObject(parent), d(new QValueSpacePublisherPrivate(path, uuid))
{
    QValueSpaceManager::instance()->init();
}

/*!
    Destroys the QValueSpacePublisher.  This will remove all values published by this publisher in
    \l {QValueSpace::TransientLayer}{non-permanent} layers.
*/
QValueSpacePublisher::~QValueSpacePublisher()
{
    if (!d->layer)
        return;

    if (d->hasSet && !(d->layer->layerOptions() & QValueSpace::PermanentLayer))
        d->layer->removeSubTree(this, d->handle);

    if (d->hasWatch)
        d->layer->removeWatches(this, d->handle);

    delete d;
}

/*!
    Returns the path that this QValueSpacePublisher refers to.
*/
QString QValueSpacePublisher::path() const
{
    return d->path;
}

/*!
    Returns true if this QValueSpacePublisher is connected to an available layer; otherwise returns
    false.
*/
bool QValueSpacePublisher::isConnected() const
{
    return (d->layer && d->handle != QAbstractValueSpaceLayer::InvalidHandle);
}

/*!
    Forcibly sync all Value Space publisher using the same layer as this publisher.

    For performance reasons attribute changes are batched internally by QValueSpacePublisher
    instances.  In cases where the visibility of changes must be synchronized with other processes,
    calling this function will flush these batches.  By the time this function returns, all other
    processes in the system will be able to see the attribute changes.

    Generally, calling this function is unnecessary.
*/
void QValueSpacePublisher::sync()
{
    if (!d || !d->layer)
        return;

    d->layer->sync();
}

/*!
    Sets the value \a name on the publisher to \a data.  If name is empty, this call will set the
    value of this publisher's path.

    For example:

    \code
        QValueSpacePublisher publisher("/Device");
        publisher.setValue("State", "Starting");
        publisher.sync();

        // QValueSpaceSubscriber("/Device/State").value() == QVariant("Starting")
    \endcode
*/
void QValueSpacePublisher::setValue(const QString &name, const QVariant &data)
{
    if (!isConnected()) {
        qWarning("setAttribute called on unconnected QValueSpacePublisher.");
        return;
    }

    d->hasSet = true;
    d->layer->setValue(this, d->handle, qCanonicalPath(name), data);
}

/*!
    Removes the value \a name and all sub-attributes from the system.

    For example:
    \code
        QValueSpacePublisher publisher("/Device");
        publisher.setValue("State", "Starting");
        publisher.setValue("State/Memory", "1000");
        publisher.sync();
        // QValueSpaceSubscriber("/Device/State").value() == QVariant("Starting")
        // QValueSpaceSubscriber("/Device/State/Memory").value() == QVariant("1000")

        publisher.resetValue("State");
        publisher.sync();
        // QValueSpaceSubscriber("/Device/State").value() == QVariant();
        // QValueSpaceSubscriber("/Device/State/Memory").value() == QVariant();
    \endcode
*/
void QValueSpacePublisher::resetValue(const QString &name)
{
    if (!isConnected()) {
        qWarning("removeAttribute called on unconnected QValueSpacePublisher.");
        return;
    }

    d->layer->removeValue(this, d->handle, qCanonicalPath(name));
}

/*!
    \reimp

    Registers this QValueSpacePublisher for notifications when QValueSpaceSubscribers are
    interested in values under path().

    Generally you do not need to call this function as it is automatically called when
    connections are made to this classes signals.  \a member is the signal that has been connected.

    If you reimplement this virtual function it is important that you call this implementation from
    your implementation.

    \sa interestChanged()
*/
void QValueSpacePublisher::connectNotify(const char *member)
{
    if (!d->hasWatch && d->layer && (*member - '0') == QSIGNAL_CODE) {
        d->layer->addWatch(this, d->handle);
        d->hasWatch = true;
    }

    QObject::connectNotify(member);
}

#include "moc_qvaluespacepublisher.cpp"
QTM_END_NAMESPACE
