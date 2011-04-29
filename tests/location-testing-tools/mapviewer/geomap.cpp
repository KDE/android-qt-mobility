/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "geomap.h"

#include <qgeomapcircleobject.h>
#include <qgeomappingmanager.h>
#include <qgeocoordinate.h>

#include <QTimer>
#include <QMenu>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPinchGesture>
#include <QGestureEvent>

#include <cmath>

QTM_USE_NAMESPACE

// TODO: Some of these could be exposed in a GUI and should probably be put elsewhere in that case (and made non-const)
#if defined(Q_OS_SYMBIAN) || defined(Q_OS_WINCE_WM) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
// Devices
static const bool enableKineticPanning = true;
static const qreal kineticPanningHalflife = 200.0; // time until kinetic panning speed slows down to 50%, in msec
static const qreal panSpeedNormal = 0.3; // keyboard panning speed without modifiers, in pixels/msec
static const qreal panSpeedFast = 1.0; // keyboard panning speed with shift, in pixels/msec
static const qreal kineticPanSpeedThreshold = 0.02; // minimum panning speed, in pixels/msec
static const int kineticPanningResolution = 75; // temporal resolution. Smaller values take more CPU but improve visual quality
static const int holdTimeThreshold = 200; // maximum time between last mouse move and mouse release for kinetic panning to kick in
#else
// Desktop
static const bool enableKineticPanning = true;
static const qreal kineticPanningHalflife = 300.0; // time until kinetic panning speed slows down to 50%, in msec
static const qreal panSpeedNormal = 0.3; // keyboard panning speed without modifiers, in pixels/msec
static const qreal panSpeedFast = 1.0; // keyboard panning speed with shift, in pixels/msec
static const qreal kineticPanSpeedThreshold = 0.005; // minimum panning speed, in pixels/msec
static const int kineticPanningResolution = 30; // temporal resolution. Smaller values take more CPU but improve visual quality
static const int holdTimeThreshold = 100; // maximum time between last mouse move and mouse release for kinetic panning to kick in
#endif

GeoMap::GeoMap(QGeoMappingManager * manager) :
        QGraphicsGeoMap(manager),
        panActive(false),
        kineticTimer(new QTimer),
        m_lastHoveredMapObject(0),
        lastCircle(0)
{
    for (int i = 0; i < 5; ++i) mouseHistory.append(MouseHistoryEntry());

    connect(kineticTimer, SIGNAL(timeout()), this, SLOT(kineticTimerEvent()));
    kineticTimer->setInterval(kineticPanningResolution);

    setAcceptHoverEvents(true);
    grabGesture(Qt::PinchGesture);
}

GeoMap::~GeoMap() {}

void GeoMap::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    setFocus();
    if (event->button() == Qt::LeftButton) {
        if (event->modifiers() & Qt::ControlModifier) {
        } else {
            panActive = true;

            // When pressing, stop the timer and stop all current kinetic panning
            kineticTimer->stop();
            kineticPanSpeed = QPointF();

            lastMoveTime = QTime::currentTime();
        }
    }

    event->accept();
}

void GeoMap::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    if (event->button() == Qt::LeftButton) {
        if (panActive) {
            panActive = false;

            if (!enableKineticPanning || lastMoveTime.msecsTo(QTime::currentTime()) > holdTimeThreshold) {
                return;
            }

            kineticPanSpeed = QPointF();
            int entries_considered = 0;

            QTime currentTime = QTime::currentTime();
            foreach(MouseHistoryEntry entry, mouseHistory) {
                // first=speed, second=time
                int deltaTime = entry.second.msecsTo(currentTime);
                if (deltaTime < holdTimeThreshold) {
                    kineticPanSpeed += entry.first;
                    entries_considered++;
                }
            }
            if (entries_considered > 0) kineticPanSpeed /= entries_considered;
            lastMoveTime = currentTime;

            // When releasing the mouse button/finger while moving, start the kinetic panning timer
            kineticTimer->start();
            panDecellerate = true;
        }
    }

    event->accept();
}

