/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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
//#ifdef SYMBIAN_BACKEND_USE_SQLITE

#include <QObject>
#include <QString>

// If this macro is defined, searches like "102" are made on one table only
#define SEARCH_FROM_ONE_TABLE

// If defined, De Morgan's laws are used to replace ORs with ANDs in the
// SELECT statement.
#define USE_DEMORGAN

class CQwertyKeyMap;
class C12keyKeyMap;

class CntSqlSearch// : public QObject
{
    //Q_OBJECT

public:

enum SearchMethod { Undefinded, ZerosEndOfFirstToken, ZeroIsFirstNumber };

enum QueryType
        {
        TwelveTable = 0,
        QwertyEmail,
        QwertyContact
        };

public:
    CntSqlSearch();
    
    ~CntSqlSearch();

    QString CreatePredictiveSearch(const QString &pattern);
    

private:
    
    QString selectQweryTable(const QString &pattern) const;
    
    QString SelectTable(const QString &pattern) const;
    
    QStringList GetTokens(const QString& pattern) const;
    
    QString CreateQwertyQuery(
            const QString& pattern) const;
    
    QString CreateQuery(const QString& pattern) const;
    
    QString ExactMatchSearch(const QString& pattern) const;
    
    QString ExactMatchSearchQwerty(const QString& pattern) const;
    
    QString CreateSpaceStringSearch(QStringList numbers, const QString &pattern) const;

    QString IntersectionSearch(const QString& pattern,
                               const QStringList& numbers) const;
    QString SearchTokensFromOneTable(const QString& pattern,
								     const QStringList& tokens,
								     QueryType queryType = TwelveTable) const;

    QString IdenticalTokensSearch(const QString& pattern,
                                  const QStringList& numbers) const;

    QString CompareTwoColumns(const QString& lower,
						 	  const QString& upper,
							  const QString& lower2,
							  const QString& upper2) const;
    
    QString CompareTwoQwertyColumns(const QString& lower,
                                    const QString& upper,
                                    const QString& lower2,
                                    const QString& upper2) const;

    QString TwoDifferentTokensSearch(const QString& pattern,
                                     const QStringList& tokens) const;
    
    QString CompareTwoColumnsWithModifiedPattern(const QString& pattern,
                                                const QStringList& tokens) const;
    
    QString ModifiedMatchColumns(const QString& pattern) const;

    QString ExactMatch(const QString& pattern, QString table = "") const;
    
    QString ExactMatchQwerty(const QString& pattern, QString table = "") const;

    QString CreateJoinTableSearch(QString pattern, QStringList numbers) const;

    QString ExactMatchColumns(QStringList numbers) const;

    QString Order(QStringList numbers, QueryType queryType = TwelveTable ) const;

    QString UpperLimit( const QString &pattern ) const;
    
    QString LowerLimit( const QString &pattern ) const;

    QString ChangeStringPadings( const QString &pattern ) const;

    bool TestPattern( const QString &pattern, SearchMethod searchMethod =  Undefinded ) const;
    
    QString Pad( const QString &pattern, char padChar ) const;
    
    bool isQwerty(const QString &pattern);
    
    QStringList qwertyTokens(const QString &pattern) const;
    
private:
    
    CQwertyKeyMap* mQertyKeyMap; 
    C12keyKeyMap* mkeyKeyMap;
    
    friend class UT_CntSqlSearch;
};
//#endif
