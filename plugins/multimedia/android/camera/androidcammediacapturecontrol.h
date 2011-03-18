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

#ifndef ANDROIDCAMMEDIACAPTURECONTROL_H
#define ANDROIDCAMMEDIACAPTURECONTROL_H

#include <QMediaRecorderControl>
#include <qmediaencodersettings.h>
#include "androidcamjni.h"
#include <QtCore/qurl.h>
#include <QElapsedTimer>

QT_USE_NAMESPACE


class AndroidCamMediaCaptureControl : public QMediaRecorderControl
{
    Q_OBJECT

public: // Contructors & Destrcutor

    AndroidCamMediaCaptureControl(QObject *parent = 0);
    ~AndroidCamMediaCaptureControl();

public: // QCameraImageCaptureControl

    QUrl outputLocation() const;
    bool setOutputLocation(const QUrl &sink);

    QMediaRecorder::State state() const;

    qint64 duration() const;

    bool isMuted() const;

    void applySettings();

    void setRecordStop();

    QElapsedTimer m_duration;

public Q_SLOTS: // QMediaRecorderControl

    void record();
    void pause();
    void stop();
    void setMuted(bool);

private slots:
    void updateState();

private:
    QMediaRecorder::State   m_state;
    QVideoEncoderSettings m_videsettings;
    QUrl m_sink;
    QString m_format;

};
#endif // ANDROIDCAMMEDIACAPTURECONTROL_H
