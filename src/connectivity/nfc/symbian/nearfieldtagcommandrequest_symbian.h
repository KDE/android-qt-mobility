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

#ifndef NEARFIELDTAGCOMMANDREQUEST_SYMBIAN_H
#define NEARFIELDTAGCOMMANDREQUEST_SYMBIAN_H

#include "nearfieldtagasyncrequest_symbian.h"
#include "nearfieldtagoperationcallback_symbian.h"

class NearFieldTagCommandRequest : public MNearFieldTagAsyncRequest,
                                   public MNearFieldTagOperationCallback 
    {
public:
    NearFieldTagCommandRequest(MNearFieldTargetOperation& aOperator);
    ~NearFieldTagCommandRequest();
    void IssueRequest();
    bool IssueRequestNoDefer();
    void ProcessTimeout()
    {
        if (iWait)
        {
            if (iWait->IsStarted())
            {
                if (iRequestIssued)
                {    
                    iOperator.DoCancelSendCommand();
                    iRequestIssued = EFalse;
                }
                ProcessResponse(HandlePassiveCommand(KErrTimedOut));
            }
        }
    }
    void ProcessEmitSignal(TInt aError);
    void HandleResponse(TInt aError);
    void SetInputCommand(QByteArray aCommand) { iCommand = aCommand; }
    void SetResponseBuffer(RBuf8 * aResponse) { iResponse = aResponse; }
    QString GetRequestCommand() { return iCommand; }
    TRequestType Type() { return ETagCommandRequest; }
private:
    void CommandComplete(TInt aError);
    TInt HandlePassiveCommand(TInt aError);
    QByteArray iCommand;
    // Not own
    RBuf8 * iResponse;
    QByteArray iRequestResponse;
    }; 

#endif
