/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef AUDIOENCODERCONTROL_H
#define AUDIOENCODERCONTROL_H

#include <qaudioencodercontrol.h>
#include <QtCore/qstringlist.h>
#include <QAudioFormat>

QT_USE_NAMESPACE

class S60AudioCaptureSession;

class S60AudioEncoderControl : public QAudioEncoderControl
{
    Q_OBJECT
public:
    S60AudioEncoderControl(QObject *session, QObject *parent = 0);
    virtual ~S60AudioEncoderControl();

    QStringList supportedAudioCodecs() const;
    QString codecDescription(const QString &codecName) const;
    
    QList<int> supportedSampleRates(const QAudioEncoderSettings &settings, bool *continuous = 0) const;
    
    QAudioEncoderSettings audioSettings() const;
    void setAudioSettings(const QAudioEncoderSettings&);
    
    QStringList supportedEncodingOptions(const QString &codec) const;
    QVariant encodingOption(const QString &codec, const QString &name) const;
    void setEncodingOption(const QString &codec, const QString &name, const QVariant &value);    
    
private:    
    QString audioCodec() const;
    bool setAudioCodec(const QString &codecName);
    int bitRate() const;
    void setBitRate(int);

    QtMediaServices::EncodingQuality quality() const;
    void setQuality(QtMediaServices::EncodingQuality);

    int sampleRate() const;
    void setSampleRate(int sampleRate);
    int channelCount() const;
    void setChannelCount(int channels);    
    int sampleSize() const;
    void setSampleSize(int sampleSize);    
private:
    S60AudioCaptureSession* m_session;
    QtMediaServices::EncodingQuality m_quality;
};

#endif
