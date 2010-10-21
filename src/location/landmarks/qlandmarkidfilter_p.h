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

#ifndef QLANDMARKIDFILTER_P_H
#define QLANDMARKIDFILTER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qlandmarkfilter_p.h"
#include "qlandmarkid.h"
#include "qlandmarkidfilter.h"

QTM_BEGIN_NAMESPACE

class QLandmarkIdFilterPrivate : public QLandmarkFilterPrivate
{
public:
    QLandmarkIdFilterPrivate(const QList<QLandmarkId> &ids =QList<QLandmarkId>());
    QLandmarkIdFilterPrivate(const QLandmarkIdFilterPrivate &other);
    virtual ~QLandmarkIdFilterPrivate();

    virtual bool compare(const QLandmarkFilterPrivate *other) const
    {
        const QLandmarkIdFilterPrivate *od = static_cast<const QLandmarkIdFilterPrivate*>(other);
        return landmarkIds == od->landmarkIds;
    }

    Q_IMPLEMENT_LANDMARKFILTER_VIRTUALCTORS(QLandmarkIdFilter, QLandmarkFilter::LandmarkIdFilter)

    QList<QLandmarkId> landmarkIds;
};

QTM_END_NAMESPACE

#endif
