/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include <centralrepository.h>
#include <cntfldst.h>

#include "cntfilterdetail.h"
#include "cntsymbianengine.h"
#include "cnttransformphonenumber.h"

// Telephony Configuration API
// Keys under this category are used in defining telephony configuration.
const TUid KCRUidTelConfiguration = {0x102828B8};
// Amount of digits to be used in contact matching.
// This allows a customer to variate the amount of digits to be matched.
const TUint32 KTelMatchDigits                               = 0x00000001;
// Default match length
const TInt KDefaultMatchLength(7);

CntFilterDetail::CntFilterDetail(CContactDatabase& contactDatabase,CntSymbianSrvConnection &cntServer,CntDbInfo& dbInfo) 
                                        : m_contactdatabase(contactDatabase),
                                          m_srvConnection(cntServer),
                                          m_dbInfo(dbInfo),
                                          m_emulateBestMatching(false)
{
}

CntFilterDetail::~CntFilterDetail()
{
}


QList<QContactLocalId> CntFilterDetail::contacts(
        const QContactFilter &filter,
        const QList<QContactSortOrder> &sortOrders,
        bool &filterSupportedflag,
        QContactManager::Error* error)
{
    //Check if any invalid filter is passed 
    if (!filterSupported(filter) ) {
        *error =  QContactManager::NotSupportedError;
        filterSupportedflag = false;
        return QList<QContactLocalId>();
    }
    QList<QContactLocalId> idList;
    QContactDetailFilter detailFilter(filter);
    QString sqlQuery;
    //Check for phonenumber. Special handling needed
    if ( (detailFilter.detailDefinitionName() == QContactPhoneNumber::DefinitionName ) ) {
        //Handle phonenumber ...
        createPhoneNumberQuery(filter,sqlQuery,error);
		
        if (*error == QContactManager::NoError) {
            //fetch the contacts
            idList =  m_srvConnection.searchContacts(sqlQuery,error);
        }
    }

    else if (detailFilter.matchFlags() == QContactFilter::MatchKeypadCollation) {
        //predictive search filter
        idList = HandlePredictiveSearchFilter(filter,error);
    }
            
    // handle other cases
    else {
        createSelectQuery(filter,sqlQuery,error);
        QString sortQuery = m_dbInfo.getSortQuery(sortOrders, sqlQuery, error);
        
        if (*error == QContactManager::NoError) {
            //fetch the contacts
            idList =  m_srvConnection.searchContacts(sortQuery, error);
        }
    }
    return idList;
}

bool CntFilterDetail::filterSupported(const QContactFilter& filter) 
{
    bool result = false;
    if (QContactFilter::ContactDetailFilter == filter.type()) {
        QContactDetailFilter detailFilter = static_cast<QContactFilter>(filter);
        if (m_dbInfo.SupportsDetail(detailFilter.detailDefinitionName(),
                detailFilter.detailFieldName())) {
            result = true;
        }
        else if (detailFilter.detailDefinitionName() == QContactPhoneNumber::DefinitionName) {
            //special case - phone number matching 
            result = true;
        }
        else if (detailFilter.detailDefinitionName() == QContactEmailAddress::DefinitionName &&
            detailFilter.detailFieldName().isEmpty()) {
            //filtering contacts having email address
            result = true;
        }
        else if (detailFilter.detailDefinitionName() == QContactOnlineAccount::DefinitionName &&
            detailFilter.detailFieldName().isEmpty()) {
            //filtering contacts having online account
            result = true;
        }
        else if (detailFilter.detailDefinitionName() == QContactType::DefinitionName &&
            detailFilter.detailFieldName() == QContactType::FieldType) {
            //filtering by contact type is supported
            result = true;
        }
        else if (detailFilter.detailDefinitionName() == QContactGuid::DefinitionName &&
            detailFilter.detailFieldName() == QContactGuid::FieldGuid) {
            //filtering by global Uid is supported
            result = true;
        }
    }
    return result;
}

void CntFilterDetail::createSelectQuery(const QContactFilter& filter,
                                        QString& sqlQuery,
                                        QContactManager::Error* error)

