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

#include "qgeorouteparser.h"

#include <QIODevice>
#include <QStringList>
#include <QString>

const quint8 HL_STREETNAME = 0x8;
const quint8 HL_ROUTENAME = 0x18;
const quint8 HL_POINTCOUNT = 0x19;
const quint8 HL_POINT = 0x1A;
const quint8 HL_DURATION = 0xB3;

const quint8 HL_MAN_INFO = 0x8C;
const quint8 HL_MAN_COUNT = 0x8D;
const quint8 HL_MAN_ACTION_ID = 0x8E;
const quint8 HL_MAN_STARTSEGMENT_INDEX = 0x95;
const quint8 HL_MAN_DIRECTIONS = 0xB1;
const quint8 HL_END_OF_RECORD = 0xFE;



QGeoRouteParser::QGeoRouteParser(const QGeoRouteRequest &request)
        : m_request(request)
{
}

QGeoRouteParser::~QGeoRouteParser()
{
}

bool QGeoRouteParser::parse(QIODevice* source)
{
    QList<QGeoRouteDataContainer> decoded = decodeTLV(QByteArray::fromHex(source->readAll()));

    if (decoded.count() == 0) {
        m_errorString = "Unknown response format";
        return false;
    }

    QGeoRoute route;
    QList<QGeoRouteSegment> routeSegments;
    route.setRequest(m_request);
    for (int itemCount = 0; itemCount < decoded.count(); ++itemCount) {
        QGeoRouteDataContainer cont = decoded[itemCount];
        if (cont.id == HL_POINTCOUNT) {
            int pointcount = int32FromByteArray(cont.data);
            ++itemCount; // step to next from HL_POINTCOUNT
            QList<QGeoCoordinate> path;
            for (int i = 0; i < pointcount && itemCount < decoded.count(); ++i, ++itemCount) {
                QGeoRouteDataContainer cont = decoded[itemCount];
                if (cont.id == HL_POINT) {
                    path.append(coordinateFromByteArray(cont.data));
                }
            }
            QGeoRouteSegment segment;
            segment.setPath(path);
            routeSegments.append(segment);
        } else if (cont.id == HL_DURATION) {
            route.setTravelTime(int32FromByteArray(cont.data));
        } else if (cont.id == HL_MAN_INFO) {
            QList<QGeoRouteDataContainer> decodedMan = decodeTLV(cont.data);
            for (int manCount = 0; manCount < decodedMan.count(); ++manCount) {
                QGeoRouteDataContainer man = decodedMan[manCount];
                if (man.id == HL_MAN_COUNT) {
                    int count = int32FromByteArray(man.data);
                    ++manCount; // step to next from HL_MAN_COUNT
                    int segmentIndex = -1;
                    QString instructionText;
                    for (int i = 0; i < count && manCount < decoded.count(); ++i, ++manCount) {
                        man = decodedMan[manCount];
                        if (man.id == HL_MAN_STARTSEGMENT_INDEX) {
                            segmentIndex = int32FromByteArray(man.data);
                        } else if (man.id == HL_MAN_DIRECTIONS) {
                            instructionText = man.data;
                        } else if (man.id == HL_STREETNAME) {
                            if (instructionText.isEmpty())
                                instructionText = man.data;
                        } else if (man.id == HL_ROUTENAME) {
                            if (instructionText.isEmpty())
                                instructionText = man.data;
                        } else if (man.id == HL_END_OF_RECORD) {
                            if (routeSegments.count() > segmentIndex) {
                                QGeoInstruction instruction;
                                instruction.setInstructionText(instructionText);
                                instruction.setPosition(routeSegments.at(segmentIndex).path().at(0));
                                routeSegments[segmentIndex].setInstruction(instruction);
                            }
                            instructionText.clear();
                            segmentIndex = -1;
                        }
                    }
                }
            }

        }
    }

    route.setRouteSegments(routeSegments);

    m_results.append(route);
    m_errorString = "";
    return true;
}

QList<QGeoRoute> QGeoRouteParser::results() const
{
    return m_results;
}

QString QGeoRouteParser::errorString() const
{
    return m_errorString;
}

QList<QGeoRouteDataContainer> QGeoRouteParser::decodeTLV(QByteArray data)
{
    QList<QGeoRouteDataContainer> decoded;
    QDataStream dataStream(&data, QIODevice::ReadOnly);
    dataStream.setByteOrder(QDataStream::LittleEndian);
    quint8 id;
    quint16 dataLen;
    while (!dataStream.atEnd()) {
        dataStream >> id >> dataLen;
        char* dataPtr = new char[dataLen];
        dataStream.readRawData(dataPtr, dataLen);
        QGeoRouteDataContainer cont;
        cont.id = id;
        cont.data = QByteArray::fromRawData(dataPtr, dataLen);
        decoded.append(cont);
    }

    return decoded;
}

QGeoCoordinate QGeoRouteParser::coordinateFromByteArray(QByteArray array)
{
    QDataStream dataStream(&array, QIODevice::ReadOnly);
    dataStream.setByteOrder(QDataStream::LittleEndian);
    quint32 latitude;
    quint32 longitude;
    dataStream >> longitude >> latitude;
    return QGeoCoordinate(fromInt32(latitude) - 90, fromInt32(longitude) - 180);
}

quint32 QGeoRouteParser::int32FromByteArray(QByteArray array)
{
    QDataStream dataStream(&array, QIODevice::ReadOnly);
    dataStream.setByteOrder(QDataStream::LittleEndian);
    quint32 value;
    dataStream >> value;
    return value;
}


double QGeoRouteParser::fromInt32(quint32 value)
{
    double converted = value + (value < 0 ? 2 ^ 32 : 0);
    converted /= 0xB60B60; // 0x100000000/360;
    return converted;
}
