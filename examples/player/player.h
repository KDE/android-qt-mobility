/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef PLAYER_H
#define PLAYER_H

#include <QtGui/QWidget>

#include <qmediaplayer.h>
#include <qmediaplaylist.h>
#include <qvideowidget.h>
#include <qaudioendpointselector.h>

#ifdef Q_OS_SYMBIAN
#include <QtGui/QDialog>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtNetwork/QHttp>
#include "mediakeysobserver.h"
#endif

QT_BEGIN_NAMESPACE
class QAbstractItemView;
class QLabel;
class QModelIndex;
class QSlider;

class QMediaPlayer;
class QVideoWidget;
QT_END_NAMESPACE

QT_USE_NAMESPACE

class PlaylistModel;

class Player : public QWidget
{
    Q_OBJECT
public:
    Player(QWidget *parent = 0);
    ~Player();

Q_SIGNALS:
    void fullScreenChanged(bool fullScreen);

private slots:
    void open();
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);
    void metaDataChanged();

    void previousClicked();

    void seek(int seconds);
    void jump(const QModelIndex &index);
    void playlistPositionChanged(int);

    void statusChanged(QMediaPlayer::MediaStatus status);
    void bufferingProgress(int progress);

    void displayErrorMessage();

#ifdef Q_OS_SYMBIAN
    void handleFullScreen(bool isFullscreen);
    void handleAspectRatio(bool aspectRatio);
    void handleStateChange(QMediaPlayer::State state);
    void handleMediaKeyEvent(MediaKeysObserver::MediaKeys key);
    void showPlayList();
    void hideOrShowCoverArt();
    void launchYoutubeDialog();
    void youtubeHttpRequestFinished(int requestId, bool error);
    void youtubeReadResponseHeader(const QHttpResponseHeader& responseHeader);
    void searchYoutubeVideo();
    void addYoutubeVideo();
    void handleAudioOutputDefault();
    void handleAudioOutputAll();
    void handleAudioOutputNone();
    void handleAudioOutputEarphone();
    void handleAudioOutputSpeaker();
    void handleAudioOutputChangedSignal(const QString&);
#else
    void showColorDialog();
#endif

private:
    void setTrackInfo(const QString &info);
    void setStatusInfo(const QString &info);
    void handleCursor(QMediaPlayer::MediaStatus status);

#ifdef Q_OS_SYMBIAN
    void createMenus();
#endif

    QMediaPlayer *player;
    QMediaPlaylist *playlist;
    QVideoWidget *videoWidget;
    QLabel *coverLabel;
    QSlider *slider;
    PlaylistModel *playlistModel;
    QAbstractItemView *playlistView;
    QString trackInfo;
    QString statusInfo;
    QAudioEndpointSelector *audioEndpointSelector;
#ifdef Q_OS_SYMBIAN
    MediaKeysObserver *mediaKeysObserver;
    QDialog *playlistDialog;
    QAction *toggleAspectRatio;
    QAction *showYoutubeDialog;
    QDialog *youtubeDialog;
    QHttp http;
    int httpGetId;
    QMenu *audioOutputMenu;
    QAction *setAudioOutputDefault;
    QAction *setAudioOutputAll;
    QAction *setAudioOutputNone;
    QAction *setAudioOutputEarphone;
    QAction *setAudioOutputSpeaker;
#else
    QDialog *colorDialog;
#endif
};

#endif
