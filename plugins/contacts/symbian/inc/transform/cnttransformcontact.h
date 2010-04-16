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

#ifndef TRANSFORMCONCTACT_H
#define TRANSFORMCONCTACT_H

#include <qtcontacts.h>

#include <cntfldst.h>
#include <cntdb.h>
#include <cntdef.h>
#include <cntitem.h>

class CntTransformContactData;

QTM_BEGIN_NAMESPACE
class QContactDetailDefinition;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntTransformContact
{
public:
	CntTransformContact();
	virtual ~CntTransformContact();

public:
	QContact transformContactL(CContactItem &contact, const QStringList& definitionRestrictions = QStringList()) const;
	void transformPostSaveDetailsL(
	        const CContactItem& contactItem,
	        QContact& contact,
	        const CContactDatabase &contactDatabase,
	        QString managerUri) const;
	void transformContactL(
	        QContact &contact,
	        CContactItem &contactItem) const;
	QList<TUid> supportedSortingFieldTypes( QString detailDefinitionName, QString detailFieldName );
    TUint32 GetIdForDetailL(const QContactDetailFilter& detailFilter,bool& isSubtype) const;
    void detailDefinitions(QMap<QString, QContactDetailDefinition>& defaultSchema, const QString& contactType, QContactManager::Error* error) const;
    QContactDetail *transformGuidItemFieldL(const CContactItem &contactItem, const CContactDatabase &contactDatabase) const;
    QContactDetail *transformTimestampItemFieldL(const CContactItem &contactItem, const CContactDatabase &contactDatabase) const;
private:
	enum ContactData
	{
		Name = 0,
		Nickname,
		PhoneNumber,
		EmailAddress,
		Address,
		URL,
		OnlineAccount,
		Presence,
		Birthday,
		Organisation,
		Avatar,
		SyncTarget,
		Gender,
		Anniversary,
		Geolocation,
		Note,
		Family,
        Ringtone,
        Thumbnail,
		Empty
	};

	void initializeCntTransformContactData();
	QList<CContactItemField *> transformDetailL(const QContactDetail &detail) const;
	QContactDetail *transformItemField(const CContactItemField& field, const QContact &contact) const;
	void transformPreferredDetailL(const QContact& contact, const QContactDetail& detail, QList<CContactItemField*> &fieldList) const;
	void transformPreferredDetail(const CContactItemField& field, const QContactDetail& detail, QContact& contact) const;
	void resetTransformObjects() const;
	
private:
	QMap<ContactData, CntTransformContactData*> m_transformContactData;
};

#endif /* TRANSFORMCONCTACT_H_ */
