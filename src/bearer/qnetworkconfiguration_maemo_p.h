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

#ifndef QNETWORKCONFIGURATIONPRIVATE_H
#define QNETWORKCONFIGURATIONPRIVATE_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qnetworkconfiguration.h>
#include <QtCore/qshareddata.h>
#include <QNetworkInterface>

QTM_BEGIN_NAMESPACE

class QNetworkConfigurationPrivate : public QSharedData
{
public:
    QNetworkConfigurationPrivate ()
	    : isValid(false), type(QNetworkConfiguration::Invalid), 
	    roamingSupported(false), purpose(QNetworkConfiguration::UnknownPurpose),
	    network_attrs(0), service_attrs(0), manager(0)
    {
    }

    ~QNetworkConfigurationPrivate()
    {
        //release pointers to member configurations
        serviceNetworkMembers.clear(); 
    }

    QString name;
    bool isValid;
    QString id;
    QNetworkConfiguration::StateFlags state;
    QNetworkConfiguration::Type type;
    bool roamingSupported;
    QNetworkConfiguration::Purpose purpose;

    QList<QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> > serviceNetworkMembers;
    QNetworkInterface serviceInterface;

    /* In Maemo the id field (defined above) is the IAP id (which typically is UUID) */
    QByteArray network_id; /* typically WLAN ssid or similar */
    QString iap_type; /* is this one WLAN or GPRS */
    uint32_t network_attrs; /* network attributes for this IAP, this is the value returned by icd and passed to it when connecting */

    QString service_type;
    QString service_id;
    uint32_t service_attrs;

    QNetworkConfigurationManagerPrivate *manager;

private:

    // disallow detaching
    QNetworkConfigurationPrivate &operator=(const QNetworkConfigurationPrivate &other);
    QNetworkConfigurationPrivate(const QNetworkConfigurationPrivate &other);
};

QTM_END_NAMESPACE

#endif //QNETWORKCONFIGURATIONPRIVATE_H
