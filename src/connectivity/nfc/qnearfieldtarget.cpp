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

#include "qnearfieldtarget.h"
#include "qnearfieldtarget_p.h"
#include "qndefmessage.h"

#include <QtCore/QString>
#include <QtCore/QUrl>

QTM_BEGIN_NAMESPACE

/*!
    \class QNearFieldTarget
    \brief The QNearFieldTarget class provides an interface for communicating with a target
           device.

    \ingroup connectivity-nfc
    \inmodule QtConnectivity

    QNearFieldTarget provides a generic interface for communicating with an NFC target device.
    Both NFC Forum devices and NFC Forum Tag targets are supported by this class.  All target
    specific classes subclass this class.

    The type() function can be used to get the type of the target device.  The uid() function
    returns the unique identifier of the target.  The AccessMethods flags returns from the
    accessMethods() function can be tested to determine which access methods are supported by the
    target.

    If the target supports NdefAccess, hasNdefMessage() can be called to test if the target has a
    stored NDEF message, ndefMessage() and setNdefMessage() functions can be used to get and set
    the NDEF message.

    If the target supports ApduAccess, sendApduCommand() can be used to send a single APDU command
    to the target and retrieve the response.  sendApduCommands() can be used to send
    multiple APDU commands to the target and retrieve all of the responses.

    If the target supports TagTypeSpecificAccess, sendCommand() can be used to send a single
    proprietary command to the target and retrieve the response.  sendCommands() can be used to
    send multiple proprietary commands to the target and retrieve all of the responses.

    If the target supports LlcpAccess, the QLlcpSocket class can be used to connected to a
    service provided by the target.
*/

/*!
    \enum QNearFieldTarget::Type

    This enum describes the type of tag the target is detected as.

    \value AnyTarget        This value is only used when registering handlers to indicate that any
                            compatible target can be used.
    \value ProprietaryTag   An unidentified proprietary target tag.
    \value NfcTagType1      An NFC tag type 1 target.
    \value NfcTagType2      An NFC tag type 2 target.
    \value NfcTagType3      An NFC tag type 3 target.
    \value NfcTagType4      An NFC tag type 4 target.
    \value MifareTag        A Mifare target.
*/

/*!
    \enum QNearFieldTarget::AccessMethod

    This enum describes the access methods a near field target supports.

    \value NdefAccess               The target supports NDEF records using ndefMessage() and
                                    setNdefMessage().
    \value ApduAccess               The target supports APDU access using sendApduCommand() and
                                    sendApduCommands().
    \value TagTypeSpecificAccess    The target supports sending tag type specific commands using
                                    sendCommand() and sendCommands().
    \value LlcpAccess               The target supports peer-to-peer LLCP communication.
*/

/*!
    Constructs a new near field target.
*/
QNearFieldTarget::QNearFieldTarget()
:   d(new QNearFieldTargetPrivate)
{
}

/*!
    Returns the UID of the near field target.
*/
QByteArray QNearFieldTarget::uid() const
{
    return d->uid;
}

/*!
    Returns the URL of the near field target.
*/
QUrl QNearFieldTarget::url() const
{
    if (!(d->accessMethods & QNearFieldTarget::NdefAccess))
        return QUrl();

    return d->url;
}

/*!
    Returns the type of tag type of this near field target.
*/
QNearFieldTarget::Type QNearFieldTarget::type() const
{
    return d->tagType;
}

/*!
    Returns the access methods support by this near field target.
*/
QNearFieldTarget::AccessMethods QNearFieldTarget::accessMethods() const
{
    return d->accessMethods;
}

/*!
    Returns true if an NDEF message is stored on the near field target; otherwise returns false.
*/
bool QNearFieldTarget::hasNdefMessage() const
{
    return false;
}

/*!
    Returns the NDEF message stored on the near field target.
*/
QNdefMessage QNearFieldTarget::ndefMessage() const
{
    return QNdefMessage();
}

/*!
    Sets the NDEF message on the near field target to \a message.
*/
void QNearFieldTarget::setNdefMessage(const QNdefMessage &message)
{
    Q_UNUSED(message);
}

/*!
    Sends the APDU \a command to the near field target and returns the result.
*/
QByteArray QNearFieldTarget::sendApduCommand(const QByteArray &command)
{
    Q_UNUSED(command);

    return QByteArray();
}

/*!
    Sends multiple APDU \a commands to the near field target and returns the results.
*/
QList<QByteArray> QNearFieldTarget::sendApduCommands(const QList<QByteArray> &commands)
{
    Q_UNUSED(commands);

    return QList<QByteArray>();
}

/*!
    Sends \a command to the near field target and returns the result.
*/
QByteArray QNearFieldTarget::sendCommand(const QByteArray &command)
{
    Q_UNUSED(command);

    return QByteArray();
}

/*!
    Sends multiple \a commands to the near field target and returns the result.
*/
QList<QByteArray> QNearFieldTarget::sendCommands(const QList<QByteArray> &commands)
{
    Q_UNUSED(commands);

    return QList<QByteArray>();
}

QTM_END_NAMESPACE
