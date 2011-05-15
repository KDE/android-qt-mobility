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

#ifndef ANDROIDVIDEOWIDGETCONTROL_H
#define ANDROIDVIDEOWIDGETCONTROL_H

#include <qvideowidgetcontrol.h>
#include <qabstractvideosurface.h>
#include <androidvideosurface.h>
#include <androidcamjni.h>
#include <QThread>
#include <qvideosurfaceformat.h>
#include <QTime>
QT_BEGIN_NAMESPACE
class QAbstractVideoSurface;
QT_END_NAMESPACE
static QSize scaledSize;
class AndroidVideoSurface;
class AndroidVideoWidgetControl;

class AndroidVideoWidget : public QWidget
{
    Q_OBJECT
public:
    AndroidVideoWidget(QWidget *parent = 0);
    ~AndroidVideoWidget();

    QAbstractVideoSurface *videoSurface() const { return m_videoSurface; }

    QSize sizeHint() const;

    void setNativeSize( const QSize &);
protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);

private:
    AndroidVideoSurface *m_videoSurface;
    QSize m_nativeSize;
};

class PreviewThread:public QThread
{
    Q_OBJECT
private :
        AndroidVideoWidgetControl * m_widgetControl;
public:
void getParent (AndroidVideoWidgetControl *widgetControl)
{
    m_widgetControl=widgetControl;
}


protected:
virtual void run();
};


class AndroidVideoWidgetControl: public QVideoWidgetControl
{
    Q_OBJECT

public: // Constructor & Destructor

    AndroidVideoWidgetControl(QObject *parent = 0);
    virtual ~AndroidVideoWidgetControl();

public: // QVideoWidgetControl

    QWidget *videoWidget();

    // Aspect Ratio
    Qt::AspectRatioMode aspectRatioMode() const;
    void setAspectRatioMode(Qt::AspectRatioMode ratio);

    // Full Screen
    bool isFullScreen() const;
    void setFullScreen(bool);

    // Brightness
    int brightness() const;
    void setBrightness(int);

    // Contrast
    int contrast() const;
    void setContrast(int);

    // Hue
    int hue() const;
    void setHue(int hue);

    // Saturation
    int saturation() const;
    void setSaturation(int);

    //
    void onBufferAvailable(QImage &);
private:
    AndroidVideoWidget * m_viewFinder;
    PreviewThread m_previewThread;
    QAbstractVideoSurface *m_surface;
signals:
    void widgetSet();

};






#endif // ANDROIDVIDEOWIDGETCONTROL_H
