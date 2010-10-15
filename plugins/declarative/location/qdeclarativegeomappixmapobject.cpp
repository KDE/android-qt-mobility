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

#include "qdeclarativegeomappixmapobject_p.h"

#include <QBrush>
#include <QUrl>
#include <QFile>
#include <QIODevice>
#include <QImage>
#include <QImageReader>

#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecontext.h>

#include <QDebug>

QTM_BEGIN_NAMESPACE

QDeclarativeGeoMapPixmapObject::QDeclarativeGeoMapPixmapObject()
    : m_status(QDeclarativeGeoMapPixmapObject::Null),
    m_reply(0)
{
    m_coordinate = new QDeclarativeCoordinate(this);

    connect(m_coordinate,
            SIGNAL(latitudeChanged(double)),
            this,
            SLOT(coordinateLatitudeChanged(double)));

    connect(m_coordinate,
            SIGNAL(longitudeChanged(double)),
            this,
            SLOT(coordinateLongitudeChanged(double)));

    connect(m_coordinate,
            SIGNAL(altitudeChanged(double)),
            this,
            SLOT(coordinateAltitudeChanged(double)));
}

QDeclarativeGeoMapPixmapObject::~QDeclarativeGeoMapPixmapObject()
{
}

void QDeclarativeGeoMapPixmapObject::setDeclarativeCoordinate(const QDeclarativeCoordinate *coordinate)
{
    if (m_coordinate->coordinate() == coordinate->coordinate())
        return;

    m_coordinate->setCoordinate(coordinate->coordinate());
    setCoordinate(coordinate->coordinate());

    emit declarativeCoordinateChanged(m_coordinate);
}

QDeclarativeCoordinate* QDeclarativeGeoMapPixmapObject::declarativeCoordinate() 
{
    return m_coordinate;
}


void QDeclarativeGeoMapPixmapObject::coordinateLatitudeChanged(double /*latitude*/)
{
    setCoordinate(m_coordinate->coordinate());
}

void QDeclarativeGeoMapPixmapObject::coordinateLongitudeChanged(double /*longitude*/)
{
    setCoordinate(m_coordinate->coordinate());
}

void QDeclarativeGeoMapPixmapObject::coordinateAltitudeChanged(double /*altitude*/)
{
    setCoordinate(m_coordinate->coordinate());
}

void QDeclarativeGeoMapPixmapObject::setSource(const QUrl &source)
{
    if (m_source == source)
        return;

    m_source = source;

    load();

    emit sourceChanged(source);
}

QUrl QDeclarativeGeoMapPixmapObject::source() const
{
    return m_source;
}


QDeclarativeGeoMapPixmapObject::Status QDeclarativeGeoMapPixmapObject::status() const
{
    return m_status;
}


void QDeclarativeGeoMapPixmapObject::setStatus(const QDeclarativeGeoMapPixmapObject::Status status)
{
    if (m_status == status)
        return;

    m_status = status;

    emit statusChanged(m_status);
}

void QDeclarativeGeoMapPixmapObject::load()
{
    // need to deal with absolute / relative local / remote files

    QUrl url = QDeclarativeEngine::contextForObject(this)->resolvedUrl(m_source);

    QString path;

    if (url.scheme().compare(QLatin1String("qrc"), Qt::CaseInsensitive) == 0) {
        if (url.authority().isEmpty())
            path = QLatin1Char(':') + url.path();
    } else if (url.scheme().compare(QLatin1String("file"), Qt::CaseInsensitive) == 0) {
        path = url.toLocalFile();
    }

    if (!path.isEmpty()) {
        QFile f(path);
        if (f.open(QIODevice::ReadOnly)) {
            QImage image;
            QImageReader imageReader(&f);
            if (imageReader.read(&image)) {
                setPixmap(QPixmap::fromImage(image));
                setStatus(QDeclarativeGeoMapPixmapObject::Ready);
            } else {
                setPixmap(QPixmap());
                setStatus(QDeclarativeGeoMapPixmapObject::Error);
                //qWarning() << "image read fail";
            }
        } else {
            setPixmap(QPixmap());
            setStatus(QDeclarativeGeoMapPixmapObject::Error);
            //qWarning() << "file open fail";
        }
    } else {
        if (m_reply) {
            m_reply->abort();
            m_reply->deleteLater();
            m_reply = 0;
        }

        QDeclarativeEngine *engine = QDeclarativeEngine::contextForObject(this)->engine();
        if (engine) {
            QNetworkAccessManager *nam = engine->networkAccessManager();
            m_reply = nam->get(QNetworkRequest(url));

            if (m_reply->isFinished()) {
                if (m_reply->error() == QNetworkReply::NoError) {
                    finished();
                } else {
                    error(m_reply->error());
                }
                delete m_reply;
                m_reply = 0;
                return;
            }

            setStatus(QDeclarativeGeoMapPixmapObject::Loading);

            connect(m_reply,
                    SIGNAL(finished()),
                    this,
                    SLOT(finished()));
            connect(m_reply,
                    SIGNAL(error(QNetworkReply::NetworkError)),
                    this,
                    SLOT(error(QNetworkReply::NetworkError)));

        } else {
            setPixmap(QPixmap());
            setStatus(QDeclarativeGeoMapPixmapObject::Error);
            //qWarning() << "null engine fail";
        }
    }
}

void QDeclarativeGeoMapPixmapObject::finished()
{
    if (m_reply->error() != QNetworkReply::NoError) {
        m_reply->deleteLater();
        m_reply = 0;
        return;
    }

    QImage image;
    QImageReader imageReader(m_reply);
    if (imageReader.read(&image)) {
        setPixmap(QPixmap::fromImage(image));
        setStatus(QDeclarativeGeoMapPixmapObject::Ready);
    } else {
        setPixmap(QPixmap());
        setStatus(QDeclarativeGeoMapPixmapObject::Error);
        //qWarning() << "image read fail";
    }

    m_reply->deleteLater();
    m_reply = 0;
}

void QDeclarativeGeoMapPixmapObject::error(QNetworkReply::NetworkError error)
{
    m_reply->deleteLater();
    m_reply = 0;

    setPixmap(QPixmap());
    setStatus(QDeclarativeGeoMapPixmapObject::Error);
    //qWarning() << "network error fail";
}

#include "moc_qdeclarativegeomappixmapobject_p.cpp"

QTM_END_NAMESPACE

