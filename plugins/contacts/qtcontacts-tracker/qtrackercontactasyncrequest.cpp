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




#include <QTimer>
#include <qtcontacts.h>
#include <QtTracker/ontologies/nie.h>
#include <QtTracker/ontologies/nco.h>
#include <qtrackercontactasyncrequest.h>
#include <QHash>

using namespace SopranoLive;

class ConversionLookup: public QHash<QString,QString>
{
public:
    ConversionLookup& operator<<(const QPair<QString, QString> &conversion)
    {
        this->insert(conversion.first, conversion.second);
        return *this;
    }
};


const QString FieldQContactLocalId("QContactLocalId");
const QString FieldAccountPath("AccountPath");
const ConversionLookup presenceConversion(ConversionLookup()
            <<QPair<QString, QString>("presence-status-offline", QContactOnlineAccount::PresenceOffline)
            <<QPair<QString, QString>("presence-status-available", QContactOnlineAccount::PresenceAvailable)
            <<QPair<QString, QString>("presence-status-away", QContactOnlineAccount::PresenceAway)
            <<QPair<QString, QString>("presence-status-extended-away", QContactOnlineAccount::PresenceExtendedAway)
            <<QPair<QString, QString>("presence-status-busy", QContactOnlineAccount::PresenceBusy)
            <<QPair<QString, QString>("presence-status-unknown", QContactOnlineAccount::PresenceUnknown)
            <<QPair<QString, QString>("presence-status-hidden", QContactOnlineAccount::PresenceHidden)
            <<QPair<QString, QString>("presence-status-dnd", QContactOnlineAccount::PresenceBusy)
);

void matchPhoneNumber(RDFVariable &variable, QContactDetailFilter &filter)
{
    // This here is the first implementation of filtering that takes into account also affiliations.
    // needs to be applied for other filters too - TODO
    RDFVariable officeContact;
    RDFVariable homeContact;

    RDFVariable rdfPhoneNumber;
    rdfPhoneNumber = homeContact.property<nco::hasPhoneNumber>().property<nco::phoneNumber>();

    RDFVariable rdfOfficePhoneNumber;
    rdfOfficePhoneNumber = officeContact.property<nco::hasAffiliation>().property<nco::hasPhoneNumber>().property<nco::phoneNumber>();

    QString filterValue = filter.value().toString();
    if (filter.matchFlags() == Qt::MatchEndsWith)
    {
        QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Nokia","Trackerplugin");
        int matchDigitCount = settings.value("phoneNumberMatchDigitCount", "7").toInt();
        filterValue = filterValue.right(matchDigitCount);
        qDebug() << "match with:" << matchDigitCount << ":" << filterValue;
        rdfPhoneNumber.hasSuffix(filterValue);
        rdfOfficePhoneNumber.hasSuffix(filterValue);
    }
    else
    {   // default to exact match
        rdfOfficePhoneNumber.matchesRegexp(filterValue);
        rdfPhoneNumber.matchesRegexp(filterValue);
    }
    // This is the key part, including both contacts and affiliations
    variable.isMemberOf(RDFVariableList()<<homeContact);// TODO report bug doesnt work in tracker <<officeContact);
}

void matchOnlineAccount(RDFVariable &variable, QContactDetailFilter &filter)
{
    if ((filter.matchFlags() & QContactFilter::MatchExactly) == QContactFilter::MatchExactly)
    {
        if (filter.detailFieldName() == "Account" || filter.detailFieldName() == QContactOnlineAccount::FieldAccountUri)
        {
            variable.property<nco::imContactId> ().isMemberOf(QStringList() << filter.value().toString());
        }
        else if (filter.detailFieldName() == FieldAccountPath)
        {
            // as it uses telepathy:account path
            variable.property<nco::fromIMAccount>().equal(QUrl(QString("telepathy:")+filter.value().toString()));
        }
        else if (filter.detailFieldName() == QContactOnlineAccount::FieldServiceProvider)
        {
            variable.property<nco::fromIMAccount>().property<nco::imDisplayName> ().isMemberOf(QStringList() << filter.value().toString());
        }
        else
            qWarning() << "QTrackerContactFetchRequest," << __FUNCTION__
                    << "Unsupported detail filter by QContactOnlineAccount.";
    }
    else
    {
        qWarning() << "QTrackerContactFetchRequest," << __FUNCTION__
                << "Unsupported match flag in detail filter by QContactOnlineAccount. Use QContactFilter::MatchExactly";
    }
}

void matchName(RDFVariable &variable, QContactDetailFilter &filter)
{
    if (filter.detailDefinitionName() != QContactName::DefinitionName) {
        qWarning() << "QTrackerContactFetchRequest," << __FUNCTION__
                   << "Unsupported definition name in detail filter, should be QContactName::DefinitionName";
        return;
    }
    QString filterValue = filter.value().toString();
    QString field = filter.detailFieldName();
    if ((filter.matchFlags() & QContactFilter::MatchExactly) == QContactFilter::MatchExactly) {
        if (field == QContactName::FieldFirst) {
            variable.property<nco::nameGiven>() = LiteralValue(filterValue);
        } else if (field == QContactName::FieldLast) {
            variable.property<nco::nameFamily>() = LiteralValue(filterValue);
        } else if (field == QContactName::FieldMiddle) {
            variable.property<nco::nameAdditional>() = LiteralValue(filterValue);
        } else if (field == QContactName::FieldPrefix) {
            variable.property<nco::nameHonorificPrefix>() = LiteralValue(filterValue);
        } else if (field == QContactName::FieldSuffix) {
            variable.property<nco::nameHonorificSuffix>() = LiteralValue(filterValue);
        }
    } else {
        qWarning() << "QTrackerContactFetchRequest," << __FUNCTION__
                   << "Unsupported match flag in detail filter by QContactName";
    }
}

/*
 * RDFVariable describes all contacts in tracker before filter is applied.
 * This method translates QContactFilter to tracker rdf filter. When query is made
 * after this method, it would return only contacts that fit the filter.
 */
