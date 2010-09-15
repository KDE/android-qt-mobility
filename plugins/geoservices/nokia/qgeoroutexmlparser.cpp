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

#include "qgeoroutexmlparser.h"

#include <QXmlStreamReader>
#include <QIODevice>
#include <QStringList>
#include <QString>

QGeoRouteXmlParser::QGeoRouteXmlParser(const QGeoRouteRequest &request)
        : m_request(request),
        m_reader(0)
{
}

QGeoRouteXmlParser::~QGeoRouteXmlParser()
{
    if (m_reader)
        delete m_reader;
}

bool QGeoRouteXmlParser::parse(QIODevice* source)
{
    if (m_reader)
        delete m_reader;
    m_reader = new QXmlStreamReader(source);

    if (!parseRootElement()) {
        m_errorString = m_reader->errorString();
        return false;
    }

    m_errorString = "";

    return true;
}

QList<QGeoRoute> QGeoRouteXmlParser::results() const
{
    return m_results;
}

QString QGeoRouteXmlParser::errorString() const
{
    return m_errorString;
}

bool QGeoRouteXmlParser::parseRootElement()
{
    if (!m_reader->readNextStartElement()) {
        m_reader->raiseError("Expected a root element named \"CalculateRoute\" (no root element found).");
        return false;
    }

    if (m_reader->name() != "CalculateRoute" && m_reader->name() != "GetRoute")  {
        m_reader->raiseError(QString("The root element is expected to have the name \"CalculateRoute\" or \"GetRoute\" (root element was named \"%1\").").arg(m_reader->name().toString()));
        return false;
    }

    if (m_reader->readNextStartElement()) {
        if (m_reader->name() != "Response") {
            m_reader->raiseError(QString("Expected a element named \"Response\" (element was named \"%1\").").arg(m_reader->name().toString()));
            return false;
        }
    }

    while (m_reader->readNextStartElement()) {
        if (m_reader->name() == "MetaInfo") {
            m_reader->skipCurrentElement();
        } else if (m_reader->name() == "Route") {
            QGeoRoute route;
            route.setRequest(m_request);

            if (!parseRoute(&route))
                continue; //route parsing failed move on to the next

            m_results.append(route);
        } else if (m_reader->name() == "Progress") {
            //TODO: updated route progress
            m_reader->skipCurrentElement();
        } else {
            m_reader->raiseError(QString("Did not expect a child element named \"%1\".").arg(
                                     m_reader->name().toString()));
            return false;
        }
    }

    return true;
}

bool QGeoRouteXmlParser::parseRoute(QGeoRoute *route)
{
    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == "Route");
    instructions.clear();
    segments.clear();

    m_reader->readNext();
    bool succeeded = true;
    while (!(m_reader->tokenType() == QXmlStreamReader::EndElement && m_reader->name() == "Route")) {
        if (m_reader->tokenType() == QXmlStreamReader::StartElement && succeeded) {
            if (m_reader->name() == "RouteId") {
                route->setRouteId(m_reader->readElementText());
            }
            //else if (m_reader->name() == "Waypoint") {
            //    succeeded = parseWaypoint(route);
            //}
            else if (m_reader->name() == "Mode") {
                succeeded = parseMode(route);
            } else if (m_reader->name() == "Shape") {
                QString elementName = m_reader->name().toString();
                QList<QGeoCoordinate> path;
                succeeded = parseGeoPoints(m_reader->readElementText(), &path, elementName);
                if (succeeded)
                    route->setPath(path);
            } else if (m_reader->name() == "BoundingBox") {
                QGeoBoundingBox bounds;
                succeeded = parseBoundingBox(bounds);
                if (succeeded)
                    route->setBounds(bounds);
            } else if (m_reader->name() == "Leg") {
                succeeded = parseLeg();
            } else if (m_reader->name() == "Summary") {
                succeeded = parseSummary(route);
            } else {
                m_reader->skipCurrentElement();
            }
        }
        m_reader->readNext();
    }
    if (succeeded)
        succeeded = postProcessRoute(route);

    return succeeded;
}

