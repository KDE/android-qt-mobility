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

#include <QSet>
#include <QDebug>

#include "qcontact.h"
#include "qcontact_p.h"
#include "qcontactdetail_p.h"
#include "qcontactmanager_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QContact
 
  \brief The QContact class represents an addressbook contact.

  \ingroup contacts-main

  Individual contacts, groups, and other types of contacts are represented with
  a QContact object.  In addition to the type, a QContact consists of information
  that belongs to the contact and some information about the relationships that the
  contact has.

  A QContact object has a collection of details (like a name, phone numbers and
  email addresses).  Each detail (which can have multiple fields) is stored
  in an appropriate subclass of QContactDetail, and the QContact allows
  retrieving these details in various ways.

  A QContact may have zero or more relationships with other contacts.  For example,
  a group contact would have a \c "HasMember" relationship with the QContacts that
  are its members.  Spouses, managers and assistants can also be represented this
  way.
 
  A QContact instance represents the in-memory version of an addressbook contact,
  and has no tie to a specific QContactManager.  It is possible for the contents
  of a QContact to change independently of the contents that are stored persistently
  in a QContactManager.  A QContact has an ID associated with it when it is first
  retrieved from a QContactManager, or after it has been first saved, and this allows
  clients to track changes using the signals in QContactManager.

  A QContact has a number of mandatory details:
  \list
   \o A QContactType, with the type of the contact (individual contact, group etc)
   \o A QContactDisplayLabel, representing the text to display
  \endlist

  If you have edited the contents of a QContact (via saving or removing details),
  you will need to ask a specific QContactManager for the new display label for the
  contact, since system settings (like the order of first or last names) can vary
  between managers.
 
  \sa QContactManager, QContactDetail
 */

/*!
 * \fn QList<T> QContact::details() const
 * Returns a list of details of the template parameter type.  The type must be
 * a subclass of QContactDetail.
 *
 * For example:
 *  \snippet doc/src/snippets/qtcontactsdocsample/qtcontactsdocsample.cpp 3
 */

/*!
 * \fn QList<T> QContact::details(const QString& fieldName, const QString& value) const
 * Returns a list of details of the template parameter type which have field called \a fieldName, with matching \a value.
 * The type must be a subclass of QContactDetail.
 *
 * For example:
 *  \snippet doc/src/snippets/qtcontactsdocsample/qtcontactsdocsample.cpp 4
 */

/*!
 * \fn T QContact::detail() const
 * Returns the first detail of the template parameter type, as returned by the template details() function.
 * The type must be a subclass of QContactDetail.
 */

/*!
 * \fn QContact::operator!=(const QContact &other) const
 * Returns true if this contacts id or details are different to those of the \a other contact.
 */

/*!
    Construct an empty contact.

    The contact will have an empty display label, an empty id, and have type \l QContactType::TypeContact.
    The isEmpty() function will return true.
*/
QContact::QContact()
    : d(new QContactData)
{
    clearDetails();
}

/*! Initializes this QContact from \a other */
QContact::QContact(const QContact& other)
    : d(other.d)
{
}

/*!
 * Returns true if this QContact is empty, false if not.
 *
 * An empty QContact has an empty label and no extra details.
 * The type of the contact is irrelevant.
 */
bool QContact::isEmpty() const
{
    /* Every contact has a display label field.. */
    if (d->m_details.count() > 2)
        return false;

    /* We know we have two details (a display label and a type) */
    const QContactDisplayLabel& label = d->m_details.at(0);
    return label.label().isEmpty();
}

/*!
 * Removes all details of the contact.
 * This function does not modify the id or type of the contact.
 * Calling isEmpty() after calling this function will return true.
 */
void QContact::clearDetails()
{
    d->m_details.clear();

    // insert the contact's display label detail.
    QContactDisplayLabel contactLabel;
    contactLabel.setValue(QContactDisplayLabel::FieldLabel, QString());
    contactLabel.d->m_id = 1;
    contactLabel.d->m_access = QContactDetail::Irremovable | QContactDetail::ReadOnly;
    d->m_details.insert(0, contactLabel);

    // and the contact type detail.
    QContactType contactType;
    contactType.setType(QContactType::TypeContact);
    contactType.d->m_id = 2;
    contactType.d->m_access = QContactDetail::Irremovable;
    d->m_details.insert(1, contactType);
}

