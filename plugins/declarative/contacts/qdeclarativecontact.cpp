/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcontactdetails.h"
#include "qcontactmanager.h"
#include "qdeclarativecontact_p.h"
#include "qdeclarativecontactdetail_p.h"
#include "qdeclarativecontactmetaobject_p.h"

#include <QDeclarativeListProperty>


QDeclarativeContact::QDeclarativeContact(QObject *parent)
    :QObject(parent),
    d(new QDeclarativeContactMetaObject(this, QContact()))
{
}


QDeclarativeContact::QDeclarativeContact(const QContact& contact, const QMap<QString, QContactDetailDefinition>& defs, QObject *parent)
    :QObject(parent),
    d(new QDeclarativeContactMetaObject(this, contact))
{
    setDetailDefinitions(defs);
}

QDeclarativeContact::~QDeclarativeContact()
{
    delete d;
}

void QDeclarativeContact::setDetailDefinitions(const QMap<QString, QContactDetailDefinition>& defs)
{
    d->m_defs = defs;
}

QMap<QString, QContactDetailDefinition> QDeclarativeContact::detailDefinitions() const
{
    return d->m_defs;
}

void QDeclarativeContact::setContact(const QContact& contact)
{
   d->setContact(contact);
}

QContact QDeclarativeContact::contact() const
{
    return d->contact();
}

QDeclarativeListProperty<QDeclarativeContactDetail> QDeclarativeContact::details()
{
    return d->details(QString()).value< QDeclarativeListProperty<QDeclarativeContactDetail> >();
}

QContactLocalId QDeclarativeContact::contactId() const
{
    return d->localId();
}


QVariant QDeclarativeContact::detail(const QString& name)
{
    return d->detail(name);
}

QVariant QDeclarativeContact::details(const QString& name)
{
    return d->details(name);
}

void QDeclarativeContact::clearDetails()
{
    d->m_details.clear();
    emit detailsChanged();
}

QDeclarativeContactAddress* QDeclarativeContact::address()
{
    if (d->m_defs.isEmpty())
}
QDeclarativeContactAnniversary* QDeclarativeContact::anniversary()
{
}
QDeclarativeContactAvatar* QDeclarativeContact::avatar()
{
}
QDeclarativeContactBirthday*  QDeclarativeContact::birthday()
{
}
QDeclarativeContactDisplayLabel*  QDeclarativeContact::displayLabel()
{
}
QDeclarativeContactEmailAddress*  QDeclarativeContact::email()
{
}
QDeclarativeContactFamily*  QDeclarativeContact::family()
{
}
QDeclarativeContactFavorite*  QDeclarativeContact::favorite()
{
}
QDeclarativeContactGender*  QDeclarativeContact::gender()
{
}
QDeclarativeContactGeoLocation*  QDeclarativeContact::geolocation()
{
}
QDeclarativeContactGlobalPresence*  QDeclarativeContact::globalPresence()
{
}
QDeclarativeContactGuid*  QDeclarativeContact::guid()
{
}
QDeclarativeContactName*  QDeclarativeContact::name()
{
}
QDeclarativeContactNickname*  QDeclarativeContact::nickname()
{
}
QDeclarativeContactNote*  QDeclarativeContact::note()
{
}
QDeclarativeContactOnlineAccount*  QDeclarativeContact::onlineAccount()
{
}
QDeclarativeContactOrganization*  QDeclarativeContact::organization()
{
}
QDeclarativeContactPhoneNumber*  QDeclarativeContact::phoneNumber()
{
}
QDeclarativeContactPresence*  QDeclarativeContact::presence()
{
}
QDeclarativeContactRingtone*  QDeclarativeContact::ringtone()
{
}
QDeclarativeContactSyncTarget*  QDeclarativeContact::syncTarget()
{
}
QDeclarativeContactTag*  QDeclarativeContact::tag()
{
}
QDeclarativeContactThumbnail*  QDeclarativeContact::thumbnail()
{
}
QDeclarativeContactType*  QDeclarativeContact::type()
{
}
QDeclarativeContactUrl*  QDeclarativeContact::url()
{
}
