#include <QSharedData>
#include <QTimer>
#include<QtCore>

#include "qcontactandroidbackend_p.h"
#include"qcontactabook_p.h"

QContactManagerEngine* ContactAndroidFactory::engine(const QMap<QString, QString>& parameters, QContactManager::Error* error)
{
    Q_UNUSED(parameters);
    Q_UNUSED(error);
    return new QContactAndroidEngine(); //FIXME Wonderfull memory leak :D
}

QString ContactAndroidFactory::managerName() const
{
    return QString("android");
}

Q_EXPORT_PLUGIN2(qtcontacts_Android, ContactAndroidFactory);
/*! Constructs a new invalid contacts backend. */
QContactAndroidEngine::QContactAndroidEngine() : d(new QContactAndroidEngineData)
{
    QContactABook *abook = d->m_abook;
    connect(abook, SIGNAL(contactsAdded(const QList<QContactLocalId>&)), SIGNAL(contactsAdded(const QList<QContactLocalId>&)));
    connect(abook, SIGNAL(contactsChanged(const QList<QContactLocalId>&)), SIGNAL(contactsChanged(const QList<QContactLocalId>&)));
    connect(abook, SIGNAL(contactsRemoved(const QList<QContactLocalId>&)), SIGNAL(contactsRemoved(const QList<QContactLocalId>&)));
}
/*! \reimp */
QContactAndroidEngine& QContactAndroidEngine::operator=(const QContactAndroidEngine& other)
                                                       {
    d = other.d;
    return *this;
}

/*! \reimp */
QString QContactAndroidEngine::managerName() const
{
    return QString("Android Froyo");
}

/* Synthesise the display label of a contact */
QString QContactAndroidEngine::synthesizedDisplayLabel(const QContact& contact, QContactManager::Error* error) const
{
    QString label;
    label = d->m_abook->getDisplayName(contact);
    // Synthesise the display label for not saved contacts
    // A. FirstName + LastName
    if (label.isEmpty())
    {
        QContactName name = contact.detail(QContactName::DefinitionName);
        QStringList nameList;

        nameList << name.firstName();
        if (name.lastName().count()){
            nameList << name.lastName();
        }

        label = nameList.join(QString(' '));
    }
    // B. Email
    if (label.isEmpty()){
        QContactEmailAddress email = contact.detail(QContactEmailAddress::DefinitionName);
        label = email.emailAddress();
    }
    //
    if (label.isEmpty()){
        *error = QContactManager::UnspecifiedError;
        return QString("No name");
    }
    *error = QContactManager::NoError;
    return label;
}

bool QContactAndroidEngine::validateContact(const QContact& contact, QContactManager::Error* error) const
{
    return QContactManagerEngine::validateContact(contact, error);
}

bool QContactAndroidEngine::validateDefinition(const QContactDetailDefinition& definition, QContactManager::Error* error) const
{
    QContactDetailDefinition existing = detailDefinition(definition.name(), QContactType::TypeContact, error);
    if (existing == definition) {
        *error = QContactManager::NoError;
        return true;
    }
    *error = QContactManager::NotSupportedError; // XXX TODO: mutable definitions?
    return false;
}

QContact QContactAndroidEngine::compatibleContact(const QContact& contact, QContactManager::Error* error) const
{
    return QContactManagerEngine::compatibleContact(contact, error);
}

QContactLocalId QContactAndroidEngine::selfContactId(QContactManager::Error* error) const
{
    Q_CHECK_PTR(d->m_abook);

    return d->m_abook->selfContactId(error);
}

QList<QContactLocalId> QContactAndroidEngine::contactIds(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error* error) const
{
    Q_CHECK_PTR(d->m_abook);

    QList<QContactLocalId> rtn;

    // do this naively for now...
    QContactManager::Error tempError = QContactManager::NoError;
    QList<QContactLocalId> allIds = d->m_abook->contactIds(filter, sortOrders, error);
    QList<QContact> sortedAndFiltered;

    foreach (const QContactLocalId& currId, allIds) {
        QContact curr = contact(currId, QContactFetchHint(), &tempError);
        if (tempError != QContactManager::NoError)
            *error = tempError;
        if (QContactManagerEngine::testFilter(filter, curr)) {
            QContactManagerEngine::addSorted(&sortedAndFiltered, curr, sortOrders);
        }
    }

    foreach (const QContact& contact, sortedAndFiltered) {
        rtn.append(contact.localId());
    }
    return rtn;
}