/*! Replace the contents of this QContact with \a other */
QContact& QContact::operator=(const QContact& other)
{
    d = other.d;
    return *this;
}

/*! Frees the memory used by this QContact */
QContact::~QContact()
{
}

/*!
    Returns the QContactId that identifies this contact.

    This may have been set when the contact was retrieved from
    a particular manager, or when the contact was first saved
    in a manager.  The QContactId is only valid with a specific
    manager.  See \l QContactManager::saveContact() for more
    information.

    \sa localId()
 */
QContactId QContact::id() const
{
    return d->m_id;
}

/*!
    Returns the QContactLocalId that identifies this contact within its manager

    This may have been set when the contact was retrieved from
    a particular manager, or when the contact was first saved
    in a manager.  The QContactLocalId is associated with a specific
    manager, but other contacts with the same local id might exist in
    different managers.

    See \l QContactManager::saveContact() for more
    information.

   \sa id()
*/
QContactLocalId QContact::localId() const
{
    return d->m_id.localId();
}

/*!
 * Returns the type of the contact.  Every contact has exactly one type which
 * is either set manually (by saving a modified copy of the QContactType
 * in the contact, or by calling \l setType()) or synthesized automatically.
 *
 * \sa setType()
 */
QString QContact::type() const
{
    // type is detail 1
    QString type = d->m_details.at(1).value(QContactType::FieldType);
    if (type.isEmpty())
        return QString(QLatin1String(QContactType::TypeContact));
    return type;
}

/*!
 * Sets the type of the contact to the given \a type.
 */
void QContact::setType(const QString& type)
{
    // type is detail 1
    QContactType newType;
    newType.setType(type);
    newType.d->m_access = QContactDetail::Irremovable;

    d->m_details[1] = newType;
}

/*!
 * Sets the type of the contact to the given \a type detail.
 */
void QContact::setType(const QContactType& type)
{
    // type is detail 1
    d->m_details[1] = type;
    d->m_details[1].d->m_access = QContactDetail::Irremovable;
}

/*!
 * Returns the display label of this contact.
 *
 * A contact which has been retrieved from a manager will have a display label set when
 * the contact is retrieved.
 *
 * The display label is usually read-only, since some managers do not support arbitrary
 * labels (see also \l QContactName::setCustomLabel()).  If you modify the contact in a way
 * that would affect the display label, you can call QContactManager::synthesizeContactDisplayLabel() to get an
 * up-to-date display label.
 *
 * See the following example for more information:
 * \snippet doc/src/snippets/qtcontactsdocsample/qtcontactsdocsample.cpp Updating the display label of a contact
 *
 * \sa QContactManager::synthesizeContactDisplayLabel()
 */
QString QContact::displayLabel() const
{
    return d->m_details.at(0).value(QContactDisplayLabel::FieldLabel);
}

/*!
 * Sets the id of this contact to \a id.
 *
 * Note that this only affects this object, not any corresponding structures stored
 * by a QContactManager.
 *
 * If you change the id of a contact and save the contact
 * in a manager, the previously existing contact will still
 * exist.  You can do this to create copies (possibly modified)
 * of an existing contact, or to save a contact in a different manager.
 *
 * \sa QContactManager::saveContact()
 */
void QContact::setId(const QContactId& id)
{
    d->m_id = id;
}

/*!
    \fn QContactDetail QContact::detail(const QLatin1Constant& definitionName) const
    Returns the first detail stored in the contact which with the given \a definitionName.
    The \a definitionName argument is typically the detail name constant provided by a
    specific subclass of QContactDetail.  For example:

    \snippet doc/src/snippets/qtcontactsdocsample/qtcontactsdocsample.cpp 0

    It would usually be more convenient to use the template version of this function, in
    the following manner:

    \snippet doc/src/snippets/qtcontactsdocsample/qtcontactsdocsample.cpp 1
*/

