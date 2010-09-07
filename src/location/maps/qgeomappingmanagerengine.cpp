/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qgeomappingmanagerengine.h"
#include "qgeomappingmanagerengine_p.h"

#include <QNetworkProxy>

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoMappingManagerEngine
    \brief The QGeoMappingManagerEngine class provides an interface and convenience methods
    to implementors of QGeoServiceProvider plugins who want to provides support for displaying
    and interacting with maps.

    \inmodule QtLocation

    \ingroup maps-impl

    Subclasses of QGeoMappingManagerEngine need to provide an implementations
    of createMapData(). The QGeoMapData instances returned by createMapData()
    can be used to contain and manage information concerning what a particular
    QGraphicsGeoMap is viewing.

    The functions
    setSupportedMapTypes(const QList<QGraphicsGeoMap::MapType> &mapTypes),
    setMinimumZoomLevel(qreal minimumZoom),
    setMaximumZoomLevel(qreal maximumZoom),
    setMinimumImageSize(const QSize &minimumSize) and
    setMaximumImageSize(const QSize &maximumSize) should be used to
    configure the reported capabilities of the engine.

    It is important that this is done before createMapData() or any of the
    capability reporting functions are used to prevent incorrect or
    inconsistent behaviour.
*/

/*!
    Constructs a new engine with the specified \a parent, using \a parameters
    to pass any implementation specific data to the engine.
*/
QGeoMappingManagerEngine::QGeoMappingManagerEngine(const QMap<QString, QVariant> &parameters, QObject *parent)
        : QObject(parent),
        d_ptr(new QGeoMappingManagerEnginePrivate()) {}

/*!
  \internal
*/
QGeoMappingManagerEngine::QGeoMappingManagerEngine(QGeoMappingManagerEnginePrivate *dd, QObject *parent)
        : QObject(parent),
        d_ptr(dd) {}

/*!
    Destroys this engine.
*/
QGeoMappingManagerEngine::~QGeoMappingManagerEngine()
{
    Q_D(QGeoMappingManagerEngine);
    delete d;
}

/*!
    Sets the name which this engine implementation uses to distinguish itself
    from the implementations provided by other plugins to \a managerName.

    The combination of managerName() and managerVersion() should be unique
    amongst plugin implementations.
*/
void QGeoMappingManagerEngine::setManagerName(const QString &managerName)
{
    d_ptr->managerName = managerName;
}

/*!
    Returns the name which this engine implementation uses to distinguish
    itself from the implementations provided by other plugins.

    The combination of managerName() and managerVersion() should be unique
    amongst plugin implementations.
*/
QString QGeoMappingManagerEngine::managerName() const
{
    return d_ptr->managerName;
}

/*!
    Sets the version of this engine implementation to \a managerVersion.

    The combination of managerName() and managerVersion() should be unique
    amongst plugin implementations.
*/
void QGeoMappingManagerEngine::setManagerVersion(int managerVersion)
{
    d_ptr->managerVersion = managerVersion;
}

/*!
    Returns the version of this engine implementation.

    The combination of managerName() and managerVersion() should be unique
    amongst plugin implementations.
*/
int QGeoMappingManagerEngine::managerVersion() const
{
    return d_ptr->managerVersion;
}

/*!
\fn QGeoMapData* QGeoMappingManagerEngine::createMapData(QGraphicsGeoMap *geoMap)

    Returns a new QGeoMapData instance for \a geoMap, which will be managed by
    this manager.

    A QGeoMapData instance contains and manages the information about
    what a QGraphicsGeoMap is looking at.  A  single manager can be used by several
    QGraphicsGeoMap instances since each instance has an associated QGeoMapData instance.

    The QGeoMapData instance can be treated as a kind of session object, or
    as a model in a model-view-controller architecture, with QGraphicsGeoMap
    as the view and QGeoMappingManagerEngine as the controller.

    Subclasses of QGeoMappingManagerEngine are free to override this function
    to return subclasses of QGeoMapData in order to customize the
    map.
*/

/*!
    Returns a list of the map types supported by this engine.
*/
QList<QGraphicsGeoMap::MapType> QGeoMappingManagerEngine::supportedMapTypes() const
{
    Q_D(const QGeoMappingManagerEngine);
    return d->supportedMapTypes;
}

/*!
    Returns the minimum zoom level supported by this engine.

    Larger values of the zoom level correspond to more detailed views of the
    map.
*/
qreal QGeoMappingManagerEngine::minimumZoomLevel() const
{
    Q_D(const QGeoMappingManagerEngine);
    return d->minimumZoomLevel;
}