QList<QContact> QContactAndroidEngine::contacts(const QContactFilter & filter, const QList<QContactSortOrder> & sortOrders, const QContactFetchHint & fetchHint,
                                                QContactManager::Error* error ) const
{

    Q_UNUSED(fetchHint); // no optimisations currently, ignore the fetchhint.
    Q_CHECK_PTR(d->m_abook);
    QList<QContact> rtn;

    QList<QContactLocalId> ids = contactIds(filter, sortOrders,error);
    foreach (QContactLocalId id, ids)
        rtn << contact(id, QContactFetchHint(), error);
    return rtn;
}

QContact QContactAndroidEngine::contact(const QContactLocalId& contactId, const QContactFetchHint& fetchHint, QContactManager::Error* error) const
{
    Q_UNUSED(fetchHint); //TODO
    Q_CHECK_PTR(d->m_abook);

    //NOTE getQContact can't set the displayLabel
    QContact *contact = d->m_abook->getQContact(contactId, error);
    QContact rtn(*contact);
    delete contact;
    if (*error == QContactManager::NoError) {
        setContactDisplayLabel(&rtn, synthesizedDisplayLabel(rtn, error));
        QContactId cid;
        cid.setLocalId(contactId);
        cid.setManagerUri(managerUri());
        rtn.setId(cid);
    }
    return rtn;
}

bool QContactAndroidEngine::saveContacts(QList<QContact>* contacts, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error)
{
    *error = QContactManager::NoError;
    QContactManager::Error tempError = QContactManager::NoError;
    QContact curr;
    for (int i = 0; i < contacts->size(); i++) {
        curr = contacts->at(i);
        if (!saveContact(&curr, &tempError)) {
            if (errorMap)
                errorMap->insert(i, tempError);
            *error = tempError;
        } else {
            contacts->replace(i, curr);
        }
    }

    return (*error == QContactManager::NoError);
}

bool QContactAndroidEngine::removeContacts(const QList<QContactLocalId>& ids, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error)
{
    *error = QContactManager::NoError;
    QContactManager::Error tempError = QContactManager::NoError;
    QContact curr;
    for (int i = 0; i < ids.size(); i++) {
        if (!removeContact(ids.at(i), &tempError)) {
            if (errorMap)
                errorMap->insert(i, tempError);
            *error = tempError;
        }
    }

    return (*error == QContactManager::NoError);
}

bool QContactAndroidEngine::saveContact(QContact* contact, QContactManager::Error* error)
{
    Q_CHECK_PTR(d->m_abook);

    if (!contact) {
        *error = QContactManager::BadArgumentError;
        return false;
    }

    // synthesize the display label for the contact
    setContactDisplayLabel(contact, synthesizedDisplayLabel(*contact, error));

    // ensure that the contact's details conform to their definitions
    if (!validateContact(*contact, error)) {
        QANDROID_DEBUG << "Validate Contact failed";
        return false;
    }

    bool retn = d->m_abook->saveContact(contact, error);
    QContactId cId = contact->id();
    cId.setManagerUri(managerUri());
    contact->setId(cId);
    return retn;
}

bool QContactAndroidEngine::removeContact(const QContactLocalId& contactId, QContactManager::Error* error)
{
    Q_CHECK_PTR(d->m_abook);
    return d->m_abook->removeContact(contactId, error);
}

