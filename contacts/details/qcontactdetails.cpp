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
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcontactdetails.h"

/*!
 * \class QContactAddress
 * \brief An address of a contact
 */

/*!
 * \class QContactAnniversary
 * \brief An anniversary of a contact
 */

/*!
 * \class QContactAvatar
 * \brief An avatar of a contact
 */

/*!
 * \class QContactBirthday
 * \brief The birthday of a contact
 */

/*!
 * \class QContactDisplayLabel
 * \brief The (possibly synthesised) display label of a contact
 */

/*!
 * \class QContactEmailAddress
 * \brief An email address of a contact
 */

/*!
 * \class QContactGender
 * \brief The gender of a contact
 */

/*!
 * \class QContactGeolocation
 * \brief A global location coordinate associated with a contact
 */

/*!
 * \class QContactGuid
 * \brief The globally unique Id of a contact
 */

/*!
 * \class QContactName
 * \brief The name of a contact
 */

/*!
 * \class QContactNickname
 * \brief A nickname of a contact
 */

/*!
 * \class QContactOnlineAccount
 * \brief An online account which the contact uses to communicate with friends and family
 */

/*!
 * \class QContactOrganization
 * \brief Details about an organization that the contact is either a part of, or stands for
 */

/*!
 * \class QContactPhoneNumber
 * \brief A phone number of a contact
 */

/*!
 * \class QContactPresence
 * \brief Presence information relating to a QContactOnlineAccount of the contact
 *
 * Details containing presence information are linked to a particular online account detail.
 * The backend should update the presence information relating to an online account when it
 * is available, and delete the presence information detail if its corresponding online account
 * detail is deleted.
 */

/*!
 * \class QContactRelationship
 * \brief Describes the relationship that this contact has to another
 */

/*!
 * \class QContactSyncTarget
 * \brief A sync target for a contact
 */

/*!
 * \class QContactTimestamp
 * \brief Contains the creation and last-modified timestamp associated with the contact
 */

/*!
 * \class QContactType
 * \brief Describes the type of the contact
 */

/*!
 * \class QContactUrl
 * \brief A url associated with a contact
 */

/*!
 * \variable QContactName::DefinitionName
 * The constant string which identifies the definition of details which are names
 */
Q_DEFINE_LATIN1_LITERAL(QContactName::DefinitionName, "Name");

/*!
 * \variable QContactNickname::DefinitionName
 * The constant string which identifies the definition of details which are nicknames
 */
Q_DEFINE_LATIN1_LITERAL(QContactNickname::DefinitionName, "Nickname");

/*!
 * \variable QContactAvatar::DefinitionName
 * The constant string which identifies the definition of details which are avatars
 */
Q_DEFINE_LATIN1_LITERAL(QContactAvatar::DefinitionName, "Avatar");

/*!
 * \variable QContactAddress::DefinitionName
 * The constant string which identifies the definition of details which are street addresses
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::DefinitionName, "StreetAddress");

/*!
 * \variable QContactPhoneNumber::DefinitionName
 * The constant string which identifies the definition of details which are phone numbers
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::DefinitionName, "PhoneNumber");

/*!
 * \variable QContactPresence::DefinitionName
 * The constant string which identifies the definition of details which contain presence information related
 * to an online account
 */
Q_DEFINE_LATIN1_LITERAL(QContactPresence::DefinitionName, "Presence");

/*!
 * \variable QContactSyncTarget::DefinitionName
 * The constant string which identifies the definition of details which are synchronisation target stores
 */
Q_DEFINE_LATIN1_LITERAL(QContactSyncTarget::DefinitionName, "SyncTarget");

/*!
 * \variable QContactTimestamp::DefinitionName
 * The constant string which identifies the definition of details which are contact synchronisation timestamps
 */
Q_DEFINE_LATIN1_LITERAL(QContactTimestamp::DefinitionName, "Timestamp");

/*!
 * \variable QContactType::DefinitionName
 * The constant string which identifies the definition of details which identify the type of the contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactType::DefinitionName, "Type");

/*!
 * \variable QContactGuid::DefinitionName
 * The constant string which identifies the definition of details which are globally unique identifiers
 */
Q_DEFINE_LATIN1_LITERAL(QContactGuid::DefinitionName, "Guid");

/*!
 * \variable QContactEmailAddress::DefinitionName
 * The constant string which identifies the definition of details which are email addresses
 */
Q_DEFINE_LATIN1_LITERAL(QContactEmailAddress::DefinitionName, "EmailAddress");

/*!
 * \variable QContactUrl::DefinitionName
 * The constant string which identifies the definition of details which are universal resource location paths
 */
Q_DEFINE_LATIN1_LITERAL(QContactUrl::DefinitionName, "Url");

/*!
 * \variable QContactBirthday::DefinitionName
 * The constant string which identifies the definition of details which are the dates of birthdays
 */
Q_DEFINE_LATIN1_LITERAL(QContactBirthday::DefinitionName, "Birthday");

/*!
 * \variable QContactAnniversary::DefinitionName
 * The constant string which identifies the definition of details which are anniversary dates
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::DefinitionName, "Anniversary");

/*!
 * \variable QContactGender::DefinitionName
 * The constant string which identifies the definition of details which identify the gender of a contact in a given context
 */
Q_DEFINE_LATIN1_LITERAL(QContactGender::DefinitionName, "Gender");

/*!
 * \variable QContactGeolocation::DefinitionName
 * The constant string which identifies the definition of details which describe a location associated with a contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactGeolocation::DefinitionName, "Geolocation");

/*!
 * \variable QContactOnlineAccount::DefinitionName
 * The constant string which identifies the definition of details which identify the organization to which a contact belongs in a given context
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::DefinitionName, "OnlineAccount");

/*!
 * \variable QContactOrganization::DefinitionName
 * The constant string which identifies the definition of details which identify the organization to which a contact belongs in a given context
 */
