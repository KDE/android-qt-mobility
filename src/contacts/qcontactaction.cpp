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

#include "qcontactaction.h"
#include "qcontactmanager_p.h"
#include "qcontactactiondescriptor.h"

#include <QSet>
#include <QString>

QTM_BEGIN_NAMESPACE

QContactAction::~QContactAction()
{
}

/*!
  \class QContactAction
  \brief The QContactAction class provides an interface for performing an
  action on a QContact or QContactDetail.
  \ingroup contacts-main

  An action is anything that can be performed on a contact, or a detail of a contact.  An example
  of an action might be "Send Email" or "Dial" or "Plot Navigation Route".  One action may be
  implemented by multiple vendors, and indeed one vendor may provide multiple implementations of
  the same action.  The name of an action identifies its semantics, while its implementation version
  distinguishes it from other implementations of the action by the same vendor.

  Invocation of an action is asynchronous; at some stage after calling \l invokeAction() the
  action instance will emit the \l progress() signal.  Any results of the action may be retrieved
  by calling \l result().

  Each instance of a QContactAction is created by a \l QContactActionFactory when
  \l QContactActionFactory::instance() is called; the caller takes ownership of the action instance.
 
  \sa QContactActionFactory, QContactActionFilter
 */

/*!
 * \fn QContactAction::~QContactAction()
 * Clears any memory in use by this instance of the action implementation
 */

/*!
 * \fn QContactAction::actionDescriptor() const
 * Returns the descriptor which uniquely identifies this action implementation.  A descriptor
 * consists of an action name, a vendor name and an implementation version.
 * The name of the action identifies the action provided; different implementations of an action
 * with the same name must provide the same functionality, but may differ in implementation semantics.
 * Hence, the action name includes the major version of the interface definition implemented.
 * The vendor name is the identification string of the vendor which has provided this implementation.
 * The implementation version is the (minor) version of the implementation, and is vendor-specific.
 *
 * \sa QContactActionDescriptor
 */

/*!
 * \fn QContactAction::metadata() const
 * Returns the metadata associated with this action, such as icons, labels or sound cues
 */

/*!
 * \fn QContactAction::contactFilter(const QVariant& value) const
 * Returns a filter which may be used to filter contacts by the availability of this action implementation for them.
 * If \a value is valid, only contacts which have a detail with the given value and for which the action is available are returned
 */

/*!
 * \fn QContactAction::supportsDetail(const QContactDetail& detail) const
 * Returns true if the provided \a detail contains the fields required for this action to be
 * performed on it; otherwise, returns false
 */

/*!
 * \fn QContactAction::supportedDetails(const QContact& contact) const
 * Returns a list of the details saved in the given \a contact which contain the fields required
 * for this action to be performed on them.
 *
 * The default implementation of this function simply tests all the details in the contact
 * using \l supportsDetail()
 */
QList<QContactDetail> QContactAction::supportedDetails(const QContact& contact) const
{
    QList<QContactDetail> ret;
    QList<QContactDetail> details = contact.details();
    for (int j=0; j < details.count(); j++) {
        if (supportsDetail(details.at(j)))
            ret.append(details.at(j));
    }
    return ret;
}

/*!
 * \fn QContactAction::invokeAction(const QContact& contact, const QContactDetail& detail = QContactDetail())
 * Initiates the implemented action on the specified \a detail of the given \a contact, or on the first
 * eligible detail saved in the contact if the given \a detail is empty.
 * At some point after invocation, one or more \l progress() signals will be emitted by the action instance.
 * The results of the action (if any) may be retrieved by calling \l result().
 *
 * \sa result(), progress()
 */

/*!
 * \fn QContactAction::result() const
 * Returns the result of the action, if any exists.  Calling this function prior to receiving the \l progress()
 * signal will not return a meaningful result.
 */

/*!
 * \enum QContactAction::Status
 * Describes the current status of the asynchronous action operation
 * \value Inactive The operation has not yet been initiated
 * \value Autonomous The operation was initiated but no further information is or will be available
 * \value Active The operation was initiated and is not yet finished
 * \value Finished The operation successfully completed
 * \value FinishedWithError The operation has finished, but an error occurred
 */

/*!
 * \fn QContactAction::progress(QContactAction::Status status, const QVariantMap& result)
 * This signal is emitted by an action instance whose functionality has been initiated with \l invokeAction().
 * It provides clients with the current \a status of the action, and any \a result associated with the action.
 * This signal must be emitted at least once by every action instance after \l invokeAction() is called.
 *
 * If the action implementation is incapable of reporting the status of the operation (for example, the
 * action is implemented via a one-way IPC call) it should emit the progress signal with \a status
 * set to \c QContactAction::Autonomous.
 */

/*!
 * Returns a list of identifiers of the available actions which are provided by the given \a vendor and of the given \a implementationVersion.
 * If \a vendor is empty, actions from all vendors and of any implementation version are returned; if \a implementationVersion is empty,
 * any actions from the given \a vendor (regardless of implementation version) are returned.
 */
QStringList QContactAction::availableActions(const QString& vendor, int implementationVersion)
{
    // SLOW naive implementation...
    QSet<QString> ret;
    QContactManagerData::loadFactories();
    QList<QContactActionDescriptor> actionDescriptors = QContactManagerData::actionDescriptors(QString(), vendor, implementationVersion);
    for (int i = 0; i < actionDescriptors.size(); i++) {
        QContactActionDescriptor descriptor = actionDescriptors.at(i);
        ret.insert(descriptor.actionName());
    }

    return ret.toList();
}

/*!
 * Returns a list of QContactActionDescriptor instances which identified implementations of the given \a actionName which are provided by the
 * given \a vendorName and are of the given \a implementationVersion.  If \a actionName is empty, descriptors for
 * implementations of all actions are returned; if \a vendorName is empty, descriptors for implementations provided by any vendor and
 * of any implementation version are returned; if \a implementationVersion is empty, descriptors for any implementations provided by the
 * given \a vendorName of the given \a actionName are returned.
 */
QList<QContactActionDescriptor> QContactAction::actionDescriptors(const QString& actionName, const QString& vendorName, int implementationVersion)
{
    QContactManagerData::loadFactories();
    return QContactManagerData::actionDescriptors(actionName, vendorName, implementationVersion);
}

/*!
 * Returns a pointer to a new instance of the action implementation identified by the given \a descriptor.
 * The caller takes ownership of the action implementation and must delete it to avoid leaking memory.
 */
QContactAction* QContactAction::action(const QContactActionDescriptor& descriptor)
{
    QContactManagerData::loadFactories();
    return QContactManagerData::action(descriptor);
}

#include "moc_qcontactaction.cpp"

QTM_END_NAMESPACE
