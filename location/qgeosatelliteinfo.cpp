/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qgeosatelliteinfo.h"

#include <QHash>
#include <QDebug>


class QGeoSatelliteInfoPrivate
{
public:
    int prn;
    int signal;
    QHash<int, qreal> doubleProps;
};


/*!
    \class QGeoSatelliteInfo
    \brief The QGeoSatelliteInfo class contains basic information about a satellite.

    \sa QGeoSatelliteInfoSource
*/

/*!
    \enum QGeoSatelliteInfo::Property
    Defines the properties for the satellite information.

    \value Elevation The elevation of the satellite, in degrees.
    \value Azimuth The azimuth to true north, in degrees.
*/


/*!
    Creates a satellite information object.
*/
QGeoSatelliteInfo::QGeoSatelliteInfo()
    : d(new QGeoSatelliteInfoPrivate)
{
    d->prn = -1;
    d->signal = -1;
}

/*!
    Creates a satellite information object with the values of \a other.
*/

QGeoSatelliteInfo::QGeoSatelliteInfo(const QGeoSatelliteInfo &other)
    : d(new QGeoSatelliteInfoPrivate)
{
    operator=(other);
}

/*!
    Destroys a satellite information object.
*/
QGeoSatelliteInfo::~QGeoSatelliteInfo()
{
    delete d;
}

/*!
    Assigns the values from \a other to this object.
*/
QGeoSatelliteInfo &QGeoSatelliteInfo::operator=(const QGeoSatelliteInfo &other)
{
    if (this == &other)
        return *this;

    d->prn = other.d->prn;
    d->signal = other.d->signal;
    d->doubleProps = other.d->doubleProps;
    return *this;
}

/*!
    Returns true if all the information for this satellite
    are the same as those of \a other.
*/
bool QGeoSatelliteInfo::operator==(const QGeoSatelliteInfo &other) const
{
    return d->prn == other.d->prn
            && d->signal == other.d->signal
            && d->doubleProps == other.d->doubleProps;
}

/*!
    \fn bool QGeoSatelliteInfo::operator!=(const QGeoSatelliteInfo &other) const;

    Returns true if any of the information for this satellite
    are not the same as those of \a other.
*/

/*!
    Sets the PRN (Pseudo-random noise) number to \a prn.

    The PRN number can be used to identify a satellite.
*/
void QGeoSatelliteInfo::setPrnNumber(int prn)
{
    d->prn = prn;
}

/*!
    Returns the PRN (Pseudo-random noise) number, or -1 if the value has not been set.
*/

int QGeoSatelliteInfo::prnNumber() const
{
    return d->prn;
}

/*!
    Sets the signal strength to \a signalStrength, in decibels.
*/
void QGeoSatelliteInfo::setSignalStrength(int signalStrength)
{
    d->signal = signalStrength;
}

/*!
    Returns the signal strength, or -1 if the value has not been set.
*/
int QGeoSatelliteInfo::signalStrength() const
{
    return d->signal;
}

/*!
    Sets the value for \a property to \a value.
*/
void QGeoSatelliteInfo::setProperty(Property property, qreal value)
{
    d->doubleProps[int(property)] = value;
}

/*!
    Returns the value of the specified \a property as a qreal value.

    Returns -1 if the value has not been set.

    \sa hasProperty(), setProperty()
*/
qreal QGeoSatelliteInfo::property(Property property) const
{
    if (d->doubleProps.contains(int(property)))
        return d->doubleProps[int(property)];
    return -1;
}

/*!
    Removes the specified \a property and its value.
*/
void QGeoSatelliteInfo::removeProperty(Property property)
{
    d->doubleProps.remove(int(property));
}

/*!
    Returns true if the specified \a property is present in this update.
*/
bool QGeoSatelliteInfo::hasProperty(Property property) const
{
    return d->doubleProps.contains(int(property));
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QGeoSatelliteInfo &info)
{
    dbg.nospace() << "QGeoSatelliteInfo(PRN=" << info.d->prn;
    dbg.nospace() << ", signal-strength=";
    dbg.nospace() << info.d->signal;

    QList<int> props = info.d->doubleProps.keys();
    for (int i=0; i<props.count(); i++) {
        dbg.nospace() << ", ";
        switch (props[i]) {
            case QGeoSatelliteInfo::Elevation:
                dbg.nospace() << "Elevation=";
                break;
            case QGeoSatelliteInfo::Azimuth:
                dbg.nospace() << "Azimuth=";
                break;
        }
        dbg.nospace() << info.d->doubleProps[props[i]];
    }
    dbg.nospace() << ')';
    return dbg;
}
#endif
