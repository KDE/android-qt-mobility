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

#include "DebugMacros.h"

#include "s60videorenderer.h"

#include <QtCore/qcoreevent.h>
#include <QtGui/qapplication.h>

/*!
    Constructs a new video renderer media end point with the given \a parent.
*/

S60VideoRenderer::S60VideoRenderer(QObject *parent)
    : QVideoRendererControl(parent)
{
    DP0("S60VideoRenderer::S60VideoRenderer +++");

    DP0("S60VideoRenderer::S60VideoRenderer ---");

}

/*!
    Destroys a video renderer media end point.
*/

S60VideoRenderer::~S60VideoRenderer()
{
    DP0("S60VideoRenderer::~S60VideoRenderer +++");
    DP0("S60VideoRenderer::~S60VideoRenderer ---");
}

/*!
   \return the surface a video producer renders to.
*/

QAbstractVideoSurface *S60VideoRenderer::surface() const
{
    DP0("S60VideoRenderer::surface");

    return m_surface;
}

/*!
   Sets the \a surface a video producer renders to.
*/

void S60VideoRenderer::setSurface(QAbstractVideoSurface *surface)
{
    DP0("S60VideoRenderer::setSurface +++");

    m_surface = surface;

    DP0("S60VideoRenderer::setSurface ---");
}

