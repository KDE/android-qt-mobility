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

/* Audio related snippets */
#include <QFile>
#include <QTimer>
#include <QDebug>

#include "qaudiodeviceinfo.h"
#include "qaudioinput.h"
#include "qaudiooutput.h"

class AudioInputExample : public QObject {
    Q_OBJECT
public:
    void setup();


public Q_SLOTS:
    void stopRecording();
    void stateChanged(QAudio::State newState);

private:
    //! [Audio input class members]
    QFile destinationFile;   // class member.
    QAudioInput* audio; // class member.
    //! [Audio input class members]
};


void AudioInputExample::setup()
//! [Audio input setup]
{
    destinationFile.setFileName("/tmp/test.raw");
    destinationFile.open( QIODevice::WriteOnly | QIODevice::Truncate );

    QAudioFormat format;
    // set up the format you want, eg.
    format.setFrequency(8000);
    format.setChannels(1);
    format.setSampleSize(8);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(format)) {
        qWarning()<<"default format not supported try to use nearest";
        format = info.nearestFormat(format);
    }

    audio = new QAudioInput(format, this);
    connect(audio, SIGNAL(stateChanged(QAudio::State)), this, SLOT(stateChanged(QAudio::State)));

    QTimer::singleShot(3000, this, SLOT(stopRecording()));
    audio->start(&destinationFile);
    // Records audio for 3000ms
}
//! [Audio input setup]

//! [Audio input stop recording]
void AudioInputExample::stopRecording()
{
    audio->stop();
    destinationFile.close();
    delete audio;
}
//! [Audio input stop recording]

//! [Audio input state changed]
void AudioInputExample::stateChanged(QAudio::State newState)
{
    switch (newState) {
    case QAudio::StoppedState:
        if (audio->error() != QAudio::NoError) {
            // Error handling
        } else {
            // Finished recording
        }
        break;

        // ...
    }
}
//! [Audio input state changed]


class AudioOutputExample : public QObject {
    Q_OBJECT
public:
    void setup();

public Q_SLOTS:
    void stateChanged(QAudio::State newState);

private:
    //! [Audio output class members]
    QFile sourceFile;   // class member.
    QAudioOutput* audio; // class member.
    //! [Audio output class members]
};


void AudioOutputExample::setup()
//! [Audio output setup]
{
    sourceFile.setFileName("/tmp/test.raw");
    sourceFile.open(QIODevice::ReadOnly);

    QAudioFormat format;
    // Set up the format, eg.
    format.setFrequency(8000);
    format.setChannels(1);
    format.setSampleSize(8);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format)) {
        qWarning() << "raw audio format not supported by backend, cannot play audio.";
        return;
    }

    audio = new QAudioOutput(format, this);
    connect(audio, SIGNAL(stateChanged(QAudio::State)), this, SLOT(stateChanged(QAudio::State)));
    audio->start(&sourceFile);
}
//! [Audio output setup]

//! [Audio output state changed]
void AudioOutputExample::stateChanged(QAudio::State newState)
{
    switch (newState) {
        case QAudio::IdleState:
            // Finished playing (no more data)
            audio->stop();
            sourceFile.close();
            delete audio;
            break;

        case QAudio::StoppedState:
            // Stopped for other reasons
            if (audio->error() != QAudio::NoError) {
                // Error handling
            }
            break;

            // ...
    }
}
//! [Audio output state changed]

void AudioDeviceInfo()
{
    //! [Setting audio format]
    QAudioFormat format;
    format.setFrequency(44100);
    // ... other format parameters
    format.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());

    if (!info.isFormatSupported(format))
        format = info.nearestFormat(format);
    //! [Setting audio format]

    //! [Dumping audio formats]
    foreach (const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
        qDebug() << "Device name: " << deviceInfo.deviceName();
    //! [Dumping audio formats]
}
