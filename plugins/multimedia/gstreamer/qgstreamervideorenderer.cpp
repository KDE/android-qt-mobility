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

#include "qgstreamervideorenderer.h"
#include "qvideosurfacegstsink.h"
#include "qabstractvideosurface.h"

#include <QEvent>
#include <QApplication>
#include <QDebug>

#include <gst/gst.h>

QGstreamerVideoRenderer::QGstreamerVideoRenderer(QObject *parent)
    :QVideoRendererControl(parent),m_videoSink(0), m_surface(0)
{
}

QGstreamerVideoRenderer::~QGstreamerVideoRenderer()
{
    if (m_videoSink)
        gst_object_unref(GST_OBJECT(m_videoSink));
}

GstElement *QGstreamerVideoRenderer::videoSink()
{
    if (!m_videoSink && m_surface) {
        m_videoSink = QVideoSurfaceGstSink::createSink(m_surface);
        gst_object_ref(GST_OBJECT(m_videoSink)); //Take ownership
        gst_object_sink(GST_OBJECT(m_videoSink));
    }

    return reinterpret_cast<GstElement*>(m_videoSink);
}


QAbstractVideoSurface *QGstreamerVideoRenderer::surface() const
{
    return m_surface;
}

void QGstreamerVideoRenderer::setSurface(QAbstractVideoSurface *surface)
{
    if (m_surface != surface) {
        //qDebug() << Q_FUNC_INFO << surface;
        if (m_videoSink)
            gst_object_unref(GST_OBJECT(m_videoSink));

        m_videoSink = 0;

        if (m_surface) {
            disconnect(m_surface, SIGNAL(supportedFormatsChanged()),
                       this, SLOT(handleFormatChange()));
        }
        
        m_surface = surface;

        if (surface && !m_surface)
            emit readyChanged(true);

        if (!surface && m_surface)
            emit readyChanged(false);

        if (m_surface) {
            connect(m_surface, SIGNAL(supportedFormatsChanged()),
                    this, SLOT(handleFormatChange()));
        }

        emit sinkChanged();
    }
}

void QGstreamerVideoRenderer::handleFormatChange()
{
    //qDebug() << "Supported formats list has changed, reload video output";

    if (m_videoSink)
        gst_object_unref(GST_OBJECT(m_videoSink));

    m_videoSink = 0;
    emit sinkChanged();
}
