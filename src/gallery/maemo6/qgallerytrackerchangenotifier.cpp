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

#include "qgallerytrackerchangenotifier_p.h"

#include "qgallerytrackerschema_p.h"
#include <QtCore/qdebug.h>

QTM_BEGIN_NAMESPACE

QGalleryTrackerChangeNotifier::QGalleryTrackerChangeNotifier(
        const QString &service,
        const QGalleryDBusInterfacePointer &daemonInterface, QObject *parent)
    : QObject(parent)
    , m_daemonInterface(daemonInterface)
    , m_service(service)
{
    connect(
        m_daemonInterface.data(), SIGNAL(GraphUpdated(QString,QVector<QGalleryTrackerGraphUpdate>,QVector<QGalleryTrackerGraphUpdate>)),
        this, SLOT(graphUpdated(QString,QVector<QGalleryTrackerGraphUpdate>,QVector<QGalleryTrackerGraphUpdate>)));
}

void QGalleryTrackerChangeNotifier::itemsEdited(const QString &service)
{
    emit itemsChanged(QGalleryTrackerSchema::serviceUpdateId(service));
}

void QGalleryTrackerChangeNotifier::graphUpdated(
        const QString &className,
        const QVector<QGalleryTrackerGraphUpdate> &,
        const QVector<QGalleryTrackerGraphUpdate> &)
{
    /*
     * className ends with e.g. ...nfo#Audio and m_service contains "nfo:Audio".
     */
    QString identifier(m_service);
    identifier.replace(':','#');
    if (className.endsWith(identifier)
            || (m_service == QLatin1String("nmm:Artist") && className.endsWith("nfo#Audio"))) {
        emit itemsChanged(QGalleryTrackerSchema::serviceUpdateId(m_service));
    }
}

#include "moc_qgallerytrackerchangenotifier_p.cpp"

QTM_END_NAMESPACE
