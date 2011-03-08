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

#ifndef QGEOSATELLITEINFOSOURCE_ANDROID_P_H
#define QGEOSATELLITEINFOSOURCE_ANDROID_P_H

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

#include <QTimer>
#include "qgeosatelliteinfosource.h"
#include "qgeosatelliteinfo.h"
#include "jni_android.h"

QTM_BEGIN_NAMESPACE

class QGeoSatelliteInfoSourceAndroid : public QGeoSatelliteInfoSource,
        public QtLocationJni::SatelliteInfoAndroidListner
{
    Q_OBJECT
public:
    explicit QGeoSatelliteInfoSourceAndroid(QObject *parent = 0);
    virtual ~QGeoSatelliteInfoSourceAndroid();

private:
    QTimer *m_requestTimer;

    enum SatelliteInfoState {
        Undefined = 0,
        Started = 1,
        Stopped = 2,
        RequestActive = 4,
    };
    int m_satelliteInfoState;

public slots:
    virtual void onSatelliteInfoAvailable(QList<QtLocationJni::SatelliteInfo> &satInfoList);
    virtual void startUpdates();
    virtual void stopUpdates();
    virtual void requestUpdate(int timeout);

signals:
    void satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &satellites);
    void satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &satellites);
    void requestTimeout();

private slots:
    void requestTimeoutElapsed();

private:
    Q_DISABLE_COPY(QGeoSatelliteInfoSourceAndroid)
};

QTM_END_NAMESPACE

#endif // QGEOSATELLITEINFOSOURCE_ANDROID_P_H
