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

#include "qservicepackage_p.h"
#include <QDebug>

QTM_BEGIN_NAMESPACE

QServicePackage::QServicePackage()
    : d(0)
{

}

QServicePackage::QServicePackage(const QServicePackage& other)
    : d(other.d)
{
}

QServicePackage& QServicePackage::operator=(const QServicePackage& other)
{
    d = other.d;
    return *this;
}

QServicePackage::~QServicePackage()
{
}

bool QServicePackage::isValid() const
{
    return d;
}

#ifndef QT_NO_DATASTREAM
QDataStream &operator<<(QDataStream &out, const QServicePackage& package)
{
    const quint32 magicNumber = 0x78AFAFB;
    out.setVersion(QDataStream::Qt_4_6);
    out << magicNumber;

    const qint8 valid = package.d ? 1 : 0;
    out << (qint8) valid;
    if (valid) {
        out << (qint8) package.d->type;
    }
    
    return out;
}

QDataStream &operator>>(QDataStream &in, QServicePackage& package)
{
    const quint32 magicNumber = 0x78AFAFB;
    in.setVersion(QDataStream::Qt_4_6);
    
    quint32 storedMagicNumber;
    in >> storedMagicNumber;
    if (storedMagicNumber != magicNumber) {
        qWarning() << "Datastream doesn't provide serialized QServiceFilter";
        return in;
    }

    qint8 valid;
    in >> valid;
    if (valid) {
        if (!package.d) {
            QServicePackagePrivate* priv = new QServicePackagePrivate();
            package.d = priv;
        } else {
            package.d.detach();
            package.d->clean();
        }
        qint8 type;
        in >>  type;
        package.d->type = (QServicePackage::Type) type;
    } else {
        if (package.d)
            package.d.reset();
    }
    
    return in;
}
#endif



QTM_END_NAMESPACE
