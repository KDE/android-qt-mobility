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

#ifndef QMEDIARESOURCE_H
#define QMEDIARESOURCE_H

#include <QtCore/qmap.h>
#include <QtCore/qmetatype.h>

#include <multimedia/qmultimediaglobal.h>

class Q_MEDIA_EXPORT QMediaResource
{
public:
    enum ResourceRole
    {
        ContentRole,
        PreviewRole,
        PosterRole,
        CoverArtRole,
        ThumbnailRole
    };

    QMediaResource();
    QMediaResource(const QUrl &uri, ResourceRole role = ContentRole);
    QMediaResource(const QUrl &uri, const QString &type, ResourceRole role = ContentRole);
    QMediaResource(const QMediaResource &other);
    QMediaResource &operator =(const QMediaResource &other);
    ~QMediaResource();

    bool isNull() const;

    bool operator ==(const QMediaResource &other) const;
    bool operator !=(const QMediaResource &other) const;

    QUrl uri() const;
    ResourceRole role() const;
    QString mimeType() const;

    QString audioCodec() const;
    void setAudioCodec(const QString &codec);

    QString videoCodec() const;
    void setVideoCodec(const QString &codec);

    qint64 size() const;
    void setSize(const qint64 size);

    qint64 duration() const;
    void setDuration(qint64 duration);

    int audioBitRate() const;
    void setAudioBitRate(int rate);

    int sampleSize() const;
    void setSampleSize(int size);

    int frequency() const;
    void setFrequency(int frequency);

    int channels() const;
    void setChannels(int channels);

    int videoBitRate() const;
    void setVideoBitRate(int rate);

    QSize resolution() const;
    void setResolution(const QSize &resolution);
    void setResolution(int width, int height);

    int bitsPerPixel() const;
    void setBitsPerPixel(int bits);

private:
    enum Property
    {
        Uri,
        MimeType,
        Role,
        AudioCodec,
        VideoCodec,
        Size,
        Duration,
        AudioBitRate,
        VideoBitRate,
        SampleSize,
        Frequency,
        Channels,
        Resolution,
        BitsPerPixel,
    };
    QMap<int, QVariant> values;
};

typedef QList<QMediaResource> QMediaResourceList;

Q_DECLARE_METATYPE(QMediaResource::ResourceRole)
Q_DECLARE_METATYPE(QMediaResource)
Q_DECLARE_METATYPE(QMediaResourceList)

#endif