{
    if (!filterSupported(filter)) {
      *error = QContactManager::NotSupportedError;
      return;
    }
    QContactDetailFilter detailFilter(filter);
    //type
    if (detailFilter.detailDefinitionName() == QContactType::DefinitionName) {
       if (detailFilter.value().toString() == QContactType::TypeContact)
           sqlQuery = "SELECT contact_id FROM contact WHERE (type_flags>>24)<=1";
       else if (detailFilter.value().toString() == QContactType::TypeGroup)
           sqlQuery = "SELECT contact_id FROM contact WHERE (type_flags>>24)=3";
    }
    else if (detailFilter.detailDefinitionName() == QContactGuid::DefinitionName) {
       if (detailFilter.detailFieldName() == QContactGuid::FieldGuid) {
           QStringList fullGuidValue = detailFilter.value().toString().split('-');
           if (fullGuidValue.count() == 3) {
               QString localGuidValue = fullGuidValue.at(1);
               sqlQuery = "SELECT contact_id FROM contact WHERE guid_string = '" + localGuidValue + '\'';
           }
       }
    }
    else if (detailFilter.detailDefinitionName() == QContactEmailAddress::DefinitionName &&
                        (detailFilter.detailFieldName().isEmpty())) {
        QString type =  QString(" type = %1").arg(CntDbInfo::EEmailAddress);
        QString whereClause = " WHERE" + type;
        sqlQuery = "SELECT DISTINCT contact_id FROM comm_addr" + whereClause;
        }
    else if (detailFilter.detailDefinitionName() == QContactOnlineAccount::DefinitionName &&
                        (detailFilter.detailFieldName().isEmpty())) {
        QString type =  QString(" type = %1").arg(CntDbInfo::ESipAddress);
        QString whereClause = " WHERE" + type;
        sqlQuery = "SELECT DISTINCT contact_id FROM comm_addr" + whereClause;
        }
    //everything else
    else {   
       QString tableName;
       QString sqlWhereClause;
       getTableNameWhereClause(detailFilter,tableName,sqlWhereClause,error);
       //Create the sql query
       sqlQuery += "SELECT DISTINCT contact_id FROM " + tableName + " WHERE " + sqlWhereClause;
    }
}

/*!
 * Updates match flags for columns.
 */
void CntFilterDetail::updateForMatchFlag(const QContactDetailFilter& filter,
                                         QString& fieldToUpdate ,
                                         QContactManager::Error* error) const
{
    // Modify the filed depending on the query
    switch (filter.matchFlags()) {
        case QContactFilter::MatchExactly: {
            // Pattern for MatchExactly:
            // " ='xyz'"
            fieldToUpdate = " ='"
                           + filter.value().toString() + '\'';
            *error = QContactManager::NoError;
            break;
        }
        case QContactFilter::MatchContains: {
            // Pattern for MatchContains:
            // " LIKE '%xyz%'"
            fieldToUpdate = " LIKE '%" + filter.value().toString() + "%'" ;
            *error = QContactManager::NoError;
            break;
        }
        case QContactFilter::MatchStartsWith: {
            // Pattern for MatchStartsWith:
            // " LIKE 'xyz%'"
            fieldToUpdate = " LIKE '" +  filter.value().toString() + "%'"  ;
            *error = QContactManager::NoError;
            break;
        }
        case QContactFilter::MatchEndsWith: {
            // Pattern for MatchEndsWith:
            // " LIKE '%xyz'"
            fieldToUpdate = " LIKE '%" + filter.value().toString() + '\'' ;
            *error = QContactManager::NoError;
            break;
        }
        case QContactFilter::MatchFixedString: {
            // Pattern for MatchFixedString:
            // " ='xyz' COLLATE NOCASE"
            fieldToUpdate = " ='"
                       + filter.value().toString() + '\'' + " COLLATE NOCASE";
            *error = QContactManager::NoError;
            break;
        }
        case QContactFilter::MatchCaseSensitive: {
            *error = QContactManager::NotSupportedError;
            break;
        }
        default: {
            *error = QContactManager::NotSupportedError;
            break;
        }
    }
}

