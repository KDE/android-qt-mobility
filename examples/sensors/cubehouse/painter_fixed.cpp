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

#include "painter.h"
#include "light.h"
#include "lightmodel.h"
#include "material.h"

FixedFunctionPainter::FixedFunctionPainter()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glDisable(GL_CULL_FACE);
}

static void setMatrix(GLenum type, const QMatrix4x4 &matrix)
{
    glMatrixMode(type);
    if (sizeof(qreal) == sizeof(GLfloat)) {
        glLoadMatrixf(reinterpret_cast<const GLfloat *>(matrix.constData()));
    } else {
        GLfloat mat[16];
        const qreal *m = matrix.constData();
        for (int index = 0; index < 16; ++index)
            mat[index] = m[index];
        glLoadMatrixf(mat);
    }
}

void FixedFunctionPainter::setMatrices
    (const QMatrix4x4 &mv, const QMatrix4x4 &proj)
{
    setMatrix(GL_PROJECTION, proj);
    setMatrix(GL_MODELVIEW, mv);
}

static void setLight(int light, const Light *parameters,
                     const QMatrix4x4& transform = QMatrix4x4())
{
    GLfloat params[4];

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    QColor color = parameters->ambientColor();
    params[0] = color.redF();
    params[1] = color.greenF();
    params[2] = color.blueF();
    params[3] = color.alphaF();
    glLightfv(light, GL_AMBIENT, params);

    color = parameters->diffuseColor();
    params[0] = color.redF();
    params[1] = color.greenF();
    params[2] = color.blueF();
    params[3] = color.alphaF();
    glLightfv(light, GL_DIFFUSE, params);

    color = parameters->specularColor();
    params[0] = color.redF();
    params[1] = color.greenF();
    params[2] = color.blueF();
    params[3] = color.alphaF();
    glLightfv(light, GL_SPECULAR, params);

    QVector4D vector = parameters->eyePosition(transform);
    params[0] = vector.x();
    params[1] = vector.y();
    params[2] = vector.z();
    params[3] = vector.w();
    glLightfv(light, GL_POSITION, params);

    QVector3D spotDirection = parameters->eyeSpotDirection(transform);
    params[0] = spotDirection.x();
    params[1] = spotDirection.y();
    params[2] = spotDirection.z();
    glLightfv(light, GL_SPOT_DIRECTION, params);

    params[0] = parameters->spotExponent();
    glLightfv(light, GL_SPOT_EXPONENT, params);

    params[0] = parameters->spotAngle();
    glLightfv(light, GL_SPOT_CUTOFF, params);

    params[0] = parameters->constantAttenuation();
    glLightfv(light, GL_CONSTANT_ATTENUATION, params);

    params[0] = parameters->linearAttenuation();
    glLightfv(light, GL_LINEAR_ATTENUATION, params);

    params[0] = parameters->quadraticAttenuation();
    glLightfv(light, GL_QUADRATIC_ATTENUATION, params);

    glPopMatrix();
}

static void setLightModel(const LightModel *lightModel)
{
    GLfloat values[4];
#ifdef GL_LIGHT_MODEL_TWO_SIDE
    if (lightModel->model() == LightModel::TwoSided)
        values[0] = 1.0f;
    else
        values[0] = 0.0f;
    glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, values);
#endif
#ifdef GL_LIGHT_MODEL_COLOR_CONTROL
    if (lightModel->colorControl() == LightModel::SeparateSpecularColor)
        values[0] = GL_SEPARATE_SPECULAR_COLOR;
    else
        values[0] = GL_SINGLE_COLOR;
    glLightModelfv(GL_LIGHT_MODEL_COLOR_CONTROL, values);
#endif
#ifdef GL_LIGHT_MODEL_LOCAL_VIEWER
    if (lightModel->viewerPosition() == LightModel::LocalViewer)
        values[0] = 1.0f;
    else
        values[0] = 0.0f;
    glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, values);
#endif
#ifdef GL_LIGHT_MODEL_AMBIENT
    QColor color = lightModel->ambientSceneColor();
    values[0] = color.redF();
    values[1] = color.blueF();
    values[2] = color.greenF();
    values[3] = color.alphaF();
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, values);
#endif
}

static void setMaterial(int face, const Material *parameters)
{
    GLfloat params[17];

    QColor mcolor = parameters->ambientColor();
    params[0] = mcolor.redF();
    params[1] = mcolor.greenF();
    params[2] = mcolor.blueF();
    params[3] = mcolor.alphaF();

    mcolor = parameters->diffuseColor();
    params[4] = mcolor.redF();
    params[5] = mcolor.greenF();
    params[6] = mcolor.blueF();
    params[7] = mcolor.alphaF();

    mcolor = parameters->specularColor();
    params[8] = mcolor.redF();
    params[9] = mcolor.greenF();
    params[10] = mcolor.blueF();
    params[11] = mcolor.alphaF();

    mcolor = parameters->emittedLight();
    params[12] = mcolor.redF();
    params[13] = mcolor.greenF();
    params[14] = mcolor.blueF();
    params[15] = mcolor.alphaF();

    params[16] = parameters->shininess();

    glMaterialfv(face, GL_AMBIENT, params);
    glMaterialfv(face, GL_DIFFUSE, params + 4);
    glMaterialfv(face, GL_SPECULAR, params + 8);
    glMaterialfv(face, GL_EMISSION, params + 12);
    glMaterialfv(face, GL_SHININESS, params + 16);
}

void FixedFunctionPainter::selectMaterial(Material *material)
{
    ::setLight(GL_LIGHT0, light);
    ::setLightModel(lightModel);
    ::setMaterial(GL_FRONT_AND_BACK, material);
    glDisable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void FixedFunctionPainter::selectTexturedMaterial(Material *material)
{
    ::setLight(GL_LIGHT0, light);
    ::setLightModel(lightModel);
    ::setMaterial(GL_FRONT_AND_BACK, material);
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
}

void FixedFunctionPainter::setVertices(const float *array, int stride)
{
    glVertexPointer(3, GL_FLOAT, stride * sizeof(float), array);
}

void FixedFunctionPainter::setTexCoords(const float *array, int stride)
{
    glTexCoordPointer(2, GL_FLOAT, stride * sizeof(float), array);
}

void FixedFunctionPainter::setNormals(const float *array, int stride)
{
    glNormalPointer(GL_FLOAT, stride * sizeof(float), array);
}