Q_DEFINE_LATIN1_LITERAL(QContactOrganization::DefinitionName, "Organization");

/*!
 * \variable QContactDisplayLabel::DefinitionName
 * The constant string which identifies the definition of details which contain a display label of a contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactDisplayLabel::DefinitionName, "DisplayLabel");

/*!
 * \variable QContactRelationship::DefinitionName
 * The constant string which identifies the definition of details which contain information about a relationship of a contact to another contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactRelationship::DefinitionName, "Relationship");


/*!
 * \variable QContactPhoneNumber::FieldNumber
 * The constant key for which the phone number value is stored in details of the QContactPhoneNumber type
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::FieldNumber, "PhoneNumber");

/*!
 * \variable QContactPhoneNumber::FieldSubType
 * The constant key for which the subtypes value is stored in details of the QContactPhoneNumber type
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::FieldSubTypes, "SubTypes");

/*!
 * \variable QContactEmailAddress::FieldEmailAddress
 * The constant key for which the email address value is stored in details of the QContactEmailAddress type
 */
Q_DEFINE_LATIN1_LITERAL(QContactEmailAddress::FieldEmailAddress, "EmailAddress");

/*!
 * \variable QContactGuid::FieldGuid
 * The constant key for which the globally unique identifier value is stored in details of the QContactGuid type
 */
Q_DEFINE_LATIN1_LITERAL(QContactGuid::FieldGuid, "Guid");

/*!
 * \variable QContactSyncTarget::FieldSyncTarget
 * The constant key for which the value of the target store for synchronisation is stored in details of the QContactSyncTarget type
 */
Q_DEFINE_LATIN1_LITERAL(QContactSyncTarget::FieldSyncTarget, "SyncTarget");

/*!
 * \variable QContactAvatar::FieldAvatar
 * The constant key for which the path the avatar value is stored in details of the QContactAvatar type
 */
Q_DEFINE_LATIN1_LITERAL(QContactAvatar::FieldAvatar, "Avatar");

/*!
 * \variable QContactAvatar::FieldSubType
 * The constant key for which the subtypes value is stored in details of the QContactAvatar type
 */
Q_DEFINE_LATIN1_LITERAL(QContactAvatar::FieldSubType, "SubType");

/*!
 * \variable QContactName::FieldPrefix
 * The constant key for which the name prefix value is stored in details of the QContactName type
 */
Q_DEFINE_LATIN1_LITERAL(QContactName::FieldPrefix, "Prefix");

/*!
 * \variable QContactName::FieldFirst
 * The constant key for which the first name value is stored in details of the QContactName type
 */
Q_DEFINE_LATIN1_LITERAL(QContactName::FieldFirst, "First");

/*!
 * \variable QContactName::FieldMiddle
 * The constant key for which the middle name value is stored in details of the QContactName type
 */
Q_DEFINE_LATIN1_LITERAL(QContactName::FieldMiddle, "Middle");

/*!
 * \variable QContactName::FieldLast
 * The constant key for which the last name value is stored in details of the QContactName type
 */
Q_DEFINE_LATIN1_LITERAL(QContactName::FieldLast, "Last");

/*!
 * \variable QContactName::FieldSuffix
 * The constant key for which the name suffix value is stored in details of the QContactName type
 */
Q_DEFINE_LATIN1_LITERAL(QContactName::FieldSuffix, "Suffix");

/*!
 * \variable QContactNickname::FieldNickname
 * The constant key for which the nickname value is stored in details of the QContactNickname type
 */
Q_DEFINE_LATIN1_LITERAL(QContactNickname::FieldNickname, "Nickname");

/*!
 * \variable QContactAddress::FieldStreet
 * The constant key for which the street value is stored in details of the QContactAddress type
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::FieldStreet, "Street");

/*!
 * \variable QContactAddress::FieldLocality
 * The constant key for which the locality value is stored in details of the QContactAddress type
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::FieldLocality, "Locality");

/*!
 * \variable QContactAddress::FieldRegion
 * The constant key for which the region value is stored in details of the QContactAddress type
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::FieldRegion, "Region");

/*!
 * \variable QContactAddress::FieldPostcode
 * The constant key for which the postcode value is stored in details of the QContactAddress type
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::FieldPostcode, "Postcode");

/*!
 * \variable QContactAddress::FieldCountry
 * The constant key for which the country value is stored in details of the QContactAddress type
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::FieldCountry, "Country");

/*!
 * \variable QContactAddress::FieldPostOfficeBox
 * The constant key for which the post office box value is stored in details of the QContactAddress type
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::FieldPostOfficeBox, "PostOfficeBox");

/*!
 * \variable QContactAddress::FieldSubTypes
 * The constant key for which the subtypes value is stored in details of the QContactAddress type
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::FieldSubTypes, "SubTypes");

/*!
 * \variable QContactUrl::FieldUrl
 * The constant key for which the url value is stored in details of the QContactUrl type
 */
Q_DEFINE_LATIN1_LITERAL(QContactUrl::FieldUrl, "Url");

/*!
 * \variable QContactUrl::FieldSubType
 * The constant key for which the subtypes value is stored in details of the QContactUrl type
 */
Q_DEFINE_LATIN1_LITERAL(QContactUrl::FieldSubType, "SubType");

/*!
 * \variable QContactBirthday::FieldBirthday
 * The constant key for which the birthday date value is stored in details of the QContactBirthday type
 */
Q_DEFINE_LATIN1_LITERAL(QContactBirthday::FieldBirthday, "Birthday");

