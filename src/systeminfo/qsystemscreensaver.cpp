/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qsystemscreensaver.h"
#include "qsysteminfocommon_p.h"

QTM_BEGIN_NAMESPACE

/////
 /*!
   \class QSystemScreenSaver
   \ingroup systeminfo
   \inmodule QtSystemInfo

    \brief The QSystemScreenSaver class provides access to screen saver and blanking.

   On platforms where there is no one default screensaver mechanism, such as Linux, this class
   may not be available.
*/

/*!
   Constructs a QSystemScreenSaver object with the given \a parent.
 */
QSystemScreenSaver::QSystemScreenSaver(QObject *parent)
    : QObject(parent)
{
#if defined(Q_OS_LINUX) && !defined(QT_SIMULATOR)
    d = new QSystemScreenSaverPrivate(static_cast<QSystemScreenSaverLinuxCommonPrivate*>(parent));
#else
    d = new QSystemScreenSaverPrivate(parent);
#endif
    screenSaverIsInhibited = screenSaverInhibited();
}

/*!
  Destroys the QSystemScreenSaver object.
 */
QSystemScreenSaver::~QSystemScreenSaver()
{
    delete d;
}

/*!
  \brief Set the screensaver to be inhibited.

   Temporarily inhibits the screensaver.

   The screensaver will be set to a non inhibited state only when this QSystemScreenSaver object gets destroyed.

   This is a non blocking function that will return true if the inhibit procedure was successful, otherwise false.


    On platforms that support it, if screensaver is secure by policy, the policy will be honored
    and this will fail.
*/
bool QSystemScreenSaver::setScreenSaverInhibit()
{
    return d->setScreenSaverInhibit();
}

/*!
  \property QSystemScreenSaver::screenSaverInhibited
  \brief Screensaver inhibited.

   Returns true if the screensaver is inhibited, otherwise false.
*/
bool QSystemScreenSaver::screenSaverInhibited()
{
    return d->screenSaverInhibited();
}

void QSystemScreenSaver::setScreenSaverInhibited(bool on)
{
     d->setScreenSaverInhibited(on);
}

#include "moc_qsystemscreensaver.cpp"


QTM_END_NAMESPACE
