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

#ifndef S60AUDIOPLAYERSESSION_H
#define S60AUDIOPLAYERSESSION_H

#include "s60mediaplayersession.h"

#ifdef S60_DRM_SUPPORTED
#include <drmaudiosampleplayer.h>
typedef CDrmPlayerUtility CAudioPlayer;
typedef MDrmAudioPlayerCallback MAudioPlayerObserver;
#else
#include <mdaaudiosampleplayer.h>  
typedef CMdaAudioPlayerUtility CAudioPlayer;
typedef MMdaAudioPlayerCallback MAudioPlayerObserver;
#endif

class S60AudioPlayerSession : public S60MediaPlayerSession, public CBase, public MAudioPlayerObserver
#ifdef S60_DRM_SUPPORTED
      , public MAudioLoadingObserver
#endif
{
    Q_OBJECT

public:
    S60AudioPlayerSession(QObject *parent);
    ~S60AudioPlayerSession();
    
    //From S60MediaPlayerSession
    bool isVideoAvailable() const;
    bool isAudioAvailable() const;
    
protected:
    //From S60MediaPlayerSession
    void doLoadL(const TDesC &path);
    void doLoadUrlL(const TDesC &path){Q_UNUSED(path)/*empty implementation*/}
    void doPlay();
    void doStop();
    void doPauseL();
    void doSetVolumeL(int volume);
    qint64 doGetPositionL() const;
    void doSetPositionL(qint64 microSeconds);
    void updateMetaDataEntriesL();
    int doGetMediaLoadingProgressL() const { /*empty implementation*/ return 0; }
    int doGetDurationL() const;
    
private:
#ifdef S60_DRM_SUPPORTED    
    // From MMdaAudioPlayerCallback
    void MdapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    void MdapcPlayComplete(TInt aError);
#else
    // From MDrmAudioPlayerCallback
    void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    void MapcPlayComplete(TInt aError);
#endif
    
#if defined(S60_DRM_SUPPORTED) &&  defined(__S60_50__) 
    // From MAudioLoadingObserver
    void MaloLoadingStarted() {};
    void MaloLoadingComplete() {};
#endif
    
private:
    CAudioPlayer *m_player;
};

#endif