void CntFilterDetail::getTableNameWhereClause(const QContactDetailFilter& detailfilter,
                                              QString& tableName,
                                              QString& sqlWhereClause ,
                                              QContactManager::Error* error) const
{
    //Get the table name and the column name
    QString columnName;
    bool isSubType;

    m_dbInfo.getDbTableAndColumnName(detailfilter.detailDefinitionName(), detailfilter.detailFieldName(), tableName, columnName, isSubType);

    // return if tableName is empty
    if (tableName.isEmpty()) {
        *error = QContactManager::NotSupportedError;
        return;
    }

    //check columnName
    if (columnName.isEmpty()) {
        *error = QContactManager::NotSupportedError;
        return;
    }
    else if (isSubType) {
        sqlWhereClause += columnName;
        sqlWhereClause += " NOT NULL ";
    }
    else if (detailfilter.detailDefinitionName() == QContactFavorite::DefinitionName) {
        bool favoritesSearch = true;
        if (detailfilter.value().canConvert(QVariant::Bool)) {
            if (!detailfilter.value().toBool()) {
                //filter to fetch non-favorite contacts
                favoritesSearch = false;    
            }
        }
        sqlWhereClause += columnName;
        if (favoritesSearch) {
            sqlWhereClause += " NOT NULL ";   
        }
        else {
            sqlWhereClause += " IS NULL ";
        }
    }
    else {
        sqlWhereClause += ' ' + columnName + ' ';
        QString fieldToUpdate;
        //Update the value depending on the match flag
        updateForMatchFlag(detailfilter,fieldToUpdate,error);
        sqlWhereClause +=  fieldToUpdate;
    }
}

QList<QContactLocalId>  CntFilterDetail::HandlePredictiveSearchFilter(const QContactFilter& filter,
                                                                      QContactManager::Error* error)
{
    if (filter.type() == QContactFilter::ContactDetailFilter) {
        const QContactDetailFilter detailFilter(filter);
        if (detailFilter.matchFlags() == QContactFilter::MatchKeypadCollation) {
            QString pattern = detailFilter.value().toString();
            //if ( detailFilter.detailFieldName() == QContactEmailAddress::FieldEmailAddress ) {
                return  m_srvConnection.searchOnServer(
                        pattern, CntSymbianSrvConnection::CntPredictiveSearchList, error);
            /*} else {
                QString sqlQuery;
                CntSqlSearch sqlSearch;
                //convert string to numeric format
                sqlQuery = sqlSearch.CreatePredictiveSearch(pattern);
                return  m_srvConnection.searchContacts(sqlQuery, error);
            }*/
        }
    }
    return QList<QContactLocalId>();
}

void CntFilterDetail::createPhoneNumberQuery(
                                      const QContactFilter& filter,
                                      QString& sqlQuery,
                                      QContactManager::Error* error)
{
    QContactDetailFilter detailFilter(filter);
    
    if (detailFilter.detailDefinitionName() != QContactPhoneNumber::DefinitionName) {
        *error = QContactManager::NotSupportedError;
        return;
    }
    
    if (detailFilter.detailFieldName().isEmpty()) {
        fetchAllPhoneNumbers(sqlQuery);
    }
    else if (detailFilter.detailFieldName() == QContactPhoneNumber::FieldNumber) {
        // Matches phonenumbers
        // Phonenumber matching algorithm used
        if (bestMatchingEnabled()) {
            bestMatchPhoneNumberQuery(filter,sqlQuery,error);
        }
        else {
            createMatchPhoneNumberQuery(filter,sqlQuery,error);
        }
    }
    else if (detailFilter.detailFieldName() == QContactPhoneNumber::FieldSubTypes) {
        // Finds all mobile numbers. other subtypes not supported
        createSelectQuery(filter,sqlQuery,error);
    }
    else {
        *error = QContactManager::NotSupportedError; 
    }
}

