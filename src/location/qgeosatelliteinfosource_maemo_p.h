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

#ifndef QGEOSATELLITEINFOSOURCE_MAEMO_H
#define QGEOSATELLITEINFOSOURCE_MAEMO_H

#include "qgeosatelliteinfosource.h"
#include "qgeosatelliteinfo.h"
#include "dbuscomm_maemo_p.h"

QTM_BEGIN_NAMESPACE

class QGeoSatelliteInfoSourceMaemo : public QGeoSatelliteInfoSource
{
    Q_OBJECT
public:
    explicit QGeoSatelliteInfoSourceMaemo(QObject *parent = 0);
    int init();

private:
    DBusComm* dbusComm;
    bool registered;

private Q_SLOTS:
    void newSatellitesInView(const QList<QGeoSatelliteInfo> &update);
    void newSatellitesInUse(const QList<QGeoSatelliteInfo> &update);
    
public Q_SLOTS:
    void startUpdates();
    void stopUpdates();
    void requestUpdate(int timeout = 5000);

Q_SIGNALS:
    void satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &satellites);
    void satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &satellites);
    void requestTimeout();

private:
    Q_DISABLE_COPY(QGeoSatelliteInfoSourceMaemo)
};

QTM_END_NAMESPACE

#endif

