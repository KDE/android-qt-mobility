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

// Internal Headers
#include "lightsensorsym.h"
#include <sensrvgeneralproperties.h>

/**
 * set the id of the Light data sensor
 */
char const * const CLightSensorSym::id("sym.light");


/**
 * Factory function, this is used to create the Light data object
 * @return CLightSensorSym if successful, leaves on failure
 */
CLightSensorSym* CLightSensorSym::NewL(QSensor *sensor)
    {
    CLightSensorSym* self = new (ELeave) CLightSensorSym(sensor);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();//self
    return self;
    }

/**
 * Destructor
 * Closes the backend resources
 */
CLightSensorSym::~CLightSensorSym()
    {
    // Release the backend resources
    Close();
    }

/**
 * Default constructor
 */
CLightSensorSym::CLightSensorSym(QSensor *sensor):CSensorBackendSym(sensor)
        {
        setReading<QLightReading>(&iReading);
        iBackendData.iSensorType = KSensrvChannelTypeIdAmbientLightLuxData;
        }

/*
 * DataReceived is used to retrieve the sensor reading from sensor server
 * It is implemented here to handle Light data sensor specific
 * reading data and provides conversion and utility code
 */
void CLightSensorSym::DataReceived(CSensrvChannel &aChannel, TInt aCount, TInt /*aDataLost*/)
    {
    ProcessData(aChannel, aCount, iData);
    }

void CLightSensorSym::ProcessReading()
    {
    TReal lightValue = iData.iAmbientLight;

    // Get a lock on the reading data
    iBackendData.iReadingLock.Wait();
    // Set the timestamp
    iReading.setTimestamp(iData.iTimeStamp.Int64());
    // Set the light data
    iReading.setLux(lightValue);
    // Release the lock
    iBackendData.iReadingLock.Signal();
    // Notify that a reading is available
    newReadingAvailable();
    }

/**
 * Second phase constructor
 * Initialize the backend resources
 */
void CLightSensorSym::ConstructL()
    {
    //Initialize the backend resources
    InitializeL();
    }


