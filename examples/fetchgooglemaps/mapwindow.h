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

#ifndef MAPWINDOW_H
#define MAPWINDOW_H

#include <qmobilityglobal.h>
#include <QMainWindow>

class QWebView;

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

QTM_BEGIN_NAMESPACE
class QGeoPositionInfo;
class QGeoPositionInfoSource;
class QNetworkSession;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class MapWindow : public QMainWindow
{
    Q_OBJECT
public:
    MapWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~MapWindow();
    void start();

private slots:
    void delayedInit();
    void waitForFix();
    void positionUpdated(const QGeoPositionInfo &info);
    void loadStarted();
    void loadFinished(bool ok);

private:
    QWebView *webView;
    QLabel *posLabel;
    QLabel *headingAndSpeedLabel;
    QLabel *dateTimeLabel;
    bool loading;
    QNetworkSession *session;
    bool usingLogFile;
    QGeoPositionInfoSource *location;
    bool waitingForFix;
};

#endif
