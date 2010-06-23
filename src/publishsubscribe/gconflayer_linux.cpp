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

#include "gconflayer_linux_p.h"
#include <QVariant>

QTM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(GConfLayer, gConfLayer);
QVALUESPACE_AUTO_INSTALL_LAYER(GConfLayer);

GConfLayer::GConfLayer()
{
    GConfItem *gconfItem = new GConfItem("/", true, this);
    connect(gconfItem, SIGNAL(subtreeChanged(const QString &, const QVariant &)), this, SLOT(notifyChanged(const QString &, const QVariant &)));
}

GConfLayer::~GConfLayer()
{
    QMutableHashIterator<QString, GConfHandle *> i(m_handles);
    while (i.hasNext()) {
        i.next();

        doRemoveHandle(Handle(i.value()));
    }
}

QString GConfLayer::name()
{
    return QLatin1String("GConf Layer");
}

QUuid GConfLayer::id()
{
    return QVALUESPACE_GCONF_LAYER;
}

unsigned int GConfLayer::order()
{
    return 0;
}

QValueSpace::LayerOptions GConfLayer::layerOptions() const
{
    return QValueSpace::PermanentLayer |
        QValueSpace::WritableLayer;
}

GConfLayer *GConfLayer::instance()
{
    return gConfLayer();
}

bool GConfLayer::startup(Type /*type*/)
{
    return true;
}

bool GConfLayer::value(Handle handle, QVariant *data)
{
    QMutexLocker locker(&m_mutex);
    GConfHandle *sh = gConfHandle(handle);
    if (!sh)
        return false;

    return getValue(InvalidHandle, sh->path, data);
}

bool GConfLayer::value(Handle handle, const QString &subPath, QVariant *data)
{
    QMutexLocker locker(&m_mutex);
    return getValue(handle, subPath, data);
}

bool GConfLayer::getValue(Handle handle, const QString &subPath, QVariant *data)
{
    if (handle != InvalidHandle && !gConfHandle(handle))
        return false;

    QString path(subPath);
    while (path.endsWith(QLatin1Char('/')))
        path.chop(1);
    if (handle != InvalidHandle)
        while (path.startsWith(QLatin1Char('/')))
            path = path.mid(1);
    int index = path.lastIndexOf(QLatin1Char('/'), -1);

    bool createdHandle = false;

    QString value;
    if (index == -1) {
        value = path;
    } else {
        // want a value that is in a sub path under handle
        value = path.mid(index + 1);
        path.truncate(index);

        handle = getItem(handle, path);
        createdHandle = true;
    }

    GConfHandle *sh = gConfHandle(handle);
    if (!sh)
        return false;

    QString fullPath(sh->path);
    if (fullPath != QLatin1String("/") && !value.isEmpty())
        fullPath.append(QLatin1Char('/'));

    fullPath.append(value);

    GConfItem gconfItem(fullPath);
    QVariant readValue = gconfItem.value();
    switch (readValue.type()) {
    case QVariant::Invalid:
    case QVariant::Bool:
    case QVariant::Int:
    case QVariant::Double:
    case QVariant::StringList:
    case QVariant::List:
        *data = readValue;
        break;
    case QVariant::String:
    {
        QString readString = readValue.toString();
        QDataStream readStream(QByteArray::fromBase64(readString.toAscii()));
        QVariant serializedValue;
        readStream >> serializedValue;
        if (serializedValue.isValid()) {
            *data = serializedValue;
        } else {
            *data = readValue;
        }
        break;
    }
    default:
        break;
    }

    if (createdHandle)
        doRemoveHandle(handle);
    return data->isValid();
}

QSet<QString> GConfLayer::children(Handle handle)
{
    QMutexLocker locker(&m_mutex);

    GConfHandle *sh = gConfHandle(handle);
    if (!sh)
        return QSet<QString>();

    GConfItem gconfItem(sh->path);

    QSet<QString> ret;
    foreach (const QString &child, gconfItem.listEntries() + gconfItem.listDirs()) {
        const int index = child.lastIndexOf(QLatin1Char('/'), -1);
        ret += child.mid(index + 1);
    }

    return ret;
}

QAbstractValueSpaceLayer::Handle GConfLayer::item(Handle parent, const QString &subPath)
{
    QMutexLocker locker(&m_mutex);
    return getItem(parent, subPath);
}

QAbstractValueSpaceLayer::Handle GConfLayer::getItem(Handle parent, const QString &subPath)
{
    QString fullPath;

    // Fail on invalid path.
    if (subPath.isEmpty() || subPath.contains(QLatin1String("//")))
        return InvalidHandle;

    if (parent == InvalidHandle) {
        fullPath = subPath;
    } else {
        GConfHandle *sh = gConfHandle(parent);
        if (!sh)
            return InvalidHandle;

        if (subPath == QLatin1String("/")) {
            fullPath = sh->path;
        } else if (sh->path.endsWith(QLatin1Char('/')) && subPath.startsWith(QLatin1Char('/')))
            fullPath = sh->path + subPath.mid(1);
        else if (!sh->path.endsWith(QLatin1Char('/')) && !subPath.startsWith(QLatin1Char('/')))
            fullPath = sh->path + QLatin1Char('/') + subPath;
        else
            fullPath = sh->path + subPath;
    }

    if (m_handles.contains(fullPath)) {
        GConfHandle *sh = m_handles.value(fullPath);
        ++sh->refCount;
        return Handle(sh);
    }

    // Create a new handle for path
    GConfHandle *sh = new GConfHandle(fullPath);
    m_handles.insert(fullPath, sh);

    return Handle(sh);
}

