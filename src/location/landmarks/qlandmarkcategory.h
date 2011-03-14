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

#ifndef QLANDMARKCATEGORY_H
#define QLANDMARKCATEGORY_H

#include "qmobilityglobal.h"
#include <QVariant>
#include <QSharedDataPointer>


class QStringList;
class QUrl;

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE
class QLandmarkCategoryId;

class QLandmarkCategoryPrivate;
class Q_LOCATION_EXPORT QLandmarkCategory
{
public:
    QLandmarkCategory();
    QLandmarkCategory(const QLandmarkCategory &other);
    ~QLandmarkCategory();

    QLandmarkCategory& operator= (const QLandmarkCategory &other);

    bool operator==(const QLandmarkCategory &other) const;
    bool operator!=(const QLandmarkCategory &other) const;

    QString name() const;
    void setName(const QString &name);

    QUrl iconUrl() const;
    void setIconUrl(const QUrl &url);

    QLandmarkCategoryId categoryId() const;
    void setCategoryId(const QLandmarkCategoryId &id);

    void clear();
private:
    QSharedDataPointer<QLandmarkCategoryPrivate> d;
};

QTM_END_NAMESPACE

Q_DECLARE_METATYPE(QTM_PREPEND_NAMESPACE(QLandmarkCategory))

Q_DECLARE_TYPEINFO(QTM_PREPEND_NAMESPACE(QLandmarkCategory), Q_MOVABLE_TYPE);

QT_END_HEADER

#endif
