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

#include "qcontactdetaildefinition.h"
#include "qcontactdetaildefinition_p.h"

#ifndef QT_NO_DEBUG_STREAM
#include <QDebug>
#endif

QTM_BEGIN_NAMESPACE

/*!
  \class QContactDetailDefinition

  \inmodule QtContacts
   \since 1.0

  The QContactDetailDefinition class provides the specification for
  a detail that can be included in any particular QContact.
  The definition does not include any data, but defines
  the semantics of the representation and use of data
  details that are stored in a QContact.
 */

/*!
  \fn QContactDetailDefinition::operator!=(const QContactDetailDefinition& other) const
  Returns true if this detail definition has different allowable field types or uniqueness to the \a other definition
  \since 1.0
 */

/*! Construct a new, invalid QContactDetailDefinition */
QContactDetailDefinition::QContactDetailDefinition()
    : d(new QContactDetailDefinitionData)
{
}

/*! Constructs a new QContactDetailDefinition from \a other
    \since 1.0
*/
QContactDetailDefinition::QContactDetailDefinition(const QContactDetailDefinition& other)
    : d(other.d)
{
}

/*! Assigns this QContactDetailDefinition to \a other
   \since 1.0
*/
QContactDetailDefinition& QContactDetailDefinition::operator=(const QContactDetailDefinition& other)
{
    d = other.d;
    return *this;
}

/*! Frees the memory used by this QContactDetailDefinition */
QContactDetailDefinition::~QContactDetailDefinition()
{
}

/*! Returns true if the definition has the same type, uniqueness and allowable value datatypes as \a other
    \since 1.0
*/
bool QContactDetailDefinition::operator==(const QContactDetailDefinition& other) const
{
    if (d->m_name != other.d->m_name)
        return false;
    if (d->m_unique != other.d->m_unique)
        return false;
    if (d->m_fields != other.d->m_fields)
        return false;
    return true;
}

/*! Returns true if the id and fields of this definition are empty
    \since 1.0
*/
bool QContactDetailDefinition::isEmpty() const
{
    if (!d->m_name.isEmpty())
        return false;
    if (!d->m_fields.isEmpty())
        return false;
    return true;
}

#ifndef QT_NO_DATASTREAM
/*!
 * Writes \a definition to the stream \a out.
 * \since 1.0
 */
QDataStream& operator<<(QDataStream& out, const QContactDetailDefinition& definition)
{
    quint8 formatVersion = 1; // Version of QDataStream format for QContactDetailDefinition
    return out << formatVersion
               << definition.name()
               << definition.isUnique()
               << definition.fields();
}

/*!
 * Reads a detail definition from stream \a in into \a definition.
 * \since 1.0
 */
QDataStream& operator>>(QDataStream& in, QContactDetailDefinition& definition)
{
    definition = QContactDetailDefinition();
    quint8 formatVersion;
    in >> formatVersion;
    if (formatVersion == 1) {
        QString name;
        bool unique;
        QMap<QString, QContactDetailFieldDefinition> fields;
        in >> name >> unique >> fields;
        definition.setName(name);
        definition.setUnique(unique);
        definition.setFields(fields);
    } else {
        in.setStatus(QDataStream::ReadCorruptData);
    }
    return in;
}
#endif

#ifndef QT_NO_DEBUG_STREAM
/*!
  Outputs \a definition to the debug stream \a dbg
 */
QDebug operator<<(QDebug dbg, const QContactDetailDefinition& definition)
{
    dbg.nospace() << "QContactDetailDefinition("
                  << "name=" << definition.name() << ","
                  << "isUnique=" << definition.isUnique() << ","
                  << "isEmpty=" << definition.isEmpty() << ","
                  << "fields=" << definition.fields()
                  << ")";
    return dbg.maybeSpace();
}
#endif

/*! Sets the unique identifier of this detail type to \a definitionName.
    \since 1.0
*/
void QContactDetailDefinition::setName(const QString& definitionName)
{
    d->m_name = definitionName;
}

/*!
  Sets whether a contact can have more than one detail of this type.
  If \a unique is true, only one detail of this type can be added.
  Otherwise, any number can be added.
  \since 1.0
 */
void QContactDetailDefinition::setUnique(bool unique)
{
    d->m_unique = unique;
}

/*! Returns the identifier of this detail definition
    \since 1.0
*/
QString QContactDetailDefinition::name() const
{
    return d->m_name;
}

/*! Returns the per-contact uniqueness of this definition
    \since 1.0
*/
bool QContactDetailDefinition::isUnique() const
{
    return d->m_unique;
}

/*! Sets the fields which constitute the data of details of this this definition to \a fields
    \since 1.0
*/
void QContactDetailDefinition::setFields(const QMap<QString, QContactDetailFieldDefinition>& fields)
{
    d->m_fields = fields;
}

/*! Returns the map of keys to fields which are present in details of this definition
    \since 1.0
*/
QMap<QString, QContactDetailFieldDefinition> QContactDetailDefinition::fields() const
{
    return d->m_fields;
}

/*! Inserts the field \a field into the map of fields which constitute the data of details of this definition for the given field key \a key.
    If another field for that key already exists in the definition, it will be overwritten.
    \since 1.0
*/
void QContactDetailDefinition::insertField(const QString& key, const QContactDetailFieldDefinition& field)
{
    d->m_fields.insert(key, field);
}

/*! Removes the field associated with the given field key \a key from the map of fields which constitute the data of details of this definition.
    \since 1.0
*/
void QContactDetailDefinition::removeField(const QString& key)
{
    d->m_fields.remove(key);
}


QTM_END_NAMESPACE
