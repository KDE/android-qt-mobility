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

#ifndef DEBUG_H_
#define DEBUG_H_
#include <QDebug>

//#define SYMBIAN_NFC_DEBUG

#ifdef SYMBIAN_NFC_DEBUG
# define BEGIN qDebug()<<__PRETTY_FUNCTION__<<" Line: "<<__LINE__ <<" Begin";
# define END qDebug()<<__PRETTY_FUNCTION__<<" Line: "<<__LINE__ <<" End";
# define BEGIN_END qDebug()<<__PRETTY_FUNCTION__<<" Line: "<<__LINE__ <<" BEGIN_End";
# define LOG(a) qDebug()<<__PRETTY_FUNCTION__<<" Line: "<<__LINE__ <<a;
#else
# define BEGIN
# define END
# define BEGIN_END
# define LOG(a)
#endif

#endif /* DEBUG_H_ */
