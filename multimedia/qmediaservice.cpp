/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <multimedia/qmediaservice.h>
#include <multimedia/qmediaservice_p.h>

#include <multimedia/qaudiodevicecontrol.h>
#include <multimedia/qvideodevicecontrol.h>

#include <QtCore/qtimer.h>




/*!
    \class QMediaService
    \brief The QMediaService class provides a common base class for media service
    implementations.
    \preliminary
*/

/*!
    \enum QMediaService::MediaEndpoint

    Enumerates the possible end points a media service may have.

    \value AudioDevice An audio device for either input or output.
*/

/*!
    Construct a QMediaService with \a parent. This class is meant as a
    base class for Multimedia services so this constructor is protected.
*/

QMediaService::QMediaService(QObject *parent)
    : QObject(parent)
    , d_ptr(new QMediaServicePrivate)
{
    d_ptr->q_ptr = this;
}

/*!
    \internal
*/
QMediaService::QMediaService(QMediaServicePrivate &dd, QObject *parent)
    : QObject(parent)
    , d_ptr(&dd)
{
    d_ptr->q_ptr = this;
}

/*!
    Destroys a media service.
*/

QMediaService::~QMediaService()
{
    delete d_ptr;
}

/*!
    Return true if \a endpointType is available.
*/

bool QMediaService::isEndpointSupported(QMediaService::MediaEndpoint endpointType)
{
    Q_UNUSED(endpointType);

    return control(QAudioDeviceControl_iid) != 0;
}

/*!
    Returns the active endpoint for \a endpointType.
*/

QString QMediaService::activeEndpoint(QMediaService::MediaEndpoint endpointType)
{
    Q_UNUSED(endpointType);

    QAudioDeviceControl *audioDeviceControl =
                            qobject_cast<QAudioDeviceControl*>(control(QAudioDeviceControl_iid));

    if (audioDeviceControl == 0)
        return QString();

    return audioDeviceControl->name(audioDeviceControl->selectedDevice());
}

/*!
    Returns true if set of the active endpoint for \a endpointType to \a endpoint succeeds.
*/

bool QMediaService::setActiveEndpoint(QMediaService::MediaEndpoint endpointType, const QString& endpoint)
{
    Q_UNUSED(endpointType);

    QAudioDeviceControl *audioDeviceControl =
                            qobject_cast<QAudioDeviceControl*>(control(QAudioDeviceControl_iid));

    if (audioDeviceControl == 0)
        return false;

    for (int i = audioDeviceControl->deviceCount() - 1; i >= 0; ++i) {
        if (endpoint == audioDeviceControl->name(i)) {
            audioDeviceControl->setSelectedDevice(i);
            return true;
        }
    }

    return false;
}

/*!
    Returns the description of the \a endpoint for the \a endpointType.
*/

QString QMediaService::endpointDescription(QMediaService::MediaEndpoint endpointType, const QString& endpoint)
{
    Q_UNUSED(endpointType);

    QAudioDeviceControl *audioDeviceControl =
                            qobject_cast<QAudioDeviceControl*>(control(QAudioDeviceControl_iid));

    if (audioDeviceControl == 0)
        return QString();

    for (int i = audioDeviceControl->deviceCount() - 1; i >= 0; ++i) {
        if (endpoint == audioDeviceControl->name(i))
            return audioDeviceControl->description(i);
    }

    return QString();
}

/*!
    Returns a list of endpoints available for the \a endpointType.
*/

QList<QString> QMediaService::supportedEndpoints(QMediaService::MediaEndpoint endpointType) const
{
    Q_UNUSED(endpointType);

    QAudioDeviceControl *audioDeviceControl =
                            qobject_cast<QAudioDeviceControl*>(control(QAudioDeviceControl_iid));

    QList<QString> endpoints;

    if (audioDeviceControl != 0) {
        for (int i = audioDeviceControl->deviceCount(); i >= 0; ++i)
            endpoints << audioDeviceControl->name(i);
    }

    return endpoints;
}

/*!
    \fn QMediaService::control(const char *interface) const

    Returns a pointer to the media control implementing \a interface.

    If the service does not implement the control a null pointer is returned instead.
*/

/*!
    \fn QMediaService::control() const

    Returns a pointer to the media control of type T implemented by a media service.

    If the service does not implment the control a null pointer is returned instead.
*/


/*!
    \fn void QMediaService::supportedEndpointsChanged()

    This signal is emitted when there is a change in the availability of devices.
*/

/*!
    \fn void QMediaService::activeEndpointChanged(QMediaService::MediaEndpoint endpointType, const QString &endpoint)

    This signal emitted when the active endpoint of type \a endpointType has been changed to \a endpoint.
*/

