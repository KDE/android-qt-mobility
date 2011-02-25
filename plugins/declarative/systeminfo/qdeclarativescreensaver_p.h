/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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


#ifndef QDECLARATIVESCREENSAVERINFO_H
#define QDECLARATIVESCREENSAVERINFO_H

#include <QObject>
#include <qsystemscreensaver.h>

QT_BEGIN_HEADER
QTM_USE_NAMESPACE

class QDeclarativeScreenSaver : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool screenSaverDelayed READ screenSaverDelayed WRITE setScreenSaverDelayed)
    Q_PROPERTY(bool screenSaverInhibited READ screenSaverInhibited CONSTANT)

public:
    explicit QDeclarativeScreenSaver(QObject *parent = 0);
    virtual ~QDeclarativeScreenSaver();

    bool screenSaverDelayed();
    bool screenSaverInhibited();
    Q_INVOKABLE bool setScreenSaverInhibit();

signals:

public slots:
    void setScreenSaverDelayed(bool on);

private:
    QSystemScreenSaver *screensaverInfo;
    bool screenSaverDelay;

};
QT_END_NAMESPACE
QT_END_HEADER

#endif // QDECLARATIVESCREENSAVERINFO_H