/*!
 * \variable QContactAnniversary::FieldOriginalDate
 * The constant key for which the anniversary original event date value is stored in details of the QContactAnniversary type
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::FieldOriginalDate, "OriginalDate");

/*!
 * \variable QContactAnniversary::FieldEvent
 * The constant key for which the name of the event is stored in details of the QContactAnniversary type
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::FieldEvent, "Event");

/*!
 * \variable QContactAnniversary::FieldCalendarId
 * The constant key for which the value of the identifier of the associated calendar entry is stored in details of the QContactAnniversary type
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::FieldCalendarId, "CalendarId");

/*!
 * \variable QContactAnniversary::FieldSubType
 * The constant key for which the subtypes value is stored in details of the QContactAnniversary type
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::FieldSubType, "SubType");

/*!
 * \variable QContactGender::FieldGender
 * The constant key for which the gender value is stored in details of the QContactGender type
 */
Q_DEFINE_LATIN1_LITERAL(QContactGender::FieldGender, "Gender");

/*!
 * \variable QContactGender::GenderMale
 * The value that identifies this contact as being male
 */
Q_DEFINE_LATIN1_LITERAL(QContactGender::GenderMale, "Male");

/*!
 * \variable QContactGender::GenderFemale
 * The value that identifies this contact as being female
 */
Q_DEFINE_LATIN1_LITERAL(QContactGender::GenderFemale, "Female");

/*!
 * \variable QContactGender::GenderUnspecified
 * The value that identifies this contact as being of unspecified gender
 */
Q_DEFINE_LATIN1_LITERAL(QContactGender::GenderUnspecified, "Unspecified");

/*!
 * \variable QContactGeolocation::FieldLabel
 * The constant key for which the location label value is stored in details of the QContactGeolocation type
 */
Q_DEFINE_LATIN1_LITERAL(QContactGeolocation::FieldLabel, "Label");

/*!
 * \variable QContactGeolocation::FieldLatitude
 * The constant key for which the latitude value is stored in details of the QContactGeolocation type
 */
Q_DEFINE_LATIN1_LITERAL(QContactGeolocation::FieldLatitude, "Latitude");

/*!
 * \variable QContactGeolocation::FieldLongitude
 * The constant key for which the longitude value is stored in details of the QContactGeolocation type
 */
Q_DEFINE_LATIN1_LITERAL(QContactGeolocation::FieldLongitude, "Longitude");

/*!
 * \variable QContactGeolocation::FieldAccuracy
 * The constant key for which the location accuracy value is stored in details of the QContactGeolocation type
 */
Q_DEFINE_LATIN1_LITERAL(QContactGeolocation::FieldAccuracy, "Accuracy");

/*!
 * \variable QContactGeolocation::FieldAltitude
 * The constant key for which the altitude value is stored in details of the QContactGeolocation type
 */
Q_DEFINE_LATIN1_LITERAL(QContactGeolocation::FieldAltitude, "Altitude");


/*!
 * \variable QContactGeolocation::FieldAltitudeAccuracy
 * The constant key for which the altitude accuracy value is stored in details of the QContactGeolocation type
 */
Q_DEFINE_LATIN1_LITERAL(QContactGeolocation::FieldAltitudeAccuracy, "AltitudeAccuracy");

/*!
 * \variable QContactGeolocation::FieldHeading
 * The constant key for which the heading value is stored in details of the QContactGeolocation type
 */
Q_DEFINE_LATIN1_LITERAL(QContactGeolocation::FieldHeading, "Heading");

/*!
 * \variable QContactGeolocation::FieldSpeed
 * The constant key for which the speed value is stored in details of the QContactGeolocation type
 */
Q_DEFINE_LATIN1_LITERAL(QContactGeolocation::FieldSpeed, "Speed");

/*!
 * \variable QContactGeolocation::FieldTimestamp
 * The constant key for which the timestamp value is stored in details of the QContactGeolocation type
 */
Q_DEFINE_LATIN1_LITERAL(QContactGeolocation::FieldTimestamp, "Timestamp");

/*!
 * \variable QContactOnlineAccount::FieldAccountUri
 * The constant key for which the remote account uri value is stored in details of the QContactOnlineAccount type
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::FieldAccountUri, "AccountUri");

/*!
 * \variable QContactOnlineAccount::FieldSubTypes
 * The constant key for which the subtypes value is stored in details of the QContactOnlineAccount type
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::FieldSubTypes, "SubTypes");

/*!
 * \variable QContactOnlineAccount::SubTypeSip
 * The constant attribute value which describes the online account as supporting SIP
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::SubTypeSip, "Sip");

/*!
 * \variable QContactOnlineAccount::SubTypeH323
 * The constant attribute value which describes the online account as supporting H323
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::SubTypeH323, "H323");

/*!
 * \variable QContactOnlineAccount::SubTypeXmpp
 * The constant attribute value which describes the online account as supporting XMPP
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::SubTypeXmpp, "Xmpp");

/*!
 * \variable QContactOnlineAccount::SubTypeInternet
 * The constant attribute value which describes the online account as supporting internet sessions
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::SubTypeInternet, "Internet");

/*!
 * \variable QContactOnlineAccount::SubTypeShareVideo
 * The constant attribute value which describes the online account as supporting ShareVideo
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::SubTypeShareVideo, "ShareVideo");

/*!
 * \variable QContactOrganization::FieldName
 * The constant key for which the name value is stored in details of the QContactOrganization type
 */
Q_DEFINE_LATIN1_LITERAL(QContactOrganization::FieldName, "Name");

/*!
 * \variable QContactOrganization::FieldLogo
 * The constant key for which the logo path value is stored in details of the QContactOrganization type
 */
Q_DEFINE_LATIN1_LITERAL(QContactOrganization::FieldLogo, "Logo");

/*!
 * \variable QContactOrganization::FieldDepartment
 * The constant key for which the organization's department value is stored in details of the QContactOrganization type
 */
Q_DEFINE_LATIN1_LITERAL(QContactOrganization::FieldDepartment, "Department");

/*!
 * \variable QContactOrganization::FieldLocation
 * The constant key for which the organization's location (or the location of the contact's part of the organization) value is stored in details of the QContactOrganization type
 */
Q_DEFINE_LATIN1_LITERAL(QContactOrganization::FieldLocation, "Location");

