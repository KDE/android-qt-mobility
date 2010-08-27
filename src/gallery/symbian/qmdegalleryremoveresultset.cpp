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

//Backend
#include "qmdegalleryremoveresultset.h"
#include "qmdesession.h"
//API
#include "qgalleryremoverequest.h"

QTM_BEGIN_NAMESPACE

QMDEGalleryRemoveResultSet::QMDEGalleryRemoveResultSet(QMdeSession *session, QObject *parent)
:QMDEGalleryResultSet(parent)
{
    m_request = static_cast<QGalleryRemoveRequest *>(parent);
    m_session = session;

    createQuery();
}

QMDEGalleryRemoveResultSet::~QMDEGalleryRemoveResultSet()
{

}
void QMDEGalleryRemoveResultSet::createQuery()
{
    m_result = m_session->RemoveObject( m_request->itemId().toUInt() );
    // This call only removes the entry from MDS DB. Should the actual binary
    // be deleted from disk???
    // After that m_ret contains error code from request
    finish(m_result, false);
}

#include "moc_qmdegalleryremoveresultset.cpp"
QTM_END_NAMESPACE
