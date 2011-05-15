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

#include "qgeoareamonitor_android_p.h"
#include <qmath.h>
#include <cmath>
#include <QDebug>
QTM_BEGIN_NAMESPACE

#define UPDATE_INTERVAL_5S  6000
#define TORADIANS           (double)3.141592653589793238/(double)180

QGeoAreaMonitorAndroid::QGeoAreaMonitorAndroid(QObject *parent) : QGeoAreaMonitor(parent)
{
    m_insideArea = false;
    m_location = QGeoPositionInfoSource::createDefaultSource(this);
    if (m_location) {
        m_location->setUpdateInterval(UPDATE_INTERVAL_5S);
        connect(m_location, SIGNAL(positionUpdated(QGeoPositionInfo)),
                this, SLOT(positionUpdated(QGeoPositionInfo)));
        m_location->startUpdates();
    }
}

QGeoAreaMonitorAndroid::~QGeoAreaMonitorAndroid()
{
    if (m_location)
    {
        m_location->stopUpdates();
        delete m_location;
    }
}

void QGeoAreaMonitorAndroid::setCenter(const QGeoCoordinate& coordinate)
{
    if (coordinate.isValid())
        QGeoAreaMonitor::setCenter(coordinate);
}

void QGeoAreaMonitorAndroid::setRadius(qreal radius)
{
    QGeoAreaMonitor::setRadius(radius);
}


