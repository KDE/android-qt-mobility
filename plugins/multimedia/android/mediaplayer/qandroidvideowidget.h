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

#ifndef QANDROIDVIDEOWIDGET_H
#define QANDROIDVIDEOWIDGET_H

#include <qvideowidgetcontrol.h>
#include <QtGui/QWidget>
#include <QMutex>
#include <QQueue>

QT_USE_NAMESPACE
class QAndroidVideoWidget : public QWidget
{
    Q_OBJECT
public:
    QAndroidVideoWidget(QWidget *parent = 0);
    ~QAndroidVideoWidget();
    void doPaint();
    uchar* m_buf;
    int m_len;
    QMutex m_mutex;
    QQueue<uchar*> m_queue;


private:
     void paintEvent(QPaintEvent *);
};

class QAndroidVideoWidgetControl
        : public QVideoWidgetControl

{
    Q_OBJECT

public:
    QAndroidVideoWidgetControl(QObject *parent = 0);
    virtual ~QAndroidVideoWidgetControl();

    void precessNewStream();

    QWidget *videoWidget();

    Qt::AspectRatioMode aspectRatioMode() const;
    void setAspectRatioMode(Qt::AspectRatioMode mode);

    bool isFullScreen() const;
    void setFullScreen(bool fullScreen);

    int brightness() const;
    void setBrightness(int brightness);

    int contrast() const;
    void setContrast(int contrast);

    int hue() const;
    void setHue(int hue);

    int saturation() const;
    void setSaturation(int saturation);
    QAndroidVideoWidget* getWidget();


signals:
    void sinkChanged();
    void readyChanged(bool);

private:
    void createVideoWidget();


    //GstElement *m_videoSink;
    QAndroidVideoWidget *m_widget;
    WId m_windowId;
    Qt::AspectRatioMode m_aspectRatioMode;
    bool m_fullScreen;
};

#endif // QANDROIDVIDEOWIDGET_H
