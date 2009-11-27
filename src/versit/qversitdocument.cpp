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

#include "qversitdocument.h"
#include "qversitdocument_p.h"
#include "qmobilityglobal.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QVersitDocument
 
  \brief The QVersitDocument class is a container for 0..n versit properties.

  \ingroup versit
 
  For example a vCard can be presented as a QVersitDocument that
  consists of 0..n properties such as a name (N),
  a telephone number (TEL) and an email address (EMAIL) to name a few.
  Each of these properties is stored as
  an instance of a QVersitProperty in a QVersitDocument.
 
  QVersitDocument supports implicit sharing.
 
  \sa QVersitProperty
 */

/*! Constructs a new empty document */
QVersitDocument::QVersitDocument() : d(new QVersitDocumentPrivate())
{
}

/*! Constructs a document that is a copy of \a other */
QVersitDocument::QVersitDocument(const QVersitDocument& other) : d(other.d)
{
}

/*! Frees the memory used by the document */
QVersitDocument::~QVersitDocument()
{
}

/*! Assigns this document to \a other */
QVersitDocument& QVersitDocument::operator=(const QVersitDocument& other)
{
    if (this != &other)
        d = other.d;
    return *this;    
}

/*!
 * Sets the versit document type to \a type.
 */
void QVersitDocument::setVersitType(VersitType type)
{
    d->mVersitType = type;
}

/*!
 * Gets the versit document type.
 */
QVersitDocument::VersitType QVersitDocument::versitType() const
{
    return d->mVersitType;
}

/*!
 * Add \a property to the list of contained versit properties.
 * The property is appended as the last property of the list.
 */
void QVersitDocument::addProperty(const QVersitProperty& property)
{
    d->mProperties.append(property);
}

/*!
 * Gets the list of the contained versit properties.
 * Note that the actual properties cannot be modified using the copy.
 */
QList<QVersitProperty> QVersitDocument::properties() const
{
    return d->mProperties;  
}

/*!
 * Removes all the properties with \a name from the versit document.
 */
void QVersitDocument::removeProperties(const QString& name)
{
    for (int i=d->mProperties.count()-1; i >=0; i--) {
        if (d->mProperties[i].name() == name) {
            d->mProperties.removeAt(i);
        }
    }
}

QTM_END_NAMESPACE
