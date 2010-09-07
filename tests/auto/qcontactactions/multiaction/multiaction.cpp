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

#ifndef ACTIONFACTORYPLUGINTARGET
#define ACTIONFACTORYPLUGINTARGET contacts_multiactionfactory
#endif
#ifndef ACTIONFACTORYPLUGINNAME
#define ACTIONFACTORYPLUGINNAME MultiActionFactory
#endif

#include "multiaction_p.h"

#include "qcontactphonenumber.h"
#include "qcontactemailaddress.h"
#include "qcontactfilters.h"

#include <QMessageBox>
#include <QTimer>

//! [Example Contact Action Plugin Implementation]

QObject* QContactMultiActionPlugin::createInstance(const QServiceInterfaceDescriptor& descriptor,
                        QServiceContext* context,
                        QAbstractSecuritySession* session)
{
    Q_UNUSED(context);
    Q_UNUSED(session);

    if (descriptor.interfaceName() == QContactActionFactory::InterfaceName
            && descriptor.serviceName() == QString(QLatin1String("tst_qcontactactions:multiaction"))
            && descriptor.majorVersion() == 1
            && descriptor.minorVersion() == 1
            && descriptor.customAttribute("ActionName") == QString(QLatin1String("call"))) {
        return new QContactMultiActionFactory;
    } else {
        return 0;
    }
}

Q_EXPORT_PLUGIN2(contacts_multiaction, QContactMultiActionPlugin);


QContactMultiActionFactory::QContactMultiActionFactory()
    : QContactActionFactory()
{
    m_actionOneDescriptor = createDescriptor("call", "tst_qcontactactions:multiaction", "sip", 1);
    m_actionTwoDescriptor = createDescriptor("call", "tst_qcontactactions:multiaction", "prop", 1);
}

QContactMultiActionFactory::~QContactMultiActionFactory()
{
}

QList<QContactActionDescriptor> QContactMultiActionFactory::actionDescriptors() const
{
    QList<QContactActionDescriptor> retn;
    retn << m_actionOneDescriptor << m_actionTwoDescriptor;
    return retn;
}

QContactAction* QContactMultiActionFactory::create(const QContactActionDescriptor& which) const
{
    if (which == m_actionOneDescriptor)
        return new QContactActionOne;
    else if (which == m_actionTwoDescriptor)
        return new QContactActionTwo;
    else
        return 0;
}

QSet<QContactActionTarget> QContactMultiActionFactory::supportedTargets(const QContact& contact, const QContactActionDescriptor& which) const
{
    QSet<QContactActionTarget> retn;
    if (which == m_actionOneDescriptor || which == m_actionTwoDescriptor) {
        // in this example, they support the same targets.
        QList<QContactPhoneNumber> pndets = contact.details<QContactPhoneNumber>();
        for (int i = 0; i < pndets.size(); ++i) {
            QContactActionTarget curr;
            curr.setContact(contact);
            curr.setDetails(QList<QContactDetail>() << pndets.at(i));
            retn << curr;
        }
    }

    return retn;
}

QContactFilter QContactMultiActionFactory::contactFilter(const QContactActionDescriptor& which) const
{
    if (which == m_actionOneDescriptor || which == m_actionTwoDescriptor) {
        QContactDetailFilter retn;
        retn.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
        return retn;
    }

    return QContactFilter();
}

QVariant QContactMultiActionFactory::metaData(const QString& key, const QList<QContactActionTarget>& targets, const QVariantMap& parameters, const QContactActionDescriptor& which) const
{
    Q_UNUSED(targets)
    Q_UNUSED(parameters)

    if (key == QContactActionDescriptor::MetaDataLabel)
        return QString("Call with VoIP");
    // Label etc

    if (key == QLatin1String("Provider")) {// our custom metadata - just return which.actionIdentifier
        return which.actionIdentifier();
    }

    return QVariant();
}

bool QContactMultiActionFactory::supportsContact(const QContact& contact, const QContactActionDescriptor& which) const
{
    if (which == m_actionOneDescriptor || which == m_actionTwoDescriptor)
        return !contact.details<QContactPhoneNumber>().isEmpty();
    return false;
}


QContactActionOne::QContactActionOne()
{

}

QContactActionOne::~QContactActionOne()
{

}

bool QContactActionOne::invokeAction(const QContactActionTarget& target, const QVariantMap& params)
{
    Q_UNUSED(params)
    // this action only works on (contact + phone number) targets.
    if (target.details().size() > 1 || target.details().at(0).definitionName() != QContactPhoneNumber::DefinitionName)
        return false;

    QTimer::singleShot(1, this, SLOT(performAction()));
    return true;
}

bool QContactActionOne::invokeAction(const QList<QContactActionTarget>& targets, const QVariantMap& params)
{
    Q_UNUSED(params)
    foreach (const QContactActionTarget& target, targets) {
        if (target.details().size() > 1 || target.details().at(0).definitionName() != QContactPhoneNumber::DefinitionName) {
            return false;
        }
    }

    QTimer::singleShot(1, this, SLOT(performAction()));
    return true;
}

QVariantMap QContactActionOne::results() const
{
    return QVariantMap();
}

QContactAction::State QContactActionOne::state() const
{
    return QContactAction::FinishedState;
}

void QContactActionOne::performAction()
{
    QMessageBox::information(0, "ActionOne", "This is action one!");
    emit stateChanged(QContactAction::FinishedState);
}

QContactActionTwo::QContactActionTwo()
{

}

QContactActionTwo::~QContactActionTwo()
{

}

bool QContactActionTwo::invokeAction(const QContactActionTarget& target, const QVariantMap& params)
{
    Q_UNUSED(params)
    // this action only works on (contact + phone number) targets.  Note that it doesn't
    // have to be the same as QContactActionOne -- it could have an entirely different implementation!
    if (target.details().size() > 1 || target.details().at(0).definitionName() != QContactPhoneNumber::DefinitionName)
        return false;

    QTimer::singleShot(1, this, SLOT(performAction()));
    return true;
}

bool QContactActionTwo::invokeAction(const QList<QContactActionTarget>& targets, const QVariantMap& params)
{
    Q_UNUSED(params)
    foreach (const QContactActionTarget& target, targets) {
        if (target.details().size() > 1 || target.details().at(0).definitionName() != QContactPhoneNumber::DefinitionName) {
            return false;
        }
    }

    QTimer::singleShot(1, this, SLOT(performAction()));
    return true;
}

QVariantMap QContactActionTwo::results() const
{
    return QVariantMap();
}

QContactAction::State QContactActionTwo::state() const
{
    return QContactAction::FinishedState;
}

void QContactActionTwo::performAction()
{
    QMessageBox::information(0, "ActionTwo", "This is action two!");
    emit stateChanged(QContactAction::FinishedState);
}

//! [Example Contact Action Plugin Implementation]
