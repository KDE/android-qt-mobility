/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt 4.7
import QtQuickTest 1.0
import QtMobility.location 1.2

Item {
    id: masterItem
    width: 200
    height: 350
    // General-purpose elements for the test:
    Plugin { id: testPlugin; name : "nokia"; PluginParameter {name: "mapping.host"; value: "for.nonexistent"}}
    Coordinate{ id: mapDefaultCenter; latitude: 10; longitude: 30}

    // This model results in 7 landmarks
    LandmarkModel {
        id: landmarkModelAll
        autoUpdate: true
        //onLandmarksChanged: console.log('all landmarks changed, count: ' + count)
    }
    // This model results in 2 landmarks
    LandmarkModel {
        id: landmarkModelNearMe
        autoUpdate: true
        //onLandmarksChanged: console.log('nearme landmarks changed, count: ' + count)
        filter: proximityFilter
    }
    LandmarkProximityFilter {
        id: proximityFilter
        center: mapDefaultCenter
        radius: 6000000
    }

    MapCircle {
        id: externalCircle
        radius: 2000000
        center: mapDefaultCenter
    }

    Map {
        id: mapWithPlugin; center: mapDefaultCenter; plugin: testPlugin;
        anchors.fill: parent; size.width: parent.width; size.height: parent.height; zoomLevel: 2
        MapCircle {
            id: internalCircle
            radius: 2000000
            center: mapDefaultCenter
        }
        MapObjectView {
            id: theObjectView
            model: landmarkModelAll
            delegate: Component {
                MapCircle {
                    radius: 1500000
                    center: landmark.coordinate
                }
            }
        }
    }
    Map {
        id: mapWithoutPlugin; center: mapDefaultCenter;
        anchors.fill: parent; size.width: parent.width; size.height: parent.height; zoomLevel: 2
        MapCircle {
            id: internalCircle2
            radius: 2000000
            center: mapDefaultCenter
        }
        MapObjectView {
            id: theObjectView2
            model: landmarkModelAll
            delegate: Component {
                MapCircle {
                    radius: 1500000
                    center: landmark.coordinate
                }
            }
        }
    }

    TestCase {
        name: "MapObjectView"
        function test_a_add_and_remove() {
            // Basic adding and removing of static object
            compare(mapWithPlugin.testGetDeclarativeMapObjectCount(), 1)
            mapWithPlugin.addMapObject(internalCircle)
            compare(mapWithPlugin.testGetDeclarativeMapObjectCount(), 1)
            mapWithPlugin.removeMapObject(internalCircle)
            compare(mapWithPlugin.testGetDeclarativeMapObjectCount(), 0)
            mapWithPlugin.removeMapObject(internalCircle)
            compare(mapWithPlugin.testGetDeclarativeMapObjectCount(), 0)
            // Basic adding and removing of dynamic object
            var dynamicCircle = Qt.createQmlObject( "import Qt 4.7; import QtMobility.location 1.2; MapCircle {radius: 4000; center: mapDefaultCenter}", mapWithPlugin, "");
            mapWithPlugin.addMapObject(dynamicCircle)
            compare(mapWithPlugin.testGetDeclarativeMapObjectCount(), 1)
            mapWithPlugin.removeMapObject(dynamicCircle)
            compare(mapWithPlugin.testGetDeclarativeMapObjectCount(), 0)
            mapWithPlugin.removeMapObject(dynamicCircle)
            compare(mapWithPlugin.testGetDeclarativeMapObjectCount(), 0)
        }
        SignalSpy {id: allCountSpy; target: landmarkModelAll; signalName: "countChanged"}
        SignalSpy {id: nearMeCountSpy; target: landmarkModelNearMe; signalName: "countChanged"}
        function test_b_model_change() {
            // Change the model of an MapObjectView on the fly
            // and verify that object counts change accordingly.
            compare(mapWithPlugin.testGetDeclarativeMapObjectCount(), 0)
            landmarkModelAll.setDbFileName("landmarks.db")
            landmarkModelNearMe.setDbFileName("landmarks.db")
            tryCompare(allCountSpy, "count", 1, 1000)
            tryCompare(nearMeCountSpy, "count", 1, 1000)
            compare(landmarkModelAll.count, 7)
            compare(landmarkModelNearMe.count, 2)
            compare(mapWithPlugin.testGetDeclarativeMapObjectCount(), 7)
            theObjectView.model = landmarkModelNearMe
            compare(mapWithPlugin.testGetDeclarativeMapObjectCount(), 2)
            theObjectView.model = landmarkModelAll
            compare(mapWithPlugin.testGetDeclarativeMapObjectCount(), 7)
        }
        SignalSpy {id: pluginChangedSpy; target: mapWithoutPlugin; signalName: "pluginChanged"}
        function test_c_plugin_set_later() {
            compare(mapWithoutPlugin.testGetDeclarativeMapObjectCount(), 0)
            mapWithoutPlugin.plugin = testPlugin
            tryCompare(pluginChangedSpy, "count", 1, 1000)
            compare(mapWithoutPlugin.testGetDeclarativeMapObjectCount(), 8) // 7 + 1
        }
    }
}
