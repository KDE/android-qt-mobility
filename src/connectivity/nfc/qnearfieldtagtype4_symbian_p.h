/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QNEARFIELDTAGTYPE4SYMBIAN_H
#define QNEARFIELDTAGTYPE4SYMBIAN_H

#include <qnearfieldtagtype4.h>
#include "symbian/nearfieldndeftarget_symbian.h"
#include "symbian/nearfieldtag_symbian.h"
#include "symbian/nearfieldtagimpl_symbian.h"
#include "symbian/debug.h"

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE



class QNearFieldTagType4Symbian : public QNearFieldTagType4, private QNearFieldTagImpl<QNearFieldTagType4Symbian>
{
    Q_OBJECT

public:

    explicit QNearFieldTagType4Symbian(CNearFieldNdefTarget *tag, QObject *parent = 0);
    
    ~QNearFieldTagType4Symbian();

    virtual QByteArray uid() const;
    Type type() const { return NfcTagType4; }

    bool hasNdefMessage();
    void readNdefMessages();
    void writeNdefMessages(const QList<QNdefMessage> &messages);

    RequestId sendCommand(const QByteArray &command);
    RequestId sendCommands(const QList<QByteArray> &commands);
    
    RequestId select(const QByteArray &name);
    RequestId select(quint16 fileIdentifier);
    
    RequestId read(quint16 length = 0, quint16 startOffset = 0);
    RequestId write(const QByteArray &data, quint16 startOffset = 0);
    bool isProcessingCommand() const { return _isProcessingRequest(); }
    bool waitForRequestCompleted(const RequestId &id, int msecs = 5000);

    void setAccessMethods(const QNearFieldTarget::AccessMethods& accessMethods)
    {
        _setAccessMethods(accessMethods);
    }

    QNearFieldTarget::AccessMethods accessMethods() const
    {
        return _accessMethods();
    }

    void handleTagOperationResponse(const RequestId &id, const QByteArray &command, const QByteArray &response);
    QVariant decodeResponse(const QByteArray &command, const QByteArray &response);
    friend class QNearFieldTagImpl<QNearFieldTagType4Symbian>;
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif // QNEARFIELDTAGTYPE4SYMBIAN_H
