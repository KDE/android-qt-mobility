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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// INTERNAL USE ONLY: Do NOT use for any other purpose.
//

#include <QtCore/qcoreapplication.h>
#include <qaudioformat.h>
#include <QtNetwork>
#include <QTime>

#include "qsoundeffect_pulse_p.h"

#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
#include <pulse/ext-stream-restore.h>
#endif

#include <unistd.h>

//#define QT_PA_DEBUG
#ifndef QTM_PULSEAUDIO_DEFAULTBUFFER
#define QT_PA_STREAM_BUFFER_SIZE_MAX (1024 * 64)  //64KB is a trade-off for balancing control latency and uploading overhead
#endif

QT_BEGIN_NAMESPACE

namespace
{
inline pa_sample_spec audioFormatToSampleSpec(const QAudioFormat &format)
{
    pa_sample_spec  spec;

    spec.rate = format.frequency();
    spec.channels = format.channels();

    if (format.sampleSize() == 8)
        spec.format = PA_SAMPLE_U8;
    else if (format.sampleSize() == 16) {
        switch (format.byteOrder()) {
            case QAudioFormat::BigEndian: spec.format = PA_SAMPLE_S16BE; break;
            case QAudioFormat::LittleEndian: spec.format = PA_SAMPLE_S16LE; break;
        }
    }
    else if (format.sampleSize() == 32) {
        switch (format.byteOrder()) {
            case QAudioFormat::BigEndian: spec.format = PA_SAMPLE_S32BE; break;
            case QAudioFormat::LittleEndian: spec.format = PA_SAMPLE_S32LE; break;
        }
    }

    return spec;
}

class PulseDaemon : public QObject
{
    Q_OBJECT
public:
    PulseDaemon(): m_prepared(false)
    {
        prepare();
    }

    ~PulseDaemon()
    {
        if (m_prepared)
            release();
    }

    inline void lock()
    {
        pa_threaded_mainloop_lock(m_mainLoop);
    }

    inline void unlock()
    {
        pa_threaded_mainloop_unlock(m_mainLoop);
    }

    inline pa_context *context() const
    {
        return m_context;
    }

    inline pa_cvolume * calcVolume(pa_cvolume *dest, int soundEffectVolume)
    {
        dest->channels = 2;
        dest->values[0] = dest->values[1] = m_vol * soundEffectVolume / 100;
        return dest;
    }

    void updateStatus(const pa_cvolume& volume)
    {
        if (m_vol != pa_cvolume_max(&volume)) {
            m_vol = pa_cvolume_max(&volume);
            emit volumeChanged();
        }
    }

Q_SIGNALS:
    void contextReady();
    void volumeChanged();

private:
    void prepare()
    {
        m_vol = PA_VOLUME_NORM;

        m_mainLoop = pa_threaded_mainloop_new();
        if (m_mainLoop == 0) {
            qWarning("PulseAudioService: unable to create pulseaudio mainloop");
            return;
        }

        if (pa_threaded_mainloop_start(m_mainLoop) != 0) {
            qWarning("PulseAudioService: unable to start pulseaudio mainloop");
            pa_threaded_mainloop_free(m_mainLoop);
            return;
        }

        m_mainLoopApi = pa_threaded_mainloop_get_api(m_mainLoop);

        lock();
        m_context = pa_context_new(m_mainLoopApi, QString(QLatin1String("QtPulseAudio:%1")).arg(::getpid()).toAscii().constData());

        pa_context_set_state_callback(m_context, context_state_callback, this);

        if (m_context == 0) {
            qWarning("PulseAudioService: Unable to create new pulseaudio context");
            pa_threaded_mainloop_free(m_mainLoop);
            return;
        }

        if (pa_context_connect(m_context, 0, (pa_context_flags_t)0, 0) < 0) {
            qWarning("PulseAudioService: pa_context_connect() failed");
            pa_context_unref(m_context);
            pa_threaded_mainloop_free(m_mainLoop);
            return;
        }
        unlock();

        m_prepared = true;
    }

