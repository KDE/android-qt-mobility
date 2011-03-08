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

#ifndef JNI_ANDROID_H
#define JNI_ANDROID_H
#include <jni.h>
#include <QList>
#include <QString>
#include <QtCore>

namespace QtLocationJni
{

    struct SatelliteInfo
    {
        int m_prnNumber;
        float m_signalStrength;
        float m_elevation;
        float m_azimuth;
    };

    enum LocationProviders
    {
        GPS=0x000000ff,
        OTHERS=0xffffff00,
        ALL=0xffffffff
        };

    class LocationAndroidListner {
    private:
        QString m_provider;
    public:
        void setProvider(LocationProviders provider);
        void callListner(const char * provider,double * data);
        LocationAndroidListner ();
        virtual ~LocationAndroidListner () {}
        virtual void onLocationChanged(double *data)=0;
    };

    class SatelliteInfoAndroidListner {
    public:
        SatelliteInfoAndroidListner () {}
        virtual ~SatelliteInfoAndroidListner () {}
        virtual void onSatelliteInfoAvailable(QList<SatelliteInfo> &satInfoList)=0;
    };

    void lastKnownPosition(double *lastKnownPositionData,int dataSize,
                           bool fromSatellitePositioningMethodsOnly);
    qint64 supportedPositioningMethods ();
    void requestUpdates (uint64_t minTime,LocationAndroidListner *);
    void disableUpdates (LocationAndroidListner * Locationlistner);
    void satelliteRequestupdates (SatelliteInfoAndroidListner * listner);
    void satelliteDisableupdates (SatelliteInfoAndroidListner * listner);

};

#endif // JNI_ANDROID_H
