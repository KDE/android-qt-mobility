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

#include "qgalleryfilter.h"

#include <QtCore/qdebug.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qurl.h>

QTM_BEGIN_NAMESPACE

class QGalleryFilterPrivate : public QSharedData
{
public:
    QGalleryFilterPrivate(QGalleryFilter::Type type)
        : type(type)
    {
    }


    virtual ~QGalleryFilterPrivate() {}

    virtual bool isEqual(const QGalleryFilterPrivate &other) const = 0;

#ifndef QT_NO_DEBUG_STREAM
    virtual void printDebug(QDebug &debug) const = 0;
#endif

    const QGalleryFilter::Type type;

protected:
    QGalleryFilterPrivate(const QGalleryFilterPrivate &other)
        : QSharedData(other)
        , type(other.type)
    {
    }
};

class QGalleryInvalidFilterPrivate : public QGalleryFilterPrivate
{
public:
    QGalleryInvalidFilterPrivate() : QGalleryFilterPrivate(QGalleryFilter::Invalid) {}

    bool isEqual(const QGalleryFilterPrivate &other) const { return type == other.type; }

#ifndef QT_NO_DEBUG_STREAM
    void printDebug(QDebug &debug) const { debug << "QGalleryFilter()"; }
#endif
};

class QGalleryIntersectionFilterPrivate : public QGalleryFilterPrivate
{
public:
    QGalleryIntersectionFilterPrivate(QGalleryFilter::Type type = QGalleryFilter::Intersection)
        : QGalleryFilterPrivate(type)
    {
    }

    QGalleryIntersectionFilterPrivate(const QGalleryIntersectionFilterPrivate &other)
        : QGalleryFilterPrivate(other)
        , filters(other.filters)
    {
    }

    bool isEqual(const QGalleryFilterPrivate &other) const
    {
        return other.type == type && static_cast<const QGalleryIntersectionFilterPrivate &>
                (other).filters == filters;
    }

#ifndef QT_NO_DEBUG_STREAM
    void printDebug(QDebug &debug) const
    {
        debug << "QGalleryIntersectionFilter(";
        QList<QGalleryFilter>::const_iterator filter = filters.begin();
        if (filter != filters.end()) {
            debug << *filter;

            while (++filter != filters.end())
                debug << " ||" << *filter;
        }
        debug << ")";
    }
#endif

    QList<QGalleryFilter> filters;
};

class QGalleryUnionFilterPrivate : public QGalleryFilterPrivate
{
public:
    QGalleryUnionFilterPrivate(QGalleryFilter::Type type = QGalleryFilter::Union)
        : QGalleryFilterPrivate(type) {}

    QGalleryUnionFilterPrivate(const QGalleryUnionFilterPrivate &other)
        : QGalleryFilterPrivate(other)
        , filters(other.filters)
    {
    }

    bool isEqual(const QGalleryFilterPrivate &other) const
    {
        return other.type == type && static_cast<const QGalleryUnionFilterPrivate &>
                (other).filters == filters;
    }

#ifndef QT_NO_DEBUG_STREAM
    void printDebug(QDebug &debug) const
    {
        debug << "QGalleryUnionFilter(";
        QList<QGalleryFilter>::const_iterator filter = filters.begin();
        if (filter != filters.end()) {
            debug << *filter;

            while (++filter != filters.end())
                debug << " &&" << *filter;
        }
        debug << ")";
    }
#endif
    QList<QGalleryFilter> filters;
};

class QGalleryMetaDataFilterPrivate : public QGalleryFilterPrivate
{
public:
    QGalleryMetaDataFilterPrivate(QGalleryFilter::Type type = QGalleryFilter::MetaData)
        : QGalleryFilterPrivate(type)
        , comparator(QGalleryFilter::Equals)
        , negated(false)
    {
    }

    QGalleryMetaDataFilterPrivate(const QGalleryMetaDataFilterPrivate &other)
        : QGalleryFilterPrivate(other)
        , comparator(other.comparator)
        , negated(other.negated)
        , property(other.property)
        , value(other.value)
    {
    }