/*!
    \fn QList<QContactDetail> QContact::details(const QLatin1Constant& definitionName) const
    Returns a list of details of the given \a definitionName.

    The \a definitionName argument is typically the detail name constant provided by a
    specific subclass of QContactDetail.  For example:

    \snippet doc/src/snippets/qtcontactsdocsample/qtcontactsdocsample.cpp 2

    It would usually be more convenient to use the template version of this function, in
    the following manner:

    \snippet doc/src/snippets/qtcontactsdocsample/qtcontactsdocsample.cpp 3
*/

/*!
    \fn QList<QContactDetail> QContact::details(const QLatin1Constant& definitionName, const QLatin1Constant& fieldName, const QString& value)
    Returns a list of details of the given \a definitionName, with fields named \a fieldName and with value \a value.
*/

/*!
    \fn QList<T> QContact::details(const char* fieldName, const QString& value) const
    \internal

    Returns a list of details of the template type which match the \a fieldName and \a value criteria
*/

/*!
    Returns the first detail stored in the contact with the given \a definitionName
*/
QContactDetail QContact::detail(const QString& definitionName) const
{
    if (definitionName.isEmpty())
        return d->m_details.first();

    // build the sub-list of matching details.
    for (int i = 0; i < d->m_details.size(); i++) {
        const QContactDetail& existing = d->m_details.at(i);
        if (QContactDetailPrivate::detailPrivate(existing)->m_definitionName == definitionName) {
            return existing;
        }
    }

    return QContactDetail();
}

/*! Returns a list of details with the given \a definitionName
    The definitionName string can be determined by the DefinitionName attribute
    of defined objects (e.g. QContactPhoneNumber::DefinitionName) or by
    requesting a list of all the definition names using
    \l {QContactManager::detailDefinitions()}{detailDefinitions()} or the
    asynchronous \l
    {QContactDetailDefinitionFetchRequest::definitionNames()}{definitionNames()}.*/
QList<QContactDetail> QContact::details(const QString& definitionName) const
{
    // build the sub-list of matching details.
    QList<QContactDetail> sublist;

    // special case
    if (definitionName.isEmpty()) {
        sublist = d->m_details;
    } else {
        for (int i = 0; i < d->m_details.size(); i++) {
            const QContactDetail& existing = d->m_details.at(i);
            if (QContactDetailPrivate::detailPrivate(existing)->m_definitionName == definitionName) {
                sublist.append(existing);
            }
        }
    }

    return sublist;
}

/*!
    Returns a list of details of the given \a definitionName, with fields named \a fieldName and with value \a value.
    The definitionName string can be determined by the DefinitionName attribute
    of defined objects (e.g. QContactPhoneNumber::DefinitionName) or by
    requesting a list of all the definition names using
    \l {QContactManager::detailDefinitions()}{detailDefinitions()} or the
    asynchronous \l
    {QContactDetailDefinitionFetchRequest::definitionNames()}{definitionNames()}.*/
QList<QContactDetail> QContact::details(const QString& definitionName, const QString& fieldName, const QString& value) const
{
    // build the sub-list of matching details.
    QList<QContactDetail> sublist;

    // special case
    if (fieldName.isEmpty()) {
        sublist = details(definitionName);
    } else {
        for (int i = 0; i < d->m_details.size(); i++) {
            const QContactDetail& existing = d->m_details.at(i);
            if (QContactDetailPrivate::detailPrivate(existing)->m_definitionName == definitionName
                && existing.hasValue(fieldName) && value == existing.value(fieldName)) {
                sublist.append(existing);
            }
        }
    }

    return sublist;
}

/*!
    \internal
    Returns the first detail stored in the contact which with the given \a definitionName
*/
QContactDetail QContact::detail(const char* definitionName) const
{
    if (definitionName == 0)
        return d->m_details.first();

    // build the sub-list of matching details.
    for (int i = 0; i < d->m_details.size(); i++) {
        const QContactDetail& existing = d->m_details.at(i);
        if (QContactDetailPrivate::detailPrivate(existing)->m_definitionName == definitionName) {
            return existing;
        }
    }

    return QContactDetail();
}

