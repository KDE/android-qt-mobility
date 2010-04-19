/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt3D module of the Qt Toolkit.
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

#include "light.h"
#include <QtCore/qmath.h>
#include <QtGui/qmatrix4x4.h>

/*!
    \class Light
    \brief The Light class represents the parameters of a light in a 3D scene.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::painting

    The functions in this class are a convenience wrapper for the OpenGL enumerations
    which control each light source in a 3D scene.  For the general ambient light in a
    scene, not from a source refer to QGLLightModel.

    The ambient, diffuse and specular components of the light can be controlled for by
    colour, and are set by the following functions:
    \list
    \o setAmbientColor()
    \o setDiffuseColor()
    \o setSpecularColor()
    \endlist
    Other than changing intensity by using darker color values, see below for how to
    change the intensity of the light with distance from the lit object.

    Light sources are of two types, directional and positional, described by the
    enumeration Light::LightType.  By default a light source is directional.

    A directional light source is infinitely distant, such that its rays are all
    parallel, to a direction \bold vector.  This vector is set by the setDirection()
    function.  If the light source is not directional when setDirection() is called, its
    type is changed to Directional.  Directional light sources model real world light
    sources like the sun.  Such light sources are not located at any particular point
    and affect the scene evenly.

    In the OpenGL model-view, a directional light is represented by a 4d vector, with
    the 4th value, \c w, set to 0.0f.  This means that the spatial data of the light
    is not changed during certain transformations, for example translation.
    See \l {3D Math Basis} for a fuller explanation of this.

    Calling the setPosition() function defines the light source to be located at a given
    \bold point, a finite distance from the lit object.  If the light source is not
    positional when this function is called its type is changed to Positional.

    A positional light source models a real world lamp, such as a light-bulb or
    car headlight.  Since it is a finite distance from the lit object the rays diverge
    and lighting calculations are thus more complex.

    In OpenGL the light has its spatial data \c w value set to 1.0f, resulting in the
    lights position being changed along with other objects in the scene by transformations
    such as translation.  See \l {3D Math Basis} for more.

    Positional lights are by default point sources, like the light-bulb where light
    radiates in all directions.  Calling the spotAngle() function sets an angle to
    restrict the light from a positional source to a cone like the car headlight.  This
    makes the light behave like a spotlight, where objects outside of the cone of the
    light do not receive any light from that source.

    The spotlight may be further specified by its spotExponent() which dictates the
    behaviour of the cutoff at the lighting cone; and by the attenuation functions
    which are the terms in the following equation:
    \image attenuation.png
    here \c d is the distance of the light from the lit object, and A is the attenuation
    factor which is a multiplier of the light source, determining its intensity at the
    lit object.  The terms in the denominator are:
    \list
    \o constantAttenuation() - default 1.0
    \raw HTML
    k<sub>c</sub>
    \endraw
    \o linearAttenuation() - default 0.0
    \raw HTML
    k<sub>l</sub>
    \endraw
    \o quadraticAttenuation() - default 0.0
    \raw HTML
    k<sub>q</sub>
    \endraw
    \endlist
    When these terms are large the light contributed by the source becomes much weaker
    with distance from the object.
*/

class LightPrivate
{
public:
    LightPrivate() :
        type(Light::Directional),
        position(0.0f, 0.0f, 1.0f),
        ambientColor(0, 0, 0, 255),
        diffuseColor(255, 255, 255, 255),
        specularColor(255, 255, 255, 255),
        spotDirection(0.0f, 0.0f, -1.0f),
        spotExponent(0.0f),
        spotAngle(180.0f),
        spotCosAngle(-1.0f),
        constantAttenuation(1.0f),
        linearAttenuation(0.0f),
        quadraticAttenuation(0.0f)
    {
    }

    Light::LightType type;
    QVector3D position;
    QColor ambientColor;
    QColor diffuseColor;
    QColor specularColor;
    QVector3D spotDirection;
    qreal spotExponent;
    qreal spotAngle;
    qreal spotCosAngle;
    qreal constantAttenuation;
    qreal linearAttenuation;
    qreal quadraticAttenuation;
};


