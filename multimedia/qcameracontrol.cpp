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

#include <qcameracontrol.h>
#include "qmediacontrol_p.h"

/*!
    \class QCameraControl
    \ingroup multimedia

    \preliminary
    \brief The QCameraControl class provides control of still or video cameras, this is supplied
    by a QMediaService object, and is used by QCamera.

    \sa QMediaService, QCamera
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
    \fn QCameraControl::start()
    Start the camera service.
*/

/*!
    \fn QCameraControl::stop()

    Stop the camera service.
*/

/*!
    \fn QCameraControl::state() const

    Returns the state of the camera service.

    \sa QCamera::state
*/

/*!
    \fn void QCameraControl::stateChanged(QCamera::State state)

    Signal emitted when \a state changes state.
*/

/*!
    \fn void QCameraControl::error(int error, const QString &errorString)

    Signal emitted when an error occurs with error code \a error and
    a description of the error \a errorString.
*/

