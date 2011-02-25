/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef CONTACTBACKENDSDEFS_H
#define CONTACTBACKENDSDEFS_H

/*!
 * The definitions below are to be used as argument when constructiong a QContactManager.
 */
/*!
 * Default backend in Symbian devices.
 */
const QString SYMBIAN_BACKEND     = "qtcontacts:symbian:";

/*!
 * SIM backend. (ADN contacts)
 */
const QString SIM_BACKEND         = "qtcontacts:symbiansim:";

/*!
 * SIM backend for ADN contacts.
 */
const QString SIM_BACKEND_ADN     = "qtcontacts:symbiansim:store=ADN";

/*!
 * SIM backend for SDN contacts.
 */
const QString SIM_BACKEND_SDN     = "qtcontacts:symbiansim:store=SDN";

/*!
 * SIM backend for FDN contacts.
 */
const QString SIM_BACKEND_FDN     = "qtcontacts:symbiansim:store=FDN";

/*!
 * SIM backend for Own Number contacts.
 *
 */
const QString SIM_BACKEND_ON     = "qtcontacts:symbiansim:store=ON";

/*
 * Definition name for MyCard handling.
 *
 */
const QString MYCARD_DEFINTION = "MyCard";


/*!
* UIDs for preferred (default) fields, used by symbian backend
*/
const int KCntDefaultFieldForCall = 0x10003E70;
const int KCntDefaultFieldForVideoCall = 0x101F85A6;
const int KCntDefaultFieldForEmail = 0x101F85A7;
const int KCntDefaultFieldForMessage = 0x101f4cf1;
const int KCntDefaultFieldForOnlineAccount = 0x2002DC81;
const int KCntDefaultFieldForUrl = 0x20031E4E;

#endif
