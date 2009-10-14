/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QIMAGEENCODERCONTROL_H
#define QIMAGEENCODERCONTROL_H

#include <multimedia/qmediacontrol.h>
#include <multimedia/qmediarecorder.h>

#include <QtCore/qsize.h>

class QByteArray;
class QStringList;

class Q_MEDIA_EXPORT QImageEncoderControl : public QMediaControl
{
    Q_OBJECT
public:
    virtual ~QImageEncoderControl();

    virtual QSize resolution() const = 0;
    virtual QSize minimumResolution() const = 0;
    virtual QSize maximumResolution() const = 0;
    virtual QList<QSize> supportedResolutions() const = 0;
    virtual void setResolution(const QSize &) = 0;

    virtual QStringList supportedImageCodecs() const = 0;
    virtual QString imageCodec() const = 0;
    virtual bool setImageCodec(const QString &codecName) = 0;

    virtual QString imageCodecDescription(const QString &codecName) const = 0;

    virtual QtMedia::EncodingQuality quality() const = 0;
    virtual void setQuality(QtMedia::EncodingQuality) = 0;

protected:
    QImageEncoderControl(QObject *parent);
};

#define QImageEncoderControl_iid "com.nokia.Qt.QImageEncoderControl/1.0"
Q_MEDIA_DECLARE_CONTROL(QImageEncoderControl, QImageEncoderControl_iid)

#endif
