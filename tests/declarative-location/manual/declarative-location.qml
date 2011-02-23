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

import "content" as Content
import Qt 4.7
import QtMobility.location 1.1

Rectangle {
    id: page
    width: 350
    height: 400
    color: "olive"
    
    Text {
        id: title
        text: "Simple position test app"
        font {pointSize: 12; bold: true}
    }
    PositionSource {
        id: positionSource
        updateInterval: 1000
	nmeaSource: "nmealog.txt"
    }
    Column {
        id: data
        anchors {top: title.bottom; left: title.left}

        Text {text: "<==== PositionSource ====>"}
        Text {text: "positioningMethod: "  + printableMethod(positionSource.positioningMethod)}
        Text {text: "nmeaSource: "         + positionSource.nmeaSource}
        Text {text: "updateInterval: "     + positionSource.updateInterval}
        Text {text: "active: "     + positionSource.active}
        Text {text: "<==== Position ====>"}
        Text {text: "latitude: "   + positionSource.position.coordinate.latitude}
        Text {text: "longitude: "   + positionSource.position.coordinate.longitude}
        Text {text: "altitude: "   + positionSource.position.coordinate.altitude}
	Text {text: "horizontalAccuracy: "   + positionSource.position.horizontalAccuracy}
	Text {text: "verticalAccuracy: "   + positionSource.position.verticalAccuracy}
        Text {text: "speed: " + positionSource.position.speed}
        Text {text: "timestamp: "  + positionSource.position.timestamp}
        Text {text: "altitudeValid: "  + positionSource.position.altitudeValid}
        Text {text: "longitudeValid: "  + positionSource.position.longitudeValid}
        Text {text: "latitudeValid: "  + positionSource.position.latitudeValid}
        Text {text: "speedValid: "     + positionSource.position.speedValid}
        Text {text: "horizontalAccuracyValid: "  + positionSource.position.horizontalAccuracyValid}
	Text {text: "verticalAccuracyValid: "  + positionSource.position.verticalAccuracyValid}
	
    }

    function printableMethod(method) {
        if (method == PositionSource.SatellitePositioningMethod)
            return "Satellite";
        else if (method == PositionSource.NoPositioningMethod)
            return "Not available"
        else if (method == PositionSource.NonSatellitePositioningMethod)
            return "Non-satellite"
        else if (method == PositionSource.AllPositioningMethods)
            return "All/multiple"
        return "source error";
    }

    Grid {
        id: actionRectangles
        x: 4;
        width: 150
        anchors { topMargin: 5; top: data.bottom}
        rows: 3; columns: 1; spacing: anchors.bottomMargin;
        Content.Cell {action: "update()"; onClicked: doAction(action)}
        Content.Cell {action: "start()"; onClicked: doAction(action)}
        Content.Cell {action: "stop()";  onClicked: doAction(action)}
    }
    function doAction(action) {
        if (action == "update()") {
            positionSource.update()
        }
        else if (action == "start()" ) {
            positionSource.start();
        }
        else if (action == "stop()" ) {
            positionSource.stop();
        }
    }
}
