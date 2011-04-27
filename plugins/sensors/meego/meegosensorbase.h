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

#ifndef MEEGOSENSORBASE_H
#define MEEGOSENSORBASE_H

#include <qsensorbackend.h>
#include <sensormanagerinterface.h>
#include <abstractsensor_i.h>

QTM_USE_NAMESPACE

        class meegosensorbase : public QSensorBackend
{
public:
    meegosensorbase(QSensor *sensor);
    virtual ~meegosensorbase();


protected:
    virtual bool doConnect()=0;
    virtual void start();
    virtual void stop();

    static const char* const ALWAYS_ON;
    static const char* const BUFFER_SIZE;
    static const char* const MAX_BUFFER_SIZE;
    static const char* const EFFICIENT_BUFFER_SIZE;
    static const float GRAVITY_EARTH;
    static const float GRAVITY_EARTH_THOUSANDTH;    //for speed
    static const int KErrNotFound;
    static const int KErrInUse;
    static QStringList m_bufferingSensors;

    void setRanges(qreal correctionFactor=1);
    virtual QString sensorName() const=0;

    template<typename T>
    void initSensor(bool &initDone)
    {

        const QString name = sensorName();

        if (!initDone) {
            if (!m_remoteSensorManager->loadPlugin(name)){
                sensorError(KErrNotFound);
                return;
            }
            m_remoteSensorManager->registerSensorInterface<T>(name);
        }
        m_sensorInterface = T::controlInterface(name);
        if (!m_sensorInterface) {
            m_sensorInterface = const_cast<T*>(T::listenInterface(name));
        }
        if (!m_sensorInterface) {
            sensorError(KErrNotFound);
            return;
        }
        if (!m_sensorInterface) {
            sensorError(KErrNotFound);
            return;
        }

        initDone = true;
        
        //metadata
        QList<DataRange> intervals = m_sensorInterface->getAvailableIntervals();

        for (int i=0, l=intervals.size(); i<l; i++){
            qreal intervalMax = ((DataRange)(intervals.at(i))).max;
            qreal intervalMin =((DataRange)(intervals.at(i))).min;

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
            addDataRate(rateMin, rateMax);
        }

        //bufferSizes
        if (m_bufferingSensors.contains(sensor()->identifier())){

            IntegerRangeList sizes = m_sensorInterface->getAvailableBufferSizes();
            int l = sizes.size();
            for (int i=0; i<l; i++){
                int second = sizes.at(i).second;
                m_maxBufferSize = second>m_bufferSize? second:m_maxBufferSize;
            }
            m_maxBufferSize = m_maxBufferSize<0?1:m_maxBufferSize;
            //SensorFW guarantees to provide the most efficient size first
            //TODO: remove from comments
            //m_efficientBufferSize  = m_sensorInterface->hwBuffering()? (l>0?sizes.at(0).first:1) : 1;
        }
        else
            m_maxBufferSize = 1;

        sensor()->setProperty(MAX_BUFFER_SIZE, m_maxBufferSize);
        sensor()->setProperty(EFFICIENT_BUFFER_SIZE, m_efficientBufferSize);

        QString type = sensor()->type();
        if (type=="QAmbientLightSensor")    return;   // SensorFW returns lux values, plugin enumerated values
        if (type=="QIRProximitySensor")    return;    // SensorFW returns raw reflectance values, plugin % of max reflectance
        if (name=="accelerometersensor") return;      // SensorFW returns milliGs, plugin m/s^2
        if (name=="magnetometersensor") return;       // SensorFW returns nanoTeslas, plugin Teslas
        if (name=="gyroscopesensor") return;          // SensorFW returns DSPs, plugin milliDSPs


        setDescription(m_sensorInterface->description());

        if (name=="tapsensor") return;
        setRanges();
    };


    AbstractSensorChannelInterface* m_sensorInterface;
    int m_bufferSize;
    int bufferSize() const;
    virtual qreal correctionFactor() const;

private:

    static SensorManagerInterface* m_remoteSensorManager;
    int m_prevOutputRange;
    bool doConnectAfterCheck();
    int m_efficientBufferSize, m_maxBufferSize;

};

#endif
