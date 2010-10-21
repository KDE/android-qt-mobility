/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef CAMERABINCAPTURESERVICE_H
#define CAMERABINCAPTURESERVICE_H

#include <qmediaservice.h>

#include <gst/gst.h>
QT_BEGIN_NAMESPACE
class QAudioEndpointSelector;
class QVideoDeviceControl;
QT_END_NAMESPACE

class CameraBinSession;
class CameraBinControl;
class QGstreamerMessage;
class QGstreamerBusHelper;
class QGstreamerVideoRenderer;
class QGstreamerVideoOverlay;
class QGstreamerVideoWidgetControl;
class QGstreamerElementFactory;
class CameraBinMetaData;
class CameraBinImageCapture;
class CameraBinMetaData;

class CameraBinService : public QMediaService
{
    Q_OBJECT

public:
    CameraBinService(const QString &service, QObject *parent = 0);
    virtual ~CameraBinService();

    QMediaControl *requestControl(const char *name);
    void releaseControl(QMediaControl *);

    static bool isCameraBinAvailable();

private:
    void setAudioPreview(GstElement*);

    CameraBinSession *m_captureSession;
    CameraBinControl *m_cameraControl;    
    CameraBinMetaData *m_metaDataControl;

    QAudioEndpointSelector *m_audioInputEndpointSelector;
    QVideoDeviceControl *m_videoInputDevice;

    QMediaControl *m_videoOutput;

    QGstreamerVideoRenderer *m_videoRenderer;
    QMediaControl *m_videoWindow;
    QGstreamerVideoWidgetControl *m_videoWidgetControl;
    CameraBinImageCapture *m_imageCaptureControl;
};

#endif // CAMERABINCAPTURESERVICE_H
