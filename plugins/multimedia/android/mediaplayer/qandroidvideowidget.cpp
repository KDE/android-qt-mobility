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

#include "qandroidvideowidget.h"
#include "qandroidvideothread.h"
#include <QtCore/qcoreevent.h>
#include <QtCore/qdebug.h>
#include <QtGui/qapplication.h>
#include <QtGui/qpainter.h>


QAndroidVideoWidget::QAndroidVideoWidget(QWidget* parent) :
        QWidget(parent),m_buf(NULL),m_len(0)
{

}

QAndroidVideoWidget::~QAndroidVideoWidget() {

}

void QAndroidVideoWidget::paintEvent(QPaintEvent * e) {
    QPainter painter(this);
    m_mutex.lock();
    if(m_buf)
    {
        QImage i = QImage::fromData(m_buf,m_len,"PPM");
        painter.drawImage(QPoint(0,0),i);
    }
    m_mutex.unlock();
}

void QAndroidVideoWidget::doPaint()
{
    this->update();
}

QAndroidVideoWidgetControl::QAndroidVideoWidgetControl(QObject *parent)
    : QVideoWidgetControl(parent)
    , m_widget(0)
    , m_fullScreen(false)
{

}

QAndroidVideoWidgetControl::~QAndroidVideoWidgetControl()
{

    delete m_widget;
}

void QAndroidVideoWidgetControl::createVideoWidget()
{
    if (m_widget)
        return;
    m_widget = new QAndroidVideoWidget;
}

QWidget *QAndroidVideoWidgetControl::videoWidget()
{
    createVideoWidget();
    return m_widget;
}

QAndroidVideoWidget *QAndroidVideoWidgetControl::getWidget()
{
    createVideoWidget();
    return m_widget;

}

Qt::AspectRatioMode QAndroidVideoWidgetControl::aspectRatioMode() const
{
    return m_aspectRatioMode;
}

void QAndroidVideoWidgetControl::setAspectRatioMode(Qt::AspectRatioMode mode)
{
    Q_UNUSED(mode);
}

bool QAndroidVideoWidgetControl::isFullScreen() const
{
    return m_fullScreen;
}

void QAndroidVideoWidgetControl::setFullScreen(bool fullScreen)
{
    emit fullScreenChanged(m_fullScreen =  fullScreen);
}

int QAndroidVideoWidgetControl::brightness() const
{
    return 0;
}

void QAndroidVideoWidgetControl::setBrightness(int brightness)
{
    Q_UNUSED(brightness);
}

int QAndroidVideoWidgetControl::contrast() const
{
    return 0;
}

void QAndroidVideoWidgetControl::setContrast(int contrast)
{
    Q_UNUSED(contrast);
}

int QAndroidVideoWidgetControl::hue() const
{
    return 0;
}

void QAndroidVideoWidgetControl::setHue(int hue)
{
    Q_UNUSED(hue);
}

int QAndroidVideoWidgetControl::saturation() const
{
    return 0;
}

void QAndroidVideoWidgetControl::setSaturation(int saturation)
{
    Q_UNUSED(saturation);
}


