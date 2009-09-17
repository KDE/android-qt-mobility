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

#include <multimedia/qmediaplaylistnavigator.h>
#include <multimedia/qmediaplaylistprovider.h>
#include <multimedia/qmediaplaylist.h>

#include <QtCore/qdebug.h>

class QMediaPlaylistNullProvider : public QMediaPlaylistProvider
{
public:
    QMediaPlaylistNullProvider() :QMediaPlaylistProvider() {}
    virtual ~QMediaPlaylistNullProvider() {}
    virtual int size() const {return 0;}
    virtual QMediaSource media(int) const { return QMediaSource(); }
};

Q_GLOBAL_STATIC(QMediaPlaylistNullProvider, _q_nullMediaPlaylist)

class QMediaPlaylistNavigatorPrivate
{
    Q_DECLARE_PUBLIC(QMediaPlaylistNavigator)
public:
    QMediaPlaylistNavigatorPrivate()
        :playlist(0),
        currentPos(-1),
        playbackMode(QMediaPlaylist::Linear),
        randomPositionsOffset(-1)
    {
    }

    QMediaPlaylistProvider *playlist;
    int currentPos;
    QMediaPlaylist::PlaybackMode playbackMode;
    QMediaSource currentItem;

    mutable QList<int> randomModePositions;
    mutable int randomPositionsOffset;

    int nextItemPos(int steps = 1) const;
    int previousItemPos(int steps = 1) const;

    void _q_itemsInserted(int start, int end);
    void _q_itemsRemoved(int start, int end);
    void _q_itemsChanged(int start, int end);

    QMediaPlaylistNavigator *q_ptr;
};


int QMediaPlaylistNavigatorPrivate::nextItemPos(int steps) const
{
    if (playlist->size() == 0)
        return -1;

    if (steps == 0)
        return currentPos;

    switch (playbackMode) {
        case QMediaPlaylist::CurrentItemOnce:
            return -1;
        case QMediaPlaylist::CurrentItemInLoop:
            return currentPos;
        case QMediaPlaylist::Linear:
            {
                int nextPos = currentPos+steps;
                return nextPos < playlist->size() ? nextPos : -1;
            }
        case QMediaPlaylist::Loop:
            return (currentPos+steps) % playlist->size();
        case QMediaPlaylist::Random:
            {
                //TODO: limit the history size

                if (randomPositionsOffset == -1) {
                    randomModePositions.clear();
                    randomModePositions.append(currentPos);
                    randomPositionsOffset = 0;
                }

                while (randomModePositions.size() < randomPositionsOffset+steps+1)
                    randomModePositions.append(-1);
                int res = randomModePositions[randomPositionsOffset+steps];
                if (res<0 || res >= playlist->size()) {
                    res = qrand() % playlist->size();
                    randomModePositions[randomPositionsOffset+steps] = res;
                }

                return res;
            }
    }

    return -1;
}

int QMediaPlaylistNavigatorPrivate::previousItemPos(int steps) const
{
    if (playlist->size() == 0)
        return -1;

    if (steps == 0)
        return currentPos;

    switch (playbackMode) {
        case QMediaPlaylist::CurrentItemOnce:
            return -1;
        case QMediaPlaylist::CurrentItemInLoop:
            return currentPos;
        case QMediaPlaylist::Linear:
            {
                int prevPos = currentPos - steps;
                return prevPos>=0 ? prevPos : -1;
            }
        case QMediaPlaylist::Loop:
            {
                int prevPos = currentPos - steps;
                while (prevPos<0)
                    prevPos += playlist->size();
                return prevPos;
            }
        case QMediaPlaylist::Random:
            {
                //TODO: limit the history size

                if (randomPositionsOffset == -1) {
                    randomModePositions.clear();
                    randomModePositions.append(currentPos);
                    randomPositionsOffset = 0;
                }

                while (randomPositionsOffset-steps < 0) {
                    randomModePositions.prepend(-1);
                    randomPositionsOffset++;
                }

                int res = randomModePositions[randomPositionsOffset-steps];
                if (res<0 || res >= playlist->size()) {
                    res = qrand() % playlist->size();
                    randomModePositions[randomPositionsOffset-steps] = res;
                }

                return res;
            }
    }

    return -1;
}



/*!
  Create a new \a playlist navigator object.
  */
