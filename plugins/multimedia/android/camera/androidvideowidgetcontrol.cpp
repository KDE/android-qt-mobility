/*
Copyright (c) 2011 Elektrobit (EB), All rights reserved.
Contact: oss-devel@elektrobit.com

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
* Neither the name of the Elektrobit (EB) nor the names of its
contributors may be used to endorse or promote products derived from
 this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY Elektrobit (EB) ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Elektrobit
(EB) BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <androidvideowidgetcontrol.h>
#include <QtGui/QPaintEvent>
#include <QRect>
#include <QtGui/QPainter>
#include <QDebug>
#include <QtGui/QApplication>
#include <QtGui/QWidget>

AndroidVideoWidget::AndroidVideoWidget(QWidget *parent)
    : QWidget(parent)
    , m_videoSurface(0)
{
    setAutoFillBackground(false);
    QPalette palette = this->palette();
    palette.setColor(QPalette::Background, Qt::black);
    setPalette(palette);

    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    m_videoSurface = new AndroidVideoSurface(this);
}

AndroidVideoWidget::~AndroidVideoWidget()
{
    delete m_videoSurface;
}

QSize AndroidVideoWidget::sizeHint() const
{
    return m_videoSurface->surfaceFormat().sizeHint();
}

void AndroidVideoWidget::paintEvent(QPaintEvent *event)
{
    QPoint point = mapToGlobal(QPoint(0,0));
    scaledSize.setWidth(event->rect().width());
    scaledSize.setHeight(event->rect().height());
    QList<int> params;
    params<<event->rect().width()<<event->rect().height()<<point.x()<<point.y();
    QtCameraJni::setVideoPreviewParams(params);
    m_videoSurface->paintLock.lock();
    QPainter painter(this);

    if (m_videoSurface->isActive()) {
        const QRect videoRect = m_videoSurface->videoRect();
        if (!videoRect.contains(event->rect())) {
            QRegion region = event->region();
            region.subtract(videoRect);
            QBrush brush = palette().background();

            foreach (const QRect &rect, region.rects())
                painter.fillRect(rect, brush);
        }

        m_videoSurface->paint(&painter);
    } else {
        painter.fillRect(event->rect(), palette().background());
    }
    m_videoSurface->paintLock.unlock();

}


void AndroidVideoWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    m_videoSurface->updateVideoRect();
}

void AndroidVideoWidget::setNativeSize( const QSize &size)
{
    if (size != m_nativeSize) {
        m_nativeSize = size;
        if (size.isEmpty())
            setMinimumSize(0,0);
        else
            setMinimumSize(160,120);

        updateGeometry();
    }
}



AndroidVideoWidgetControl::AndroidVideoWidgetControl(QObject *parent):
        QVideoWidgetControl(parent)
        , m_surface(0)
{
    m_previewThread.getParent(this);
    connect(this,SIGNAL(widgetSet()),&m_previewThread,SLOT(start()));
}

AndroidVideoWidgetControl::~AndroidVideoWidgetControl()
{
    delete(m_viewFinder);
    m_previewThread.exit();
}

QWidget *AndroidVideoWidgetControl::videoWidget()
{
    m_viewFinder = new AndroidVideoWidget();
    m_surface = m_viewFinder->videoSurface();
    emit widgetSet();
    return m_viewFinder;
}

Qt::AspectRatioMode AndroidVideoWidgetControl::aspectRatioMode() const
{
    return Qt::AspectRatioMode();
}

void AndroidVideoWidgetControl::setAspectRatioMode(Qt::AspectRatioMode ratio)
{
    Q_UNUSED(ratio);
}

bool AndroidVideoWidgetControl::isFullScreen() const
{
    return false;
}

void AndroidVideoWidgetControl::setFullScreen(bool fullScreen)
{
    Q_UNUSED(fullScreen);
}

int AndroidVideoWidgetControl::brightness() const
{
    return 0;
}

void AndroidVideoWidgetControl::setBrightness(int brightness)
{
    Q_UNUSED(brightness);
}

int AndroidVideoWidgetControl::contrast() const
{
    return 0;
}

void AndroidVideoWidgetControl::setContrast(int contrast)
{
    Q_UNUSED(contrast);
}

int AndroidVideoWidgetControl::hue() const
{
    return 0;
}

void AndroidVideoWidgetControl::setHue(int hue)
{
    Q_UNUSED(hue);
}

int AndroidVideoWidgetControl::saturation() const
{
    return 0;
}

void AndroidVideoWidgetControl::setSaturation(int saturation)
{
    Q_UNUSED(saturation);
}

void AndroidVideoWidgetControl::onBufferAvailable(QImage &image)
{
    QImage scaledImage;
    scaledImage = image.scaled(scaledSize);
    QVideoFrame frame(scaledImage);

    if (!frame.isValid())
        return;

    QVideoSurfaceFormat currentFormat = m_surface->surfaceFormat();

    if (frame.pixelFormat() != currentFormat.pixelFormat()
        || frame.size() != currentFormat.frameSize()) {
        QVideoSurfaceFormat format(frame.size(), frame.pixelFormat());

        if (!m_surface->start(format))
            return;
    }
    if (!m_surface->present(frame)) {
        m_surface->stop();

        return;
    } else {
        return;
    }
}


void PreviewThread::run ()
{
    while(1)
    {
        QtCameraJni::waitlocked();
        QtCameraJni::wait();
        QImage & getImage= QtCameraJni::getBuffer();
        m_widgetControl->onBufferAvailable(getImage);
        QtCameraJni::waitUnlocked();
    }
}