/*!
    \enum Light::LightType
    This enum defines the possible types of light.

    \value Directional The default.  Directional lights are infinitely
           distant from the lit object, and its rays are parallel to a
           direction vector.  Use setDirection() to make a light Directional.
    \value Positional  Positional lights are a finite distance from the
           lit object.  Complex lighting with spot-lights, and attenuation
           are enabled with Positional lighting.  Use setPosition() to
           make a light Positional.
*/

/*!
    Constructs a new light parameters object with default values
    and attaches it to \a parent.
*/
Light::Light(QObject *parent)
    : QObject(parent), d_ptr(new LightPrivate)
{
}

/*!
    Destroys this light parameters object.
*/
Light::~Light()
{
}

/*!
    \property Light::type
    \brief the type of this light; Positional or Directional.

    \sa position(), direction()
*/
Light::LightType Light::type() const
{
    Q_D(const Light);
    return d->type;
}

/*!
    \property Light::position
    \brief the position of this light if it is Positional; or a null
    QVector3D if it is Directional.  By default, the light is Directional.

    For a Positional light, the return value is the location in model space
    of the light, at some finite distance from the origin.  The value is
    transformed by the model-view transformation when the light
    parameters are applied.

    When the light is Positional OpenGL will obey other parameters relating
    to the light's position, such as attenuation and spot angle.

    Setting the position converts the light from Directional to Positional.

    \sa direction(), type(), positionChanged()
*/
QVector3D Light::position() const
{
    Q_D(const Light);
    if (d->type == Positional)
        return d->position;
    else
        return QVector3D();
}

void Light::setPosition(const QVector3D& point)
{
    Q_D(Light);
    if (d->type == Positional) {
        if (d->position != point) {
            // Only the position() has changed.
            d->position = point;
            emit positionChanged();
            emit lightChanged();
        }
    } else {
        // Both the position() and direction() are changed.
        d->type = Positional;
        d->position = point;
        emit positionChanged();
        emit directionChanged();
        emit lightChanged();
    }
}

/*!
    \property Light::direction
    \brief the direction of this light if it is Directional; or a null
    QVector3D if it is Positional.  By default, the light is Directional.

    For a Directional light, the return value is the direction vector of
    an infinitely distant directional light, like the sun.

    The default value is (0, 0, 1), for a directional light pointing along
    the positive z-axis.

    OpenGL will ignore other parameters such as attenuation and spot angle
    when this value is set, since a directional light has no location.
    In order to set the direction of a spot light, use the setSpotDirection()
    function.

    Setting the direction converts the light from Positional to Directional.

    \sa position(), type(), directionChanged()
*/
QVector3D Light::direction() const
{
    Q_D(const Light);
    if (d->type == Directional)
        return d->position;
    else
        return QVector3D();
}

void Light::setDirection(const QVector3D& value)
{
    Q_D(Light);
    if (d->type == Directional) {
        if (d->position != value) {
            // Only the direction() has changed.
            d->position = value;
            emit directionChanged();
            emit lightChanged();
        }
    } else {
        // Both the position() and direction() are changed.
        d->type = Directional;
        d->position = value;
        emit positionChanged();
        emit directionChanged();
        emit lightChanged();
    }
}

/*!
    \property Light::ambientColor
    \brief the ambient color of this light.  The default value is black.

    \sa diffuseColor(), specularColor(), ambientColorChanged()
*/
QColor Light::ambientColor() const
{
    Q_D(const Light);
    return d->ambientColor;
}

void Light::setAmbientColor(const QColor& value)
{
    Q_D(Light);
    if (d->ambientColor != value) {
        d->ambientColor = value;
        emit ambientColorChanged();
        emit lightChanged();
    }
}

/*!
    \property Light::diffuseColor
    \brief the diffuse color of this light.  The default value is white.

    \sa ambientColor(), specularColor(), diffuseColorChanged()
*/
QColor Light::diffuseColor() const
{
    Q_D(const Light);
    return d->diffuseColor;
}

