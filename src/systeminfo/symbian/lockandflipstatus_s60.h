/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef LOCKANDFLIPSTATUS_S60_H
#define LOCKANDFLIPSTATUS_S60_H

#include <e32base.h>
#include <e32property.h>
#include <avkondomainpskeys.h>
#include <hwrmdomainpskeys.h>
#include <QList>
#include <AvkonInternalCRKeys.h>
#include <AknFepInternalPSKeys.h>
#include <centralrepository.h>

class MKeylockStatusObserver
{
public:
    virtual void keylockStatusChanged(TInt aLockType) = 0;
};

class CKeylockStatus : public CActive
{
public:
    CKeylockStatus();
    ~CKeylockStatus();

    void addObserver(MKeylockStatusObserver *observer);
    void removeObserver(MKeylockStatusObserver *observer);
    int getLockStatus();

protected:  //from CActive
    void DoCancel();
    void RunL();

private:
    void startMonitoring();


private:
    RProperty m_keylockProperty;
    TInt m_currentStatus;
    QList<MKeylockStatusObserver *> m_observers;
};

class MFlipStatusObserver
{
public:
    virtual void flipStatusChanged(TInt aFlipType , TInt aFilpKeyBoard) = 0;
};

class CFlipStatus : public CActive
{
public:
    CFlipStatus();
    ~CFlipStatus();
    TInt flipStatus() const;

    void addObserver(MFlipStatusObserver *observer);
    void removeObserver(MFlipStatusObserver *observer);
    bool getFlipStatus();
    bool IsFlipSupported();

protected:  //from CActive
    void DoCancel();
    void RunL();

private:
    void startMonitoring();

private:
    RProperty m_FlipProperty;
    TInt m_flipStatus;
    QList<MFlipStatusObserver *> m_observers;
    TInt m_filpKeyBoard ;
    //Added flags to distinguish flip handling in 5.0 and symbain3 devices
    bool m_flipenabled ;
    bool m_gripenabled ;
};

#endif //LOCKANDFLIPSTATUS_S60_H