void GeoMap::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
    QGeoMapObject * objectAtCursor = 0;
    if (event) {
        QList<QGeoMapObject *> objectsAtCursor = mapObjectsAtScreenPosition(event->scenePos());
        if (!objectsAtCursor.isEmpty())
            objectAtCursor = objectsAtCursor.last();
    }

    if (m_lastHoveredMapObject == objectAtCursor)
        return;

    // unhover the old object if applicable
    if (m_lastHoveredMapObject) {
        disconnect(m_lastHoveredMapObject, SIGNAL(destroyed()), this, SLOT(hoveredMapObjectDestroyed()));

        m_lastHoveredMapObject->setProperty("pen", m_lastHoveredMapObjectPen);
    }

    m_lastHoveredMapObject = objectAtCursor;

    // hover the new object if applicable
    if (m_lastHoveredMapObject) {
        m_lastHoveredMapObjectPen = m_lastHoveredMapObject->property("pen").value<QPen>();
        QPen hoverPen = m_lastHoveredMapObjectPen;
        hoverPen.setColor(Qt::green);
        m_lastHoveredMapObject->setProperty("pen", hoverPen);

        connect(m_lastHoveredMapObject, SIGNAL(destroyed()), this, SLOT(hoveredMapObjectDestroyed()));
    }
}

void GeoMap::hoveredMapObjectDestroyed()
{
    m_lastHoveredMapObject = 0;
}
void GeoMap::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    hoverMoveEvent(0);
}


void GeoMap::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        if (lastCircle)
            lastCircle->setCenter(this->screenPositionToCoordinate(event->pos()));
    } else if (panActive) {
        // Calculate time delta
        QTime currentTime = QTime::currentTime();
        int deltaTime = lastMoveTime.msecsTo(currentTime);
        lastMoveTime = currentTime;

        // Calculate position delta
        QPointF delta = event->lastPos() - event->pos();

        // Calculate and set speed
        if (deltaTime > 0) {
            kineticPanSpeed = delta / deltaTime;

            mouseHistory.push_back(MouseHistoryEntry(kineticPanSpeed, currentTime));
            mouseHistory.pop_front();
        }

        // Pan map
        panFloatWrapper(delta);
    }

    event->accept();
}

void GeoMap::kineticTimerEvent()
{
    QTime currentTime = QTime::currentTime();
    int deltaTime = lastMoveTime.msecsTo(currentTime);
    lastMoveTime = currentTime;

    if (panDecellerate)
        kineticPanSpeed *= pow(qreal(0.5), qreal(deltaTime / kineticPanningHalflife));

    QPointF scaledSpeed = kineticPanSpeed * deltaTime;

    if (kineticPanSpeed.manhattanLength() < kineticPanSpeedThreshold) {
        // Kinetic panning is almost halted -> stop it.
        kineticTimer->stop();
        return;
    }
    panFloatWrapper(scaledSpeed);
}

// Wraps the pan(int, int) method to achieve floating point accuracy, which is needed to scroll smoothly.
void GeoMap::panFloatWrapper(const QPointF & delta)
{
    // Add to previously stored panning distance
    remainingPan += delta;

    // Convert to integers
    QPoint move = remainingPan.toPoint();

    // Commit mouse movement
    pan(move.x(), move.y());

    // Store committed mouse movement
    remainingPan -= move;

}

void GeoMap::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{
    setFocus();

    pan(event->lastPos().x() -  size().width() / 2.0, event->lastPos().y() - size().height() / 2.0);
    if (zoomLevel() < maximumZoomLevel())
        setZoomLevel(zoomLevel() + 1);

    event->accept();
}

void GeoMap::keyPressEvent(QKeyEvent * event)
{
    switch (event->key()) {
        case Qt::Key_Minus:
#ifdef Q_OS_SYMBIAN
        case Qt::Key_VolumeDown:
#endif
            if (zoomLevel() > minimumZoomLevel()) {
                setZoomLevel(zoomLevel() - 1);
            }
            break;

        case Qt::Key_Plus:
#ifdef Q_OS_SYMBIAN
        case Qt::Key_VolumeUp:
#endif
            if (zoomLevel() < maximumZoomLevel()) {
                setZoomLevel(zoomLevel() + 1);
            }
            break;

        case Qt::Key_T:
            if (mapType() == QGraphicsGeoMap::StreetMap)
                setMapType(QGraphicsGeoMap::SatelliteMapDay);
            else if (mapType() == QGraphicsGeoMap::SatelliteMapDay)
                setMapType(QGraphicsGeoMap::StreetMap);
            break;

        case Qt::Key_Shift:
            // If there's no current movement, we don't need to handle shift.
            if (panDir.manhattanLength() == 0) break;
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Up:
        case Qt::Key_Down:
            if (!event->isAutoRepeat()) {
                switch (event->key()) {
                    case Qt::Key_Left:
                        panDir.setX(-1);
                        break;

                    case Qt::Key_Right:
                        panDir.setX(1);
                        break;

                    case Qt::Key_Up:
                        panDir.setY(-1);
                        break;

                    case Qt::Key_Down:
                        panDir.setY(1);
                        break;
                }

                lastMoveTime = QTime::currentTime();
                kineticTimer->start();
                panDecellerate = false;

                applyPan(event->modifiers());
            }
            break;
    }

    event->accept();
}