    bool isEqual(const QGalleryFilterPrivate &other) const
    {
        if (other.type == type) {
            const QGalleryMetaDataFilterPrivate &o
                    = static_cast<const QGalleryMetaDataFilterPrivate &>(other);

            return o.comparator == comparator
                    && o.negated == negated
                    && o.property == property
                    && o.value == value;
        } else {
            return false;
        }
    }

#ifndef QT_NO_DEBUG_STREAM
    void printDebug(QDebug &debug) const
    {
        if (negated)
            debug << "!";
        debug << "QGalleryMetaDataFilter(";
        if (!property.isNull())
            debug << "propertyName: " << property << " ";
        debug << "comparator: " << comparator;
        if (!value.isNull())
            debug << " value: " << value;
        debug << ")";
    }
#endif

    QGalleryFilter::Comparator comparator;
    bool negated;
    QString property;
    QVariant value;
};

/*!
    \class QGalleryIntersectionFilter

    \ingroup gallery
    \ingroup gallery-filters
    \since 1.1

    \inmodule QtGallery

    \brief The QGalleryIntersectionFilter class provides a filter which matches
    the intersection of two or more meta-data filters.

    An intersection filter represents the intersection of a list of gallery
    filters.  Filters can be added to an intersection using either the append(),
    prepend(), or insert() functions, or using the
    \l {operator<<()}{<< operator}.

    Intersection filters can also combining two gallery filters with the
    && operator.

    \sa QGalleryFilter, QGalleryMetaDataFilter
*/

/*!
    Constructs an empty intersection.
    \since 1.1
*/

QGalleryIntersectionFilter::QGalleryIntersectionFilter()
    : d(new QGalleryIntersectionFilterPrivate)
{
}

/*!
    Constructs an intersection filter from a meta-data \a filter.
    \since 1.1
*/

QGalleryIntersectionFilter::QGalleryIntersectionFilter(const QGalleryMetaDataFilter &filter)
    : d(new QGalleryIntersectionFilterPrivate)
{
    d->filters.append(filter);
}

/*!
    Constructs an intersection filter from a union \a filter.
    \since 1.1
*/

QGalleryIntersectionFilter::QGalleryIntersectionFilter(const QGalleryUnionFilter &filter)
    : d(new QGalleryIntersectionFilterPrivate)
{
    d->filters.append(filter);
}

/*!
    Constructs a copy of an intersection \a filter.
    \since 1.1
*/

QGalleryIntersectionFilter::QGalleryIntersectionFilter(const QGalleryIntersectionFilter &filter)
    : d(filter.d)
{
}

/*!
    \internal
*/

inline QGalleryIntersectionFilter::QGalleryIntersectionFilter(QGalleryFilterPrivate *d)
    : d(static_cast<QGalleryIntersectionFilterPrivate *>(d))
{
}

/*!
    \internal
*/

inline QGalleryIntersectionFilter::QGalleryIntersectionFilter(QGalleryFilter::Type type)
    : d(new QGalleryIntersectionFilterPrivate(type))
{
}

/*!
    Destroys an intersection filter.
*/

QGalleryIntersectionFilter::~QGalleryIntersectionFilter()
{
}

/*!
    Assigns the value of \a filter to another intersection filter.
    \since 1.1
*/

QGalleryIntersectionFilter &QGalleryIntersectionFilter::operator =(
        const QGalleryIntersectionFilter &filter)
{
    d = filter.d;

    return *this;
}

/*!
    Returns true if the filter is a valid intersection filter.

    An invalid filter can be obtained by casting a non
    QGalleryFilter::Intersection type QGalleryFilter to
    QGalleryIntersectionFilter.
    \since 1.1
*/

bool QGalleryIntersectionFilter::isValid() const
{
    return d->type == QGalleryFilter::Intersection;
}

/*!
    Returns the number of filters in an intersection.
    \since 1.1
*/

int QGalleryIntersectionFilter::filterCount() const
{
    return d->filters.count();
}

/*!
    Returns true if an intersection does not contain any filters, and returns
    false otherwise.
    \since 1.1
*/

bool QGalleryIntersectionFilter::isEmpty() const
{
    return d->filters.isEmpty();
}

/*!
    Return the filters contained in an intersection.
    \since 1.1
*/

QList<QGalleryFilter> QGalleryIntersectionFilter::filters() const
{
    return d->filters;
}

/*!
    Appends a meta-data \a filter to an intersection.
    \since 1.1
*/

void QGalleryIntersectionFilter::append(const QGalleryMetaDataFilter &filter)
{
    d->filters.append(filter);
}

/*!
    Appens a union \a filter to an intersection.
    \since 1.1
*/

