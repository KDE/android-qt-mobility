/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVECONTACTORGANIZATION_H
#define QDECLARATIVECONTACTORGANIZATION_H

#include "qdeclarativecontactdetail_p.h"
#include "qcontactorganization.h"

class QDeclarativeContactOrganization : public QDeclarativeContactDetail
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY fieldsChanged)
    Q_PROPERTY(QUrl logoUrl READ logoUrl WRITE setLogoUrl NOTIFY fieldsChanged)
    Q_PROPERTY(QStringList department READ department WRITE setDepartment NOTIFY fieldsChanged)
    Q_PROPERTY(QString location READ location WRITE setLocation NOTIFY fieldsChanged)
    Q_PROPERTY(QString role READ role WRITE setRole NOTIFY fieldsChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY fieldsChanged)
    Q_PROPERTY(QString assistantName READ assistantName WRITE setAssistantName NOTIFY fieldsChanged)
    Q_CLASSINFO("DefaultProperty", "name")
    Q_ENUMS(FieldType);
public:
    enum FieldType {
        Name = 0,
        LogoUrl,
        Department,
        Location,
        Role,
        Title,
        AssistantName
    };

    QDeclarativeContactOrganization(QObject* parent = 0)
        :QDeclarativeContactDetail(parent)
    {
        setDetail(QContactOrganization());
    }
    ContactDetailType detailType() const
    {
        return QDeclarativeContactDetail::Organization;
    }

    void setName(const QString& name) {detail().setValue(QContactOrganization::FieldName, name);}
    QString name() const {return detail().value(QContactOrganization::FieldName);}
    void setLogoUrl(const QUrl& logo) {detail().setValue(QContactOrganization::FieldLogoUrl, logo);}
    QUrl logoUrl() const {return detail().value(QContactOrganization::FieldLogoUrl);}
    void setDepartment(const QStringList& department) {detail().setValue(QContactOrganization::FieldDepartment, department);}
    QStringList department() const {return detail().value<QStringList>(QContactOrganization::FieldDepartment);}
    void setLocation(const QString& location) {detail().setValue(QContactOrganization::FieldLocation, location);}
    QString location() const {return detail().value(QContactOrganization::FieldLocation);}
    void setRole(const QString& role) {detail().setValue(QContactOrganization::FieldRole, role);}
    QString role() const {return detail().value(QContactOrganization::FieldRole);}
    void setTitle(const QString& title) {detail().setValue(QContactOrganization::FieldTitle, title);}
    QString title() const {return detail().value(QContactOrganization::FieldTitle);}
    void setAssistantName(const QString& assistantName) {detail().setValue(QContactOrganization::FieldAssistantName, assistantName);}
    QString assistantName() const {return detail().value(QContactOrganization::FieldAssistantName);}
signals:
    void fieldsChanged();

};
QML_DECLARE_TYPE(QDeclarativeContactOrganization)

#endif
