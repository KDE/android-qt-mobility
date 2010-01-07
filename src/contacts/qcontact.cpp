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

#include <QSet>

#include "qcontact.h"
#include "qcontact_p.h"
#include "qcontactdetail_p.h"
#include "qcontactmanager_p.h"
#include "qcontactaction.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QContact
 
  \brief The QContact class provides an addressbook contact.

  \ingroup contacts-main
 
  A QContact consists of zero or more details.
 
  An instance of the QContact class represents an in-memory contact,
  and may not reflect the state of that contact found in persistent
  storage until the appropriate synchronisation method is called
  on the QContactManager (i.e., saveContact, removeContact).
 
  \sa QContactManager, QContactDetail
 */

/*!
 * \fn QList<T> QContact::details() const
 * Returns a list of details of the template type
 */

/*!
 * \fn QList<T> QContact::details(const QString& fieldName, const QString& value) const
 * Returns a list of details of the template type which match the \a fieldName and \a value criteria
 */

/*!
 * \fn T QContact::detail() const
 * Returns the first detail of the template type
 */

/*!
 * \fn QContact::operator!=(const QContact &other) const
 * Returns true if this contacts Id or details are different to those of the \a other contact
 */

/*! Construct an empty contact. */
QContact::QContact()
    : d(new QContactData)
{
    // insert the contact's display label detail.
    QContactDisplayLabel contactLabel;
    contactLabel.setValue(QContactDisplayLabel::FieldLabel, QString());
    contactLabel.d->m_id = 1;
    d->m_details.insert(0, contactLabel);

    // and the contact type detail.
    QContactType contactType;
    contactType.setType(QContactType::TypeContact);
    contactType.d->m_id = 2;
    d->m_details.insert(1, contactType);
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
    //stopcompilation;

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
    QContactDisplayLabel dl = d->m_details.at(0);
    dl.setValue(QContactDisplayLabel::FieldLabel, QString());
    dl.d->m_id = 1;
    QContactType typeDet = d->m_details.at(1);
    typeDet.d->m_id = 2;

    d->m_details.clear();
    d->m_details.insert(0, dl);
    d->m_details.insert(1, typeDet);
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

/*! Returns the QContactId that identifies this contact */
QContactId QContact::id() const
{
    return d->m_id;
}

/*! Returns the QContactLocalId that identifies this contact within its manager */
QContactLocalId QContact::localId() const
{
    return d->m_id.localId();
}

/*!
 * Returns the type of the contact.  Every contact has exactly one type which
 * is either set manually (by saving a modified copy of the QCotnactType
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
    d->m_details[1] = newType;
}

/*!
 * Sets the type of the contact to the given \a type.
 */
void QContact::setType(const QContactType& type)
{
    // type is detail 1
    d->m_details[1] = type;
}

/*!
 * Returns the read-only display label of this contact.
 * A contact which has been retrieved from a manager will have a display label synthesized for it.
 */
QString QContact::displayLabel() const
{
    return d->m_details.at(0).value(QContactDisplayLabel::FieldLabel);
}

/*!
 * Sets the id of this contact to \a id.  Note that this only affects
 * this structure, not any corresponding structures stored
 * by a QContactManager.
 *
 * If you change the id of a contact and save the contact
 * in a manager, the previously existing contact will still
 * exist.  You can do this to create copies (possibly modified)
 * of an existing contact.
 *
 * Returns true if the \a id was set successfully, otherwise
 * returns false.
 */
void QContact::setId(const QContactId& id)
{
    d->m_id = id;
}

/*! Returns the first detail stored in the contact which is of the given \a definitionName */
QContactDetail QContact::detail(const QString& definitionName) const
{
    // build the sub-list of matching details.
    for (int i = 0; i < d->m_details.size(); i++) {
        const QContactDetail& existing = d->m_details.at(i);
        if (definitionName.isEmpty() || definitionName == existing.definitionName()) {
            return existing;
        }
    }

    return QContactDetail();
}

/*! Returns a list of details of the given \a definitionName */
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
            if (definitionName == existing.definitionName()) {
                sublist.append(existing);
            }
        }
    }

    return sublist;
}

/*! Returns a list of details of the given \a definitionName, \a fieldName and field \a value*/
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
            if (definitionName == existing.definitionName() && existing.hasValue(fieldName) && value == existing.value(fieldName)) {
                sublist.append(existing);
            }
        }
    }

    return sublist;
}

/*!
 * Saves the given \a detail in the list of stored details, and sets its Id.
 * If another detail of the same type and Id has been previously saved in
 * this contact, that detail is overwritten.  Otherwise, a new Id is generated
 * and set in the detail, and the detail is added to the list.
 *
 * If \a detail is a contact type, the existing contact type will
 * be overwritten with \a detail.  There is never more than one contact type
 * in a contact.
 *
 * If \a detail is a display label, the operation will fail.
 * The display label can never be updated manually and must be synthesized by the backend.
 *
 * Returns true if the detail was saved successfully, otherwise returns false
 */
