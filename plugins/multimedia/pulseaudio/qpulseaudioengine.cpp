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

#include <QtCore/qdebug.h>

#include <qaudiodeviceinfo.h>
#include "qpulseaudioengine.h"
#include "qaudiodeviceinfo_pulse.h"
#include "qaudiooutput_pulse.h"
#include "qpulsehelpers.h"

QT_BEGIN_NAMESPACE

static void serverInfoCallback(pa_context *context, const pa_server_info *info, void *userdata)
{
    if (!info) {
        qWarning() << QString("Failed to get server information: %s").arg(pa_strerror(pa_context_errno(context)));
        return;
    }

#ifdef DEBUG_PULSE
    char ss[PA_SAMPLE_SPEC_SNPRINT_MAX], cm[PA_CHANNEL_MAP_SNPRINT_MAX];

    pa_sample_spec_snprint(ss, sizeof(ss), &info->sample_spec);
    pa_channel_map_snprint(cm, sizeof(cm), &info->channel_map);

    qDebug() << QString("User name: %1\n"
             "Host Name: %2\n"
             "Server Name: %3\n"
             "Server Version: %4\n"
             "Default Sample Specification: %5\n"
             "Default Channel Map: %6\n"
             "Default Sink: %7\n"
             "Default Source: %8\n").arg(
           info->user_name,
           info->host_name,
           info->server_name,
           info->server_version,
           ss,
           cm,
           info->default_sink_name,
           info->default_source_name);
#endif

    QPulseAudioEngine *pulseEngine = static_cast<QPulseAudioEngine*>(userdata);
    pulseEngine->m_defaultSink = info->default_sink_name;
    pulseEngine->m_defaultSource = info->default_source_name;

    pa_threaded_mainloop_signal(pulseEngine->mainloop(), 0);
}

static void sinkInfoCallback(pa_context *context, const pa_sink_info *info, int isLast, void *userdata)
{
    QPulseAudioEngine *pulseEngine = static_cast<QPulseAudioEngine*>(userdata);
    QMap<pa_sink_state, QString> stateMap;
    stateMap[PA_SINK_INVALID_STATE] = "n/a";
    stateMap[PA_SINK_RUNNING] = "RUNNING";
    stateMap[PA_SINK_IDLE] = "IDLE";
    stateMap[PA_SINK_SUSPENDED] = "SUSPENDED";

    if (isLast < 0) {
        qWarning() << QString("Failed to get sink information: %s").arg(pa_strerror(pa_context_errno(context)));
        return;
    }

    if (isLast) {
        pa_threaded_mainloop_signal(pulseEngine->mainloop(), 0);
        return;
    }

    Q_ASSERT(info);

#ifdef DEBUG_PULSE
    qDebug() << QString("Sink #%1\n"
             "\tState: %2\n"
             "\tName: %3\n"
             "\tDescription: %4\n"
            ).arg(QString::number(info->index),
                  stateMap.value(info->state),
                  info->name,
                  info->description);
#endif

    QAudioFormat format = QPulseAudioInternal::sampleSpecToAudioFormat(info->sample_spec);
    pulseEngine->m_preferredFormats.insert(info->name, format);
    pulseEngine->m_sinks.append(info->name);
}

static void sourceInfoCallback(pa_context *context, const pa_source_info *info, int isLast, void *userdata)
{
    Q_UNUSED(context)
    QPulseAudioEngine *pulseEngine = static_cast<QPulseAudioEngine*>(userdata);

    QMap<pa_source_state, QString> stateMap;
    stateMap[PA_SOURCE_INVALID_STATE] = "n/a";
    stateMap[PA_SOURCE_RUNNING] = "RUNNING";
    stateMap[PA_SOURCE_IDLE] = "IDLE";
    stateMap[PA_SOURCE_SUSPENDED] = "SUSPENDED";

    if (isLast) {
        pa_threaded_mainloop_signal(pulseEngine->mainloop(), 0);
        return;
    }

    Q_ASSERT(info);

#ifdef DEBUG_PULSE
    qDebug() << QString("Source #%1\n"
         "\tState: %2\n"
         "\tName: %3\n"
         "\tDescription: %4\n"
        ).arg(QString::number(info->index),
              stateMap.value(info->state),
              info->name,
              info->description);
#endif

    QAudioFormat format = QPulseAudioInternal::sampleSpecToAudioFormat(info->sample_spec);
    pulseEngine->m_preferredFormats.insert(info->name, format);
    pulseEngine->m_sources.append(info->name);
}

static void contextStateCallbackInit(pa_context *context, void *userdata)
{
    Q_UNUSED(context);
#ifdef DEBUG_PULSE
    qDebug() << QPulseAudioInternal::stateToQString(pa_context_get_state(context));
#endif
    QPulseAudioEngine *pulseEngine = reinterpret_cast<QPulseAudioEngine*>(userdata);
    pa_threaded_mainloop_signal(pulseEngine->mainloop(), 0);
}

static void contextStateCallback(pa_context *context, void *userdata)
{
    Q_UNUSED(userdata);
    Q_UNUSED(context);

#ifdef DEBUG_PULSE
    pa_context_state_t state = pa_context_get_state(context);
    qDebug() << QPulseAudioInternal::stateToQString(state);
#endif
}

