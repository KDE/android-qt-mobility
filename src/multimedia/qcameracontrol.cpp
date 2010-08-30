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

#include <qcameracontrol.h>
#include  "qmediacontrol_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QCameraControl

    \preliminary

    \brief The QCameraControl class is an abstract base class for
    classes that control still cameras or video cameras.

    \inmodule QtMultimediaKit
    \ingroup camera

    This service is provided by a QMediaService object via
    QMediaService::control().  It is used by QCamera.

    The interface name of QCameraControl is \c com.nokia.Qt.QCameraControl/1.0 as
    defined in QCameraControl_iid.


The Camera API of Qt Mobility is still in \bold Technology Preview. It has
not undergone the same level of review and testing as the rest of the APIs.

The API exposed by the classes in this component are not stable, and will
undergo modification or removal prior to the final release of Qt Mobility.

    \sa QMediaService::requestControl(), QCamera
*/

/*!
    \macro QCameraControl_iid

    \c com.nokia.Qt.QCameraControl/1.0

    Defines the interface name of the QCameraControl class.

    \relates QCameraControl
*/

/*!
    Constructs a camera control object with \a parent.
*/

QCameraControl::QCameraControl(QObject *parent):
    QMediaControl(*new QMediaControlPrivate, parent)
{
}

/*!
    Destruct the camera control object.
*/

QCameraControl::~QCameraControl()
{
}

/*!
    \fn QCameraControl::state() const

    Returns the state of the camera service.

    \sa QCamera::state
*/

/*!
    \fn QCameraControl::setState(QCamera::State state)

    Sets the camera \a state.

    States changes are synchronous and indicate more user
    intention than the internal backen status,
    which is synchronously reported with QCameraControl::statusChanged() signal.

    \sa QCamera::State
*/

/*!
    \fn void QCameraControl::stateChanged(QCamera::State state)

    Signal emitted when the camera \a state changes.

    In most cases the state chage is caused by QCameraControl::setState(),
    but if crytical error has occured the state changes to QCamera::UnloadedState.
*/

/*!
    \fn QCameraControl::status() const

    Returns the status of the camera service.

    \sa QCamera::state
*/

/*!
    \fn void QCameraControl::statusChanged(QCamera::Status status)

    Signal emitted when the camera \a status changes.

    QCameraControl::state changes are synchronous and reflect user input/intention
    while QCameraControl::status is asynchronous and used to notify application
    about actual state of backend.
*/


/*!
    \fn void QCameraControl::error(int error, const QString &errorString)

    Signal emitted when an error occurs with error code \a error and
    a description of the error \a errorString.
*/

/*!
    \fn Camera::CaptureMode QCameraControl::captureMode() const = 0

    Returns the current capture mode.
*/

/*!
    \fn void QCameraControl::setCaptureMode(QCamera::CaptureMode mode) = 0;

    Sets the current capture \a mode.
*/

/*!
    \fn bool QCameraControl::isCaptureModeSupported(QCamera::CaptureMode mode) const = 0;

    Returns true if the capture \a mode is suported.

    Backend should return supported modes even in Stopped state.
*/

/*!
    \fn QCameraControl::captureModeChanged(QCamera::CaptureMode mode)

    Signal emitted when the camera capture \a mode changes.
 */

#include "moc_qcameracontrol.cpp"
QT_END_NAMESPACE
