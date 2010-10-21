/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QLANDMARKFILTER_H
#define QLANDMARKFILTER_H

#include "qmobilityglobal.h"
#include <QSharedData>
#include <Qt>

QTM_BEGIN_NAMESPACE

#define Q_DECLARE_LANDMARKFILTER_PRIVATE(Class) \
    inline Class##Private* d_func(); \
    inline const Class##Private* d_func() const; \
    friend class Class##Private;

class QLandmarkFilterPrivate;
class Q_LOCATION_EXPORT QLandmarkFilter
{

friend class QLandmarkFilterPrivate;

public:
    enum FilterType {InvalidFilter, DefaultFilter,
                     NameFilter, ProximityFilter,
                     CategoryFilter, BoxFilter, IntersectionFilter,
                     UnionFilter, AttributeFilter,
                     LandmarkIdFilter
                    };

    enum MatchFlag {
        MatchExactly = Qt::MatchExactly,        //0
        MatchContains = Qt::MatchContains,      //1
        MatchStartsWith = Qt::MatchStartsWith,  //2
        MatchEndsWith = Qt::MatchEndsWith,      //3
        MatchFixedString =  Qt::MatchFixedString, //8
        MatchCaseSensitive = Qt::MatchCaseSensitive //16
    };

    Q_DECLARE_FLAGS(MatchFlags, MatchFlag)

    QLandmarkFilter();
    QLandmarkFilter(const QLandmarkFilter &other);
    QLandmarkFilter &operator=(const QLandmarkFilter &other);
    virtual ~QLandmarkFilter();

    FilterType type() const;

    bool operator==(const QLandmarkFilter &other) const;
    bool operator!=(const QLandmarkFilter &other) const {
        return !(*this == other);
    }

protected:
    QLandmarkFilter(QLandmarkFilterPrivate *dd);
    QSharedDataPointer<QLandmarkFilterPrivate> d_ptr;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QLandmarkFilter::MatchFlags);

const Q_LOCATION_EXPORT QLandmarkFilter operator&(const QLandmarkFilter &left, const QLandmarkFilter &right);
const Q_LOCATION_EXPORT QLandmarkFilter operator|(const QLandmarkFilter &left, const QLandmarkFilter &right);

QTM_END_NAMESPACE

Q_DECLARE_TYPEINFO(QTM_PREPEND_NAMESPACE(QLandmarkFilter), Q_MOVABLE_TYPE);


#endif
