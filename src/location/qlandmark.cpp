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

#include "qlandmark.h"
#include "qlandmark_p.h"

#include "qlandmarkcategoryid.h"

#include <QVariant>
#include <QStringList>

QTM_USE_NAMESPACE

// ----- QLandmarkCategoryPrivate -----

QLandmarkCategoryPrivate::QLandmarkCategoryPrivate() {}

QLandmarkCategoryPrivate::QLandmarkCategoryPrivate(const QLandmarkCategoryPrivate &other)
{
    name = other.name;
    icon = other.icon;
    description = other.description;
}

QLandmarkCategoryPrivate::~QLandmarkCategoryPrivate() {}

QLandmarkCategoryPrivate& QLandmarkCategoryPrivate::operator= (const QLandmarkCategoryPrivate & other)
{
    name = other.name;
    icon = other.icon;
    description = other.description;

    return *this;
}

bool QLandmarkCategoryPrivate::operator== (const QLandmarkCategoryPrivate &other) const
{
    return ((name == other.name)
            && (icon == other.icon)
            && (description == other.description));
}
// ---- QLandmarkId ----
/*!
    \class QLandmarkId
    \ingroup location

    \brief The QLandmarkId class provides a unique identifier for a
    landmark.

    It consists of a manager URI which identifies the manager which
    contains the landmark, and the id for that landmark.
*/

/*!
    Creates an invalid identifier, id() and managerUri() will return null
    strings.
*/
QLandmarkId::QLandmarkId()
{
}

/*!
    Constructs a copy of \a other.
*/
QLandmarkId::QLandmarkId(const QLandmarkId &other)
{
}

/*!
    Destroys the identifier.
*/
QLandmarkId::~QLandmarkId()
{
}

/*!
    Returns true if the identifier is valid, meaning that both
    a mangaer URI and id have been set.
*/
bool QLandmarkId::isValid() const
{
    return false;
}

/*!
    Returns the id of the landmark identifier.
*/
QString QLandmarkId::id()
{
    return QString();
}

/*!
    Sets the \a id of the landmark identifier.
*/
void QLandmarkId::setId(const QString &id)
{
}

/*!
    Returns the URI of the manager which this landmark identifier
    belongs to.
*/
QString QLandmarkId::managerUri() const
{
    return QString();
}

/*!
    Sets the manager \a uri which this landmark identifier
    belongs to.
*/
void QLandmarkId::setManagerUri(const QString &uri)
{
}

/*!
    Assigns \a other to this landmark identifier and returns a
    reference to this landmark identifier.
*/
QLandmarkId &QLandmarkId::operator=(const QLandmarkId &other)
{
    return *this;
}

/*!
    Returns true if this landmark identifier is equal to \a other,
    otherwise returnse false.

    Two QLandmarks are considered equal if both have the same
    manager URI and id.
*/
bool QLandmarkId::operator==(const QLandmarkId &other) const
{
    return false;
}

/*!
    Returns true if this landmark identifier is not equal to \a other,
    otherwise returns false.

    If either the manager URIs or ids differ, then the
    QLandmarkIds are not considered equal.
*/
bool QLandmarkId::operator!=(const QLandmarkId &other) const
{
    return true;
}

// ----- QLandmarkPrivate -----

QLandmarkPrivate::QLandmarkPrivate() {}

QLandmarkPrivate::QLandmarkPrivate(const QLandmarkPrivate &other)
{
    name = other.name;
    coordinate = other.coordinate;
    description = other.description;
    icon = other.icon;
    radius = other.radius;
    street = other.street;
    locality = other.locality;
    region = other.region;
    country = other.country;
    postcode = other.postcode;
    phone = other.phone;
    url = other.url;
    categories = other.categories;
    attributes = other.attributes;
}

QLandmarkPrivate::~QLandmarkPrivate() {}

