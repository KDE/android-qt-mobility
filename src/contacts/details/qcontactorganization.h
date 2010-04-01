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

#ifndef QCONTACTORGANIZATION_H
#define QCONTACTORGANIZATION_H

#include <QString>
#include <QUrl>

#include "qtcontactsglobal.h"
#include "qcontactdetail.h"
#include "qcontact.h"

QTM_BEGIN_NAMESPACE

/* Leaf class */
class Q_CONTACTS_EXPORT QContactOrganization : public QContactDetail
{
public:
#ifdef Q_QDOC
    static const QLatin1Constant DefinitionName;
    static const QLatin1Constant FieldName;
    static const QLatin1Constant FieldLogoUrl;
    static const QLatin1Constant FieldDepartment;
    static const QLatin1Constant FieldLocation;
    static const QLatin1Constant FieldRole;
    static const QLatin1Constant FieldTitle;
    static const QLatin1Constant FieldAssistantName;

    static const QLatin1Constant FieldLogo; // deprecated
#else
    Q_DECLARE_CUSTOM_CONTACT_DETAIL(QContactOrganization, "Organization")
    Q_DECLARE_LATIN1_CONSTANT(FieldName, "Name");
    Q_DECLARE_LATIN1_CONSTANT(FieldLogoUrl, "LogoUrl");
    Q_DECLARE_LATIN1_CONSTANT(FieldDepartment, "Department");
    Q_DECLARE_LATIN1_CONSTANT(FieldLocation, "Location");
    Q_DECLARE_LATIN1_CONSTANT(FieldRole, "Role");
    Q_DECLARE_LATIN1_CONSTANT(FieldTitle, "Title");
    Q_DECLARE_LATIN1_CONSTANT(FieldAssistantName, "AssistantName");

    // deprecated keys:
    Q_DECLARE_LATIN1_CONSTANT(FieldLogo, "Logo");

#endif

    void setName(const QString& name) {setValue(FieldName, name);}
    QString name() const {return value(FieldName);}
    void setLogoUrl(const QUrl& logo) {setValue(FieldLogoUrl, logo);}
    QUrl logoUrl() const {return value(FieldLogoUrl);}
    void setDepartment(const QStringList& department) {setValue(FieldDepartment, department);}
    QStringList department() const {return value<QStringList>(FieldDepartment);}
    void setLocation(const QString& location) {setValue(FieldLocation, location);}
    QString location() const {return value(FieldLocation);}
    void setRole(const QString& role) {setValue(FieldRole, role);}
    QString role() const {return value(FieldRole);}
    void setTitle(const QString& title) {setValue(FieldTitle, title);}
    QString title() const {return value(FieldTitle);}
    void setAssistantName(const QString& assistantName) {setValue(FieldAssistantName, assistantName);}
    QString assistantName() const {return value(FieldAssistantName);} 

    // old, deprecated API: to be removed after the transition period has elapsed.
    void setLogo(const QString& logo) {setValue(FieldLogo, logo);}
    QString logo() const {return value(FieldLogo);}
};

QTM_END_NAMESPACE

#endif