/*
 * Creates an sql query to fetch contact item IDs for all the contact items
 * which may contain the specified telephone number in a telephone, fax
 * or SMS type field.
 *
 * The comparison method used is not exact.  The number is compared starting from
 * the right side of the number and the method returns an array of candidate
 * matches.  Punctuation (e.g. spaces) and other alphabetic characters are ignored
 * when comparing.
 * 
 * Note that due to the way numbers are stored in the database, it is recommended
 * that at least 7 match digits are specified even when matching a number
 * containing fewer digits.  Failure to follow this guideline may (depending on the
 * database contents) mean that the function will not return the expected Contact
 * IDs.
 */
void CntFilterDetail::createMatchPhoneNumberQuery(
                                      const QContactFilter& filter,
                                      QString& sqlQuery,
                                      QContactManager::Error* error)

{
    if (!filterSupported(filter) ) {
      *error = QContactManager::NotSupportedError;
      return;
    }
          
    QContactDetailFilter detailFilter(filter);
    QString number((detailFilter.value()).toString());
    TPtrC numberPtr(reinterpret_cast<const TUint16*>(number.utf16()));
    
    TInt matchLengthFromRight(KDefaultMatchLength);
    // no need to propagate error, we can use the default match length
    TRAP_IGNORE(getMatchLengthL(matchLengthFromRight));
    
    TInt numLowerDigits = matchLengthFromRight;
    TInt numUpperDigits = 0;
    
    if (numLowerDigits > KLowerSevenDigits) {
        numLowerDigits = KLowerSevenDigits;
        numUpperDigits = matchLengthFromRight - KLowerSevenDigits;
    }
    else if (numLowerDigits == 0) {
        // best match phonenumbers
        numLowerDigits = KLowerSevenDigits;
    }
    
    TMatch phoneDigits = createPaddedPhoneDigits(
                          numberPtr, numLowerDigits, numUpperDigits, error);
    
    if (*error == QContactManager::NoError) {
        // select fields for contacts that match phone lookup
        //  SELECT contact_id FROM comm_addr
        //      WHERE value = [value string] AND type = [type value];
        //
        QString type =  QString(" type = %1").arg(CntDbInfo::EPhoneNumber);
        QString value =  QString(" value = %1").arg(phoneDigits.iLowerSevenDigits);
        QString whereClause = " WHERE" + value + " AND" + type;
        if (matchLengthFromRight <= KLowerSevenDigits) {
            // Matching 7 or less digits...
            sqlQuery = "SELECT contact_id FROM comm_addr" + whereClause;
        }
        else {
            // Checking the upper digits...
        
            // select fields for contacts that match phone lookup
            //  SELECT contact_id, extra_value FROM comm_addr
            //      WHERE value = [value string] AND type = [type value];
            //
            QString type =  QString(" type = %1").arg(CntDbInfo::EPhoneNumber);
            QString value =  QString(" value = %1").arg(phoneDigits.iLowerSevenDigits);
            QString whereClause = " WHERE" + value + " AND" + type;
            sqlQuery = "SELECT contact_id, extra_value FROM comm_addr" + whereClause;
            
            QList<QPair<QContactLocalId, QString> > contactMatches =  m_srvConnection.searchPhoneNumbers(sqlQuery, error);
            
            // Check if search query was successful
            if (*error != QContactManager::NoError) {
                  return;
                }
            
            QStringList list;
            for (int i=0; i<contactMatches.count(); ++i) {
                // Check the upper digits...
                TInt32 storedUpperDigits(0);
                QString extraValue = contactMatches.at(i).second;
                TPtrC extValString(reinterpret_cast<const TUint16*>(extraValue.utf16()));
                if (TLex(extValString).Val(storedUpperDigits) == KErrNone) {
                
                    const TInt KDigitsToRemove = KMaxPhoneMatchLength - KLowerSevenDigits - phoneDigits.iNumUpperDigits;
                    for (TInt j = 0; j < KDigitsToRemove; ++j) {
                        // repeatedly divide by 10 to lop off the appropriate number of digits from the right
                        storedUpperDigits /= 10;
                    }
                
                    storedUpperDigits = TMatch::padOutPhoneMatchNumber(storedUpperDigits, KDigitsToRemove);
                
                    if (phoneDigits.iUpperDigits == storedUpperDigits) {
                        list.append(QString("%1").arg(contactMatches.at(i).first));
                    }
                }
                else {
                    *error = QContactManager::UnspecifiedError;
                    return;
                }
            }
            // Recreate query to fetch all match ids
            // SELECT DISTINCT contact_id FROM contact WHERE contact_id in (
            //      ..
            // )  
            QString ids = list.join(" ,");
            sqlQuery = "SELECT DISTINCT contact_id FROM contact WHERE contact_id in (";
            sqlQuery += ids;
            sqlQuery += ')';
        }
    }
}

