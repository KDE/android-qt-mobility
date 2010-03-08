/*
* ==============================================================================
*  Name        : QMapTileCache.h
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#ifndef QMAPTILECACHE_H
#define QMAPTILECACHE_H

#include <QString>
#include <QDir>
#include <QDateTime>

#include "qmaptilereply.h"
#include "qmaptilerequest.h"

QTM_BEGIN_NAMESPACE

/*!
* This class handles the caching of downloaded map tiles.
* If no explicit base directory is provided, the cache will
* be constructed in Qt::QDir::home(). When the cache becomes full,
* a LRU strategy is applied to clean up.
*/
class QMapTileCache
{
public:
    /*!
    * Constructor.
    */
    QMapTileCache();
    /*!
    * Constructor.
    * @param directory The base directory of the cache.
    */
    QMapTileCache(QDir& directory);

    /*!
    * Sets the limits of the cache in bytes. When the
    * total size of all tiles in the cache reaches <i>high</i>,
    * tiles are removed from the cache - according to a LRU strategy -
    * until the cache size drops to <i>low</i>.
    * @param high The high watermark in bytes.
    * @param low The low watermark in bytes.
    */
    void setWatermark(quint64 high, quint64 low);
    /*!
    * @return The high watermark of the cache in bytes.
    */
    quint64 highWatermark() const {
        return highWm;
    }
    /*!
    * @return The low watermark of the cache in bytes.
    */
    quint64 lowWatermark() const {
        return lowWm;
    }

    /*!
    * Sets the base directory of the map tile cache.
    * @param directory The base directory.
    */
    void setDirectory(const QString& directory);
    /*!
    * Sets the base directory of the map tile cache.
    * @param directory The base directory.
    */
    void setDirectory(const QDir& directory);
    /*!
    * @return The base directory of the map tile cache.
    */
    QDir directory() const {
        return cacheDir;
    }

    /*!
    * Sets the maximum duration for which a tile will remain in the cache.
    * @param seconds The total number of seconds after which a cached tile will expire.
    */
    void setExpireAfter(const quint32 seconds);
    /*!
    * Sets the maximum duration for which a tile will remain in the cache.
    * @param days The total number of days after which a cached tile will expire.
    */
    void setExpireAfter(const quint16 days);
    /*!
    * @return The total number of seconds after which a cached tile will expire.
    */
    quint32 expireAfter() const {
        return expire;
    }

    /*!
    * Removes all cached map tiles.
    */
    void clear();
    /*!
    * Removes all expired map tiles from the cache.
    * If after that, the cache size still >= <i>highWm</i>,
    * tiles are removed from the cache - according to a LRU strategy -
    * until the cache size drops to <i>lowWm</i>.
    * @param force If true, all expired map tiles will be removed.
                   Otherwise, as soon as <i>lowWm</i> is reached, cleanup returns.
    */
    void cleanup(bool force = false);

    /*!
    * Checks whether the map tile being requested can be served
    * from the cache.
    * @param request The request specifying the requested map tile.
    * @return A QMapTileReply containing the request map tile if found,
              NULL otherwise.
    */
    QMapTileReply* get(const QMapTileRequest& request);
    /*!
    * Inserts the map tile contained in the <i>reply</i>
    * into the cache.
    * @param reply The reply containing the map tile.
    */
    void cache(QMapTileReply& reply);
    /*!
    * @return The current size of the cache.
    */
    quint64 size() const {
        return sz;
    }

private:
    /*!
    * @return Whether the given file timestamp is considered expired.
    */
    bool hasExpired(const QDateTime& timestamp);
    /*!
    * Initializes the cache.
    */
    void init();

    /*!
    * Constructs a hash key out of a file name.
    * 0 <= hash key <= 255.
    * This hash key is used to determine the subfolder for
    * the given file name.
    * @param fileName The file name.
    * @return The hash key.
    */
    static int constrHashKey(const QString& fileName);
    /*!
    * Constructs a file name under which the map tile contained in
    * the request will be stored.
    * @param request The request containing the map tile.
    * @return The file name.
    */
    static QString constrFileName(const QMapTileRequest& request);

private:
    QDir cacheDir; //!< The base directory of the cache.
    quint32 expire; //!< Total number of seconds after which a cached tile will expire.
    quint64 sz; //!< The current size of the cache.
    quint64 highWm; //!< High watermark in bytes.
    quint64 lowWm; //!< Low watermark in bytes.
};

QTM_END_NAMESPACE

#endif
