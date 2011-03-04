/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Mobility Components.
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

import Qt 4.7
import QtMobility.location 1.2

Item {
    width: 500
    height: 500

    focus : true

    Map {
        id: map
        z : 1
        plugin : Plugin {
                            name : "nokia"
                        }
        size.width: parent.width
        size.height: parent.height
        zoomLevel: 7
        center: Coordinate {
                    latitude: -27
                    longitude: 153
                }


            MapCircle {
                center : Coordinate {
                    //latitude: 0
                    //longitude: 0
                                    latitude : -27
                                    longitude : 153
                                    }
                color : "red"
                radius : 1000.0
                MapMouseArea {
                    onClicked : { console.log('clicked in circle') }
                    onDoubleClicked : { console.log('double clicked in circle') }
                    onPressed: {console.log('pressed in circle') }
                    onReleased: { console.log('released in circle') }
                }
            }

/*
        MapMouseArea {
            onClicked : { 
                console.log('clicked in map') 
                mouse.accepted = false
            }
            onDoubleClicked : { 
                console.log('double clicked in map') 
                mouse.accepted = false
            }
            onPressed: { 
                console.log('pressed in map') 
                mouse.accepted = false
            }
            onReleased: { 
                console.log('released in map') 
                mouse.accepted = false
            }
        }
*/
    }

    MouseArea {

        z : 0
        anchors.fill : parent

        property bool mouseDown : false
        property int lastX : -1
        property int lastY : -1

        hoverEnabled : true
        onPressed : {
            mouseDown = true 
            lastX = mouse.x
            lastY = mouse.y
        }
        onReleased : { 
            mouseDown = false 
            lastX = -1
            lastY = -1
        }
        onPositionChanged: {
            if (mouseDown) {
                var dx = mouse.x - lastX
                var dy = mouse.y - lastY
                map.pan(-dx, -dy)
                lastX = mouse.x
                lastY = mouse.y
            }
        }
        onDoubleClicked: {
            map.center = map.toCoordinate(Qt.point(mouse.x, mouse.y))
            map.zoomLevel += 1
        }
    }

    Keys.onPressed: {
        if (event.key == Qt.Key_Plus) {
            map.zoomLevel += 1
        } else if (event.key == Qt.Key_Minus) {
            map.zoomLevel -= 1
        } else if (event.key == Qt.Key_T) {
            if (map.mapType == Map.StreetMap) {
                map.mapType = Map.SatelliteMapDay
            } else if (map.mapType == Map.SatelliteMapDay) {
                map.mapType = Map.StreetMap
            }
        }
    }
}