/*
 * Fetch all contacts with phone number sql query
 */
void CntFilterDetail::fetchAllPhoneNumbers( QString& sqlQuery )

{
    QString type =  QString(" type = %1").arg(CntDbInfo::EPhoneNumber);
    QString whereClause = " WHERE" + type;
    sqlQuery = "SELECT DISTINCT contact_id FROM comm_addr" + whereClause;
}

/*
 * Returns an sql query to fetch contact item IDs for all the contact items which may contain
 * the specified telephone number in a telephone, fax or SMS type field.
 *
 * This is improved version of createMatchPhoneNumberQuery method.
 * The number is compared starting from the right side of the number and 
 * the method returns an array of candidate matches.  
 * Punctuation (e.g. spaces) and other alphabetic characters are ignored
 * when comparing. Leading zeros are removed. Digits are compared up to 
 * the length of shorter number.
 */
void CntFilterDetail::bestMatchPhoneNumberQuery(const QContactFilter& filter,
                                                QString& sqlQuery,
                                                QContactManager::Error* error)
    {
    if (!filterSupported(filter) ) {
          *error = QContactManager::NotSupportedError;
          return;
        }
              
    QContactDetailFilter detailFilter(filter);
    QString number((detailFilter.value()).toString());
    TPtrC numberPtr(reinterpret_cast<const TUint16*>(number.utf16()));
    
    const TInt KUpperMaxLength = KMaxPhoneMatchLength - KLowerSevenDigits;
    TMatch phoneDigits = createPaddedPhoneDigits(numberPtr, KLowerSevenDigits, KUpperMaxLength, error);
    
    if (*error == QContactManager::NoError) {
        // select fields for contacts that match phone lookup
        //  SELECT contact_id, extra_value FROM comm_addr
        //      WHERE value = [value string] AND type = [type value];
        //
        QString type =  QString(" type = %1").arg(CntDbInfo::EPhoneNumber);
        QString value =  QString(" value = %1").arg(phoneDigits.iLowerSevenDigits);
        QString whereClause = " WHERE" + value + " AND" + type;
        sqlQuery = "SELECT contact_id, extra_value FROM comm_addr" + whereClause;
        
        QList<QPair<QContactLocalId, QString> > contactMatches =  m_srvConnection.searchPhoneNumbers(sqlQuery, error);
        
        // Check if search query was successful
        if (*error != QContactManager::NoError) {
              return;
            }
        
        QStringList list;
        for (int i=0; i<contactMatches.count(); ++i) {
            // Check the upper digits...
            TInt32 number = phoneDigits.iUpperDigits;
            QString extraValue = contactMatches.at(i).second;
            TPtrC extValString(reinterpret_cast<const TUint16*>(extraValue.utf16()));
            
            TInt32 storedUpperDigits(0);
            if (TLex(extValString).Val(storedUpperDigits) == KErrNone) {
                
                TInt32 stored = storedUpperDigits;
                TBool nonZeroInStoredFound = EFalse;
                TBool nonZeroInNumberFound = EFalse;
                while ((number != 0) && (stored != 0)) {
                    nonZeroInNumberFound |= (number % 10 != 0);
                    nonZeroInStoredFound |= (stored % 10 != 0);
                    if (nonZeroInStoredFound && nonZeroInNumberFound) {
                        break;
                    }
                    number /= 10;
                    stored /= 10;
                }
                
                if ((phoneDigits.iUpperDigits == 0) || (storedUpperDigits == 0) ||
                     (number == stored)) {
                    list.append(QString("%1").arg(contactMatches.at(i).first));
                }
            }
            else {
                *error = QContactManager::UnspecifiedError;
                return;
            }
        }
        // Recreate query to fetch all match ids
        // SELECT DISTINCT contact_id FROM contact WHERE contact_id in (
        //      ..
        // )  
        QString ids = list.join(" ,");
        sqlQuery = "SELECT DISTINCT contact_id FROM contact WHERE contact_id in (";
        sqlQuery += ids;
        sqlQuery += ')';
        }
    }

