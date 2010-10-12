/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef QDECLARATIVECONTACTNAME_H
#define QDECLARATIVECONTACTNAME_H

#include "qdeclarativecontactdetail_p.h"
#include "qcontactname.h"

class QDeclarativeContactName : public QDeclarativeContactDetail
{
    Q_OBJECT
    Q_PROPERTY(QString prefix READ prefix WRITE setPrefix NOTIFY fieldsChanged)
    Q_PROPERTY(QString firstName READ firstName WRITE setFirstName NOTIFY fieldsChanged)
    Q_PROPERTY(QString middleName READ middleName WRITE setMiddleName NOTIFY fieldsChanged)
    Q_PROPERTY(QString lastName READ lastName WRITE setLastName NOTIFY fieldsChanged)
    Q_PROPERTY(QString suffix READ suffix WRITE setSuffix NOTIFY fieldsChanged)
    Q_PROPERTY(QString customLabel READ customLabel WRITE setCustomLabel NOTIFY fieldsChanged)

    Q_ENUMS(FieldType)
    Q_CLASSINFO("DefaultProperty", "firstName")
public:
    enum FieldType {
        FieldPrefix = 0,
        FieldFirstName,
        FieldMiddleName,
        FieldLastName,
        FieldSuffix,
        FieldCustomLabel
    };

    QDeclarativeContactName(QObject* parent = 0)
        :QDeclarativeContactDetail(parent)
    {
        setDetail(QContactName());
    }
    ContactDetailType detailType() const
    {
        return QDeclarativeContactDetail::Name;
    }
    static QString fieldNameFromFieldType(int fieldType)
    {
        switch (fieldType) {
        case FieldPrefix:
            return QContactName::FieldPrefix;
        case FieldFirstName:
            return QContactName::FieldFirstName;
        case FieldMiddleName:
            return QContactName::FieldMiddleName;
        case FieldLastName:
            return QContactName::FieldLastName;
        case FieldSuffix:
            return QContactName::FieldSuffix;
        case FieldCustomLabel:
            return QContactName::FieldCustomLabel;
        default:
            break;
        }
        return "";
    }
    QString prefix() const {return detail().value(QContactName::FieldPrefix);}
    QString firstName() const {return detail().value(QContactName::FieldFirstName);}
    QString middleName() const {return detail().value(QContactName::FieldMiddleName);}
    QString lastName() const {return detail().value(QContactName::FieldLastName);}
    QString suffix() const {return detail().value(QContactName::FieldSuffix);}
    QString customLabel() const{return detail().value(QContactName::FieldCustomLabel);}

    void setPrefix(const QString& prefix) {detail().setValue(QContactName::FieldPrefix, prefix);}
    void setFirstName(const QString& firstName) {detail().setValue(QContactName::FieldFirstName, firstName);}
    void setMiddleName(const QString& middleName) {detail().setValue(QContactName::FieldMiddleName, middleName);}
    void setLastName(const QString& lastName) {detail().setValue(QContactName::FieldLastName, lastName);}
    void setSuffix(const QString& suffix) {detail().setValue(QContactName::FieldSuffix, suffix);}
    void setCustomLabel(const QString& customLabel) {detail().setValue(QContactName::FieldCustomLabel, customLabel);}

signals:
    void fieldsChanged();
};

QML_DECLARE_TYPE(QDeclarativeContactName)
#endif
