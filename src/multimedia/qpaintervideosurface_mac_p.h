/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QPAINTERVIDEOSURFACE_MAC_P_H
#define QPAINTERVIDEOSURFACE_MAC_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qpaintervideosurface_p.h"
#include <qvideosurfaceformat.h>
#include <qvideoframe.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QVideoSurfaceCoreGraphicsPainter : public QVideoSurfacePainter
{
public:
    QVideoSurfaceCoreGraphicsPainter(bool glSupported);
    ~QVideoSurfaceCoreGraphicsPainter();

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
            QAbstractVideoBuffer::HandleType handleType) const;

    bool isFormatSupported(
            const QVideoSurfaceFormat &format, QVideoSurfaceFormat *similar) const;

    QAbstractVideoSurface::Error start(const QVideoSurfaceFormat &format);
    void stop();

    QAbstractVideoSurface::Error setCurrentFrame(const QVideoFrame &frame);

    QAbstractVideoSurface::Error paint(
            const QRectF &target, QPainter *painter, const QRectF &source);

    void updateColors(int brightness, int contrast, int hue, int saturation);

private:
    void* ciContext;
    QList<QVideoFrame::PixelFormat> m_imagePixelFormats;
    QVideoFrame m_frame;
    QSize m_imageSize;
    QImage::Format m_imageFormat;
    QVector<QAbstractVideoBuffer::HandleType> m_supportedHandles;
    QVideoSurfaceFormat::Direction m_scanLineDirection;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
