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

#ifndef QSERVICE_TYPE_REGISTER
#define QSERVICE_TYPE_REGISTER

#include "qmobilityglobal.h"
#include <QList>
#include <QPair>
#include <QDebug>
#include <QHash>

QT_BEGIN_NAMESPACE
class QDataStream;
class QDebug;
QT_END_NAMESPACE

QTM_BEGIN_NAMESPACE

struct QRemoteServiceIdentifier
{
    QByteArray name;
    QByteArray iface;
    QByteArray version;

    QRemoteServiceIdentifier() : name(QByteArray()), iface(QByteArray()), version(QByteArray()) {}

    QRemoteServiceIdentifier(const QByteArray &n, const QByteArray &i, const QByteArray &v) :
        name(n), iface(i), version(v) {}

    QRemoteServiceIdentifier& operator=(const QRemoteServiceIdentifier& other)
    { name = other.name; iface = other.iface; version = other.version; return *this; }

    bool operator==(const QRemoteServiceIdentifier& other) const {
        if ( name == other.name && iface == other.iface && version == other.version ) return true;
        return false;
    }

    inline bool operator!=(const QRemoteServiceIdentifier& other ) { return !operator==(other); }
};

inline uint qHash(const QRemoteServiceIdentifier& key) { 
    return ( qHash(key.name) + qHash(key.iface) + qHash(key.version) );
}

#ifndef QT_NO_DATASTREAM
inline QDataStream& operator>>(QDataStream& s, QRemoteServiceIdentifier& ident) {
    s >> ident.name >> ident.iface >> ident.version;
    return s;
}

inline QDataStream& operator<<(QDataStream& s, const QRemoteServiceIdentifier& ident) {
    s << ident.name << ident.iface << ident.version;
    return s;
}
#endif

#ifndef QT_NO_DEBUG_STREAM
inline QDebug operator<<(QDebug dbg, const QRemoteServiceIdentifier& ident) {
    dbg.nospace() << "QRemoteServiceIdentifier(" 
                  << ident.name << ", "
                  << ident.iface << ", "
                  << ident.version << ")";
    return dbg.space();
}
#endif

class Q_SERVICEFW_EXPORT QRemoteServiceClassRegister 
{
public:
    typedef QObject *(*CreateServiceFunc)();
    typedef QRemoteServiceIdentifier (*TypeIdentFunc)();

    enum InstanceType {
        SharedInstance = 0,  //every new request for service gets same service instance
        UniqueInstance       //every new request for service gets new service instance
    };

    QList<QRemoteServiceIdentifier> types() const;
    const QMetaObject* metaObject(const QRemoteServiceIdentifier& ident) const;

    static bool registerType(const QMetaObject* meta, CreateServiceFunc, TypeIdentFunc, InstanceType instance = UniqueInstance);
    template <class T> static bool registerType( QRemoteServiceClassRegister::InstanceType instanceType = UniqueInstance) 
    {
        return QRemoteServiceClassRegister::registerType(
                &T::staticMetaObject, &T::qt_sfw_create_instance, &T::qt_sfw_type_ident, instanceType);
    }
};

#define Q_SERVICE(T, service, interface, version) \
public:\
    static QObject* qt_sfw_create_instance() \
    { \
        return new T;\
    } \
    static QRemoteServiceIdentifier qt_sfw_type_ident() \
    { \
        return QRemoteServiceIdentifier(service, interface, version); \
    } \
private:

QTM_END_NAMESPACE
#endif // QSERVICE_TYPE_REGISTER

