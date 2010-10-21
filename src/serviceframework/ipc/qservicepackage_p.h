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

#ifndef QSERVICE_PACKAGE_H
#define QSERVICE_PACKAGE_H

#include "qmobilityglobal.h"
#include <QExplicitlySharedDataPointer>
#include <QSharedData>
#include <QUuid>
#include <QVariant>
#include "qremoteserviceregister.h"

QT_BEGIN_NAMESPACE
class QDataStream;
class QDebug;
QT_END_NAMESPACE

QTM_BEGIN_NAMESPACE

class QServicePackagePrivate;
class QM_AUTOTEST_EXPORT QServicePackage
{
public:
    QServicePackage();
    QServicePackage(const QServicePackage& other);
    QServicePackage& operator=(const QServicePackage& other);
    ~QServicePackage();

    enum Type {
        ObjectCreation = 0,
        MethodCall,
        PropertyCall
    };

    enum ResponseType {
        NotAResponse = 0,
        Success,
        Failed
    };

    QServicePackage createResponse() const;

    bool isValid() const;
    
    QExplicitlySharedDataPointer<QServicePackagePrivate> d;

#ifndef QT_NO_DATASTREAM
    friend QDataStream &operator<<(QDataStream &, const QServicePackage&);
    friend QDataStream &operator>>(QDataStream &, QServicePackage&);
#endif
};

#ifndef QT_NO_DATASTREAM
QDataStream &operator<<(QDataStream &, const QServicePackage&);
QDataStream &operator>>(QDataStream &, QServicePackage&);
#endif

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug, const QServicePackage&);
#endif

class QServicePackagePrivate : public QSharedData
{
public:
    QServicePackagePrivate() 
        :   packageType(QServicePackage::ObjectCreation),
            entry(QRemoteServiceRegister::Entry()), payload(QVariant()),
            messageId(QUuid()), instanceId(QUuid()), responseType(QServicePackage::NotAResponse)
    {
    }

    QServicePackage::Type packageType;
    QRemoteServiceRegister::Entry entry;
    QVariant payload;
    QUuid messageId;
    QUuid instanceId;
    QServicePackage::ResponseType responseType;

    virtual void clean()
    {
        packageType = QServicePackage::ObjectCreation;
        messageId = QUuid();
        instanceId = QUuid();
        payload = QVariant();
        entry = QRemoteServiceRegister::Entry();
        responseType = QServicePackage::NotAResponse;
    }
};

QTM_END_NAMESPACE

#endif
