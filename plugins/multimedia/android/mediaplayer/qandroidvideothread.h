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
#ifndef QANDROIDVIDEOTHREAD_H
#define QANDROIDVIDEOTHREAD_H
#include "qandroidvideowidget.h"
#include "qandroidplayersession.h"
#include <QThread>
#ifdef __cplusplus
extern "C" {
#endif
#include "ffmpeg/libavformat/avformat.h"
#include "ffmpeg/libavcodec/avcodec.h"
#include "ffmpeg/libswscale/swscale.h"
#include "ffmpeg.h"
#ifdef __cplusplus
}
#endif
class QAndroidDisplayThread;
class QAndroidPlayerSession;
class QAndroidVideoThread: public QThread {
    Q_OBJECT
public:
    QAndroidVideoThread(QAndroidVideoWidget* v);
    virtual ~QAndroidVideoThread();
    void setSeek(int position);
    void clearSeek();
    bool quitThread();
    void setQuit(bool status);
    bool isSafetoQuit();
    void setSafeToQuit();
    void run();
    void setPath(QString path);
    bool isThreadQuit();
    QAndroidDisplayThread *m_thread;
    bool m_quitNow;
    bool m_safeToQuit;
private:

    QAndroidVideoWidget* m_widget;
    QString m_path;
    int playVideo();

    bool m_running;
    int64_t m_currentPts;
    int64_t m_timeStamp;
    int64_t m_tsIncrement;
    int64_t m_tsOffset;
    int m_seekFalgs;
    bool m_seekRequired;
    int m_seekPosition;
    bool m_threadQuit;

signals:
    void updateWidget();
    void videoSetup(int lengthMsec);
};
class QAndroidDisplayThread : public QThread
{
    Q_OBJECT
public :
        QAndroidDisplayThread(QAndroidVideoThread* thread,QAndroidVideoWidget* widget,AVCodecContext* cxt);
        virtual ~QAndroidDisplayThread();
        bool quitThread();
        void setQuit(bool status);
        void run();
        bool m_quitNow;

private:
        QAndroidVideoWidget* m_widget;
        QAndroidVideoThread* m_thread;
        AVCodecContext *m_context;
        void startDisplay();
};

class QAndroidVideoHelper : public QObject
{
    Q_OBJECT
public :
        QAndroidVideoHelper(QAndroidVideoWidget* widget);
        virtual ~QAndroidVideoHelper();
        void startDecoding();
        void doTheWork(int event,int position);
        void setPath(QString path);

private :
        QAndroidVideoThread *m_videoThread;
        QAndroidDisplayThread *m_displayThread;
        QAndroidVideoWidget *m_widget;
        int m_state;
        int64_t m_startTime;
        int64_t m_pauseTime;
        QString m_path;
};
#endif // QANDROIDVIDEOTHREAD_H
