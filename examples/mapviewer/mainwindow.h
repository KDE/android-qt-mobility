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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qgeomappingmanager.h>
#include <qgeoroutingmanager.h>
#include <qgraphicsgeomap.h>
#include <qgeoserviceprovider.h>
#include <qgeomappixmapobject.h>
#include <qgeomapcircleobject.h>

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QPixmap>
#include <QList>
#include <QTime>
#include <qnetworksession.h>


class QResizeEvent;
class QShowEvent;

class QSlider;
class QRadioButton;
class QLineEdit;
class QPushButton;
class QToolButton;

QTM_USE_NAMESPACE

class MapWidget : public QGraphicsGeoMap
{
    Q_OBJECT
public:
    MapWidget(QGeoMappingManager *manager);
    ~MapWidget();

public slots:
    void setMouseClickCoordQuery(bool state);

signals:
    void coordQueryResult(const QGeoCoordinate &coord);

private slots:
    void kineticTimerEvent();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent* event);
    void wheelEvent(QGraphicsSceneWheelEvent* event);

private:
    bool coordQueryState;
    bool panActive;
    bool panDecellerate;

    // Fractional pan, used by panFloatWrapper
    QPointF remainingPan;

    // current kinetic panning speed, in pixel/msec
    QPointF kineticPanSpeed;
    QPoint panDir;
    QTimer *kineticTimer;
    QTime lastMoveTime;

    // An entry in the mouse history. first=speed, second=time
    typedef QPair<QPointF, QTime> MouseHistoryEntry;
    // A history of the last (currently 5) mouse move events is stored in order to smooth out movement detection for kinetic panning
    QList<MouseHistoryEntry> mouseHistory;

    void panFloatWrapper(const QPointF& delta);
    void applyPan(const Qt::KeyboardModifiers& modifiers);
public:
    QGeoMapCircleObject *lastCircle;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent* event);
    void showEvent(QShowEvent *);

private:
    void setupUi();
    void setProvider(QString providerId);
    void createMenus();
    void createPixmapIcon();

private slots:
    void demo1(bool checked);
    void demo2(bool checked);
    void demo3(bool checked);
    void drawRect(bool checked);
    void drawPixmap(bool checked);
    void drawPolyline(bool checked);
    void drawPolygon(bool checked);
    void drawCircle(bool checked);
    void drawText(bool checked);
    void calcRoute(bool checked);
    void customContextMenuRequest(const QPoint&);
    void routeFinished();
    void removePixmaps();
    void selectObjects();

    void sliderValueChanged(int zoomLevel);
    void mapZoomLevelChanged(qreal zoomLevel);
    void mapTypeToggled(bool checked);
    void mapTypeChanged(QGraphicsGeoMap::MapType type);
    void setCoordsClicked();
    void updateCoords(const QGeoCoordinate &coords);

    void networkSessionOpened();
    void error(QNetworkSession::SessionError error);

private:
    QGeoServiceProvider *m_serviceProvider;
    QGeoMappingManager *m_mapManager;
    QGeoRoutingManager *m_routingManager;

    MapWidget *m_mapWidget;
    QMenu* m_popupMenu;
    QPixmap m_markerIcon;
    QPoint m_lastClicked;
    QList<QGeoMapPixmapObject*> m_markerObjects;

    QGraphicsView* m_qgv;
    QSlider* m_slider;
    QList<QRadioButton*> m_mapControlButtons;
    QList<QGraphicsGeoMap::MapType> m_mapControlTypes;
    QLineEdit *m_latitudeEdit;
    QLineEdit *m_longitudeEdit;
    QToolButton *m_captureCoordsButton;
    QPushButton *m_setCoordsButton;

    QNetworkSession *m_session;
};

#endif // MAINWINDOW_H
