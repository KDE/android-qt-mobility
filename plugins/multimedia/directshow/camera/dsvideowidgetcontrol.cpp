/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtCore/qcoreevent.h>
#include <QtCore/qtimer.h>

#include "DSVideoWidgetControl.h"
#include "dscamerasession.h"

QT_BEGIN_NAMESPACE

DSVideoWidgetSurface::DSVideoWidgetSurface(QLabel *pWidget, QObject *parent)
{
    widget = pWidget;
    myPixmap = 0;
}

QList<QVideoFrame::PixelFormat> DSVideoWidgetSurface::supportedPixelFormats(
        QAbstractVideoBuffer::HandleType handleType) const
{
    if (handleType == QAbstractVideoBuffer::NoHandle) {
        return QList<QVideoFrame::PixelFormat>()
                << QVideoFrame::Format_RGB32
                << QVideoFrame::Format_RGB24;
    } else {
        return QList<QVideoFrame::PixelFormat>();
    }
}


bool DSVideoWidgetSurface::present(const QVideoFrame &frame)
{
    QVideoFrame myFrame = frame;
    myFrame.map(QAbstractVideoBuffer::ReadOnly);
    QImage image(
                     frame.bits(),
                     frame.width(),
                     frame.height(),
                     frame.bytesPerLine(),
                     imageFormat);
    if (image.isNull())
    {
        // Try to adapt
        QImage image2(
                frame.bits(),
                frame.width(),
                frame.height(),
                frame.bytesPerLine(),
                QImage::Format_RGB888);
        image = image2;
    }
    myFrame.unmap();
    delete myPixmap;
    myPixmap = new QPixmap(QPixmap::fromImage(image).scaled(widget->size()));
    widget->setPixmap(*myPixmap);
    widget->repaint();
    return true;
}

void DSVideoWidgetSurface::setImageFormat(QImage::Format fmt)
{
    imageFormat = fmt;
}

void DSVideoWidgetSurface::updateVideoRect()
{
}

void DSVideoWidgetSurface::paint(QPainter *painter)
{
}


DSVideoWidgetControl::DSVideoWidgetControl(DSCameraSession* session, QObject *parent) :
    m_session(session), QVideoWidgetControl(parent),
    m_widget(new QLabel()),
    m_fullScreen(false)
{
    m_widget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    m_widget->setAlignment(Qt::AlignCenter);
    m_widget->setAttribute(Qt::WA_NoSystemBackground, true);
    
    surface = new DSVideoWidgetSurface(m_widget);

    QPalette palette;
    palette.setColor(QPalette::Background, Qt::black);
    m_widget->setPalette(palette);
    m_widget->setAutoFillBackground( true );

    // Request QEvents
    m_widget->installEventFilter(this);
    m_windowId = m_widget->effectiveWinId();

    surface->setImageFormat(QImage::Format_RGB888);
    session->setSurface(surface);
}

DSVideoWidgetControl::~DSVideoWidgetControl()
{
    delete m_widget;
}

bool DSVideoWidgetControl::eventFilter(QObject *object, QEvent *e)
{
    if (object == m_widget) {
        switch (e->type()) {
            case QEvent::ParentChange:
            case QEvent::WinIdChange:
            case QEvent::Show:
                m_windowId = m_widget->effectiveWinId();
                emit widgetUpdated();
                break;
            case QEvent::Resize:
                emit widgetResized(m_widget->size());
                break;
            case QEvent::PolishRequest:
                m_widget->ensurePolished();
                break;
            
            default:
                // Do nothing
                break;
        }
    }
    return false;
}

QWidget *DSVideoWidgetControl::videoWidget()
{
    return m_widget;
}

Qt::AspectRatioMode DSVideoWidgetControl::aspectRatioMode() const
{
    return m_aspectRatioMode;
}

void DSVideoWidgetControl::setAspectRatioMode(Qt::AspectRatioMode ratio)
{
    if (m_aspectRatioMode==ratio) {
        return;
    }
    m_aspectRatioMode = ratio;

    if (m_aspectRatioMode == Qt::KeepAspectRatio)
        m_widget->setScaledContents(false);
    else {
        m_widget->setScaledContents(true);
    }
}

bool DSVideoWidgetControl::isFullScreen() const
{
    return m_fullScreen;
}

void DSVideoWidgetControl::setFullScreen(bool fullScreen)
{
    if (m_widget && !fullScreen && m_fullScreen) {
        m_widget->showNormal();
        m_fullScreen = false;
    } else if (m_widget && fullScreen) {
        m_widget->showFullScreen();
        m_fullScreen = true;
    }

    emit fullScreenChanged(fullScreen);
}

int DSVideoWidgetControl::brightness() const
{
    return 0;
}

void DSVideoWidgetControl::setBrightness(int brightness)
{
    Q_UNUSED(brightness);
}

int DSVideoWidgetControl::contrast() const
{
    return 0;
}

void DSVideoWidgetControl::setContrast(int contrast)
{
    Q_UNUSED(contrast);
}

int DSVideoWidgetControl::hue() const
{
    return 0;
}

void DSVideoWidgetControl::setHue(int hue)
{
    Q_UNUSED(hue);
}

int DSVideoWidgetControl::saturation() const
{
    return 0;
}

void DSVideoWidgetControl::setSaturation(int saturation)
{
    Q_UNUSED(saturation);
}

QT_END_NAMESPACE

// End of file
