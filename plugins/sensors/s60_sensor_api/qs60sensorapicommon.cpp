/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qs60sensorapicommon.h"

const TInt KAccelerometerSensorUID = 0x10273024;

QS60SensorApiCommon::QS60SensorApiCommon(QSensor *sensor)
    : QSensorBackend(sensor)
    , m_nativeSensor(NULL)
{
    TRAPD(err, findAndCreateNativeSensorL());
    if(err != KErrNone)
        sensorError(err);
}

QS60SensorApiCommon::~QS60SensorApiCommon()
{
    stop();
    delete m_nativeSensor;
    m_nativeSensor = NULL;
}

void QS60SensorApiCommon::start()
{
    if(!m_nativeSensor)
        return;
    
    m_nativeSensor->AddDataListener(this);    
}

void QS60SensorApiCommon::stop()
{
    if(!m_nativeSensor)
        return;
    
    m_nativeSensor->RemoveDataListener();
}

void QS60SensorApiCommon::poll()
{
    //empty implementation
}

void QS60SensorApiCommon::findAndCreateNativeSensorL()
{
    if(m_nativeSensor)
        return;
    
    RArray<TRRSensorInfo> sensorList;
    CRRSensorApi::FindSensorsL(sensorList);
    CleanupClosePushL(sensorList);

    TInt index = 0;
    do {
        if (sensorList[index].iSensorId == KAccelerometerSensorUID)
            m_nativeSensor = CRRSensorApi::NewL(sensorList[index]);
    } while(!m_nativeSensor || index < sensorList.Count());
        
    if (!m_nativeSensor)
        User::Leave(KErrNotFound);
    
    CleanupStack::PopAndDestroy(&sensorList);
}