    void release()
    {
        if (!m_prepared) return;
        pa_threaded_mainloop_stop(m_mainLoop);
        pa_threaded_mainloop_free(m_mainLoop);
        m_prepared = false;
    }

    static void context_state_callback(pa_context *c, void *userdata)
    {
        PulseDaemon *self = reinterpret_cast<PulseDaemon*>(userdata);
        switch (pa_context_get_state(c)) {
            case PA_CONTEXT_CONNECTING:
            case PA_CONTEXT_AUTHORIZING:
            case PA_CONTEXT_SETTING_NAME:
                break;
            case PA_CONTEXT_READY:
    #if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
                pa_ext_stream_restore_read(c, &stream_restore_info_callback, self);
                pa_ext_stream_restore_set_subscribe_cb(c, &stream_restore_monitor_callback, self);
                pa_ext_stream_restore_subscribe(c, 1, 0, self);
    #endif
                QMetaObject::invokeMethod(self, "contextReady", Qt::QueuedConnection);
                break;
            default:
                break;
        }
    }

#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)

    static void stream_restore_monitor_callback(pa_context *c, void *userdata)
    {
        PulseDaemon *self = reinterpret_cast<PulseDaemon*>(userdata);
        pa_ext_stream_restore_read(c, &stream_restore_info_callback, self);
    }

    static void stream_restore_info_callback(pa_context *c,
            const pa_ext_stream_restore_info *info,
            int eol, void *userdata)
    {
        Q_UNUSED(c)

        PulseDaemon *self = reinterpret_cast<PulseDaemon*>(userdata);

        if (!eol) {
            if (QString(info->name).startsWith(QLatin1String("sink-input-by-media-role:x-maemo"))) {
#ifdef QT_PA_DEBUG
                qDebug() << "x-maemo volume =(" << info->volume.values[0] * 100 / PA_VOLUME_NORM << ","
                         << info->volume.values[1] * 100 / PA_VOLUME_NORM << "), "
                         << "mute = " << info->mute;
#endif
                self->updateStatus(info->volume);
            }
        }
    }
#endif

    pa_volume_t m_vol;

    bool m_prepared;
    pa_context *m_context;
    pa_threaded_mainloop *m_mainLoop;
    pa_mainloop_api *m_mainLoopApi;
};

}

Q_GLOBAL_STATIC(PulseDaemon, daemon)
Q_GLOBAL_STATIC(QSampleCache, sampleCache)

namespace
{
class PulseDaemonLocker
{
public:
    PulseDaemonLocker()
    {
        daemon()->lock();
    }

    ~PulseDaemonLocker()
    {
        daemon()->unlock();
    }
};
}

QSoundEffectPrivate::QSoundEffectPrivate(QObject* parent):
    QObject(parent),
    m_pulseStream(0),
    m_sinkInputId(-1),
    m_emptying(false),
    m_sampleReady(false),
    m_playing(false),
    m_status(QSoundEffect::Null),
    m_muted(false),
    m_playQueued(false),
    m_stopping(false),
    m_volume(100),
    m_loopCount(1),
    m_runningCount(0),
    m_sample(0) ,
    m_position(0)
{
    pa_sample_spec_init(&m_pulseSpec);
}

QSoundEffectPrivate::~QSoundEffectPrivate()
{
    unloadPulseStream();

    if (m_sample)
        m_sample->release();
}

QStringList QSoundEffectPrivate::supportedMimeTypes()
{
    QStringList supportedTypes;
    supportedTypes << QLatin1String("audio/x-wav") << QLatin1String("audio/vnd.wave") ;
    return supportedTypes;
}

QUrl QSoundEffectPrivate::source() const
{
    return m_source;
}

