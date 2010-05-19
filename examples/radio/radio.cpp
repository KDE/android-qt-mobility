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

#include "radio.h"

#include <QtGui>

Radio::Radio()
{
    radio = new QRadioTuner;
    connect(radio,SIGNAL(frequencyChanged(int)),this,SLOT(freqChanged(int)));
    connect(radio,SIGNAL(signalStrengthChanged(int)),this,SLOT(signalChanged(int)));

    if(radio->isBandSupported(QRadioTuner::FM))
        radio->setBand(QRadioTuner::FM);

    QWidget *window = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;
    QHBoxLayout* buttonBar = new QHBoxLayout;
#if defined Q_OS_SYMBIAN // this is so that we can see all buttons also in 3.1 devices, where the screens are smaller..
    QHBoxLayout* buttonBar2 = new QHBoxLayout;
#endif
    QHBoxLayout* topBar = new QHBoxLayout;

    layout->addLayout(topBar);

    freq = new QLabel;
    freq->setText(QString("%1 kHz").arg(radio->frequency()/1000));
    topBar->addWidget(freq);

    signal = new QLabel;
    if (radio->isAvailable())
        signal->setText(tr("No Signal"));
    else
        signal->setText(tr("No radio found"));
    topBar->addWidget(signal);

    volumeSlider = new QSlider(Qt::Vertical,this);
    volumeSlider->setRange(0,100);
#if defined Q_OS_SYMBIAN
    volumeSlider->setRange(0,10);
#endif
    volumeSlider->setValue(radio->volume());
    connect(volumeSlider,SIGNAL(valueChanged(int)),this,SLOT(updateVolume(int)));
    topBar->addWidget(volumeSlider);

    layout->addLayout(buttonBar);
#if defined Q_OS_SYMBIAN
    layout->addLayout(buttonBar2);
#endif

    searchLeft = new QPushButton;
    searchLeft->setText(tr("scan Down"));
    connect(searchLeft,SIGNAL(clicked()),SLOT(searchDown()));
    buttonBar->addWidget(searchLeft);

    left = new QPushButton;
    left->setText(tr("Freq Down"));
    connect(left,SIGNAL(clicked()),SLOT(freqDown()));
#if defined Q_OS_SYMBIAN
    buttonBar2->addWidget(left);
#else
    buttonBar->addWidget(left);
#endif

    right = new QPushButton;
    connect(right,SIGNAL(clicked()),SLOT(freqUp()));
    right->setText(tr("Freq Up"));
#if defined Q_OS_SYMBIAN
    buttonBar2->addWidget(right);
#else
    buttonBar->addWidget(right);
#endif

    searchRight = new QPushButton;
    searchRight->setText(tr("scan Up"));
    connect(searchRight,SIGNAL(clicked()),SLOT(searchUp()));
    buttonBar->addWidget(searchRight);

    window->setLayout(layout);
    setCentralWidget(window);
    window->show();

    radio->start();
}

Radio::~Radio()
{
}

void Radio::freqUp()
{
    int f = radio->frequency();
    f = f + radio->frequencyStep(QRadioTuner::FM);
    radio->setFrequency(f);
}

void Radio::freqDown()
{
    int f = radio->frequency();
    f = f - radio->frequencyStep(QRadioTuner::FM);
    radio->setFrequency(f);
}

void Radio::searchUp()
{
    radio->searchForward();
}

void Radio::searchDown()
{
    radio->searchBackward();
}

void Radio::freqChanged(int)
{
    freq->setText(QString("%1 kHz").arg(radio->frequency()/1000));
}

void Radio::signalChanged(int)
{
    if(radio->signalStrength() > 25)
        signal->setText(tr("Got Signal"));
    else
        signal->setText(tr("No Signal"));
}

void Radio::updateVolume(int v)
{
    radio->setVolume(v);
}