void QGalleryIntersectionFilter::append(const QGalleryUnionFilter &filter)
{
    d->filters.append(filter);
}

/*!
    Appends the contents of an intersection \a filter to an intersection.
    \since 1.1
*/

void QGalleryIntersectionFilter::append(const QGalleryIntersectionFilter &filter)
{
    d->filters += filter.d->filters;
}

/*!
    Prepends a meta-data \a filter to an intersection.
    \since 1.1
*/

void QGalleryIntersectionFilter::prepend(const QGalleryMetaDataFilter &filter)
{
    d->filters.prepend(filter);
}

/*!
    Prepends a union \a filter to an intersection.
    \since 1.1
*/

void QGalleryIntersectionFilter::prepend(const QGalleryUnionFilter &filter)
{
    d->filters.prepend(filter);
}

/*!
    Prepends the contents of an intersection \a filter to an intersection.
    \since 1.1
*/

void QGalleryIntersectionFilter::prepend(const QGalleryIntersectionFilter &filter)
{
    d->filters = filter.d->filters + d->filters;
}

/*!
    Inserts a meta-data \a filter into an intersection at \a index.
    \since 1.1
*/

void QGalleryIntersectionFilter::insert(int index, const QGalleryMetaDataFilter &filter)
{
    d->filters.insert(index, filter);
}

/*!
    Inserts a union \a filter into an intersection at \a index.
    \since 1.1
*/

void QGalleryIntersectionFilter::insert(int index, const QGalleryUnionFilter &filter)
{
    d->filters.insert(index, filter);
}

/*!
    Inserts the contents of an intersection  \a filter into an intersection at
    \a index.
    \since 1.1
*/

void QGalleryIntersectionFilter::insert(int index, const QGalleryIntersectionFilter &filter)
{
    QList<QGalleryFilter> start = d->filters.mid(0, index);
    QList<QGalleryFilter> end = d->filters.mid(index);

    d->filters = start + filter.d->filters + end;
}

/*!
    Replaces the filter at \a index in an intersection with a meta-data
    \a filter.
    \since 1.1
*/

void QGalleryIntersectionFilter::replace(int index, const QGalleryMetaDataFilter &filter)
{
    d->filters.replace(index, filter);
}

/*!
    Replaces the filter at \a index in an intersection with a union
    \a filter.
    \since 1.1
*/

void QGalleryIntersectionFilter::replace(int index, const QGalleryUnionFilter &filter)
{
    d->filters.replace(index, filter);
}

/*!
    Removes the filter at \a index from an intersection.
    \since 1.1
*/

void QGalleryIntersectionFilter::remove(int index)
{
    d->filters.removeAt(index);
}

/*!
    Clears all the filters from an intersection.
    \since 1.1
*/

void QGalleryIntersectionFilter::clear()
{
    d->filters.clear();
}

/*!
    Appends a \a filter to an intersection.
    \since 1.1
*/

QGalleryIntersectionFilter &QGalleryIntersectionFilter::operator <<(
        const QGalleryIntersectionFilter &filter)
{
    d->filters.append(filter.d->filters);

    return *this;
}

/*!
    \fn QGalleryIntersectionFilter operator &&(const QGalleryIntersectionFilter &filter1, const QGalleryIntersectionFilter &filter2)

    Returns a gallery filter matches the intersection of \a filter1 and
    \a filter2.

    \relates QGalleryIntersectionFilter
    \since 1.1
*/

QGalleryIntersectionFilter operator &&(
        const QGalleryIntersectionFilter &filter1, const QGalleryIntersectionFilter &filter2)
{
    QGalleryIntersectionFilter filter = filter1;
    filter.append(filter2);
    return filter;
}

/*!
    \class QGalleryUnionFilter

    \ingroup gallery
    \ingroup gallery-filters
    \since 1.1

    \inmodule QtGallery

    \brief The QGalleryUnionFilter class provides a filter which matches the
    union of two or more meta-data filters.

    A union filter represents the union of a list of gallery filters.
    Filters can be added to an union using either the append(), prepend(), or
    insert() functions, or using the \l {operator<<()}{<< operator}.

    Union filters can also combining two gallery filters with the
    && operator.

    \sa QGalleryFilter, QGalleryMetaDataFilter
*/

/*!
    Constructs an empty union.
    \since 1.1
*/

