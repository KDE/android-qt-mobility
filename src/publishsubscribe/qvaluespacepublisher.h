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

#ifndef QVALUESPACEPUBLISHER_H
#define QVALUESPACEPUBLISHER_H

#include "qmobilityglobal.h"
#include "qvaluespace.h"

#include <QObject>
#include <QString>
#include <QUuid>

QT_BEGIN_HEADER

QT_USE_NAMESPACE

QTM_BEGIN_NAMESPACE

class QValueSpacePublisherPrivate;
class Q_PUBLISHSUBSCRIBE_EXPORT QValueSpacePublisher : public QObject
{
    friend class QAbstractValueSpaceLayer;

    Q_OBJECT

public:
    explicit QValueSpacePublisher(const QString &path, QObject *parent = 0);

    QValueSpacePublisher(QValueSpace::LayerOptions filter, const QString &path,
                        QObject *parent = 0);

    QValueSpacePublisher(const QUuid &uuid, const QString &path, QObject *parent = 0);

    virtual ~QValueSpacePublisher();

    QString path() const;

    bool isConnected() const;

    void sync();

signals:
    void interestChanged(const QString &attribute, bool interested);

public slots:
    void setValue(const QString &name, const QVariant &data);
    void resetValue(const QString &name);

protected:
    virtual void connectNotify(const char *);

private:
    Q_DISABLE_COPY(QValueSpacePublisher)

    QValueSpacePublisherPrivate *d;
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif // QVALUESPACEPUBLISHER_H