QLandmarkPrivate& QLandmarkPrivate::operator= (const QLandmarkPrivate & other)
{
    name = other.name;
    coordinate = other.coordinate;
    description = other.description;
    icon = other.icon;
    radius = other.radius;
    street = other.street;
    locality = other.locality;
    region = other.region;
    country = other.country;
    postcode = other.postcode;
    phone = other.phone;
    url = other.url;
    categories = other.categories;
    attributes = other.attributes;

    return *this;
}

bool QLandmarkPrivate::operator== (const QLandmarkPrivate &other) const
{
    return ((name == other.name)
            && (coordinate == other.coordinate)
            && (description == other.description)
            && (icon == other.icon)
            && (radius == other.radius)
            && (street == other.street)
            && (locality == other.locality)
            && (region == other.region)
            && (country == other.country)
            && (postcode == other.postcode)
            && (phone == other.phone)
            && (url == other.url)
            && (categories == other.categories)
            && (attributes == other.attributes));
}

/*!
    \class QLandmark
    \ingroup location

    \brief The QLandmark class represents a location or point of interest.


    Each landmark consists of a number of properties such as name,
    coordinates, icon etc.  Landmarks may also be assigned a set of
    generic attributes which may be accessed and modified by using the attribute()
    and setAttribute() functions.

    Each QLandmark may be associated with zero or more categories.
    A category  defines a type of landmark such as  restaurants or
    cinemas.  To set the category that a landmark belongs to, use
    the setCategories() or addCategory() functions.  A landmark may
    be removed from a category by using the removeCategory() function.

    Each QLandmark is an in memory representation of a landmark;
    it does not reflect the actual landmark state in persistent
    storage, until the appropriate synchronization method is called
    on the QLandmarkManager(i.e. saveLandmark(), removeLandmark()).
*/

/*!
    Constructs an empty landmark.

    A new QLandmark will return an invalid QLandmarkId when
    categoryId() is called.
*/
QLandmark::QLandmark()
{
    d = new QLandmarkPrivate();
}

/*!
    Constructs a copy of \a other.
*/
QLandmark::QLandmark(const QLandmark &other)
{
    d = new QLandmarkPrivate(*(other.d));
}

/*!
    Destroys the landmark.
*/
QLandmark::~QLandmark()
{
    delete d;
}

/*!
    Assigns \a other to this landmark and returns a reference to this landmark.
*/
QLandmark& QLandmark::operator= (const QLandmark & other)
{
    *d = *(other.d);
    return *this;
}

/*!
    Returns true if this landmark is equal to \a other, otherwise
    returns false.
*/
bool QLandmark::operator== (const QLandmark &other) const
{
    return (*d == *(other.d));
}

/*!
    Returns the name of the landmark.
*/
QString QLandmark::name() const
{
    return d->name;
}

/*!
    Sets the \a name of the landmark.
*/
void QLandmark::setName(const QString &name)
{
    d->name = name;
}

/*!
    Returns the coordinate of the landmark.
*/
QGeoCoordinate QLandmark::coordinate() const
{
    return d->coordinate;
}

/*!
    Sets the \a coordinate of the landmark.
*/
void QLandmark::setCoordinate(const QGeoCoordinate& coordinate)
{
    d->coordinate = coordinate;
}

/*!
    Returns a of list identifiers of categories that this landmark
    belongs to.
*/
QList<QLandmarkCategoryId> QLandmark::categories() const
{
    return d->categories;
}

/*!
    Sets the categories that this landmark belongs to via
    a list of \a categoryIds.

*/
void QLandmark::setCategories(const QList<QLandmarkCategoryId> &categoryIds)
{
    // remove duplicates
    d->categories.clear();
    for (int i = 0; i < categoryIds.size(); ++i) {
        if (!d->categories.contains(categoryIds.at(i)))
            d->categories.append(categoryIds.at(i));
    }
}

/*!
    Adds another category that this landmark will be associated
    with via its \a categoryId.
*/
void QLandmark::addCategory(const QLandmarkCategoryId &categoryId)
{
    if (!d->categories.contains(categoryId))
        d->categories.append(categoryId);
}

