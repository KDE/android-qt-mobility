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

#include <qnearfieldtarget.h>
#include <qnearfieldtarget_p.h>
#include "nearfieldtag_symbian.h"
#include "nearfieldndeftarget_symbian.h"
#include "nearfieldutility_symbian.h"
#include "nearfieldndeftarget_symbian.h"
#include "nearfieldtagasyncrequest_symbian.h"
#include "nearfieldtargetoperation_symbian.h"

#include "nearfieldtagoperationcallback_symbian.h"
#include "nearfieldtagndefoperationcallback_symbian.h"

#include "nearfieldtagndefrequest_symbian.h"
#include "nearfieldtagcommandrequest_symbian.h"
#include "nearfieldtagcommandsrequest_symbian.h"
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
class QNearFieldTagImpl : public MNearFieldTargetOperation
{
public: // From MNearFieldTargetOperation
    bool DoReadNdefMessages(MNearFieldNdefOperationCallback * const aCallback);
    bool DoSetNdefMessages(const QList<QNdefMessage> &messages, MNearFieldNdefOperationCallback * const aCallback);
    bool DoHasNdefMessages();
    bool DoSendCommand(const QByteArray& command, MNearFieldTagOperationCallback * const aCallback, bool deferred = true);
    bool IssueNextRequest(QNearFieldTarget::RequestId aId);
    void RemoveRequestFromQueue(QNearFieldTarget::RequestId aId);
    QNearFieldTarget::RequestId AllocateRequestId();
    void HandleResponse(const QNearFieldTarget::RequestId &id, const QByteArray &command, const QByteArray &response);
    void HandleResponse(const QNearFieldTarget::RequestId &id, const QVariantList& response, int error);
    QVariant decodeResponse(const QByteArray& command, const QByteArray& response);

    void EmitNdefMessageRead(const QNdefMessage &message);
    void EmitNdefMessagesWritten();
    void EmitError(int error, const QNearFieldTarget::RequestId &id);

    void DoCancelSendCommand();
    void DoCancelNdefAccess();

public:
    QNearFieldTagImpl(CNearFieldNdefTarget *tag);
    virtual ~QNearFieldTagImpl();
    bool _hasNdefMessage();
    void _ndefMessages();
    void _setNdefMessages(const QList<QNdefMessage> &messages);

    void _setAccessMethods(const QNearFieldTarget::AccessMethods& accessMethods)
    {
        mAccessMethods = accessMethods;
    }

    QNearFieldTarget::AccessMethods _accessMethods() const
    {
        return mAccessMethods;
    }

    QNearFieldTarget::RequestId _sendCommand(const QByteArray &command);
    QNearFieldTarget::RequestId _sendCommands(const QList<QByteArray> &command);
    bool _waitForRequestCompleted(const QNearFieldTarget::RequestId &id, int msecs = 5000);
    int _waitForRequestCompletedNoSignal(const QNearFieldTarget::RequestId &id, int msecs = 5000);

    QByteArray _uid() const;

    bool _isProcessingRequest() const;

protected:
    QNearFieldTarget::Error SymbianError2QtError(int error);
protected:
    CNearFieldNdefTarget * mTag;
    QNearFieldTarget::AccessMethods mAccessMethods;
    mutable QByteArray mUid;
    RPointerArray<CNdefMessage> mMessageList;
    RBuf8 mResponse;

protected:
    // own each async request in the list
    QList<MNearFieldTagAsyncRequest *> mPendingRequestList;
    MNearFieldTagAsyncRequest * mCurrentRequest;
};

template<typename TAGTYPE>
bool QNearFieldTagImpl<TAGTYPE>::DoReadNdefMessages(MNearFieldNdefOperationCallback * const aCallback)
{
    BEGIN
    int error = KErrGeneral;
    CNearFieldNdefTarget * ndefTarget = mTag->CastToNdefTarget();
    if (ndefTarget)
    {
        LOG("switched to ndef connection");
        ndefTarget->SetNdefOperationCallback(aCallback);
        // TODO: consider re-entry, since signal is emit one by one
        mMessageList.Reset();
        error = ndefTarget->ndefMessages(mMessageList);
        LOG("error code is"<<error);
    }

    if (error != KErrNone)
    {
        aCallback->ReadComplete(error, 0);
    }
    END
    return (error == KErrNone);
}

