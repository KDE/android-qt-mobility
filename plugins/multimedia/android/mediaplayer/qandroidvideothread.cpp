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
#include <QtCore/qdatetime.h>
#include <QtCore/qdebug.h>
#include <QtCore/qsize.h>
#include <QtCore/qdebug.h>
#include <QTimer>
#include <stdint.h>

#include "qandroidvideothread.h"
#include "qandroidplayersession.h"
#include "mediaPlayerJNI.h"
#ifdef __cplusplus
extern "C" {
#endif
#ifndef INT64_C
# define INT64_C(c) c ## LL
#endif
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#ifdef __cplusplus
}
#endif
enum
{
    PLAY=0,
    PAUSE,
    STOP,
    SEEK
};

QQueue<AVFrame*> frameQueue;
QMutex frameQueueMutex;
QWaitCondition scaleWait;
int count=0;
QMutex waitPleaseMutex;

QAndroidVideoThread::QAndroidVideoThread(QAndroidVideoWidget* v):
        QThread(),
        m_widget(v),
        m_running(true),
        m_seekRequired(false),
        m_seekPosition(0),
        m_quitNow(false),
        m_threadQuit(true)


{
    m_thread = NULL;
}

QAndroidVideoThread::~QAndroidVideoThread()
{
    delete(m_thread);
}

void QAndroidVideoThread::run()
{
    playVideo();
}

void QAndroidVideoThread::setPath(QString path)
{
    m_path=path;
}

void QAndroidVideoThread::setSeek(int position)
{
    m_seekRequired = true;
    m_seekPosition = position;
    m_seekFalgs= 0;
}

void QAndroidVideoThread::clearSeek()
{
    m_seekRequired = false;
}

bool QAndroidVideoThread::isThreadQuit()
{
    return m_threadQuit;
}