void QSoundEffectPrivate::setSource(const QUrl &url)
{
    Q_ASSERT(m_source != url);
#ifdef QT_PA_DEBUG
    qDebug() << this << "setSource =" << url;
#endif
    stop();
    if (m_sample) {
        if (!m_sampleReady) {
            disconnect(m_sample, SIGNAL(error()), this, SLOT(decoderError()));
            disconnect(m_sample, SIGNAL(ready()), this, SLOT(sampleReady()));
        }
        m_sample->release();
        m_sample = 0;
    }

    m_source = url;
    m_sampleReady = false;

    PulseDaemonLocker locker;
    m_runningCount = 0;
    if (m_pulseStream && !pa_stream_is_corked(m_pulseStream)) {
        pa_stream_set_write_callback(m_pulseStream, 0, 0);
        pa_stream_set_underflow_callback(m_pulseStream, 0, 0);
        pa_operation_unref(pa_stream_cork(m_pulseStream, 1, 0, 0));
    }
    setPlaying(false);

    if (url.isEmpty()) {
        setStatus(QSoundEffect::Null);
        return;
    }

    setStatus(QSoundEffect::Loading);
    m_sample = sampleCache()->requestSample(url);
    connect(m_sample, SIGNAL(error()), this, SLOT(decoderError()));
    connect(m_sample, SIGNAL(ready()), this, SLOT(sampleReady()));
    switch(m_sample->state()) {
    case QSample::Ready:
        sampleReady();
        break;
    case QSample::Error:
        decoderError();
        break;
    }
}

int QSoundEffectPrivate::loopCount() const
{
    return m_loopCount;
}

void QSoundEffectPrivate::setLoopCount(int loopCount)
{
    if (loopCount == 0)
        loopCount = 1;
    m_loopCount = loopCount;
}

int QSoundEffectPrivate::volume() const
{
    return m_volume;
}

void QSoundEffectPrivate::setVolume(int volume)
{
    m_volume = volume;
    emit volumeChanged();
    updateVolume();
}

void QSoundEffectPrivate::updateVolume()
{
    if (m_sinkInputId < 0)
        return;
    PulseDaemonLocker locker;
    pa_cvolume volume;
    pa_operation_unref(pa_context_set_sink_input_volume(daemon()->context(), m_sinkInputId, daemon()->calcVolume(&volume, m_volume), setvolume_callback, this));
    Q_ASSERT(pa_cvolume_valid(&volume));
#ifdef QT_PA_DEBUG
    qDebug() << this << "updateVolume =" << pa_cvolume_max(&volume);
#endif
}

bool QSoundEffectPrivate::isMuted() const
{
    return m_muted;
}

void QSoundEffectPrivate::setMuted(bool muted)
{
    m_muted = muted;
    emit mutedChanged();
    updateMuted();
}

void QSoundEffectPrivate::updateMuted()
{
    if (m_sinkInputId < 0)
        return;
    PulseDaemonLocker locker;
    pa_operation_unref(pa_context_set_sink_input_mute(daemon()->context(), m_sinkInputId, m_muted, setmuted_callback, this));
#ifdef QT_PA_DEBUG
    qDebug() << this << "updateMuted = " << daemon()->calcMuted(m_muted);
#endif
}

bool QSoundEffectPrivate::isLoaded() const
{
    return m_status == QSoundEffect::Ready;
}

bool QSoundEffectPrivate::isPlaying() const
{
    return m_playing;
}

QSoundEffect::Status QSoundEffectPrivate::status() const
{
    return m_status;
}

void QSoundEffectPrivate::setPlaying(bool playing)
{
#ifdef QT_PA_DEBUG
    qDebug() << this << "setPlaying(" << playing << ")";
#endif
    if (m_playing == playing)
        return;
    if (!playing)
        m_playQueued = false;
    m_playing = playing;
    emit playingChanged();
}

void QSoundEffectPrivate::setStatus(QSoundEffect::Status status)
{
#ifdef QT_PA_DEBUG
    qDebug() << this << "setStatus" << status;
#endif
    if (m_status == status)
        return;
    bool oldLoaded = isLoaded();
    m_status = status;
    emit statusChanged();
    if (oldLoaded != isLoaded())
        emit loadedChanged();
}

