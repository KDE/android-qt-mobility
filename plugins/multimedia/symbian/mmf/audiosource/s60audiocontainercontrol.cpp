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

#include "S60audiocontainercontrol.h"
#include "s60audiocapturesession.h"
#include <QtCore/qdebug.h>

S60AudioContainerControl::S60AudioContainerControl(QObject *parent)
    : QMediaContainerControl(parent)
{
}

S60AudioContainerControl::S60AudioContainerControl(QObject *session, QObject *parent)
   : QMediaContainerControl(parent)
{
    m_session = qobject_cast<S60AudioCaptureSession*>(session);   
}

QStringList S60AudioContainerControl::supportedContainers() const
{
    return m_session->supportedAudioContainers();
}

QString S60AudioContainerControl::containerMimeType() const
{    
    return m_session->audioContainer();    
}

void S60AudioContainerControl::setContainerMimeType(const QString &containerMimeType)
{
    m_session->setAudioContainer(containerMimeType);
}

QString S60AudioContainerControl::containerDescription(const QString &containerMimeType) const
{
    return m_session->audioContainerDescription(containerMimeType);
}