/*!
 * \variable QContactOrganization::FieldTitle
 * The constant key for which the contact's title within the organization is stored in details of the QContactOrganization type
 */
Q_DEFINE_LATIN1_LITERAL(QContactOrganization::FieldTitle, "Title");

/*!
 * \variable QContactPresence::FieldAccountUri
 * The constant key for which the remote account uri value is stored in details of the QContactPresence type
 */
Q_DEFINE_LATIN1_LITERAL(QContactPresence::FieldAccountUri, "AccountUri");

/*!
 * \variable QContactPresence::FieldNickname
 * The constant key for which the nickname value is stored in details of the QContactPresence type
 */
Q_DEFINE_LATIN1_LITERAL(QContactPresence::FieldNickname, "Nickname");

/*!
 * \variable QContactPresence::FieldPresence
 * The constant key for which the presence value is stored in details of the QContactPresence type
 */
Q_DEFINE_LATIN1_LITERAL(QContactPresence::FieldPresence, "Presence");

/*!
 * \variable QContactPresence::PresenceAvailable
 * The value for presence which specifies that the contact's current status is available
 */
Q_DEFINE_LATIN1_LITERAL(QContactPresence::PresenceAvailable, "Available");

/*!
 * \variable QContactPresence::PresenceHidden
 * The value for presence which specifies that the contact's current status is hidden
 */
Q_DEFINE_LATIN1_LITERAL(QContactPresence::PresenceHidden, "Hidden");

/*!
 * \variable QContactPresence::PresenceBusy
 * The value for presence which specifies that the contact's current status is busy
 */
Q_DEFINE_LATIN1_LITERAL(QContactPresence::PresenceBusy, "Busy");

/*!
 * \variable QContactPresence::PresenceAway
 * The value for presence which specifies that the contact's current status is away
 */
Q_DEFINE_LATIN1_LITERAL(QContactPresence::PresenceAway, "Away");

/*!
 * \variable QContactPresence::PresenceExtendedAway
 * The value for presence which specifies that the contact's current status is extended away
 */
Q_DEFINE_LATIN1_LITERAL(QContactPresence::PresenceExtendedAway, "ExtendedAway");

/*!
 * \variable QContactPresence::PresenceUnknown
 * The value for presence which specifies that the contact's current status is unknown
 */
Q_DEFINE_LATIN1_LITERAL(QContactPresence::PresenceUnknown, "Unknown");

/*!
 * \variable QContactPresence::PresenceOffline
 * The value for presence which specifies that the contact's current status is offline
 */
Q_DEFINE_LATIN1_LITERAL(QContactPresence::PresenceOffline, "Offline");

/*!
 * \variable QContactPresence::FieldStatusMessage
 * The constant key for which the status message value is stored in details of the QContactPresence type
 */
Q_DEFINE_LATIN1_LITERAL(QContactPresence::FieldStatusMessage, "StatusMessage");

/*!
 * \variable QContactDisplayLabel::FieldLabel
 * The constant key for which the display label value is stored in details of the QContactDisplayLabel type
 */
Q_DEFINE_LATIN1_LITERAL(QContactDisplayLabel::FieldLabel, "Label");

/*!
 * \variable QContactDisplayLabel::FieldSynthesised
 * The constant key for the value which is stored in details of the QContactDisplayLabel type which describes whether the label was synthesised by a QContactManager or set manually.
 */
Q_DEFINE_LATIN1_LITERAL(QContactDisplayLabel::FieldSynthesised, "Synthesised");

/*!
 * \variable QContactRelationship::FieldRelationshipType
 * The constant key for the value which is stored in details of the QContactRelationship type which defines the type of relationship that is being described in the detail
 */
Q_DEFINE_LATIN1_LITERAL(QContactRelationship::FieldRelationshipType, "RelationshipType");

/*!
 * \variable QContactRelationship::RelationshipTypeIsAggregatedBy
 * The value for the relationship type which identifies this contact as being aggregated by the other contact into a master contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactRelationship::RelationshipTypeIsAggregatedBy, "IsAggregatedBy");

/*!
 * \variable QContactRelationship::RelationshipTypeAggregates
 * The value for the relationship type which identifies this contact as aggregating the other contact (and possibly others) into a master contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactRelationship::RelationshipTypeAggregates, "Aggregates");

/*!
 * \variable QContactRelationship::RelationshipTypeHasMember
 * The value for the relationship type which identifies this contact as a group which has the other contact as a member
 */
Q_DEFINE_LATIN1_LITERAL(QContactRelationship::RelationshipTypeHasMember, "HasMember");

/*!
 * \variable QContactRelationship::RelationshipTypeIsMemberOf
 * The value for the relationship type which identifies this contact as being a member of the group represented by the other contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactRelationship::RelationshipTypeIsMemberOf, "IsMemberOf");

/*!
 * \variable QContactRelationship::RelationshipTypeIs
 * The value for the relationship type which identifies this contact as being the other contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactRelationship::RelationshipTypeIs, "Is");

/*!
 * \variable QContactRelationship::RelationshipTypeAssistant
 * The value for the relationship type which identifies this contact as being the assistant of the other contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactRelationship::RelationshipTypeAssistant, "Assistant");

/*!
 * \variable QContactRelationship::RelationshipTypeManager
 * The value for the relationship type which identifies this contact as being the manager of the other contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactRelationship::RelationshipTypeManager, "Manager");

/*!
 * \variable QContactRelationship::RelationshipTypeSpouse
 * The value for the relationship type which identifies this contact as being the spouse of the other contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactRelationship::RelationshipTypeSpouse, "Spouse");

/*!
 * \variable QContactRelationship::FieldRelatedContactLabel
 * The constant key for the value which is stored in details of the QContactRelationship type which contains the label of the other contact with whom this contact has the relationship
 */
Q_DEFINE_LATIN1_LITERAL(QContactRelationship::FieldRelatedContactLabel, "RelatedContactLabel");

