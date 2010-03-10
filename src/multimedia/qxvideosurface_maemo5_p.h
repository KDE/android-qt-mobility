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

#ifndef QXVIDEOSURFACE_MAEMO5_H
#define QXVIDEOSURFACE_MAEMO5_H

#include <QtCore/qhash.h>
#include <QtGui/qwidget.h>
#include <QtMultimedia/qabstractvideosurface.h>

#include <X11/Xlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xv.h>
#include <X11/extensions/Xvlib.h>

QT_USE_NAMESPACE

class QXVideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    QXVideoSurface(QObject *parent = 0);
    ~QXVideoSurface();

    WId winId() const;
    void setWinId(WId id);

    QRect displayRect() const;
    void setDisplayRect(const QRect &rect);

    QColor colorKey() const;
    void setColorKey(QColor key);

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
            QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;

public slots:
    bool start(const QVideoSurfaceFormat &format);
    void stop();

    bool present(const QVideoFrame &frame);
    void repaintLastFrame();

private:
    WId m_winId;
    XvPortID m_portId;
    int m_xvFormatId;
    GC m_gc;
    XvImage *m_image;
    XShmSegmentInfo	m_shminfo;
    QList<QVideoFrame::PixelFormat> m_supportedPixelFormats;
    QVector<int> m_formatIds;
    QRect m_viewport;
    QRect m_displayRect;
    QColor m_colorKey;

    QVideoFrame lastFrame;

    bool findPort();
    void querySupportedFormats();

    int getAttribute(const char *attribute) const;
    void setAttribute(const char *attribute, int value);
};

Q_DECLARE_METATYPE(XvImage*)

#endif
