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

#include <QtCore/qcoreapplication.h>
#include <QtCore/qdebug.h>

#include "qaudiooutput_pulse.h"
#include "qaudiodeviceinfo_pulse.h"
#include "qpulseaudioengine.h"
#include "qpulsehelpers.h"

QT_BEGIN_NAMESPACE

const int PeriodTimeMs = 20;

static void  outputStreamWriteCallback(pa_stream *stream, size_t length, void *userdata)
{
    Q_UNUSED(stream);
    Q_UNUSED(length);
    Q_UNUSED(userdata);
    QPulseAudioEngine *pulseEngine = QPulseAudioEngine::instance();
    pa_threaded_mainloop_signal(pulseEngine->mainloop(), 0);
}

static void outputStreamStateCallback(pa_stream *stream, void *userdata)
{
    Q_UNUSED(userdata)
    pa_stream_state_t state = pa_stream_get_state(stream);
#ifdef DEBUG_PULSE
    qDebug() << "Stream state: " << QPulseAudioInternal::stateToQString(state);
#endif
    switch (state) {
        case PA_STREAM_CREATING:
        case PA_STREAM_READY:
        case PA_STREAM_TERMINATED:
            break;

        case PA_STREAM_FAILED:
        default:
            qWarning() << QString("Stream error: %1").arg(pa_strerror(pa_context_errno(pa_stream_get_context(stream))));
            QPulseAudioEngine *pulseEngine = QPulseAudioEngine::instance();
            pa_threaded_mainloop_signal(pulseEngine->mainloop(), 0);
            break;
    }
}

static void outputStreamUnderflowCallback(pa_stream *stream, void *userdata)
{
    Q_UNUSED(stream)
    ((QPulseAudioOutput*)userdata)->streamUnderflowCallback();
    qWarning() << "Got a buffer underflow!";
}

static void outputStreamOverflowCallback(pa_stream *stream, void *userdata)
{
    Q_UNUSED(stream)
    Q_UNUSED(userdata)
    qWarning() << "Got a buffer overflow!";
}

static void outputStreamLatencyCallback(pa_stream *stream, void *userdata)
{
    Q_UNUSED(stream)
    Q_UNUSED(userdata)

#ifdef DEBUG_PULSE
    const pa_timing_info *info = pa_stream_get_timing_info(stream);

    qDebug() << "Write index corrupt: " << info->write_index_corrupt;
    qDebug() << "Write index: " << info->write_index;
    qDebug() << "Read index corrupt: " << info->read_index_corrupt;
    qDebug() << "Read index: " << info->read_index;
    qDebug() << "Sink usec: " << info->sink_usec;
    qDebug() << "Configured sink usec: " << info->configured_sink_usec;
#endif
}

static void outputStreamSuccessCallback(pa_stream *stream, int success, void *userdata)
{
    Q_UNUSED(stream);
    Q_UNUSED(success);
    Q_UNUSED(userdata);

    QPulseAudioEngine *pulseEngine = QPulseAudioEngine::instance();
    pa_threaded_mainloop_signal(pulseEngine->mainloop(), 0);
}

static void outputStreamDrainComplete(pa_stream *stream, int success, void *userdata)
{
    Q_UNUSED(stream);
    Q_UNUSED(success);
    Q_UNUSED(userdata);

#ifdef DEBUG_PULSE
    qDebug() << "Draining completed successfully: " << (bool)success;
#endif
}

QPulseAudioOutput::QPulseAudioOutput(const QByteArray &device)
    : m_device(device)
    , m_errorState(QAudio::NoError)
    , m_deviceState(QAudio::StoppedState)
    , m_pullMode(true)
    , m_opened(false)
    , m_audioSource(0)
    , m_bytesAvailable(0)
    , m_stream(0)
    , m_notifyInterval(1000)
    , m_periodSize(0)
    , m_bufferSize(0)
    , m_totalTimeValue(0)
    , m_tickTimer(new QTimer(this))
    , m_audioBuffer(0)
    , m_resuming(false)
{
    connect(m_tickTimer, SIGNAL(timeout()), SLOT(userFeed()));
}

