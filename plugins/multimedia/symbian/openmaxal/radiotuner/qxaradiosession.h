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

#ifndef QXARADIOSESSION_H
#define QXARADIOSESSION_H

#include <QObject>
#include <QUrl>
#include <qradiotuner.h>
#include "xaradiosessionimplobserver.h"

QT_USE_NAMESPACE

#include <e32debug.h>
#define SF_TRACE_FN_ENT_PRINT RDebug::Printf(">%s", __PRETTY_FUNCTION__)
#define SF_TRACE_FN_EXT_PRINT RDebug::Printf("<%s", __PRETTY_FUNCTION__)
#define SF_PRINT1_LOG(s) RDebug::Print(s)
#define SF_PRINT2_LOG(s,a1) RDebug::Print(s,a1)
#define SF_PRINT3_LOG(s,a1,a2) RDebug::Print(s,a1,a2)
#define SF_PRINT4_LOG(s,a1,a2,a3) RDebug::Print(s,a1,a2,a3)

class XARadioSessionImpl;

class QXARadioSession : public QObject, public XARadioSessionImplObserver
{
Q_OBJECT

public:
    QXARadioSession(QObject *parent);
    virtual ~QXARadioSession();

    QRadioTuner::State state() const;
    QRadioTuner::Band band() const;
    void setBand(QRadioTuner::Band band);
    bool isBandSupported(QRadioTuner::Band band) const;
    int frequency() const;
    int frequencyStep(QRadioTuner::Band b) const;
    QPair<int,int> frequencyRange(QRadioTuner::Band b) const;
    void setFrequency(int frequency);
    bool isStereo() const;
    QRadioTuner::StereoMode stereoMode() const;
    void setStereoMode(QRadioTuner::StereoMode mode);
    int signalStrength() const;
    int volume() const;
    int setVolume(int volume);
    bool isMuted() const;
    void setMuted(bool muted);
    bool isSearching() const;
    void searchForward();
    void searchBackward();
    void cancelSearch();
    void start();
    void stop();
    bool isAvailable() const;
    QtMultimediaKit::AvailabilityError availabilityError() const;
    QRadioTuner::Error error() const;
    QString errorString() const;

    /* Callbacks from XARadioSessionImplObserver begin */
    void CBBandChanged(QRadioTuner::Band band);
    void CBStateChanged(QRadioTuner::State state);
    void CBFrequencyChanged(TInt newFrequency);
    void CBStereoStatusChanged(bool isStereo);
    void CBSignalStrengthChanged(int signalStrength);
    void CBVolumeChanged(int volume);
    void CBMutedChanged(bool isMuted);
    void CBSearchingChanged(bool isSearching);
    void CBError(QRadioTuner::Error err);
    /* Callbacks from XARadioSessionImplObserver end */

signals:
    void stateChanged(QRadioTuner::State state);
    void bandChanged(QRadioTuner::Band band);
    void frequencyChanged(int frequency);
    void stereoStatusChanged(bool stereo);
    void searchingChanged(bool stereo);
    void signalStrengthChanged(int signalStrength);
    void volumeChanged(int volume);
    void mutedChanged(bool muted);
    void error(int err, QString str);

private:
    /* Own */
    QRadioTuner::Error iError;
    XARadioSessionImpl* m_impl;
};

#endif /*QXARADIOSESSION_H*/
