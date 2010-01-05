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
#include "cnttransformorganisation.h"

QList<CContactItemField *> CntTransformOrganisation::transformDetailL(const QContactDetail &detail)
{
    if(detail.definitionName() != QContactOrganization::DefinitionName)
       User::Leave(KErrArgument);

    QList<CContactItemField *> fieldList;

	//cast to orgenisation
    const QContactOrganization &orgDetails(static_cast<const QContactOrganization&>(detail));

    //create new fields without contexts
    transformToTextFieldL(orgDetails, fieldList, orgDetails.name(), KUidContactFieldCompanyName, KUidContactFieldVCardMapORG, false);
	QStringList departments = orgDetails.department();
	if(departments.count()) // Take only the first department
	    transformToTextFieldL(orgDetails, fieldList, departments[0], KUidContactFieldDepartmentName, KUidContactFieldVCardMapDepartment, false);
	transformToTextFieldL(orgDetails, fieldList, orgDetails.title(), KUidContactFieldJobTitle, KUidContactFieldVCardMapTITLE, false);
    transformToTextFieldL(orgDetails, fieldList, orgDetails.assistantName(), KUidContactFieldAssistant, KUidContactFieldVCardMapAssistant, false);

	return fieldList;
}


QContactDetail *CntTransformOrganisation::transformItemField(const CContactItemField& field, const QContact &contact)
{
    QContactOrganization *organisation = new QContactOrganization(contact.detail<QContactOrganization>());

	CContactTextField* storage = field.TextStorage();
	QString orgDetail = QString::fromUtf16(storage->Text().Ptr(), storage->Text().Length());

	for (int i = 0; i < field.ContentType().FieldTypeCount(); i++) {
		//Company
		if (field.ContentType().FieldType(i) == KUidContactFieldCompanyName) {
            organisation->setName(orgDetail);
		}
		//Department
		else if (field.ContentType().FieldType(i) == KUidContactFieldDepartmentName) {
		    // Assume only a single department
		    QStringList departments = QStringList(orgDetail);
            organisation->setDepartment(departments);
		}
		//Job title
		else if (field.ContentType().FieldType(i) == KUidContactFieldJobTitle) {
            organisation->setTitle(orgDetail);
		}
	    //Assistant name
	    else if (field.ContentType().FieldType(i) == KUidContactFieldAssistant) {
            organisation->setAssistantName(orgDetail);
	    }
	}

	return organisation;
}

bool CntTransformOrganisation::supportsField(TUint32 fieldType) const
{
    bool ret = false;
    if (fieldType == KUidContactFieldCompanyName.iUid ||
        fieldType == KUidContactFieldDepartmentName.iUid ||
        fieldType == KUidContactFieldJobTitle.iUid ||
        fieldType == KUidContactFieldAssistant.iUid) {
        ret = true;
    }
    return ret;
}

bool CntTransformOrganisation::supportsDetail(QString detailName) const
{
    bool ret = false;
    if (detailName == QContactOrganization::DefinitionName) {
        ret = true;
    }
    return ret;
}

QList<TUid> CntTransformOrganisation::supportedSortingFieldTypes(QString detailFieldName) const
{
    QList<TUid> uids;

    if (detailFieldName == QContactOrganization::FieldName)
        return uids << KUidContactFieldCompanyName;

    if (detailFieldName == QContactOrganization::FieldDepartment)
        return uids << KUidContactFieldDepartmentName;

    if (detailFieldName == QContactOrganization::FieldTitle)
        return uids << KUidContactFieldJobTitle;

    if (detailFieldName == QContactOrganization::FieldAssistantName)
        return uids << KUidContactFieldAssistant;

    return uids;
}


/*!
 * Checks whether the subtype is supported
 *
 * \a subType The subtype to be checked
 * \return True if this subtype is supported
 */
bool CntTransformOrganisation::supportsSubType(const QString& subType) const
{
    Q_UNUSED(subType);
    return false;
}

/*!
 * Returns the filed id corresponding to a field
 *
 * \a fieldName The name of the supported field
 * \return fieldId for the fieldName, 0  if not supported
 */
quint32 CntTransformOrganisation::getIdForField(const QString& fieldName) const
{

    if (QContactOrganization::FieldName  == fieldName)
        return KUidContactFieldCompanyName.iUid;
    else if (QContactOrganization::FieldLogo == fieldName)
        return 0;
    else if (QContactOrganization::FieldDepartment == fieldName)
        return KUidContactFieldDepartmentName.iUid;
    else if (QContactOrganization::FieldLocation == fieldName)
        return 0;
    else if (QContactOrganization::FieldTitle == fieldName)
        return KUidContactFieldJobTitle.iUid;
    else if (QContactOrganization::FieldAssistantName == fieldName)
        return KUidContactFieldAssistant.iUid;
    else
        return 0;

}

/*!
 * Modifies the detail definitions. The default detail definitions are
 * queried from QContactManagerEngine::schemaDefinitions and then modified
 * with this function in the transform leaf classes.
 *
 * \a definitions The detail definitions to modify.
 * \a contactType The contact type the definitions apply for.
 */
void CntTransformOrganisation::detailDefinitions(QMap<QString, QContactDetailDefinition> &definitions, const QString& contactType) const
{
    Q_UNUSED(contactType);

    if(definitions.contains(QContactOrganization::DefinitionName)) {
        QContactDetailDefinition d = definitions.value(QContactOrganization::DefinitionName);
        QMap<QString, QContactDetailDefinitionField> fields = d.fields();
        QContactDetailDefinitionField f;

        // Not all fields are supported, replace:
        fields.clear();
        f.setDataType(QVariant::String);
        f.setAllowableValues(QVariantList());
        fields.insert(QContactOrganization::FieldName, f);
        fields.insert(QContactOrganization::FieldTitle, f);
        fields.insert(QContactOrganization::FieldAssistantName, f);
        f.setDataType(QVariant::StringList);
        fields.insert(QContactOrganization::FieldDepartment, f);

        d.setFields(fields);

        // Replace original definitions
        definitions.insert(d.name(), d);
    }
}