QPulseAudioOutput::~QPulseAudioOutput()
{
    close();
    disconnect(m_tickTimer, SIGNAL(timeout()));
    QCoreApplication::processEvents();
}

QAudio::Error QPulseAudioOutput::error() const
{
    return m_errorState;
}

QAudio::State QPulseAudioOutput::state() const
{
    return m_deviceState;
}

void QPulseAudioOutput::streamUnderflowCallback()
{
    if (m_deviceState != QAudio::IdleState && !m_resuming) {
        m_errorState = QAudio::UnderrunError;
        emit errorChanged(m_errorState);
        m_deviceState = QAudio::IdleState;
        emit stateChanged(m_deviceState);
    }
}

void QPulseAudioOutput::start(QIODevice *device)
{
    if (m_deviceState != QAudio::StoppedState)
        m_deviceState = QAudio::StoppedState;

    m_errorState = QAudio::NoError;

    // Handle change of mode
    if (m_audioSource && !m_pullMode) {
        delete m_audioSource;
        m_audioSource = 0;
    }

    close();

    m_pullMode = true;
    m_audioSource = device;

    m_deviceState = QAudio::ActiveState;

    open();

    emit stateChanged(m_deviceState);
}

QIODevice *QPulseAudioOutput::start()
{
    if (m_deviceState != QAudio::StoppedState)
        m_deviceState = QAudio::StoppedState;

    m_errorState = QAudio::NoError;

    // Handle change of mode
    if (m_audioSource && !m_pullMode) {
        delete m_audioSource;
        m_audioSource = 0;
    }

    close();

    m_audioSource = new OutputPrivate(this);
    m_audioSource->open(QIODevice::WriteOnly|QIODevice::Unbuffered);
    m_pullMode = false;

    m_deviceState = QAudio::IdleState;

    open();

    emit stateChanged(m_deviceState);

    return m_audioSource;
}

bool QPulseAudioOutput::open()
{
    if (m_opened)
        return false;

    pa_sample_spec spec = QPulseAudioInternal::audioFormatToSampleSpec(m_format);

    if (!pa_sample_spec_valid(&spec)) {
        m_errorState = QAudio::OpenError;
        m_deviceState = QAudio::StoppedState;
        return false;
    }

    m_totalTimeValue = 0;
    m_elapsedTimeOffset = 0;
    m_timeStamp.restart();

    if (m_streamName.isNull())
        m_streamName = QString(QLatin1String("QtmPulseStream-%1-%2")).arg(::getpid()).arg(quintptr(this)).toUtf8();

#ifdef DEBUG_PULSE
        qDebug() << "Format: " << QPulseAudioInternal::sampleFormatToQString(spec.format);
        qDebug() << "Rate: " << spec.rate;
        qDebug() << "Channels: " << spec.channels;
        qDebug() << "Frame size: " << pa_frame_size(&spec);
#endif

    QPulseAudioEngine *pulseEngine = QPulseAudioEngine::instance();
    pa_threaded_mainloop_lock(pulseEngine->mainloop());
    m_stream = pa_stream_new(pulseEngine->context(), m_streamName.constData(), &spec, 0);

    pa_stream_set_state_callback(m_stream, outputStreamStateCallback, this);
    pa_stream_set_write_callback(m_stream, outputStreamWriteCallback, this);

    pa_stream_set_underflow_callback(m_stream, outputStreamUnderflowCallback, this);
    pa_stream_set_overflow_callback(m_stream, outputStreamOverflowCallback, this);
    pa_stream_set_latency_update_callback(m_stream, outputStreamLatencyCallback, this);

    if (pa_stream_connect_playback(m_stream, m_device.data(), NULL, (pa_stream_flags_t)0, NULL, NULL) < 0) {
        qWarning() << "pa_stream_connect_playback() failed!";
        return false;
    }

    while (pa_stream_get_state(m_stream) != PA_STREAM_READY) {
        pa_threaded_mainloop_wait(pulseEngine->mainloop());
    }
    pa_threaded_mainloop_unlock(pulseEngine->mainloop());

#ifdef DEBUG_PULSE
    const pa_buffer_attr *buffer = pa_stream_get_buffer_attr(m_stream);

    qDebug() << "Buffering info:";
    qDebug() << "\tMax length: " << buffer->maxlength;
    qDebug() << "\tTarget length: " << buffer->tlength;
    qDebug() << "\tPre-buffering: " << buffer->prebuf;
    qDebug() << "\tMinimum request: " << buffer->minreq;
    qDebug() << "\tFragment size: " << buffer->fragsize;
#endif

    m_periodSize = pa_usec_to_bytes(PeriodTimeMs*1000, &spec);
    m_opened = true;
    m_tickTimer->start(PeriodTimeMs);

    m_elapsedTimeOffset = 0;
    m_timeStamp.restart();
    m_clockStamp.restart();

    return true;
}

