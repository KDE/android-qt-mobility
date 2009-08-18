/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/


#ifndef QCONTACTMANAGERINFO_H
#define QCONTACTMANAGERINFO_H

#include <QVariant>
#include <QStringList>
#include <QSharedDataPointer>

#include "qtcontactsglobal.h"

class QContactFilter;

class QContactManagerData;
class QTCONTACTS_EXPORT QContactManagerInfo
{
public:
    enum ManagerFeature {
        Synchronous = 0,
        Asynchronous,
        Groups,
        Locking,
        Batch,
        ActionPreferences,
        ReadOnlyDetails,
        CreateOnlyDetails,
        MutableDefinitions,
        NativeFiltering,
        NativeSorting
    };

    bool hasFeature(QContactManagerInfo::ManagerFeature feature) const;
    virtual bool filterSupported(const QContactFilter& filter) const;
    QList<QVariant::Type> supportedDataTypes() const;

private:
    /* Can't instantiate apart from through manager */
    QContactManagerInfo();
    Q_DISABLE_COPY(QContactManagerInfo)

    /* Can't delete except via the manager either */
    ~QContactManagerInfo();

    /* can't compare either */
    bool operator==(const QContactManagerInfo& other) const;
    bool operator!=(const QContactManagerInfo& other) const;

    friend class QContactManager;
    QContactManagerData * d;
};

#endif