/*!
 * \variable QContactRelationship::FieldRelatedContactId
 * The constant key for the value which is stored in details of the QContactRelationship type which defines the unique identifier of the other contact to whom this contact has the relationship
 */
Q_DEFINE_LATIN1_LITERAL(QContactRelationship::FieldRelatedContactId, "RelatedContactId");

/*!
 * \variable QContactRelationship::FieldRelatedContactManagerUri
 * The constant key for the value which is stored in details of the QContactRelationship type which defines the universal resource identifier of the manager which contains the contact to whom this contact has the relationship
 */
Q_DEFINE_LATIN1_LITERAL(QContactRelationship::FieldRelatedContactManagerUri, "RelatedContactManagerUri");

/*!
 * \variable QContactTimestamp::FieldModificationTimestamp
 * The constant key for the value which is stored in details of the QContactTimestamp type which describes the last modification date and time of a contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactTimestamp::FieldModificationTimestamp, "ModificationTimestamp");

/*!
 * \variable QContactTimestamp::FieldCreationTimestamp
 * The constant key for the value which is stored in details of the QContactTimestamp type which describes the creation date and time of a contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactTimestamp::FieldCreationTimestamp, "CreationTimestamp");

/*!
 * \variable QContactType::FieldType
 * The constant key for the type value which is stored in details of the QContactType definition
 */
Q_DEFINE_LATIN1_LITERAL(QContactType::FieldType, "Type");


/*!
 * \variable QContactType::TypeContact
 * The constant attribute value which describes the contact as being an ordinary contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactType::TypeContact, "Contact");

/*!
 * \variable QContactType::TypeGroup
 * The constant attribute value which describes the contact as being a group
 */
Q_DEFINE_LATIN1_LITERAL(QContactType::TypeGroup, "Group");

/*!
 * \variable QContactPhoneNumber::SubTypeLandline
 * The constant attribute value which describes the phone number as identifying a landline phone
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeLandline, "Landline");

/*!
 * \variable QContactPhoneNumber::SubTypeMobile
 * The constant attribute value which describes the phone number as identifying a mobile phone
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeMobile, "Mobile");

/*!
 * \variable QContactPhoneNumber::SubTypeFacsimile
 * The constant attribute value which describes the phone number as identifying a facsimile machine
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeFacsimile, "Facsimile");

/*!
 * \variable QContactPhoneNumber::SubTypePager
 * The constant attribute value which describes the phone number as identifying a pager device
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypePager, "Pager");

/*!
 * \variable QContactPhoneNumber::SubTypeCar
 * The constant attribute value which describes the phone number as identifying a car phone
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeCar, "Car");

/*!
 * \variable QContactPhoneNumber::SubTypeBulletinBoardSystem
 * The constant attribute value which describes the phone number as identifying a bulletin board system
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeBulletinBoardSystem, "BulletinBoardSystem");

/*!
 * \variable QContactPhoneNumber::SubTypeVoice
 * The constant attribute value which describes the phone number as supporting voice transmission
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeVoice, "Voice");

/*!
 * \variable QContactPhoneNumber::SubTypeModem
 * The constant attribute value which describes the phone number as supporting digital data transfer
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeModem, "Modem");

/*!
 * \variable QContactPhoneNumber::SubTypeVideo
 * The constant attribute value which describes the phone number as supporting video transmission
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeVideo, "Video");

/*!
 * \variable QContactPhoneNumber::SubTypeMessagingCapable
 * The constant attribute value which describes the phone number as supporting messaging services
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeMessagingCapable, "MessagingCapable");

/*!
 * \variable QContactPhoneNumber::SubTypeAssistant
 * The constant attribute value which describes the phone number as an assistant phone number
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeAssistant, "Assistant");

/*!
 * \variable QContactPhoneNumber::SubTypeDtmfMenu
 * The constant attribute value which describes the phone number as supporting DTMF-controlled electronic menu navigation
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeDtmfMenu, "DtmfMenu");

/*!
 * \variable QContactAddress::SubTypeParcel
 * The constant attribute value which describes the address as being an address for parcel delivery
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::SubTypeParcel, "Parcel");

/*!
 * \variable QContactAddress::SubTypePostal
 * The constant attribute value which describes the address as being an address for postal delivery
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::SubTypePostal, "Postal");

/*!
 * \variable QContactAddress::SubTypeDomestic
 * The constant attribute value which describes the address as being a domestic address
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::SubTypeDomestic, "Domestic");

/*!
 * \variable QContactAddress::SubTypeInternational
 * The constant attribute value which describes the address as being an international address
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::SubTypeInternational, "International");

/*!
 * \variable QContactAvatar::SubTypeImage
 * The constant attribute value which describes the avatar as being an image
 */
Q_DEFINE_LATIN1_LITERAL(QContactAvatar::SubTypeImage, "Image");

/*!
 * \variable QContactAvatar::SubTypeVideo
 * The constant attribute value which describes the avatar as being a video
 */
Q_DEFINE_LATIN1_LITERAL(QContactAvatar::SubTypeVideo, "Video");

/*!
 * \variable QContactAvatar::SubTypeTexturedMesh
 * The constant attribute value which describes the avatar as being a textured, 3D mesh
 */
Q_DEFINE_LATIN1_LITERAL(QContactAvatar::SubTypeTexturedMesh, "TexturedMesh");

