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

#ifdef USE_PRIVATE_QWIDGET_METHODS
#include <QtGui/private/qwidget_p.h>
#endif

#include "s60videowidgetcontrol.h"

#include <coemain.h>    // CCoeEnv
#include <coecntrl.h>   // CCoeControl
#include <w32std.h>


S60ViewFinderWidget::S60ViewFinderWidget(QWidget *parent):
    QLabel(parent),
    m_isDirect(false)
{
}

S60ViewFinderWidget::~S60ViewFinderWidget()
{
}

void S60ViewFinderWidget::reconfigureWidget(const bool isVFDirect)
{
    if (isVFDirect) {
        m_isDirect = true;
        // Enable drawing of the ViewFinder
#ifndef USING_NGA // Pre-Symbian^3 Platforms
        setAttribute(Qt::WA_OpaquePaintEvent, true);
        setAttribute(Qt::WA_NoSystemBackground, true);
        setAutoFillBackground(false);
        setPalette(QPalette(Qt::black));
#endif // !USING_NGA

#ifdef USE_PRIVATE_QWIDGET_METHODS
#ifdef USING_NGA
        qt_widget_private(this)->createExtra();
        qt_widget_private(this)->extraData()->nativePaintMode = QWExtra::Disable;
#else // Pre-Symbian^3 Platforms
        qt_widget_private(this)->extraData()->nativePaintMode = QWExtra::ZeroFill;
        qt_widget_private(this)->extraData()->receiveNativePaintEvents = true;
#endif // USING_NGA
#endif // USE_PRIVATE_QWIDGET_METHODS
    } else {
        m_isDirect = false;
        setAlignment(Qt::AlignCenter);
        setAttribute(Qt::WA_NoSystemBackground, true);

        QPalette palette;
        palette.setColor(QPalette::Background, Qt::black);
        setPalette(palette);
    }
}

void S60ViewFinderWidget::beginNativePaintEvent(const QRect& /*controlRect*/)
{
    emit beginVideoWindowNativePaint();
}
    
void S60ViewFinderWidget::endNativePaintEvent(const QRect& /*controlRect*/)
{
    CCoeEnv::Static()->WsSession().Flush();
    emit endVideoWindowNativePaint();
}

void S60ViewFinderWidget::paintEvent(QPaintEvent *event)
{
    if (!m_isDirect) {
        QLabel::paintEvent(event);
    }
}

//#############################################################################

S60VideoWidgetControl::S60VideoWidgetControl(QObject *parent):
    QVideoWidgetControl(parent),
    m_widget(new S60ViewFinderWidget),
    m_fullScreen(false),
    m_isViewFinderDirect(false) // Default is Bitmap ViewFinder
{
    m_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Request QEvents
    m_widget->installEventFilter(this);
}

S60VideoWidgetControl::~S60VideoWidgetControl()
{
    if (m_widget)
        delete m_widget;
}

bool S60VideoWidgetControl::eventFilter(QObject *object, QEvent *e)
{
    if (object == m_widget) {
        
        switch (e->type()) {
            case QEvent::ParentChange:
            case QEvent::WinIdChange:
            case QEvent::Show:
                emit widgetUpdated();
                if (e->type() == QEvent::Show)
                    emit widgetVisible(true);
                break;
            case QEvent::Resize:
                emit widgetResized(m_widget->size());
                break;
            case QEvent::Hide:
                emit widgetVisible(false);
                break;
            
            default:
                // Do nothing
                break;
        }
    }
    return false;
}

void S60VideoWidgetControl::reconfigureWidget(const bool directVF)
{
    if (m_isViewFinderDirect == directVF)
        return;

    if (directVF) { // DirectScreen ViewFinder
        m_isViewFinderDirect = true;

        // Reconfigure to use methods needed by DirectScreen ViewFinder
        m_widget->reconfigureWidget(true);

        // Create a native window
        m_windowId = m_widget->winId();
    } else { // Bitmap ViewFinder
        m_isViewFinderDirect = false;

        // Reconfigure to use methods needed by Bitmap ViewFinder
        m_widget->reconfigureWidget(false);
    }
}

QWidget *S60VideoWidgetControl::videoWidget()
{
    return m_widget;
}

WId S60VideoWidgetControl::windowId()
{
    if (m_isViewFinderDirect) {
        m_windowId = m_widget->winId();
    } else {
        if (m_widget->internalWinId())
            m_windowId = m_widget->internalWinId();
        else
            m_windowId = m_widget->effectiveWinId();
    }
    return m_windowId;
}

Qt::AspectRatioMode S60VideoWidgetControl::aspectRatioMode() const
{
    return m_aspectRatioMode;
}

void S60VideoWidgetControl::setAspectRatioMode(Qt::AspectRatioMode ratio)
{
    if (m_aspectRatioMode==ratio) {
        return;
    }
    m_aspectRatioMode = ratio;

    if (!m_isViewFinderDirect) {
        if (m_aspectRatioMode == Qt::KeepAspectRatio)
            m_widget->setScaledContents(false);
        else {
            m_widget->setScaledContents(true);
        }
    }
}

bool S60VideoWidgetControl::isFullScreen() const
{
    return m_fullScreen;
}

void S60VideoWidgetControl::setFullScreen(bool fullScreen)
{
    if (m_widget && !fullScreen && m_fullScreen) {
        m_widget->showMaximized();
        m_fullScreen = false;
    } else if (m_widget && fullScreen) {
        m_widget->showFullScreen();
        m_fullScreen = true;
    }

    emit fullScreenChanged(fullScreen);
}

int S60VideoWidgetControl::brightness() const
{
    return 0;
}

void S60VideoWidgetControl::setBrightness(int brightness)
{
    Q_UNUSED(brightness);
}

int S60VideoWidgetControl::contrast() const
{
    return 0;
}

void S60VideoWidgetControl::setContrast(int contrast)
{
    Q_UNUSED(contrast);
}

int S60VideoWidgetControl::hue() const
{
    return 0;
}

void S60VideoWidgetControl::setHue(int hue)
{
    Q_UNUSED(hue);
}

int S60VideoWidgetControl::saturation() const
{
    return 0;
}

void S60VideoWidgetControl::setSaturation(int saturation)
{
    Q_UNUSED(saturation);
}

// End of file
