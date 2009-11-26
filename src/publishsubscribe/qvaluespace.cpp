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

#include "qvaluespace.h"
#include "qvaluespace_p.h"
#include "qvaluespacemanager_p.h"
#include "qmallocpool_p.h"
#include "qvaluespacepublisher.h"

#include <QObject>
#include <QMap>
#include <QPair>
#include <QCoreApplication>
#include <QSet>
#include <QString>
#include <QVarLengthArray>

#include <QtCore/qdebug.h>

QTM_BEGIN_NAMESPACE

/*!
    \class QAbstractValueSpaceLayer
    \brief The QAbstractValueSpaceLayer class provides support for adding new logical data layers
           to the Qt Value Space.
    \ingroup publishsubscribe

    To create a new layer in the Value Space subclass this class and reimplement all of the virtual
    functions.  The new layer is installed by either calling QValueSpace::installLayer() or by
    adding the QVALUESPACE_AUTO_INSTALL_LAYER() macro in your implementation file.
*/

/*!
    \macro QVALUESPACE_AUTO_INSTALL_LAYER(className)

    \relates QAbstractValueSpaceLayer

    This macro installs new Value Space layer. \a className is the name of the class implementing
    the new layer.

    The method \c {className *className::instance()} must exist and return a pointer to an instance
    of the layer to install.  This method will only be invoked \i {after} QApplication has been
    constructed, making it safe to use any Qt class in your layer's constructor.

    This macro can only be used once for any given class and it should be used where the
    implementation is written rather than in a header file.
*/

/*!
    \typedef QAbstractValueSpaceLayer::Handle

    The Handle type is an opaque, pointer sized contextual handle used to represent paths within
    Value Space layers.  Handles are only ever created by QAbstractValueSpaceLayer::item() and are
    always released by calls to QAbstractValueSpaceLayer::removeHandle().  The special value,
    \c {InvalidHandle} is reserved to represent an invalid handle.
*/

/*!
    \enum QAbstractValueSpaceLayer::Type

    Value Space layers are initialized in either a "Server" or a "Client" context.  There is only
    a single server in the Value Space architecture, and its layers are always initialized before
    any clients.  This distinction allows layers to implement Client/Server architecture
    \i {if required}.  If not, layers are free to treat Server and Client contexts identically.

    \value Server The layer is being initialized in the "server" context.
    \value Client The layer is being initialized in the "client" context.
*/

/*!
    \enum QAbstractValueSpaceLayer::Properties

    To allow for efficient layer implementations, expensive handle operations, currently only
    monitoring for changes, are enabled and disabled as needed on a per-handle basis.  The
    Properties enumeration is a bitmask representing the different properties that can exist on a
    handle.

    \value Publish Enable change notification for the handle.  When set, the layer should emit
                   QAbstractValueSpaceLayer::handleChanged() signals when appropriate for the
                   handle.
*/

/*!
    \fn QString QAbstractValueSpaceLayer::name()

    Returns the name of the Value Space layer.  This name is only used for diagnostics purposes.
*/

/*!
    \fn bool QAbstractValueSpaceLayer::startup(Type type)

    Called by the Value Space system to initialize each layer.  The \a type parameter will be set
    accordingly, and layer implementors can use this to implement a client/server architecture if
    desired.

    Returns true upon success; otherwise returns false.
*/

/*!
    \fn QUuid QAbstractValueSpaceLayer::id()

    Returns a globally unique identifier for the layer.  This id is used to break ordering ties.
*/

/*!
    \fn unsigned int QAbstractValueSpaceLayer::order()

    Return the position in the Value Space layer stack that this layer should reside.  Higher
    numbers mean the layer has a higher precedence and its values will "shadow" those below it.
    If two layers specify the same ordering, the id() value is used to break the tie.
*/