/*!
    \internal
    Returns a list of details with the given \a definitionName
*/
QList<QContactDetail> QContact::details(const char* definitionName) const
{
    // build the sub-list of matching details.
    QList<QContactDetail> sublist;

    // special case
    if (definitionName == 0) {
        sublist = d->m_details;
    } else {
        for (int i = 0; i < d->m_details.size(); i++) {
            const QContactDetail& existing = d->m_details.at(i);
            if (QContactDetailPrivate::detailPrivate(existing)->m_definitionName == definitionName) {
                sublist.append(existing);
            }
        }
    }

    return sublist;
}

/*!
    \internal
    Returns a list of details with the given \a definitionName, \a fieldName and field \a value
*/
QList<QContactDetail> QContact::details(const char* definitionName, const char* fieldName, const QString& value) const
{
    // build the sub-list of matching details.
    QList<QContactDetail> sublist;

    // special case
    if (fieldName == 0) {
        sublist = details(definitionName);
    } else {
        for (int i = 0; i < d->m_details.size(); i++) {
            const QContactDetail& existing = d->m_details.at(i);
            if (QContactDetailPrivate::detailPrivate(existing)->m_definitionName == definitionName
                && existing.hasValue(fieldName) && value == existing.value(fieldName)) {
                sublist.append(existing);
            }
        }
    }

    return sublist;
}

/*!
 * Saves the given \a detail in the list of stored details, and sets the detail's id.
 * If another detail of the same type and id has been previously saved in
 * this contact, that detail is overwritten.  Otherwise, a new id is generated
 * and set in the detail, and the detail is added to the contact.
 *
 * If the detail's access constraint includes \c QContactDetail::ReadOnly,
 * this function will return true and save the detail in the contact,
 * however attempting to save the contact in a manager may fail (if that manager
 * decides that the read only detail should not be updated).
 * Details with the \c QContactDetail::ReadOnly constraint set are typically provided
 * in a contact by the manager, and are usually information that is either
 * synthesized, or not intended to be changed by the user (e.g. presence information
 * for other contacts).
 *
 * If \a detail is a QContactType, the existing contact type will
 * be overwritten with \a detail.  There is never more than one contact type
 * in a contact.
 *
 * If \a detail is a QContactDisplayLabel, the contact will not be updated,
 * and the function will return false.  Since the display label formatting is specific
 * to each manager, use the QContactManager::synthesizeContactDisplayLabel() function
 * instead.
 *
 * Returns true if the detail was saved successfully, otherwise returns false.
 *
 * Note that the caller retains ownership of the detail.
 * \sa QContactManager::synthesizeContactDisplayLabel()
 */
bool QContact::saveDetail(QContactDetail* detail)
{
    if (!detail)
        return false;

    /* Also handle contact type specially - only one of them. */
    if (QContactDetailPrivate::detailPrivate(*detail)->m_definitionName == QContactType::DefinitionName.latin1()) {
        detail->d->m_access |= QContactDetail::Irremovable;
        d->m_details[1] = *detail;
        return true;
    }

    /* And display label.. */
    if (QContactDetailPrivate::detailPrivate(*detail)->m_definitionName == QContactDisplayLabel::DefinitionName.latin1()) {
        return false;
    }

    // try to find the "old version" of this field
    // ie, the one with the same type and id, but different value or attributes.
    for (int i = 0; i < d->m_details.size(); i++) {
        const QContactDetail& curr = d->m_details.at(i);
        if (detail->d->m_definitionName == curr.d->m_definitionName && detail->d->m_id == curr.d->m_id) {
            // update the detail constraints of the supplied detail
            detail->d->m_access = d->m_details[i].accessConstraints();
            // Found the old version.  Replace it with this one.
            d->m_details[i] = *detail;
            return true;
        }
    }

    // this is a new detail!  add it to the contact.
    d->m_details.append(*detail);
    return true;
}

/*!
 * Removes the \a detail from the contact.
 *
 * The detail in the contact which has the same key as that of the given \a detail
 * will be removed if it exists.  Only the key is used for comparison - that is, the
 * information in the detail may be different.
 *
 * If the detail's access constraint includes \c QContactDetail::Irremovable,
 * this function will return false.
 *
 * Returns true if the detail was removed successfully, false if an error occurred.
 *
 * Note that the caller retains ownership of the detail.
 */