qreal QGeoAreaMonitorAndroid::getDistance(double lat1, double lon1, double lat2, double lon2)
{
    //adopted by android
    // Based on http://www.ngs.noaa.gov/PUBS_LIB/inverse.pdf
    // using the "Inverse Formula" (section 4)

    //    a, b = major & minor semiaxes of the ellipsoid
    //    f = flattening (a−b)/a
    //    φ1, φ2 = geodetic latitude
    //    L = difference in longitude
    //    U1 = atan((1−f).tanφ1) (U is ‘reduced latitude’)
    //    U2 = atan((1−f).tanφ2)
    //    λ = L (first approximation)
    //    iterate until change in λ is negligible (e.g. 10-12 ≈ 0.06mm) {
    //            sinσ = √[ (cosU2.sinλ)² + (cosU1.sinU2 − sinU1.cosU2.cosλ)² ] 	(14)
    //            cosσ = sinU1.sinU2 + cosU1.cosU2.cosλ 	(15)
    //            σ = atan2(sinσ, cosσ) 	(16)
    //            sinα = cosU1.cosU2.sinλ / sinσ 	(17)
    //            cos²α = 1 − sin²α (trig identity; §6)
    //            cos2σm = cosσ − 2.sinU1.sinU2/cos²α 	(18)
    //            C = f/16.cos²α.[4+f.(4−3.cos²α)] 	(10)
    //            λ′ = L + (1−C).f.sinα.{σ+C.sinσ.[cos2σm+C.cosσ.(−1+2.cos²2σm)]} 	(11)
    //    }
    //    u² = cos²α.(a²−b²)/b²
    //    A = 1+u²/16384.{4096+u².[−768+u².(320−175.u²)]} 	(3)
    //    B = u²/1024.{256+u².[−128+u².(74−47.u²)]} 	(4)
    //    Δσ = B.sinσ.{cos2σm+B/4.[cosσ.(−1+2.cos²2σm) − B/6.cos2σm.(−3+4.sin²σ).(−3+4.cos²2σm)]} 	(6)
    //    s = b.A.(σ−Δσ) 	(19)
    //    α1 = atan2(cosU2.sinλ, cosU1.sinU2 − sinU1.cosU2.cosλ) 	(20)
    //    α2 = atan2(cosU1.sinλ, −sinU1.cosU2 + cosU1.sinU2.cosλ) 	(21)

    int MAXITERS = 20;
    // Convert lat/long to radians
    lat1 *= TORADIANS;
    lat2 *= TORADIANS;
    lon1 *= TORADIANS;
    lon2 *= TORADIANS;
    double a = 6378137.0; // WGS84 major axis
    double b = 6356752.3142; // WGS84 semi-major axis
    double f = (a - b) / a;
    double aSqMinusBSqOverBSq = (a * a - b * b) / (b * b);

    double L = lon2 - lon1;
    double A = 0.0;
    double U1 = ::atan((1.0 - f) * ::tan(lat1));
    double U2 = ::atan((1.0 - f) * ::tan(lat2));

    double cosU1 = ::cos(U1);
    double cosU2 = ::cos(U2);
    double sinU1 = ::sin(U1);
    double sinU2 = ::sin(U2);
    double cosU1cosU2 = cosU1 * cosU2;
    double sinU1sinU2 = sinU1 * sinU2;

    double sigma = 0.0;
    double deltaSigma = 0.0;
    double cosSqAlpha = 0.0;
    double cos2SM = 0.0;
    double cosSigma = 0.0;
    double sinSigma = 0.0;
    double cosLambda = 0.0;
    double sinLambda = 0.0;

    double lambda = L; // initial guess
    for (int iter = 0; iter < MAXITERS; iter++) {
        double lambdaOrig = lambda;
        cosLambda = ::cos(lambda);
        sinLambda = ::sin(lambda);
        double t1 = cosU2 * sinLambda;
        double t2 = cosU1 * sinU2 - sinU1 * cosU2 * cosLambda;
        double sinSqSigma = t1 * t1 + t2 * t2; // (14)
        sinSigma = ::sqrt(sinSqSigma);
        cosSigma = sinU1sinU2 + cosU1cosU2 * cosLambda; // (15)
        sigma = ::atan2(sinSigma, cosSigma); // (16)
        double sinAlpha = (sinSigma == 0) ? 0.0 :
                          cosU1cosU2 * sinLambda / sinSigma; // (17)
        cosSqAlpha = 1.0 - sinAlpha * sinAlpha;
        cos2SM = (cosSqAlpha == 0) ? 0.0 :
                 cosSigma - 2.0 * sinU1sinU2 / cosSqAlpha; // (18)

        double uSquared = cosSqAlpha * aSqMinusBSqOverBSq; // defn
        A = 1 + (uSquared / 16384.0) * // (3)
            (4096.0 + uSquared *
             (-768 + uSquared * (320.0 - 175.0 * uSquared)));
        double B = (uSquared / 1024.0) * // (4)
                   (256.0 + uSquared *
                    (-128.0 + uSquared * (74.0 - 47.0 * uSquared)));
        double C = (f / 16.0) *
                   cosSqAlpha *
                   (4.0 + f * (4.0 - 3.0 * cosSqAlpha)); // (10)
        double cos2SMSq = cos2SM * cos2SM;
        deltaSigma = B * sinSigma * // (6)
                     (cos2SM + (B / 4.0) *
                      (cosSigma * (-1.0 + 2.0 * cos2SMSq) -
                       (B / 6.0) * cos2SM *
                       (-3.0 + 4.0 * sinSigma * sinSigma) *
                       (-3.0 + 4.0 * cos2SMSq)));

        lambda = L +
                 (1.0 - C) * f * sinAlpha *
                 (sigma + C * sinSigma *
                  (cos2SM + C * cosSigma *
                   (-1.0 + 2.0 * cos2SM * cos2SM))); // (11)

        double delta = (lambda - lambdaOrig) / lambda;
        if (qAbs(delta) < 1.0e-12) {
            break;
        }
    }

    qreal distance = (qreal) (b * A * (sigma - deltaSigma));

    return distance;
}

void QGeoAreaMonitorAndroid::positionUpdated(const QGeoPositionInfo &info)
{
    QGeoCoordinate coordinate=info.coordinate();
    double lat1=coordinate.latitude();
    double lon1=coordinate.longitude();
    double lat2=QGeoAreaMonitor::center().latitude();
    double lon2=QGeoAreaMonitor::center().longitude();
    qreal distance=getDistance(lat1,lon1,lat2,lon2);

    if (distance <= QGeoAreaMonitor::radius())
    {
        if (!m_insideArea)
            emit areaEntered(info);
        m_insideArea = true;
    }
    else if (m_insideArea)
    {
        emit areaExited(info);
        m_insideArea = false;
    }
}

#include "moc_qgeoareamonitor_android_p.cpp"
QTM_END_NAMESPACE
