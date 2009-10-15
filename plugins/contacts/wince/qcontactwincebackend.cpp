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
#include <QDebug>

#include "qcontact_p.h"
#include "qcontactmanager.h"
#include "qcontactmanager_p.h"
#include "qcontactchangeset.h"

#define INITGUID
#include "qcontactwincebackend_p.h"

/*
 * This file is most of the engine plumbing - conversion to/from POOM is in
 * the contactconversions.cpp file.
 */

/*
 * TODOS
 *
 * - refactor (much needed)
 *  - single properties should have common code
 *  - error reporting when we run out of room
 * - use QScopedPointer
 * - pronunciation (needs schema update)
 * - any XXX comments
 * - change qDebugs to qWarnings or remove, as appropriate
 * - change factory to create the data, if any failures, return NULL manager.
 * - store extra metadata
 *  - Voice subtype for phone, don't always assume
 * - Address formatting - PIMPR_HOME_ADDRESS seems to be read only
 */


QContactWinCEEngine::QContactWinCEEngine(const QMap<QString, QString>& , QContactManager::Error& error)
    : d(new QContactWinCEEngineData)
{
    error = QContactManager::NoError;
    
    buildHashForContactDetailToPoomPropId();
    
    if (SUCCEEDED(d->m_cominit.hr())) {
        if (SUCCEEDED(CoCreateInstance(CLSID_Application, NULL,
                                       CLSCTX_INPROC_SERVER, IID_IPOutlookApp2,
                                       reinterpret_cast<void **>(&d->m_app)))) {
            if(FAILED(d->m_app->Logon(NULL))) {
                qDebug() << "Failed to log on";
                d->m_app = 0;
            } else {
                if(SUCCEEDED(d->m_app->GetDefaultFolder(olFolderContacts, &d->m_folder))) {
                    if(SUCCEEDED(d->m_folder->get_Items(&d->m_collection))) {
                        // Register/retrieve our custom ids
                        LPCWSTR customIds[2] = { L"QTCONTACTS_PHONE_META", L"QTCONTACTS_EMAIL_META" };
                        CEPROPID outIds[2];

                        if (SUCCEEDED(d->m_app->GetIDsFromNames(2, customIds, PIM_CREATE | CEVT_LPWSTR, outIds))) {
                            d->m_phonemeta = outIds[0];
                            d->m_emailmeta = outIds[1];
                        }

                        // get an IPOLItems2 pointer for the collection, too
                        if (SUCCEEDED(d->m_collection->QueryInterface<IPOlItems2>(&d->m_items2))) {
                            d->m_ids = convertP2QIdList(d->m_collection);
                        }
                    } else {
                        qDebug() << "Failed to get items";
                        d->m_collection = 0;
                    }
                } else {
                    qDebug() << "Failed to get contacts folder";
                    d->m_folder = 0;
                }
            }
        }
    }
    d->m_requestWorker.start();
}

QContactWinCEEngine::QContactWinCEEngine(const QContactWinCEEngine& other)
    : QContactManagerEngine(), d(other.d)
{

}

QContactWinCEEngine& QContactWinCEEngine::operator=(const QContactWinCEEngine& other)
{
    // assign
    d = other.d;

    return *this;
}

QContactWinCEEngine::~QContactWinCEEngine()
{
    if (d->m_app) {
        d->m_app->Logoff();
    }
}

void QContactWinCEEngine::deref()
{
    if (!d->m_refCount.deref())
        delete this;
}

QContact QContactWinCEEngine::contact(const QContactLocalId& contactId, QContactManager::Error& error) const
{
    QContact ret;

    // id == 0 gives a bad argument error from POOM, so don't even try it
    if (contactId != 0) {
        // Fetch!
        SimpleComPointer<IItem> item = 0;
        HRESULT hr = d->m_app->GetItemFromOidEx(contactId, 0, &item);
        if (SUCCEEDED(hr)) {
            if (item) {
                error = QContactManager::NoError;
                ret = convertToQContact(item);
            } else {
                error = QContactManager::DoesNotExistError;
            }
        } else {
            if (HRESULT_CODE(hr) == ERROR_NOT_FOUND) {
                error = QContactManager::DoesNotExistError;
            } else {
                qDebug() << "Failed to retrieve contact:" << HRESULT_CODE(hr);
                error = QContactManager::UnspecifiedError;
            }
        }
    } else {
        error = QContactManager::DoesNotExistError;
    }
    return ret;
}

