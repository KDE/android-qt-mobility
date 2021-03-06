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

#ifndef QREMOTESERVICEREGISTER_P_H
#define QREMOTESERVICEREGISTER_P_H

#include "qremoteserviceregister.h"
#include "instancemanager_p.h"
#include "qserviceinterfacedescriptor.h"

QTM_BEGIN_NAMESPACE

class ObjectEndPoint;
class QRemoteServiceRegisterPrivate: public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool quitOnLastInstanceClosed READ quitOnLastInstanceClosed WRITE setQuitOnLastInstanceClosed)
public:
    QRemoteServiceRegisterPrivate(QObject* parent);
    virtual ~QRemoteServiceRegisterPrivate();

    virtual void publishServices(const QString& ident ) = 0;

    virtual bool quitOnLastInstanceClosed() const;
    virtual void setQuitOnLastInstanceClosed(const bool quit);

    virtual QRemoteServiceRegister::SecurityFilter setSecurityFilter(QRemoteServiceRegister::SecurityFilter filter);

public slots:
    // Must be implemented in the subclass
    //void processIncoming();

protected:
    virtual QRemoteServiceRegister::SecurityFilter getSecurityFilter();

private:
    bool m_quit;
    QRemoteServiceRegister::SecurityFilter iFilter;

public:
    static QObject* proxyForService(const QRemoteServiceRegister::Entry& entry, const QString& location);
    static QRemoteServiceRegisterPrivate* constructPrivateObject(QObject *parent);    
};

QTM_END_NAMESPACE

#endif
