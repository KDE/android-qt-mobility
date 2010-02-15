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
#ifndef SAMPLESERVICEPLUGIN_H
#define SAMPLESERVICEPLUGIN_H

#include <QObject>
#include <qserviceplugininterface.h>
#include <qserviceinterfacedescriptor.h>

QTM_USE_NAMESPACE

class SampleServicePlugin : public QObject, public QServicePluginInterface
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QServicePluginInterface)

public:
    ~SampleServicePlugin();
    QObject* createInstance(const QServiceInterfaceDescriptor& descriptor,
                            QServiceContext* context,
                            QAbstractSecuritySession* session);

    virtual void installService();
    virtual void uninstallService();
};


class SampleServicePluginClass : public QObject
{
    Q_OBJECT
public:
    SampleServicePluginClass(const QServiceInterfaceDescriptor& descriptor,
                             QServiceContext* context,
                             QAbstractSecuritySession* session);
    virtual ~SampleServicePluginClass() {}

    QServiceInterfaceDescriptor descriptor() const;
    QServiceContext *context() const { return m_context; }
    QAbstractSecuritySession *securitySession() const { return m_security; }

public slots:
    void testSlotOne();
    void testSlotTwo() {}

protected:
    QServiceInterfaceDescriptor m_descriptor;
    QServiceContext *m_context;
    QAbstractSecuritySession *m_security;
};

class DerivedSampleServicePluginClass : public SampleServicePluginClass
{
    Q_OBJECT
public:
    DerivedSampleServicePluginClass(const QServiceInterfaceDescriptor& descriptor,
                             QServiceContext* context,
                             QAbstractSecuritySession* session);
    virtual ~DerivedSampleServicePluginClass() {}
public slots:
    void testSlotOne();
};

#endif