QMediaPlaylistNavigator::QMediaPlaylistNavigator(QMediaPlaylistProvider *playlist, QObject *parent)
    : QObject(parent)
    , d_ptr(new QMediaPlaylistNavigatorPrivate)
{
    d_ptr->q_ptr = this;

    setPlaylist(playlist ? playlist : _q_nullMediaPlaylist());
}

/*!
  Destroys the playlist.
  */
QMediaPlaylistNavigator::~QMediaPlaylistNavigator()
{
    delete d_ptr;
}


/*!
  */
QMediaPlaylist::PlaybackMode QMediaPlaylistNavigator::playbackMode() const
{
    return d_func()->playbackMode;
}

/*!
  */
void QMediaPlaylistNavigator::setPlaybackMode(QMediaPlaylist::PlaybackMode mode)
{
    Q_D(QMediaPlaylistNavigator);
    if (d->playbackMode == mode)
        return;

    if (mode == QMediaPlaylist::Random) {
        d->randomPositionsOffset = 0;
        d->randomModePositions.append(d->currentPos);
    } else if (d->playbackMode == QMediaPlaylist::Random) {
        d->randomPositionsOffset = -1;
        d->randomModePositions.clear();
    }

    d->playbackMode = mode;

    emit playbackModeChanged(mode);
    emit surroundingItemsChanged();
}

QMediaPlaylistProvider *QMediaPlaylistNavigator::playlist() const
{
    return d_func()->playlist;
}

void QMediaPlaylistNavigator::setPlaylist(QMediaPlaylistProvider *playlist)
{
    Q_D(QMediaPlaylistNavigator);

    if (d->playlist == playlist)
        return;

    if (d->playlist) {
        d->playlist->disconnect(this);
    }

    if (playlist) {
        d->playlist = playlist;
    } else {
        //assign to shared readonly null playlist
        d->playlist = _q_nullMediaPlaylist();
    }

    connect(d->playlist, SIGNAL(itemsInserted(int,int)), SLOT(_q_itemsInserted(int,int)));
    connect(d->playlist, SIGNAL(itemsRemoved(int,int)), SLOT(_q_itemsRemoved(int,int)));
    connect(d->playlist, SIGNAL(itemsChanged(int,int)), SLOT(_q_itemsChanged(int,int)));

    d->randomPositionsOffset = -1;
    d->randomModePositions.clear();

    if (d->currentPos != -1) {
        d->currentPos = -1;
        emit currentPositionChanged(-1);
    }

    if (!d->currentItem.isNull()) {
        d->currentItem = QMediaSource();
        emit activated(d->currentItem); //stop playback
    }
}

/*!
  Returns the current playlist item.
  It is the same as itemAt(currentPosition());

  \sa currentPosition()
  */
QMediaSource QMediaPlaylistNavigator::currentItem() const
{
    return itemAt(d_func()->currentPos);
}

/*!
  */
QMediaSource QMediaPlaylistNavigator::nextItem(int steps) const
{
    return itemAt(nextPosition(steps));
}

/*!
  */

QMediaSource QMediaPlaylistNavigator::previousItem(int steps) const
{
    return itemAt(previousPosition(steps));
}

/*!
  Returns the media source at playlist position \a pos or
  invalid media source object if \a pos is out the playlist positions range.
  */
QMediaSource QMediaPlaylistNavigator::itemAt(int pos) const
{
    return d_func()->playlist->media(pos);
}

/*!
  Returns the current items position in the playlist
  or -1 if no items should be currently played.

  \sa nextPosition(int), previousPosition(int), seek(int)
  */
int QMediaPlaylistNavigator::currentPosition() const
{
    return d_func()->currentPos;
}

/*!
  Returns the next position in the playlist to be played or -1 if playback should be stopped.
  It's usually but not necessary currentPosition()+1 depending on playbackMode().

  For example, for CurrentItemInLoop playback mode it's always the same as currentPosition().

  If \a steps parameter is passed, the item position expected after \a step advance() actions returned.

  \sa currentPosition(), previousPosition(int), PlaybackMode
  */
int QMediaPlaylistNavigator::nextPosition(int steps) const
{
    return d_func()->nextItemPos(steps);
}

