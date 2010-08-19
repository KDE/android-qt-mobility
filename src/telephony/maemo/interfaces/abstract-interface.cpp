/* Abstract base class for Telepathy D-Bus interfaces.
 *
 * Copyright (C) 2009 Collabora Ltd. <http://www.collabora.co.uk/>
 * Copyright (C) 2009 Nokia Corporation
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "maemo/interfaces/abstract-interface.h"
#include "maemo/dbus-proxy.h"

namespace Tp
{

struct AbstractInterface::Private
{
    Private();
    QString mError;
    QString mMessage;
};

AbstractInterface::Private::Private()
    : mError(), mMessage()
{
}

AbstractInterface::AbstractInterface(const QString &busName,
        const QString &path, const char *interface,
        const QDBusConnection &dbusConnection, QObject *parent)
    : QDBusAbstractInterface(busName, path, interface, dbusConnection, parent),
      mPriv(new Private)
{
}

AbstractInterface::AbstractInterface(DBusProxy *parent, const char *interface)
    : QDBusAbstractInterface(parent->busName(), parent->objectPath(),
            interface, parent->dbusConnection(), parent),
      mPriv(new Private())
{
    connect(parent, SIGNAL(invalidated(Tp::DBusProxy *, const QString &, const QString &)),
            this, SLOT(invalidate(Tp::DBusProxy *, const QString &, const QString &)));
}

AbstractInterface::~AbstractInterface()
{
    delete mPriv;
}

bool AbstractInterface::isValid() const
{
    return QDBusAbstractInterface::isValid() && mPriv->mError.isEmpty();
}

QString AbstractInterface::invalidationReason() const
{
    return mPriv->mError;
}

QString AbstractInterface::invalidationMessage() const
{
    return mPriv->mMessage;
}

void AbstractInterface::invalidate(DBusProxy *proxy,
        const QString &error, const QString &message)
{
    Q_ASSERT(!error.isEmpty());

    if (mPriv->mError.isEmpty()) {
        mPriv->mError = error;
        mPriv->mMessage = message;
    }
}

} // Tp