void QPulseAudioOutput::userFeed()
{
    if (m_deviceState == QAudio::StoppedState || m_deviceState == QAudio::SuspendedState)
        return;

    if (m_deviceState ==  QAudio::IdleState)
        m_bytesAvailable = bytesFree();

    deviceReady();
}

void QPulseAudioOutput::close()
{
    m_tickTimer->stop();

    if (m_stream) {
        QPulseAudioEngine *pulseEngine = QPulseAudioEngine::instance();
        pa_threaded_mainloop_lock(pulseEngine->mainloop());
        pa_operation *o;

        pa_stream_set_write_callback(m_stream, NULL, NULL);

        if (!(o = pa_stream_drain(m_stream, outputStreamDrainComplete, NULL))) {
            qWarning() << QString("pa_stream_drain(): %1").arg(pa_strerror(pa_context_errno(pa_stream_get_context(m_stream))));
            return;
        }
        pa_operation_unref(o);

        pa_stream_disconnect(m_stream);
        pa_stream_unref(m_stream);
        m_stream = NULL;

        pa_threaded_mainloop_unlock(pulseEngine->mainloop());
    }

    if (!m_pullMode && m_audioSource) {
        delete m_audioSource;
        m_audioSource = 0;
    }
    m_opened = false;
}

bool QPulseAudioOutput::deviceReady()
{
    m_resuming = false;

    if (m_pullMode) {
        int l = 0;
        int chunks = m_bytesAvailable/m_periodSize;
        if (chunks==0) {
            m_bytesAvailable = bytesFree();
            return false;
        }

        char buffer[m_periodSize];

        l = m_audioSource->read(buffer, m_periodSize);
        if (l > 0) {
            if (m_deviceState != QAudio::ActiveState)
                return true;

            qint64 bytesWritten = write(buffer, l);
            Q_UNUSED(bytesWritten);
        }
    }

    if (m_deviceState != QAudio::ActiveState)
        return true;

    if (m_notifyInterval && (m_timeStamp.elapsed() + m_elapsedTimeOffset) > m_notifyInterval) {
        emit notify();
        m_elapsedTimeOffset = m_timeStamp.elapsed() + m_elapsedTimeOffset - m_notifyInterval;
        m_timeStamp.restart();
    }

    return true;
}

qint64 QPulseAudioOutput::write(const char *data, qint64 len)
{
    QPulseAudioEngine *pulseEngine = QPulseAudioEngine::instance();

    pa_threaded_mainloop_lock(pulseEngine->mainloop());
    len = qMin(len, static_cast<qint64>(pa_stream_writable_size(m_stream)));
    pa_stream_write(m_stream, data, len, 0, 0, PA_SEEK_RELATIVE);
    pa_threaded_mainloop_unlock(pulseEngine->mainloop());
    m_totalTimeValue += len;

    m_errorState = QAudio::NoError;
    if (m_deviceState != QAudio::ActiveState) {
        m_deviceState = QAudio::ActiveState;
        emit stateChanged(m_deviceState);
    }

    return len;
}

void QPulseAudioOutput::stop()
{
    if (m_deviceState == QAudio::StoppedState)
        return;

    m_errorState = QAudio::NoError;
    m_deviceState = QAudio::StoppedState;
    close();
    emit stateChanged(m_deviceState);
}

int QPulseAudioOutput::bytesFree() const
{
    if (m_deviceState != QAudio::ActiveState && m_deviceState != QAudio::IdleState)
        return 0;

    return pa_stream_writable_size(m_stream);
}

