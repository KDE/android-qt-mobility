/*
Copyright (c) 2011 Elektrobit (EB), All rights reserved.
Contact: oss-devel@elektrobit.com

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are
met:
* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* Neither the name of the Elektrobit (EB) nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY Elektrobit (EB) ''AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Elektrobit (EB) BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <qgeopositioninfosource_android_p.h>
#include <jni_android.h>
#include <qnumeric.h>

QTM_BEGIN_NAMESPACE

QGeoPositionInfoSourceAndroid::QGeoPositionInfoSourceAndroid(QObject *parent)
            : QGeoPositionInfoSource(parent)
{
    // default values
    m_updateTimer = new QTimer(this);
    m_updateTimer->setSingleShot(true);
    connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(updateTimeoutElapsed()));

    m_requestTimer = new QTimer(this);
    m_requestTimer->setSingleShot(true);
    connect(m_requestTimer, SIGNAL(timeout()), this, SLOT(requestTimeoutElapsed()));

    m_positionInfoState = QGeoPositionInfoSourceAndroid::Undefined;
}

QGeoPositionInfoSourceAndroid::~QGeoPositionInfoSourceAndroid()
{
    delete m_requestTimer;
    delete m_updateTimer;
    QtLocationJni::disableUpdates (this);
}

void QGeoPositionInfoSourceAndroid::setUpdateInterval(int msec)
{
    int timerInterval = (msec < DEFAULT_UPDATE_INTERVAL) ? DEFAULT_UPDATE_INTERVAL : msec;
    QGeoPositionInfoSource::setUpdateInterval(timerInterval);
}

QGeoPositionInfo QGeoPositionInfoSourceAndroid::lastKnownPosition(bool fromSatellitePositioningMethodsOnly) const
{
    //needs furthur implementation
    double data[8];
    QtLocationJni::lastKnownPosition(data,sizeof(data),fromSatellitePositioningMethodsOnly);
    QGeoCoordinate geoCoordinate(data[0],data[1],data[2]);
    QDateTime timestamp=(geoCoordinate.latitude()!=0 ||geoCoordinate.longitude()!=0)
                        ?QDateTime::currentDateTime():QDateTime ();
    QGeoPositionInfo positionInfo(geoCoordinate,timestamp);
    positionInfo.setAttribute(QGeoPositionInfo::Direction,data[4]);
    positionInfo.setAttribute(QGeoPositionInfo::GroundSpeed,data[5]);
    //no vertical speed avaialable-that is why we are dont set it
    positionInfo.setAttribute(QGeoPositionInfo::MagneticVariation,data[6]);
    //horizontal and vertical accuracy have the same values
    positionInfo.setAttribute(QGeoPositionInfo::HorizontalAccuracy,data[7]);
    positionInfo.setAttribute(QGeoPositionInfo::VerticalAccuracy,data[7]);
    return positionInfo;
}

void QGeoPositionInfoSourceAndroid::setPreferredPositioningMethods(PositioningMethods methods)
{

    if(methods & supportedPositioningMethods())
    {
        setProvider((QtLocationJni::LocationProviders)(long)methods);
    }
    QGeoPositionInfoSource::setPreferredPositioningMethods(methods);
}

QGeoPositionInfoSource::PositioningMethods QGeoPositionInfoSourceAndroid::supportedPositioningMethods() const
{
    QGeoPositionInfoSource::PositioningMethods methods;
    methods=(QGeoPositionInfoSource::PositioningMethods)QtLocationJni::supportedPositioningMethods();
    return methods;
}


int QGeoPositionInfoSourceAndroid::minimumUpdateInterval() const
{
    return MINIMUM_UPDATE_INTERVAL;
}

// public slots:
void QGeoPositionInfoSourceAndroid::startUpdates()
{
    QtLocationJni::requestUpdates(updateInterval(),this);
    m_positionInfoState |=QGeoPositionInfoSourceAndroid::Started;
    m_positionInfoState &=~QGeoPositionInfoSourceAndroid::Stopped;
    m_positionInfoState |= QGeoPositionInfoSourceAndroid::StartUpdateActive;
    activateTimer();
}

void QGeoPositionInfoSourceAndroid::stopUpdates()
{
    m_positionInfoState &= ~QGeoPositionInfoSourceAndroid::StartUpdateActive;

    if (!(m_positionInfoState & QGeoPositionInfoSourceAndroid::RequestActive))
    {
        m_updateTimer->stop();
        QtLocationJni::disableUpdates(this);
    }

    m_positionInfoState &= ~QGeoPositionInfoSourceAndroid::Started;
    m_positionInfoState |= QGeoPositionInfoSourceAndroid::Stopped;
}

void QGeoPositionInfoSourceAndroid::requestUpdate(int timeout)
{
    int timeoutForRequest = 0;
    if (timeout < MINIMUM_UPDATE_INTERVAL)
    {
        if (m_positionInfoState & QGeoPositionInfoSourceAndroid::RequestActive)
            return;

        emit updateTimeout();
        return;
    }
    else
    {
        timeoutForRequest = timeout;
    }

    m_positionInfoState |= QGeoPositionInfoSourceAndroid::RequestActive;
    QtLocationJni::requestUpdates(updateInterval(),this);
    activateTimer();
    m_requestTimer->start(timeoutForRequest);
}

void QGeoPositionInfoSourceAndroid::updateTimeoutElapsed()
{
    QGeoPositionInfo position;
    position=m_lastUpdate;

    if (position.isValid())
    {
        if (m_positionInfoState & QGeoPositionInfoSourceAndroid::RequestActive)
        {
            m_requestTimer->stop();
            m_positionInfoState &= ~QGeoPositionInfoSourceAndroid::RequestActive;

            if (m_positionInfoState & QGeoPositionInfoSourceAndroid::Stopped
                ||!(m_positionInfoState & QGeoPositionInfoSourceAndroid::StartUpdateActive))
            {
                QtLocationJni::disableUpdates(this);
            }

        }
        emit positionUpdated(position);
    }
    else
    {
        emit updateTimeout();
    }
    activateTimer();
}

void QGeoPositionInfoSourceAndroid::activateTimer()
{
    if (m_positionInfoState & QGeoPositionInfoSourceAndroid::RequestActive)
    {
        m_updateTimer->start(MINIMUM_UPDATE_INTERVAL);
    }

    else if (m_positionInfoState & QGeoPositionInfoSourceAndroid::StartUpdateActive)
    {
        int timerInterval=updateInterval();
        m_updateTimer->start(timerInterval);
    }
    return;
}

void QGeoPositionInfoSourceAndroid::requestTimeoutElapsed()
{
    m_updateTimer->stop();
    m_positionInfoState &=~QGeoPositionInfoSourceAndroid::RequestActive;

    if (m_positionInfoState & QGeoPositionInfoSourceAndroid::Stopped
        ||!(m_positionInfoState & QGeoPositionInfoSourceAndroid::StartUpdateActive))
    {
        QtLocationJni::disableUpdates(this);
    }
    emit updateTimeout();

    activateTimer();
}

void QGeoPositionInfoSourceAndroid::onLocationChanged(double *data)
{
    QGeoCoordinate geoCoordinate(data[0],data[1],data[2]);
    QGeoPositionInfo positionInfo(geoCoordinate,QDateTime::currentDateTime());
    positionInfo.setAttribute(QGeoPositionInfo::Direction,data[4]);
    positionInfo.setAttribute(QGeoPositionInfo::GroundSpeed,data[5]);
    //no vertical speed avaialable
    positionInfo.setAttribute(QGeoPositionInfo::MagneticVariation,data[6]);
    //horizontal and vertical accuracy have the same values
    positionInfo.setAttribute(QGeoPositionInfo::HorizontalAccuracy,data[7]);
    positionInfo.setAttribute(QGeoPositionInfo::VerticalAccuracy,data[7]);
    m_lastUpdate=positionInfo;
}


#include "moc_qgeopositioninfosource_android_p.cpp"

QTM_END_NAMESPACE

