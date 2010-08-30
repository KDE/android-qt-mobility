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

#include "qgallerytrackereditableresultset_p.h"

#include "qgallerytrackerresultset_p_p.h"
#include "qgallerytrackerschema_p.h"

#include <QtCore/qcoreapplication.h>
#include <QtDBus/qdbuspendingreply.h>

Q_DECLARE_METATYPE(QVector<QStringList>)

QTM_BEGIN_NAMESPACE

class QGalleryTrackerEditableResultSetPrivate : public QGalleryTrackerResultSetPrivate
{
    Q_DECLARE_PUBLIC(QGalleryTrackerEditableResultSet)
public:
    QGalleryTrackerEditableResultSetPrivate(
            const QGalleryTrackerResultSetArguments &arguments,
            const QGalleryDBusInterfacePointer &metaDataInterface,
            bool live,
            int cursorPosition,
            int minimumPagedItems)
        : QGalleryTrackerResultSetPrivate(arguments, live, cursorPosition, minimumPagedItems)
        , metaDataInterface(metaDataInterface)
        , fieldNames(arguments.fieldNames)
    {
    }

    const QGalleryDBusInterfacePointer metaDataInterface;
    const QStringList fieldNames;
};

QGalleryTrackerEditableResultSet::QGalleryTrackerEditableResultSet(
        const QGalleryTrackerResultSetArguments &arguments,
        const QGalleryDBusInterfacePointer &metaDataInterface,
        bool live,
        int cursorPosition,
        int minimumPagedItems,
        QObject *parent)
    : QGalleryTrackerResultSet(
            *new QGalleryTrackerEditableResultSetPrivate(
                    arguments, metaDataInterface, live, cursorPosition, minimumPagedItems),
            parent)
{
}

QGalleryTrackerEditableResultSet::~QGalleryTrackerEditableResultSet()
{
}

bool QGalleryTrackerEditableResultSet::setMetaData(int key, const QVariant &value)
{
    Q_D(QGalleryTrackerEditableResultSet);

    if (!d->currentRow || key < d->valueOffset || key >= d->columnCount)
        return false;
    else if (key >= d->aliasOffset)
        key = d->aliasColumns.at(key - d->aliasOffset);

    if (key >= d->compositeOffset)
        return false;

    if (*(d->currentRow + key) == value)
        return true;

    QGalleryTrackerMetaDataEdit *edit = 0;

    typedef QList<QGalleryTrackerMetaDataEdit *>::iterator iterator;
    for (iterator it = d->edits.begin(), end = d->edits.end(); it != end; ++it) {
        if ((*it)->index() == d->currentIndex) {
            edit = *it;
            break;
        }
    }

    if (!edit) {
        edit = new QGalleryTrackerMetaDataEdit(
                d->metaDataInterface,
                d->currentRow->toString(),
                (d->currentRow + 1)->toString(),
                this);
        edit->setIndex(d->currentIndex);

        connect(edit, SIGNAL(finished(QGalleryTrackerMetaDataEdit*)),
                this, SLOT(_q_editFinished(QGalleryTrackerMetaDataEdit*)));

        connect(this, SIGNAL(itemsInserted(int,int)), edit, SLOT(itemsInserted(int,int)));
        connect(this, SIGNAL(itemsRemoved(int,int)), edit, SLOT(itemsRemoved(int,int)));

        d->edits.append(edit);

        d->requestUpdate();
    }

    edit->setValue(
            d->fieldNames.at(key - d->valueOffset),
            d->valueColumns.at(key - d->valueOffset)->toString(value));

    return true;
}

#include "moc_qgallerytrackereditableresultset_p.cpp"

QTM_END_NAMESPACE