QGalleryUnionFilter::QGalleryUnionFilter()
    : d(new QGalleryUnionFilterPrivate)
{
}

/*!
    Constructs a union filter from a meta data \a filter.
    \since 1.1
*/

QGalleryUnionFilter::QGalleryUnionFilter(const QGalleryMetaDataFilter &filter)
    : d(new QGalleryUnionFilterPrivate)
{
    d->filters.append(filter);
}

/*!
    Constructs a union filter from an intersection \a filter.
    \since 1.1
*/

QGalleryUnionFilter::QGalleryUnionFilter(const QGalleryIntersectionFilter &filter)
    : d(new QGalleryUnionFilterPrivate)
{
    d->filters.append(filter);
}

/*!
    Constructs a copy of a union \a filter.
    \since 1.1
*/

QGalleryUnionFilter::QGalleryUnionFilter(const QGalleryUnionFilter &filter)
    : d(filter.d)
{
}

/*!
    \internal
*/

QGalleryUnionFilter::QGalleryUnionFilter(QGalleryFilterPrivate *d)
    : d(static_cast<QGalleryUnionFilterPrivate *>(d))
{
}

/*!
    \internal
*/

inline QGalleryUnionFilter::QGalleryUnionFilter(QGalleryFilter::Type type)
    : d(new QGalleryUnionFilterPrivate(type))
{
}

/*!
    Destroys a union filter.
*/

QGalleryUnionFilter::~QGalleryUnionFilter()
{
}

/*!
    Assigns the value of \a filter to another union filter.
    \since 1.1
*/

QGalleryUnionFilter &QGalleryUnionFilter::operator =(const QGalleryUnionFilter &filter)
{
    d = filter.d;

    return *this;
}

/*!
    Returns true if the filter is a valid union filter.

    An invalid filter can be obtained by casting a non QGalleryFilter::Union
    type QGalleryFilter to QGalleryUnionFilter.
    \since 1.1
*/

bool QGalleryUnionFilter::isValid() const
{
    return d->type == QGalleryFilter::Union;
}

/*!
    Returns the number of filters in a union.
    \since 1.1
*/

int QGalleryUnionFilter::filterCount() const
{
    return d->filters.count();
}

/*!
    Returns true if a union contains no filters, and false otherwise.
    \since 1.1
*/

bool QGalleryUnionFilter::isEmpty() const
{
    return d->filters.isEmpty();
}

/*!
    Returns the filters contained in a union.
    \since 1.1
*/

QList<QGalleryFilter> QGalleryUnionFilter::filters() const
{
    return d->filters;
}

/*!
    Appends a meta-data \a filter to a union.
    \since 1.1
*/

void QGalleryUnionFilter::append(const QGalleryMetaDataFilter &filter)
{
    d->filters.append(filter);
}

/*!
    Appends an intersection \a filter to a union.
    \since 1.1
*/

void QGalleryUnionFilter::append(const QGalleryIntersectionFilter &filter)
{
    d->filters.append(filter);
}

/*!
    Appends the contents of a union \a filter to a union.
    \since 1.1
*/

void QGalleryUnionFilter::append(const QGalleryUnionFilter &filter)
{
    d->filters += filter.d->filters;
}

/*!
    Prepends a meta-data \a filter to a union.
    \since 1.1
*/

void QGalleryUnionFilter::prepend(const QGalleryMetaDataFilter &filter)
{
    d->filters.prepend(filter);
}

/*!
    Prepends an intersection \a filter to a union.
    \since 1.1
*/

void QGalleryUnionFilter::prepend(const QGalleryIntersectionFilter &filter)
{
    d->filters.prepend(filter);
}

/*!
    Prepends the contents of a union \a filter to a union.
    \since 1.1
*/

void QGalleryUnionFilter::prepend(const QGalleryUnionFilter &filter)
{
    d->filters = filter.d->filters + d->filters;
}

/*!
    Inserts a meta-data \a filter into a union at \a index.
    \since 1.1
*/

void QGalleryUnionFilter::insert(int index, const QGalleryMetaDataFilter &filter)
{
    d->filters.insert(index, filter);
}

/*!
    Inserts an intersection \a filter into a union at \a index.
    \since 1.1
*/

void QGalleryUnionFilter::insert(int index, const QGalleryIntersectionFilter &filter)
{
    d->filters.insert(index, filter);
}