bool QContactWinCEEngine::saveContact(QContact* contact, QContactManager::Error& error)
{
    if (contact == 0) {
        error = QContactManager::BadArgumentError;
        return false;
    }

    QContactChangeSet cs;

    // ensure that the contact's details conform to their definitions
    if (!validateContact(*contact, error)) {
        error = QContactManager::InvalidDetailError;
        return false;
    }

    SimpleComPointer<IItem> icontact;
    bool wasOld = false;
    // Figure out if this is a new or old contact
    if (d->m_ids.contains(contact->id())) {
        // update existing contact
        HRESULT hr = d->m_app->GetItemFromOidEx(contact->id(), 0, &icontact);
        if (SUCCEEDED(hr)) {
            wasOld = true;
        } else {
            if (HRESULT_CODE(hr) == ERROR_NOT_FOUND) {
                // Well, doesn't exist any more
                error = QContactManager::DoesNotExistError;
                d->m_ids.removeAll(contact->id());
            } else {
                qDebug() << "Didn't get old contact" << HRESULT_CODE(hr);
                error = QContactManager::UnspecifiedError;
            }
        }
    } else if (contact->id() == 0) {
        // new contact!
        SimpleComPointer<IDispatch> idisp = 0;
        HRESULT hr = d->m_collection->Add(&idisp);
        if (SUCCEEDED(hr)) {
            // now get an actual item out of it (was IContact, which is not IItem)
            hr = idisp->QueryInterface<IItem>(&icontact);

            if (SUCCEEDED(hr)) {
            } else {
                qDebug() << "Failed to query interface" << HRESULT_CODE(hr);
                error = QContactManager::UnspecifiedError;
            }
        } else {
            qDebug() << "Failed to create contact: "<< HRESULT_CODE(hr);
            error = QContactManager::OutOfMemoryError;
        }
    } else {
        // Saving a contact with a non zero id, but that doesn't exist
        error = QContactManager::DoesNotExistError;
    }

    if (icontact) {
        // Convert our QContact to the Icontact (via setProps)
        if (convertFromQContact(*contact, icontact, error)) {
            HRESULT hr = icontact->Save();
            if (SUCCEEDED(hr)) {
                // yay! we also need to set the new contact id
                long oid = 0;
                hr = icontact->get_Oid(&oid);
                if (SUCCEEDED(hr)) {
                    error = QContactManager::NoError; 
                    QContact c = this->contact((QUniqueId)oid, error);
                    
                    if (error == QContactManager::NoError) {
                        *contact = c;
                        if (wasOld) {
                            cs.changedContacts().insert(contact->id());
                        } else {
                            cs.addedContacts().insert(contact->id());
                            d->m_ids.append(contact->id());
                        }
                    }

                    cs.emitSignals(this);
                    return true;
                }
                qDebug() << "Saved contact, but couldn't retrieve id again??" << HRESULT_CODE(hr);
                // Blargh.
                error = QContactManager::UnspecifiedError;
            } else {
                qDebug() << "Failed to save contact" << HRESULT_CODE(hr);
            }
        } else {
            qDebug() << "Failed to convert contact";
        }
    }

    // error should have been set.
    return false;
}

bool QContactWinCEEngine::removeContact(const QContactLocalId& contactId, QContactManager::Error& error)
{
    // Fetch an IItem* for this
    if (contactId != 0) {
        SimpleComPointer<IItem> item ;
        QContactChangeSet cs;

        HRESULT hr = d->m_app->GetItemFromOidEx(contactId, 0, &item);
        if (SUCCEEDED(hr)) {
            hr = item->Delete();
            if (SUCCEEDED(hr)) {
                error = QContactManager::NoError;
                d->m_ids.removeAll(contactId);
                cs.removedContacts().insert(contactId);
                cs.emitSignals(this);
                return true;
            }
            qDebug() << "Failed to delete:" << HRESULT_CODE(hr);
            error = QContactManager::UnspecifiedError;
        } else {
            if (HRESULT_CODE(hr) == ERROR_NOT_FOUND) {
                error = QContactManager::DoesNotExistError;
            } else {
                qDebug() << "Failed to retrieve item pointer in delete" << HRESULT_CODE(hr);
                error = QContactManager::UnspecifiedError;
            }
        }
    } else {
        // Id 0 does not exist
        error = QContactManager::DoesNotExistError;
    }
    return false;
}