QMap<QString, QContactDetailDefinition> QContactAndroidEngine::detailDefinitions(const QString& contactType, QContactManager::Error* error) const
{

    QMap<QString, QMap<QString, QContactDetailDefinition> > defns = QContactManagerEngine::schemaDefinitions();

    QMap<QString, QContactDetailFieldDefinition> fields;

    QContactDetailFieldDefinition gsfd; //Generic string field definition
    gsfd.setDataType(QVariant::String);

    // QContactAddress
    fields = defns[contactType][QContactAddress::DefinitionName].fields();
    fields.remove(QContactAddress::FieldSubTypes);
    fields.insert(QContactDetail::FieldDetailUri, gsfd);
    defns[contactType][QContactAddress::DefinitionName].setFields(fields);
    defns[contactType][QContactAddress::DefinitionName].setUnique(false);

    // No QContactAnniversary
    defns[contactType].remove(QContactAnniversary::DefinitionName);

    // No QContactAvatar
    defns[contactType].remove(QContactAvatar::DefinitionName);

    // QContactBirthday
    fields = defns[contactType][QContactBirthday::DefinitionName].fields();
    fields.remove(QContactDetail::FieldContext);
    defns[contactType][QContactBirthday::DefinitionName].setFields(fields);
    defns[contactType][QContactBirthday::DefinitionName].setUnique(true);

    // QContactDisplayLabel
    fields = defns[contactType][QContactDisplayLabel::DefinitionName].fields();
    defns[contactType][QContactDisplayLabel::DefinitionName].setFields(fields);
    defns[contactType][QContactDisplayLabel::DefinitionName].setUnique(true);

    // QContactEmailAddress
    fields = defns[contactType][QContactEmailAddress::DefinitionName].fields();
    fields.insert(QContactDetail::FieldDetailUri, gsfd);
    defns[contactType][QContactEmailAddress::DefinitionName].setFields(fields);
    defns[contactType][QContactEmailAddress::DefinitionName].setUnique(false);

    // QContactFamily
    fields = defns[contactType][QContactFamily::DefinitionName].fields();
    fields.remove(QContactDetail::FieldContext);
    defns[contactType][QContactFamily::DefinitionName].setFields(fields);
    defns[contactType][QContactFamily::DefinitionName].setUnique(true);

    // QContactGender
    fields = defns[contactType][QContactGender::DefinitionName].fields();
    fields.remove(QContactDetail::FieldContext);
    defns[contactType][QContactGender::DefinitionName].setFields(fields);
    defns[contactType][QContactGender::DefinitionName].setUnique(true);

    // No QContactGeoLocation
    defns[contactType].remove(QContactGeoLocation::DefinitionName);

    // QContactGuid
    fields = defns[contactType][QContactGuid::DefinitionName].fields();
    fields.remove(QContactDetail::FieldContext);
    defns[contactType][QContactGuid::DefinitionName].setFields(fields);
    defns[contactType][QContactGuid::DefinitionName].setUnique(true);

    // No QContactGlobalPresence
    defns[contactType].remove(QContactGlobalPresence::DefinitionName);

    // QContactName
    fields = defns[contactType][QContactName::DefinitionName].fields();
    fields.remove(QContactDetail::FieldContext);
    fields.remove(QContactName::FieldCustomLabel);
    fields.remove(QContactName::FieldMiddleName);
    fields.remove(QContactName::FieldPrefix);
    fields.remove(QContactName::FieldSuffix);
    defns[contactType][QContactName::DefinitionName].setFields(fields);
    defns[contactType][QContactName::DefinitionName].setUnique(true);

    // QContactNickname
    fields = defns[contactType][QContactNickname::DefinitionName].fields();
    fields.remove(QContactDetail::FieldContext);
    defns[contactType][QContactNickname::DefinitionName].setFields(fields);
    defns[contactType][QContactNickname::DefinitionName].setUnique(true);

    // QContactNote
    fields = defns[contactType][QContactNote::DefinitionName].fields();
    fields.remove(QContactDetail::FieldContext);
    defns[contactType][QContactNote::DefinitionName].setFields(fields);
    defns[contactType][QContactNote::DefinitionName].setUnique(true);

    // QContactOnlineAccount
    fields = defns[contactType][QContactOnlineAccount::DefinitionName].fields();
    fields.remove(QContactDetail::FieldContext);
    fields.remove(QContactOnlineAccount::FieldAccountUri);
    fields.remove(QContactOnlineAccount::FieldSubTypes);
    fields.insert("AccountPath", gsfd);
    defns[contactType][QContactOnlineAccount::DefinitionName].setFields(fields);
    defns[contactType][QContactOnlineAccount::DefinitionName].setUnique(false);

    // QContactOrganization
    fields = defns[contactType][QContactOrganization::DefinitionName].fields();
    fields.remove(QContactDetail::FieldContext);
    fields.remove(QContactOrganization::FieldAssistantName);
    fields.remove(QContactOrganization::FieldDepartment);
    fields.remove(QContactOrganization::FieldLocation);
    fields.remove(QContactOrganization::FieldLogoUrl);
    fields.remove(QContactOrganization::FieldTitle);
    fields.remove(QContactOrganization::FieldRole);
    defns[contactType][QContactOrganization::DefinitionName].setFields(fields);
    defns[contactType][QContactOrganization::DefinitionName].setUnique(true);

    // QContactPhoneNumber
    fields = defns[contactType][QContactPhoneNumber::DefinitionName].fields();
    fields.insert(QContactDetail::FieldDetailUri, gsfd);
    QContactDetailFieldDefinition phoneSubtype = fields.value(QContactPhoneNumber::FieldSubTypes);
    QVariantList allowableValues;
    allowableValues << QContactPhoneNumber::SubTypeMobile;
    allowableValues << QContactPhoneNumber::SubTypeVoice;
    phoneSubtype.setAllowableValues(allowableValues);
    fields.insert(QContactPhoneNumber::FieldSubTypes, phoneSubtype);
    defns[contactType][QContactPhoneNumber::DefinitionName].setFields(fields);
    defns[contactType][QContactPhoneNumber::DefinitionName].setUnique(false);

    // QContactPresence
    fields = defns[contactType][QContactPresence::DefinitionName].fields();
    fields.remove(QContactPresence::FieldTimestamp);
    fields.remove(QContactPresence::FieldPresenceState);
    defns[contactType][QContactPresence::DefinitionName].setFields(fields);
    defns[contactType][QContactPresence::DefinitionName].setUnique(false);

    // No QContactRingtone
    defns[contactType].remove(QContactRingtone::DefinitionName);

    // No QContactSyncTarget
    defns[contactType].remove(QContactSyncTarget::DefinitionName);

    // No QContactTag
    defns[contactType].remove(QContactTag::DefinitionName);

    // QContactTimestamp
    fields = defns[contactType][QContactTimestamp::DefinitionName].fields();
    fields.remove(QContactDetail::FieldContext);
    defns[contactType][QContactTimestamp::DefinitionName].setFields(fields);

    // QContactType
    fields = defns[contactType][QContactType::DefinitionName].fields();
    fields.remove(QContactDetail::FieldContext);
    defns[contactType][QContactType::DefinitionName].setFields(fields);

    // QContactUrl
    fields = defns[contactType][QContactUrl::DefinitionName].fields();
    fields.remove(QContactUrl::FieldSubType);
    defns[contactType][QContactUrl::DefinitionName].setFields(fields);
    defns[contactType][QContactUrl::DefinitionName].setUnique(false);
    *error = QContactManager::NoError;
    return defns[contactType];
}

