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
#include "transformbirthday.h"

QList<CContactItemField *> TransformBirthday::transformDetailL(const QContactDetail &detail)
{
	QList<CContactItemField *> fieldList; 
	
	//cast to birthday
	const QContactBirthday &birthday(static_cast<const QContactUrl&>(detail));
	
	//create new field
	TDateTime dateTime(birthday.date().year(), TMonth(birthday.date().month() - 1), birthday.date().day(), 0, 0, 0, 0);
	CContactItemField* newField = CContactItemField::NewLC(KStorageTypeDateTime, KUidContactFieldBirthday);
 	newField->DateTimeStorage()->SetTime(dateTime);
	newField->SetMapping(KUidContactFieldVCardMapBDAY);
	
	fieldList.append(newField);
	CleanupStack::Pop(newField);
		
	return fieldList;
}

QContactDetail *TransformBirthday::transformItemField(const CContactItemField& field, const QContact &contact)
{
	Q_UNUSED(contact);
	
	QContactBirthday *birthday = new QContactBirthday();
	
	CContactDateField* storage = field.DateTimeStorage();
	TTime time(storage->Time());
	QDate qDate(time.DateTime().Year(), time.DateTime().Month() + 1, time.DateTime().Day());
	birthday->setDate(qDate);
	
	return birthday;
}

bool TransformBirthday::supportsField(TUint32 fieldType) const
{
    bool ret = false;
    if (fieldType == KUidContactFieldBirthday.iUid) {
        ret = true;
    }
    return ret;
}

bool TransformBirthday::supportsDetail(QString detailName) const
{
    bool ret = false;
    if (detailName == QContactBirthday::DefinitionName) {
        ret = true;
    }
    return ret;
}

QList<TUid> TransformBirthday::supportedSortingFieldTypes(QString detailFieldName) const
{
    QList<TUid> uids;
    if( detailFieldName == QContactBirthday::FieldBirthday )
        uids << KUidContactFieldBirthday;
    return uids;
}
