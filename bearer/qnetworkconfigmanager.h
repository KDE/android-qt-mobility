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
#ifndef QNETWORKCONFIGURATIONMANAGER_H
#define QNETWORKCONFIGURATIONMANAGER_H

#include "qbearerglobal.h"

#include <QtCore/qobject.h>
#include <qnetworkconfiguration.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Network)

class QNetworkConfigurationManagerPrivate;
class Q_BEARER_EXPORT QNetworkConfigurationManager : public QObject
{
    Q_OBJECT

public:
    
    enum CapabilityFlag {
         BearerManagement  = 0x00000001,
         DirectConnectionRouting = 0x00000002,
         SystemSessionSupport = 0x00000004,
         ApplicationLevelRoaming = 0x00000008,
         ForcedRoaming = 0x00000010,
         DataStatistics = 0x00000020
    };

    Q_DECLARE_FLAGS(CapabilityFlags, CapabilityFlag)

    QNetworkConfigurationManager( QObject* parent = 0 );
    virtual ~QNetworkConfigurationManager();

    
    QNetworkConfigurationManager::CapabilityFlags capabilities() const;

    QNetworkConfiguration defaultConfiguration() const; 
    QList<QNetworkConfiguration> allConfigurations(QNetworkConfiguration::StateFlags flags = 0) const;
    QNetworkConfiguration configurationFromIdentifier(const QString& identifier) const;
    void updateConfigurations();

Q_SIGNALS:
    void configurationAdded(const QNetworkConfiguration& config);
    void configurationRemoved(const QNetworkConfiguration& config);
    void configurationChanged(const QNetworkConfiguration& config);
    void onlineStateChanged(bool isOnline);
    void updateCompleted();

};

Q_DECLARE_OPERATORS_FOR_FLAGS(QNetworkConfigurationManager::CapabilityFlags)

QT_END_NAMESPACE

QT_END_HEADER

#endif //QNETWORKCONFIGURATIONMANAGER_H