QMap<QString, QContactDetailDefinition> QContactWinCEEngine::detailDefinitions(QContactManager::Error& error) const
{
    error = QContactManager::NoError;
    QMap<QString, QContactDetailDefinition> defns = QContactManagerEngine::schemaDefinitions();

    // Remove the details we don't support
    defns.remove(QContactSyncTarget::DefinitionName);
    defns.remove(QContactPresence::DefinitionName);
    defns.remove(QContactGeolocation::DefinitionName);
    defns.remove(QContactTimestamp::DefinitionName);
    defns.remove(QContactGuid::DefinitionName);
    defns.remove(QContactGender::DefinitionName); // ? Surprising
    defns.remove(QContactRelationship::DefinitionName); // XXX perhaps we should fake it

    // Remove the fields we don't support

    // Simple anniversarys
    defns[QContactAnniversary::DefinitionName].fields().remove(QContactAnniversary::FieldCalendarId);
    defns[QContactAnniversary::DefinitionName].fields().remove(QContactAnniversary::FieldEvent);
    defns[QContactAnniversary::DefinitionName].fields().remove(QContactAnniversary::FieldSubType);

    // No logo for organisation
    defns[QContactOrganization::DefinitionName].fields().remove(QContactOrganization::FieldLogo);

    // No subtypes for these details
    defns[QContactAddress::DefinitionName].fields().remove(QContactAddress::FieldSubTypes);
    defns[QContactUrl::DefinitionName].fields().remove(QContactUrl::FieldSubType);
    defns[QContactAvatar::DefinitionName].fields().remove(QContactAvatar::FieldSubType);

    // No contexts for these details
    defns[QContactAvatar::DefinitionName].fields().remove(QContactDetail::FieldContext);
    defns[QContactAnniversary::DefinitionName].fields().remove(QContactDetail::FieldContext);
    defns[QContactBirthday::DefinitionName].fields().remove(QContactDetail::FieldContext);
    defns[QContactName::DefinitionName].fields().remove(QContactDetail::FieldContext);
    defns[QContactNickname::DefinitionName].fields().remove(QContactDetail::FieldContext);
    defns[QContactOrganization::DefinitionName].fields().remove(QContactDetail::FieldContext);
    defns[QContactUrl::DefinitionName].fields().remove(QContactDetail::FieldContext);

    // Simple phone number types (non multiple)
    // defns[QContactPhoneNumber::DefinitionName].fields()[QContactPhoneNumber::FieldSubTypes].dataType = QVariant::String; // XXX doesn't work
    defns[QContactPhoneNumber::DefinitionName].fields()[QContactPhoneNumber::FieldSubTypes].allowableValues.removeAll(QString(QLatin1String(QContactPhoneNumber::SubTypeBulletinBoardSystem)));
    defns[QContactPhoneNumber::DefinitionName].fields()[QContactPhoneNumber::FieldSubTypes].allowableValues.removeAll(QString(QLatin1String(QContactPhoneNumber::SubTypeLandline)));
    defns[QContactPhoneNumber::DefinitionName].fields()[QContactPhoneNumber::FieldSubTypes].allowableValues.removeAll(QString(QLatin1String(QContactPhoneNumber::SubTypeMessagingCapable)));
    defns[QContactPhoneNumber::DefinitionName].fields()[QContactPhoneNumber::FieldSubTypes].allowableValues.removeAll(QString(QLatin1String(QContactPhoneNumber::SubTypeModem)));
    defns[QContactPhoneNumber::DefinitionName].fields()[QContactPhoneNumber::FieldSubTypes].allowableValues.removeAll(QString(QLatin1String(QContactPhoneNumber::SubTypeVideo)));

    // XXX temporary definitions that we should support but don't yet.
    defns.remove(QContactOnlineAccount::DefinitionName);
    defns.remove(QContactAvatar::DefinitionName);

    return defns;
}


