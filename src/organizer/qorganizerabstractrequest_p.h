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

#ifndef QORGANIZERABSTRACTREQUEST_P_H
#define QORGANIZERABSTRACTREQUEST_P_H

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

#include "qorganizermanager.h"
#include "qorganizermanager_p.h"
#include "qorganizerabstractrequest.h"

#include <QList>
#include <QPointer>
#include <QMutex>

QTM_BEGIN_NAMESPACE

class QOrganizerAbstractRequestPrivate
{
public:
    QOrganizerAbstractRequestPrivate()
        : m_error(QOrganizerManager::NoError),
            m_state(QOrganizerAbstractRequest::InactiveState),
            m_manager(0)
    {
    }

    virtual ~QOrganizerAbstractRequestPrivate()
    {
    }

    virtual QOrganizerAbstractRequest::RequestType type() const
    {
        return QOrganizerAbstractRequest::InvalidRequest;
    }

    // XXX this should have been used in 1.1
    static void notifyEngine(QOrganizerAbstractRequest* request)
    {
        Q_ASSERT(request);
        QOrganizerAbstractRequestPrivate* d = request->d_ptr;
        if (d) {
            QMutexLocker ml(&d->m_mutex);
            QOrganizerManagerEngine *engine = QOrganizerManagerData::engine(d->m_manager);
            ml.unlock();
            if (engine) {
                engine->requestDestroyed(request);
            }
        }
    }

    QOrganizerManager::Error m_error;
    QOrganizerAbstractRequest::State m_state;
    QPointer<QOrganizerManager> m_manager;
    QPointer<QOrganizerManagerEngine> m_engine;

    mutable QMutex m_mutex;
};

QTM_END_NAMESPACE

#endif