int QPulseAudioOutput::periodSize() const
{
    return m_periodSize;
}

void QPulseAudioOutput::setBufferSize(int value)
{
    m_bufferSize = value;
}

int QPulseAudioOutput::bufferSize() const
{
    return m_bufferSize;
}

void QPulseAudioOutput::setNotifyInterval(int ms)
{
    m_notifyInterval = qMax(0, ms);
}

int QPulseAudioOutput::notifyInterval() const
{
    return m_notifyInterval;
}

qint64 QPulseAudioOutput::processedUSecs() const
{
    qint64 result = qint64(1000000) * m_totalTimeValue /
        (m_format.channels() * (m_format.sampleSize() / 8)) /
        m_format.frequency();

    return result;
}

void QPulseAudioOutput::resume()
{
    if (m_deviceState == QAudio::SuspendedState) {
        m_resuming = true;

        QPulseAudioEngine *pulseEngine = QPulseAudioEngine::instance();

        pa_threaded_mainloop_lock(pulseEngine->mainloop());

        pa_operation *operation = pa_stream_cork(m_stream, 0, outputStreamSuccessCallback, NULL);

        while (pa_operation_get_state(operation) == PA_OPERATION_RUNNING)
            pa_threaded_mainloop_wait(pulseEngine->mainloop());

        pa_operation_unref(operation);

        operation = pa_stream_trigger(m_stream, outputStreamSuccessCallback, NULL);

        while (pa_operation_get_state(operation) == PA_OPERATION_RUNNING)
            pa_threaded_mainloop_wait(pulseEngine->mainloop());

        pa_operation_unref(operation);

        pa_threaded_mainloop_unlock(pulseEngine->mainloop());

        m_deviceState = QAudio::ActiveState;

        m_errorState = QAudio::NoError;
        m_tickTimer->start(PeriodTimeMs);

        emit stateChanged(m_deviceState);
    }
}

void QPulseAudioOutput::setFormat(const QAudioFormat &format)
{
    m_format = format;
}

QAudioFormat QPulseAudioOutput::format() const
{
    return m_format;
}

void QPulseAudioOutput::suspend()
{
    if (m_deviceState == QAudio::ActiveState || m_deviceState == QAudio::IdleState) {
        m_tickTimer->stop();
        m_deviceState = QAudio::SuspendedState;
        m_errorState = QAudio::NoError;
        emit stateChanged(m_deviceState);

        QPulseAudioEngine *pulseEngine = QPulseAudioEngine::instance();
        pa_operation *operation;

        pa_threaded_mainloop_lock(pulseEngine->mainloop());

        operation = pa_stream_cork(m_stream, 1, outputStreamSuccessCallback, NULL);

        while (pa_operation_get_state(operation) == PA_OPERATION_RUNNING)
            pa_threaded_mainloop_wait(pulseEngine->mainloop());

        pa_operation_unref(operation);

        pa_threaded_mainloop_unlock(pulseEngine->mainloop());
    }
}

qint64 QPulseAudioOutput::elapsedUSecs() const
{
    if (m_deviceState == QAudio::StoppedState)
        return 0;

    return m_clockStamp.elapsed() * 1000;
}

void QPulseAudioOutput::reset()
{
    stop();
}

OutputPrivate::OutputPrivate(QPulseAudioOutput *audio)
{
    m_audioDevice = qobject_cast<QPulseAudioOutput*>(audio);
}

qint64 OutputPrivate::readData(char *data, qint64 len)
{
    Q_UNUSED(data)
    Q_UNUSED(len)

    return 0;
}

qint64 OutputPrivate::writeData(const char *data, qint64 len)
{
    int retry = 0;
    qint64 written = 0;

    if ((m_audioDevice->m_deviceState == QAudio::ActiveState)
            ||(m_audioDevice->m_deviceState == QAudio::IdleState)) {
         while(written < len) {
            int chunk = m_audioDevice->write(data+written, (len-written));
            if (chunk <= 0)
                retry++;
            written+=chunk;
            if (retry > 10)
                return written;
        }
    }

    return written;
}

QT_END_NAMESPACE

#include "moc_qaudiooutput_pulse.cpp"
