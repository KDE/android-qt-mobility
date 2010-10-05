/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef S60AUDIOENCODERCONTROL_H
#define S60AUDIOENCODERCONTROL_H

#include <QtCore/qstringlist.h>
#include <QtCore/qmap.h>

#include <qaudioencodercontrol.h>

QT_USE_NAMESPACE

class S60VideoCaptureSession;

/*
 * Control for audio settings when recording video using QMediaRecorder.
 */
class S60AudioEncoderControl : public QAudioEncoderControl
{
    Q_OBJECT
    
public: // Constructor & Destructor
    
    S60AudioEncoderControl(QObject *parent = 0);
    S60AudioEncoderControl(S60VideoCaptureSession *session, QObject *parent = 0);
    virtual ~S60AudioEncoderControl();

public: // QAudioEncoderControl
    
    // Audio Codec
    QStringList supportedAudioCodecs() const;
    QString codecDescription(const QString &codecName) const;

    // Sample Rate
    QList<int> supportedSampleRates(const QAudioEncoderSettings &settings, bool *continuous = 0) const;

    // Audio Settings
    QAudioEncoderSettings audioSettings() const;
    void setAudioSettings(const QAudioEncoderSettings &settings);

    // Encoding Option
    QStringList supportedEncodingOptions(const QString &codec) const;
    QVariant encodingOption(const QString &codec, const QString &name) const;
    void setEncodingOption(const QString &codec, const QString &name, const QVariant &value);

private: // Data

    S60VideoCaptureSession* m_session;
};

#endif // S60AUDIOENCODERCONTROL_H