template<typename TAGTYPE>
bool QNearFieldTagImpl<TAGTYPE>::DoSetNdefMessages(const QList<QNdefMessage> &messages, MNearFieldNdefOperationCallback * const aCallback)
{
    BEGIN
    int error = KErrGeneral;
    CNearFieldNdefTarget * ndefTarget = mTag->CastToNdefTarget();

    if (ndefTarget)
    {
        ndefTarget->SetNdefOperationCallback(aCallback);
        if (ndefTarget)
        {
            RPointerArray<CNdefMessage> result;
            TRAP( error,
                for (int i = 0; i < messages.count(); ++i)
                {
                    result.Append(QNFCNdefUtility::QNdefMsg2CNdefMsgL(messages.at(i)));
                }
            )

            if (error == KErrNone)
            {
                ndefTarget->setNdefMessages(result);
            }

            result.Close();
        }
    }
    if (error != KErrNone)
    {
        aCallback->WriteComplete(error);
    }
    END
    return (error == KErrNone);
}

template<typename TAGTYPE>
bool QNearFieldTagImpl<TAGTYPE>::DoHasNdefMessages()
{
    BEGIN

    LOG("use async request to check ndef message");
    NearFieldTagNdefRequest * readNdefRequest = new NearFieldTagNdefRequest(*this);

    QNearFieldTarget::RequestId requestId;

    if (readNdefRequest)
    {
        readNdefRequest->SetRequestId(requestId);
        readNdefRequest->SetNdefRequestType(NearFieldTagNdefRequest::EReadRequest);
        int index = mPendingRequestList.indexOf(mCurrentRequest);

        if (!_isProcessingRequest())
        {
            // issue the request
            LOG("the request will be issued at once");
            mCurrentRequest = readNdefRequest;
            if ((index < 0) || (index == mPendingRequestList.count() - 1))
            {
                // no next request
                mPendingRequestList.append(readNdefRequest);
            }
            else
            {
                mPendingRequestList.insert(index+1, readNdefRequest);
            }
            readNdefRequest->IssueRequest();
        }
        else
        {
            if ((index < 0) || (index == mPendingRequestList.count() - 1))
            {
                // no next request
                mPendingRequestList.append(readNdefRequest);
            }
            else
            {
                mPendingRequestList.insert(index+1, readNdefRequest);
            }
        }

        readNdefRequest->WaitRequestCompletedNoSignal(5000);
        if (mMessageList.Count() == 0)
        {
            END
            return false;
        }
        else
        {
            mMessageList.Reset();
            END
            return true;
        }
    }
    else
    {
        LOG("unexpect error to create async request");
        END
        return false;
    }
}

template<typename TAGTYPE>
bool QNearFieldTagImpl<TAGTYPE>::DoSendCommand(const QByteArray& command, MNearFieldTagOperationCallback * const aCallback, bool deferred)
{
    BEGIN
    int error = KErrGeneral;

    if (command.count() > 0)
    {
        CNearFieldTag * tag = mTag->CastToTag();

        if (tag)
        {
            tag->SetTagOperationCallback(aCallback);
            TPtrC8 cmd = QNFCNdefUtility::QByteArray2TPtrC8(command);
            TRAP( error,
                // Lazy creation
                if (mResponse.MaxLength() == 0)
                {
                    // the response is not created yet.
                    mResponse.CreateL(TagConstValue<TAGTYPE>::MaxResponseSize);
                }
                else
                {
                    mResponse.Zero();
                }

                TTimeIntervalMicroSeconds32 timeout;
                timeout = TagConstValue<TAGTYPE>::Timeout;

                error = tag->RawModeAccess(cmd, mResponse, timeout);
            )
        }
    }
    if (error != KErrNone)
    {
        if (deferred)
        {
            aCallback->CommandComplete(error);
        }
    }
    END
    return (error == KErrNone);
}

