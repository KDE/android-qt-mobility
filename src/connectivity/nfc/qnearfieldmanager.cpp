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

#include "qnearfieldmanager.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QNearFieldManager
    \brief The QNearFieldManager class provides access to notifications for NFC events.

    \ingroup connectivity-nfc
    \inmodule QtConnectivity

    NFC Forum devices support two modes of communications.  The first mode, peer-to-peer
    communications, is used to communicate between two NFC Forum devices.  The second mode,
    master/slave communications, is used to communicate between an NFC Forum device and an NFC
    Forum Tag or Contactess Card.  The targetDetected() signal is emitted when a target device
    enters communications range.  Communications can be initiated from the slot connected to this
    signal.

    NFC Forum devices generally operate as the master in master/slave communications.  Some devices
    are capable of operate as the slave, so called Card Emulation mode.  In this mode the local NFC
    device emulates a NFC Forum Tag or Contactless Card and can be used to perform transactions.
    The transaction happens entirely within a secure element on the device and only a notification
    of the transaction is provided.  The transactionDetected() signal is emitted whenever a
    transaction occurs.

    NFC Forum Tags can contain one or more messages in a standardized format.  These messages are
    encapsulated in the QNdefMessage class.  Use the registerTargetDetectedHandler() functions to
    register message handlers with particular criteria.  Handlers can be unregistered with the
    unregisterTargetDetectedHandler() function.
*/

/*!
    \fn void QNearFieldManager::targetDetected(const QNearFieldTarget &target)

    This signal is emitted whenever a target is detected. The \a target parameter represents the
    detected target.

    This signal will be emitted for all detected targets.
*/

/*!
    \fn void QNearFieldManager::transactionDetected(const QByteArray &applicationIdentifier)

    This signal is emitted when ever a transaction is performed with the application identified by
    \a applicationIdentifier.

    The \a applicationIdentifier is a byte array of up to 16 bytes as defined by ISO 7816-4 and
    uniquely identifies the application and application vendor that was involved in the
    transaction.
*/

/*!
    \fn int QNearFieldManager::registerTargetDetectedHandler(QNearFieldTarget::Type targetType,
                                                               const QObject *object, const char *slot)

    Registers \a object to receive notifications on \a slot when a tag with a tag type of
    \a targetType has been detected and has an NDEF record that matches template argument.  The
    \a slot method on \a object should have the prototype
    'void targetDetected(const QNdefMessage &message, const QNearFieldTarget &target)'.

    Returns an identifier, which can be used to unregister the handler, on success; otherwise
    returns -1.
*/

/*!
    Constructs a new near field manager with \a parent.
*/
QNearFieldManager::QNearFieldManager(QObject *parent)
:   QObject(parent)
{
}

/*!
    Destroys the near field manager.
*/
QNearFieldManager::~QNearFieldManager()
{
}

/*!
    Registers \a object to receive notifications on \a slot when a tag with a tag type of
    \a targetType has been detected and has an NDEF record that matchings \a typeNameFormat and
    \a type.  The \a slot method on \a object should have the prototype
    'void targetDetected(const QNdefMessage &message, const QNearFieldTarget &target)'.

    Returns an identifier, which can be used to unregister the handler, on success; otherwise
    returns -1.
*/

int QNearFieldManager::registerTargetDetectedHandler(QNearFieldTarget::Type targetType,
                                                     QNdefRecord::TypeNameFormat typeNameFormat,
                                                     const QByteArray &type,
                                                     const QObject *object, const char *slot)
{
    return registerTargetDetectedHandler(targetType, quint8(typeNameFormat), type, object, slot);
}

/*!
    Registers \a object to receive notifications on \a slot when a tag with a tag type of
    \a targetType has been detected and has an NDEF record that matchings \a typeNameFormat and
    \a type.  The \a slot method on \a object should have the prototype
    'void targetDetected(const QNdefMessage &message, const QNearFieldTarget &target)'.

    Returns an identifier, which can be used to unregister the handler, on success; otherwise
    returns -1.
*/
int QNearFieldManager::registerTargetDetectedHandler(QNearFieldTarget::Type targetType,
                                                     quint8 typeNameFormat, const QByteArray &type,
                                                     const QObject *object, const char *slot)
{
    if (!QMetaObject::checkConnectArgs(SIGNAL(targetDetected(QNdefMessage,QNearFieldTarget)),
                                       QMetaObject::normalizedSignature(slot))) {
        qWarning("Signatures do not match: %s:%d\n", __FILE__, __LINE__);
        return -1;
    }

    Q_UNUSED(targetType);
    Q_UNUSED(typeNameFormat);
    Q_UNUSED(type);
    Q_UNUSED(object);

    return -1;
}

/*!
    Registers \a object to receive notifications on \a slot when a tag with a tag type of
    \a targetType has been detected and has an NDEF message that matches \a filter is detected. The
    \a slot method on \a object should have the prototype
    'void targetDetected(const QNdefMessage &message, const QNearFieldTarget &target)'.

    Returns an identifier, which can be used to unregister the handler, on success; otherwise
    returns -1.
*/
int QNearFieldManager::registerTargetDetectedHandler(QNearFieldTarget::Type targetType,
                                                     const QNdefFilter &filter,
                                                     const QObject *object, const char *slot)
{
    if (!QMetaObject::checkConnectArgs(SIGNAL(targetDetected(QNdefMessage,QNearFieldTarget)),
                                       QMetaObject::normalizedSignature(slot))) {
        qWarning("Signatures do not match: %s:%d\n", __FILE__, __LINE__);
        return -1;
    }

    Q_UNUSED(targetType);
    Q_UNUSED(filter);
    Q_UNUSED(object);

    return -1;
}

/*!
    Unregisters the target detect handler identified by \a handlerId.

    Returns true on success; otherwise returns false.
*/
bool QNearFieldManager::unregisterTargetDetectedHandler(int handlerId)
{
    Q_UNUSED(handlerId);

    return false;
}

#include "moc_qnearfieldmanager.cpp"

QTM_END_NAMESPACE
