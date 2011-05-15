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

#include <qgeosatelliteinfosource_android_p.h>
#include <qnumeric.h>
#include <qmath.h>


QTM_BEGIN_NAMESPACE

QGeoSatelliteInfoSourceAndroid::QGeoSatelliteInfoSourceAndroid(QObject *parent)
            : QGeoSatelliteInfoSource(parent)
{
    m_requestTimer = new QTimer(this);
    m_requestTimer->setSingleShot(true);
    connect(m_requestTimer, SIGNAL(timeout()), this, SLOT(requestTimeoutElapsed()));
    m_satelliteInfoState = QGeoSatelliteInfoSourceAndroid::Undefined;
}

QGeoSatelliteInfoSourceAndroid::~QGeoSatelliteInfoSourceAndroid()
{
    delete m_requestTimer;
    QtLocationJni::satelliteDisableupdates (this);
}

// public slots:
void QGeoSatelliteInfoSourceAndroid::startUpdates()
{
    QtLocationJni::satelliteRequestupdates(this);
    m_satelliteInfoState |=QGeoSatelliteInfoSourceAndroid::Started;
    m_satelliteInfoState &=~QGeoSatelliteInfoSourceAndroid::Stopped;
}

void QGeoSatelliteInfoSourceAndroid::stopUpdates()
{
    if (!(m_satelliteInfoState & QGeoSatelliteInfoSourceAndroid::RequestActive))
    {
        m_requestTimer->stop();
        QtLocationJni::satelliteDisableupdates(this);
    }

    m_satelliteInfoState &= ~QGeoSatelliteInfoSourceAndroid::Started;
    m_satelliteInfoState |= QGeoSatelliteInfoSourceAndroid::Stopped;
}

void QGeoSatelliteInfoSourceAndroid::requestUpdate(int timeout)
{
    m_satelliteInfoState |= QGeoSatelliteInfoSourceAndroid::RequestActive;
    QtLocationJni::satelliteRequestupdates(this);
    m_requestTimer->start(timeout);
}


void QGeoSatelliteInfoSourceAndroid::requestTimeoutElapsed()
{
    m_satelliteInfoState &=~QGeoSatelliteInfoSourceAndroid::RequestActive;

    if (m_satelliteInfoState & QGeoSatelliteInfoSourceAndroid::Stopped)
    {
        QtLocationJni::satelliteDisableupdates(this);
    }

    emit requestTimeout();
}

void QGeoSatelliteInfoSourceAndroid::onSatelliteInfoAvailable(QList<QtLocationJni::SatelliteInfo> &satInfoList)
{
    QtLocationJni::SatelliteInfo satInfo;
    QList<QGeoSatelliteInfo> qSatellites;

    if (m_satelliteInfoState & QGeoSatelliteInfoSourceAndroid::RequestActive)
    {
        m_requestTimer->stop();
        m_satelliteInfoState &= ~QGeoSatelliteInfoSourceAndroid::RequestActive;
        if(m_satelliteInfoState & QGeoSatelliteInfoSourceAndroid::Stopped)
        {
            QtLocationJni::satelliteDisableupdates(this);
        }
    }

    foreach(satInfo,satInfoList)
    {
        QGeoSatelliteInfo qSatellite;
        qSatellite.setPrnNumber(satInfo.m_prnNumber);
        //10 log10 (ratio of the powers)
        int signalStrength=(satInfo.m_signalStrength==0) ? 0:(int)(10 * ::log10(satInfo.m_signalStrength));
        qSatellite.setSignalStrength(signalStrength);
        qSatellite.setAttribute(QGeoSatelliteInfo::Elevation,satInfo.m_elevation);
        qSatellite.setAttribute(QGeoSatelliteInfo::Azimuth,satInfo.m_azimuth);
        qSatellites.append(qSatellite);
    }

    emit satellitesInUseUpdated((const QList<QGeoSatelliteInfo> &)qSatellites);
    emit satellitesInViewUpdated((const QList<QGeoSatelliteInfo> &)qSatellites);
}

#include"moc_qgeosatelliteinfosource_android_p.cpp"

QTM_END_NAMESPACE
