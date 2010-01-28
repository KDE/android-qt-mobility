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

#ifndef S60CAMERAFOCUSCONTROL_H
#define S60CAMERAFOCUSCONTROL_H

#include <QtCore/qobject.h>
#include "qcamerafocuscontrol.h"
#include "s60camerasettings.h"

QTM_USE_NAMESPACE

class S60CameraService;
class S60CameraSession;

class S60CameraFocusControl : public QCameraFocusControl
{
    Q_OBJECT
public:
    S60CameraFocusControl(QObject *parent = 0);
    S60CameraFocusControl(QObject *session, QObject *parent = 0);
    ~S60CameraFocusControl();

    QCamera::FocusMode focusMode() const;
    void setFocusMode(QCamera::FocusMode mode);
    QCamera::FocusModes supportedFocusModes() const;
    QCamera::FocusStatus focusStatus() const;

    bool macroFocusingEnabled() const;
    bool isMacroFocusingSupported() const;
    void setMacroFocusingEnabled(bool);

    qreal maximumOpticalZoom() const;
    qreal maximumDigitalZoom() const;
    
    qreal opticalZoom() const;
    qreal digitalZoom() const;

    void zoomTo(qreal optical, qreal digital);

public Q_SLOTS:

    void startFocusing();
    void cancelFocusing();
    void focusChanged(QCamera::FocusStatus status);
   
private:
    S60CameraSession *m_session;
    S60CameraService *m_service;
    
    S60CameraSettings *m_advancedSettings;
    
    bool m_focusLocked;
    qreal m_digitalZoomValue;
    qreal m_opticalZoomValue;
    bool m_macroFocusingEnabled;
    QCamera::FocusMode m_focusMode;
    QCamera::FocusStatus m_focusStatus;
    qreal m_maxZoom;
    qreal m_maxDigitalZoom;
    QCamera::Error m_error;
};

#endif
