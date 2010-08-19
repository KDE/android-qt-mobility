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

#ifndef QCONTACTLESSMANAGER_H
#define QCONTACTLESSMANAGER_H

#include <qmobilityglobal.h>

#include <QtCore/QObject>

#include <qcontactlesstarget.h>
#include <qndefrecord.h>

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class Q_CONNECTIVITY_EXPORT QContactlessManager : public QObject
{
    Q_OBJECT

public:
    explicit QContactlessManager(QObject *parent = 0);
    ~QContactlessManager();

    template<typename T>
    int registerTargetDetectedHandler(QContactlessTarget::TagType tagType,
                                      const QObject *object, const char *slot);
    int registerTargetDetectedHandler(QContactlessTarget::TagType tagType,
                                      QNdefRecord::TypeNameFormat typeNameFormat,
                                      const QByteArray &type,
                                      const QObject *object, const char *slot);
    int registerTargetDetectedHandler(QContactlessTarget::TagType tagType,
                                      quint8 typeNameFormat, const QByteArray &type,
                                      const QObject *object, const char *slot);

    bool unregisterTargetDetectedHandler(int handlerId);

signals:
    void targetDetected(const QContactlessTarget &target);
    void transactionDetected(const QByteArray &applicationIdentifier);
};

template<typename T>
int QContactlessManager::registerTargetDetectedHandler(QContactlessTarget::TagType tagType,
                                                        const QObject *object, const char *slot)
{
    T record;

    return registerTargetDetectedHandler(tagType, record.userTypeNameFormat(), record.type(),
                                         object, slot);
}

QTM_END_NAMESPACE

QT_END_HEADER

#endif // QCONTACTLESSMANAGER_H
