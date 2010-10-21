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

#ifndef QLANDMARKIMPORTREQUEST_H
#define QLANDMARKIMPORTREQUEST_H

#include "qlandmarkabstractrequest.h"
class QIODevice;
QTM_BEGIN_NAMESPACE

class QLandmarkImportRequestPrivate;
class Q_LOCATION_EXPORT QLandmarkImportRequest : public QLandmarkAbstractRequest
{
    Q_OBJECT
public:
    QLandmarkImportRequest(QLandmarkManager *manager, QObject *parent = 0);
    ~QLandmarkImportRequest();

    QIODevice *device() const;
    void setDevice(QIODevice *device);

    QString fileName() const;
    void setFileName(const QString &fileName);

    QString format() const;
    void setFormat(const QString &format);

    QLandmarkManager::TransferOption transferOption() const;
    void setTransferOption(QLandmarkManager::TransferOption option);

    QLandmarkCategoryId categoryId() const;
    void setCategoryId(const QLandmarkCategoryId &categoryId);

    QList<QLandmarkId> landmarkIds() const;
private:
    Q_DISABLE_COPY(QLandmarkImportRequest)
    Q_DECLARE_PRIVATE(QLandmarkImportRequest)
    friend class QLandmarkManagerEngine;
};

QTM_END_NAMESPACE

#endif

