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

#ifndef QORGANIZERITEM_H
#define QORGANIZERITEM_H

#include <QVariant>
#include <QString>
#include <QPair>
#include <QMultiMap>
#include <QList>
#include <QDateTime>
#include <QSharedDataPointer>

#include "qorganizeritemid.h"
#include "qorganizeritemdetail.h"
#include "qorganizeritemtype.h"
#include "qorganizeritemdisplaylabel.h"
#include "qorganizeritemdescription.h"
#include "qorganizercollection.h"

class QDataStream;

QTM_BEGIN_NAMESPACE

class QOrganizerManager;
class QOrganizerItemData;
class QOrganizerItemName;

// MSVC needs the function declared before the friend declaration
class QOrganizerItem;
#ifndef QT_NO_DATASTREAM
Q_ORGANIZER_EXPORT QDataStream& operator<<(QDataStream& out, const QOrganizerItem& item);
Q_ORGANIZER_EXPORT QDataStream& operator>>(QDataStream& in, QOrganizerItem& item);
#endif

class Q_ORGANIZER_EXPORT QOrganizerItem
{
public:
    QOrganizerItem();

    ~QOrganizerItem();

    QOrganizerItem(const QOrganizerItem& other);
    QOrganizerItem& operator=(const QOrganizerItem& other);

    bool operator==(const QOrganizerItem &other) const;
    bool operator!=(const QOrganizerItem &other) const {return !(other == *this);}

    /* Unique ID */
    QOrganizerItemId id() const;
    void setId(const QOrganizerItemId& id);

    /* The collection to which an item belongs - read only */
    QOrganizerCollectionId collectionId() const;
    void setCollectionId(const QOrganizerCollectionId& collectionId);

    /* Is this an empty organizer item? */
    bool isEmpty() const;
    void clearDetails();

    QOrganizerItemDetail detail(const QString& definitionId) const;
    QList<QOrganizerItemDetail> details(const QString& definitionId = QString()) const;

    QList<QOrganizerItemDetail> details(const QString& definitionName, const QString& fieldName, const QString& value) const;

    QOrganizerItemDetail detail(const char* definitionId) const;
    QList<QOrganizerItemDetail> details(const char* definitionId) const;
    QList<QOrganizerItemDetail> details(const char* definitionId, const char* fieldName, const QString& value) const;

    /* Templated retrieval for definition names */
#ifdef Q_QDOC
    QOrganizerItemDetail detail(const QLatin1Constant& definitionName) const;
    QList<QOrganizerItemDetail> details(const QLatin1Constant& definitionName) const;
    QList<QOrganizerItemDetail> details(const QLatin1Constant& definitionName, const QLatin1Constant& fieldName, const QString& value);
#else
    template <int N> QOrganizerItemDetail detail(const QLatin1Constant<N>& definitionName) const
    {
        return detail(definitionName.latin1());
    }
    template <int N> QList<QOrganizerItemDetail> details(const QLatin1Constant<N>& definitionName) const
    {
        return details(definitionName.latin1());
    }
    template <int N, int M> QList<QOrganizerItemDetail> details(const QLatin1Constant<N>& definitionName, const QLatin1Constant<M>& fieldName, const QString& value)
    {
        return details(definitionName.latin1(), fieldName.latin1(), value);
    }
#endif

    /* Templated (type-specific) detail retrieval */
    template<typename T> QList<T> details() const
    {
        QList<QOrganizerItemDetail> props = details(T::DefinitionName.latin1());
        QList<T> ret;
        for (int i = 0; i < props.count(); i++)
            ret.append(T(props.at(i)));
        return ret;
    }

    /* Templated (type-specific) detail retrieval base on given detail field name and field value */
    template<typename T> QList<T> details(const QString& fieldName, const QString& value) const
    {
        QList<QOrganizerItemDetail> props = details(T::DefinitionName, fieldName, value);
        QList<T> ret;
        for (int i = 0; i < props.count(); i++)
            ret.append(T(props.at(i)));
        return ret;
    }

    template<typename T> QList<T> details(const char* fieldName, const QString& value) const
    {
        QList<QOrganizerItemDetail> props = details(T::DefinitionName.latin1(), fieldName, value);
        QList<T> ret;
        for (int i = 0; i < props.count(); i++)
            ret.append(T(props.at(i)));
        return ret;
    }

    template<typename T> T detail() const
    {
        return T(detail(T::DefinitionName.latin1()));
    }

    /* generic detail addition/removal functions */
    bool saveDetail(QOrganizerItemDetail* detail);   // modifies the detail - sets its ID if detail already exists
    bool removeDetail(QOrganizerItemDetail* detail); // modifies the detail - unsets its ID

    // Some common convenience detail accessors
    // has to be things that all subclasses (including Occurrences) have.

    /* Type - event, todo, journal, note... */
    QString type() const;
    void setType(const QString& type);
    void setType(const QOrganizerItemType& type);

    /* The display label of the organizer item */
    QString displayLabel() const;
    void setDisplayLabel(const QString& label);
    void setDisplayLabel(const QOrganizerItemDisplayLabel& label);

    /* The description of the organizer item */
    QString description() const;
    void setDescription(const QString& description);
    void setDescription(const QOrganizerItemDescription& description);

    QStringList comments() const;
    void clearComments();
    void addComment(const QString& comment);

    QString guid() const;
    void setGuid(const QString& guid);

protected:
    explicit QOrganizerItem(const char* type);
    QOrganizerItem(const QOrganizerItem &other, const char* expectedType);
    QOrganizerItem& assign(const QOrganizerItem &other, const char* expectedType);

protected:
    friend class QOrganizerItemData;
    friend class QOrganizerManager;
    friend class QOrganizerManagerData;
    friend class QOrganizerManagerEngine;
    Q_ORGANIZER_EXPORT friend QDataStream& operator<<(QDataStream& out, const QOrganizerItem& item);
    Q_ORGANIZER_EXPORT friend QDataStream& operator>>(QDataStream& in, QOrganizerItem& item);

    QSharedDataPointer<QOrganizerItemData> d;
};

Q_ORGANIZER_EXPORT uint qHash(const QOrganizerItem& key);
#ifndef QT_NO_DEBUG_STREAM
Q_ORGANIZER_EXPORT QDebug operator<<(QDebug dbg, const QOrganizerItem& organizeritem);
#endif

#define Q_DECLARE_CUSTOM_ORGANIZER_ITEM(className, typeConstant) \
    className() : QOrganizerItem(typeConstant.latin1()) {} \
    className(const QOrganizerItem& other) : QOrganizerItem(other, typeConstant.latin1()) {} \
    className& operator=(const QOrganizerItem& other) {assign(other, typeConstant.latin1()); return *this;}

QTM_END_NAMESPACE

Q_DECLARE_TYPEINFO(QTM_PREPEND_NAMESPACE(QOrganizerItem), Q_MOVABLE_TYPE);


#endif