/*!
    Removes a category from a landmark, by using its \a categoryId.
*/
void QLandmark::removeCategory(const QLandmarkCategoryId &categoryId)
{
    d->categories.removeAll(categoryId);
}

/*!
    Returns a description of the landmark.
*/
QString QLandmark::description() const
{
    return d->description;
}

/*!
    Sets the \a description of the landmark.
*/
void QLandmark::setDescription(const QString &description)
{
    d->description = description;
}

/*!
    Returns the file path of the landmark's icon.
*/
QString QLandmark::icon() const
{
    return d->icon;
}

/*!
    Sets the \a filePath of the landmark's icon.
*/
void QLandmark::setIcon(const QString &filePath)
{
    d->icon = filePath;
}

/*!
    Returns the coverage radius of the landmark.

    The coverage radius is relevant for large landmarks
    such as cities.  Note that landmark searches over a given area
    do not factor in the coverage radius.
*/
double QLandmark::radius() const
{
    return d->radius;
}

/*!
    Sets the coverage \a radius of the landmark.
*/
void QLandmark::setRadius(double radius)
{
    d->radius = radius;
}

/*!
    Returns the value of the attribute corresponding to \a attributeName.
*/
QVariant QLandmark::attribute(const QString &attributeName) const
{
    return d->attributes.value(attributeName);
}

/*!
    Sets the \a value of the attribute corresponding to \a attributeName.
*/
void QLandmark::setAttribute(const QString &attributeName, const QVariant &value)
{
    d->attributes[attributeName] = value;
}

/*!
    Returns a list of attribute names.
*/
QStringList QLandmark::attributes() const
{
    return d->attributes.keys();
}

/*!
    Returns the street of the landmark.
*/
QString QLandmark::street() const
{
    return d->street;
}

/*!
    Sets the \a street of the landmark.
*/
void QLandmark::setStreet(const QString &street)
{
    d->street = street;
}

/*!
    Returns the locality of the landmark.
*/
QString QLandmark::locality() const
{
    return d->locality;
}

/*!
    Sets the \a locality of the landmark.
*/
void QLandmark::setLocality(const QString &locality)
{
    d->locality = locality;
}

/*!
    Returns the region of the landmark.
*/
QString QLandmark::region() const
{
    return d->region;
}

/*!
    Sets the \a region of the landmark
*/
void QLandmark::setRegion(const QString &region)
{
    d->region = region;
}

/*!
    Returns the country of the landmark.
*/
QString QLandmark::country() const
{
    return d->country;
}

/*!
    Sets the \a country of the landmark.
*/
void QLandmark::setCountry(const QString &country)
{
    d->country = country;
}

/*!
    Returns the postcode of the landmark.
*/
QString QLandmark::postcode() const
{
    return d->postcode;
}

/*!
    Sets the \a postcode of the landmark.
*/
void QLandmark::setPostcode(const QString &postcode)
{
    d->postcode = postcode;
}

/*!
    Returns the post office box of the landmark.
*/
QString QLandmark::postOfficeBox() const
{
    return QString();
}

/*!
    Sets the \a postOfficeBox of the landmark.
*/
void QLandmark::setPostOfficeBox(const QString &postOfficeBox)
{
}

/*!
    Returns the phone number of the landmark.
*/
QString QLandmark::phone() const
{
    return d->phone;
}

/*!
    Sets the \a phone number of the landmark.
*/
void QLandmark::setPhone(const QString &phone)
{
    d->phone = phone;
}
/*!
    Returns the url of the landmark.
*/
QString QLandmark::url() const
{
    return d->url;
}

/*!
    Sets the \a url of the landmark.
*/
void QLandmark::setUrl(const QString &url)
{
    d->url = url;
}

/*!
    Returns the identifier of the landmark.

    Saving a new landmark using a QLandmarkManager will assign
    the landmark a valid identifier.
*/
QLandmarkId QLandmark::landmarkId()
{
    return QLandmarkId();
}