bool QGeoRouteXmlParser::parseLeg()
{
    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == "Leg");

    m_reader->readNext();
    bool succeeded = true;
    while (!(m_reader->tokenType() == QXmlStreamReader::EndElement && m_reader->name() == "Leg")) {
        if (m_reader->tokenType() == QXmlStreamReader::StartElement && succeeded) {
            if (m_reader->name() == "Maneuver") {
                succeeded = parseManeuver();
            } else if (m_reader->name() == "Link") {
                succeeded = parseLink();
            } else {
                m_reader->skipCurrentElement();
            }
        }
        m_reader->readNext();
    }

    return succeeded;
}

bool QGeoRouteXmlParser::postProcessRoute(QGeoRoute *route)
{
    QList<QGeoRouteSegment> routesegments;

    for (int i = 0; i < segments.count(); ++i) {
        for (int j = instructions.count() - 1; j >= 0; --j) {
            if (instructions[j].id == segments[i].instructionId || instructions[j].toId == segments[i].id) {
                if (!segments[i].segment.instruction().instructionText().isEmpty()) {
                    if (segments[i].segment.instruction() != instructions[j].instruction) {
                        QGeoRouteSegment segment;
                        segment.setInstruction(instructions[j].instruction);
                        routesegments.append(segment);
                        instructions.removeAt(j);
                    }
                } else {
                    segments[i].segment.setInstruction(instructions[j].instruction);
                    instructions.removeAt(j);
                }
            }
        }
        routesegments.append(segments[i].segment);
    }

    for (int i = 0;i < instructions.count();++i) {
        //Add orphan instruction into new empty segment
        QGeoRouteSegment segment;
        segment.setInstruction(instructions[i].instruction);
        routesegments.append(segment);
        instructions.removeAt(i);
    }
    route->setRouteSegments(routesegments);

    instructions.clear();
    segments.clear();
    return true;
}

/*
bool QGeoRouteXmlParser::parseWaypoint(QGeoRoute *route)
{
    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == "Waypoint");
    m_reader->readNext();
    QList<QGeoCoordinate> path(route->pathSummary());

    while (!(m_reader->tokenType() == QXmlStreamReader::EndElement && m_reader->name() == "Waypoint")) {
        if (m_reader->tokenType() == QXmlStreamReader::StartElement) {
            if (m_reader->name() == "MappedPosition") {
                QGeoCoordinate coordinates;
                if(!parseCoordinates(coordinates))
                    return false;
                path.append(coordinates);
            }
            else {
                m_reader->skipCurrentElement();
            }
        }
        m_reader->readNext();
    }
    route->setPathSummary(path);
    return true;
}
*/

bool QGeoRouteXmlParser::parseMode(QGeoRoute *route)
{
    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == "Mode");
    m_reader->readNext();

    while (!(m_reader->tokenType() == QXmlStreamReader::EndElement && m_reader->name() == "Mode")) {
        if (m_reader->tokenType() == QXmlStreamReader::StartElement) {
            if (m_reader->name() == "TransportModes") {
                QString value = m_reader->readElementText();
                if (value == "car")
                    route->setTravelMode(QGeoRouteRequest::CarTravel);
                else if (value == "pedestrian")
                    route->setTravelMode(QGeoRouteRequest::PedestrianTravel);
                else if (value == "publicTransport")
                    route->setTravelMode(QGeoRouteRequest::PublicTransitTravel);
                else if (value == "bicycle")
                    route->setTravelMode(QGeoRouteRequest::BicycleTravel);
                else if (value == "truck")
                    route->setTravelMode(QGeoRouteRequest::TruckTravel);
                else // unsupported optimization
                    return false;
            } else {
                m_reader->skipCurrentElement();
            }
        }
        m_reader->readNext();
    }
    return true;
}