void QSoundEffectPrivate::play()
{
#ifdef QT_PA_DEBUG
    qDebug() << this << "play";
#endif
    if (m_status == QSoundEffect::Null || m_status == QSoundEffect::Error || m_playQueued)
        return;

    PulseDaemonLocker locker;
    if (!m_sampleReady || m_stopping || m_emptying) {
#ifdef QT_PA_DEBUG
        qDebug() << this << "play deferred";
#endif
        m_playQueued = true;
    } else {
        if (m_playing) { //restart playing from the beginning
#ifdef QT_PA_DEBUG
           qDebug() << this << "restart playing";
#endif
            m_runningCount = 0;
            m_playQueued = true;
            Q_ASSERT(m_pulseStream);
            emptyStream();
            return;
        }
        m_runningCount = m_loopCount;
        playSample();
    }

    setPlaying(true);
}

void QSoundEffectPrivate::emptyStream()
{
    m_emptying = true;
    pa_stream_set_write_callback(m_pulseStream, 0, this);
    pa_stream_set_underflow_callback(m_pulseStream, 0, this);
    pa_operation_unref(pa_stream_flush(m_pulseStream, stream_flush_callback, this));
}

void QSoundEffectPrivate::emptyComplete()
{
    PulseDaemonLocker locker;
    m_emptying = false;
    pa_operation_unref(pa_stream_cork(m_pulseStream, 1, stream_cork_callback, this));
}

void QSoundEffectPrivate::sampleReady()
{
#ifdef QT_PA_DEBUG
    qDebug() << this << "sampleReady";
#endif
    disconnect(m_sample, SIGNAL(error()), this, SLOT(decoderError()));
    disconnect(m_sample, SIGNAL(ready()), this, SLOT(sampleReady()));
    pa_sample_spec newFormatSpec = audioFormatToSampleSpec(m_sample->format());

    if (m_pulseStream && (memcmp(&m_pulseSpec, &newFormatSpec, sizeof(m_pulseSpec)) != 0)) {
        unloadPulseStream();
    }
    m_pulseSpec = newFormatSpec;

    m_sampleReady = true;
    m_position = 0;

    if (m_name.isNull())
        m_name = QString(QLatin1String("QtPulseSample-%1-%2")).arg(::getpid()).arg(quintptr(this)).toUtf8();

    PulseDaemonLocker locker;
    if (m_pulseStream) {
#ifdef QT_PA_DEBUG
        qDebug() << this << "reuse existing pulsestream";
#endif
#ifdef QTM_PULSEAUDIO_DEFAULTBUFFER
        const pa_buffer_attr *bufferAttr = pa_stream_get_buffer_attr(m_pulseStream);
        if (bufferAttr->prebuf > uint32_t(m_sample->data().size())) {
            pa_buffer_attr newBufferAttr;
            newBufferAttr = *bufferAttr;
            newBufferAttr.prebuf = m_sample->data().size();
            pa_stream_set_buffer_attr(m_pulseStream, &newBufferAttr, stream_adjust_prebuffer_callback, this);
        } else {
            streamReady();
        }
#else
        const pa_buffer_attr *bufferAttr = pa_stream_get_buffer_attr(m_pulseStream);
        if (bufferAttr->tlength < m_sample->data().size() && bufferAttr->tlength < QT_PA_STREAM_BUFFER_SIZE_MAX) {
            pa_buffer_attr newBufferAttr;
            newBufferAttr.maxlength = -1;
            newBufferAttr.tlength = qMin(m_sample->data().size(), QT_PA_STREAM_BUFFER_SIZE_MAX);
            newBufferAttr.minreq = bufferAttr->tlength / 2;
            newBufferAttr.prebuf = -1;
            newBufferAttr.fragsize = -1;
            pa_stream_set_buffer_attr(m_pulseStream, &newBufferAttr, stream_reset_buffer_callback, this);
        } else if (bufferAttr->prebuf > uint32_t(m_sample->data().size())) {
            pa_buffer_attr newBufferAttr;
            newBufferAttr = *bufferAttr;
            newBufferAttr.prebuf = m_sample->data().size();
            pa_stream_set_buffer_attr(m_pulseStream, &newBufferAttr, stream_adjust_prebuffer_callback, this);
        } else {
            streamReady();
        }
#endif
    } else {
        if (pa_context_get_state(daemon()->context()) != PA_CONTEXT_READY) {
            connect(daemon(), SIGNAL(contextReady()), SLOT(contextReady()));
            return;
        }
        createPulseStream();
    }
}

