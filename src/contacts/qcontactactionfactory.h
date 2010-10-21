/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
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
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/


#ifndef QCONTACTACTIONFACTORY_H
#define QCONTACTACTIONFACTORY_H

#include "qtcontactsglobal.h"
#include "qcontactactiondescriptor.h"
#include "qcontactactiontarget.h"

#include <QObject>
#include <QtPlugin>
#include <QString>
#include <QList>
#include <QVariantMap>
#include <QStringList>
#include <QHash>

QTM_BEGIN_NAMESPACE

class QContactAction;
class Q_CONTACTS_EXPORT QContactActionFactory : public QObject
{
    Q_OBJECT

public:
    QContactActionFactory(QObject *parent = 0) : QObject(parent) {}
    virtual ~QContactActionFactory();
    virtual QList<QContactActionDescriptor> actionDescriptors() const = 0;
    virtual QContactAction* create(const QContactActionDescriptor& which) const = 0;

    // Backend functionality for QCAD
    virtual QSet<QContactActionTarget> supportedTargets(const QContact& contact, const QContactActionDescriptor& which) const = 0;
    virtual QContactFilter contactFilter(const QContactActionDescriptor& which) const = 0;
    virtual QVariant metaData(const QString& key, const QList<QContactActionTarget>& targets, const QVariantMap& parameters, const QContactActionDescriptor& which) const = 0;

    virtual bool supportsContact(const QContact& contact, const QContactActionDescriptor& which) const; // virtual but not pure virtual; default impl. calls supportedTargets.isEmpty().

#ifdef Q_QDOC
    static const QLatin1Constant InterfaceName;
#else
    Q_DECLARE_LATIN1_CONSTANT(InterfaceName, "com.nokia.qt.mobility.contacts.action");
#endif

protected:
    QContactActionDescriptor createDescriptor(const QString& actionName, const QString& serviceName, const QString& actionIdentifier, int implementationVersion) const;
};

QTM_END_NAMESPACE

#endif