int QAndroidVideoThread::playVideo()
{
    AVFormatContext *pFormatCtx;
    int             i, videoStream;
    AVCodecContext  *pCodecCtx;
    AVCodec         *pCodec;
    AVFrame         *pFrame;
    AVPacket        packet;
    int             frameFinished;
    m_safeToQuit = false;
    m_threadQuit = false;
    av_register_all();

    QByteArray mediaPathByteArray = m_path.toLatin1();
    const char* m_rawpath = mediaPathByteArray.data();
    if (avformat_open_input(&pFormatCtx, m_rawpath, NULL, NULL) != 0)
    {

        QANDROID_DEBUG<<"Could not open the file";
        m_widget->m_mutex.lock();
        QtMediaPlayerJNI::wait(&(m_widget->m_mutex));
        m_widget->m_mutex.unlock();
        m_threadQuit = true;
        if(m_thread)
        {
            m_thread->setQuit(true);
            scaleWait.wakeAll();
        }
        return -1;
    }

    if (av_find_stream_info(pFormatCtx) < 0)
    {
        QANDROID_DEBUG<<"Could not find stream info";
        m_widget->m_mutex.lock();
        QtMediaPlayerJNI::wait(&(m_widget->m_mutex));
        m_widget->m_mutex.unlock();
        m_threadQuit = true;
        if(m_thread)
        {
            m_thread->setQuit(true);
            scaleWait.wakeAll();
        }
        return -1;
    }

    videoStream = -1;
    for (i = 0; i < pFormatCtx->nb_streams; i++)
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
        videoStream = i;
        break;
    }
    if (videoStream == -1)
    {
        QANDROID_DEBUG<<"Unable to find Video Stream";
        m_widget->m_mutex.lock();
        QtMediaPlayerJNI::wait(&(m_widget->m_mutex));
        m_widget->m_mutex.unlock();
        m_threadQuit = true;
        if(m_thread)
        {
            m_thread->setQuit(true);
            scaleWait.wakeAll();
        }
        return -1;
    }

    pCodecCtx = pFormatCtx->streams[videoStream]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == NULL) {
        QANDROID_DEBUG<<"Unsupported codec!\n";
        m_widget->m_mutex.lock();
        QtMediaPlayerJNI::wait(&(m_widget->m_mutex));
        m_widget->m_mutex.unlock();
        m_threadQuit = true;
        if(m_thread)
        {
            m_thread->setQuit(true);
            scaleWait.wakeAll();
        }
        return -1;
    }

    if (avcodec_open(pCodecCtx, pCodec) < 0)
    {

        m_widget->m_mutex.lock();
        QtMediaPlayerJNI::wait(&(m_widget->m_mutex));
        m_widget->m_mutex.unlock();
        m_threadQuit = true;
        if(m_thread)
        {
            m_thread->setQuit(true);
            scaleWait.wakeAll();
        }
        return -1;
    }
    if(!m_thread)
    {
        m_thread = new QAndroidDisplayThread(this,m_widget,pCodecCtx);
    }

    m_thread->setQuit(false);
    m_thread->start();
    waitPleaseMutex.lock();
    frameQueue.clear();
    waitPleaseMutex.unlock();

    m_widget->m_mutex.lock();
    QtMediaPlayerJNI::wait(&(m_widget->m_mutex));
    m_widget->m_mutex.unlock();
    QANDROID_DEBUG;
    count=0;
    m_tsOffset = av_rescale_q(pFormatCtx->start_time, AV_TIME_BASE_Q, pFormatCtx->streams[videoStream]->time_base);

    if (pCodecCtx->codec->flush != NULL)
    {

        pCodecCtx->codec->flush(pCodecCtx);
    }

    while (!quitThread()) {



        if(m_seekRequired)
        {
            m_tsIncrement = av_rescale(m_seekPosition/1000, pFormatCtx->streams[videoStream]->time_base.den, pFormatCtx->streams[videoStream]->time_base.num);

            int64_t next_timestamp = m_currentPts + m_tsIncrement;
            if (av_seek_frame(pFormatCtx, videoStream, next_timestamp + m_tsOffset, 0) >= 0)

            {
                if (pCodecCtx->codec->flush != NULL)
                    pCodecCtx->codec->flush(pCodecCtx);

            }

            m_seekRequired = false;

        }

        if(av_read_frame(pFormatCtx, &packet) < 0)
        {
            QANDROID_DEBUG<<"Unable to read Frame";
            m_widget->m_mutex.lock();
            QtMediaPlayerJNI::wait(&(m_widget->m_mutex));
            m_widget->m_mutex.unlock();
            m_threadQuit = true;
            if(m_thread)
            {
                m_thread->setQuit(true);
                scaleWait.wakeAll();
            }
            return -1;
        }
        if(packet.pts != AV_NOPTS_VALUE)
        {
            m_timeStamp = packet.pts;
        }
        else
        {
            m_timeStamp = packet.dts;
        }
        m_currentPts = m_timeStamp - m_tsOffset;
        pFrame = avcodec_alloc_frame();
        if (packet.stream_index == videoStream) {
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
            if (frameFinished) {
                waitPleaseMutex.lock();
                frameQueue.enqueue(pFrame);
                scaleWait.wakeAll();
                waitPleaseMutex.unlock();
            }
        }
    }
    m_thread->setQuit(true);
    scaleWait.wakeAll();
    while(!isSafetoQuit());
    m_threadQuit = true;
    av_free_packet(&packet);
    avcodec_close(pCodecCtx);
    av_close_input_file(pFormatCtx);
    return 0;
}

bool QAndroidVideoThread::quitThread()
{
    return m_quitNow;
}

void QAndroidVideoThread::setQuit(bool status)
{
    m_quitNow = status;
}

void QAndroidVideoThread::setSafeToQuit()
{

    m_safeToQuit = true;
}

bool QAndroidVideoThread::isSafetoQuit()
{
    return m_safeToQuit;
}

QAndroidDisplayThread::QAndroidDisplayThread(QAndroidVideoThread *thread,QAndroidVideoWidget *widget,AVCodecContext *cxt):
        m_widget(widget),m_context(cxt),m_quitNow(false),m_thread(thread)
{

}

QAndroidDisplayThread::~QAndroidDisplayThread()
{

}

bool QAndroidDisplayThread::quitThread()
{
    return m_quitNow;
}