QContactManager::Error QTrackerContactFetchRequest::applyFilterToContact(RDFVariable &variable,
        const QContactFilter &filter)
{
    if (filter.type() == QContactFilter::LocalIdFilter) {
        QContactLocalIdFilter filt = filter;
        if (!filt.ids().isEmpty()) {
            variable.property<nco::contactUID>().isMemberOf(filt.ids());
        } else {
            qWarning() << Q_FUNC_INFO << "QContactLocalIdFilter idlist is empty";
            return QContactManager::BadArgumentError;
        }
    } else if (filter.type() == QContactFilter::ContactDetailFilter) {
        // this one is tricky as we need to match in contacts or in affiliations

        QContactDetailFilter filt = filter;
        if ( QContactPhoneNumber::DefinitionName == filt.detailDefinitionName()
             && QContactPhoneNumber::FieldNumber == filt.detailFieldName()) {
            matchPhoneNumber(variable, filt);
        }
        else if(QContactOnlineAccount::DefinitionName == filt.detailDefinitionName())
        {
            matchOnlineAccount(variable, filt);
        }
        else if (QContactName::DefinitionName == filt.detailDefinitionName()) {
            matchName(variable, filt);
        }
        else if (filt.matchFlags() == Qt::MatchExactly) {
            if (QContactEmailAddress::DefinitionName == filt.detailDefinitionName()
                       && QContactEmailAddress::FieldEmailAddress == filt.detailFieldName()) {
                RDFVariable rdfEmailAddress;
                rdfEmailAddress = variable.property<nco::hasEmailAddress>();
                rdfEmailAddress.property<nco::emailAddress>() = LiteralValue(filt.value().toString());
            } else {
                qWarning() << __PRETTY_FUNCTION__ << "QContactTrackerEngine: Unsupported QContactFilter::ContactDetail"
                    << filt.detailDefinitionName();
                return QContactManager::NotSupportedError;
            }
        }
    }
    else if (filter.type() == QContactFilter::ContactDetailRangeFilter)
    {
        return applyDetailRangeFilterToContact(variable, filter);
    }
    else if (filter.type() == QContactFilter::ChangeLogFilter) {
        const QContactChangeLogFilter& clFilter = static_cast<const QContactChangeLogFilter&>(filter);
        // do not return facebook and telepathy contacts here
        // it is a temp implementation for the what to offer to synchronization constraint
        variable.property<nao::hasTag>().property<nao::prefLabel>() = LiteralValue("addressbook");

        if (clFilter.eventType() == QContactChangeLogFilter::EventRemoved) { // Removed since
            qWarning() << "QContactTrackerEngine: Unsupported QContactChangeLogFilter::Removed (contacts removed since)";
            return QContactManager::NotSupportedError;
        } else if (clFilter.eventType() == QContactChangeLogFilter::EventAdded) { // Added since
            variable.property<nie::contentCreated>() >= LiteralValue(clFilter.since().toString(Qt::ISODate));
        } else if (clFilter.eventType() == QContactChangeLogFilter::EventChanged) { // Changed since
            variable.property<nie::contentLastModified>() >= LiteralValue(clFilter.since().toString(Qt::ISODate));
        }
    } else if (filter.type() == QContactFilter::UnionFilter) {
        const QContactUnionFilter unionFilter(filter);
        foreach (QContactFilter f, unionFilter.filters()) {
            QContactManager::Error error = applyFilterToContact(variable, f);
            if (QContactManager::NoError != error)
                return error;
        }
    }
    else if(filter.type() == QContactFilter::InvalidFilter || filter.type() == QContactFilter::DefaultFilter)
        return QContactManager::NoError;
    else
        return QContactManager::NotSupportedError;
    return QContactManager::NoError;
}

//!\sa applyFilterToContact
QContactManager::Error QTrackerContactFetchRequest::applyDetailRangeFilterToContact(RDFVariable &variable, const QContactFilter &filter)
{
    Q_ASSERT(filter.type() == QContactFilter::ContactDetailRangeFilter);
    if (filter.type() == QContactFilter::ContactDetailRangeFilter) {
        QContactDetailRangeFilter filt = filter;
        // birthday range
        if (QContactBirthday::DefinitionName == filt.detailDefinitionName()
                && QContactBirthday::FieldBirthday == filt.detailFieldName())
        {
            RDFVariable time = variable.property<nco::birthDate>();
            if (filt.rangeFlags() & QContactDetailRangeFilter::IncludeUpper)
                time <= LiteralValue(filt.maxValue().toDateTime().toString(Qt::ISODate));
            else
                time < LiteralValue(filt.maxValue().toDateTime().toString(Qt::ISODate));
            if (filt.rangeFlags() & QContactDetailRangeFilter::ExcludeLower)
                time > LiteralValue(filt.minValue().toDateTime().toString(Qt::ISODate));
            else
                time >= LiteralValue(filt.minValue().toDateTime().toString(Qt::ISODate));
            return QContactManager::NoError;
        }
    }
    qWarning() << __PRETTY_FUNCTION__ << "Unsupported detail range filter";
    return QContactManager::NotSupportedError;
}


/*
 * To understand why all the following methods have for affiliation param, check nco ontology:
 * every contact has all these properties and also linked to affiliations (also contacts - nco:Role)
 * that again have the same properties. So it was needed to make the same query 2-ce - once for contact
 * and once for affiliations
 */
RDFSelect preparePhoneNumbersQuery(RDFVariable &rdfcontact1, bool forAffiliations)
{
    RDFVariable phone;
    if (!forAffiliations)
        phone = rdfcontact1.property<nco::hasPhoneNumber>();
    else
        phone = rdfcontact1.property<nco::hasAffiliation>().property<nco::hasPhoneNumber>();
    RDFVariable type = phone.type();
    type.property<rdfs::subClassOf>().notEqual(nco::ContactMedium::iri()); // sparql cannot handle exact type but returns all super types as junk rows
    type.property<rdfs::subClassOf>().notEqual(rdfs::Resource::iri()); // sparql cannot handle exact type but returns all super types as junk rows
    // therefore we eliminate those rows that are not of interest
    // columns
    RDFSelect queryidsnumbers;
    queryidsnumbers.addColumn("contactId", rdfcontact1.property<nco::contactUID> ());
    queryidsnumbers.addColumn("phoneno", phone.property<nco::phoneNumber> ());
    queryidsnumbers.addColumn("type", type);
    queryidsnumbers.distinct();
    return queryidsnumbers;
}

