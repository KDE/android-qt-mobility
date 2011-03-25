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

#include "qorganizeritemdetailfielddefinition.h"
#include "qorganizeritemdetailfielddefinition_p.h"

#ifndef QT_NO_DEBUG_STREAM
#include <QDebug>
#endif

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerItemDetailFieldDefinition
  \brief The QOrganizerItemDetailFieldDefinition class provides a field in a QOrganizerItemDetail.

  \inmodule QtOrganizer

  Encapsulates information about a particular datum which may be part of a
  QOrganizerItemDetail, including the type, allowable values and access constraints.
 */

/*!
 * Constructs a new field with no constraints and an invalid data type.
 */
QOrganizerItemDetailFieldDefinition::QOrganizerItemDetailFieldDefinition()
        : d(new QOrganizerItemDetailFieldDefinitionPrivate)
{
}

/*!
 * Cleans up the memory in use by the field
 */
QOrganizerItemDetailFieldDefinition::~QOrganizerItemDetailFieldDefinition()
{
}

/*!
 * Constructs a copy of the \a other field
 */
QOrganizerItemDetailFieldDefinition::QOrganizerItemDetailFieldDefinition(const QOrganizerItemDetailFieldDefinition& other)
        : d(other.d)
{
}

/*!
 * Assigns the field to be equal to the \a other field
 */
QOrganizerItemDetailFieldDefinition& QOrganizerItemDetailFieldDefinition::operator=(const QOrganizerItemDetailFieldDefinition& other)
{
    d = other.d;
    return *this;
}

/*!
 * Returns the data type of the field
 */
int QOrganizerItemDetailFieldDefinition::dataType() const
{
    return d->m_dataType;
}

/*!
 * Sets the data type of the field to \a type
 */
void QOrganizerItemDetailFieldDefinition::setDataType(int type)
{
    d->m_dataType = type;
}

/*!
 * Returns the list of allowable values which this field may store
 */
QVariantList QOrganizerItemDetailFieldDefinition::allowableValues() const
{
    return d->m_allowableValues;
}

/*!
 * Sets the list of allowable values which this field may store to \a values
 */
void QOrganizerItemDetailFieldDefinition::setAllowableValues(const QVariantList values)
{
    d->m_allowableValues = values;
}

/*!
 * Returns true if the allowable values and data type of the \a other field are equal to those of this field
 */
bool QOrganizerItemDetailFieldDefinition::operator==(const QOrganizerItemDetailFieldDefinition& other) const
{
    if (d->m_allowableValues != other.d->m_allowableValues)
        return false;
    if (d->m_dataType != other.d->m_dataType)
        return false;
    return true;
}

/*!
 * Returns true if the allowable values or data type of the \a other field differ from those of this field
 */
bool QOrganizerItemDetailFieldDefinition::operator!=(const QOrganizerItemDetailFieldDefinition& other) const
{
    return !(*this == other);
}

#ifndef QT_NO_DATASTREAM
/*!
 * Writes the detail field definition \a definition to the stream \a out.
 */
QDataStream& operator<<(QDataStream& out, const QOrganizerItemDetailFieldDefinition& definition)
{
    quint8 formatVersion = 1; // Version of QDataStream format for QOrganizerItemDetailFieldDefinition
    return out << formatVersion << static_cast<qint32>(definition.dataType()) << definition.allowableValues();
}

/*!
 * Reads a detail field definition from stream \a in into \a definition.
 */
QDataStream& operator>>(QDataStream& in, QOrganizerItemDetailFieldDefinition& definition)
{
    quint8 formatVersion;
    in >> formatVersion;
    if (formatVersion == 1) {
        qint32 dataType;
        QVariantList allowableValues;
        in >> dataType >> allowableValues;
        definition.setDataType(dataType);
        definition.setAllowableValues(allowableValues);
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
QDebug operator<<(QDebug dbg, const QOrganizerItemDetailFieldDefinition& definition)
{
    dbg.nospace() << "QOrganizerItemDetailFieldDefinition(";
    dbg.nospace() << "dataType=";
    dbg.nospace() << static_cast<qint32>(definition.dataType());
    dbg.nospace() << ",";
    dbg.nospace() << "allowableValues=";
    dbg.nospace() << definition.allowableValues();
    dbg.nospace() << ")";
    return dbg.maybeSpace();
}
#endif

QTM_END_NAMESPACE
