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
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifndef QAUDIOINPUTPULSE_H
#define QAUDIOINPUTPULSE_H

#include <QtCore/qfile.h>
#include <QtCore/qtimer.h>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qdatetime.h>

#include "qaudio.h"
#include "qaudiodeviceinfo.h"
#include "qaudiosystem.h"

#include <pulse/pulseaudio.h>

QT_BEGIN_NAMESPACE

class InputPrivate;

class QPulseAudioInput : public QAbstractAudioInput
{
    Q_OBJECT

public:
    QPulseAudioInput(const QByteArray &device);
    ~QPulseAudioInput();

    qint64 read(char *data, qint64 len);

    void start(QIODevice *device);
    QIODevice *start();
    void stop();
    void reset();
    void suspend();
    void resume();
    int bytesReady() const;
    int periodSize() const;
    void setBufferSize(int value);
    int bufferSize() const;
    void setNotifyInterval(int milliSeconds);
    int notifyInterval() const;
    qint64 processedUSecs() const;
    qint64 elapsedUSecs() const;
    QAudio::Error error() const;
    QAudio::State state() const;
    void setFormat(const QAudioFormat &format);
    QAudioFormat format() const;

    qint64 m_totalTimeValue;
    QIODevice *m_audioSource;
    QAudioFormat m_format;
    QAudio::Error m_errorState;
    QAudio::State m_deviceState;

private slots:
    void userFeed();
    bool deviceReady();

private:
    int checkBytesReady();
    bool open();
    void close();

    bool m_pullMode;
    bool m_opened;
    int m_bytesAvailable;
    int m_bufferSize;
    int m_periodSize;
    int m_intervalTime;
    unsigned int m_bufferTime;
    unsigned int m_periodTime;
    QTimer *m_timer;
    qint64 m_elapsedTimeOffset;
    char *audioBuffer;
    pa_stream *m_stream;
    QTime m_timeStamp;
    QTime m_clockStamp;
    QByteArray m_streamName;
    QByteArray m_device;
    QByteArray m_tempBuffer;
};

class InputPrivate : public QIODevice
{
    Q_OBJECT
public:
    InputPrivate(QPulseAudioInput *audio);
    ~InputPrivate() {};

    qint64 readData(char *data, qint64 len);
    qint64 writeData(const char *data, qint64 len);

    void trigger();

private:
    QPulseAudioInput *m_audioDevice;
};

QT_END_NAMESPACE

#endif
