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
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef S60VIDEOWIDGET_H
#define S60VIDEOWIDGET_H

#include <qvideowidgetcontrol.h>
#include <qmediaplayer.h>

QT_USE_NAMESPACE

class QAbstractVideoWidget : public QWidget
{
    Q_OBJECT

public:
    QAbstractVideoWidget(QWidget *parent = 0);
    virtual ~QAbstractVideoWidget();
};

class QBlackSurface : public QAbstractVideoWidget
{
    Q_OBJECT

public:
    QBlackSurface(QWidget *parent = 0);
    virtual ~QBlackSurface();

protected:
    void paintEvent(QPaintEvent *event);
};

class QBlackWidget : public QAbstractVideoWidget
{
    Q_OBJECT
    
public:
    QBlackWidget(QWidget *parent = 0);
    virtual ~QBlackWidget();

signals:
    void beginVideoWindowNativePaint();
    void endVideoWindowNativePaint();    
    
public slots:
    void beginNativePaintEvent(const QRect&);
    void endNativePaintEvent(const QRect&);
    
protected:
    void paintEvent(QPaintEvent *event);
};

class S60VideoWidgetControl : public QVideoWidgetControl
{
    Q_OBJECT
	
public:
    S60VideoWidgetControl(QObject *parent = 0);
    virtual ~S60VideoWidgetControl();

    // from QVideoWidgetControl
    QWidget *videoWidget();
    Qt::AspectRatioMode aspectRatioMode() const;
    void setAspectRatioMode(Qt::AspectRatioMode ratio);
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
    // from QObject
    bool eventFilter(QObject *object, QEvent *event);

    //new methods
    WId videoWidgetWId();
    QSize videoWidgetSize();

private:
    void initializeVideoOutput();
    
signals:
    void widgetUpdated();
    void widgetResized();
    void beginVideoWindowNativePaint();
    void endVideoWindowNativePaint();
    
private slots:
    void videoStateChanged(QMediaPlayer::State state);
    
private:
    QAbstractVideoWidget *m_widget;
    Qt::AspectRatioMode m_aspectRatioMode;
    bool m_fullScreenEnabled;
};

#endif // S60VIDEOWIDGET_H
