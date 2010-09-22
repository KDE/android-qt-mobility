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

#ifndef QMDEGALLERYTYPERESULTSET_H
#define QMDEGALLERYTYPERESULTSET_H

#include <qstringlist.h>

#include "qmdegalleryresultset.h"

QTM_BEGIN_NAMESPACE

class QGalleryTypeRequest;

class QMDEGalleryTypeResultSet : public QMDEGalleryResultSet
{
    Q_OBJECT
public:
    QMDEGalleryTypeResultSet(QMdeSession *session, QObject *parent = 0);
    ~QMDEGalleryTypeResultSet();

    void createQuery();

public: // from QGalleryResultSet

    virtual int itemCount() const;

    virtual QVariant itemId() const;
    virtual QUrl itemUrl() const;

    virtual QVariant metaData(int key) const;
    virtual bool setMetaData(int key, const QVariant &value);

    virtual bool fetch(int index);
    /* non pure virtual */
    virtual bool fetchNext();
    virtual bool fetchPrevious();
    virtual bool fetchFirst();
    virtual bool fetchLast();

private:

    QGalleryTypeRequest *m_request;
    QStringList m_propertyList;

};

QTM_END_NAMESPACE
#endif // QMDEGALLERYTYPERESULTSET_H