/*!
    Inserts the contents of a union \a filter into a union at \a index.
    \since 1.1
*/

void QGalleryUnionFilter::insert(int index, const QGalleryUnionFilter &filter)
{
    QList<QGalleryFilter> start = d->filters.mid(0, index);
    QList<QGalleryFilter> end = d->filters.mid(index);

    d->filters = start + filter.d->filters + end;
}

/*!
    Replaces the filter at \a index in a union with a meta-data \a filter.
    \since 1.1
*/

void QGalleryUnionFilter::replace(int index, const QGalleryMetaDataFilter &filter)
{
    d->filters.replace(index, filter);
}

/*!
    Replaces the filter at \a index in a union with an intersetion \a filter.
    \since 1.1
*/

void QGalleryUnionFilter::replace(int index, const QGalleryIntersectionFilter &filter)
{
    d->filters.replace(index, filter);
}

/*!
    Removes the filter at \a index from a union.
    \since 1.1
*/

void QGalleryUnionFilter::remove(int index)
{
    d->filters.removeAt(index);
}

/*!
    Clears all the filters from a union.
    \since 1.1
*/

void QGalleryUnionFilter::clear()
{
    d->filters.clear();
}

/*!
    Appends a \a filter to a union.
    \since 1.1
*/

QGalleryUnionFilter &QGalleryUnionFilter::operator <<(const QGalleryUnionFilter &filter)
{
    d->filters.append(filter.d->filters);

    return *this;
}

/*!
    \fn QGalleryUnionFilter operator ||(const QGalleryUnionFilter &filter1, const QGalleryUnionFilter &filter2)

    Returns a gallery filter which matches the union of \a filter1 and
    \a filter2.

    \relates QGalleryUnionFilter
    \since 1.1
*/

QGalleryUnionFilter operator ||(
        const QGalleryUnionFilter &filter1, const QGalleryUnionFilter &filter2)
{
    QGalleryUnionFilter filter = filter1;
    filter.append(filter2);
    return filter;
}

/*!
    \class QGalleryMetaDataFilter

    \ingroup gallery
    \ingroup gallery-filters

    \inmodule QtGallery
    \since 1.1

    \brief The QGalleryMetaDataFilter class provides a filter which accepts
    items with meta-data properties matching a specific value.

    A meta-data filter represents a \l {comparator()}{comparison} between the
    value of an item's meta-data property() and a fixed value().  A filter
    can be constructed by passing the property name, value, and comparator
    to the filter contructor or by using one of the operator overloads or
    constructor functions on QGalleryProperty.

    Meta-data filters can also be combined to create \l {QGalleryUnionFilter}
    {union} or \l {QGalleryIntersectionFilter}{intersection} filters using the
    || and && operators respectively.

    \sa QGalleryFilter, QGalleryProperty
*/

/*!
    Constructs an empty meta-data filter.
*/

QGalleryMetaDataFilter::QGalleryMetaDataFilter()
    : d(new QGalleryMetaDataFilterPrivate)
{
}

/*!
    Constructs a meta-data filter which tests if a meta-data \a property
    matches a \a value according to a \a comparison operator.
    \since 1.1
*/

QGalleryMetaDataFilter::QGalleryMetaDataFilter(
        const QString &property,
        const QVariant &value,
        QGalleryFilter::Comparator comparison)
            : d(new QGalleryMetaDataFilterPrivate)
{
    d->property = property;
    d->value = value;
    d->comparator = comparison;
}

/*!
    Constructs a copy of a meta-data \a filter.
    \since 1.1
*/

QGalleryMetaDataFilter::QGalleryMetaDataFilter(const QGalleryMetaDataFilter &filter)
    : d(filter.d)
{
}

/*!
    \internal
*/

QGalleryMetaDataFilter::QGalleryMetaDataFilter(QGalleryFilterPrivate *d)
    : d(static_cast<QGalleryMetaDataFilterPrivate *>(d))
{
}

/*!
    \internal
*/

inline QGalleryMetaDataFilter::QGalleryMetaDataFilter(QGalleryFilter::Type type)
    : d(new QGalleryMetaDataFilterPrivate(type))
{
}

/*!
    Destroys a meta-data filter.
*/

QGalleryMetaDataFilter::~QGalleryMetaDataFilter()
{
}

/*!
    Assigns the value of \a filter to another meta-data filter.
    \since 1.1
*/

