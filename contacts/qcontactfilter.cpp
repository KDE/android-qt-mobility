/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcontactfilter.h"
#include "qcontactfilter_p.h"

#include "qcontactintersectionfilter.h"
#include "qcontactunionfilter.h"

#include "qcontactmanager.h"

/*!
 * \class QContactFilter
 * \brief The QContactFilter class is used to select contacts made available through a QContactManager
 *
 * This class is used as a parameter to various functions offered by QContactManager, to allow
 * selection of contacts which have certain details or properties.
 */

/*!
 * \enum QContactFilter::FilterType
 * Describes the type of the filter
 * \value Invalid An invalid filter which matches nothing
 * \value ContactDetail A filter which matches contacts containing one or more details of a particular definition with a particular value
 * \value ContactDetailRange A filter which matches contacts containing one or more details of a particular definition whose values are within a particular range
 * \value ChangeLog A filter which matches contacts whose timestamps have been updated since some particular date and time
 * \value Action A filter which matches contacts for which a particular action is available, or which contain a detail with a particular value for which a particular action is available
 * \value GroupMembership A filter which matches contacts which are members of a particular group
 * \value Intersection A filter which matches all contacts that are matched by all filters it includes
 * \value Union A filter which matches any contact that is matched by any of the filters it includes
 * \value IdList A filter which matches any contact whose id is contained in a particular list of contact ids
 * \value Default A filter which matches everything
 */

/*!
 * \fn QContactFilter::operator!=(const QContactFilter& other) const
 * Returns true if this filter is not identical to the \a other filter.
 * \sa operator==()
 */

template<> QContactFilterPrivate *QSharedDataPointer<QContactFilterPrivate>::clone()
{
    return d->clone();
}

/*! Constructs an empty filter */
QContactFilter::QContactFilter()
    : d_ptr(0)
{
}

/*! Constructs a new copy of \a other */
QContactFilter::QContactFilter(const QContactFilter& other)
    : d_ptr(other.d_ptr)
{
}

/*! Assigns this filter to be \a other */
QContactFilter& QContactFilter::operator=(const QContactFilter& other)
{
    if (this != &other) {
        d_ptr = other.d_ptr;
    }
    return *this;
}

/*! Cleans up the memory used by this filter */
QContactFilter::~QContactFilter()
{
}

/*! Returns the type of the filter */
QContactFilter::FilterType QContactFilter::type() const
{
    if (!d_ptr)
        return QContactFilter::Default;
    return d_ptr->type();
}

/*! Returns true if the filter has the same type and criteria as \a other */
bool QContactFilter::operator==(const QContactFilter& other) const
{
    /* A default filter is only equal to other default filters */
    if (!d_ptr)
        return !other.d_ptr;

    /* Different types can't be equal */
    if (other.type() != type())
        return false;

    /* Otherwise, use the virtual op == */
    return d_ptr->compare(other.d_ptr);
}

/*! Constructs a new filter from the given data pointer \a d */
QContactFilter::QContactFilter(QContactFilterPrivate *d)
    : d_ptr(d)
{

}

/*! Intersects the \a left and \a right filters */
const QContactFilter operator&&(const QContactFilter& left, const QContactFilter& right)
{
    // XXX TODO: empty intersection/union operations are not well defined yet.
    //if (left.type() == QContactFilter::Intersection) {
    //    QContactIntersectionFilter bf(left);
    //    /* we can just add the right to this one */
    //    bf.append(right);
    //    return bf;
    //}

    //if (right.type() == QContactFilter::Intersection) {
    //    QContactIntersectionFilter bf(right);
    //    /* we can prepend the left to this one */
    //    bf.prepend(left);
    //    return bf;
    //}

    /* usual fallback case */
    QContactIntersectionFilter nif;
    nif << left << right;
    return nif;
}

/*! Unions the \a left and \a right filters */
const QContactFilter operator||(const QContactFilter& left, const QContactFilter& right)
{
    if (left.type() == QContactFilter::Union) {
        QContactUnionFilter bf(left);
        /* we can just add the right to this one */
        bf.append(right);
        return bf;
    }

    if (right.type() == QContactFilter::Union) {
        QContactUnionFilter bf(right);
        /* we can prepend the left to this one */
        bf.prepend(left);
        return bf;
    }

    /* usual fallback case */
    QContactUnionFilter nif;
    nif << left << right;
    return nif;
}