/*!
 * \variable QContactUrl::SubTypeHomePage
 * The constant attribute value which describes the url as being the homepage of the contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactUrl::SubTypeHomePage, "HomePage");

/*!
 * \variable QContactUrl::SubTypeFavourite
 * The constant attribute value which describes the url as being a favourite page of the contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactUrl::SubTypeFavourite, "Favourite");

/*!
 * \variable QContactAnniversary::SubTypeWedding
 * The constant attribute value which describes the anniversary as being a wedding anniversary
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::SubTypeWedding, "Wedding");

/*!
 * \variable QContactAnniversary::SubTypeEngagement
 * The constant attribute value which describes the anniversary as being an engagement anniversary
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::SubTypeEngagement, "Engagement");

/*!
 * \variable QContactAnniversary::SubTypeHouse
 * The constant attribute value which describes the anniversary as being an anniversary of ownership of a particular residence
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::SubTypeHouse, "House");

/*!
 * \variable QContactAnniversary::SubTypeEmployment
 * The constant attribute value which describes the anniversary as being an anniversary of employment at a particular company
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::SubTypeEmployment, "Employment");

/*!
 * \variable QContactAnniversary::SubTypeMemorial
 * The constant attribute value which describes the anniversary as being an anniversary of an event of sentimental significance
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::SubTypeMemorial, "Memorial");


/*!
 * \fn QContactSyncTarget::syncTarget() const
 * Returns the identifier of the backend store to which the contact containing this detail should be synchronised
 */

/*!
 * \fn QContactSyncTarget::setSyncTarget(const QString& syncTarget)
 * Sets the identifier of the backend store to which the contact containing this detail should be synchronised to \a syncTarget
 */

/*!
 * \fn QContactEmailAddress::emailAddress() const
 * Returns the email address of the contact which is stored in this detail
 */

/*!
 * \fn QContactEmailAddress::setEmailAddress(const QString& emailAddress)
 * Sets the email address of the contact which is stored in this detail to \a emailAddress
 */

/*!
 * \fn QContactAnniversary::originalDate() const
 * Returns the original date of occurrance of the event stored in this detail
 */

/*!
 * \fn QContactAnniversary::setOriginalDate(const QDate& date)
 * Sets the original date of occurrance of the event stored in this detail to \a date
 */

/*!
 * \fn QContactAnniversary::calendarId() const
 * Returns the identifier of the calendar entry associated with this anniversary
 */

/*!
 * \fn QContactAnniversary::setCalendarId(const QString& calendarId)
 * Sets the identifier of the calendar entry associated with this anniversary to \a calendarId
 */

/*!
 * \fn QContactAnniversary::event() const
 * Returns the name of the event for which this detail contains information
 */

/*!
 * \fn QContactAnniversary::setEvent(const QString& event)
 * Sets the name of the event for which this detail contains information to \a event
 */

/*!
 * \fn QContactAnniversary::setSubType(const QString& subType)
 * Sets the subtype which this detail implements to be the given \a subType
 */

/*!
 * \fn QContactAnniversary::subType() const
 * Returns the subtype that this detail implements, if defined
 */

/*!
 * \fn QContactAvatar::avatar() const
 * Returns the location of an avatar file associated with the contact
 */

/*!
 * \fn QContactAvatar::setAvatar(const QString& avatar)
 * Sets the location of an avatar file associated with the contact to \a avatar
 */

/*!
 * \fn QContactAvatar::setSubType(const QString& subType)
 * Sets the subtype which this detail implements to be the given \a subType
 */

/*!
 * \fn QContactAvatar::subType() const
 * Returns the subtype that this detail implements, if defined
 */

/*!
 * \fn QContactAddress::postOfficeBox() const
 * Returns the post office box segment of the address stored in this detail
 */

/*!
 * \fn QContactAddress::setPostOfficeBox(const QString& postOfficeBox)
 * Sets the post office box segment of the address stored in this detail to \a postOfficeBox
 */

/*!
 * \fn QContactAddress::street() const
 * Returns the street segment of the address stored in this detail
 */

/*!
 * \fn QContactAddress::setStreet(const QString& street)
 * Sets the street segment of the address stored in this detail to \a street
 */

/*!
 * \fn QContactAddress::locality() const
 * Returns the locality segment of the address stored in this detail
 */

/*!
 * \fn QContactAddress::setLocality(const QString& locality)
 * Sets the locality segment of the address stored in this detail to \a locality
 */

/*!
 * \fn QContactAddress::region() const
 * Returns the region segment of the address stored in this detail
 */

/*!
 * \fn QContactAddress::setRegion(const QString& region)
 * Sets the region segment of the address stored in this detail to \a region
 */

/*!
 * \fn QContactAddress::postcode() const
 * Returns the postcode segment of the address stored in this detail
 */

/*!
 * \fn QContactAddress::setPostcode(const QString& postcode)
 * Sets the postcode segment of the address stored in this detail to \a postcode
 */

/*!
 * \fn QContactAddress::country() const
 * Returns the country segment of the address stored in this detail
 */

/*!
 * \fn QContactAddress::setCountry(const QString& country)
 * Sets the country segment of the address stored in this detail to \a country
 */

/*!
 * \fn QContactAddress::setSubTypes(const QStringList& subTypes)
 * Sets the subtypes which this detail implements to be those contained in the list of given \a subTypes
 */

/*!
 * \fn QContactAddress::setSubTypes(const QString& subType)
 * Sets the subtypes which this detail implements to be just the given \a subType
 */

/*!
 * \fn QContactAddress::subTypes() const
 * Returns the list of subtypes that this detail implements
 */

/*!
 * \fn QContactUrl::url() const
 * Returns the url stored in this detail
 */

/*!
 * \fn QContactUrl::setUrl(const QString& url)
 * Sets the url stored in this detail to \a url
 */

/*!
 * \fn QContactUrl::setSubType(const QString& subType)
 * Sets the subtype which this detail implements to be the given \a subType
 */

/*!
 * \fn QContactUrl::subType() const
 * Returns the subtype that this detail implements, if defined
 */

/*!
 * \fn QContactPhoneNumber::number() const
 * Returns the phone number stored in this detail
 */

/*!
 * \fn QContactPhoneNumber::setNumber(const QString& number)
 * Sets the phone number stored in this detail to \a number
 */

/*!
 * \fn QContactPhoneNumber::setSubTypes(const QStringList& subTypes)
 * Sets the subtypes which this detail implements to be those contained in the list of given \a subTypes
 */