void QSoundEffectPrivate::decoderError()
{
    qWarning("QSoundEffect(pulseaudio): Error decoding source");
    disconnect(m_sample, SIGNAL(error()), this, SLOT(decoderError()));
    bool playingDirty = false;
    if (m_playing) {
        m_playing = false;
        playingDirty = true;
    }
    setStatus(QSoundEffect::Error);
    if (playingDirty)
        emit playingChanged();
}

void QSoundEffectPrivate::unloadPulseStream()
{
#ifdef QT_PA_DEBUG
    qDebug() << this << "unloadPulseStream";
#endif
    m_sinkInputId = -1;
    PulseDaemonLocker locker;
    if (m_pulseStream) {
        pa_stream_set_state_callback(m_pulseStream, 0, 0);
        pa_stream_set_write_callback(m_pulseStream, 0, 0);
        pa_stream_set_underflow_callback(m_pulseStream, 0, 0);
        pa_stream_disconnect(m_pulseStream);
        pa_stream_unref(m_pulseStream);
        disconnect(daemon(), SIGNAL(volumeChanged()), this, SLOT(updateVolume()));
        m_pulseStream = 0;
    }
}

void QSoundEffectPrivate::prepare()
{
    if (!m_pulseStream || !m_sampleReady)
        return;
    PulseDaemonLocker locker;
    pa_stream_set_write_callback(m_pulseStream, stream_write_callback, this);
    pa_stream_set_underflow_callback(m_pulseStream, stream_underrun_callback, this);
    m_stopping = false;
    size_t writeBytes = size_t(qMin(m_pulseBufferSize, m_sample->data().size()));
#ifdef QT_PA_DEBUG
    qDebug() << this << "prepare(): writable size =" << pa_stream_writable_size(m_pulseStream)
             << "actual writeBytes =" << writeBytes
             << "m_playQueued =" << m_playQueued;
#endif
    m_position = int(writeBytes);
    if (pa_stream_write(m_pulseStream, reinterpret_cast<void *>(const_cast<char*>(m_sample->data().data())), writeBytes,
                        stream_write_done_callback, 0, PA_SEEK_RELATIVE) != 0) {
        qWarning("QSoundEffect(pulseaudio): pa_stream_write, error = %s", pa_strerror(pa_context_errno(daemon()->context())));
    }
    if (m_playQueued) {
        m_playQueued = false;
        m_runningCount = m_loopCount;
        playSample();
    }
}

