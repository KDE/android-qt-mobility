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

#include "qgeotiledmapreply.h"
#include "qgeotiledmapreply_p.h"

#include "qgeotiledmaprequest.h"

#include <qdebug.h>

QTM_BEGIN_NAMESPACE
/*!
    \class QGeoTiledMapReply
    \brief The QGeoTiledMapReply class represents the response from a geocoding service.

    This class represents the response from a geocoding service.
    It also takes responsibility for any errors that happen while the request is
    in submitted to and processed by the service.
*/

/*!
    \enum QGeoTiledMapReply::ErrorCode

    \value NoError
        No error has occurred.
    \value NetworkError
        A networking error occurred.
    \value NoContentError
        The reply contained no content.
    \value UnknownError
        An error occurred which does not fit into any of the other categories.
*/

/*!
    Constructs a QGeoTiledMapReply with parent \a parent.
*/
QGeoTiledMapReply::QGeoTiledMapReply(const QGeoTiledMapRequest &request, QObject *parent)
        : QObject(parent),
        d_ptr(new QGeoTiledMapReplyPrivate(request))
{
}

QGeoTiledMapReply::QGeoTiledMapReply(Error error, const QString &errorString, QObject *parent)
        : QObject(parent),
        d_ptr(new QGeoTiledMapReplyPrivate(error, errorString)) {}

/*!
    Destructor.
*/
QGeoTiledMapReply::~QGeoTiledMapReply()
{
    delete d_ptr;
}

void QGeoTiledMapReply::setFinished(bool finished)
{
    d_ptr->isFinished = finished;
    if (d_ptr->isFinished)
        emit this->finished();
}

bool QGeoTiledMapReply::isFinished() const
{
    return d_ptr->isFinished;
}

void QGeoTiledMapReply::setError(QGeoTiledMapReply::Error error, const QString &errorString)
{
    d_ptr->error = error;
    d_ptr->errorString = errorString;
    emit this->error(error, errorString);
    setFinished(true);
}

QGeoTiledMapReply::Error QGeoTiledMapReply::error() const
{
    return d_ptr->error;
}

QString QGeoTiledMapReply::errorString() const
{
    return d_ptr->errorString;
}

bool QGeoTiledMapReply::isCached() const
{
    return d_ptr->isCached;
}

void QGeoTiledMapReply::setCached(bool cached)
{
    d_ptr->isCached = cached;
}

QGeoTiledMapRequest QGeoTiledMapReply::request() const
{
    return d_ptr->request;
}

QByteArray QGeoTiledMapReply::mapImageData() const
{
    return d_ptr->mapImageData;
}

void QGeoTiledMapReply::setMapImageData(const QByteArray &data)
{
    d_ptr->mapImageData = data;
}

QString QGeoTiledMapReply::mapImageFormat() const
{
    return d_ptr->mapImageFormat;
}

void QGeoTiledMapReply::setMapImageFormat(const QString &format)
{
    d_ptr->mapImageFormat = format;
}

void QGeoTiledMapReply::abort() {}

/*!
    \fn void QGeoTiledMapReply::finished()

    Indicates that the reply has been received and processed without error, and is ready to be used.
*/
/*!
    \fn void QGeoTiledMapReply::error(QGeoTiledMapReply::Error error, const QString &errorString)

    Indicates that an error occurred during the receiving or processing of the reply.
*/

/*******************************************************************************
*******************************************************************************/

QGeoTiledMapReplyPrivate::QGeoTiledMapReplyPrivate(const QGeoTiledMapRequest &request)
        : error(QGeoTiledMapReply::NoError),
        errorString(""),
        isFinished(false),
        isCached(false),
        request(request) {}

QGeoTiledMapReplyPrivate::QGeoTiledMapReplyPrivate(QGeoTiledMapReply::Error error, const QString &errorString)
        : error(error),
        errorString(errorString),
        isFinished(true),
        isCached(false) {}

QGeoTiledMapReplyPrivate::~QGeoTiledMapReplyPrivate() {}

#include "moc_qgeotiledmapreply.cpp"

QTM_END_NAMESPACE