RDFSelect prepareEmailAddressesQuery(RDFVariable &rdfcontact1, bool forAffiliations)
{
    RDFVariable email;
    if (!forAffiliations)
        email = rdfcontact1.property<nco::hasEmailAddress>();
    else
        email = rdfcontact1.property<nco::hasAffiliation>().property<nco::hasEmailAddress>();
    const RDFVariable& type = email.type();
    type.property<rdfs::subClassOf>().notEqual(nco::Resource::iri()); // sparql cannot handle exact type but returns all super types as junk rows
    // therefore we eliminate those rows that are not of interest
    // columns
    RDFSelect queryidsnumbers;
    queryidsnumbers.addColumn("contactId", rdfcontact1.property<nco::contactUID> ());
    queryidsnumbers.addColumn("emailaddress", email.property<nco::emailAddress> ());
    rdfcontact1.property<nco::hasEmailAddress> ().isOfType( nco::EmailAddress::iri(), true);
    queryidsnumbers.addColumn("type", type);
    queryidsnumbers.distinct();
    return queryidsnumbers;
}

RDFSelect prepareIMContactsQuery(RDFVariable  &imcontact )
{
    // columns
    RDFSelect queryidsimacccounts;
    imcontact = queryidsimacccounts.newColumn<nco::IMContact>("contact");
    queryidsimacccounts.groupBy(imcontact);
    queryidsimacccounts.addColumn("contactId", imcontact.property<nco::contactUID> ());

    queryidsimacccounts.addColumn("IMId", imcontact.property<nco::imContactId> ());
    queryidsimacccounts.addColumn("status", imcontact.optional().property<nco::imContactPresence> ());
    queryidsimacccounts.addColumn("message", imcontact.optional().property<nco::imContactStatusMessage> ());
    queryidsimacccounts.addColumn("nick", imcontact.optional().property<nco::imContactNickname> ());
    queryidsimacccounts.addColumn("type", imcontact.optional().property<nco::fromIMAccount> ());
    queryidsimacccounts.addColumn("capabilities",
                imcontact.optional().property<nco::imContactCapability>().filter("GROUP_CONCAT", LiteralValue(",")));
    queryidsimacccounts.addColumn("metacontact", imcontact.optional().property<nco::metacontact> ());
    queryidsimacccounts.addColumn("serviceprovider", imcontact.optional().property<nco::fromIMAccount>().property<nco::imDisplayName>());

    return queryidsimacccounts;

}

RDFSelect prepareIMAccountsQuery(RDFVariable &rdfPersonContact)
{
    RDFVariable imAccount;
    imAccount = rdfPersonContact.property<nco::hasIMAccount> ();
    RDFSelect queryidsimaccounts;

    queryidsimaccounts.addColumn("protocol", imAccount.property<nco::imID> ());
    queryidsimaccounts.addColumn("presence",imAccount.optional().property<nco::imPresence> ());
    queryidsimaccounts.addColumn("message", imAccount.optional().property<nco::imStatusMessage> ());
    queryidsimaccounts.addColumn("nick", imAccount.optional().property<nco::imNickname> ());
    queryidsimaccounts.addColumn("displayname", imAccount.optional().property<nco::imDisplayName> ());

    return queryidsimaccounts;
}


QTrackerContactAsyncRequest::QTrackerContactAsyncRequest(QContactAbstractRequest* request)
: req(request)
{
}

const QString rdfPhoneType2QContactSubtype(const QString rdfPhoneType)
{
    if( rdfPhoneType.endsWith("VoicePhoneNumber") )
        return QContactPhoneNumber::SubTypeVoice;
    else if ( rdfPhoneType.endsWith("CarPhoneNumber") )
        return QContactPhoneNumber::SubTypeCar;
    else if ( rdfPhoneType.endsWith("CellPhoneNumber") )
        return QContactPhoneNumber::SubTypeMobile;
    else if ( rdfPhoneType.endsWith("BbsPhoneNumber") )
        return QContactPhoneNumber::SubTypeBulletinBoardSystem;
    else if ( rdfPhoneType.endsWith("FaxNumber") )
        return QContactPhoneNumber::SubTypeFacsimile;
    else if ( rdfPhoneType.endsWith("ModemNumber") )
        return QContactPhoneNumber::SubTypeModem;
    else if ( rdfPhoneType.endsWith("PagerNumber") )
        return QContactPhoneNumber::SubTypePager;
    else if ( rdfPhoneType.endsWith("MessagingNumber") )
        return QContactPhoneNumber::SubTypeMessagingCapable;
    else
        qWarning() << Q_FUNC_INFO << "Not handled phone number type:" << rdfPhoneType;
    return "";
}

QTrackerContactAsyncRequest::~QTrackerContactAsyncRequest()
{

}

/*!
 * The method was initially created to add default fields in case client did not supply
 * fields constraint - in that case the constraint is that default contact fields (ones
 * being edited in contact card and synchronized) are queried.
 */
void QTrackerContactFetchRequest::validateRequest()
{
    Q_ASSERT(req);
    Q_ASSERT(req->type() == QContactAbstractRequest::ContactFetchRequest);
    QContactFetchRequest* r = qobject_cast<QContactFetchRequest*> (req);
    if (r && r->definitionRestrictions().isEmpty()) {
        QStringList fields;
        fields << QContactAvatar::DefinitionName
            << QContactBirthday::DefinitionName
            << QContactAddress::DefinitionName
            << QContactEmailAddress::DefinitionName
            << QContactGender::DefinitionName
            << QContactAnniversary::DefinitionName
            << QContactName::DefinitionName
            << QContactOnlineAccount::DefinitionName
            << QContactOrganization::DefinitionName
            << QContactPhoneNumber::DefinitionName
            << QContactUrl::DefinitionName;
        r->setDefinitionRestrictions(fields);
    }
}

QTrackerContactFetchRequest::QTrackerContactFetchRequest(QContactAbstractRequest* request,
                                                         QContactManagerEngine* parent) :
    QObject(parent),QTrackerContactAsyncRequest(request),
    queryPhoneNumbersNodesPending(0),
    queryEmailAddressNodesPending(0)
{
    Q_ASSERT(parent);
    Q_ASSERT(request);
    QContactManagerEngine::updateRequestState(req, QContactAbstractRequest::ActiveState);

    QTimer::singleShot(0, this, SLOT(run()));
}