/*! \reimp */
void QContactWinCEEngine::requestDestroyed(QContactAbstractRequest* req)
{
    d->m_requestWorker.removeRequest(req);
}

/*! \reimp */
bool QContactWinCEEngine::startRequest(QContactAbstractRequest* req)
{
    return d->m_requestWorker.addRequest(req);
}

/*! \reimp */
bool QContactWinCEEngine::cancelRequest(QContactAbstractRequest* req)
{
    return  d->m_requestWorker.cancelRequest(req);
}

/*! \reimp */
bool QContactWinCEEngine::waitForRequestProgress(QContactAbstractRequest* req, int msecs)
{
    return d->m_requestWorker.waitRequest(req, msecs);
}

/*! \reimp */
bool QContactWinCEEngine::waitForRequestFinished(QContactAbstractRequest* req, int msecs)
{
    return d->m_requestWorker.waitRequest(req, msecs) && req->isFinished();
}

/*! \reimp */
bool QContactWinCEEngine::hasFeature(QContactManagerInfo::ManagerFeature feature) const
{
    // The Windows CE backend is an "isolated" backend
    if (feature == QContactManagerInfo::Anonymous)
        return true;

    // Windows CE backend does not support Mutable Definitions, Groups or Action Preferences
    return false;
}

/* Synthesise the display label of a contact */
QString QContactWinCEEngine::synthesiseDisplayLabel(const QContact& contact, QContactManager::Error& error) const
{
    Q_UNUSED(error)
    // The POOM API (well, lack thereof) makes this a bit strange.
    // It's basically just "Last, First" or "Company", if "FileAs" is not set.
    QContactName name = contact.detail<QContactName>();
    QContactOrganization org = contact.detail<QContactOrganization>();

    // Basically we ignore any existing labels for this contact, since we're being
    // asked what the synthesized label would be

    // XXX For greatest accuracy we might be better off converting this contact to
    // a real item (but don't save it), and then retrieve it...
    if (!name.last().isEmpty()) {
        if (!name.first().isEmpty()) {
            return QString(QLatin1String("%1, %2")).arg(name.last()).arg(name.first());
        } else {
            // Just last
            return name.last();
        }
    } else if (!name.first().isEmpty()) {
        return name.first();
    } else if (!org.name().isEmpty()) {
        return org.name();
    } else {
        // XXX grargh.
        return QLatin1String("Unnamed");
    }
}

/*! \reimp */
bool QContactWinCEEngine::filterSupported(const QContactFilter& filter) const
{
    switch (filter.type()) {
        case QContactFilter::InvalidFilter:
        case QContactFilter::DefaultFilter:
        case QContactFilter::IdListFilter:
        case QContactFilter::ContactDetailFilter:
        case QContactFilter::ContactDetailRangeFilter:
        case QContactFilter::ActionFilter:
        case QContactFilter::IntersectionFilter:
        case QContactFilter::UnionFilter:
            return true;
    }
    return false;
}
/*!
 * Returns the list of data types supported by the WinCE engine
 */
QList<QVariant::Type> QContactWinCEEngine::supportedDataTypes() const
{
    QList<QVariant::Type> st;
    st.append(QVariant::String);
    st.append(QVariant::Int);
    st.append(QVariant::UInt);
    st.append(QVariant::Double);
    st.append(QVariant::Date);
    st.append(QVariant::DateTime);

    return st;
}

/* Factory lives here in the basement */
QContactManagerEngine* ContactWinceFactory::engine(const QMap<QString, QString>& parameters, QContactManager::Error& error)
{
    return new QContactWinCEEngine(parameters, error);
}

QString ContactWinceFactory::managerName() const
{
    return QString("wince");
}
Q_EXPORT_PLUGIN2(contacts_wince, ContactWinceFactory);
