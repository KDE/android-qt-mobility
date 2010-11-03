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

#ifndef QORGANIZERSIMULATOR_P_H
#define QORGANIZERSIMULATOR_P_H

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

#include <QSharedData>
#include <QMap>
#include <QMultiMap>
#include <QList>
#include <QQueue>
#include <QPair>
#include <QSet>
#include <QDateTime>
#include <QString>
#include <QObject>

#include "qorganizeritem.h"
#include "qorganizermanager.h"
#include "qorganizermanagerengine.h"
#include "qorganizermanagerenginefactory.h"
#include "qorganizeritemdetaildefinition.h"
#include "qorganizerabstractrequest.h"
#include "qorganizeritemchangeset.h"

#include "engines/qorganizeritemmemorybackend_p.h"

QTM_USE_NAMESPACE

class QOrganizerSimulatorFactory : public QObject, public QOrganizerManagerEngineFactory
{
  Q_OBJECT
  Q_INTERFACES(QtMobility::QOrganizerManagerEngineFactory)
  public:
    QOrganizerManagerEngine* engine(const QMap<QString, QString>& parameters, QOrganizerManager::Error*);
    QOrganizerItemEngineId* createItemEngineId(const QMap<QString, QString>& parameters, const QString& engineIdString) const;
    QOrganizerCollectionEngineId* createCollectionEngineId(const QMap<QString, QString>& parameters, const QString& engineIdString) const;
    QString managerName() const;  
};

class QOrganizerSimulatorEngine : public QOrganizerItemMemoryEngine
{
    Q_OBJECT

public:
    QString managerName() const;
    int managerVersion() const;

    bool saveCollection(QOrganizerCollection *collection, QOrganizerManager::Error *error);
    bool removeCollection(const QOrganizerCollectionId &collectionId, QOrganizerManager::Error *error);

protected:
    QOrganizerSimulatorEngine(QOrganizerItemMemoryEngineData* data);

protected:
    bool saveItem(QOrganizerItem* theOrganizerItem, QOrganizerItemChangeSet& changeSet, QOrganizerManager::Error* error);
    bool removeItem(const QOrganizerItemId& organizeritemId, QOrganizerItemChangeSet& changeSet, QOrganizerManager::Error* error);
    bool saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& organizeritemType, QOrganizerItemChangeSet& changeSet, QOrganizerManager::Error* error);
    bool removeDetailDefinition(const QString& definitionId, const QString& organizeritemType, QOrganizerItemChangeSet& changeSet, QOrganizerManager::Error* error);

private:
    static QOrganizerItemMemoryEngineData* engineData;

    friend class QOrganizerSimulatorFactory;
};

#endif // QORGANIZERSIMULATOR_P_H

