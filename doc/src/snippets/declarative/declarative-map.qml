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
    id: page
    width: 320
    height: 480
    focus: true

    PositionSource {
        id: myPositionSource
        active: true
        updateInterval: 2000
        nmeaSource: 'nmealog.txt'
        onPositionChanged: console.log("Position changed in PositionSource")
    }
    LandmarkBoxFilter {
        id: boxFilter
        topLeft: map.toCoordinate(Qt.point(0,0))
        bottomRight: map.toCoordinate(Qt.point(page.width, page.height))
    }
    
     LandmarkProximityFilter {
         id: proximityFilter
         center: myPositionSource.position.coordinate
	 //center: Coordinate {latitude: -27; longitude: 153}
         radius: 500000
     }
       
    LandmarkModel {
        id: landmarkModelAll
        autoUpdate: true
        filter: boxFilter
        limit: 50
        onModelChanged: {
            console.log("All landmark model changed, landmark count: " + count)
        }
    }
    
    LandmarkModel {
        id: landmarkModelNear
        autoUpdate: true
        filter: proximityFilter
        limit: 50
        onModelChanged: {
            console.log("Near landmark model changed, landmark count: " + count)
        }
    }
    
    Rectangle {
        id: dataArea
        anchors.fill: parent
        color: "#343434"

//![Basic MapObjects and View on Map]
        Map {
            id: map
            plugin : Plugin {name : "nokia"}
            anchors.fill: parent
            size.width: parent.width
            size.height: parent.height
            zoomLevel: 10
            
            MapObjectView {
	        id: allLandmarks
		model: landmarkModelAll
		delegate: Component {
		    MapCircle {
			color: "green"
			radius: 1000
		        center: Coordinate {
	                    latitude: landmark.coordinate.latitude
		            longitude: landmark.coordinate.longitude 
	                }
	            }
	        }
	    }

	    MapCircle {
		id: myPosition
		color: "yellow"
		radius: 1000
		center: myPositionSource.position.coordinate
	    }
//![Basic MapObjects and View on Map]

	    MapObjectView {
	        id: landmarksNearMe
		model: landmarkModelNear
		delegate: Component {
		    MapGroup {
		        MapCircle {
	                    color: "red"
	                    radius: 1000
		            center: Coordinate {
	                        latitude: landmark.coordinate.latitude
		                longitude: landmark.coordinate.longitude
	                    }
	                }
	            }
		}
	    }
	    
            // center: Coordinate {latitude: -27; longitude: 153}
	    center: myPositionSource.position.coordinate
	    
            onZoomLevelChanged: {
                console.log("Zoom changed")
                updateFilters();
            }
            onCenterChanged: {
                console.log("Center changed")
                updateFilters();
            }
            function updateFilters () {
                // The pixel coordinates used are the map's pixel coordinates, not
                // the absolute pixel coordinates of the application. Hence e.g. (0,0) is always
                // the top left point
                var topLeftCoordinate = map.toCoordinate(Qt.point(0,0))
                var bottomRightCoordinate = map.toCoordinate(Qt.point(map.size.width, map.size.height))
                boxFilter.topLeft = topLeftCoordinate;
                boxFilter.bottomRight = bottomRightCoordinate;
            }
        } // map
    } // dataArea
} // page
