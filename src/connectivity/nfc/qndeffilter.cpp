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

#include "qndeffilter.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QNdefFilter
    \brief The QNdefFilter class provides a filter for match NDEF messages.

    \ingroup connectivity-nfc
    \inmodule QtConnectivity

    The QNdefFilter encapsulates the structure of an NDEF message and is used by
    QNearFieldManager::registerTargetDetectHandler() to match NDEF message that have a particular
    structure.

    The following filter matches NDEF messages that contain a single smart poster record:

    \code
    QNdefFilter filter;
    filter.append(QNdefRecord::NfcRtd, "Sp");
    \endcode

    The following filter matches NDEF messages that contain a URI, a localized piece of text and an
    optional JPEG image.  The order of the records must be in the order specified:

    \code
    QNdefFilter filter;
    filter.setOrderMatch(true);
    filter.appendRecord(QNdefRecord::NfcRtd, "U");
    filter.appendRecord<QNdefNfcTextRecord>();
    filter.appendRecord(QNdefRecord::Mime, "image/jpeg", 0, 1);
    \endcode
*/

/*!
    \fn void QNdefFilter::appendRecord(unsigned int min, unsigned int max)

    Appends a record matching the template parameter to the NDEF filter.  The record must occur
    between \a min and \a max times in the NDEF message.
*/

/*!
    Constructs a new NDEF filter.
*/
QNdefFilter::QNdefFilter()
:   m_orderMatching(false)
{
}

/*!
    Destroys the NDEF filter.
*/
QNdefFilter::~QNdefFilter()
{
}

/*!
    Clears the filter.
*/
void QNdefFilter::clear()
{
    m_orderMatching = false;
}

/*!
    Sets the ording requirements of the filter. If \a on is true the filter will only match if the
    order of records in the filter matches the order of the records in the NDEF message. If \a on
    is false the order of the records is not taken into account when matching.

    By default record order is not taken into account.
*/
void QNdefFilter::setOrderMatch(bool on)
{
    m_orderMatching = on;
}

/*!
    Returns true if the filter takes NDEF record order into account when matching; otherwise
    returns false.
*/
bool QNdefFilter::orderMatch() const
{
    return m_orderMatching;
}

/*!
    Appends a record with type name format \a typeNameFormat and type \a type to the NDEF filter.
    The record must occur between \a min and \a max times in the NDEF message.
*/
void QNdefFilter::appendRecord(QNdefRecord::TypeNameFormat typeNameFormat, const QByteArray &type,
                               unsigned int min, unsigned int max)
{
    Q_UNUSED(typeNameFormat);
    Q_UNUSED(type);
    Q_UNUSED(min);
    Q_UNUSED(max);
}

/*!
    Appends a record with type name format \a typeNameFormat and type \a type to the NDEF filter.
    The record must occur between \a min and \a max times in the NDEF message.
*/
void QNdefFilter::appendRecord(quint8 typeNameFormat, const QByteArray &type,
                               unsigned int min, unsigned int max)
{
    Q_UNUSED(typeNameFormat);
    Q_UNUSED(type);
    Q_UNUSED(min);
    Q_UNUSED(max);
}


QTM_END_NAMESPACE