QContactDetailDefinition QContactAndroidEngine::detailDefinition(const QString& definitionName, const QString& contactType, QContactManager::Error* error) const
{
    return QContactManagerEngine::detailDefinition(definitionName, contactType, error);
}

bool QContactAndroidEngine::hasFeature(QContactManager::ManagerFeature feature, const QString& contactType) const {
    Q_UNUSED(contactType);
    if (feature == QContactManager::Anonymous)
        return true;

    return false;
}

bool QContactAndroidEngine::isFilterSupported(const QContactFilter& filter) const {
    switch (filter.type()) {
    case QContactFilter::InvalidFilter:
    case QContactFilter::DefaultFilter:
    case QContactFilter::LocalIdFilter:
    case QContactFilter::ContactDetailFilter:
    case QContactFilter::ActionFilter:
    case QContactFilter::IntersectionFilter:
    case QContactFilter::UnionFilter:
        return true;
    default:
        return false;
    }
}

QList<QVariant::Type> QContactAndroidEngine::supportedDataTypes() const {
    QList<QVariant::Type> st;
    st.append(QVariant::String);
    st.append(QVariant::Int);
    st.append(QVariant::UInt);
    st.append(QVariant::Double);
    st.append(QVariant::Date);
    st.append(QVariant::DateTime);

    return st;
}