void QTrackerContactFetchRequest::run()
{
    validateRequest();
    QContactFetchRequest* r = qobject_cast<QContactFetchRequest*> (req);

    RDFVariable RDFContact = RDFVariable::fromType<nco::PersonContact>();
    QContactManager::Error error = applyFilterToContact(RDFContact, r->filter());
    if (error != QContactManager::NoError)
    {
        emitFinished(error);
        return;
    }
    if (r->definitionRestrictions().contains(QContactPhoneNumber::DefinitionName)) {
        queryPhoneNumbersNodes.clear();
        queryPhoneNumbersNodesPending = 2;
        for(int forAffiliations = 0; forAffiliations <= 1; forAffiliations++) {
            // prepare query to get all phone numbers
            RDFVariable rdfcontact1 = RDFVariable::fromType<nco::PersonContact>();
            applyFilterToContact(rdfcontact1, r->filter());
            // criteria - only those with phone numbers
            RDFSelect queryidsnumbers = preparePhoneNumbersQuery(rdfcontact1, forAffiliations);
            queryPhoneNumbersNodes << ::tracker()->modelQuery(queryidsnumbers);
            // need to store LiveNodes in order to receive notification from model
            QObject::connect(queryPhoneNumbersNodes[forAffiliations].model(),
                             SIGNAL(modelUpdated()), this, SLOT(phoneNumbersReady()));
        }
    }

    if (r->definitionRestrictions().contains(QContactEmailAddress::DefinitionName)) {
        queryEmailAddressNodes.clear();
        queryEmailAddressNodesPending = 2;
        for(int forAffiliations = 0; forAffiliations <= 1; forAffiliations++) {
            // prepare query to get all email addresses
            RDFVariable rdfcontact1 = RDFVariable::fromType<nco::PersonContact>();
            applyFilterToContact(rdfcontact1, r->filter());
            // criteria - only those with email addresses
            RDFSelect queryidsnumbers = prepareEmailAddressesQuery(rdfcontact1,forAffiliations);
            queryEmailAddressNodes << ::tracker()->modelQuery(queryidsnumbers);
            // need to store LiveNodes in order to receive notification from model
            QObject::connect(queryEmailAddressNodes[forAffiliations].model(),
                             SIGNAL(modelUpdated()), this, SLOT(emailAddressesReady()));
        }
    }

    if ( r->definitionRestrictions().contains( QContactOnlineAccount::DefinitionName) ) {
        queryIMAccountNodesPending = 1;

        RDFSelect queryidsimaccounts;
        RDFVariable rdfIMContact;
        rdfIMContact = rdfIMContact.fromType<nco::IMContact> ();

        if(isMeContact(r->filter())) {
            RDFVariable rdfPersonContact;
            rdfPersonContact = rdfPersonContact.fromType<nco::PersonContact> ();
            // Prepare a query to get all IMAccounts from all  accounts.
            // nco:PersonContact -- nco:hasIMAccount -- nco:IMAccount
            queryidsimaccounts = prepareIMAccountsQuery(rdfPersonContact);
        } else {
            // Prepare a query to get all IMContacts from all accounts.
            queryidsimaccounts = prepareIMContactsQuery(rdfIMContact);
        }

        if( r->filter().type() != QContactFilter::DefaultFilter )
        {
            // need to get all IMContacts with the same contact id (1) and all IMContacts
            // that have the same metacontact (2)

            // (1)rdfcontact1 represent the contacts that fits to the filter
            RDFVariable rdfcontact1;
            applyFilterToContact(rdfcontact1, r->filter());

            // (2) select all contacts that have the same metacontact as some contact corresponding to given filter
            // and if metacontact exist.
            RDFVariable rdfcontact3;
            rdfcontact3.property<nco::metacontact> () = rdfcontact1.optional().property<nco::metacontact> ();

            // aggregated criteria - only those with im contacts that match rdfcontact1 (same id) or rdfcontact3 (same metacontact)
            rdfIMContact.isMemberOf(RDFVariableList()<<rdfcontact1<<rdfcontact3);
        }

        queryIMAccountNodes = ::tracker()->modelQuery(queryidsimaccounts);
        QObject::connect(queryIMAccountNodes.model(),
                SIGNAL(modelUpdated()), SLOT(iMAcountsReady()));
    }

    QList<QContactLocalId> ids;
    RDFVariable RDFContact1 = RDFVariable::fromType<nco::PersonContact>();
    applyFilterToContact(RDFContact1, r->filter());
    RDFSelect quer;
    RDFVariable prefix = RDFContact1.optional().property<nco::nameHonorificPrefix> ();
    RDFVariable lastname = RDFContact1.optional().property<nco::nameFamily> ();
    RDFVariable middlename = RDFContact1.optional().property<nco::nameAdditional> ();
    RDFVariable firstname = RDFContact1.optional().property<nco::nameGiven> ();
    RDFVariable nickname = RDFContact1.optional().property<nco::nickname> ();
    quer.addColumn("contactId", RDFContact1.property<nco::contactUID> ());
    quer.addColumn("metacontact",RDFContact1.optional().property<nco::metacontact> ());
    quer.addColumn("prefix", prefix);
    quer.addColumn("firstname", firstname);
    quer.addColumn("middlename", middlename);
    quer.addColumn("secondname", lastname);
    quer.addColumn("photo", RDFContact1.optional().property<nco::photo> ());
    quer.addColumn("nickname", nickname);

    // for now adding columns to main query. later separate queries
    if (r->definitionRestrictions().contains(QContactAddress::DefinitionName)) {
        RDFVariable address = RDFContact.optional().property< nco::hasPostalAddress> ();
        quer.addColumn("street",address.optional().property<nco::streetAddress> ());
        quer.addColumn("city", address.optional().property<nco::locality> ());
        quer.addColumn("country", address.optional().property<nco::country> ());
        quer.addColumn("pcode", address.optional().property<nco::postalcode> ());
        quer.addColumn("reg", address.optional().property<nco::region> ());
    }
    if (r->definitionRestrictions().contains(QContactUrl::DefinitionName)) {
        quer.addColumn("homepage", RDFContact.optional().property<nco::websiteUrl> ());
        quer.addColumn("url", RDFContact.optional().property<nco::url> ());
        quer.addColumn("work_homepage", RDFContact.optional().property<nco::hasAffiliation> ().property<nco::websiteUrl> ());
        quer.addColumn("work_url", RDFContact.optional().property<nco::hasAffiliation> ().property<nco::url> ());
    }
    if (r->definitionRestrictions().contains(QContactBirthday::DefinitionName)) {
        quer.addColumn("birth",RDFContact.optional().property<nco::birthDate> ());
    }
    if (r->definitionRestrictions().contains(QContactGender::DefinitionName)) {
        quer.addColumn("gender", RDFContact.optional().property<nco::gender> ());
    }
    if (r->definitionRestrictions().contains(QContactOrganization::DefinitionName)) {
        RDFVariable rdforg = RDFContact.optional().property<nco::hasAffiliation> ().optional().property<nco::org> ();
        quer.addColumn("org", rdforg.optional().property<nco::fullname> ());
        quer.addColumn("logo", rdforg.optional().property<nco::logo> ());
    }


    // QContactAnniversary - no such thing in tracker
    // QContactGeolocation - nco:hasLocation is not having class defined in nco yet. no properties. maybe rdfs:Resource:label

    // supporting sorting only here, difficult and no requirements in UI for sorting in multivalue details (phones, emails)
    foreach(QContactSortOrder sort, r->sorting()) {
        if (sort.detailDefinitionName() == QContactName::DefinitionName) {
            if (sort.detailFieldName() == QContactName::FieldFirst)
                quer.orderBy(firstname);
            else if (sort.detailFieldName() == QContactName::FieldLast)
                quer.orderBy(lastname);
            else
                qWarning() << "QTrackerContactFetchRequest" << "sorting by"
                    << sort.detailDefinitionName()
                    << sort.detailFieldName() << "is not yet supported";
        } else {
            qWarning() << "QTrackerContactFetchRequest" << "sorting by"
                << sort.detailDefinitionName()
                << "is not yet supported";
        }
    }
    query = ::tracker()->modelQuery(quer);
    // need to store LiveNodes in order to receive notification from model
    QObject::connect(query.model(), SIGNAL(modelUpdated()), this, SLOT(contactsReady()));
}