void QSoundEffectPrivate::uploadSample()
{
    if (m_runningCount == 0) {
#ifdef QT_PA_DEBUG
    qDebug() << this << "uploadSample: return due to 0 m_runningCount";
#endif
        return;
    }
#ifdef QT_PA_DEBUG
    qDebug() << this << "uploadSample: m_runningCount =" << m_runningCount;
#endif
    if (m_position == m_sample->data().size()) {
        m_position = 0;
        if (m_runningCount > 0)
            m_runningCount--;
        if (m_runningCount == 0) {
            return;
        }
    }

    int writtenBytes = 0;
    int writableSize = int(pa_stream_writable_size(m_pulseStream));
    int firstPartLength = qMin(m_sample->data().size() - m_position, writableSize);
    if (pa_stream_write(m_pulseStream, reinterpret_cast<void *>(const_cast<char*>(m_sample->data().data()) + m_position),
                        firstPartLength, stream_write_done_callback, 0, PA_SEEK_RELATIVE) != 0) {
        qWarning("QSoundEffect(pulseaudio): pa_stream_write, error = %s", pa_strerror(pa_context_errno(daemon()->context())));
    }
    writtenBytes = firstPartLength;
    m_position += firstPartLength;
    if (m_position == m_sample->data().size()) {
        m_position = 0;
        if (m_runningCount > 0)
            m_runningCount--;
        if (m_runningCount != 0 && firstPartLength < writableSize)
        {
            while (writtenBytes < writableSize) {
                int writeSize = qMin(writableSize - writtenBytes, m_sample->data().size());
                if (pa_stream_write(m_pulseStream, reinterpret_cast<void *>(const_cast<char*>(m_sample->data().data())),
                                    writeSize, stream_write_done_callback, 0, PA_SEEK_RELATIVE) != 0) {
                    qWarning("QSoundEffect(pulseaudio): pa_stream_write, error = %s", pa_strerror(pa_context_errno(daemon()->context())));
                }
                writtenBytes += writeSize;
                if (writeSize < m_sample->data().size()) {
                    m_position = writeSize;
                    break;
                }
                if (m_runningCount > 0)
                    m_runningCount--;
                if (m_runningCount == 0)
                    break;
            }
        }
    }
#ifdef QT_PA_DEBUG
    qDebug() << this << "uploadSample: use direct write, writeable size =" << writableSize
             << "actual writtenBytes =" << writtenBytes;
#endif
}

void QSoundEffectPrivate::playSample()
{
#ifdef QT_PA_DEBUG
    qDebug() << this << "playSample";
#endif
    Q_ASSERT(m_pulseStream);
    pa_operation_unref(pa_stream_cork(m_pulseStream, 0, 0, 0));
}

void QSoundEffectPrivate::stop()
{
#ifdef QT_PA_DEBUG
    qDebug() << this << "stop";
#endif
    if (!m_playing)
        return;
    setPlaying(false);
    PulseDaemonLocker locker;
    m_stopping = true;
    if (m_pulseStream)
        emptyStream();
    m_runningCount = 0;
    m_position = 0;
    m_playQueued = false;
}

void QSoundEffectPrivate::underRun()
{
    stop();
}

void QSoundEffectPrivate::streamReady()
{
#ifdef QT_PA_DEBUG
    qDebug() << this << "streamReady";
#endif
    PulseDaemonLocker locker;
    m_sinkInputId =  pa_stream_get_index(m_pulseStream);
    updateMuted();
    updateVolume();
#ifdef QT_PA_DEBUG
    const pa_buffer_attr *realBufAttr = pa_stream_get_buffer_attr(m_pulseStream);
    qDebug() << this << "m_sinkInputId =" << m_sinkInputId
             << "tlength =" << realBufAttr->tlength << "maxlength =" << realBufAttr->maxlength
             << "minreq = " << realBufAttr->minreq << "prebuf =" << realBufAttr->prebuf;
#endif
    prepare();
    setStatus(QSoundEffect::Ready);
}

