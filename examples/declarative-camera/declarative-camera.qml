/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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
import Qt.multimedia 4.7

Rectangle {
    id : cameraUI
    width : 800
    height : 480
    color: "black"
    state: "PhotoCapture"

    states: [
        State {
            name: "PhotoCapture"
            StateChangeScript {
                script: {
                    camera.visible = true                    
                    stillControls.visible = true
                    photoPreview.visible = false
                }
            }
        },
        State {
            name: "PhotoPreview"
            StateChangeScript {
                script: {
                    //don't stop the camera to allow processing of pending images
                    camera.visible = false
                    stillControls.visible = false
                    photoPreview.visible = true
                }
            }
        }
    ]

    PhotoPreview {
        id : photoPreview
        anchors.fill : parent
        onClosed: cameraUI.state = "PhotoCapture"
        focus: visible

        Keys.onPressed : {
            //return to capture mode if the shutter button is touched
            if (event.key == Qt.Key_CameraFocus) {
                cameraUI.state = "PhotoCapture"
                event.accepted = true;
            }
        }
    }

    Camera {
        id: camera
        x : 0
        y : 0
        width : 640
        height : 480
        focus : visible //to receive focus and capture key events

        flashMode: stillControls.flashMode
        whiteBalanceMode: stillControls.whiteBalance
        exposureCompensation: stillControls.exposureCompensation

        onImageCaptured : {
            photoPreview.source = preview
            stillControls.previewAvailable = true
            cameraUI.state = "PhotoPreview"
        }
    }

    CaptureControls {
        id: stillControls
        anchors.fill: parent
        camera: camera
        onPreviewSelected: cameraUI.state = "PhotoPreview"
    }
}