void Light::setDiffuseColor(const QColor& value)
{
    Q_D(Light);
    if (d->diffuseColor != value) {
        d->diffuseColor = value;
        emit diffuseColorChanged();
        emit lightChanged();
    }
}

/*!
    \property Light::specularColor
    \brief the specular color of this light.  The default value is white.

    \sa ambientColor(), diffuseColor(), specularColorChanged()
*/
QColor Light::specularColor() const
{
    Q_D(const Light);
    return d->specularColor;
}

void Light::setSpecularColor(const QColor& value)
{
    Q_D(Light);
    if (d->specularColor != value) {
        d->specularColor = value;
        emit specularColorChanged();
        emit lightChanged();
    }
}

/*!
    \property Light::spotDirection
    \brief the direction that a spot-light is shining in.

    A spotlight is a positional light that has a value other than the default
    for spot angle.  The default value is (0, 0, -1).

    This parameter has no effect on a Directional light.

    \sa spotExponent(), spotDirectionChanged()
*/
QVector3D Light::spotDirection() const
{
    Q_D(const Light);
    return d->spotDirection;
}

void Light::setSpotDirection(const QVector3D& vector)
{
    Q_D(Light);
    if (d->spotDirection != vector) {
        d->spotDirection = vector;
        emit spotDirectionChanged();
        emit lightChanged();
    }
}

/*!
    \property Light::spotExponent
    \brief the exponent value between 0 and 128 that indicates the
    intensity distribution of a spot-light.  The default value is 0,
    indicating uniform light distribution.

    This parameter has no effect on a Directional light.

    \sa spotAngle(), setPosition(), spotExponentChanged()
*/
qreal Light::spotExponent() const
{
    Q_D(const Light);
    return d->spotExponent;
}

void Light::setSpotExponent(qreal value)
{
    Q_D(Light);
    if (d->spotExponent != value) {
        d->spotExponent = value;
        emit spotExponentChanged();
        emit lightChanged();
    }
}

/*!
    \property Light::spotAngle
    \brief the angle over which light is spread from this light.
    It should be between 0 and 90 for spot lights, and 180 for
    uniform light distribution from a remote light source.
    The default value is 180.

    This parameter has no effect on a Directional light.

    \sa spotCosAngle(), spotAngleChanged()
*/
qreal Light::spotAngle() const
{
    Q_D(const Light);
    return d->spotAngle;
}

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void Light::setSpotAngle(qreal value)
{
    Q_D(Light);
    if (d->spotAngle != value) {
        d->spotAngle = value;
        if (value != 180.0f)
            d->spotCosAngle = qCos(value * M_PI / 180.0f);
        else
            d->spotCosAngle = -1.0f;
        emit spotAngleChanged();
        emit lightChanged();
    }
}

/*!
    Returns the cosine of spotAngle(), or -1 if spotAngle() is 180.

    The cosine of spotAngle() is useful in lighting algorithms.
    This function returns a cached copy of the cosine so that it
    does not need to be computed every time the lighting parameters
    are read.

    \sa spotAngle()
*/
qreal Light::spotCosAngle() const
{
    Q_D(const Light);
    return d->spotCosAngle;
}

/*!
    \property Light::constantAttenuation
    \brief the constant light attenuation factor.  The default value is 1.

    This parameter has no effect on a Directional light.

    \sa linearAttenuation(), quadraticAttenuation()
    \sa constantAttenuationChanged()
*/
qreal Light::constantAttenuation() const
{
    Q_D(const Light);
    return d->constantAttenuation;
}

void Light::setConstantAttenuation(qreal value)
{
    Q_D(Light);
    if (d->constantAttenuation != value) {
        d->constantAttenuation = value;
        emit constantAttenuationChanged();
        emit lightChanged();
    }
}

/*!
    \property Light::linearAttenuation
    \brief the linear light attenuation factor.  The default value is 0.

    This parameter has no effect on a Directional light.

    \sa constantAttenuation(), quadraticAttenuation()
    \sa linearAttenuationChanged()
*/
qreal Light::linearAttenuation() const
{
    Q_D(const Light);
    return d->linearAttenuation;
}

