/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeosearchreply_nokia.h"
#include "qgeocodexmlparser.h"

QGeoSearchReplyNokia::QGeoSearchReplyNokia(QNetworkReply *reply, QObject *parent)
        : QGeoSearchReply(parent),
        m_reply(reply)
{
    connect(m_reply,
            SIGNAL(finished()),
            this,
            SLOT(networkFinished()));

    connect(m_reply,
            SIGNAL(error(QNetworkReply::NetworkError)),
            this,
            SLOT(networkError(QNetworkReply::NetworkError)));
}

QGeoSearchReplyNokia::~QGeoSearchReplyNokia()
{
    //TODO: possible mem leak -> m_reply->deleteLater() ?
}

void QGeoSearchReplyNokia::abort()
{
    m_reply->abort();
    m_reply->deleteLater();
}

void QGeoSearchReplyNokia::networkFinished()
{
    if (m_reply->error() != QNetworkReply::NoError) {
        setError(QGeoSearchReply::CommunicationError, m_reply->errorString());
        m_reply->deleteLater();
        return;
    }

    QGeoCodeXmlParser parser;
    if (parser.parse(m_reply)) {
        // TODO trim results based on bounds
        setPlaces(parser.results());
        setFinished(true);
    } else {
        setError(QGeoSearchReply::ParseError,parser.errorString());
    }

    m_reply->deleteLater();
}

void QGeoSearchReplyNokia::networkError(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error)
    setError(QGeoSearchReply::CommunicationError, m_reply->errorString());
    m_reply->deleteLater();
}