/*!
    \fn Handle QAbstractValueSpaceLayer::item(Handle parent, const QString &subPath)

    Returns a new opaque handle for the requested \a subPath of \a parent.  If \a parent is an
    InvalidHandle, \a subPath is interpreted as an absolute path.

    The caller should call removeHandle() to free resources used by the handle when it is no longer
    required.
*/

/*!
    \fn void QAbstractValueSpaceLayer::removeHandle(Handle handle)

    Releases a \a handle previously returned from QAbstractValueSpaceLayer::item().
*/

/*!
    \fn void QAbstractValueSpaceLayer::setProperty(Handle handle, Properties property)

    Apply the specified \a property mask to \a handle.
*/

/*!
    \fn bool QAbstractValueSpaceLayer::value(Handle handle, QVariant *data)

    Returns the value for a particular \a handle.  If a value is available, the layer will set
    \a data and return true.  If no value is available, false is returned.
*/

/*!
    \fn bool QAbstractValueSpaceLayer::value(Handle handle, const QString &subPath, QVariant *data)

    Returns the value for a particular \a subPath of \a handle.  If a value is available, the
    layer will set \a data and return true.  If no value is available, false is returned.
*/

/*!
    \fn QSet<QString> QAbstractValueSpaceLayer::children(Handle handle)

    Returns the set of children of \a handle.  For example, in a layer providing the following
    items:

    \code
        /Device/Configuration/Applications/FocusedApplication
        /Device/Configuration/Buttons/PrimaryInput
        /Device/Configuration/Name
    \endcode

    a request for children of "/Device/Configuration" will return
    { "Applications", "Buttons", "Name" }.
*/

/*!
    \fn QValueSpace::LayerOptions QAbstractValueSpaceLayer::layerOptions() const

    Returns the QValueSpace::LayerOptions describing this layer.

    \sa QValueSpace::LayerOption
*/

/*!
    \fn bool QAbstractValueSpaceLayer::supportsInterestNotification() const

    Returns true if the layer supports interest notifications; otherwise returns false.
*/

/*!
    \fn bool QAbstractValueSpaceLayer::notifyInterest(Handle handle, bool interested)

    Registers or unregisters that the caller is interested in \a handle and any subpaths under it.
    If \a interested is true interest in \a handle is registered; otherwise it is unregistered.

    The caller should ensure that all calls to this function with \a interested set to true have a
    matching call with \a interested set to false.

    Returns true if the notification was successfully sent; otherwise returns false.
*/

/*!
    \fn bool QAbstractValueSpaceLayer::setValue(QValueSpacePublisher *creator, Handle handle,
                                                const QString &subPath, const QVariant &value)

    Process calls to QValueSpacePublisher::setValue() by setting the value specified by the
    \a subPath under \a handle to \a value.  Ownership of the Value Space item is assigned to
    \a creator.

    Returns true on success; otherwise returns false.
*/

/*!
    \fn bool QAbstractValueSpaceLayer::removeValue(QValueSpacePublisher *creator, Handle handle,
                                                   const QString &subPath)

    Process calls to QValueSpacePublisher::resetValue() by removing the Value Space item
    identified by \a handle and \a subPath and created by \a creator.

    Returns true on success; otherwise returns false.
*/

/*!
    \fn bool QAbstractValueSpaceLayer::removeSubTree(QValueSpacePublisher *creator, Handle handle)

    Process calls to QValueSpacePublisher::~QValueSpacePublisher() by removing the entire sub tree
    created by \a creator under \a handle.

    Returns true on success; otherwise returns false.
*/

/*!
    \fn void QAbstractValueSpaceLayer::addWatch(QValueSpacePublisher *creator, Handle handle)

    Registers \a creator for change notifications to values under \a handle.

    \sa removeWatches()
*/

/*!
    \fn void QAbstractValueSpaceLayer::removeWatches(QValueSpacePublisher *creator, Handle parent)

    Removes all registered change notifications for \a creator under \a parent.

    \sa addWatch()
*/

/*!
    \fn void QAbstractValueSpaceLayer::sync()

    Flushes all pending changes made by calls to setValue(), removeValue() and removeSubTree().
*/