bool detailExisting(const QString &definitionName, const QContact &contact, const QContactDetail &adetail)
{
    QList<QContactDetail> details = contact.details(definitionName);
    foreach(const QContactDetail &detail, details) {
        if (detail == adetail) {
            return true;
        }
    }
    return false;
}

void QTrackerContactFetchRequest::contactsReady()
{
    // 1) process IMContacts: queryIMAccountNodes
    // 2) process contacts: query and during it handle metacontacts
    // 3) process phonenumbers: queryPhoneNumbersNodes
    // 4) process emails: queryPhoneNumbersNodes
    // 5) update display label details

    QContactFetchRequest* request = qobject_cast<QContactFetchRequest*> (req);
    Q_ASSERT( request ); // signal is supposed to be used only for contact fetch
    // fastest way to get this working. refactor
    if (!request) {
        QContactManagerEngine::updateRequestState(req, QContactAbstractRequest::FinishedState);
        return;
    }

   /*
    * 1) process IMContacts: queryIMAccountNodes
    * Processing IMContacts need to be called before processing PersonContacts - \sa result is filled with IMContacts first,
    * then metacontacts are processed while processing addressbook contacts. This is because QContactOnlineAccount details
    * need to be constructed before linking contacts with same metacontact.
    */
    if (request->definitionRestrictions().contains(QContactOnlineAccount::DefinitionName)) {
        processQueryIMContacts(queryIMAccountNodes);
    }

    // 2) process contacts: query and during it handle metacontacts
    for(int i = 0; i < query->rowCount(); i++) {
        QContact contact; // one we will be filling with this row

        bool ok;
        QContactLocalId contactid = query->index(i, 0).data().toUInt(&ok);
        if (!ok) {
            qWarning()<< Q_FUNC_INFO <<"Invalid contact ID: "<< query->index(i, 0).data().toString();
            continue;
        }
        QContactId id; id.setLocalId(contactid);

        QContactManagerEngine *engine = qobject_cast<QContactManagerEngine *>(parent());
        Q_ASSERT(engine);
        if(engine)
            id.setManagerUri(engine->managerUri());

        contact.setId(id);
        QString metacontact = query->index(i, 1).data().toString();

        // using redundancy to get less queries to tracker - it is possible
        // that rows are repeating: information for one contact is in several rows
        // that's why we update existing contact and append details to it if they
        // are not already in QContact
        QHash<quint32, int>::const_iterator it = id2ContactLookup.find(contactid);
        //
        int index = result.size(); // where to place new contact
        if (id2ContactLookup.end() != it) {
            if (it.value() < result.size() && it.value() >= 0) {
                index = it.value();
                contact = result[index];
            }
            Q_ASSERT(query->index(i, 0).data().toUInt() == contact.localId());
        }

        readFromQueryRowToContact(contact ,i);
        addContactToResultSet(contact, metacontact);
    }

    // 3) process phonenumbers: queryPhoneNumbersNodes
    if (request->definitionRestrictions().contains(QContactPhoneNumber::DefinitionName)) {
        Q_ASSERT(queryPhoneNumbersNodes.size() == 2);
        for( int cnt = 0; cnt < queryPhoneNumbersNodes.size(); cnt++) {
            processQueryPhoneNumbers(queryPhoneNumbersNodes[cnt], cnt);
        }
    }

    // 4) process emails: queryPhoneNumbersNodes
    if (request->definitionRestrictions().contains(QContactEmailAddress::DefinitionName)) {
        qDebug() << "processQueryEmailAddresses";
        Q_ASSERT(queryEmailAddressNodes.size() == 2);
        for (int cnt = 0; cnt < queryEmailAddressNodes.size(); cnt++) {
            processQueryEmailAddresses(queryEmailAddressNodes[cnt], cnt);
        }
    }

    // 5) update display labels
    QContactManagerEngine *engine = dynamic_cast<QContactManagerEngine*>(parent());
    Q_ASSERT(engine);
    for(int i = 0; i < result.count(); i++)
    {
        QContact &cont(result[i]);
        QContactDisplayLabel dl = cont.detail(QContactDisplayLabel::DefinitionName);
        if (dl.label().isEmpty()) {
            QContactManager::Error synthError;
            result[i] = engine->setContactDisplayLabel(engine->synthesizedDisplayLabel(cont, synthError), cont);
        }
    }
    emitFinished();
}

