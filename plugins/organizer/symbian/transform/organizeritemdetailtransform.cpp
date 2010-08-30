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
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "organizeritemdetailtransform.h"

OrganizerItemDetailTransform::OrganizerItemDetailTransform()
{

}

OrganizerItemDetailTransform::~OrganizerItemDetailTransform()
{

}

void OrganizerItemDetailTransform::modifyBaseSchemaDefinitions(QMap<QString, QMap<QString, QOrganizerItemDetailDefinition> > &schemaDefs) const
{
    Q_UNUSED(schemaDefs);
    // empty default implementation 
}

void OrganizerItemDetailTransform::transformToDetailL(const CCalInstance& instance, QOrganizerItem *itemInstance)
{
    // In most cases we can use the other transformToDetailL function without modification
    transformToDetailL(instance.Entry(), itemInstance);
}

void OrganizerItemDetailTransform::transformToDetailPostSaveL(const CCalEntry& entry, QOrganizerItem *item)
{
    Q_UNUSED(entry);
    Q_UNUSED(item);
    // empty default implementation
}

QString OrganizerItemDetailTransform::toQString(const TDesC8 &des)
{
    return QString::fromUtf8((const char *)des.Ptr(), des.Length());
}

QString OrganizerItemDetailTransform::toQString(const TDesC16 &des)
{
    return QString::fromUtf16(des.Ptr(), des.Length());
}

TPtrC8 OrganizerItemDetailTransform::toPtrC8(const QByteArray &bytes)
{
    return TPtrC8(reinterpret_cast<const TUint8*>(bytes.constData()), bytes.size());
}

TPtrC16 OrganizerItemDetailTransform::toPtrC16(const QString &string)
{
    return TPtrC16(reinterpret_cast<const TUint16*>(string.utf16()), string.size());
}

TCalTime OrganizerItemDetailTransform::toTCalTimeL(QDateTime dateTime)
{
    TCalTime calTime;
    calTime.SetTimeUtcL(Time::NullTTime());

    if (dateTime.isValid()) {
        uint secondsFrom1970 = dateTime.toTime_t();
        quint64 usecondsFrom1970 = ((quint64) secondsFrom1970) * ((quint64) 1000000) + ((quint64) dateTime.time().msec() * (quint64)1000);
        TTime time1970(_L("19700000:000000.000000"));
        quint64 usecondsBCto1970 = time1970.MicroSecondsFrom(TTime(0)).Int64();
        quint64 useconds = usecondsBCto1970 + usecondsFrom1970;

        TTime time(useconds);
        calTime.SetTimeUtcL(time);
    }

    return calTime;
}

TCalTime OrganizerItemDetailTransform::toTCalTimeL(QDate date)
{
    TCalTime calTime;
    calTime.SetTimeUtcL(Time::NullTTime());

    if (date.isValid()) {
        QDateTime dateTime(date);
        calTime = toTCalTimeL(dateTime);
    }

    return calTime;
}

QDateTime OrganizerItemDetailTransform::toQDateTimeL(TCalTime calTime)
{
    const TTime time1970(_L("19700000:000000.000000"));
    quint64 usecondsBCto1970 = time1970.MicroSecondsFrom(TTime(0)).Int64();
    quint64 useconds = calTime.TimeUtcL().Int64() - usecondsBCto1970;
    quint64 seconds = useconds / (quint64)1000000;
    quint64 msecondscomponent = (useconds - seconds * (quint64)1000000) / (quint64)1000;
    QDateTime dateTime;
    dateTime.setTime_t(seconds);
    dateTime.setTime(dateTime.time().addMSecs(msecondscomponent));
    return dateTime;
}
