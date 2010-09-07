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

#ifndef AUDIOCAPTURESESSION_H
#define AUDIOCAPTURESESSION_H

#include <QFile>
#include <QUrl>
#include <QDir>

#include "audioencodercontrol.h"
#include "audioendpointselector.h"
#include "audiomediarecordercontrol.h"

#include <qaudioformat.h>
#include <qaudioinput.h>
#include <qaudiodeviceinfo.h>

QT_USE_NAMESPACE

class AudioCaptureSession : public QObject
{
    Q_OBJECT

public:
    AudioCaptureSession(QObject *parent = 0);
    ~AudioCaptureSession();

    QAudioFormat format() const;
    QAudioDeviceInfo* deviceInfo() const;
    bool isFormatSupported(const QAudioFormat &format) const;
    bool setFormat(const QAudioFormat &format);
    QStringList supportedContainers() const;
    QString containerMimeType() const;
    void setContainerMimeType(const QString &formatMimeType);
    QString containerDescription(const QString &formatMimeType) const;

    QUrl outputLocation() const;
    bool setOutputLocation(const QUrl& sink);
    qint64 position() const;
    int state() const;
    void record();
    void pause();
    void stop();

public slots:
    void setCaptureDevice(const QString &deviceName);

signals:
    void stateChanged(QMediaRecorder::State state);
    void positionChanged(qint64 position);
    void error(int error, const QString &errorString);

private slots:
    void stateChanged(QAudio::State state);
    void notify();

private:
    QDir defaultDir() const;
    QString generateFileName(const QDir &dir, const QString &ext) const;

    QFile file;
    QString m_captureDevice;
    QUrl m_sink;
    QUrl m_actualSink;
    QMediaRecorder::State m_state;
    QAudioInput *m_audioInput;
    QAudioDeviceInfo *m_deviceInfo;
    QAudioFormat m_format;
    qint64 m_position;
    bool wavFile;

    // WAV header stuff

    struct chunk
    {
        char        id[4];
        quint32     size;
    };

    struct RIFFHeader
    {
        chunk       descriptor;
        char        type[4];
    };

    struct WAVEHeader
    {
        chunk       descriptor;
        quint16     audioFormat;        // PCM = 1
        quint16     numChannels;
        quint32     sampleRate;
        quint32     byteRate;
        quint16     blockAlign;
        quint16     bitsPerSample;
    };

    struct DATAHeader
    {
        chunk       descriptor;
//        quint8      data[];
    };

    struct CombinedHeader
    {
        RIFFHeader  riff;
        WAVEHeader  wave;
        DATAHeader  data;
    };

    CombinedHeader      header;
};

#endif
