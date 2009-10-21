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

#ifndef QMEDIASLIDESHOWSERVICE_P_H
#define QMEDIASLIDESHOWSERVICE_P_H

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

#include <multimedia/qmediaservice.h>
#include <multimedia/qmediaimageviewer.h>
#include <multimedia/qvideooutputcontrol.h>
#include <multimedia/qvideorenderercontrol.h>
#include <multimedia/qvideowidget.h>
#include <multimedia/qvideowidgetcontrol.h>

#include <QtGui/qimage.h>

class QAbstractVideoSurface;
class QNetworkAccessManager;

class QMediaImageViewerServicePrivate;

class Q_AUTOTEST_EXPORT QMediaImageViewerService : public QMediaService
{
    Q_OBJECT
public:
    explicit QMediaImageViewerService(QObject *parent = 0);
    ~QMediaImageViewerService();

    QMediaControl *control(const char *name) const;

    QNetworkAccessManager *networkManager() const;
    void setNetworkManager(QNetworkAccessManager *manager);

private:
    Q_DECLARE_PRIVATE(QMediaImageViewerService)
    Q_PRIVATE_SLOT(d_func(), void _q_outputChanged(QVideoOutputControl::Output output))
    friend class QMediaImageViewerControl;
    friend class QMediaImageViewerControlPrivate;
};

class QMediaImageViewerControlPrivate;

class QMediaImageViewerControl : public QMediaControl
{
    Q_OBJECT
public:
    explicit QMediaImageViewerControl(QMediaImageViewerService *parent);
    ~QMediaImageViewerControl();

    QMediaImageViewer::MediaStatus mediaStatus() const;

    void showMedia(const QMediaContent &media);

Q_SIGNALS:
    void mediaStatusChanged(QMediaImageViewer::MediaStatus status);

private:
    Q_DECLARE_PRIVATE(QMediaImageViewerControl)
    Q_PRIVATE_SLOT(d_func(), void _q_headFinished())
    Q_PRIVATE_SLOT(d_func(), void _q_getFinished())
};

#define QMediaImageViewerControl_iid "com.nokia.Qt.QMediaImageViewerControl/1.0"
Q_MEDIA_DECLARE_CONTROL(QMediaImageViewerControl, QMediaImageViewerControl_iid)

#ifndef QT_NO_MULTIMEDIA
class QMediaImageViewerRenderer : public QVideoRendererControl
{
    Q_OBJECT
public:
    QMediaImageViewerRenderer(QObject *parent = 0);
    ~QMediaImageViewerRenderer();

    QAbstractVideoSurface *surface() const;
    void setSurface(QAbstractVideoSurface *surface);

    void showImage(const QImage &image);

Q_SIGNALS:
    void surfaceChanged(QAbstractVideoSurface *surface);

private:
    QAbstractVideoSurface *m_surface;
    QImage m_image;
};
#endif

class QMediaImageViewerWidget : public QWidget
{
    Q_OBJECT
public:
    QMediaImageViewerWidget(QWidget *parent = 0);

    void showImage(const QImage &image);

    QVideoWidget::AspectRatioMode aspectRatioMode() const { return m_aspectRatioMode; }
    void setAspectRatioMode(QVideoWidget::AspectRatioMode mode);

    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *event);

private:
    QVideoWidget::AspectRatioMode m_aspectRatioMode;
    QImage m_image;
};

class QMediaImageViewerWidgetControl : public QVideoWidgetControl
{
    Q_OBJECT
public:
    QMediaImageViewerWidgetControl(QMediaImageViewerWidget *widget, QObject *parent = 0);

    QWidget *videoWidget();

    QVideoWidget::AspectRatioMode aspectRatioMode() const { return m_widget->aspectRatioMode(); }
    void setAspectRatioMode(QVideoWidget::AspectRatioMode mode);

    bool isFullScreen() const { return m_fullScreen; }
    void setFullScreen(bool fullScreen);

    int brightness() const { return 0; }
    void setBrightness(int) {}

    int contrast() const { return 0; }
    void setContrast(int) {}

    int hue() const { return 0; }
    void setHue(int) {}

    int saturation() const { return 0; }
    void setSaturation(int) {}

private:
    QMediaImageViewerWidget *m_widget;
    bool m_fullScreen;
};

class QMediaImageViewerOutputControl : public QVideoOutputControl
{
    Q_OBJECT
public:
    QMediaImageViewerOutputControl(QObject *parent = 0);

    QList<Output> availableOutputs() const;

    Output output() const { return m_output; }
    void setOutput(Output output);

Q_SIGNALS:
    void outputChanged(QVideoOutputControl::Output output);

private:
    Output m_output;
};

#endif