/*!
  Returns the previously position in playlist.
  It's usually but not necessary currentPosition()-1 depending on playbackMode().

  If \a steps parameter is passed, the item position expected after \a step back() actions returned.

  \sa nextPosition
  */
int QMediaPlaylistNavigator::previousPosition(int steps) const
{
    return d_func()->previousItemPos(steps);
}

/*!
  Advance to the next item in the playlist.

  \sa back(), seek(int), playbackMode()
  */
void QMediaPlaylistNavigator::advance()
{
    Q_D(QMediaPlaylistNavigator);

    int nextPos = d->nextItemPos();
    if (nextPos >= 0) {
        if ( playbackMode() == QMediaPlaylist::Random )
            d->randomPositionsOffset++;

        jump(nextPos);

        if (playbackMode() == QMediaPlaylist::CurrentItemInLoop ||
            (playbackMode() == QMediaPlaylist::Loop && d->playlist->size() == 1)) {
                emit activated(d->currentItem);
        }
    }
}

/*!
  Advance to the previously item in the playlist,
  depending on playback mode.

  \sa advance(), seek(int), playbackMode()
  */
void QMediaPlaylistNavigator::back()
{
    Q_D(QMediaPlaylistNavigator);

    int prevPos = d->previousItemPos();
    if (prevPos >= 0) {
        if ( playbackMode() == QMediaPlaylist::Random )
            d->randomPositionsOffset--;

        jump(prevPos);

        if (playbackMode() == QMediaPlaylist::CurrentItemInLoop ||
            (playbackMode() == QMediaPlaylist::Loop && d->playlist->size() == 1)) {
                emit activated(d->currentItem);
        }
    }
}

/*!
  Jump to the item at position \a pos.
  The item is also activated.
  */
void QMediaPlaylistNavigator::jump(int pos)
{
    Q_D(QMediaPlaylistNavigator);

    if (pos<0 || pos>=d->playlist->size()) {
        qWarning() << "Jump outside playlist range";
        return;
    }

    if (playbackMode() == QMediaPlaylist::Random) {
        if (d->randomModePositions[d->randomPositionsOffset] != pos) {
            d->randomModePositions.clear();
            d->randomModePositions.append(pos);
            d->randomPositionsOffset = 0;
        }
    }

    if (pos != d->currentPos) {
        d->currentPos = pos;
        emit currentPositionChanged(d->currentPos);
        emit surroundingItemsChanged();
    }

    QMediaSource src = d->playlist->media(pos);
    if (src != d->currentItem) {
        d->currentItem = src;
        emit activated(src);
    };
}

/*!
  \internal
  */
void QMediaPlaylistNavigatorPrivate::_q_itemsInserted(int start, int end)
{
    Q_Q(QMediaPlaylistNavigator);

    if (currentPos >= start) {
        currentPos = end-start+1;
        q->jump(currentPos);
    }

    //TODO: check if they really changed
    emit q->surroundingItemsChanged();
}

/*!
  \internal
  */
void QMediaPlaylistNavigatorPrivate::_q_itemsRemoved(int start, int end)
{
    Q_Q(QMediaPlaylistNavigator);

    if (currentPos > end) {
        currentPos = currentPos - end-start+1;
        q->jump(currentPos);
    } else if (currentPos >= start) {
        //current item was removed
        currentPos = qMin(start, playlist->size()-1);
        q->jump(currentPos);
    }

    //TODO: check if they really changed
    emit q->surroundingItemsChanged();
}

/*!
  \internal
  */
void QMediaPlaylistNavigatorPrivate::_q_itemsChanged(int start, int end)
{
    Q_Q(QMediaPlaylistNavigator);

    if (currentPos >= start && currentPos<=end) {
        QMediaSource src = playlist->media(currentPos);
        if (src != currentItem) {
            currentItem = src;
            emit q->activated(src);
        }
    }

    //TODO: check if they really changed
    emit q->surroundingItemsChanged();
}

/*!
    \fn void QMediaPlaylistNavigator::activated(const QMediaSource &&source)

    Signal the playback of \a source should be started.
    it's usually related to change of the current item
    in playlist.
*/

/*!
  \fn void QMediaPlaylistNavigator::currentPositionChanged(int)
  */
/*!
  \fn void QMediaPlaylistNavigator::playbackModeChanged(PlaybackMode mode)
  */


#include "moc_qmediaplaylistnavigator.cpp"