void QAndroidDisplayThread::setQuit(bool status)
{
    m_quitNow = status;
}

void QAndroidDisplayThread::run()
{
    startDisplay();
}

void QAndroidDisplayThread::startDisplay()
{
    AVFrame  *pFrameRGB;
    AVFrame  *pFrame;
    int             numBytes;
    uint8_t         *buffer;

    pFrameRGB = avcodec_alloc_frame();

    if(pFrameRGB==NULL)
    {
        return;
    }

    PixelFormat dst_fmt = PIX_FMT_RGB24;
    int dst_w = m_widget->width();
    int dst_h = m_widget->height();
    numBytes = avpicture_get_size(dst_fmt, dst_w, dst_h);
    buffer = new uint8_t[numBytes + 64];
    int headerlen = sprintf((char *) buffer, "P6\n%d %d\n255\n", dst_w, dst_h);

    m_widget->m_buf = (uchar*)buffer;
    m_widget->m_len = avpicture_get_size(dst_fmt,dst_w,dst_h) + headerlen;


    avpicture_fill((AVPicture *)pFrameRGB,
                   buffer + headerlen,
                   dst_fmt,
                   dst_w,
                   dst_h);

    SwsContext *img_convert_ctx;
    img_convert_ctx = sws_getContext(
            m_context->width, m_context->height, m_context->pix_fmt,
            dst_w, dst_h, dst_fmt,
            SWS_BICUBIC, NULL, NULL, NULL);


    while(!quitThread())
    {
        waitPleaseMutex.lock();
        scaleWait.wait(&waitPleaseMutex);
        pFrame=frameQueue.dequeue();
        count++;
        waitPleaseMutex.unlock();

        waitPleaseMutex.lock();
        m_widget->m_mutex.lock();
        sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0,m_context->height, pFrameRGB->data, pFrameRGB->linesize);
        m_widget->m_mutex.unlock();
        m_widget->doPaint();
        av_free(pFrame);
        count--;
        waitPleaseMutex.unlock();

    }
    m_thread->setSafeToQuit();
}

QAndroidVideoHelper::QAndroidVideoHelper(QAndroidVideoWidget *widget):
        m_widget(widget),m_state(0)
{
    m_videoThread = new QAndroidVideoThread(m_widget);
}

QAndroidVideoHelper::~QAndroidVideoHelper()
{
    delete(m_videoThread);
}

void QAndroidVideoHelper::doTheWork(int event,int position)
{
    if(!m_videoThread)
    {
        m_videoThread = new QAndroidVideoThread(m_widget);
    }

    switch(event)
    {
    case PLAY:
        if(m_state!=PAUSE)
        {
            QANDROID_DEBUG;
            m_startTime=av_gettime()/1000000.0;
            if(position==0)
            {
                m_videoThread->clearSeek();
            }
            m_videoThread->setQuit(false);
            startDecoding();
            m_state=PLAY;

        }
        else
        {
            m_videoThread->setQuit(false);
            int resumeTime = m_pauseTime-m_startTime;
            m_startTime = av_gettime()/1000000.0;
            m_videoThread->setSeek(resumeTime);
            startDecoding();
            m_state=PLAY;
        }
        break;
    case PAUSE:
        if(m_state!=STOP)
        {

            m_pauseTime = av_gettime()/1000000.0;
            m_videoThread->setQuit(true);
            while(!m_videoThread->isThreadQuit());
            m_state = PAUSE;
        }
        break;
    case STOP:
        {
            m_videoThread->setQuit(true);
            while(!m_videoThread->isThreadQuit());
            m_videoThread->clearSeek();
            m_state = STOP;
        }
        break;
    case SEEK:
        if(m_state!=STOP)
        {

            m_videoThread->setQuit(true);
            while(!m_videoThread->isThreadQuit());
            m_videoThread->setSeek(position);
        }
        break;
    }
}

void QAndroidVideoHelper::startDecoding()
{
    m_videoThread->setPath(m_path);
    m_videoThread->start();
}

void QAndroidVideoHelper::setPath(QString path)
{
    m_path = path;
}