/*!
    Emits the QValueSpacePublisher::interestChanged() signal on \a publisher with \a path
    and \a interested.
*/
void QAbstractValueSpaceLayer::emitInterestChanged(QValueSpacePublisher *publisher,
                                                   const QString &path,
                                                   bool interested)
{
    emit publisher->interestChanged(path, interested);
}

/*!
    \fn void QAbstractValueSpaceLayer::handleChanged(quintptr handle)

    Emitted whenever the \a handle's value, or any sub value, changes.
*/


/*!
    \namespace QValueSpace
    \brief The QValueSpace namespace contains miscellaneous identifiers used throughtout the
           Publish and Subscribe API.
    \ingroup publishsubscribe
*/

/*!
    \enum QValueSpace::LayerOption

    This enum describes the behaviour of the Value Space layer.  In addition this enum is used as
    a filter when constructing a QValueSpacePublisher or QValueSpaceSubscriber.

    \value UnspecifiedLayer     Used as a filter to specify that any layer should be used.
    \value PermanentLayer       Indicates that the layer uses a permanent backing store.  When used
                                as a filter only layers that use a permanent backing store will be
                                used.
                                \br
                                Values stored in a layer with this option will persist with in the
                                layer after the QValueSpacePublisher that published them is
                                destroyed.  Whether the value persists in the layer after the
                                server or device is restarted is system dependent.
                                \br
                                This option and the TransientLayer option are mutually
                                exclusive.
    \value TransientLayer       Indicates that the layer does not use a permanent backing store.
                                When used as a filter only layers that do not use permanent backing
                                stores will be used.
                                \br
                                Values stored in a layer with this option will be removed when the
                                QValueSpacePublisher that published them is destroyed.
                                \br
                                This option and the PermanentLayer option are mutually exclusive.
    \value WritableLayer        Indicates that the layer can update its contents.  When used as a
                                filter only layers that are writable will be used.
                                \br
                                Applications can use QValueSpacePublisher to publish values to
                                layers that have this option.
                                \br
                                This option and the ReadOnlyLayer option are mutually exclusive.
    \value ReadOnlyLayer        Indicates that the layer cannot update its contents.  When used as
                                a filter only layers that are read-only will be used.
                                \br
                                Applications can not publish values to layers with this option.
                                \br
                                This option and the WritableLayer option are mutually exclusive.
*/

/*!
    \typedef QValueSpace::LayerCreateFunc
    \internal

    Support type used by the QVALUESPACE_AUTO_INSTALL_LAYER() macro.
*/

/*!
    \class QValueSpace::AutoInstall
    \internal

    Support class used by the QVALUESPACE_AUTO_INSTALL_LAYER() macro.
*/

/*!
    \fn QValueSpace::AutoInstall::AutoInstall(LayerCreateFunc func)

    Installs the Value Space layer at static construction time by calling the layer creation
    function \a func.
*/

/*!
    Initialize the Value Space manager as the server.  This method only needs to be called by the
    process acting as the server and should be called before any process in the system uses a value
    space class.
*/
void QValueSpace::initValueSpaceServer()
{
    QValueSpaceManager::instance()->initServer();
}

/*!
    Used by Value Space layer implementations to install themselves into the system.  \a layer
    should be a pointer to the layer to install.

    \sa QVALUESPACE_AUTO_INSTALL_LAYER()
*/
void QValueSpace::installLayer(QAbstractValueSpaceLayer *layer)
{
    QValueSpaceManager::instance()->install(layer);
}

/*!
    \internal

    Called by the QVALUESPACE_AUTO_INSTALL_LAYER() macro to install the layer at static
    initialization time.
*/
void QValueSpace::installLayer(LayerCreateFunc func)
{
    QValueSpaceManager::instance()->install(func);
}

