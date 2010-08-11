/*
* This file is part of TelepathyQt4
*
* Copyright (C) 2008 Collabora Ltd. <http://www.collabora.co.uk/>
* Copyright (C) 2008 Nokia Corporation
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

#include "maemo/types.h"

#include "maemo/types-internal.h"
#include "maemo/interfaces/cli-types.h"


#ifndef HAVE_QDBUSVARIANT_OPERATOR_EQUAL

/* FIXME This is a workaround that should be removed when Qt has support for
*       this. There is already a merge request
*       (http://qt.gitorious.org/qt/qt/merge_requests/1657) in place and the
*       fix should be in next Qt versions.
*/
inline bool operator==(const QDBusVariant &v1, const QDBusVariant &v2)
{ return v1.variant() == v2.variant(); }

#endif

//#include "_gen/types-body.hpp"

//#include "_gen/future-types-body.hpp"

namespace Tp {
    /**
    * \\ingroup types
    * \headerfile TelepathyQt4/types.h "Types"
    *
    * Register the types used by the library with the QtDBus type system.
    *
    * Call this function to register the types used before using anything else in
    * the library.
    */
    void registerTypes()
    {
        qDBusRegisterMetaType<Tp::SUSocketAddress>();

        Tp::_registerTypes();
        //TpFuture::_registerTypes();
    }

    bool operator==(const SUSocketAddress& v1, const SUSocketAddress& v2)
    {
        return ((v1.address == v2.address)
            && (v1.port == v2.port)
            );
    }

    QDBusArgument& operator<<(QDBusArgument& arg, const SUSocketAddress& val)
    {
        arg.beginStructure();
        arg << val.address << val.port;
        arg.endStructure();
        return arg;
    }

    const QDBusArgument& operator>>(const QDBusArgument& arg, SUSocketAddress& val)
    {
        arg.beginStructure();
        arg >> val.address >> val.port;
        arg.endStructure();
        return arg;
    }

}