/*!
 * \fn QContactPhoneNumber::setSubTypes(const QString& subType)
 * Sets the subtypes which this detail implements to be just the given \a subType
 */

/*!
 * \fn QContactPhoneNumber::subTypes() const
 * Returns the list of subtypes that this detail implements
 */

/*!
 * \fn QContactBirthday::date() const
 * Returns the date of the birthday which is stored in this detail
 */

/*!
 * \fn QContactBirthday::setDate(const QDate& date)
 * Sets the date of the birthday which is stored in this detail to \a date
 */

/*!
 * \fn QContactGender::gender() const
 * Returns the gender of the contact, as stored in this detail.  The possible values for the value stored are "Male", "Female" and "Unspecified"
 */

/*!
 * \fn QContactGender::setGender(const QString& gender)
 * Sets the gender of the contact (as stored in this detail) to \a gender, if \a gender is either "Male" or "Female", otherwise sets it to "Unspecified"
 */

/*!
 * \fn QContactGeolocation::setLabel(const QString& label)
 * Sets the label of the location stored in the detail to \a label
 */

/*!
 * \fn QContactGeolocation::label() const
 * Returns the label of the location stored in the detail
 */

/*!
 * \fn QContactGeolocation::setLatitude(double latitude)
 * Sets the latitude portion of the coordinate (in decimal degrees) of the location stored in the detail to \a latitude
 */

/*!
 * \fn QContactGeolocation::latitude() const
 * Returns the latitude portion of the coordinate (specified in decimal degrees) of the location stored in the detail
 */

/*!
 * \fn QContactGeolocation::setLongitude(double longitude)
 * Sets the longitude portion of the coordinate (in decimal degrees) of the location stored in the detail to \a longitude
 */

/*!
 * \fn QContactGeolocation::longitude() const
 * Returns the longitude portion of the coordinate (specified in decimal degrees) of the location stored in the detail
 */

/*!
 * \fn QContactGeolocation::setAccuracy(double accuracy)
 * Specifies that the latitude and longitude portions of the location stored in the detail are accurate to within \a accuracy metres
 */

/*!
 * \fn QContactGeolocation::accuracy() const
 * Returns the accuracy (in metres) of the latitude and longitude of the location stored in the detail
 */

/*!
 * \fn QContactGeolocation::setAltitude(double altitude)
 * Sets the altitude portion of the coordinate (in metres above the ellipsoid) of the location stored in the detail to \a altitude
 */

/*!
 * \fn QContactGeolocation::altitude() const
 * Returns the altitude (in metres) of the location stored in the detail
 */

/*!
 * \fn QContactGeolocation::setAltitudeAccuracy(double altitudeAccuracy)
 * Sets the altitude-accuracy portion of the coordinate (in metres) of the location stored in the detail to \a altitudeAccuracy
 */

/*!
 * \fn QContactGeolocation::altitudeAccuracy() const
 * Returns the accuracy of the altitude portion of the location stored in the detail
 */

/*!
 * \fn QContactGeolocation::setHeading(double heading)
 * Sets the heading portion of the coordinate (in decimal degrees clockwise relative to true north) of the location-aware device at the time of measurement to \a heading
 */

/*!
 * \fn QContactGeolocation::heading() const
 * Returns the heading (at the time of measurement) of the location-aware device that recorded (or was provided) the measurement
 */

/*!
 * \fn QContactGeolocation::setSpeed(double speed)
 * Sets the speed portion of the coordinate (in metres per second) of the location-aware device at the time of measurement to \a speed
 */

/*!
 * \fn QContactGeolocation::speed() const
 * Returns the speed (at the time of measurement) of the location-aware device that recorded (or was provided) the measurement
 */

/*!
 * \fn QContactGeolocation::setTimestamp(const QDateTime& timestamp)
 * Sets the creation (or first-valid) timestamp of the location information to \a timestamp
 */

/*!
 * \fn QContactGeolocation::timestamp() const
 * Returns the timestamp associated with the location stored in the detail
 */

/*!
 * \fn QContactGuid::guid() const
 * Returns the globally unique identifier which is stored in this detail
 */

/*!
 * \fn QContactGuid::setGuid(const QString& guid)
 * Sets the globally unique identifier which is stored in this detail to \a guid
 */

/*!
 * \fn QContactName::prefix() const
 * Returns the prefix segment of the name stored in this detail
 */

/*!
 * \fn QContactName::setPrefix(const QString& prefix)
 * Sets the prefix segment of the name stored in this detail to \a prefix
 */

/*!
 * \fn QContactName::first() const
 * Returns the first (given) name segment of the name stored in this detail
 */

/*!
 * \fn QContactName::setFirst(const QString& first)
 * Sets the first name segment of the name stored in this detail to \a first
 */

/*!
 * \fn QContactName::middle() const
 * Returns the middle (additional, or other) name segment of the name stored in this detail
 */

/*!
 * \fn QContactName::setMiddle(const QString& middle)
 * Sets the middle name segment of the name stored in this detail to \a middle
 */

/*!
 * \fn QContactName::last() const
 * Returns the last (family, or surname) name segment of the name stored in this detail
 */

/*!
 * \fn QContactName::setLast(const QString& last)
 * Sets the last name segment of the name stored in this detail to \a last
 */

/*!
 * \fn QContactName::suffix() const
 * Returns the suffix segment of the name stored in this detail
 */

/*!
 * \fn QContactName::setSuffix(const QString& suffix)
 * Sets the suffix segment of the name stored in this detail to \a suffix
 */

/*!
 * \fn QContactNickname::setNickname(const QString& nickname)
 * Sets the nickname of the contact which is stored in this detail to \a nickname
 */

/*!
 * \fn QContactNickname::nickname() const
 * Returns the nickname of the contact which is stored in this detail
 */

/*!
 * \fn QContactTimestamp::created() const
 * Returns the creation timestamp saved in this detail
 */

