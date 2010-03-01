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

#include "qlandmarkcategory.h"
#include "qlandmarkcategory_p.h"
#include "qlandmarkcategoryid.h"

#include <QStringList>
#include <QVariant>

QTM_USE_NAMESPACE

/*!
    \class QLandmarkCategory

    \brief The QLandmarkCategory class designates a grouping of
    landmarks of similar type.

    \ingroup location

    Landmarks of similar type may be grouped together into categories,
    e.g. restaurants, accommodation etc.  A QLandmarkCategory object
    represents one of these and allows access to category properties
    such as description and icon.

    More than one category can be assigned to a landmark.  Assignment
    of a category to a landmark is achieved by using the QLandmark class
    in conjunction with the QLandmarkCategoryId class.


    A QLandmarkCategory instance is an in memory representation and may
    not reflect the state of the category found in persistent storage,
    until the appropriate synchronization method is called on the
    QLandmarkManager(i.e.QLandmarkManager::saveCategory(),
    QLandmarkManager::removeCategory()).
*/

/*!
    Constructs an empty category.
*/
QLandmarkCategory::QLandmarkCategory()
{
    d = new QLandmarkCategoryPrivate();
}

/*!
    Constructs a copy of \a other.
*/
QLandmarkCategory::QLandmarkCategory(const QLandmarkCategory &other)
{
    d = new QLandmarkCategoryPrivate(*(other.d));
}

/*!
    Destroys the category.
*/
QLandmarkCategory::~QLandmarkCategory()
{
    delete d;
}
/*!
    Assigns \a other to this category and returns a reference to this category.
*/
QLandmarkCategory& QLandmarkCategory::operator= (const QLandmarkCategory & other)
{
    *d = *(other.d);
    return *this;
}

/*!
    Returns true if this category is equal to \a other, otherwise
    returns false.
*/
bool QLandmarkCategory::operator== (const QLandmarkCategory &other) const
{
    return (*d == *(other.d));
}

/*!
    Returns the name of the category
*/
QString QLandmarkCategory::name() const
{
    return d->name;
}

/*!
    Sets the \a name of the category
*/
void QLandmarkCategory::setName(const QString &name){
    d->name = name;
}

/*!
    Returns the filepath of the category icon
*/
QString QLandmarkCategory::icon() const
{
    return d->icon;
}

/*!
    Sets the \a filePath of the category icon
*/
void QLandmarkCategory::setIcon(const QString &filePath)
{
    d->icon = filePath;
}

/*!
    Returns the description of the category.
*/
QString QLandmarkCategory::description() const
{
    return d->description;
}

/*!
    Sets the \a description of the category
*/
void QLandmarkCategory::setDescription(const QString &description)
{
    d->description = description;
}

/*!
    Returns whether the category is read-only.
*/
bool QLandmarkCategory::isReadOnly() const
{
    return true;
}

/*!
    Returns the identifier of the category.
*/
QLandmarkCategoryId QLandmarkCategory::categoryId() const
{
    return QLandmarkCategoryId();
}

/*!
    Returns the value of the attribute corresponding to \a attributeName.
*/
QVariant QLandmarkCategory::attribute(const QString &attributeName) const
{
    return QVariant();
}

/*!
    Sets the \a value of the attribute corresponding to \a attributeName.
*/
void QLandmarkCategory::setAttribute(const QString &attributeName, const QVariant &value)
{

}

/*!
    Returns a list of attribute names.
*/
QStringList QLandmarkCategory::attributes() const
{
    return QStringList();
}
