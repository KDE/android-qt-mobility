/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qorganizeritemlocalidfilter.h"
#include "qorganizeritemlocalidfilter_p.h"
#include "qorganizeritemfilter_p.h"
#include "qorganizeritemmanager.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerItemLocalIdFilter
  \brief The QOrganizerItemLocalIdFilter class provides a filter based around a list of organizer item ids
  \inmodule QtOrganizer
  \ingroup organizer-filters
  
  It may be used to select organizer items whose ids are contained in the given list of ids.

  Note: a QOrganizerItemLocalIdFilter will not be preserved if streamed to a QDataStream.
 */

Q_IMPLEMENT_ORGANIZERITEMFILTER_PRIVATE(QOrganizerItemLocalIdFilter);

/*!
 * \fn QOrganizerItemLocalIdFilter::QOrganizerItemLocalIdFilter(const QOrganizerItemFilter& other)
 * Constructs a copy of \a other if possible, otherwise constructs a new organizer item local id filter
 */

/*!
 * Constructs a new organizer item local id filter
 */
QOrganizerItemLocalIdFilter::QOrganizerItemLocalIdFilter()
    : QOrganizerItemFilter(new QOrganizerItemLocalIdFilterPrivate)
{
}

/*!
 * Sets the list which contains the ids of possible matching organizer items to \a ids
 * \sa ids()
 */
void QOrganizerItemLocalIdFilter::setIds(const QList<QOrganizerItemLocalId>& ids)
{
    Q_D(QOrganizerItemLocalIdFilter);
    d->m_ids = ids;
}

/*!
 * Inserts the id \a id into the list which contains the ids of possible matching items
 * \sa setIds()
 */
void QOrganizerItemLocalIdFilter::insert(const QOrganizerItemLocalId& id)
{
    Q_D(QOrganizerItemLocalIdFilter);
    if (!d->m_ids.contains(id))
        d->m_ids.append(id);
}

/*!
 * Removes the id \a id from the list which contains the ids of possible matching items,
 * if it is contained in the list, otherwise has no effect.
 * \sa clear()
 */
void QOrganizerItemLocalIdFilter::remove(const QOrganizerItemLocalId& id)
{
    Q_D(QOrganizerItemLocalIdFilter);
    d->m_ids.removeAll(id);
}

/*!
 * Clears the list which contains the ids of possible matching items.
 * A local id filter with a cleared list will match no items.
 * \sa setIds()
 */
void QOrganizerItemLocalIdFilter::clear()
{
    Q_D(QOrganizerItemLocalIdFilter);
    d->m_ids.clear();
}

/*!
 * Returns the list of ids of organizer items which match this filter
 * \sa setIds()
 */
QList<QOrganizerItemLocalId> QOrganizerItemLocalIdFilter::ids() const
{
    Q_D(const QOrganizerItemLocalIdFilter);
    return d->m_ids;
}

QTM_END_NAMESPACE
