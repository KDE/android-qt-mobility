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

#ifndef QGEOAREAMONITOR_ANDROID_P_H
#define QGEOAREAMONITOR_ANDROID_P_H
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

#include "qgeoareamonitor.h"
#include "qgeopositioninfosource.h"

QTM_BEGIN_NAMESPACE

 /**
 *  QGeoAreaMonitorAndroid
 *
 */
class QGeoAreaMonitorAndroid : public QGeoAreaMonitor
{
    Q_OBJECT

public :
QGeoAreaMonitorAndroid(QObject *parent = 0);
~QGeoAreaMonitorAndroid();
void setCenter(const QGeoCoordinate &coordinate);
void setRadius(qreal radius);

private :
        qreal getDistance (double x1,double y1,double x2,double y2);

private slots:
void positionUpdated(const QGeoPositionInfo &info);

private:
bool m_insideArea;
QGeoPositionInfoSource *m_location;

};

QTM_END_NAMESPACE
#endif // QGEOAREAMONITOR_ANDROID_P_H
