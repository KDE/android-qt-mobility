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

#ifndef QNEARFIELDTAGIMPL_H
#define QNEARFIELDTAGIMPL_H

#include "nearfieldtagimplcommon_symbian.h"
#include "debug.h"

QTM_BEGIN_NAMESPACE
class QNearFieldTagType1Symbian;
class QNearFieldTagType2Symbian;
class QNearFieldTagType3Symbian;
class QNearFieldTagType4Symbian;

template<typename TAGTYPE>
struct TagConstValue
{
    enum { MaxResponseSize = 4096, Timeout = 100 * 1000 };
};

template<>
struct TagConstValue<QNearFieldTagType1Symbian>
{
    enum { MaxResponseSize = 131, Timeout = 5 * 1000 };
};

template<>
struct TagConstValue<QNearFieldTagType2Symbian>
{
    enum { MaxResponseSize = 18, Timeout = 5 * 1000 };
};

template<>
struct TagConstValue<QNearFieldTagType3Symbian>
{
    enum { MaxResponseSize = 256, Timeout = 500 * 1000 };
};

template <typename TAGTYPE>
class QNearFieldTagImpl : public QNearFieldTagImplCommon
{
public: // From MNearFieldTargetOperation

    void HandleResponse(const QNearFieldTarget::RequestId &id, const QByteArray &command, const QByteArray &response, bool emitRequestCompleted);
    void HandleResponse(const QNearFieldTarget::RequestId &id, const QVariantList& response, int error);
    QVariant decodeResponse(const QByteArray& command, const QByteArray& response);

    void EmitNdefMessageRead(const QNdefMessage &message);
    void EmitNdefMessagesWritten();
    void EmitError(int error, const QNearFieldTarget::RequestId &id);

public:
    QNearFieldTagImpl(CNearFieldNdefTarget *tag);
};

template<typename TAGTYPE>
QNearFieldTagImpl<TAGTYPE>::QNearFieldTagImpl(CNearFieldNdefTarget *tag) : QNearFieldTagImplCommon(tag)
{
    TRAP_IGNORE(mResponse.CreateL(TagConstValue<TAGTYPE>::MaxResponseSize));
    mTimeout = TagConstValue<TAGTYPE>::Timeout;
}

template<typename TAGTYPE>
void QNearFieldTagImpl<TAGTYPE>::HandleResponse(const QNearFieldTarget::RequestId &id, const QByteArray &command, const QByteArray &response, bool emitRequestCompleted)
{
    BEGIN
    TAGTYPE * tag = static_cast<TAGTYPE *>(this);
    tag->handleTagOperationResponse(id, command, response, emitRequestCompleted);
    END
}

template<typename TAGTYPE>
void QNearFieldTagImpl<TAGTYPE>::HandleResponse(const QNearFieldTarget::RequestId &id, const QVariantList& response, int error)
{
    BEGIN
    TAGTYPE * tag = static_cast<TAGTYPE *>(this);
    tag->setResponseForRequest(id, response, (error == KErrNone));
    END
}

template<typename TAGTYPE>
QVariant QNearFieldTagImpl<TAGTYPE>::decodeResponse(const QByteArray& command, const QByteArray& response)
{
    BEGIN
    TAGTYPE * tag = static_cast<TAGTYPE *>(this);
    END
    return tag->decodeResponse(command, response);
}

template<typename TAGTYPE>
void QNearFieldTagImpl<TAGTYPE>::EmitNdefMessageRead(const QNdefMessage &message)
{
    BEGIN
    TAGTYPE * tag = static_cast<TAGTYPE *>(this);
    int err;
    QT_TRYCATCH_ERROR(err, emit tag->ndefMessageRead(message));
    Q_UNUSED(err);
    END
}

template<typename TAGTYPE>
void QNearFieldTagImpl<TAGTYPE>::EmitNdefMessagesWritten()
{
    BEGIN
    TAGTYPE * tag = static_cast<TAGTYPE *>(this);
    int err;
    QT_TRYCATCH_ERROR(err, emit tag->ndefMessagesWritten());
    Q_UNUSED(err);
    END
}

template<typename TAGTYPE>
void QNearFieldTagImpl<TAGTYPE>::EmitError(int error, const QNearFieldTarget::RequestId &id)
{
    BEGIN
    TAGTYPE * tag = static_cast<TAGTYPE *>(this);
    int err;
    QT_TRYCATCH_ERROR(err, emit tag->error(SymbianError2QtError(error), id));
    Q_UNUSED(err);
    END
}

QTM_END_NAMESPACE
#endif // QNEARFIELDTAGIMPL_H