void QTrackerContactFetchRequest::emitFinished(QContactManager::Error error)
{
    QContactFetchRequest *fetchRequest = qobject_cast<QContactFetchRequest *>(req);
    Q_ASSERT(fetchRequest);
    if(fetchRequest) {
        QContactManagerEngine::updateRequestState(fetchRequest, QContactAbstractRequest::FinishedState);
        QContactManagerEngine::updateContactFetchRequest(fetchRequest, result, error);
    }
}

/*!
 *  Appending contact to \sa result, id2ContactLookup,  of the request - during the operation it is resolved if to
 *  link (merge) contact with existing one in result, to add it as new or to replace existing in result.
 */
void QTrackerContactFetchRequest::addContactToResultSet(QContact &contact, const QString &metacontact)
{
    QHash<quint32, int>::const_iterator it = id2ContactLookup.find(contact.localId());
    int index = -1; // where to place new contact, -1 - not defined
    if (id2ContactLookup.end() != it) {
        if (it.value() < result.size() && it.value() >= 0)
            index = it.value();
    }
    QContact *contact_ = &contact;
    if( -1 == index && !metacontact.isEmpty() ) {
        if( metacontactLookup.contains(metacontact) )
        {
           // we'll link them and update contact on existing position
           index = metacontactLookup[metacontact];
           contact_ = &(linkContactsWithSameMetaContact(contact, result[index]));

        }
    }

    if ( -1 == index ) {
        result.append(*contact_);
        id2ContactLookup[contact_->localId()] = result.size()-1;
        if( !metacontact.isEmpty())
        {
            metacontactLookup[metacontact] = result.size()-1;
        }
    } else {
        result[index] = *contact_;
        id2ContactLookup[contact_->localId()] = index;
    }
}

/*!
 * brief Processes one query record-row during read from tracker to QContact.
 * Order or columns in query is fixed to order defined in \sa run()
 */
void QTrackerContactFetchRequest::readFromQueryRowToContact(QContact &contact, int i)
{
    int column = 2; // 0 - for QContactLocalId, 1 for metacontact
    QContactName name = contact.detail(QContactName::DefinitionName);
    name.setPrefix(query->index(i, column++).data().toString());
    name.setFirstName(query->index(i, column++).data().toString());
    name.setMiddleName(query->index(i, column++).data().toString());
    name.setLastName(query->index(i, column++).data().toString());
    contact.saveDetail(&name);

    QContactAvatar avatar = contact.detail(QContactAvatar::DefinitionName);
    avatar.setAvatar(query->index(i, column++).data().toString());
    if (!avatar.avatar().isEmpty()) {
        contact.saveDetail(&avatar);
    }
    QContactNickname nick = contact.detail(QContactNickname::DefinitionName);
    nick.setNickname(query->index(i, column++).data().toString());
    contact.saveDetail(&nick);

    QContactFetchRequest* request = qobject_cast<QContactFetchRequest*> (req);
    Q_ASSERT( request ); // this is handled already in caller
    if( !request )
        return;
    // TODO extract generic from bellow ... mapping field names
    if (request->definitionRestrictions().contains(QContactAddress::DefinitionName)) {
        QString street = query->index(i, column++).data().toString();
        QString loc = query->index(i, column++).data().toString();
        QString country = query->index(i, column++).data().toString();
        QString pcode = query->index(i, column++).data().toString();
        QString region = query->index(i, column++).data().toString();
        if (!(country.isEmpty() && pcode.isEmpty() && region.isEmpty()
              && street.isEmpty() && loc.isEmpty())) {
            // for multivalue fields is a bit tricky - try to find existing an
            QContactAddress a;
            a.setStreet(street);
            a.setLocality(loc);
            a.setCountry(country);
            a.setPostcode(pcode);
            a.setRegion(region);
            if (!detailExisting(QContactAddress::DefinitionName, contact, a)) {
                contact.saveDetail(&a);
            }
        }
    }
    if (request->definitionRestrictions().contains(QContactUrl::DefinitionName)) {
        // check query preparation (at the moment in constructor TODO refactor)
        // home website
        // if it is websiteUrl then interpret as homepage, if it is nco:url then fovourite url
        QContactUrl url;
        url.setSubType(QContactUrl::SubTypeHomePage);
        url.setContexts(QContactUrl::ContextHome);
        url.setUrl(query->index(i, column++).data().toString());
        if (url.url().isEmpty()) {
            // website url is at the same time url, so we handle duplication here
            // if only url then set it as favourite
            url.setUrl(query->index(i, column++).data().toString());
            url.setSubType(QContactUrl::SubTypeFavourite);
        }

        if (!url.url().isEmpty() && !detailExisting(QContactUrl::DefinitionName, contact, url)) {
            contact.saveDetail(&url);
        }
        // office website
        QContactUrl workurl;
        workurl.setContexts(QContactUrl::ContextWork);
        workurl.setSubType(QContactUrl::SubTypeHomePage);
        workurl.setUrl(query->index(i, column++).data().toString());
        if (workurl.url().isEmpty()) {
            workurl.setUrl(query->index(i, column++).data().toString());
            workurl.setSubType(QContactUrl::SubTypeFavourite);
        }
        if (!workurl.url().isEmpty() && !detailExisting(QContactUrl::DefinitionName, contact, workurl)) {
            contact.saveDetail(&workurl);
        }
    }
    if (request->definitionRestrictions().contains(QContactBirthday::DefinitionName)) {
        QVariant var = query->index(i, column++).data();
        if (!var.toString().isEmpty() /* enable reading wrong && var.toDate().isValid()*/) {
            QContactBirthday birth = contact.detail(QContactBirthday::DefinitionName);
            birth.setDate(var.toDate());
            contact.saveDetail(&birth);
        }
    }
    if (request->definitionRestrictions().contains(QContactGender::DefinitionName)) {
        QString var = query->index(i, column++).data().toString();
        if (!var.isEmpty()) {
            QContactGender g = contact.detail(QContactGender::DefinitionName);
            g.setGender(var);
            contact.saveDetail(&g);
        }
    }
    if (request->definitionRestrictions().contains(QContactOrganization::DefinitionName)) {
        QString org = query->index(i, column++).data().toString();
        QString logo = query->index(i, column++).data().toString();
        if (!( org.isEmpty() && logo.isEmpty())) {
            QContactOrganization o;
            o.setName(org);
            o.setLogo(logo);
            if (!detailExisting(QContactOrganization::DefinitionName, contact, o)) {
                contact.saveDetail(&o);
            }
        }
    }

}

