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

#ifndef XARECORDSESSIONCOMMON_H
#define XARECORDSESSIONCOMMON_H

#include <e32base.h>
#ifdef PLUGIN_SYMBIAN_TRACE_ENABLED
#   include <e32debug.h>
#endif /* PLUGIN_SYMBIAN_TRACE_ENABLED */

#ifdef PLUGIN_SYMBIAN_TRACE_ENABLED
#   define TRACE_FUNCTION_ENTRY RDebug::Printf( "%s >", __PRETTY_FUNCTION__)
#   define TRACE_FUNCTION_EXIT RDebug::Printf( "%s <", __PRETTY_FUNCTION__)
#   define TRACE_FUNCTION_ENTRY_EXIT RDebug::Printf( "%s ><", __PRETTY_FUNCTION__)
#   define TRACE_LOG(s) RDebug::Print s
#else
#   define TRACE_FUNCTION_ENTRY
#   define TRACE_FUNCTION_EXIT
#   define TRACE_FUNCTION_ENTRY_EXIT
#   define TRACE_LOG
#endif /* PLUGIN_SYMBIAN_TRACE_ENABLED */

#define RET_IF_FALSE(e) \
    if (e == false) \
        { \
        return; \
        }

#define RET_BOOL_IF_FALSE(e) \
    if (e == false) \
        { \
        return e; \
        }

#define RET_ERR_IF_ERR(e) \
    if (e != 0) \
        { \
        return e; \
        }

#define MAX_NUMBER_INTERFACES 20
#define MAX_NUMBER_INPUT_DEVICES 10
#define MAX_NUMBER_ENCODERS 10

//const TInt32 KExtErr = (TInt32)(-2147483648);
const TInt32 KExtErr = -32768;
const TInt32 KExtErrUnspecifiedCodecForContainer = (KExtErr+1);
const TInt32 KExtErrUnsupportedCodecForContainer = (KExtErr+2);
const TInt32 KExtErrUnsupportedURISuffixForContainer = (KExtErr+3);

class XARecordObserver
{
public:
    virtual void cbDurationChanged(TInt64 new_pos) = 0;
    virtual void cbAvailableAudioInputsChanged() = 0;
    virtual void cbRecordingStarted() = 0;
    virtual void cbRecordingStopped() = 0;
};

#endif /* XARECORDSESSIONCOMMON_H */
