/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MAEMO6SENSORBASE_H
#define MAEMO6SENSORBASE_H

#include <qsensorbackend.h>
#include <sensormanagerinterface.h>
#include <abstractsensor_i.h>

QTM_USE_NAMESPACE

        class maemo6sensorbase : public QSensorBackend
{
public:
    maemo6sensorbase(QSensor *sensor);
    virtual ~maemo6sensorbase();


protected:
    virtual void start();
    virtual void stop();
    AbstractSensorChannelInterface* m_sensorInterface;

    static const float GRAVITY_EARTH;
    static const float GRAVITY_EARTH_THOUSANDTH;    //for speed

    void setRanges(qreal correctionFactor=1);

    template<typename T>
    void initSensor(QString sensorName, bool &initDone)
    {

        if (!initDone) {
            m_remoteSensorManager->loadPlugin(sensorName);
            m_remoteSensorManager->registerSensorInterface<T>(sensorName);
        }
        m_sensorInterface = T::controlInterface(sensorName);
        if (!m_sensorInterface) {
            m_sensorInterface = const_cast<T*>(T::listenInterface(sensorName));
        }

        initDone = true;


        //metadata
        int l = m_sensorInterface->getAvailableIntervals().size();


        for (int i=0; i<l; i++){
            qreal intervalMax = ((DataRange)(m_sensorInterface->getAvailableIntervals().at(i))).max;
            qreal intervalMin =((DataRange)(m_sensorInterface->getAvailableIntervals().at(i))).min;


            if (intervalMin==0 && intervalMax==0){
                // 0 interval has different meanings in e.g. magge/acce
                // magge -> best-effort
                // acce -> lowest possible
                // in Qt API setting 0 means default
                continue;
            }



            qreal rateMin = intervalMax<1 ? 1 : 1/intervalMax * 1000;
            rateMin = rateMin<1 ? 1 : rateMin;

            intervalMin = intervalMin<1 ? 10: intervalMin;     // do not divide with 0
            qreal rateMax = 1/intervalMin * 1000;



            //            qreal rateMax = (intervalMin<1) ? rateMin : 1/intervalMin * 1000; // TODO: replace the two lines above with this one once sensord does provide interval>0
            addDataRate(rateMin, rateMax);
        }

        if (sensorName=="alssensor") return;                // SensorFW returns lux values, plugin enumerated values
        if (sensorName=="accelerometersensor") return;      // SensorFW returns milliGs, plugin m/s^2
        if (sensorName=="magnetometersensor") return;       // SensorFW returns nanoTeslas, plugin Teslas

        setDescription(m_sensorInterface->property("description").toString());

        if (sensorName=="tapsensor") return;
        setRanges();
    };

private:
    static SensorManagerInterface* m_remoteSensorManager;

};

#endif
