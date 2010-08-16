/* Abstract base class for Telepathy interfaces
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

#ifndef _TelepathyQt4_abstract_interface_h_HEADER_GUARD_
#define _TelepathyQt4_abstract_interface_h_HEADER_GUARD_

#include <QtDBus/QDBusAbstractInterface>

namespace Tp
{

class DBusProxy;

class AbstractInterface : public QDBusAbstractInterface
{
    Q_OBJECT
    Q_DISABLE_COPY(AbstractInterface)

public:
    virtual ~AbstractInterface();

    bool isValid() const;
    QString invalidationReason() const;
    QString invalidationMessage() const;

protected Q_SLOTS:
    virtual void invalidate(Tp::DBusProxy *proxy,
            const QString &error, const QString &message);

public:
    AbstractInterface(DBusProxy *parent, const char *interface);
    AbstractInterface(const QString &busName, const QString &path,
            const char *interface, const QDBusConnection &connection,
            QObject *parent);

private:
    struct Private;
    friend struct Private;
    Private *mPriv;
};

} // Tp

#endif
