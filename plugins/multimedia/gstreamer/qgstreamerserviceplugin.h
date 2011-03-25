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


#ifndef QGSTREAMERSERVICEPLUGIN_H
#define QGSTREAMERSERVICEPLUGIN_H

#include <qmediaserviceproviderplugin.h>
#include <QtCore/qset.h>

QT_USE_NAMESPACE


class QGstreamerServicePlugin
    : public QMediaServiceProviderPlugin
    , public QMediaServiceSupportedDevicesInterface
    , public QMediaServiceFeaturesInterface
    , public QMediaServiceSupportedFormatsInterface
{
    Q_OBJECT
    Q_INTERFACES(QMediaServiceSupportedDevicesInterface)
    Q_INTERFACES(QMediaServiceFeaturesInterface)
    Q_INTERFACES(QMediaServiceSupportedFormatsInterface)
public:
    QStringList keys() const;
    QMediaService* create(QString const& key);
    void release(QMediaService *service);

    QMediaServiceProviderHint::Features supportedFeatures(const QByteArray &service) const;

    QList<QByteArray> devices(const QByteArray &service) const;
    QString deviceDescription(const QByteArray &service, const QByteArray &device);
    QVariant deviceProperty(const QByteArray &service, const QByteArray &device, const QByteArray &property);

    QtMultimediaKit::SupportEstimate hasSupport(const QString &mimeType, const QStringList& codecs) const;
    QStringList supportedMimeTypes() const;

private:
    void updateDevices() const;

    mutable QList<QByteArray> m_cameraDevices;
    mutable QStringList m_cameraDescriptions;
    mutable QSet<QString> m_supportedMimeTypeSet; //for fast access

    void updateSupportedMimeTypes() const;
};

#endif // QGSTREAMERSERVICEPLUGIN_H
