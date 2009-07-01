/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QSATELLITEINFOSOURCE_H
#define QSATELLITEINFOSOURCE_H

#include "qlocationglobal.h"
#include "qsatelliteinfo.h"

#include <QObject>
#include <QList>

class QSatelliteInfoSourcePrivate;

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class Q_LOCATION_EXPORT QSatelliteInfoSource : public QObject
{
    Q_OBJECT
public:
    explicit QSatelliteInfoSource(QObject *parent = 0);

    static QSatelliteInfoSource *createSource(QObject *parent = 0);

public slots:
    virtual void startUpdates() = 0;
    virtual void stopUpdates() = 0;

    virtual void requestUpdate(int timeout = 5000) = 0;

signals:
    void satellitesInViewUpdated(const QList<QSatelliteInfo> &satellites);
    void satellitesInUseUpdated(const QList<QSatelliteInfo> &satellites);
    void requestTimeout();

private:
    Q_DISABLE_COPY(QSatelliteInfoSource)
    QSatelliteInfoSourcePrivate *d;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