/*!
 * \fn QContactTimestamp::lastModified() const
 * Returns the last-modified timestamp saved in this detail
 */

/*!
 * \fn QContactTimestamp::setCreated(const QDateTime& dateTime)
 * Sets the creation timestamp saved in this detail to \a dateTime
 */

/*!
 * \fn QContactTimestamp::setLastModified(const QDateTime& dateTime)
 * Sets the last-modified timestamp saved in this detail to \a dateTime
 */

/*!
 * \fn QContactType::type() const
 * Returns the contact type value stored in this detail
 */

/*!
 * \fn QContactType::setType(const QString& type)
 * Sets the type of the contact to be the give \a type
 */

/*!
 * \fn QContactRelationship::setRelationshipType(const QString& relationshipType)
 * Sets the relationship type saved in this detail to \a relationshipType
 */

/*!
 * \fn QContactRelationship::relationshipType() const
 * Returns the relationship type saved in this detail
 */

/*!
 * \fn QContactRelationship::setRelatedContactLabel(const QString& label)
 * Sets the label for the contact to whom this contact has a relationship with to \a label.
 * This is especially useful if no unique identifier (or other information) for the related contact is known.
 */

/*!
 * \fn QContactRelationship::relatedContactLabel() const
 * Returns the label for the contact to whom this contact has the relationship
 */

/*!
 * \fn QContactRelationship::setRelatedContactId(const QUniqueId& id)
 * Sets the value of the unique identifier of the contact to whom this contact has a relationship with to \a id
 */

/*!
 * \fn QContactRelationship::relatedContactId() const
 * Returns the unique identifier of the contact to whom this contact has the relationship
 */

/*!
 * \fn QContactRelationship::setRelatedContactManagerUri(const QString& managerUri)
 * Sets the value of the universal resource identifier of the manager containing the contact to whom this contact has a relationship with to \a managerUri
 */

/*!
 * \fn QContactRelationship::relatedContactManagerUri() const
 * Returns the universal resource identifier of the manager which contains the contact with whom this contact has a relationship
 */

/*!
 * \fn QContactDisplayLabel::isSynthesised() const
 * Returns true if the display label was synthesised automatically by the manager backend
 */

/*!
 * \fn QContactDisplayLabel::label() const
 * Returns the display label of the contact
 */

/*!
 * \fn QContactDisplayLabel::setLabel(const QString& label)
 * Sets the display label of the contact to \a label
 */

/*!
 * \fn QContactDisplayLabel::setSynthesised(bool synthesised)
 * Sets the synthetic status of the display label to \a synthesised
 */

/*!
 * \fn QContactOnlineAccount::setAccountUri(const QString& accountUri)
 * Sets the universal resource identifier of the contact's online account to \a accountUri
 */

/*!
 * \fn QContactOnlineAccount::accountUri() const
 * Returns the universal resource identifier of the online account of the contact
 */

/*!
 * \fn QContactOnlineAccount::setSubTypes(const QStringList& subTypes)
 * Sets the subtypes which this detail implements to be those contained in the list of given \a subTypes
 */

/*!
 * \fn QContactOnlineAccount::setSubTypes(const QString& subType)
 * Sets the subtypes which this detail implements to be just the given \a subType
 */

/*!
 * \fn QContactOnlineAccount::subTypes() const
 * Returns the list of subtypes that this detail implements
 */

/*!
 * \fn QContactOrganization::setName(const QString& name)
 * Sets the name of the organization stored in this detail to \a name
 */

/*!
 * \fn QContactOrganization::name() const
 * Returns the name of the organization stored in this detail
 */

/*!
 * \fn QContactOrganization::setLogo(const QString& logo)
 * Sets the logo of the organization stored in this detail to \a logo
 */

/*!
 * \fn QContactOrganization::logo() const
 * Returns the logo of the organization stored in this detail
 */


/*!
 * \fn QContactOrganization::setDepartment(const QString& department)
 * Sets the contact's department of the organization stored in this detail to \a department
 */

/*!
 * \fn QContactOrganization::department() const
 * Returns the contact's department stored in this detail
 */


/*!
 * \fn QContactOrganization::setLocation(const QString& location)
 * Sets the location (e.g. city or suburb) of the organization stored in this detail to \a location
 */

/*!
 * \fn QContactOrganization::location() const
 * Returns the location of the organization stored in this detail
 */


/*!
 * \fn QContactOrganization::setTitle(const QString& title)
 * Sets the contact's title within the organization stored in this detail to \a title
 */

/*!
 * \fn QContactOrganization::title() const
 * Returns the contact's title within the organization stored in this detail
 */

/*!
 * \fn QContactPresence::setAccountUri(const QString& accountUri)
 * Sets the universal resource identifier of the contact's online account about which
 * this detail stores presence information to \a accountUri
 */

/*!
 * \fn QContactPresence::accountUri() const
 * Returns the universal resource identifier of the online account of the contact about which
 * this detail stores presence information
 */

/*!
 * \fn QContactPresence::setNickname(const QString& nickname)
 * Sets the last-known nickname used by the contact during communications via the online account
 * about which this detail stores presence information to \a nickname
 */

/*!
 * \fn QContactPresence::nickname() const
 * Returns the last-known nickname used by the contact during communications via the online account
 * about which this detail stores presence information
 */

/*!
 * \fn QContactPresence::setPresence(const QString& presence)
 * Sets the presence of the online account according to the presence information provider to \a presence
 */

/*!
 * \fn QContactPresence::presence() const
 * Returns the presence of the online account according to the presence provider
 */

/*!
 * \fn QContactPresence::setStatusMessage(const QString& statusMessage)
 * Sets the last-known status message of the contact which was communicated via the online account about
 * which this detail stores presence information to \a statusMessage
 */

/*!
 * \fn QContactPresence::statusMessage() const
 * Returns the last-known status message of the contact which was communicated via the online account about
 * which this detail stores presence information
 */

