/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/


#ifndef QORGANIZERITEMMANAGERDATAHOLDER_H
#define QORGANIZERITEMMANAGERDATAHOLDER_H

#include <QMap>
#include <QMultiMap>
#include <QList>
#include <QString>
#include <QDebug>

#include "qorganizeritemmanager.h"

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

QTM_BEGIN_NAMESPACE
class QOrganizerItem;
class QOrganizerItemManagerDataHolder
{
public:
    QOrganizerItemManagerDataHolder()
    {
        QStringList managerNames = QOrganizerItemManager::availableManagers();

        foreach(const QString& mgr, managerNames) {
            // Don't bother with these
            if (mgr == "memory" || mgr == "invalid" || mgr == "testdummy" || mgr == "maliciousplugin")
                continue;
            QMap<QString, QString> params;
            QString mgrUri = QOrganizerItemManager::buildUri(mgr, params);
            QOrganizerItemManager* cm = QOrganizerItemManager::fromUri(mgrUri);
            if (cm) {
                qDebug() << "Saving items for" << mgrUri;
                QList<QOrganizerItem> items = cm->items();
                savedItems.insert(cm->managerName(),items);
                QList<QOrganizerItemLocalId> ids;
                foreach(const QOrganizerItem& item, items)
                    ids.append(item.localId());
                cm->removeItems(ids, 0);
                delete cm;
            }
        }
    }

    ~QOrganizerItemManagerDataHolder()
    {
        foreach(const QString& mgrUri, savedItems.keys()) {
            QOrganizerItemManager* cm = QOrganizerItemManager::fromUri(mgrUri);
            if (cm) {
                qDebug() << "Restoring items for" << mgrUri;
                QList<QOrganizerItem> items = savedItems.value(mgrUri);
                cm->saveItems(&items, 0);
                // XXX this doesn't restore relationships..
                delete cm;
            }
        }
    }
private:
    QMap<QString, QList<QOrganizerItem> > savedItems;
};

QTM_END_NAMESPACE

#endif