bool QContact::saveDetail(QContactDetail* detail)
{
    if (!detail)
        return false;

    /* Handle display labels specially - cannot save them! */
    if (detail->definitionName() == QContactDisplayLabel::DefinitionName) {
        return false;
    }

    /* Also handle contact type specially - only one of them. */
    if (detail->definitionName() == QContactType::DefinitionName) {
        d->m_details[1] = *detail;
        detail->d->m_id = 2;
        return true;
    }

    // try to find the "old version" of this field
    // ie, the one with the same type and id, but different value or attributes.
    for (int i = 0; i < d->m_details.size(); i++) {
        const QContactDetail& curr = d->m_details.at(i);
        if (detail->d->m_definitionName == curr.d->m_definitionName && detail->d->m_id == curr.d->m_id) {
            // Found the old version.  Replace it with this one.
            d->m_details[i] = *detail;
            return true;
        }
    }

    // this is a new detail!  add it to the contact.
    detail->d->m_id = ++d->m_nextDetailId;
    d->m_details.append(*detail);
    return true;
}

/*!
 * Removes the \a detail from the contact.
 *
 * Any preference for the given field is also removed.
 * The Id of the \a detail is removed, to signify that it is no longer
 * part of the contact.
 *
 * If the detail is the contact type for this contact, the type
 * will be reset to \c QContactType::TypeContact, and the function will return success.
 *
 * If the detail is a display label, the operation will fail and return false.
 *
 * Returns true if the detail was removed successfully, false if an error occurred
 */
bool QContact::removeDetail(QContactDetail* detail)
{
    if (!detail)
        return false;

    // Check if this a display label
    if (detail->d->m_definitionName == QContactDisplayLabel::DefinitionName) {
        return false;
    }

    // Check if this a type
    if (detail->d->m_definitionName == QContactType::DefinitionName) {
        QContactType type = d->m_details[1];
        type.setType(QContactType::TypeContact);
        d->m_details[1] = type;
        return true;
    }

    if (!d->m_details.contains(*detail))
        return false;

    // remove any preferences we may have stored for the detail.
    QStringList keys = d->m_preferences.keys();
    for (int i = 0; i < keys.size(); i++) {
        QString prefKey = keys.at(i);
        if (d->m_preferences.value(prefKey) == detail->d->m_id) {
            d->m_preferences.remove(prefKey);
        }
    }

    // then remove the detail.
    d->m_details.removeOne(*detail);
    detail->d->m_id = 0;
    return true;
}

/*! Returns true if this contact is equal to the \a other contact, false if either the id or stored details are not the same */
bool QContact::operator==(const QContact& other) const
{
    return other.d->m_id == d->m_id &&
        other.d->m_details == d->m_details;
}


/*! Retrieve the first detail for which the given \a actionName is available */
QContactDetail QContact::detailWithAction(const QString& actionName) const
{
    if (actionName.isEmpty())
        return QContactDetail();

    QList<QContactDetail> dets = detailsWithAction(actionName);
    if (dets.isEmpty())
        return QContactDetail();

    QContactDetail retn = dets.first();
    return retn;
}

/*! Retrieve any details for which the given \a actionName is available */
QList<QContactDetail> QContact::detailsWithAction(const QString& actionName) const
{
    if (actionName.isEmpty())
        return QList<QContactDetail>();

    // ascertain which details are supported by any implementation of the given action
    QList<QContactDetail> retn;
    QList<QContactActionDescriptor> descriptors = QContactManagerData::actionDescriptors(actionName);
    for (int i = 0; i < descriptors.size(); i++) {
        QContactAction *currImpl = QContactManagerData::action(descriptors.at(i));
        for (int i = 0; i < d->m_details.size(); i++) {
            QContactDetail detail = d->m_details.at(i);
            if (currImpl->supportsDetail(detail)) {
                retn.append(detail);
                break;
            }
        }

        // clean up
        delete currImpl;
    }

    return retn;
}

/*! Returns a list of relationships of the given \a relationshipType in which the contact was a participant at the time that it was retrieved from the manager */
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

/*! Returns a list of ids of contacts which are related to this contact in a relationship of the given \a relationshipType, where those other contacts participate in the relationship in the given \a role */
QList<QContactId> QContact::relatedContacts(const QString& relationshipType, QContactRelationshipFilter::Role role) const
{
    QList<QContactId> retn;
    for (int i = 0; i < d->m_relationshipsCache.size(); i++) {
        QContactRelationship curr = d->m_relationshipsCache.at(i);
        if (curr.relationshipType() == relationshipType || relationshipType.isEmpty()) {
            // check that the other contacts fill the given role
            if (role == QContactRelationshipFilter::First) {
                if (curr.first() != d->m_id) {
                    retn.append(curr.first());
                }
            } else if (role == QContactRelationshipFilter::Second) {
                if (curr.first() == d->m_id) {
                    retn.append(curr.second());
                }
            } else { // role == Either.
                if (curr.first() == d->m_id) {
                    retn.append(curr.second());
                } else {
                    retn.append(curr.first());
                }
            }
        }
    }

    QList<QContactId> removeDuplicates;
    for (int i = 0; i < retn.size(); i++) {
        QContactId curr = retn.at(i);
        if (!removeDuplicates.contains(curr)) {
            removeDuplicates.append(curr);
        }
    }

    return removeDuplicates;
}

