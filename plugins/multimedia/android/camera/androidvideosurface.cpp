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


#include "androidvideosurface.h"
#include <qabstractvideosurface.h>
#include <qvideosurfaceformat.h>
#include <QDebug>


AndroidVideoSurface::AndroidVideoSurface(QWidget *m_widget, QObject *parent)
    : QAbstractVideoSurface(parent)
    , m_widget(m_widget)
    , m_imageFormat(QImage::Format_Invalid)
{
    connect(this,SIGNAL(surfaceStarted(const QVideoSurfaceFormat&)),this,SLOT(surfaceStart(const QVideoSurfaceFormat&)));
    connect(this,SIGNAL(surfaceStopped()),this,SLOT(surfaceStop()));
}


void AndroidVideoSurface::surfaceStart(const QVideoSurfaceFormat& format)
{
    QAbstractVideoSurface::start(format);
}

void AndroidVideoSurface::surfaceStop()
{
    QAbstractVideoSurface::stop();
}

QList<QVideoFrame::PixelFormat> AndroidVideoSurface::supportedPixelFormats(
        QAbstractVideoBuffer::HandleType handleType) const
{
    if (handleType == QAbstractVideoBuffer::NoHandle) {
        return QList<QVideoFrame::PixelFormat>()
                << QVideoFrame::Format_RGB32
                << QVideoFrame::Format_ARGB32
                << QVideoFrame::Format_ARGB32_Premultiplied
                << QVideoFrame::Format_RGB565
                << QVideoFrame::Format_RGB555;
    } else {
        return QList<QVideoFrame::PixelFormat>();
    }
}

bool AndroidVideoSurface::isFormatSupported(
        const QVideoSurfaceFormat &format, QVideoSurfaceFormat *similar) const
{
    Q_UNUSED(similar);

    const QImage::Format m_imageFormat = QVideoFrame::imageFormatFromPixelFormat(format.pixelFormat());
    const QSize size = format.frameSize();

    return m_imageFormat != QImage::Format_Invalid
            && !size.isEmpty()
            && format.handleType() == QAbstractVideoBuffer::NoHandle;
}

bool AndroidVideoSurface::start(const QVideoSurfaceFormat &format)
{
    const QImage::Format m_imageFormat = QVideoFrame::imageFormatFromPixelFormat(format.pixelFormat());
    const QSize size = format.frameSize();

    if (m_imageFormat != QImage::Format_Invalid && !size.isEmpty()) {
        this->m_imageFormat = m_imageFormat;
        m_imageSize = size;
        m_sourceRect = format.viewport();
        emit(surfaceStarted(format));
        m_widget->updateGeometry();
        updateVideoRect();
        return true;
    } else {
        return false;
    }
}

void AndroidVideoSurface::stop()
{
    m_currentFrame = QVideoFrame();
    m_targetRect = QRect();
    emit(surfaceStopped());
    m_widget->update();
}

bool AndroidVideoSurface::present(const QVideoFrame &frame)
{
    if (surfaceFormat().pixelFormat() != frame.pixelFormat()
        || surfaceFormat().frameSize() != frame.size()) {
        setError(IncorrectFormatError);
        stop();
        return false;
    } else {
        paintLock.lock();
        m_currentFrame = frame;
        m_widget->update(m_targetRect);
        paintLock.unlock();
        return true;
    }
}


void AndroidVideoSurface::updateVideoRect()
{
    QSize size = surfaceFormat().sizeHint();
    size.scale(m_widget->size().boundedTo(size), Qt::KeepAspectRatio);

    m_targetRect = QRect(QPoint(0, 0), size);
    m_targetRect.moveCenter(m_widget->rect().center());
}

void AndroidVideoSurface::paint(QPainter *painter)
{
    if (m_currentFrame.map(QAbstractVideoBuffer::ReadOnly)) {
        const QTransform oldTransform = painter->transform();

        if (surfaceFormat().scanLineDirection() == QVideoSurfaceFormat::BottomToTop) {
            painter->scale(1, -1);
            painter->translate(0, -m_widget->height());
        }

        QImage image(
                m_currentFrame.bits(),
                m_currentFrame.width(),
                m_currentFrame.height(),
                m_currentFrame.bytesPerLine(),
                m_imageFormat);

        painter->drawImage(m_targetRect, image, m_sourceRect);
        painter->setTransform(oldTransform);
        m_currentFrame.unmap();
    }
}


