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
#ifndef QNMEAPOSITIONINFOSOURCE_P_H
#define QNMEAPOSITIONINFOSOURCE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qnmeapositioninfosource.h"
#include "qgeopositioninfo.h"

#include <QObject>
#include <QQueue>
#include <QPointer>

class QNmeaReader;
class QBasicTimer;
class QTimerEvent;
class QTimer;

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

struct QPendingGeoPositionInfo
{
    QGeoPositionInfo info;
    bool hasFix;
};


class QNmeaPositionInfoSourcePrivate : public QObject
{
    Q_OBJECT
public:
    explicit QNmeaPositionInfoSourcePrivate(QNmeaPositionInfoSource *parent);
    ~QNmeaPositionInfoSourcePrivate();

    void startUpdates();
    void stopUpdates();
    void requestUpdate(int msec);

    void notifyNewUpdate(QGeoPositionInfo *update, bool fixStatus);

    QNmeaPositionInfoSource::UpdateMode m_updateMode;
    QPointer<QIODevice> m_device;
    QGeoPositionInfo m_lastUpdate;
    bool m_invokedStart;

public slots:
    void readyRead();

protected:
    void timerEvent(QTimerEvent *event);

private slots:
    void emitPendingUpdate();
    void sourceDataClosed();
    void updateRequestTimeout();

private:
    bool openSourceDevice();
    bool initialize();
    void prepareSourceDevice();
    void emitUpdated(const QGeoPositionInfo &update);

    QNmeaPositionInfoSource *m_source;
    QNmeaReader *m_nmeaReader;
    QBasicTimer *m_updateTimer;
    QGeoPositionInfo m_pendingUpdate;
    QDate m_currentDate;
    QTimer *m_requestTimer;
    bool m_noUpdateLastInterval;
    bool m_connectedReadyRead;
};


class QNmeaReader
{
public:
    explicit QNmeaReader(QNmeaPositionInfoSourcePrivate *sourcePrivate)
        : m_proxy(sourcePrivate) {}
    virtual ~QNmeaReader() {}

    virtual void readAvailableData() = 0;

protected:
    QNmeaPositionInfoSourcePrivate *m_proxy;
};


class QNmeaRealTimeReader : public QNmeaReader
{
public:
    explicit QNmeaRealTimeReader(QNmeaPositionInfoSourcePrivate *sourcePrivate);
    virtual void readAvailableData();
};


class QNmeaSimulatedReader : public QObject, public QNmeaReader
{
    Q_OBJECT
public:
    explicit QNmeaSimulatedReader(QNmeaPositionInfoSourcePrivate *sourcePrivate);
    ~QNmeaSimulatedReader();
    virtual void readAvailableData();

protected:
    virtual void timerEvent(QTimerEvent *event);

private slots:
    void simulatePendingUpdate();

private:
    bool setFirstDateTime();
    void processNextSentence();

    QQueue<QPendingGeoPositionInfo> m_pendingUpdates;
    int m_currTimerId;
    bool m_hasValidDateTime;
};


QT_END_NAMESPACE

QT_END_HEADER

#endif
