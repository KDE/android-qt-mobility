/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore>
#include <qambientlightsensor.h>

QTM_USE_NAMESPACE

class AmbientLightFilter : public QAmbientLightFilter
{
public:
    qtimestamp stamp;
    bool filter(QAmbientLightReading *reading)
    {
        int diff = ( reading->timestamp() - stamp );
        stamp = reading->timestamp();
        QString output;
        switch (reading->lightLevel()) {
            case QAmbientLightReading::Dark:      output = "Dark    ";  break;
            case QAmbientLightReading::Twilight:  output = "Twilight";  break;
            case QAmbientLightReading::Light:     output = "Light   ";  break;
            case QAmbientLightReading::Bright:    output = "Bright  ";  break;
            case QAmbientLightReading::Sunny:     output = "Sunny   ";  break;
            case QAmbientLightReading::Undefined: output = "Undefined"; break;
            default: output = "Invalid enum value";
        }
        QTextStream out(stdout);
        out << "Ambient light level: " << output
            << QString(" (%1 ms since last, %2 Hz)").arg(diff / 1000, 5).arg( 1000000.0 / diff, 3, 'f', 1) << endl;
        return false; // don't store the reading in the sensor
    }
};

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QStringList args = app.arguments();
    int rate_place = args.indexOf("-r");
    int rate_val = 0;
    if (rate_place != -1)
        rate_val = args.at(rate_place + 1).toInt();
    QAmbientLightSensor sensor;
    if (rate_val > 0) {
        sensor.setDataRate(rate_val);
    }
    AmbientLightFilter filter;
    sensor.addFilter(&filter);
    sensor.start();
    if (!sensor.isActive()) {
        qWarning("Ambientlightsensor didn't start!");
        return 1;
    }

    return app.exec();
}
