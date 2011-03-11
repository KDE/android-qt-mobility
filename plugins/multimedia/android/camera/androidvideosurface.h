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

#ifndef ANDROIDVIDEOSURFACE_H
#define ANDROIDVIDEOSURFACE_H


#include <QtCore/QRect>
#include <QtGui/QImage>
#include <qabstractvideosurface.h>
#include <qvideoframe.h>
#include <QMutex>
#include <QtGui/QWidget>
#include <QDebug>
#include <QTime>
#include <QtGui/QPainter>

class AndroidVideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    AndroidVideoSurface(QWidget *widget, QObject *parent = 0);

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
            QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;
    bool isFormatSupported(const QVideoSurfaceFormat &format, QVideoSurfaceFormat *similar) const;

    bool start(const QVideoSurfaceFormat &format);
    void stop();

    bool present(const QVideoFrame &frame);

    QRect videoRect() const { return m_targetRect; }
    void updateVideoRect();

    void paint(QPainter *painter);
    void repainting ();
    QMutex paintLock;


private:
    QWidget *m_widget;
    QImage::Format m_imageFormat;
    QRect m_targetRect;
    QSize m_imageSize;
    QRect m_sourceRect;
    QVideoFrame m_currentFrame;

signals:
    void surfaceStarted(const QVideoSurfaceFormat&);
    void surfaceStopped();

private slots:
    void surfaceStart(const QVideoSurfaceFormat&);
    void surfaceStop();

};



#endif // ANDROIDVIDEOSURFACE_H