QGalleryMetaDataFilter &QGalleryMetaDataFilter::operator =(const QGalleryMetaDataFilter &filter)
{
    d = filter.d;

    return *this;
}

/*!
    Returns true if the filter is a valid meta-data filter.

    An invalid filter can be obtained by casting a non QGalleryFilter::MetaData
    type QGalleryFilter to QGalleryMetaDataFilter.
    \since 1.1
*/

bool QGalleryMetaDataFilter::isValid() const
{
    return d->type == QGalleryFilter::MetaData;
}

/*!
    Returns the name of the property the value of a meta-data filter is
    compared against.
    \since 1.1
*/

QString QGalleryMetaDataFilter::propertyName() const
{
    return d->property;
}

/*!
    Sets the \a name of the property the value of meta-data filter is compared
    against.
    \since 1.1
*/

void QGalleryMetaDataFilter::setPropertyName(const QString &name)
{
    d->property = name;
}

/*!
    Returns the value a property is expected to match for a meta-data filter to
    pass.
    \since 1.1
*/

QVariant QGalleryMetaDataFilter::value() const
{
    return d->value;
}

/*!
    Sets the \a value a property is expected to match for a meta-data filter to
    pass.
    \since 1.1
*/

void QGalleryMetaDataFilter::setValue(const QVariant &value)
{
    d->value = value;
}

/*!
    Returns the comparator used by a meta-data filter.
    \since 1.1
*/

QGalleryFilter::Comparator QGalleryMetaDataFilter::comparator() const
{
    return d->comparator;
}

/*!
    Sets the \a comparator used by a meta-data filter.
    \since 1.1
*/

void QGalleryMetaDataFilter::setComparator(QGalleryFilter::Comparator comparator)
{
    d->comparator = comparator;
}

/*!
    Returns whether the result of a meta-data filter should be negated.
    \since 1.1
*/

bool QGalleryMetaDataFilter::isNegated() const
{
    return d->negated;
}

/*!
    Sets whether the result of a meta-data filter should be \a negated.
    \since 1.1
*/

void QGalleryMetaDataFilter::setNegated(bool negated)
{
    d->negated = negated;
}

/*!
    Returns a meta-data filter which tests the negation of an existing filter.
    \since 1.1
*/

QGalleryMetaDataFilter QGalleryMetaDataFilter::operator !() const
{
    QGalleryMetaDataFilter filter(*this);
    filter.setNegated(!d->negated);
    return filter;
}

/*!
    \class QGalleryFilter

    \ingroup gallery
    \ingroup gallery-filters
    \since 1.1

    \inmodule QtGallery

    \brief The QGalleryFilter class provides filtering criteria for gallery
    requests.

    A gallery filter may be a {QGalleryMetaData}{meta-data},
    {QGalleryUnionFilter}{union}, or {QGalleryIntersectionFilter}{intersection}
    filter.  The type of a QGalleryFilter is given by the type() function, and
    a filter can be cast to its specific type using one of toMetaFilter(),
    toUnionFilter() or toIntersectionFilter().  Casting to different type of
    filter will always result in an invalid filter.

    \sa QGalleryMetaDataFilter, QGalleryUnionFilter, QGalleryIntersectionFilter
*/

/*!
    \enum QGalleryFilter::Type

    Identifies the type of a filter.

    \value Invalid  The filter is a null QGalleryFilter.
    \value Intersection The filter is a QGalleryIntersectionFilter.
    \value Union The filter is a QGalleryUnionFilter.
    \value MetaData The filter is a QGalleryMetaDataFilter.
*/

/*!
    \enum QGalleryFilter::Comparator

    Identifies the comparison made by a QGalleryMetaDataFilter.

    \value Equals The filter tests if a meta-data property is equal to a value.
    \value LessThan The filter tests if a meta-data property is less than a
    value.
    \value GreaterThan The filter tests if a meta-data property is greater
    than a value.
    \value LessThanEquals The filter tests if a meta-data property is less than
    or equal to a value.
    \value GreaterThanEquals The filter tests if a meta-data property is
    greater than or equal to a value.
    \value Contains The filter tests if a meta-data property contains a
    sub-string.
    \value StartsWith The filter tests if a meta-data property starts with a
    string.
    \value EndsWith The filter tests if a meta-data property ends with a
    string.
    \value Wildcard The filter tests if a meta-data property matches a wildcard
    string.
    \value RegExp The filter tests if a meta-data property matches a regular
    expression.
*/

