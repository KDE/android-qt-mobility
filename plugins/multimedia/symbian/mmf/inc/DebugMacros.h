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

#include <QtCore/qdebug.h>

#ifndef __DEBUGMACROS_H__
#define __DEBUGMACROS_H__

// MACROS
#ifdef _DEBUG
#define DP0(string)                                 qDebug()<<string
#define DP1(string,arg1)                            qDebug()<<string<<arg1
#define DP2(string,arg1,arg2)                       qDebug()<<string<<arg1<<arg2
#define DP3(string,arg1,arg2,arg3)                  qDebug()<<string<<arg1<<arg2<<arg3
#define DP4(string,arg1,arg2,arg3,arg4)             qDebug()<<string<<arg1<<arg2<<arg3<<arg4
#define DP5(string,arg1,arg2,arg3,arg4,arg5)        qDebug()<<string<<arg1<<arg2<<arg3<<arg4<<arg5
#define DP6(string,arg1,arg2,arg3,arg4,arg5,arg6)   qDebug()<<string<<arg1<<arg2<<arg3<<arg4<<arg5<<arg6
#else
#define DP0(string)
#define DP1(string,arg1)
#define DP2(string,arg1,arg2)
#define DP3(string,arg1,arg2,arg3)
#define DP4(string,arg1,arg2,arg3,arg4)
#define DP5(string,arg1,arg2,arg3,arg4,arg5)
#define DP6(string,arg1,arg2,arg3,arg4,arg5,arg6)
#endif

#endif //__DEBUGMACROS_H__
