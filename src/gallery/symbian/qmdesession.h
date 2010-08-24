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


#ifndef QMDESESSION_H
#define QMDESESSION_H

#include <qmobilityglobal.h>

#include <QObject>
#include <QEventloop>
#include <mdesession.h>
#include <mdequery.h>
#include <e32std.h>

QTM_BEGIN_NAMESPACE

class QGalleryAbstractResponse;

class QMdeSession : public QObject, public MMdESessionObserver
{
    Q_OBJECT
public:

    QMdeSession(QObject *parent = 0);
    virtual ~QMdeSession();

public: // From MMdESessionObserver
    /**
     * For checking MdE initialization status
     * @param aSession  MdE Session which was opened
     * @param aError  Error code from the opening
     */
    void HandleSessionOpened( CMdESession &aSession, int aError );

    /**
     * For checking MdE session errors
     * @param aSession  MdE Session which was opened
     * @param aError  Error which has occurred
     */
    void HandleSessionError( CMdESession &aSession, int aError );

    CMdENamespaceDef& GetDefaultNamespaceDefL();

    CMdEObject* GetFullObjectL( const unsigned int aId );

    CMdEObjectQuery* NewObjectQueryL( CMdENamespaceDef &aNamespaceDef,
        CMdEObjectDef &aObjectDef, MMdEQueryObserver *aObserver );

    int RemoveObject( const unsigned int aItemId );

private:

    QEventLoop m_eventLoop;
    CMdESession *m_cmdeSession;
};
QTM_END_NAMESPACE

#endif // QMDESESSION_H
