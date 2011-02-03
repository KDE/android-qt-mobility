/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef S60CAMERAVIEWFINDERENGINE_H
#define S60CAMERAVIEWFINDERENGINE_H

#include <QtCore/qsize.h>
#include <QtGui/qpixmap.h>

#include <qvideosurfaceformat.h>

#include "s60cameraengineobserver.h"

class CCameraEngine;
class S60CameraControl;
class QAbstractVideoSurface;

// For DirectScreen ViewFinder
class RWsSession;
class CWsScreenDevice;
class RWindowBase;
class QDesktopWidget;

/*
 * Class implementing video output selection for the viewfinder and the handler of
 * all common viewfinder operations.
 */
class S60CameraViewfinderEngine : public QObject, public MCameraViewfinderObserver
{
    Q_OBJECT

public: // Constructor & Destructor

    S60CameraViewfinderEngine(QObject *parent, CCameraEngine *engine);
    virtual ~S60CameraViewfinderEngine();

public: // Methods

    // Setting Viewfinder Output
    void setVideoWidgetControl(QObject *viewfinderOutput);
    void setVideoRendererControl(QObject *viewfinderOutput);
    void setVideoWindowControl(QObject *viewfinderOutput);
    void releaseCurrentControl();

    // Controls
    void startViewfinder(const bool internalStart = false);
    void stopViewfinder(const bool internalStop = false);

    // Start using new CameraEngine
    void setNewCameraEngine(CCameraEngine *engine);

protected: // MCameraViewfinderObserver

    void MceoViewFinderFrameReady(CFbsBitmap& aFrame);

private: // Internal operation

    void checkAndRotateCamera();

Q_SIGNALS:

    void error(int error, const QString &errorString);
    void viewFinderFrameReady(const QPixmap &pixmap);

private Q_SLOTS:

    void resetViewfinderSize(QSize size);
    void resetViewfinderDisplay();
    void viewFinderBitmapReady(const QPixmap &pixmap);
    void handleVisibilityChange(const bool isVisible);
    void handleDesktopResize(int screen);

private: // Enums

    /*
     * Defines whether viewfinder output backend control is of type
     * QVideoWidgetControl, QVideoRendererControl or QVideoWindowControl
     */
    enum ViewfinderOutputType {
        OutputTypeNotSet = 0,   // No viewfinder output connected
        OutputTypeVideoWidget,  // Using QVideoWidget
        OutputTypeRenderer,     // Using QGraphicsVideoItem
        OutputTypeVideoWindow   // Using QVideoWindow
    };

    /*
     * Defines the internal state of the viewfinder. ViewFinder will only be
     * started if output is connected to Camera and Camera is started (and
     * ViewFinder widget is visible in case of QVideoWidget).
     */
    enum ViewFinderState {
        EVFNotConnectedNotStarted = 0,      // 0 - No output connected, viewfinder is not started
        EVFNotConnectedIsStarted,           // 1 - No output connected, viewfinder is started
        EVFIsConnectedNotStarted,           // 2 - Output is connected, viewfinder is not started
        EVFIsConnectedIsStartedNotVisible,  // 3 - Output is connected, viewfinder is started but is not visible
        EVFIsConnectedIsStartedIsVisible    // 4 - Output is connected, viewfinder is started and is visible
    };

    /*
     * The native type of ViewFinder. DirectScreen ViewFinder is used with
     * QVideoWidget if support for it is available in the platform. For
     * QGraphicsVideoItem Bitmap ViewFinder is always used.
     */
    enum NativeViewFinderType {
        EBitmapViewFinder = 0,
        EDirectScreenViewFinder
    };

private: // Data

    CCameraEngine           *m_cameraEngine;
    S60CameraControl        *m_cameraControl;
    QObject                 *m_viewfinderOutput;
    QAbstractVideoSurface   *m_viewfinderSurface; // Used only by QVideoRendererControl
    RWsSession              &m_wsSession;
    CWsScreenDevice         &m_screenDevice;
    RWindowBase             *m_window;
    QDesktopWidget          *m_desktopWidget;
    ViewFinderState         m_vfState;
    QSize                   m_viewfinderSize;
    // Actual viewfinder size, which may differ from requested
    // (m_viewfinderSize), if the size/aspect ratio was not supported.
    QSize                   m_actualViewFinderSize;
    ViewfinderOutputType    m_viewfinderType;
    NativeViewFinderType    m_viewfinderNativeType;
    QVideoSurfaceFormat     m_surfaceFormat; // Used only by QVideoRendererControl
    bool                    m_isViewFinderVisible;
    bool                    m_uiLandscape; // For detecting UI rotation
    int                     m_vfErrorsSignalled;
};

#endif // S60CAMERAVIEWFINDERENGINE_H
