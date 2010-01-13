/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef QGSTREAMERCAMERACONTROL_H
#define QGSTREAMERCAMERACONTROL_H

#include <QHash>
#include <experimental/qcameracontrol.h>
#include "qgstreamercapturesession.h"

QTM_USE_NAMESPACE
QT_USE_NAMESPACE

class QGstreamerCameraControl : public QCameraControl, public QGstreamerVideoInput
{
    Q_OBJECT
public:
    QGstreamerCameraControl( QGstreamerCaptureSession *session );
    virtual ~QGstreamerCameraControl();

    bool isValid() const { return true; }

    QCamera::State state() const;

    GstElement *buildElement();

    void start();
    void stop();

    QList<qreal> supportedFrameRates(const QSize &frameSize = QSize()) const;
    QList<QSize> supportedResolutions(qreal frameRate = -1) const;

    QCamera::CaptureMode captureMode() const { return m_captureMode; }
    void setCaptureMode(QCamera::CaptureMode mode)
    {
        if (m_captureMode != mode) {
            m_captureMode = mode;
            emit captureModeChanged(mode);
        }
    }

    QCamera::CaptureModes supportedCaptureModes() const
    {
        return QCamera::CaptureStillImage | QCamera::CaptureVideo;
    }

public slots:
    void setDevice(const QString &device);

private slots:
    void updateState();

private:
    void updateSupportedResolutions(const QString &device);

    QList<qreal> m_frameRates;
    QList<QSize> m_resolutions;

    QHash<QSize, QSet<int> > m_ratesByResolution;

    QCamera::CaptureMode m_captureMode;

    QGstreamerCaptureSession *m_session;
    QByteArray m_device;
    QCamera::State m_state;    
};

#endif // QGSTREAMERCAMERACONTROL_H
