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

#ifndef FLICKRDEMO_H
#define FLICKRDEMO_H

#include <QDialog>
#include <QMainWindow>
#include <QNetworkReply>
#include <QSize>

QT_BEGIN_NAMESPACE
class QAction;
class QDialogButtonBox;
class QLabel;
class QNetworkAccessManager;
class QNetworkRequest;
class QProgressDialog;
class QPushButton;
class QWidget;
QT_END_NAMESPACE

//// QtMobility API headers
#include <qmobilityglobal.h>

#include "xqlistwidget.h"

QTM_BEGIN_NAMESPACE
class QNetworkSession;
class QGeoPositionInfoSource;
class QGeoPositionInfo;
class QGeoSatelliteInfoSource;
class QGeoSatelliteInfo;
QTM_END_NAMESPACE

#include "connectivityhelper.h"

// Use the QtMobility namespace
QTM_USE_NAMESPACE

class FlickrDemo: public QMainWindow
{
    Q_OBJECT

public:
    FlickrDemo(QWidget* parent = 0);
    ~FlickrDemo();

private:
    void createMenus();
    void viewSatelliteInfo();
    bool parsePictureList(QString xmlString);
    void downloadFlickerPictureList();
    void downloadPictureFromFlickr();
    void downloadNextThumbnail();

private Q_SLOTS:
    void delayedInit();
    void networkSessionOpened();

    void downloadButtonClicked();
    void cancelDownload();

    /*
        void httpRequestFinished(int requestId, bool error);
        void readResponseHeader(const QHttpResponseHeader& responseHeader);
        void updateDataReadProgress(int bytesRead, int totalBytes);
    */

    void pictureListDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void pictureListFinished();
    void pictureListError(QNetworkReply::NetworkError code);
    void clearPictureListRequest();

    void thumbnailFinished();
    void thumbnailError(QNetworkReply::NetworkError code);
    void clearThumbnailRequest();

    void pictureDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void pictureFinished();
    void pictureError(QNetworkReply::NetworkError code);
    void clearPictureRequest();

    // QGeoPositionInfoSource
    void positionUpdated(const QGeoPositionInfo &gpsPos);

    // QGeoSatelliteInfoSource
    void satellitesInViewUpdated(const QList<QGeoSatelliteInfo>&);
    void satellitesInUseUpdated(const QList<QGeoSatelliteInfo>&);

private:
    static const QSize gridSize;
    static const QSize thumbnailSize;
    static const QSize imageSize;
    static const QString apikey;
    static const QString savePath;

    void displayImage(const QPixmap &pixmap);

    QLabel *locationLabel;
    QLabel *satellitesLabel;
    XQListWidget *listWidget;
    QPushButton *downloadButton;

    QAction* m_downloadAct;

    bool m_logfileInUse;
    QGeoPositionInfoSource* m_location;
    QGeoSatelliteInfoSource* m_satellite;

    QNetworkSession* m_session;
    ConnectivityHelper *m_connectivityHelper;

    QProgressDialog *m_progressDialog;

    QNetworkAccessManager *m_nam;
    QNetworkReply *m_pictureListReply;
    QNetworkReply *m_thumbnailReply;
    QNetworkReply *m_pictureReply;

    int m_pages;
    int m_page;

    int m_satellitesInView;
    int m_satellitesUsed;
    double m_latitude;
    double m_longitude;

    bool m_downloadPictureList;
    int m_nameCounter;
    QStringList m_names;
    bool m_shuttingDown;
};

class PictureDialog: public QDialog
{
    Q_OBJECT

public:
    PictureDialog(const QPixmap& pixmapd, const QString& pictureName, QWidget* parent = 0);

private:
    QLabel *label;
    QLabel *imageLabel;
    QDialogButtonBox *buttonBox;
};

#endif // FLICKRDEMO_H

