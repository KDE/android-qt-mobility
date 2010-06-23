/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef AUDIORECORDER_H
#define AUDIORECORDER_H

#include <QtGui>
#include <QPair>

#include <qmediarecorder.h>

QT_BEGIN_NAMESPACE
class QComboBox;
class QLabel;

class QAudioCaptureSource;
QT_END_NAMESPACE

QT_USE_NAMESPACE

class AudioRecorder : public QMainWindow
{
    Q_OBJECT
public:
    AudioRecorder();
    ~AudioRecorder();

private:
    QUrl recordPathAudio(QUrl filePath);
    void updateSamplerates(int idx);
    void updateChannelCount(int idx);
    void updateQuality(int idx);
    static inline void swap(QString& a, QString& b){qSwap(a,b);}
    
private slots:
    void deviceChanged(int idx);
    void containerChanged(int idx);
    void codecChanged(int idx);
    void qualityChanged(int idx);
    void sampleRateChanged(int idx);
    void channelCountChanged(int idx);
    void encmodeChanged(int idx);
    void selectOutputFile();
    void togglePause();
    void toggleRecord();
    void stateChanged(QMediaRecorder::State);
    void updateProgress(qint64 pos);
    void errorChanged(QMediaRecorder::Error);

private:
    QAudioCaptureSource* audiosource;
    QMediaRecorder* capture;

    QComboBox*     deviceBox;
    QComboBox*     containersBox;
    QComboBox*     codecsBox;
    QComboBox*     qualityBox;
    QComboBox*     sampleRateBox; 
    QComboBox*     channelBox;
    QComboBox*     encModeBox;
    QLabel*        recTime;
    QLabel*        statusLabel;
    QPushButton*   button;
    QPushButton*   fileButton;
    QPushButton*   pauseButton;
    bool           active;
    int            currentTime;
    QUrl           destination;
    bool           paused; 
};

#endif
