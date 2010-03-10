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

#ifndef QMETADATACONTROLMETAOBJECT_P_H
#define QMETADATACONTROLMETAOJBECT_P_H

#include <qtmedianamespace.h>

#include <QtCore/qmetaobject.h>
#include <QtCore/private/qobject_p.h>
#include <QtDeclarative/private/qmetaobjectbuilder_p.h>

QTM_BEGIN_NAMESPACE

class QMetaDataControl;

class QMetaDataControlMetaObject : public QAbstractDynamicMetaObject
{
public:
    QMetaDataControlMetaObject(QMetaDataControl *control, QObject *object);
    ~QMetaDataControlMetaObject();

    int metaCall(QMetaObject::Call call, int _id, void **arguments);
    int createProperty(const char *, const char *);

    void metaDataChanged();

private:
    QMetaDataControl *m_control;
    QObject *m_object;
    QMetaObject *m_mem;

    int m_propertyOffset;
    int m_signalOffset;

    QVector<QtMedia::MetaData> m_keys;
    QMetaObjectBuilder m_builder;
};

QTM_END_NAMESPACE

#endif