Q_GLOBAL_STATIC(QPulseAudioEngine, pulseEngine);

QPulseAudioEngine::QPulseAudioEngine(QObject *parent)
    : QObject(parent)
{
    bool keepGoing = true;
    bool ok = true;

    m_mainLoop = pa_threaded_mainloop_new();
    if (m_mainLoop == 0) {
        qWarning("Unable to create pulseaudio mainloop");
        return;
    }

    if (pa_threaded_mainloop_start(m_mainLoop) != 0) {
        qWarning("Unable to start pulseaudio mainloop");
        pa_threaded_mainloop_free(m_mainLoop);
        return;
    }

    m_mainLoopApi = pa_threaded_mainloop_get_api(m_mainLoop);

    pa_threaded_mainloop_lock(m_mainLoop);

    m_context = pa_context_new(m_mainLoopApi, QString(QLatin1String("QtmPulseContext:%1")).arg(::getpid()).toAscii().constData());
    pa_context_set_state_callback(m_context, contextStateCallbackInit, this);

    if (!m_context) {
        qWarning("Unable to create new pulseaudio context");
        pa_threaded_mainloop_free(m_mainLoop);
        return;
    }

    if (pa_context_connect(m_context, NULL, (pa_context_flags_t)0, NULL) < 0) {
        qWarning("Unable to create a connection to the pulseaudio context");
        pa_context_unref(m_context);
        pa_threaded_mainloop_free(m_mainLoop);
        return;
    }

    pa_threaded_mainloop_wait(m_mainLoop);

    while (keepGoing) {
        switch (pa_context_get_state(m_context)) {
            case PA_CONTEXT_CONNECTING:
            case PA_CONTEXT_AUTHORIZING:
            case PA_CONTEXT_SETTING_NAME:
                break;

            case PA_CONTEXT_READY:
#ifdef DEBUG_PULSE
                qDebug("Connection established.");
#endif
                keepGoing = false;
                break;

            case PA_CONTEXT_TERMINATED:
                qCritical("Context terminated.");
                keepGoing = false;
                ok = false;
                break;

            case PA_CONTEXT_FAILED:
            default:
                qCritical() << QString("Connection failure: %1").arg(pa_strerror(pa_context_errno(m_context)));
                keepGoing = false;
                ok = false;
        }

        if (keepGoing) {
            pa_threaded_mainloop_wait(m_mainLoop);
        }
    }

    if (ok) {
        pa_context_set_state_callback(m_context, contextStateCallback, this);
    } else {
        if (m_context) {
            pa_context_unref(m_context);
            m_context = 0;
        }
    }

    pa_threaded_mainloop_unlock(m_mainLoop);

    serverInfo();
    sinks();
    sources();
}

QPulseAudioEngine::~QPulseAudioEngine()
{
    if (m_context) {
        pa_threaded_mainloop_lock(m_mainLoop);
        pa_context_disconnect(m_context);
        pa_threaded_mainloop_unlock(m_mainLoop);
        m_context = 0;
    }

    if (m_mainLoop) {
        pa_threaded_mainloop_stop(m_mainLoop);
        pa_threaded_mainloop_free(m_mainLoop);
        m_mainLoop = 0;
    }
}

void QPulseAudioEngine::serverInfo()
{
    pa_operation *operation;

    pa_threaded_mainloop_lock(m_mainLoop);

    operation = pa_context_get_server_info(m_context, serverInfoCallback, this);

    while (pa_operation_get_state(operation) == PA_OPERATION_RUNNING)
        pa_threaded_mainloop_wait(m_mainLoop);

    pa_operation_unref(operation);

    pa_threaded_mainloop_unlock(m_mainLoop);
}

void QPulseAudioEngine::sinks()
{
    pa_operation *operation;

    pa_threaded_mainloop_lock(m_mainLoop);

    operation = pa_context_get_sink_info_list(m_context, sinkInfoCallback, this);

    while (pa_operation_get_state(operation) == PA_OPERATION_RUNNING)
        pa_threaded_mainloop_wait(m_mainLoop);

    pa_operation_unref(operation);

    pa_threaded_mainloop_unlock(m_mainLoop);

    // Swap the default sink to index 0
    m_sinks.removeOne(m_defaultSink);
    m_sinks.prepend(m_defaultSink);
}

void QPulseAudioEngine::sources()
{
    pa_operation *operation;

    pa_threaded_mainloop_lock(m_mainLoop);

    operation = pa_context_get_source_info_list(m_context, sourceInfoCallback, this);

    while (pa_operation_get_state(operation) == PA_OPERATION_RUNNING)
        pa_threaded_mainloop_wait(m_mainLoop);

    pa_operation_unref(operation);

    pa_threaded_mainloop_unlock(m_mainLoop);

    // Swap the default source to index 0
    m_sources.removeOne(m_defaultSource);
    m_sources.prepend(m_defaultSource);
}

QPulseAudioEngine *QPulseAudioEngine::instance()
{
    return pulseEngine();
}

QList<QByteArray> QPulseAudioEngine::availableDevices(QAudio::Mode mode) const
{
    return mode == QAudio::AudioOutput ? m_sinks : m_sources;
}

QT_END_NAMESPACE
