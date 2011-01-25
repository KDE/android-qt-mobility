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

#include "qgeopositioninfosourcefactory.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QGeoPositionInfoSourceFactory

  \brief The QGeoPositionInfoSourceFactory class is a factory class used
  as the plugin interface for external providers of positioning data.

  \inmodule QtLocation

  Implementers must provide a unique combination of sourceName() and
  sourceVersion() per plugin.

  The other functions must be overridden by all plugins, other than
  sourcePriority() which defaults to returning 0. Higher values of
  priority will be preferred to lower ones.
  */

/*!
  \fn QGeoPositionInfoSource *QGeoPositionInfoSourceFactory::positionInfoSource(QObject *parent)

  Returns a new QGeoPositionInfoSource associated with this plugin. Can
  also return 0, in which case the factory with the next highest priority
  will be used instead.
  */

/*!
  \fn QGeoSatelliteInfoSource *QGeoPositionInfoSourceFactory::satelliteInfoSource(QObject *parent)

  Returns a new QGeoSatelliteInfoSource associated with this plugin. Can
  also return 0, in which case the factory with the next highest priority
  will be used instead.
  */

/*!
  \fn QString QGeoPositionInfoSourceFactory::sourceName() const

  Returns the string used to identify the position provider behind this
  implementation.

  The combination of sourceName() and sourceVersion() should be unique
  amongst the plugins.
  */

/*!
  \fn int QGeoPositionInfoSourceFactory::sourceVersion() const

  Returns the version of the plugin.

  The combination of sourceName() and sourceVersion() should be unique
  amongst the plugins.
  */

/*!
  Returns the priority of this factory in the list of available
  factories. Factories with higher values of priority will be preferred
  to those with lower values.
  */
int QGeoPositionInfoSourceFactory::sourcePriority() const
{
    return 0;
}

QGeoPositionInfoSourceFactory::~QGeoPositionInfoSourceFactory()
{}

QTM_END_NAMESPACE