/*!
    \macro QVALUESPACE_SHAREDMEMORY_LAYER
    \relates QValueSpace

    The UUID of the Shared Memory layer as a QUuid.  The actual UUID value is
    {d81199c1-6f60-4432-934e-0ce4d37ef252}.

    This value can be passed to the constructor of QValueSpacePublisher or QValueSpaceSubscriber to
    force the constructed object to only access the Shared Memory layer.

    You can test if the Shared Memory layer is available by checking if the list returned by
    QValueSpace::availableLayers() contains this value.
*/

/*!
    \macro QVALUESPACE_VOLATILEREGISTRY_LAYER
    \relates QValueSpace

    The UUID of the Volatile Registry layer as a QUuid.  The actual UUID value is
    {8ceb5811-4968-470f-8fc2-264767e0bbd9}.

    This value can be passed to the constructor of QValueSpacePublisher or QValueSpaceSubscriber to
    force the constructed object to only access the Volatile Registry layer.

    You can test if the Volatile Registry layer is available by checking if the list returned by
    QValueSpace::availableLayers() contains this value.  The Volatile Registry layer is only
    available on Windows platforms.
*/

/*!
    \macro QVALUESPACE_NONVOLATILEREGISTRY_LAYER
    \relates QValueSpace

    The UUID of the Non-Volatile Registry layer as a QUuid.  The actual UUID value is
    {8e29561c-a0f0-4e89-ba56-080664abc017}.

    This value can be passed to the constructor of QValueSpacePublisher or QValueSpaceSubscriber to
    force the constructed object to only access the Non-Volatile Registry layer.

    You can test if the Non-Volatile Registry layer is available by checking if the list returned
    by QValueSpace::availableLayers() contains this value.  The Non-Volatile Registry layer is only
    available on Windows platforms.
*/

/*!
    \macro QVALUESPACE_CONTEXTKIT_LAYER
    \relates QValueSpace

    The UUID of the ContextKit layer as a QUuid.  The actual UUID values is
    {2c769b9e-d949-4cd1-848f-d32241fe07ff}.

    This value can be passed to the constructor of QValueSpacePublisher or QValueSpaceSubscriber to
    force the constructed object to only access the ContextKit layer.

    You can test if the ContextKit layer is available by checking if the list returned by
    QValueSpace::availableLayers() contains this value.
*/

/*!
    \macro QVALUESPACE_SYMBIAN_SETTINGS_LAYER
    \relates QValueSpace

    The UUID of the Symbian Settings layer as a QUuid.  The actual UUID value is
    {40d7b059-66ac-442f-b222-9c8ab98b9c2d}.

    This value can be passed to the constructor of QValueSpacePublisher or QValueSpaceSubscriber to
    force the constructed object to only access the Symbian Settings layer.

    You can test if the Symbian Settings layer is available by checking if the list returned by
    QValueSpace::availableLayers() contains this value.
*/

/*!
    Returns a list of QUuids of all of the available layers.
*/
QList<QUuid> QValueSpace::availableLayers()
{
    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();

    QList<QUuid> uuids;

    for (int i = 0; i < layers.count(); ++i)
        uuids.append(layers.at(i)->id());

    return uuids;
}

/*!
    \internal
    \ingroup publishsubscribe

    Returns \a path with all duplicate '/' characters removed.
*/
QString qCanonicalPath(const QString &path)
{
    QString result;
    result.resize(path.length());
    const QChar *from = path.constData();
    const QChar *fromend = from + path.length();
    int outc=0;
    QChar *to = result.data();
    do {
        to[outc++] = QLatin1Char('/');
        while (from!=fromend && *from == QLatin1Char('/'))
            ++from;
        while (from!=fromend && *from != QLatin1Char('/'))
            to[outc++] = *from++;
    } while (from != fromend);
    if (outc > 1 && to[outc-1] == QLatin1Char('/'))
        --outc;
    result.resize(outc);
    return result;
}

#include "moc_qvaluespace_p.cpp"

QTM_END_NAMESPACE
