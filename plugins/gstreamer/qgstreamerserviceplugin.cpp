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

#include <QtCore/qstring.h>
#include <QtCore/qdebug.h>

#include "qgstreamerserviceplugin.h"

#ifdef QMEDIA_GSTREAMER_PLAYER
#include "qgstreamerplayerservice.h"
#endif
#ifdef QMEDIA_GSTREAMER_CAPTURE
#include "qgstreamercaptureservice.h"
#endif

#include <multimedia/qmediaserviceprovider.h>


QStringList QGstreamerServicePlugin::keys() const
{
    return QStringList()
#ifdef QMEDIA_GSTREAMER_PLAYER
            << QLatin1String("mediaplayer")
#endif
#ifdef QMEDIA_GSTREAMER_CAPTURE
            << QLatin1String("audiosource")
            << QLatin1String("camera")
#endif
            ;
}

QAbstractMediaService* QGstreamerServicePlugin::create(const QString &key)
{
#ifdef QMEDIA_GSTREAMER_PLAYER
    if (key == QLatin1String("mediaplayer"))
        return new QGstreamerPlayerService;
#endif
#ifdef QMEDIA_GSTREAMER_CAPTURE
    if (key == QLatin1String("audiosource"))
        return new QGstreamerCaptureService(key);

    if (key == QLatin1String("camera"))
        return new QGstreamerCaptureService(key);
#endif

    qDebug() << "unsupported key:" << key;
    return 0;
}

Q_EXPORT_PLUGIN2(gstengine, QGstreamerServicePlugin);