void Light::setLinearAttenuation(qreal value)
{
    Q_D(Light);
    if (d->linearAttenuation != value) {
        d->linearAttenuation = value;
        emit linearAttenuationChanged();
        emit lightChanged();
    }
}

/*!
    \property Light::quadraticAttenuation
    \brief the quadratic light attenuation factor.  The default value is 0.

    This parameter has no effect on a Directional light.

    \sa constantAttenuation(), linearAttenuation()
    \sa quadraticAttenuationChanged()
*/
qreal Light::quadraticAttenuation() const
{
    Q_D(const Light);
    return d->quadraticAttenuation;
}

void Light::setQuadraticAttenuation(qreal value)
{
    Q_D(Light);
    if (d->quadraticAttenuation != value) {
        d->quadraticAttenuation = value;
        emit quadraticAttenuationChanged();
        emit lightChanged();
    }
}

/*!
    Returns the position of this light after transforming it from
    world co-ordinates to eye co-ordinates using \a transform.

    If the light is Directional, then direction() will be transformed,
    after setting the w co-ordinate to 0.  If the light is Positional,
    then position() will be transformed after setting the w co-ordinate
    to 1.

    The returned result is suitable to be applied to the GL_POSITION
    property of \c{glLight()}, assuming that the modelview transformation
    in the GL context is set to the identity.

    \sa eyeSpotDirection()
*/
QVector4D Light::eyePosition(const QMatrix4x4& transform) const
{
    Q_D(const Light);
    if (d->type == Directional)
        return transform * QVector4D(d->position, 0.0f);
    else
        return transform * QVector4D(d->position, 1.0f);
}

/*!
    Returns the spotDirection() for this light after transforming it
    from world co-ordinates to eye co-ordinates using the top-left
    3x3 submatrix within \a transform.

    The returned result is suitable to be applied to the GL_SPOT_DIRECTION
    property of \c{glLight()}, assuming that the modelview transformation
    in the GL context is set to the identity.

    \sa eyePosition()
*/
QVector3D Light::eyeSpotDirection
    (const QMatrix4x4& transform) const
{
    Q_D(const Light);
    return transform.mapVector(d->spotDirection);
}

/*!
    \fn void Light::positionChanged()

    This signal is emitted when position() changes, or when the type()
    changes between Directional and Positional.

    \sa position(), lightChanged()
*/

/*!
    \fn void Light::directionChanged()

    This signal is emitted when direction() changes, or when the type()
    changes between Directional and Positional.

    \sa direction(), lightChanged()
*/

/*!
    \fn void Light::ambientColorChanged()

    This signal is emitted when ambientColor() changes.

    \sa ambientColor(), lightChanged()
*/

/*!
    \fn void Light::diffuseColorChanged()

    This signal is emitted when diffuseColor() changes.

    \sa diffuseColor(), lightChanged()
*/

/*!
    \fn void Light::specularColorChanged()

    This signal is emitted when specularColor() changes.

    \sa specularColor(), lightChanged()
*/

/*!
    \fn void Light::spotDirectionChanged()

    This signal is emitted when spotDirection() changes.

    \sa spotDirection(), lightChanged()
*/

/*!
    \fn void Light::spotExponentChanged()

    This signal is emitted when spotExponent() changes.

    \sa spotExponent(), lightChanged()
*/

/*!
    \fn void Light::spotAngleChanged()

    This signal is emitted when spotAngle() changes.

    \sa spotAngle(), lightChanged()
*/

/*!
    \fn void Light::constantAttenuationChanged()

    This signal is emitted when constantAttenuation() changes.

    \sa constantAttenuation(), lightChanged()
*/

/*!
    \fn void Light::linearAttenuationChanged()

    This signal is emitted when linearAttenuation() changes.

    \sa linearAttenuation(), lightChanged()
*/

/*!
    \fn void Light::quadraticAttenuationChanged()

    This signal is emitted when quadraticAttenuation() changes.

    \sa quadraticAttenuation(), lightChanged()
*/

/*!
    \fn void Light::lightChanged()

    This signal is emitted when any of the properties on the light changes.
*/
