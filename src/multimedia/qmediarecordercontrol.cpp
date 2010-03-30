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

#include "qmediarecordercontrol.h"

QTM_BEGIN_NAMESPACE


/*!
    \class QMediaRecorderControl
    \ingroup multimedia-serv

    \preliminary
    \brief The QMediaRecorderControl class provides access to the recording
    functionality of a QMediaService.

    If a QMediaService can record media it will implement QMediaRecorderControl.
    This control provides a means to set the \l {outputLocation()}{output location},
    and \l {record()}{start}, \l {pause()}{pause} and \l {stop()}{stop}
    recording.  It also provides feedback on the \l {duration()}{duration}
    of the recording.

    The functionality provided by this control is exposed to application
    code through the QMediaRecorder class.

    The interface name of QMediaRecorderControl is \c com.nokia.Qt.QMediaRecorderControl/1.0 as
    defined in QMediaRecorderControl_iid.

    \sa QMediaService::control(), QMediaRecorder

*/

/*!
    \macro QMediaRecorderControl_iid

    \c com.nokia.Qt.QMediaRecorderControl/1.0

    Defines the interface name of the QMediaRecorderControl class.

    \relates QMediaRecorderControl
*/

/*!
    Constructs a media recorder control with the given \a parent.
*/

QMediaRecorderControl::QMediaRecorderControl(QObject* parent)
    : QMediaControl(parent)
{
}

/*!
    Destroys a media recorder control.
*/

QMediaRecorderControl::~QMediaRecorderControl()
{
}

/*!
    \fn QUrl QMediaRecorderControl::outputLocation() const

    Returns the current output location being used.
*/

/*!
    \fn bool QMediaRecorderControl::setOutputLocation(const QUrl &location)

    Sets the output \a location and returns if this operation is successful.
*/

/*!
    \fn int QMediaRecorderControl::state() const

    Return the current recording state.
*/

/*!
    \fn qint64 QMediaRecorderControl::duration() const

    Return the current duration in milliseconds.
*/

/*!
    \fn void QMediaRecorderControl::record()

    Start recording.
*/

/*!
    \fn void QMediaRecorderControl::pause()

    Pause recording.
*/

/*!
    \fn void QMediaRecorderControl::stop()

    Stop recording.
*/

/*!
    \fn void QMediaRecorderControl::applySettings()

    Commits the encoder settings and performs pre-initialization to reduce delays when recording
    is started.
*/


/*!
    \fn void QMediaRecorderControl::stateChanged(QMediaRecorder::State state)

    Signals that the \a state of a media recorder has changed.
*/

/*!
    \fn void QMediaRecorderControl::durationChanged(qint64 duration)

    Signals that the \a duration of the recorded media has changed.

    This only emitted when there is a discontinuous change in the duration such as being reset to 0.
*/

/*!
    \fn void QMediaRecorderControl::error(int error, const QString &errorString)

    Signals that an \a error has occurred.  The \a errorString describes the error.
*/

#include "moc_qmediarecordercontrol.cpp"
QTM_END_NAMESPACE