bool QGeoRouteXmlParser::parseSummary(QGeoRoute *route)
{
    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == "Summary");
    m_reader->readNext();

    while (!(m_reader->tokenType() == QXmlStreamReader::EndElement && m_reader->name() == "Summary")) {
        if (m_reader->tokenType() == QXmlStreamReader::StartElement) {
            if (m_reader->name() == "Distance") {
                route->setDistance(m_reader->readElementText().toDouble());
            } else if (m_reader->name() == "TrafficTime") {
                route->setTravelTime(m_reader->readElementText().toDouble());
            } else {
                m_reader->skipCurrentElement();
            }
        }
        m_reader->readNext();
    }

    return true;
}

bool QGeoRouteXmlParser::parseCoordinates(QGeoCoordinate &coord)
{
    QString currentElement = m_reader->name().toString();
    m_reader->readNext();

    while (!(m_reader->tokenType() == QXmlStreamReader::EndElement && m_reader->name() == currentElement)) {
        if (m_reader->tokenType() == QXmlStreamReader::StartElement) {
            QString name = m_reader->name().toString();
            QString value = m_reader->readElementText();
            if (name == "Latitude")
                coord.setLatitude(value.toDouble());
            else if (name == "Longitude")
                coord.setLongitude(value.toDouble());
        }
        m_reader->readNext();
    }

    return true;
}

bool QGeoRouteXmlParser::parseManeuver()
{
    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == "Maneuver");

    if (!m_reader->attributes().hasAttribute("id")) {
        m_reader->raiseError("The element \"Maneuver\" did not have the required attribute \"id\".");
        return false;
    }
    QGeoInstructionContainer instructionContainer;
    instructionContainer.id = m_reader->attributes().value("id").toString();

    m_reader->readNext();
    while (!(m_reader->tokenType() == QXmlStreamReader::EndElement && m_reader->name() == "Maneuver")) {
        if (m_reader->tokenType() == QXmlStreamReader::StartElement) {
            if (m_reader->name() == "Position") {
                QGeoCoordinate coordinates;
                if (parseCoordinates(coordinates))
                    instructionContainer.instruction.setPosition(coordinates);
            } else if (m_reader->name() == "Instruction") {
                instructionContainer.instruction.setInstructionText(m_reader->readElementText());
            } else if (m_reader->name() == "ToLink") {
                instructionContainer.toId = m_reader->readElementText();
            } else if (m_reader->name() == "TravelTime") {
                instructionContainer.instruction.setTimeToNextInstruction(m_reader->readElementText().toInt());
            } else if (m_reader->name() == "Length") {
                instructionContainer.instruction.setDistanceToNextInstruction(m_reader->readElementText().toDouble());
            } else if (m_reader->name() == "Direction") {
                QString value = m_reader->readElementText();
                if (value == "forward")
                    instructionContainer.instruction.setDirection(QGeoInstruction::DirectionForward);
                else if (value == "bearRight")
                    instructionContainer.instruction.setDirection(QGeoInstruction::DirectionBearRight);
                else if (value == "lightRight")
                    instructionContainer.instruction.setDirection(QGeoInstruction::DirectionLightRight);
                else if (value == "right")
                    instructionContainer.instruction.setDirection(QGeoInstruction::DirectionRight);
                else if (value == "hardRight")
                    instructionContainer.instruction.setDirection(QGeoInstruction::DirectionHardRight);
                else if (value == "uTurnRight")
                    instructionContainer.instruction.setDirection(QGeoInstruction::DirectionUTurnRight);
                else if (value == "uTurnLeft")
                    instructionContainer.instruction.setDirection(QGeoInstruction::DirectionUTurnLeft);
                else if (value == "hardLeft")
                    instructionContainer.instruction.setDirection(QGeoInstruction::DirectionHardLeft);
                else if (value == "left")
                    instructionContainer.instruction.setDirection(QGeoInstruction::DirectionLeft);
                else if (value == "lightLeft")
                    instructionContainer.instruction.setDirection(QGeoInstruction::DirectionLightLeft);
                else if (value == "bearLeft")
                    instructionContainer.instruction.setDirection(QGeoInstruction::DirectionBearLeft);
                else
                    instructionContainer.instruction.setDirection(QGeoInstruction::NoDirection);
            } else {
                m_reader->skipCurrentElement();
            }
        }
        m_reader->readNext();
    }

    instructions.append(instructionContainer);
    return true;
}

