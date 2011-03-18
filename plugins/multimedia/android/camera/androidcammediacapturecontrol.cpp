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

#include <androidcammediacapturecontrol.h>
#include <QDateTime>


AndroidCamMediaCaptureControl::AndroidCamMediaCaptureControl(QObject *parent) :
        QMediaRecorderControl(parent)

{
    m_state= QMediaRecorder::StoppedState;
    m_sink.setPath(tr("/sdcard/"));
}

AndroidCamMediaCaptureControl::~AndroidCamMediaCaptureControl()
{

}

void AndroidCamMediaCaptureControl::applySettings()
{
    //no implementation
}

QUrl AndroidCamMediaCaptureControl::outputLocation() const
{
    return m_sink;
}

bool AndroidCamMediaCaptureControl::setOutputLocation(const QUrl &sink)
{
    m_sink=sink;
    QString path(m_sink.path());
    QtCameraJni::setVideoOutputLocation(path);
    return 1;
}

QMediaRecorder::State AndroidCamMediaCaptureControl::state() const
{
    return m_state;
}

qint64 AndroidCamMediaCaptureControl::duration() const
{
    if(m_duration.isValid())
    {
        return m_duration.elapsed();
    }
    else
    {
        return (qint64)0;
    }
}

bool AndroidCamMediaCaptureControl::isMuted() const
{
    return false;
}

void AndroidCamMediaCaptureControl::record()
{
    m_state=QMediaRecorder::RecordingState;
    m_duration.start();
    QtCameraJni::startrecord(this);
    updateState();

}

void AndroidCamMediaCaptureControl::pause()
{
    //not supported in android
}

void AndroidCamMediaCaptureControl::stop()
{
    m_duration.invalidate();
    QtCameraJni::stoprecord();
    m_state= QMediaRecorder::StoppedState;
    updateState();
}

void AndroidCamMediaCaptureControl::setRecordStop()
{
    m_duration.invalidate();
    m_state= QMediaRecorder::StoppedState;
    updateState();
}

void AndroidCamMediaCaptureControl::updateState()
{
    emit stateChanged(m_state);
}

void AndroidCamMediaCaptureControl::setMuted(bool sound)
{
    Q_UNUSED(sound);
}