#ifdef PBK_UNIT_TEST
void CntFilterDetail::emulateBestMatching()
{
    m_emulateBestMatching = true;
}
#endif

/*
 * Best matching number if matchLengthFromRight set to 0
 */
bool CntFilterDetail::bestMatchingEnabled() 
{
#ifdef PBK_UNIT_TEST
    if (m_emulateBestMatching) {
        return true;
    }
#endif
    bool result = false;
    TInt matchLengthFromRight(KDefaultMatchLength);
    TRAP_IGNORE(getMatchLengthL(matchLengthFromRight));
    if (matchLengthFromRight == 0) {
        result = true;
    }
    return result;
}

/*
 * Get the match length setting. Digits to be used in matching (counted from
 * right).
 */
bool CntFilterDetail::getMatchLengthL(TInt& matchLength)
{
#ifdef PBK_UNIT_TEST
    if (m_emulateBestMatching) {
        matchLength = 0;
        return true;
    }
#endif
    //Get number of digits used to match
    bool result = false;
    CRepository* repository = CRepository::NewL(KCRUidTelConfiguration);
    TInt err = repository->Get(KTelMatchDigits, matchLength);
    delete repository;
    result = (err == KErrNone);
    return result;
}

/*
 * Convert the supplied string to a matchable phone number.
 *
 * \param text Descriptor containing phone number.
 * \param lowerMatchlength Number of least significant phone digits to use.
 * \param upperMatchLength Number of most significant phone digits to use.
 * \param error Qt error code.
 * \return The hash code(s) to use when matching the supplied phone number.
 */
CntFilterDetail::TMatch CntFilterDetail::createPaddedPhoneDigits(
                                            const TDesC& number, 
                                            const TInt numLowerDigits, 
                                            const TInt numUpperDigits,
                                            QContactManager::Error* error)
{
    TMatch phoneNumber = createPhoneMatchNumber(
                                            number, numLowerDigits, numUpperDigits, error);
    if (*error == QContactManager::NoError) {
        if (phoneNumber.iNumLowerDigits + phoneNumber.iNumUpperDigits == 0) {
            // No digits, do nothing
        }
        else if (phoneNumber.iNumLowerDigits < KLowerSevenDigits) {
            // Only the lower-digits hash is used, pad out the number to
            // KLowerSevenDigits.
            TInt pad = KLowerSevenDigits - phoneNumber.iNumLowerDigits;
            phoneNumber.iLowerSevenDigits = TMatch::padOutPhoneMatchNumber(phoneNumber.iLowerSevenDigits,pad);
        }
        else if (phoneNumber.iNumUpperDigits < (KMaxPhoneMatchLength - KLowerSevenDigits) ) {
            // The lower-digits hash is full, pad out the upper hash if less than 15
            // digits total.
            TInt pad = KMaxPhoneMatchLength - KLowerSevenDigits - phoneNumber.iNumUpperDigits;
            phoneNumber.iUpperDigits = TMatch::padOutPhoneMatchNumber(phoneNumber.iUpperDigits,pad);
        }
    }
    return phoneNumber;
}