/*!
 * When 2 contacts have the same metacontact, decide which of these 2 contacts needs
 * to be returned as addressbook contact - the other one is void from returned set
 * Info about linking stored in returned contact - reference to either first or second
 */
QContact &QTrackerContactFetchRequest::linkContactsWithSameMetaContact(QContact &first, QContact &second)
{
    bool returnFirst = true;
    // 1) resolve which one to return as metacontact(mastercontact) contact
    // 2) insert link the one not returned
    // now we only merge IMContacts to addressbook contacts - if that change, changing this too
    // check if there is existence of previous merging information or online account info
    QList<QContactDetail> details = first.details(QContactOnlineAccount::DefinitionName);

    // 1) resolve which one is to be returned and which one linked from it
    bool allreadyContainsLinkingInfo(false);
    foreach (QContactDetail detail, details)
    {
        if( !detail.value(FieldQContactLocalId).isEmpty())
        {
            allreadyContainsLinkingInfo = true;
            break;
        }
        else if( !detail.value(FieldAccountPath).isEmpty() || !detail.value(QContactOnlineAccount::FieldPresence).isEmpty() )
        {
            returnFirst = false;
            break;
        }
    }
    QContact *returned, *linked;
    if( returnFirst )
    {
        returned = &first;
        linked = &second;
    }
    else
    {
        returned = &second;
        linked = &first;
    }

    // 2) now insert linking information to returned contact
    details = linked->details(QContactOnlineAccount::DefinitionName);

    foreach (QContactDetail detail, details)
    {
        detail.setValue(FieldQContactLocalId, linked->localId());
        returned->saveDetail(&detail);
    }
    return *returned;
}


void QTrackerContactFetchRequest::phoneNumbersReady()
{
    queryPhoneNumbersNodesPending--;
}

void QTrackerContactFetchRequest::emailAddressesReady()
{
    queryEmailAddressNodesPending--;
}

void QTrackerContactFetchRequest::iMAcountsReady()
{
    queryIMAccountNodesPending--;
    // now we know that the query is ready before get all contacts, check how it works with transactions
}

/*!
 * An internal helper method for converting nco:PhoneNumber subtype to
 * QContactPhoneNumber:: subtype attribute
 */
void QTrackerContactFetchRequest::processQueryPhoneNumbers(SopranoLive::LiveNodes queryPhoneNumbers,
                                                           bool affiliationNumbers )
{
    Q_ASSERT_X( queryPhoneNumbersNodesPending==0, Q_FUNC_INFO, "Phonenumbers query was supposed to be ready and it is not." );
    for (int i = 0; i < queryPhoneNumbers->rowCount(); i++) {
        // ignore if next one is the same - asked iridian about making query to ignore supertypes
        // TODO remove after his answer
        if ( i-1 >= 0
             && (queryPhoneNumbers->index(i, 0).data().toString()
                 == queryPhoneNumbers->index(i-1, 0).data().toString())
             && (queryPhoneNumbers->index(i, 1).data().toString()
                 == queryPhoneNumbers->index(i-1, 1).data().toString())) {
            // this is for ignoring duplicates. bad approach, asked iridian about
            // how to eliminate super types in query results
            continue;
        }

        QString subtype = rdfPhoneType2QContactSubtype(queryPhoneNumbers->index(i, 2).data().toString());
        QContactLocalId contactid = queryPhoneNumbers->index(i, 0).data().toUInt();

        QHash<quint32, int>::const_iterator it = id2ContactLookup.find(contactid);
        if (it != id2ContactLookup.end() && it.key() == contactid && it.value() >= 0 && it.value() < result.size())
        {
            QContactPhoneNumber number;
            if( affiliationNumbers )
                number.setContexts(QContactPhoneNumber::ContextWork);
            else
                number.setContexts(QContactPhoneNumber::ContextHome);
            number.setNumber(queryPhoneNumbers->index(i, 1).data().toString());
            number.setSubTypes(subtype);
            result[it.value()].saveDetail(&number);
        }
        else
            Q_ASSERT(false);
    }
}

void QTrackerContactFetchRequest::processQueryEmailAddresses( SopranoLive::LiveNodes queryEmailAddresses,
                                                              bool affiliationEmails)
{
    Q_ASSERT_X(queryEmailAddressNodesPending == 0, Q_FUNC_INFO, "Email query was supposed to be ready and it is not." );
    for (int i = 0; i < queryEmailAddresses->rowCount(); i++) {
        // ignore if next one is the same - asked iridian about making query to ignore supertypes
        // TODO remove after his answer
        if ( i-1 >= 0
             && (queryEmailAddresses->index(i, 0).data().toString()
                 == queryEmailAddresses->index(i-1, 0).data().toString())
             && (queryEmailAddresses->index(i, 1).data().toString()
                 == queryEmailAddresses->index(i-1, 1).data().toString())) {
            // this is for ignoring duplicates. bad approach, asked iridian
            // about how to eliminate super types in query results
            continue;
        }

        //QString subtype = rdfPhoneType2QContactSubtype(queryEmailAddresses->index(i, 2).data().toString());
        QContactLocalId contactid = queryEmailAddresses->index(i, 0).data().toUInt();

        QHash<quint32, int>::const_iterator it = id2ContactLookup.find(contactid);
        if (it != id2ContactLookup.end() && it.key() == contactid && it.value() >= 0 && it.value() < result.size())
        {
            QContactEmailAddress email;
            if (affiliationEmails)
                email.setContexts(QContactEmailAddress::ContextWork);
            else
                email.setContexts(QContactEmailAddress::ContextHome);
            email.setEmailAddress(queryEmailAddresses->index(i, 1).data().toString());
            //email.setSubTypes(subtype);
            result[it.value()].saveDetail(&email);
        }
        else
            Q_ASSERT(false);
    }
}


