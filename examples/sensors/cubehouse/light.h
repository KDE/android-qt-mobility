/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt3D module of the Qt Toolkit.
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

#ifndef LIGHT_H
#define LIGHT_H

#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>
#include <QtGui/qcolor.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qvector4d.h>

class LightPrivate;
QT_BEGIN_NAMESPACE
class QMatrix4x4;
QT_END_NAMESPACE

class Light : public QObject
{
    Q_OBJECT
    Q_ENUMS(LightType)
    Q_PROPERTY(LightType type READ type)
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QVector3D direction READ direction WRITE setDirection NOTIFY directionChanged)
    Q_PROPERTY(QColor ambientColor READ ambientColor WRITE setAmbientColor NOTIFY ambientColorChanged)
    Q_PROPERTY(QColor diffuseColor READ diffuseColor WRITE setDiffuseColor NOTIFY diffuseColorChanged)
    Q_PROPERTY(QColor specularColor READ specularColor WRITE setSpecularColor NOTIFY specularColorChanged)
    Q_PROPERTY(QVector3D spotDirection READ spotDirection WRITE setSpotDirection NOTIFY spotDirectionChanged)
    Q_PROPERTY(qreal spotExponent READ spotExponent WRITE setSpotExponent NOTIFY spotExponentChanged)
    Q_PROPERTY(qreal spotAngle READ spotAngle WRITE setSpotAngle NOTIFY spotAngleChanged)
    Q_PROPERTY(qreal constantAttenuation READ constantAttenuation WRITE setConstantAttenuation NOTIFY constantAttenuationChanged)
    Q_PROPERTY(qreal linearAttenuation READ linearAttenuation WRITE setLinearAttenuation NOTIFY linearAttenuationChanged)
    Q_PROPERTY(qreal quadraticAttenuation READ quadraticAttenuation WRITE setQuadraticAttenuation NOTIFY quadraticAttenuationChanged)
public:
    enum LightType {
        Directional,
        Positional
    };

    Light(QObject *parent = 0);
    ~Light();

    Light::LightType type() const;

    QVector3D position() const;
    void setPosition(const QVector3D& value);

    QVector3D direction() const;
    void setDirection(const QVector3D& value);

    QColor ambientColor() const;
    void setAmbientColor(const QColor& value);

    QColor diffuseColor() const;
    void setDiffuseColor(const QColor& value);

    QColor specularColor() const;
    void setSpecularColor(const QColor& value);

    QVector3D spotDirection() const;
    void setSpotDirection(const QVector3D& value);

    qreal spotExponent() const;
    void setSpotExponent(qreal value);

    qreal spotAngle() const;
    void setSpotAngle(qreal value);

    qreal spotCosAngle() const;

    qreal constantAttenuation() const;
    void setConstantAttenuation(qreal value);

    qreal linearAttenuation() const;
    void setLinearAttenuation(qreal value);

    qreal quadraticAttenuation() const;
    void setQuadraticAttenuation(qreal value);

    QVector4D eyePosition(const QMatrix4x4& transform) const;
    QVector3D eyeSpotDirection(const QMatrix4x4& transform) const;

Q_SIGNALS:
    void positionChanged();
    void directionChanged();
    void ambientColorChanged();
    void diffuseColorChanged();
    void specularColorChanged();
    void spotDirectionChanged();
    void spotExponentChanged();
    void spotAngleChanged();
    void constantAttenuationChanged();
    void linearAttenuationChanged();
    void quadraticAttenuationChanged();
    void lightChanged();

private:
    QScopedPointer<LightPrivate> d_ptr;

    Q_DECLARE_PRIVATE(Light)
    Q_DISABLE_COPY(Light)
};

#endif
