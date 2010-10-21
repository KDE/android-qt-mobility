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
#ifndef QLANDMARK_P_H
#define QLANDMARK_P_H

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

#include "qlandmark.h"
#include "qlandmarkid.h"
#include "qgeoplace_p.h"
#include <QList>
#include <QHash>
#include <QSharedData>
#include <QUrl>

QTM_BEGIN_NAMESPACE

class QLandmarkPrivate : public QGeoPlacePrivate
{
public:
    QLandmarkPrivate();
    QLandmarkPrivate(const QGeoPlacePrivate &other);
    QLandmarkPrivate(const QLandmarkPrivate &other);
    ~QLandmarkPrivate();

    QLandmarkPrivate& operator= (const QLandmarkPrivate &other);

    bool operator== (const QLandmarkPrivate &other) const;
    virtual QGeoPlacePrivate* clone() const { return new QLandmarkPrivate(*this); }

    QString name;
    QList<QLandmarkCategoryId> categoryIds;
    QString description;
    QUrl iconUrl;
    qreal radius;
    QString phoneNumber;
    QUrl url;
    QLandmarkId id;
    static QStringList commonKeys;
};

QTM_END_NAMESPACE

#endif