template<typename TAGTYPE>
bool QNearFieldTagImpl<TAGTYPE>::IssueNextRequest(QNearFieldTarget::RequestId aId)
{
    BEGIN
    // find the request after current request
    int index = mPendingRequestList.indexOf(mCurrentRequest);
    LOG("index = "<<index);
    if ((index < 0) || (index == mPendingRequestList.count() - 1))
    {
        // no next request
        mCurrentRequest = 0;
        END
        return false;
    }
    else
    {
        if (aId == mCurrentRequest->RequestID())
        {
            mCurrentRequest = mPendingRequestList.at(index + 1);
            mCurrentRequest->IssueRequest();
        }
        else
        {
            LOG("re-entry happened");
        }

        END
        return true;
    }
}

template<typename TAGTYPE>
void QNearFieldTagImpl<TAGTYPE>::RemoveRequestFromQueue(QNearFieldTarget::RequestId aId)
{
    BEGIN
    for(int i = 0; i < mPendingRequestList.count(); ++i)
    {
        MNearFieldTagAsyncRequest * request = mPendingRequestList.at(i);
        if (request->RequestID() == aId)
        {
            LOG("remove request id");
            mPendingRequestList.removeAt(i);
            break;
        }
    }
    END
}

template<typename TAGTYPE>
QNearFieldTarget::RequestId QNearFieldTagImpl<TAGTYPE>::AllocateRequestId()
{
    BEGIN
    QNearFieldTarget::RequestIdPrivate * p = new QNearFieldTarget::RequestIdPrivate;
    QNearFieldTarget::RequestId id(p);
    END
    return id;
}

