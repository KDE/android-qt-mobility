/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/


#ifndef QORGANIZERITEMDETAIL_H
#define QORGANIZERITEMDETAIL_H

#include "qtorganizerglobal.h"

#include <QSharedDataPointer>
#include <QStringList>
#include <QVariant>

class QDataStream;

QTM_BEGIN_NAMESPACE

class QOrganizerItemDetailPrivate;

// MSVC needs the function declared before the friend declaration
class QOrganizerItemDetail;
#ifndef QT_NO_DATASTREAM
Q_ORGANIZER_EXPORT QDataStream& operator<<(QDataStream& out, const QOrganizerItemDetail& detail);
Q_ORGANIZER_EXPORT QDataStream& operator>>(QDataStream& in, QOrganizerItemDetail& detail);
#endif

class Q_ORGANIZER_EXPORT QOrganizerItemDetail
{
public:
    QOrganizerItemDetail();
    explicit QOrganizerItemDetail(const char* definitionName); // possibly internal
    explicit QOrganizerItemDetail(const QString& definitionName);
    ~QOrganizerItemDetail();

    QOrganizerItemDetail(const QOrganizerItemDetail& other);
    QOrganizerItemDetail& operator=(const QOrganizerItemDetail& other);

    enum AccessConstraint {
        NoConstraint = 0,
        ReadOnly = 0x01,
        Irremovable = 0x02
    };
    Q_DECLARE_FLAGS(AccessConstraints, AccessConstraint)

    AccessConstraints accessConstraints() const;

    bool operator==(const QOrganizerItemDetail& other) const;
    bool operator!=(const QOrganizerItemDetail& other) const {return !(other == *this);}

    QString definitionName() const;
    bool isEmpty() const;

    int key() const;
    void resetKey();

    QString value(const QString& key) const;
    bool setValue(const QString& key, const QVariant& value);
    bool removeValue(const QString& key);
    bool hasValue(const QString& key) const;

    QVariantMap variantValues() const;
    QVariant variantValue(const QString& key) const;
    template <typename T> T value(const QString& key) const
    {
        return variantValue(key).value<T>();
    }

    /* These are probably internal */
    QString value(const char* key) const;
    bool setValue(const char* key, const QVariant& value);
    bool removeValue(const char* key);
    bool hasValue(const char* key) const;
    QVariant variantValue(const char *key) const;
    template<typename T> T value(const char *key) const
    {
        return variantValue(key).value<T>();
    }
#ifdef Q_QDOC
    QString value(const QLatin1Constant& key) const;
    bool setValue(const QLatin1Constant& key, const QVariant& value);
    bool removeValue(const QLatin1Constant& key);
    bool hasValue(const QLatin1Constant& key) const;
    QVariant variantValue(const QLatin1Constant& key) const;
    T value(const QLatin1Constant& key) const;
#else
    template<int N> QString value(const QLatin1Constant<N>& key) const
    {
        return value(key.latin1());
    }
    template<int N> bool setValue(const QLatin1Constant<N>& key, const QVariant& value)
    {
        return setValue(key.latin1(), value);
    }
    template<int N> bool removeValue(const QLatin1Constant<N>& key)
    {
        return removeValue(key.latin1());
    }
    template<int N> bool hasValue(const QLatin1Constant<N>& key) const
    {
        return hasValue(key.latin1());
    }
    template<int N> QVariant variantValue(const QLatin1Constant<N>& key) const
    {
        return variantValue(key.latin1());
    }
    template<typename T, int N> T value(const QLatin1Constant<N>& key) const
    {
        return value<T>(key.latin1());
    }
#endif

protected:
    QOrganizerItemDetail(const QOrganizerItemDetail &other, const QString& expectedDefinitionId);
    QOrganizerItemDetail& assign(const QOrganizerItemDetail &other, const QString& expectedDefinitionId);
    QOrganizerItemDetail(const QOrganizerItemDetail &other, const char* expectedDefinitionId);
    QOrganizerItemDetail& assign(const QOrganizerItemDetail &other, const char* expectedDefinitionId);

private:
    friend class QOrganizerItem;
    friend class QOrganizerItemDetailPrivate;
#ifndef QT_NO_DATASTREAM
    Q_ORGANIZER_EXPORT friend QDataStream& operator>>(QDataStream& in, QOrganizerItemDetail& detail);
#endif
    QSharedDataPointer<QOrganizerItemDetailPrivate> d;
};

Q_ORGANIZER_EXPORT uint qHash(const QOrganizerItemDetail& key);
#ifndef QT_NO_DEBUG_STREAM
Q_ORGANIZER_EXPORT QDebug operator<<(QDebug dbg, const QOrganizerItemDetail& detail);
#endif

Q_DECLARE_OPERATORS_FOR_FLAGS(QOrganizerItemDetail::AccessConstraints);

#define Q_DECLARE_CUSTOM_ORGANIZER_DETAIL(className, definitionNameString) \
    className() : QOrganizerItemDetail(DefinitionName.latin1()) {} \
    className(const QOrganizerItemDetail& field) : QOrganizerItemDetail(field, DefinitionName.latin1()) {} \
    className& operator=(const QOrganizerItemDetail& other) {assign(other, DefinitionName.latin1()); return *this;} \
    \
    Q_DECLARE_LATIN1_CONSTANT(DefinitionName, definitionNameString);

#define Q_IMPLEMENT_CUSTOM_ORGANIZER_DETAIL(className, definitionNameString) \
    Q_DEFINE_LATIN1_CONSTANT(className::DefinitionName, definitionNameString)

QTM_END_NAMESPACE

Q_DECLARE_TYPEINFO(QTM_PREPEND_NAMESPACE(QOrganizerItemDetail), Q_MOVABLE_TYPE);


#endif