/*!
 * Sets the order of importance of the relationships for this contact by saving a \a reordered list of relationships which involve the contact.
 * The list must include all of the relationships in which the contact is involved, and must not include any relationships which do
 * not involve the contact.  In order for the ordering preference to be persisted, the contact must be saved in its manager.
 *
 * It is possible that relationships will have been added or removed from the contact stored in the manager,
 * thus rendering the relationship cache of the contact in memory stale.   If this happens, attempting to save the contact after reordering
 * its relationships will result in an error occurring. The updated relationships list must be retrieved from the manager, reordered and set
 * in the contact before the contact can be saved successfully.
 *
 * \sa relationships(), relationshipOrder()
 */
void QContact::setRelationshipOrder(const QList<QContactRelationship>& reordered)
{
    d->m_reorderedRelationshipsCache = reordered;
}

/*!
 * Returns the ordered list of relationships in which the contact is involved.  By default, this list is equal to the cached
 * list of relationships which is available by calling relationships().
 *
 * \sa setRelationshipOrder()
 */
QList<QContactRelationship> QContact::relationshipOrder() const
{
    return d->m_reorderedRelationshipsCache;
}

/*!
 * Return a list of actions available to be performed on this contact which are offered
 * by the vendor whose name is the given \a vendorName, where the action instance has
 * the implementation version given by \a implementationVersion.
 * If \a vendorName is empty, actions from any vendor are supplied; if \a implementationVersion
 * is \c -1, action implementations of any version will be returned.
 */
QList<QContactActionDescriptor> QContact::availableActions(const QString& vendorName, int implementationVersion) const
{
    // check every action implementation to see if it supports me.
    QSet<QContactActionDescriptor> retn;
    QList<QContactActionDescriptor> descriptors = QContactManagerData::actionDescriptors();
    for (int i = 0; i < descriptors.size(); i++) {
        QContactActionDescriptor currDescriptor = descriptors.at(i);
        QContactAction *currImpl = QContactManagerData::action(currDescriptor);
        if (QContactManagerEngine::testFilter(currImpl->contactFilter(), *this)) {
            if ((vendorName.isEmpty() || currDescriptor.vendorName() == vendorName) &&
                    (implementationVersion == -1 || currDescriptor.implementationVersion() == implementationVersion)) {
                retn.insert(currDescriptor);
            }
        }

        // clean up the implementation to avoid leak.
        delete currImpl;
    }

    return retn.toList();
}

/*! Set a particular detail as the \a preferredDetail for a given \a actionName.  Returns true if the detail was successfully set as the preferred detail for the action identified by \a actionName, otherwise returns false  */
bool QContact::setPreferredDetail(const QString& actionName, const QContactDetail& preferredDetail)
{
    // if the given action name is empty, bad argument.
    if (actionName.isEmpty())
        return false;

    // check to see whether the the given preferredDetail is saved in this contact
    if (!d->m_details.contains(preferredDetail))
        return false;

    // otherwise, save the preference.
    d->m_preferences.insert(actionName, preferredDetail.d->m_id);
    return true;
}

/*! Returns true if the given \a detail is a preferred detail for the given \a actionName, or for any action if the \a actionName is empty */
bool QContact::isPreferredDetail(const QString& actionName, const QContactDetail& detail) const
{
    if (!d->m_details.contains(detail))
        return false;

    if (actionName.isEmpty())
         return d->m_preferences.values().contains(detail.d->m_id);

    QMap<QString, quint32>::const_iterator it = d->m_preferences.find(actionName);
    if (it != d->m_preferences.end() && it.value() == detail.d->m_id)
        return true;

    return false;
}

/*! Returns the preferred detail for a given \a actionName */
QContactDetail QContact::preferredDetail(const QString& actionName) const
{
    // if the given action name is empty, bad argument.
    if (actionName.isEmpty())
        return QContactDetail();

    if (!d->m_preferences.contains(actionName))
        return QContactDetail();

    QContactDetail retn;
    quint32 detId = d->m_preferences.value(actionName);
    for (int i = 0; i < d->m_details.size(); i++) {
        QContactDetail det = d->m_details.at(i);
        if (det.d->m_id == detId) {
            // found it.
            retn = det;
            break;
        }
    }

    return retn;
}

QTM_END_NAMESPACE