template<typename TAGTYPE>
void QNearFieldTagImpl<TAGTYPE>::HandleResponse(const QNearFieldTarget::RequestId &id, const QByteArray &command, const QByteArray &response)
{
    BEGIN
    TAGTYPE * tag = static_cast<TAGTYPE *>(this);
    tag->handleTagOperationResponse(id, command, response);
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
QNearFieldTagImpl<TAGTYPE>::QNearFieldTagImpl(CNearFieldNdefTarget *tag) : mTag(tag)
{
    mCurrentRequest = 0;
}

template<typename TAGTYPE>
QNearFieldTagImpl<TAGTYPE>::~QNearFieldTagImpl()
{
    BEGIN
    LOG("pending request count is "<<mPendingRequestList.count());
    for (int i = 0; i < mPendingRequestList.count(); ++i)
    {
        delete mPendingRequestList[i];
    }

    mPendingRequestList.clear();
    mCurrentRequest = 0;

    delete mTag;

    mMessageList.Close();

    mResponse.Close();
    END
}

template<typename TAGTYPE>
bool QNearFieldTagImpl<TAGTYPE>::_hasNdefMessage()
{
    return DoHasNdefMessages();
}

template<typename TAGTYPE>
void QNearFieldTagImpl<TAGTYPE>::_ndefMessages()
{
    BEGIN
    NearFieldTagNdefRequest * readNdefRequest = new NearFieldTagNdefRequest(*this);
    QNearFieldTarget::RequestId requestId = AllocateRequestId();

    if (readNdefRequest)
    {
        LOG("read ndef request created");
        readNdefRequest->SetRequestId(requestId);
        readNdefRequest->SetNdefRequestType(NearFieldTagNdefRequest::EReadRequest);

        if (!_isProcessingRequest())
        {
            LOG("the request will be issued at once");
            // issue the request
            mCurrentRequest = readNdefRequest;
            mPendingRequestList.append(readNdefRequest);
            readNdefRequest->IssueRequest();
        }
        else
        {
            mPendingRequestList.append(readNdefRequest);
        }
    }
    else
    {
        EmitError(KErrNoMemory, QNearFieldTarget::RequestId());
    }
    END
}

template<typename TAGTYPE>
void QNearFieldTagImpl<TAGTYPE>::_setNdefMessages(const QList<QNdefMessage> &messages)
{
    BEGIN
    NearFieldTagNdefRequest * writeNdefRequest = new NearFieldTagNdefRequest(*this);
    QNearFieldTarget::RequestId requestId = AllocateRequestId();

    if (writeNdefRequest)
    {
        LOG("write ndef request created");
        writeNdefRequest->SetRequestId(requestId);
        writeNdefRequest->SetNdefRequestType(NearFieldTagNdefRequest::EWriteRequest);
        writeNdefRequest->SetInputNdefMessages(messages);

        if (!_isProcessingRequest())
        {
            // issue the request
            LOG("the request will be issued at once");
            mCurrentRequest = writeNdefRequest;
            mPendingRequestList.append(writeNdefRequest);
            writeNdefRequest->IssueRequest();
        }
        else
        {
            mPendingRequestList.append(writeNdefRequest);
        }
    }
    else
    {
        EmitError(KErrNoMemory, QNearFieldTarget::RequestId());
    }
    END
}

template<typename TAGTYPE>
QNearFieldTarget::RequestId QNearFieldTagImpl<TAGTYPE>::_sendCommand(const QByteArray &command)
{
    BEGIN
    NearFieldTagCommandRequest * rawCommandRequest = new NearFieldTagCommandRequest(*this);
    QNearFieldTarget::RequestId requestId = AllocateRequestId();

    if (rawCommandRequest)
    {
        LOG("send command request created");
        rawCommandRequest->SetInputCommand(command);
        rawCommandRequest->SetRequestId(requestId);
        rawCommandRequest->SetResponseBuffer(&mResponse);

        if (!_isProcessingRequest())
        {
            // issue the request
            LOG("the request will be issued at once");
            mCurrentRequest = rawCommandRequest;

            if (rawCommandRequest->IssueRequestNoDefer())
            {
                mPendingRequestList.append(rawCommandRequest);
            }
            else
            {
                END
                return QNearFieldTarget::RequestId();
            }
        }
        else
        {
            mPendingRequestList.append(rawCommandRequest);
        }
    }
    else
    {
        END
        return QNearFieldTarget::RequestId();
    }
    END
    return requestId;
}

template<typename TAGTYPE>
QNearFieldTarget::RequestId QNearFieldTagImpl<TAGTYPE>::_sendCommands(const QList<QByteArray> &commands)
{
    BEGIN
    NearFieldTagCommandsRequest * rawCommandsRequest = new NearFieldTagCommandsRequest(*this);
    QNearFieldTarget::RequestId requestId = AllocateRequestId();

    if (rawCommandsRequest)
    {
        LOG("send commands request created");
        rawCommandsRequest->SetInputCommands(commands);
        rawCommandsRequest->SetRequestId(requestId);
        rawCommandsRequest->SetResponseBuffer(&mResponse);

        if (!_isProcessingRequest())
        {
            // issue the request
            LOG("the request will be issued at once");
            mCurrentRequest = rawCommandsRequest;

            if (rawCommandsRequest->IssueRequestNoDefer())
            {
                mPendingRequestList.append(rawCommandsRequest);
            }
            else
            {
                END
                return QNearFieldTarget::RequestId();
            }
        }
        else
        {
            mPendingRequestList.append(rawCommandsRequest);
        }
    }
    else
    {
        END
        return QNearFieldTarget::RequestId();
    }
    END
    return requestId;
}

template<typename TAGTYPE>
QByteArray QNearFieldTagImpl<TAGTYPE>::_uid() const
{
    BEGIN
    if (mUid.isEmpty())
    {
        mUid = QNFCNdefUtility::TDesC2QByteArray(mTag->Uid());
        LOG(mUid);
    }
    END
    return mUid;
}

template<typename TAGTYPE>
bool QNearFieldTagImpl<TAGTYPE>::_isProcessingRequest() const
{
    BEGIN
    bool result = mPendingRequestList.count() > 0;
    LOG(result);
    END
    return result;

}

template<typename TAGTYPE>
bool QNearFieldTagImpl<TAGTYPE>::_waitForRequestCompleted(const QNearFieldTarget::RequestId &id, int msecs)
{
    BEGIN
    int index = -1;
    for (int i = 0; i < mPendingRequestList.count(); ++i)
    {
        if (id == mPendingRequestList.at(i)->RequestID())
        {
            index = i;
            break;
        }
    }

    if (index < 0)
    {
        // request ID is not in pending list. So maybe it is already completed.
        END
        return false;
    }

    MNearFieldTagAsyncRequest * request = mPendingRequestList.at(index);
    LOG("get the request from pending request list");
    END
    return request->WaitRequestCompleted(msecs);
}

template<typename TAGTYPE>
int QNearFieldTagImpl<TAGTYPE>::_waitForRequestCompletedNoSignal(const QNearFieldTarget::RequestId &id, int msecs)
{
    BEGIN
    int index = -1;
    for (int i = 0; i < mPendingRequestList.count(); ++i)
    {
        if (id == mPendingRequestList.at(i)->RequestID())
        {
            index = i;
            break;
        }
    }

    if (index < 0)
    {
        // request ID is not in pending list. So either it may not be issued, or has already completed
        END
        return false;
    }

    MNearFieldTagAsyncRequest * request = mPendingRequestList.at(index);
    LOG("get the request from pending request list");
    END
    return request->WaitRequestCompletedNoSignal(msecs);
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

template<typename TAGTYPE>
void QNearFieldTagImpl<TAGTYPE>::DoCancelSendCommand()
{
    BEGIN
    CNearFieldTag * tag = mTag->CastToTag();
    if (tag)
    {
        LOG("Cancel raw command operation");
        tag->SetTagOperationCallback(0);
        tag->Cancel();
    }
    END
}

template<typename TAGTYPE>
void QNearFieldTagImpl<TAGTYPE>::DoCancelNdefAccess()
{
    BEGIN
    CNearFieldNdefTarget * ndefTarget = mTag->CastToNdefTarget();
    if(ndefTarget)
    {
        LOG("Cancel ndef operation");
        ndefTarget->SetNdefOperationCallback(0);
        ndefTarget->Cancel();
    }
    END
}

template<typename TAGTYPE>
QNearFieldTarget::Error QNearFieldTagImpl<TAGTYPE>::SymbianError2QtError(int error)
{
    BEGIN
    QNearFieldTarget::Error qtError = QNearFieldTarget::InvalidParametersError;
    switch(error)
    {
        case KErrNone:
        {
            qtError = QNearFieldTarget::NoError;
            break;
        }
        case KErrNotSupported:
        {
            qtError = QNearFieldTarget::UnsupportedError;
            break;
        }
        case KErrTimedOut:
        {
            qtError = QNearFieldTarget::NoResponseError;
            break;
        }
        case KErrAccessDenied:
        case KErrEof:
        case KErrServerTerminated:
        {
            if (mCurrentRequest)
            {
                if(mCurrentRequest->Type() == MNearFieldTagAsyncRequest::ENdefRequest)
                {
                    NearFieldTagNdefRequest * req = static_cast<NearFieldTagNdefRequest *>(mCurrentRequest);
                    if (req->GetNdefRequestType() == NearFieldTagNdefRequest::EReadRequest)
                    {
                        qtError = QNearFieldTarget::NdefReadError;
                    }
                    else if (req->GetNdefRequestType() == NearFieldTagNdefRequest::EWriteRequest)
                    {
                        qtError = QNearFieldTarget::NdefWriteError;
                    }
                }
            }
            break;
        }
        case KErrTooBig:
        {
            qtError = QNearFieldTarget::TargetOutOfRangeError;
            break;
        }
        default:
        {
            break;
        }
    }
    LOG(qtError);
    END
    return qtError;
}
QTM_END_NAMESPACE
#endif // QNEARFIELDTAGIMPL_H