/*!
    Constructs a gallery filter of type \l Invalid.
*/

QGalleryFilter::QGalleryFilter()
    : d(new QGalleryInvalidFilterPrivate)
{
}

/*!
    Constructs a copy of a gallery \a filter.
    \since 1.1
*/

QGalleryFilter::QGalleryFilter(const QGalleryFilter &filter)
    : d(filter.d)
{
}

/*!
    Constructs a copy of a gallery intersection \a filter.
    \since 1.1
*/

QGalleryFilter::QGalleryFilter(const QGalleryIntersectionFilter &filter)
    : d(const_cast<QGalleryIntersectionFilterPrivate *>(filter.d.constData()))
{
}

/*!
    Constructs a copy of a gallery union \a filter.
    \since 1.1
*/

QGalleryFilter::QGalleryFilter(const QGalleryUnionFilter &filter)
    : d(const_cast<QGalleryUnionFilterPrivate *>(filter.d.constData()))
{
}

/*!
    Constructs a copy of a gallery meta-data \a filter.
    \since 1.1
*/

QGalleryFilter::QGalleryFilter(const QGalleryMetaDataFilter &filter)
    : d(const_cast<QGalleryMetaDataFilterPrivate *>(filter.d.constData()))
{
}

/*!
    Destroys a gallery filter.
*/

QGalleryFilter::~QGalleryFilter()
{
}

/*!
    Assigns the value of \a filter to another filter.
    \since 1.1
*/

QGalleryFilter &QGalleryFilter::operator =(const QGalleryFilter &filter)
{
    d = filter.d;

    return *this;
}

/*!
    Returns the type of a filter.
    \since 1.1
*/

QGalleryFilter::Type QGalleryFilter::type() const
{
    return d->type;
}

/*!
    Returns true if the type() of the filter is not equal to Invalid.
    \since 1.1
*/

bool QGalleryFilter::isValid() const
{
    return d->type != QGalleryFilter::Invalid;
}

/*!
    Casts a filter to an intersection filter.  The filter must be of type
    Intersection or this will return an Invalid filter.

    Returns a QGalleryIntersectionFilter.
    \since 1.1
*/

QGalleryIntersectionFilter QGalleryFilter::toIntersectionFilter() const
{
    return d->type == Intersection
            ? QGalleryIntersectionFilter(const_cast<QGalleryFilterPrivate *>(d.constData()))
            : QGalleryIntersectionFilter(QGalleryFilter::Invalid);
}

/*!
    Casts a filter to a union filter.  The filter must be of type Union or this
    will return an Invalid filter.

    Returns a QGalleryUnionFilter.
    \since 1.1
*/

QGalleryUnionFilter QGalleryFilter::toUnionFilter() const
{
    return d->type == Union
            ? QGalleryUnionFilter(const_cast<QGalleryFilterPrivate *>(d.constData()))
            : QGalleryUnionFilter(QGalleryFilter::Invalid);
}

/*!
    Casts a filter to a meta-data filter.  The filter must be of type MetaData
    or this will return an Invalid filter.

    Returns a QGalleryMetaDataFilter.
    \since 1.1
*/

QGalleryMetaDataFilter QGalleryFilter::toMetaDataFilter() const
{
    return d->type == MetaData
            ? QGalleryMetaDataFilter(const_cast<QGalleryFilterPrivate *>(d.constData()))
            : QGalleryMetaDataFilter(QGalleryFilter::Invalid);
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator <<(QDebug debug, const QGalleryFilter &filter)
{
    filter.d->printDebug(debug.nospace());

    return debug;
}
#endif

/*!
    Compares \a filter1 to filter2.

    Returns true if the filters are identical, and false otherwise.
    \since 1.1
*/

bool operator ==(const QGalleryFilter &filter1, const QGalleryFilter &filter2)
{
    return filter1.d == filter2.d || filter1.d->isEqual(*filter2.d);
}

/*!
    Compares \a filter1 to filter2.

    Returns true if the filters are not identical, and false otherwise.
    \since 1.1
*/

bool operator !=(const QGalleryFilter &filter1, const QGalleryFilter &filter2)
{
    return filter1.d != filter2.d && !filter1.d->isEqual(*filter2.d);
}

QTM_END_NAMESPACE