void QSoundEffectPrivate::createPulseStream()
{
#ifdef QT_PA_DEBUG
    qDebug() << this << "createPulseStream";
#endif

    pa_proplist *propList = pa_proplist_new();
    pa_proplist_sets(propList, PA_PROP_MEDIA_ROLE, "soundeffect");
    pa_stream *stream = pa_stream_new_with_proplist(daemon()->context(), m_name.constData(), &m_pulseSpec, 0, propList);
    pa_proplist_free(propList);

    connect(daemon(), SIGNAL(volumeChanged()), this, SLOT(updateVolume()));

    if (stream == 0) {
        qWarning("QSoundEffect(pulseaudio): Failed to create stream");
        m_pulseStream = 0;
        setStatus(QSoundEffect::Error);
        setPlaying(false);
        return;
    }
    else {
        pa_stream_set_state_callback(stream, stream_state_callback, this);
        pa_stream_set_write_callback(stream, stream_write_callback, this);
        pa_stream_set_underflow_callback(stream, stream_underrun_callback, this);
    }
    m_pulseStream = stream;

#ifndef QTM_PULSEAUDIO_DEFAULTBUFFER
    pa_buffer_attr bufferAttr;
    bufferAttr.tlength = qMin(m_sample->data().size(), QT_PA_STREAM_BUFFER_SIZE_MAX);
    bufferAttr.maxlength = -1;
    bufferAttr.minreq = bufferAttr.tlength / 2;
    bufferAttr.prebuf = -1;
    bufferAttr.fragsize = -1;
    if (pa_stream_connect_playback(m_pulseStream, 0, &bufferAttr,
#else
    if (pa_stream_connect_playback(m_pulseStream, 0, 0,
#endif
                                   m_muted ? pa_stream_flags_t(PA_STREAM_START_MUTED | PA_STREAM_START_CORKED)
                                           : pa_stream_flags_t(PA_STREAM_START_UNMUTED | PA_STREAM_START_CORKED),
                                   0, 0) < 0) {
        qWarning("QSoundEffect(pulseaudio): Failed to connect stream, error = %s",
                 pa_strerror(pa_context_errno(daemon()->context())));
    }
}

void QSoundEffectPrivate::contextReady()
{
    disconnect(daemon(), SIGNAL(contextReady()), this, SLOT(contextReady()));
    PulseDaemonLocker locker;
    createPulseStream();
}

void QSoundEffectPrivate::stream_write_callback(pa_stream *s, size_t length, void *userdata)
{
    Q_UNUSED(length);
    Q_UNUSED(s)

    QSoundEffectPrivate *self = reinterpret_cast<QSoundEffectPrivate*>(userdata);
#ifdef QT_PA_DEBUG
    qDebug() << self << "stream_write_callback";
#endif
    self->uploadSample();
}

void QSoundEffectPrivate::stream_state_callback(pa_stream *s, void *userdata)
{
    QSoundEffectPrivate *self = reinterpret_cast<QSoundEffectPrivate*>(userdata);
    switch (pa_stream_get_state(s)) {
        case PA_STREAM_READY:
        {
#ifdef QT_PA_DEBUG
            qDebug() << self << "pulse stream ready";
#endif
            const pa_buffer_attr *bufferAttr = pa_stream_get_buffer_attr(self->m_pulseStream);
            self->m_pulseBufferSize = bufferAttr->tlength;
            if (bufferAttr->prebuf > uint32_t(self->m_sample->data().size())) {
                pa_buffer_attr newBufferAttr;
                newBufferAttr = *bufferAttr;
                newBufferAttr.prebuf = self->m_sample->data().size();
                pa_stream_set_buffer_attr(self->m_pulseStream, &newBufferAttr, stream_adjust_prebuffer_callback, userdata);
            } else {
                QMetaObject::invokeMethod(self, "streamReady", Qt::QueuedConnection);
            }
            break;
        }
        case PA_STREAM_CREATING:
#ifdef QT_PA_DEBUG
            qDebug() << self << "pulse stream creating";
#endif
            break;
        case PA_STREAM_TERMINATED:
#ifdef QT_PA_DEBUG
            qDebug() << self << "pulse stream terminated";
#endif
            break;

        case PA_STREAM_FAILED:
        default:
            qWarning("QSoundEffect(pulseaudio): Error in pulse audio stream");
            break;
    }
}

void QSoundEffectPrivate::stream_reset_buffer_callback(pa_stream *s, int success, void *userdata)
{
    Q_UNUSED(s);
    if (!success)
        qWarning("QSoundEffect(pulseaudio): faild to reset buffer attribute");
    QSoundEffectPrivate *self = reinterpret_cast<QSoundEffectPrivate*>(userdata);
#ifdef QT_PA_DEBUG
    qDebug() << self << "stream_reset_buffer_callback";
#endif
    const pa_buffer_attr *bufferAttr = pa_stream_get_buffer_attr(self->m_pulseStream);
    self->m_pulseBufferSize = bufferAttr->tlength;
    if (bufferAttr->prebuf > uint32_t(self->m_sample->data().size())) {
        pa_buffer_attr newBufferAttr;
        newBufferAttr = *bufferAttr;
        newBufferAttr.prebuf = self->m_sample->data().size();
        pa_stream_set_buffer_attr(self->m_pulseStream, &newBufferAttr, stream_adjust_prebuffer_callback, userdata);
    } else {
        QMetaObject::invokeMethod(self, "streamReady", Qt::QueuedConnection);
    }
}

void QSoundEffectPrivate::stream_adjust_prebuffer_callback(pa_stream *s, int success, void *userdata)
{
    Q_UNUSED(s);
    if (!success)
        qWarning("QSoundEffect(pulseaudio): faild to adjust pre-buffer attribute");
    QSoundEffectPrivate *self = reinterpret_cast<QSoundEffectPrivate*>(userdata);
#ifdef QT_PA_DEBUG
    qDebug() << self << "stream_adjust_prebuffer_callback";
#endif
    QMetaObject::invokeMethod(self, "streamReady", Qt::QueuedConnection);
}

void QSoundEffectPrivate::setvolume_callback(pa_context *c, int success, void *userdata)
{
    Q_UNUSED(c);
    Q_UNUSED(userdata);
#ifdef QT_PA_DEBUG
    qDebug() << reinterpret_cast<QSoundEffectPrivate*>(userdata) << "setvolume_callback";
#endif
    if (!success) {
        qWarning("QSoundEffect(pulseaudio): faild to set volume");
    }
}

void QSoundEffectPrivate::setmuted_callback(pa_context *c, int success, void *userdata)
{
    Q_UNUSED(c);
    Q_UNUSED(userdata);
#ifdef QT_PA_DEBUG
    qDebug() << reinterpret_cast<QSoundEffectPrivate*>(userdata) << "setmuted_callback";
#endif
    if (!success) {
        qWarning("QSoundEffect(pulseaudio): faild to set muted");
    }
}

void QSoundEffectPrivate::stream_underrun_callback(pa_stream *s, void *userdata)
{
    Q_UNUSED(s);
    QSoundEffectPrivate *self = reinterpret_cast<QSoundEffectPrivate*>(userdata);
#ifdef QT_PA_DEBUG
    qDebug() << self << "stream_underrun_callback";
#endif
    if (self->m_runningCount == 0 && !self->m_playQueued)
        QMetaObject::invokeMethod(self, "underRun", Qt::QueuedConnection);
#ifdef QT_PA_DEBUG
    else
        qDebug() << "underun corked =" << pa_stream_is_corked(s);
#endif
}

void QSoundEffectPrivate::stream_cork_callback(pa_stream *s, int success, void *userdata)
{
    Q_UNUSED(s);
    if (!success)
        qWarning("QSoundEffect(pulseaudio): faild to stop");
    QSoundEffectPrivate *self = reinterpret_cast<QSoundEffectPrivate*>(userdata);
#ifdef QT_PA_DEBUG
    qDebug() << self << "stream_cork_callback";
#endif
    QMetaObject::invokeMethod(self, "prepare", Qt::QueuedConnection);
}

void QSoundEffectPrivate::stream_flush_callback(pa_stream *s, int success, void *userdata)
{
    Q_UNUSED(s);
    if (!success)
        qWarning("QSoundEffect(pulseaudio): faild to drain");
    QSoundEffectPrivate *self = reinterpret_cast<QSoundEffectPrivate*>(userdata);
#ifdef QT_PA_DEBUG
    qDebug() << self << "stream_flush_callback";
#endif
    QMetaObject::invokeMethod(self, "emptyComplete", Qt::QueuedConnection);
}

void QSoundEffectPrivate::stream_write_done_callback(void *p)
{
    Q_UNUSED(p);
#ifdef QT_PA_DEBUG
    qDebug() << "stream_write_done_callback";
#endif
}

QT_END_NAMESPACE

#include "moc_qsoundeffect_pulse_p.cpp"
#include "qsoundeffect_pulse_p.moc"
