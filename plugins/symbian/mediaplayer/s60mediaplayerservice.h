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

#ifndef S60VIDEOPLAYERSERVICE_H
#define S60VIDEOPLAYERSERVICE_H

#include <QtCore/qobject.h>

#include <multimedia/qmediaservice.h>
#include <multimedia/qvideooutputcontrol.h>

#include "s60videooutputcontrol.h"

class QMediaMetaData;
class QMediaPlayerControl;
class QMediaPlaylist;

class S60VideoMetaData;
class S60VideoOutputControl;
class S60VideoPlayerSession;
class S60VideoPlayerControl;
class S60VideoMetaDataProvider;
class S60VideoWidgetControl;

#ifndef QT_NO_MULTIMEDIA
class S60VideoRenderer;
class S60VideoOverlay;
#endif

class QMediaPlaylistNavigator;

class S60MediaPlayerService : public QMediaService
{
    Q_OBJECT
public:
    S60MediaPlayerService(QObject *parent = 0);
    ~S60MediaPlayerService();

    QMediaControl *control(const char *name) const;
    
private slots:
    void videoOutputChanged(QVideoOutputControl::Output output);

private:
    S60VideoPlayerControl *m_control;
    S60VideoOutputControl *m_videoOutput;
    S60VideoPlayerSession *m_session;
    S60VideoMetaDataProvider *m_metaData;   
#ifndef QT_NO_MULTIMEDIA
    S60VideoRenderer *m_videoRenderer;
    S60VideoOverlay *m_videoWindow;
#endif
    S60VideoWidgetControl *m_videoWidget;
};

#endif