/*!
    Returns the maximum zoom level supported by this engine.

    Larger values of the zoom level correspond to more detailed views of the
    map.
*/
qreal QGeoMappingManagerEngine::maximumZoomLevel() const

{
    Q_D(const QGeoMappingManagerEngine);
    return d->maximumZoomLevel;
}

/*!
    Returns the size of the smallest map image which is supported by this engine.

    An invalid size indicates that this QGeoMappingManagerEngine instance places
    no restrictions on the minimum size of the map image.
*/
QSize QGeoMappingManagerEngine::minimumImageSize() const
{
    Q_D(const QGeoMappingManagerEngine);
    return d->minimumImageSize;
}

/*!
    Returns the size of the largest map image which is supported by this engine.

    An invalid size indicates that this QGeoMappingManagerEngine instance places
    no restrictions on the maximum size of the map image.
*/
QSize QGeoMappingManagerEngine::maximumImageSize() const
{
    Q_D(const QGeoMappingManagerEngine);
    return d->maximumImageSize;
}

/*!
    Sets the list of map types supported by this engine to \a mapTypes.

    Subclasses of QGeoMappingManagerEngine should use this function to ensure
    that supportedMapTypes() provides accurate information.
*/
void QGeoMappingManagerEngine::setSupportedMapTypes(const QList<QGraphicsGeoMap::MapType> &mapTypes)
{
    Q_D(QGeoMappingManagerEngine);
    d->supportedMapTypes = mapTypes;
}

/*!
    Sets the minimum zoom level supported by this engine.

    Larger values of the zoom level correspond to more detailed views of the
    map.

    Subclasses of QGeoMappingManagerEngine should use this function to ensure
    minimumZoomLevel() provides accurate information.
*/
void QGeoMappingManagerEngine::setMinimumZoomLevel(qreal minimumZoom)
{
    Q_D(QGeoMappingManagerEngine);
    d->minimumZoomLevel = minimumZoom;
}

/*!
    Sets the maximum zoom level supported by this engine.

    Larger values of the zoom level correspond to more detailed views of the
    map.

    Subclasses of QGeoMappingManagerEngine should use this function to ensure
    maximumZoomLevel() provides accurate information.
*/
void QGeoMappingManagerEngine::setMaximumZoomLevel(qreal maximumZoom)
{
    Q_D(QGeoMappingManagerEngine);
    d->maximumZoomLevel = maximumZoom;
}

/*!
    Sets the size of the smallest map image which is supported by this
    engine.

    An invalid size indicates that this engine places
    no restrictions on the minimum size of the map image.

    Subclasses of QGeoMappingManagerEngine should use this function to ensure
    minimumImageSize() provides accurate information.
*/
void QGeoMappingManagerEngine::setMinimumImageSize(const QSize &minimumImageSize)
{
    Q_D(QGeoMappingManagerEngine);
    d->minimumImageSize = minimumImageSize;
}

/*!
    Sets the size of the largest map image which is supported by this
    engine.

    An invalid size indicates that this engine places
    no restrictions on the maximum size of the map image.

    Subclasses of QGeoMappingManagerEngine should use this function to ensure
    maximumImageSize() provides accurate information.
*/
void QGeoMappingManagerEngine::setMaximumImageSize(const QSize &maximumImageSize)
{
    Q_D(QGeoMappingManagerEngine);
    d->maximumImageSize = maximumImageSize;
}

/*!
    Sets the locale to be used by the this manager to \a locale.

    If this mapping manager supports returning map labels
    in different languages, they will be returned in the language of \a locale.

    The locale used defaults to the system locale if this is not set.
*/
void QGeoMappingManagerEngine::setLocale(const QLocale &locale)
{
    d_ptr->locale = locale;
}

/*!
    Returns the locale used to hint to this mapping manager about what
    language to use for map labels.
*/
QLocale QGeoMappingManagerEngine::locale() const
{
    return d_ptr->locale;
}

/*******************************************************************************
*******************************************************************************/

QGeoMappingManagerEnginePrivate::QGeoMappingManagerEnginePrivate()
        : managerVersion(-1) {}

QGeoMappingManagerEnginePrivate::~QGeoMappingManagerEnginePrivate() {}

#include "moc_qgeomappingmanagerengine.cpp"

QTM_END_NAMESPACE