bool QContact::removeDetail(QContactDetail* detail)
{
    if (!detail)
        return false;

    // find the detail stored in the contact which has the same key as the detail argument
    int removeIndex = -1;
    for (int i = 0; i < d->m_details.size(); i++) {
        if (d->m_details.at(i).key() == detail->key()) {
            removeIndex = i;
            break;
        }
    }

    // make sure the detail exists (in some form) in the contact.
    if (removeIndex < 0)
        return false;

    if (detail->accessConstraints() & QContactDetail::Irremovable)
        return false;

    if (!d->m_details.contains(*detail))
        return false;

    // then remove the detail.
    d->m_details.removeAt(removeIndex);
    return true;
}

/*! Returns true if this contact is equal to the \a other contact, false if either the id or stored details are not the same */
bool QContact::operator==(const QContact& other) const
{
    return other.d->m_id == d->m_id &&
        other.d->m_details == d->m_details;
}

/*!
    \relates QContact
    Returns the hash value for \a key.
*/
uint qHash(const QContact &key)
{
    uint hash = qHash(key.id());
    foreach (const QContactDetail& detail, key.details()) {
        hash += qHash(detail);
    }
    return hash;
}

QDebug operator<<(QDebug dbg, const QContact& contact)
{
    dbg.nospace() << "QContact(" << contact.id() << ")";
    foreach (const QContactDetail& detail, contact.details()) {
        dbg.space() << '\n' << detail;
    }
    return dbg.maybeSpace();
}

/*!
    Returns a list of relationships of the given \a relationshipType in which this contact is a participant.

    If \a relationshipType is empty, all relationships will be returned.

    \note This function only examines the relationships that were present when this contact
    was retrieved from a manager.  You can also query the manager directly, if you require
    the most up to date information.

    \snippet doc/src/snippets/qtcontactsdocsample/qtcontactsdocsample.cpp 5

    \sa QContactRelationshipFetchRequest, QContactManager::relationships()
 */
QList<QContactRelationship> QContact::relationships(const QString& relationshipType) const
{
    // if empty, then they want all relationships
    if (relationshipType.isEmpty())
        return d->m_relationshipsCache;

    // otherwise, filter on type.
    QList<QContactRelationship> retn;
    for (int i = 0; i < d->m_relationshipsCache.size(); i++) {
        QContactRelationship curr = d->m_relationshipsCache.at(i);
        if (curr.relationshipType() == relationshipType) {
            retn.append(curr);
        }
    }

    return retn;
}

/*!
    Returns a list of the ids of contacts which have a relationship of the given \a relationshipType with this contact.
    The \a role parameter describes the role that the related contacts have in the relationship.

    If \a relationshipType is empty, relationships of all types will be considered.

    \note This function only examines the relationships that were present when this contact
    was retrieved from a manager.  You can also query the manager directly, if you require
    the most up to date information.

    \snippet doc/src/snippets/qtcontactsdocsample/qtcontactsdocsample.cpp 6

    \sa QContactRelationshipFetchRequest, QContactManager::relationships()
 */
QList<QContactId> QContact::relatedContacts(const QString& relationshipType, QContactRelationship::Role role) const
{
    QList<QContactId> retn;
    for (int i = 0; i < d->m_relationshipsCache.size(); i++) {
        QContactRelationship curr = d->m_relationshipsCache.at(i);
        if (relationshipType.isEmpty() || curr.relationshipType() == relationshipType) {
            // check that the other contacts fill the given role
            if (role == QContactRelationship::First) {
                if (curr.first() != d->m_id) {
                    if (!retn.contains(curr.first())) {
                        retn.append(curr.first());
                    }
                }
            } else if (role == QContactRelationship::Second) {
                if (curr.first() == d->m_id) {
                    if (!retn.contains(curr.second())) {
                        retn.append(curr.second());
                    }
                }
            } else { // role == Either.
                if (curr.first() == d->m_id) {
                    if (!retn.contains(curr.second())) {
                        retn.append(curr.second());
                    }
                } else {
                    if (!retn.contains(curr.first())) {
                        retn.append(curr.first());
                    }
                }
            }
        }
    }

    return retn;
}

QTM_END_NAMESPACE
