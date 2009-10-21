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

#include <multimedia/qaudiodevicecontrol.h>

/*!
    \class QAudioDeviceControl
    \preliminary
    \brief The QAudioDeviceControl class provides an audio input/output device selector media control.

    The QAudioDeviceControl class provides descriptions of the audio devices available on a system
    and allows one to be selected as the input/outut of a media service.

    The interface name of QAudioDeviceControl is \c com.nokia.Qt.AudioDeviceControl as
    defined in QAudioDeviceControl_iid.

*/

/*!
    \macro QAudioDeviceControl_iid

    \c com.nokia.Qt.AudioDeviceControl

    Defines the interface name of QAudioDeviceControl.

    \relates QAudioDeviceControl
*/

/*!
    Constructs a new audio device control with the given \a parent.
*/
QAudioDeviceControl::QAudioDeviceControl(QObject *parent)
    :QMediaControl(parent)
{
}

/*!
    Destroys an audio device control.
*/
QAudioDeviceControl::~QAudioDeviceControl()
{
}

/*!
    \fn QAudioDeviceControl::deviceCount() const

    Returns the number of available devices;
*/

/*!
    \fn QAudioDeviceControl::name(int device) const

    Returns the name of a \a device.
*/

/*!
    \fn QAudioDeviceControl::description(int device) const

    Returns a description of a \a device.
*/

/*!
    \fn QAudioDeviceControl::icon(int device) const

    Returns an icon for a \a device.
*/

/*!
    \fn QAudioDeviceControl::defaultDevice() const

    Returns the index of the default input device.
*/

/*!
    \fn QAudioDeviceControl::selectedDevice() const

    Returns the index of the selected audio device.
*/

/*!
    \fn QAudioDeviceControl::selectedDeviceChanged(const QString &deviceName)

    Signals that the audio device has changed to \a deviceName.
*/

/*!
    \fn QAudioDeviceControl::setSelectedDevice(int index)

    Selects the audio device at \a index.
*/


/*!
    \fn QAudioDeviceControl::selectedDeviceChanged(int index)

    Signals that the \a index of the selected device has changed.
*/

/*!
    \fn QAudioDeviceControl::devicesChanged()

    Signals that list of available devices was changed.
*/