/*
 * Returns the hash code(s) to use when matching the supplied phone number.  If the
 * number supplied has more actual phone digits (i.e. not including spaces) than
 * KLowerSevenDigits, a second hash is generated to hold the remaining most
 * significant phone digits. Removes the non-digit characters.

 * \param text Descriptor containing contacts phone number field.
 * \param lowerMatchlength Number of least significant phone digits to use.
 * \param upperMatchLength Number of most significant phone digits to use.
 * \param error Qt error code.
 * \return The hash code(s) to use when matching the supplied phone number.
 */
CntFilterDetail::TMatch CntFilterDetail::createPhoneMatchNumber(
                                            const TDesC& text, 
                                            TInt lowerMatchLength, 
                                            TInt upperMatchLength,
                                            QContactManager::Error* error)
{
    const TInt KBufLength = KCntMaxTextFieldLength+1;
    TBuf<KBufLength> buf;
    
    if (text.Length() <= KBufLength) {
        buf = text;
    }
    else {
        buf = text.Right(KBufLength);
    }
    TMatch::stripOutNonDigitChars(buf);
    
    TMatch phoneNumber;
    if (buf.Length() == 0) {
        *error = QContactManager::BadArgumentError;
        return phoneNumber;
    }
    
    // Generate a hash for the upper digits only if the phone number string is
    // large enough and more than 7 digits are to be matched.
    TInt phoneNumberlength = buf.Length();
    if ((phoneNumberlength > KLowerSevenDigits) && (upperMatchLength > 0)) {
        TPtrC upperPart = buf.Left(phoneNumberlength - KLowerSevenDigits);
        phoneNumber.iUpperDigits = TMatch::createHash(upperPart,
            upperMatchLength, phoneNumber.iNumUpperDigits);
    }
    // Generate a hash of the lower digits.
    phoneNumber.iLowerSevenDigits = TMatch::createHash(buf, 
            lowerMatchLength, phoneNumber.iNumLowerDigits);
    
    return phoneNumber;
}

//CntFilterDetail::TMatch constructor.
CntFilterDetail::TMatch::TMatch()
    :
    iLowerSevenDigits(0),
    iUpperDigits(0),
    iNumLowerDigits(0),
    iNumUpperDigits(0)
{
}

/*
 * Generates a hash value by reversing the matchLength least significant digits,
 * ignoring non-digits and zeroes at the end of the number.  Returns error if no phone
 * digits are supplied.

 * \param phoneNumberString A descriptor containing a phone number.
 * \param matchLength The number of digits from the right of the phone number to use.
 * \param numPhoneDigits The number of digits found in the phone number string.
 * \param error Qt error code.*
 * \return An integer representation of the phone number string in reverse.
 */
TInt32 CntFilterDetail::TMatch::createHash(
                                    const TDesC& phoneNumberString, 
                                    TInt matchLength, 
                                    TInt& numPhoneDigits)
{
    TInt phoneNumberLength = phoneNumberString.Length();
    TInt startIndex = 0;
    if (phoneNumberLength > matchLength) {
        startIndex = phoneNumberLength - matchLength;
    }
    
    numPhoneDigits = 0;
    TUint reversedDigits = 0;
    TInt mult = 1;
    
    for (TInt chrIndex = startIndex; (numPhoneDigits < matchLength) && (chrIndex < phoneNumberLength); chrIndex++) {
        TChar chr = phoneNumberString[chrIndex];
        if (chr.IsDigit()) {
            reversedDigits += (chr.GetNumericValue()) * mult;
            mult = mult * 10;
            ++numPhoneDigits;
        }
    }
    return reversedDigits ;
}

void CntFilterDetail::TMatch::stripOutNonDigitChars(TDes& text)
{
    for (TInt chrPos = 0; chrPos < text.Length(); ++chrPos) {
        TChar chr = text[chrPos];
        if (!chr.IsDigit()) {
            text.Delete(chrPos, 1);
            --chrPos;
        }
    }
}

TInt32 CntFilterDetail::TMatch::padOutPhoneMatchNumber(TInt32& phoneNumber,
                                                       TInt padOutLength)
{
    TInt32 result(phoneNumber);
    const TInt KMult(10);
    for (TInt i = 0; i < padOutLength; ++i) {
        result *= KMult;
    }
    phoneNumber = result;
    return result;
}
