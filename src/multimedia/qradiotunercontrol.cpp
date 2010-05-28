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

#include <qmobilityglobal.h>
#include "qradiotunercontrol.h"
#include "qmediacontrol_p.h"

QT_BEGIN_NAMESPACE


/*!
    \class QRadioTunerControl
    \ingroup multimedia

    \preliminary
    \brief The QRadioTunerControl class provides access to the radio tuning
    functionality of a QMediaService.

    If a QMediaService can tune an analog radio device it will implement
    QRadioTunerControl.  This control provides a means to tune a radio device
    to a specific \l {setFrequency()}{frequency} as well as search \l
    {searchForward()}{forwards} and \l {searchBackward()}{backwards} for a
    signal.

    The functionality provided by this control is exposed to application code
    through the QRadioTuner class.

    The interface name of QRadioTunerControl is \c com.nokia.Qt.QRadioTunerControl/1.0 as
    defined in QRadioTunerControl_iid.

    \sa QMediaService::requestControl(), QRadioTuner
*/

/*!
    \macro QRadioTunerControl_iid

    \c com.nokia.Qt.QRadioTunerControl/1.0

    Defines the interface name of the QRadioTunerControl class.

    \relates QRadioTunerControl
*/

/*!
    Constructs a radio tuner control with the given \a parent.
*/

QRadioTunerControl::QRadioTunerControl(QObject *parent):
    QMediaControl(*new QMediaControlPrivate, parent)
{
}

/*!
    Destroys a radio tuner control.
*/

QRadioTunerControl::~QRadioTunerControl()
{
}

/*!
    \fn bool QRadioTunerControl::isAvailable() const

    Returns true if the radio service is ready to use.
*/

/*!
    \fn QtMultimedia::AvailabilityError QRadioTunerControl::availabilityError() const

    Returns the error state of the radio service.
*/

/*!
    \fn QRadioTuner::State QRadioTunerControl::state() const

    Returns the current radio tuner state.
*/

/*!
    \fn QRadioTuner::Band QRadioTunerControl::band() const

    Returns the frequency band a radio tuner is tuned to.
*/

/*!
    \fn void QRadioTunerControl::bandChanged(QRadioTuner::Band band)

    Signals that the frequency \a band a radio tuner is tuned to has changed.
*/

/*!
    \fn void QRadioTunerControl::setBand(QRadioTuner::Band band)

    Sets the frequecy \a band a radio tuner is tuned to.

    Changing the frequency band will reset the frequency to the minimum frequency of the new band.
*/

/*!
    \fn bool QRadioTunerControl::isBandSupported(QRadioTuner::Band band) const

    Identifies if a frequency \a band is supported.

    Returns true if the band is supported, and false if it is not.
*/

/*!
    \fn int QRadioTunerControl::frequency() const

    Returns the frequency a radio tuner is tuned to.
*/

/*!
    \fn int QRadioTunerControl::frequencyStep(QRadioTuner::Band band) const

    Returns the number of Hertz to increment the frequency by when stepping through frequencies
    within a given \a band.
*/

/*!
    \fn QPair<int,int> QRadioTunerControl::frequencyRange(QRadioTuner::Band band) const

    Returns a frequency \a band's minimum and maximum frequency.
*/

/*!
    \fn void QRadioTunerControl::setFrequency(int frequency)

    Sets the \a frequency a radio tuner is tuned to.
*/

/*!
    \fn bool QRadioTunerControl::isStereo() const

    Identifies if a radio tuner is receiving a stereo signal.

    Returns true if the tuner is receiving a stereo signal, and false if it is not.
*/

/*!
    \fn QRadioTuner::StereoMode QRadioTunerControl::stereoMode() const

    Returns a radio tuner's stereo mode.

    \sa QRadioTuner::StereoMode
*/

/*!
    \fn void QRadioTunerControl::setStereoMode(QRadioTuner::StereoMode mode)

    Sets a radio tuner's stereo \a mode.

    \sa QRadioTuner::StereoMode
*/

/*!
    \fn int QRadioTunerControl::signalStrength() const

    Return a radio tuner's current signal strength as a percentage.
*/

/*!
    \fn int QRadioTunerControl::volume() const

    Returns the volume of a radio tuner's audio output as a percentage.
*/

/*!
    \fn void QRadioTunerControl::setVolume(int volume)

    Sets the percentage \a volume of a radio tuner's audio output.
*/

/*!
    \fn bool QRadioTunerControl::isMuted() const

    Identifies if a radio tuner's audio output is muted.

    Returns true if the audio is muted, and false if it is not.
*/

/*!
    \fn void QRadioTunerControl::setMuted(bool muted)

    Sets the \a muted state of a radio tuner's audio output.
*/

/*!
    \fn bool QRadioTunerControl::isSearching() const

    Identifies if a radio tuner is currently scanning for signal.

    Returns true if the tuner is scanning, and false if it is not.
*/

/*!
    \fn  void QRadioTunerControl::searchForward()

    Starts a forward scan for a signal, starting from the current \l frequency().
*/

/*!
    \fn void QRadioTunerControl::searchBackward()

    Starts a backwards scan for a signal, starting from the current \l frequency().
*/

/*!
    \fn void QRadioTunerControl::cancelSearch()

    Stops scanning for a signal.
*/

/*!
    \fn void QRadioTunerControl::start()

    Activate the radio device.
*/

/*!
    \fn QRadioTunerControl::stop()

    Deactivate the radio device.
*/

/*!
    \fn QRadioTuner::Error QRadioTunerControl::error() const

    Returns the error state of a radio tuner.
*/

/*!
    \fn QString QRadioTunerControl::errorString() const

    Returns a string describing a radio tuner's error state.
*/

/*!
    \fn void QRadioTunerControl::stateChanged(QRadioTuner::State state)

    Signals that the \a state of a radio tuner has changed.
*/


/*!
    \fn void QRadioTunerControl::frequencyChanged(int frequency)

    Signals that the \a frequency a radio tuner is tuned to has changed.
*/

/*!
    \fn void QRadioTunerControl::stereoStatusChanged(bool stereo)

    Signals that the \a stereo state of a radio tuner has changed.
*/

/*!
    \fn void QRadioTunerControl::searchingChanged(bool searching)

    Signals that the \a searching state of a radio tuner has changed.
*/

/*!
    \fn void QRadioTunerControl::signalStrengthChanged(int strength)

    Signals that the percentage \a strength of the signal received by a radio tuner has changed.
*/

/*!
    \fn void QRadioTunerControl::volumeChanged(int volume)

    Signals that the percentage \a volume of radio tuner's audio output has changed.
*/

/*!
    \fn void QRadioTunerControl::mutedChanged(bool muted)

    Signals that the \a muted state of a radio tuner's audio output has changed.
*/

/*!
    \fn void QRadioTunerControl::error(QRadioTuner::Error error)

    Signals that an \a error has occured.
*/

#include "moc_qradiotunercontrol.cpp"
QT_END_NAMESPACE