bool QGeoRouteXmlParser::parseLink()
{
    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == "Link");
    m_reader->readNext();

    QGeoRouteSegmentContainer segmentContainer;

    while (!(m_reader->tokenType() == QXmlStreamReader::EndElement && m_reader->name() == "Link")) {
        if (m_reader->tokenType() == QXmlStreamReader::StartElement) {
            if (m_reader->name() == "LinkId") {
                segmentContainer.id = m_reader->readElementText();
            } else if (m_reader->name() == "Shape") {
                QString elementName = m_reader->name().toString();
                QList<QGeoCoordinate> path;
                parseGeoPoints(m_reader->readElementText(), &path, elementName);
                segmentContainer.segment.setPath(path);
            } else if (m_reader->name() == "Length") {
                segmentContainer.segment.setDistance(m_reader->readElementText().toDouble());
            } else if (m_reader->name() == "Maneuver") {
                segmentContainer.instructionId = m_reader->readElementText();
            } else {
                m_reader->skipCurrentElement();
            }
        }
        m_reader->readNext();
    }
    segments.append(segmentContainer);
    return true;
}

bool QGeoRouteXmlParser::parseGeoPoints(const QString& strPoints, QList<QGeoCoordinate> *geoPoints, const QString &elementName)
{
    QStringList rawPoints = strPoints.split(' ');

    for (int i = 0; i < rawPoints.length(); ++i) {
        QStringList coords = rawPoints[i].split(',');

        if (coords.length() != 2) {
            m_reader->raiseError(QString("Each of the space separated values of \"%1\" is expected to be a comma separated pair of coordinates (value was \"%2\")").arg(elementName).arg(rawPoints[i]));
            return false;
        }

        bool ok = false;
        QString latString = coords[0];
        double lat = latString.toDouble(&ok);

        if (!ok) {
            m_reader->raiseError(QString("The latitude portions of \"%1\" are expected to have a value convertable to a double (value was \"%2\")").arg(elementName).arg(latString));
            return false;
        }

        QString lngString = coords[1];
        double lng = lngString.toDouble(&ok);

        if (!ok) {
            m_reader->raiseError(QString("The longitude portions of \"%1\" are expected to have a value convertable to a double (value was \"%2\")").arg(elementName).arg(lngString));
            return false;
        }

        QGeoCoordinate geoPoint(lat, lng);
        geoPoints->append(geoPoint);
    }

    return true;
}

bool QGeoRouteXmlParser::parseBoundingBox(QGeoBoundingBox &bounds)
{
    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == "BoundingBox");

    QGeoCoordinate tl;
    QGeoCoordinate br;

    m_reader->readNext();
    while (!(m_reader->tokenType() == QXmlStreamReader::EndElement && m_reader->name() == "BoundingBox")) {
        if (m_reader->tokenType() == QXmlStreamReader::StartElement) {
            if (m_reader->name() == "TopLeft") {
                QGeoCoordinate coordinates;
                if (parseCoordinates(coordinates))
                    tl = coordinates;
            } else if (m_reader->name() == "BottomRight") {
                QGeoCoordinate coordinates;
                if (parseCoordinates(coordinates))
                    br = coordinates;
            } else {
                m_reader->skipCurrentElement();
            }
        }
        m_reader->readNext();
    }

    if (tl.isValid() && br.isValid()) {
        bounds = QGeoBoundingBox(tl, br);
        return true;
    }

    return false;
}