/*!
 * \brief Processes one query record-row during read from tracker to QContactOnlineAccount.
 * Order or columns in query is fixed to order defined in \sa prepareIMContactsQuery()
 */
QContactOnlineAccount QTrackerContactFetchRequest::getOnlineAccountFromIMQuery(LiveNodes imAccountQuery, int queryRow)
{
    QContactOnlineAccount account;
    QContactFetchRequest* r = qobject_cast<QContactFetchRequest*> (req);
    if(isMeContact(r->filter())) {
        account = getIMAccountFromIMQuery(imAccountQuery, queryRow);
    } else {
        account = getIMContactFromIMQuery(imAccountQuery, queryRow);
    }
    return account;
}

/*!
 * \brief processes IMQuery results. \sa prepareIMContactsQuery, contactsReady
 * Note: in contactsReady(), this method is called before processing PersonContacts - \sa result is filled with IMContacts first,
 * then metacontacts are processed while processing addressbook contacts - this is because QContactOnlineAccount details
 * need to be constructed before linking contacts with same metacontact.
 */
void QTrackerContactFetchRequest::processQueryIMContacts(SopranoLive::LiveNodes queryIMContacts)
{
    Q_ASSERT_X(queryIMAccountNodesPending == 0, Q_FUNC_INFO, "IMAccount query was supposed to be ready and it is not." );
    for (int i = 0; i < queryIMContacts->rowCount(); i++) {
        QContactOnlineAccount account = getOnlineAccountFromIMQuery(queryIMContacts, i);
        QContactLocalId contactid = queryIMContacts->index(i, IMContact::ContactId).data().toUInt();

        QHash<quint32, int>::const_iterator it = id2ContactLookup.find(contactid);
        if (it != id2ContactLookup.end() && it.key() == contactid && it.value() >= 0 && it.value() < result.size())
        {
            result[it.value()].saveDetail(&account);
        }
        else
        {
            QContact contact;
            QContactId id; id.setLocalId(contactid);

            QContactManagerEngine *engine = qobject_cast<QContactManagerEngine *>(parent());
            Q_ASSERT(engine);
            if(engine)
                id.setManagerUri(engine->managerUri());

            contact.setId(id);
            contact.saveDetail(&account);
            QString metacontact = queryIMContacts->index(i, IMContact::MetaContact).data().toString(); // \sa prepareIMContactsQuery()
            addContactToResultSet(contact, metacontact);
        }
    }
}

bool  QTrackerContactFetchRequest::isMeContact(const QContactFilter &filter) {
    if (filter.type() == QContactFilter::LocalIdFilter) {
         QContactManagerEngine *engine = dynamic_cast<QContactManagerEngine*>(parent());
         if(!engine) {
             qWarning() << __PRETTY_FUNCTION__ << ": Could not get QContactManager. Cannot retrieve IMAccounts for me-contact.";
             return false;
         }

        QContactManager::Error e;
        QContactLocalId selfId = engine->selfContactId(e);
        QContactLocalIdFilter filt = filter;
        if (filt.ids().contains(selfId)) {
            return true;
        }
    }
    return false;
}


QContactOnlineAccount QTrackerContactFetchRequest::getIMAccountFromIMQuery(LiveNodes imAccountQuery, int queryRow) {
    QContactOnlineAccount account;

    // Custom value in QContactrOnlineAccount detail to store the account path to - to determine in My Profile to ignore the ring-account.
    account.setValue("Account", imAccountQuery->index(queryRow, IMAccount::ContactIMId).data().toString()); // IMId
    // the same is supposed to be in FieldAccountUri field
    account.setValue(QContactOnlineAccount::FieldAccountUri, imAccountQuery->index(queryRow, IMAccount::ContactIMId).data().toString()); // IMId

    account.setNickname(imAccountQuery->index(queryRow, IMAccount::ContactNickname).data().toString()); // nick

    QString presence = imAccountQuery->index(queryRow, IMAccount::ContactPresence).data().toString(); // imPresence iri
    presence = presence.right(presence.length() - presence.lastIndexOf("presence-status"));
    account.setPresence(presenceConversion[presence]);
    qDebug() << "Presence converted: " << account.presence() << "raw presence: " << presence;

    account.setStatusMessage(imAccountQuery->index(queryRow, IMAccount::ContactMessage).data().toString()); // imStatusMessage

    return account;
}

QContactOnlineAccount QTrackerContactFetchRequest::getIMContactFromIMQuery(LiveNodes imContactQuery, int queryRow) {
    QContactOnlineAccount account;

    account.setValue("Account", imContactQuery->index(queryRow, IMContact::ContactIMId).data().toString()); // IMId
    if (!imContactQuery->index(queryRow, IMContact::AccountType).data().toString().isEmpty()) {
        QString accountPathURI = imContactQuery->index(queryRow, IMContact::AccountType).data().toString();
        QStringList decoded = accountPathURI.split(":");
        // taking out the prefix "telepathy:"
        qDebug() << __PRETTY_FUNCTION__ << decoded.value(1);
        account.setValue(FieldAccountPath, decoded.value(1));
    }
    account.setNickname(imContactQuery->index(queryRow, IMContact::ContactNickname).data().toString()); // nick

    QString cap = imContactQuery->index(queryRow, IMContact::Capabilities).data().toString();
    cap = cap.right(cap.length() - cap.lastIndexOf("im-capability"));
    account.setValue(QContactOnlineAccount::FieldCapabilities, cap);

    QString presence = imContactQuery->index(queryRow, IMContact::ContactPresence).data().toString(); // imPresence iri
    presence = presence.right(presence.length() - presence.lastIndexOf("presence-status"));
    account.setPresence(presenceConversion[presence]);
 
    account.setStatusMessage(imContactQuery->index(queryRow, IMContact::ContactMessage).data().toString()); // imStatusMessage
    account.setServiceProvider(imContactQuery->index(queryRow, IMContact::ServiceProvider).data().toString()); // service name

    return account;
}
