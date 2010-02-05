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


#ifndef QCONTACTNAME_H
#define QCONTACTNAME_H

#include <QString>

#include "qtcontactsglobal.h"
#include "qcontactdetail.h"
#include "qcontactfilter.h"

QTM_BEGIN_NAMESPACE

class Q_CONTACTS_EXPORT QContactName : public QContactDetail
{
public:
#ifdef Q_QDOC
    const char* DefinitionName;
    const char* FieldPrefix;
    const char* FieldFirst; // deprecated
    const char* FieldMiddle;// deprecated
    const char* FieldLast;  // deprecated
    const char* FieldFirstName;
    const char* FieldMiddleName;
    const char* FieldLastName;
    const char* FieldSuffix;
    const char* FieldCustomLabel;
#else
    Q_DECLARE_CUSTOM_CONTACT_DETAIL(QContactName, "Name")
    Q_DECLARE_LATIN1_LITERAL(FieldPrefix, "Prefix");
    Q_DECLARE_LATIN1_LITERAL(FieldFirst, "FirstName");  // deprecated
    Q_DECLARE_LATIN1_LITERAL(FieldMiddle, "MiddleName");// deprecated
    Q_DECLARE_LATIN1_LITERAL(FieldLast, "LastName");    // deprecated
    Q_DECLARE_LATIN1_LITERAL(FieldFirstName, "FirstName");
    Q_DECLARE_LATIN1_LITERAL(FieldMiddleName, "MiddleName");
    Q_DECLARE_LATIN1_LITERAL(FieldLastName, "LastName");
    Q_DECLARE_LATIN1_LITERAL(FieldSuffix, "Suffix");
    Q_DECLARE_LATIN1_LITERAL(FieldCustomLabel, "CustomLabel");
#endif

    QString prefix() const {return value(FieldPrefix);}
    QString firstName() const {return value(FieldFirstName);}
    QString middleName() const {return value(FieldMiddleName);}
    QString lastName() const {return value(FieldLastName);}
    QString suffix() const {return value(FieldSuffix);}
    QString customLabel() const{return value(FieldCustomLabel);}

    void setPrefix(const QString& prefix) {setValue(FieldPrefix, prefix);}
    void setFirstName(const QString& firstName) {setValue(FieldFirstName, firstName);}
    void setMiddleName(const QString& middleName) {setValue(FieldMiddleName, middleName);}
    void setLastName(const QString& lastName) {setValue(FieldLastName, lastName);}
    void setSuffix(const QString& suffix) {setValue(FieldSuffix, suffix);}
    void setCustomLabel(const QString& customLabel) {setValue(FieldCustomLabel, customLabel);}

    // deprecated functions, will be removed after transition period according to process.
    QString Q_DECL_DEPRECATED first() const;
    QString Q_DECL_DEPRECATED middle() const;
    QString Q_DECL_DEPRECATED last() const;
    void Q_DECL_DEPRECATED setFirst(const QString& first);
    void Q_DECL_DEPRECATED setMiddle(const QString& middle);
    void Q_DECL_DEPRECATED setLast(const QString& last);

    // Convenience filter
    static QContactFilter match(const QString& name);
    static QContactFilter match(const QString& firstName, const QString& lastName);
};

QTM_END_NAMESPACE

#endif
