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


#include <qmediaplaylistcontrol.h>
#include <qabstractmediacontrol_p.h>

/*!
    \class QMediaPlaylistControl
    \ingroup multimedia

    \preliminary
    \brief The abstract class for controling media playlist navigation,
    this is provided by a QAbstractMediaService object,
    and is used by QMediaPlayer for playback.

    \sa QAbstractMediaService, QMediaPlayer
*/

/*!
  Create a new playlist control object with the given \a parent.
*/
QMediaPlaylistControl::QMediaPlaylistControl(QObject *parent):
    QAbstractMediaControl(*new QAbstractMediaControlPrivate, parent)
{
}

/*!
  Destroys the playlist control.
*/
QMediaPlaylistControl::~QMediaPlaylistControl()
{
}


/*!
  \fn QMediaPlaylistControl::playlistProvider() const

  Returns the playlist used by this media player.
*/

/*!
  \fn QMediaPlaylistControl::setPlaylistProvider(QMediaPlaylistProvider *playlist)

  Set the playlist of this media player to \a playlist.

  In many cases it is possible just to use the playlist
  constructed by player, but sometimes replacing the whole
  playlist allows to avoid copyting of all the items bettween playlists.

  Returns true if player can use this passed playlist; otherwise returns false.

*/

/*!
  \fn QMediaPlaylistControl::currentPosition() const

  Returns position of the current media source in the playlist.
*/

/*!
  \fn QMediaPlaylistControl::setCurrentPosition(int position)

  Jump to the item at the given \a position.
*/

/*!
  \fn QMediaPlaylistControl::nextPosition(int step) const

  Returns the position of item, which were current after calling advance()
  \a step times.

  Returned value depends on the size of playlist, current position
  and playback mode.

  \sa QMediaPlaylist::playbackMode
*/

/*!
  \fn QMediaPlaylistControl::previousPosition(int step) const

  Returns the position of item, which were current after calling back()
  \a step times.

  \sa QMediaPlaylist::playbackMode
*/

/*!
  \fn QMediaPlaylistControl::advance()

  Moves to the next item in playlist.
*/

/*!
  \fn QMediaPlaylistControl::back()

  Returns to the previous item in playlist.
*/

/*!
  \fn QMediaPlaylistControl::playbackMode() const

  Returns the playlist navigation mode.

  \sa QMediaPlaylist::PlaybackMode
*/

/*!
  \fn QMediaPlaylistControl::setPlaybackMode(QMediaPlaylist::PlaybackMode mode)

  Sets the playback \a mode.

  \sa QMediaPlaylist::PlaybackMode
*/

/*!
  \fn QMediaPlaylistControl::playlistProviderChanged()

  Signal emited when the playlist provider is changed.
*/

/*!
  \fn QMediaPlaylistControl::playlistPositionChanged(int position)

  Signal emited when the playlist \a position is changed.
*/

/*!
  \fn QMediaPlaylistControl::playbackModeChanged(QMediaPlaylist::PlaybackMode mode)

  Signal emited when the playback \a mode is changed.
*/
