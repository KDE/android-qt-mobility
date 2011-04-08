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
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef DSVIDEOWIDGETCONTROL_H
#define DSVIDEOWIDGETCONTROL_H

#include <QtCore/qobject.h>
#include <QtGui>
#include <QtMultimedia/qvideoframe.h>
#include <QtMultimedia/qabstractvideosurface.h>
#include <QtMultimedia/qvideosurfaceformat.h>

#include <qvideowidgetcontrol.h>
#include "DsCameraControl.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class DSVideoWidgetSurface : public QAbstractVideoSurface
{
     Q_OBJECT
 public:
     DSVideoWidgetSurface(QLabel *pWidget, QObject *parent = 0);

     QList<QVideoFrame::PixelFormat> supportedPixelFormats(
             QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;

     bool present(const QVideoFrame &frame);

     QRect videoRect() const { return targetRect; }
     void updateVideoRect();

     void paint(QPainter *painter);
     void setImageFormat(QImage::Format fmt);

 private:
     QLabel *widget;
     QImage::Format imageFormat;
     QRect targetRect;
     QSize imageSize;
     QRect sourceRect;
     QPixmap* myPixmap;
 };

class DSVideoWidgetControl : public QVideoWidgetControl
{
    Q_OBJECT
	
    DSVideoWidgetSurface* surface;
public: // Constructor & Destructor
    
    DSVideoWidgetControl(DSCameraSession* session, QObject *parent = 0);
    virtual ~DSVideoWidgetControl();

public: // QVideoWidgetControl
    
    QWidget *videoWidget();

    // Aspect Ratio
    Qt::AspectRatioMode aspectRatioMode() const;
    void setAspectRatioMode(Qt::AspectRatioMode ratio);

    // Full Screen
    bool isFullScreen() const;
    void setFullScreen(bool fullScreen);

    // Brightness
    int brightness() const;
    void setBrightness(int brightness);

    // Contrast
    int contrast() const;
    void setContrast(int contrast);

    // Hue
    int hue() const;
    void setHue(int hue);

    // Saturation
    int saturation() const;
    void setSaturation(int saturation);

public: // Internal
    
    bool eventFilter(QObject *object, QEvent *event);
    
/*    
Q_SIGNALS: // QVideoWidgetControl

    void fullScreenChanged(bool fullScreen);
    void brightnessChanged(int brightness);
    void contrastChanged(int contrast);
    void hueChanged(int hue);
    void saturationChanged(int saturation);
*/

Q_SIGNALS: // Internal Signals

    void widgetResized(QSize size);
    void widgetUpdated();

private: // Data

    DSCameraSession*    m_session;
    QLabel              *m_widget;
    WId                 m_windowId;
    Qt::AspectRatioMode m_aspectRatioMode;
    bool                m_fullScreen;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // DSVideoWidgetControl_H