void GConfLayer::setProperty(Handle handle, Properties properties)
{
    QMutexLocker locker(&m_mutex);

    GConfHandle *sh = gConfHandle(handle);
    if (!sh)
        return;
    QString basePath = sh->path;
    if (!basePath.endsWith(QLatin1Char('/'))) {
        basePath += QLatin1Char('/');
    }

    if (properties & QAbstractValueSpaceLayer::Publish) {
        m_monitoringHandles.insert(sh);
    } else {
        m_monitoringHandles.remove(sh);
    }
}

void GConfLayer::removeHandle(Handle handle)
{
    QMutexLocker locker(&m_mutex);
    doRemoveHandle(handle);
}

void GConfLayer::doRemoveHandle(Handle handle)
{
    GConfHandle *sh = gConfHandle(handle);
    if (!sh)
        return;

    if (--sh->refCount)
        return;

    m_monitoringHandles.remove(sh);
    m_handles.remove(sh->path);

    delete sh;
}

bool GConfLayer::setValue(QValueSpacePublisher */*creator*/,
                                    Handle handle,
                                    const QString &subPath,
                                    const QVariant &data)
{
    QMutexLocker locker(&m_mutex);

    GConfHandle *sh = gConfHandle(handle);
    if (!sh)
        return false;

    QString path(subPath);
    while (path.endsWith(QLatin1Char('/')))
        path.chop(1);

    int index = path.lastIndexOf(QLatin1Char('/'), -1);

    bool createdHandle = false;

    QString value;
    if (index == -1) {
        value = path;
    } else {
        // want a value that is in a sub path under handle
        value = path.mid(index + 1);
        path.truncate(index);

        if (path.isEmpty())
            path.append(QLatin1Char('/'));

        sh = gConfHandle(getItem(Handle(sh), path));
        createdHandle = true;
    }

    QString fullPath(sh->path);
    if (fullPath != QLatin1String("/"))
        fullPath.append(QLatin1Char('/'));

    fullPath.append(value);

    GConfItem gconfItem(fullPath);
    switch (data.type()) {
    case QVariant::Invalid:
    case QVariant::Bool:
    case QVariant::Int:
    case QVariant::Double:
    case QVariant::String:
    case QVariant::StringList:
    case QVariant::List:
        gconfItem.set(data);
        break;
    default:
        QByteArray byteArray;
        QDataStream writeStream(&byteArray, QIODevice::WriteOnly);
        writeStream << data;
        QString serializedValue(byteArray.toBase64());
        gconfItem.set(serializedValue);
    }

    if (createdHandle)
        doRemoveHandle(Handle(sh));

    return true;
}

void GConfLayer::sync()
{
    //Not needed
}

bool GConfLayer::removeSubTree(QValueSpacePublisher * /*creator*/, Handle /*handle*/)
{
    //Not needed
    return false;
}

bool GConfLayer::removeValue(QValueSpacePublisher */*creator*/,
    Handle handle,
    const QString &subPath)
{
    QMutexLocker locker(&m_mutex);

    QString fullPath;

    GConfHandle *sh = gConfHandle(handle);
    if (!sh)
        return false;

    if (handle == InvalidHandle) {
        fullPath = subPath;
    } else {
        if (subPath == QLatin1String("/"))
            fullPath = sh->path;
        else if (sh->path.endsWith(QLatin1Char('/')) && subPath.startsWith(QLatin1Char('/')))
            fullPath = sh->path + subPath.mid(1);
        else if (!sh->path.endsWith(QLatin1Char('/')) && !subPath.startsWith(QLatin1Char('/')))
            fullPath = sh->path + QLatin1Char('/') + subPath;
        else
            fullPath = sh->path + subPath;
    }

    GConfItem gconfItem(fullPath);
    gconfItem.unset();

    return true;
}

void GConfLayer::addWatch(QValueSpacePublisher *, Handle)
{
    //Not needed
}

void GConfLayer::removeWatches(QValueSpacePublisher *, Handle)
{
    //Not needed
}

bool GConfLayer::supportsInterestNotification() const
{
    return false;
}

bool GConfLayer::notifyInterest(Handle, bool)
{
    //Not needed
    return false;
}

void GConfLayer::notifyChanged(const QString &key, const QVariant & /*value*/)
{
    foreach (GConfHandle *handle, m_monitoringHandles.values()) {
        if (key.startsWith(handle->path)) {
            emit handleChanged(Handle(handle));
        }
    }
}

#include "moc_gconflayer_linux_p.cpp"

QTM_END_NAMESPACE