void GeoMap::keyReleaseEvent(QKeyEvent * event)
{
    event->accept();

    // Qt seems to have auto-repeated release events too...
    if (event->isAutoRepeat()) return;

    switch (event->key()) {
        case Qt::Key_Left:
        case Qt::Key_Right:
            panDir.setX(0);
            break;

        case Qt::Key_Up:
        case Qt::Key_Down:
            panDir.setY(0);
            break;

        case Qt::Key_Shift:
            if (panDir.manhattanLength() == 0)
                return;

            break;

        default:
            return;
    }

    applyPan(event->modifiers());
}

// Evaluates the panDir field and sets kineticPanSpeed accordingly. Used in GeoMap::keyPressEvent and GeoMap::keyReleaseEvent
void GeoMap::applyPan(const Qt::KeyboardModifiers & modifiers)
{
    Q_ASSERT(panDir.manhattanLength() <= 2);

    if (panDir.manhattanLength() == 0) {
        // If no more direction keys are held down, decellerate
        panDecellerate = true;
    } else {
        // Otherwise, set new direction
        qreal panspeed = (modifiers & Qt::ShiftModifier) ? panSpeedFast : panSpeedNormal;

        if (panDir.manhattanLength() == 2) {
            // If 2 keys are held down, adjust speed to achieve the same speed in all 8 possible directions
            panspeed *= sqrt(0.5);
        }

        // Finally set the current panning speed
        kineticPanSpeed = QPointF(panDir) * panspeed;
    }
}

void GeoMap::wheelEvent(QGraphicsSceneWheelEvent * event)
{
    QPointF mousePos = event->pos();
    QGeoCoordinate mouseGeo = screenPositionToCoordinate(mousePos);

    if (event->delta() > 0) {
        //zoom in
        if (zoomLevel() < maximumZoomLevel()) {
            setZoomLevel(zoomLevel() + 1);
        }
    }
    else {
        //zoom out
        if (zoomLevel() > minimumZoomLevel()) {
            setZoomLevel(zoomLevel() - 1);
        }
    }

    QPointF panOffset = coordinateToScreenPosition(mouseGeo) - mousePos;
    panFloatWrapper(panOffset);

    event->accept();
}

void GeoMap::contextMenuEvent(QGraphicsSceneContextMenuEvent * event)
{
    QList<QGeoMapObject *> objectsAtCursor = mapObjectsAtScreenPosition(event->scenePos());

    QGeoMapObject * clickedMapObject = 0;

    if (!objectsAtCursor.isEmpty())
        clickedMapObject = objectsAtCursor.last();

    emit contextMenu(event, clickedMapObject); // TODO: problems arise if a signal attached to this is not a direct call
}

bool GeoMap::sceneEvent(QEvent *event)
{
    if (event->type() == QEvent::Gesture) {
        QGestureEvent * gestureEvent = static_cast<QGestureEvent *>(event);

        event->accept();
        foreach (QGesture * gesture, gestureEvent->gestures()) {
            switch (gesture->gestureType()) {
            case Qt::PinchGesture:
                handlePinchGesture(qobject_cast<QPinchGesture *>(gesture));
                return true;

            default:
                break;
            }
        }

        event->ignore();
    }

    return QGraphicsGeoMap::sceneEvent(event);
}

void GeoMap::handlePinchGesture(QPinchGesture * gesture)
{
    /*QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
    if (changeFlags & QPinchGesture::RotationAngleChanged) {
        qreal value = gesture->property("rotationAngle").toReal();
        qreal lastValue = gesture->property("lastRotationAngle").toReal();
        rotationAngle += value - lastValue;
    }
    if (changeFlags & QPinchGesture::ScaleFactorChanged) {
        qreal value = gesture->scaleFactor();

        currentStepScaleFactor = value;
    }
    if (gesture->state() == Qt::GestureFinished) {
        scaleFactor *= currentStepScaleFactor;
        currentStepScaleFactor = 1;
    }
    update();*/

    if (gesture->state() == Qt::GestureFinished) {
        if (gesture->totalChangeFlags() & QPinchGesture::ScaleFactorChanged) {
            panFloatWrapper(rect().center() - gesture->lastCenterPoint());
            setZoomLevel(zoomLevel() + log(gesture->totalScaleFactor()) / log(2.0));
            panFloatWrapper(gesture->centerPoint() - rect().center());
        }
    }
}
