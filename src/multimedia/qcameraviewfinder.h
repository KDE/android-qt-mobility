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

#ifndef QCAMERAVIEWFINDER_H
#define QCAMERAVIEWFINDER_H

#include <QtCore/qstringlist.h>
#include <QtCore/qpair.h>
#include <QtCore/qsize.h>
#include <QtCore/qpoint.h>
#include <QtCore/qrect.h>

#include <qmediacontrol.h>
#include <qmediaobject.h>
#include <qmediaservice.h>
#include <qvideowidget.h>

#include <qmediaserviceprovider.h>

QT_BEGIN_NAMESPACE

class QCamera;

class QCameraViewfinderPrivate;
class Q_MULTIMEDIA_EXPORT QCameraViewfinder : public QVideoWidget
{
    Q_OBJECT
public:
    QCameraViewfinder(QWidget *parent = 0);
    ~QCameraViewfinder();

    QMediaObject *mediaObject() const;

protected:
    bool setMediaObject(QMediaObject *object);

private:
    Q_DISABLE_COPY(QCameraViewfinder)
    Q_DECLARE_PRIVATE(QCameraViewfinder)
};

QT_END_NAMESPACE

#endif  // QCAMERA_H
